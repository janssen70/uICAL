/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/util.h"
#include "uICAL/datetime.h"
#include "uICAL/rrule.h"
#include "uICAL/vevent.h"
#include "uICAL/vline.h"
#include "uICAL/vobject.h"
#include "uICAL/error.h"

namespace uICAL {
    VEvent::VEvent(const VObject_ptr& obj, const TZMap_ptr& tzmap) {

        VLine_ptr dtStart = obj->getPropertyByName("DTSTART");
        VLine_ptr dtEnd = obj->getPropertyByName("DTEND");
        VLine_ptr rRule = obj->getPropertyByName("RRULE");
        VLine_ptr summary = obj->getPropertyByName("SUMMARY");

        if (dtStart == nullptr) { throw ImplementationError("all events must have a DTSTART"); }
        if (dtEnd == nullptr) { throw ImplementationError("all events must have a DTEND"); }
        if (summary == nullptr) { throw ImplementationError("all events must have a SUMMARY"); }

        this->start = DateTime(dtStart->value + dtStart->getParam("TZID"), tzmap);
        this->end = DateTime(dtEnd->value + dtStart->getParam("TZID"), tzmap);

        this->summary = summary->value;

        if (rRule != nullptr) {
            this->rrule = new_ptr<RRule>(rRule->value, this->start);
        } else {
            this->rrule = new_ptr<RRule>("", this->start);
        }

        auto exdates = obj->getPropertiesByName("EXDATE");
        for (auto exdate : exdates) {
            this->rrule->exclude(DateTime(exdate->value + exdate->getParam("TZID"), tzmap));
        }
    }

    void VEvent::str(ostream& out) const {
        out << "VEVENT: " << this->summary << uICAL::endl;
        out << " - start: " << this->start << uICAL::endl;
        out << " - end: " << this->end << uICAL::endl;
        out << " - rrule: " << this->rrule << uICAL::endl;
    }
}
