/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/util.h"
#include "uICAL/logging.h"
#include "uICAL/calendar.h"
#include "uICAL/calendarentry.h"
#include "uICAL/tz.h"
#include "uICAL/tzmap.h"
#include "uICAL/vevent.h"
#include "uICAL/vobject.h"
#include "uICAL/vobjectstream.h"
#include "uICAL/vline.h"
#include "uICAL/vlinestream.h"

namespace uICAL {
    Calendar_ptr Calendar::load(istream& ical) {
        TZMap_ptr tzmap = new_ptr<TZMap>();
        return Calendar::load(ical, tzmap, [](const VEvent&) { return true; });
    }

    Calendar_ptr Calendar::load(istream& ical, eventP_t addEvent) {
        TZMap_ptr tzmap = new_ptr<TZMap>();
        return Calendar::load(ical, tzmap, addEvent);
    }

    Calendar_ptr Calendar::load(istream& ical, TZMap_ptr& tzmap) {
        return Calendar::load(ical, tzmap, [](const VEvent&) { return true; });
    }

    Calendar_ptr Calendar::load(istream& ical, TZMap_ptr& tzmap, eventP_t addEvent) {
        VLineStream lines(ical);

        VObjectStream::lineP_t useLine = [](const string parent, const string line) {
            if (parent == "VCALENDAR") {
                if (line.empty()) return true;
                if (line == "X-WR-TIMEZONE") return true;
            }
            else
            if (parent == "VTIMEZONE") {
                if (line.empty()) return true;
                if (line == "TZID") return true;
            }
            else if (parent == "STANDARD" || parent == "DAYLIGHT") {
                if (line.empty()) return true;
                if (line == "TZOFFSETFROM") return true;
                if (line == "TZOFFSETTO") return true;
                if (line == "TZNAME") return true;
                if (line == "DTSTART") return true;
                if (line == "DTSTART") return true;
                if (line == "RRULE") return true;
                if (line == "EXDATE") return true;
                if (line == "RDATE") return true;
            }
            else if (parent == "VEVENT") {
                if (line.empty()) return true;
                if (line == "SUMMARY") return true;
                if (line == "DTSTAMP") return true;
                if (line == "DTSTART") return true;
                if (line == "DTEND") return true;
                if (line == "RRULE") return true;
                if (line == "EXDATE") return true;
                if (line == "RDATE") return true;

            }
            return false;
        };

        VObjectStream stm(lines, useLine);

        string default_tz;
        {
            VObject_ptr obj = stm.nextObject(false);

            if (obj->getName() != "VCALENDAR") {
                log_error("Parse error, did not expect: %s", obj->getName().c_str());
                throw ParseError(string("Parse error, did not expect: ") + obj->getName().c_str());
            }
            if (obj->getPropertyByName("X-WR-TIMEZONE")) {
                default_tz = obj->getPropertyByName("X-WR-TIMEZONE")->value;
            }
        }

        Calendar_ptr cal = new_ptr<Calendar>();

        for (;;) {
            auto child = stm.nextObject(true);
            if (child == nullptr) {
                break;
            }
            if (child->getName() == "VTIMEZONE") {
                tzmap->add(child);
            }

            else if (child->getName() == "VEVENT") {
                VEvent_ptr event = new_ptr<VEvent>(child, tzmap, default_tz);
                if (addEvent(*event)) {
                    cal->addEvent(event);
                } else {
                    log_debug("Event ignored: %s @ %s", event->summary.c_str(), event->start.as_str().c_str());
                }
            }
        }
        if (!default_tz.empty()) {
            try {
                cal->default_tz = new_ptr<TZ>(default_tz, tzmap);
            } catch (uICAL::ValueError &e) {
                cal->default_tz = new_ptr<TZ>("Z");
            }
        } else {
            cal->default_tz = new_ptr<TZ>("Z");
        }
        return cal;
    }

    TZ_ptr Calendar::tz() {
        return default_tz;
    }

    Calendar::Calendar(): default_tz(TZ::undef()) {}

    void Calendar::addEvent(const VEvent_ptr& event) {
        this->events.push_back(event);
    }

    void Calendar::str(ostream& out) const {
        out << "CALENDAR" << uICAL::endl;
    }
}
