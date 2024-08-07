#include "../catch.hpp"

#include "uICAL.h"

#include <iostream>
#include <fstream>
#include <list>

TEST_CASE("TZ::test1", "[uICAL][TZ]") {
    std::ifstream input(std::string("test/data/ical2.txt"));
    uICAL::istream_stl ical(input);

    uICAL::TZMap_ptr tzmap = uICAL::new_ptr<uICAL::TZMap>();
    auto vcalendar = uICAL::Calendar::load(ical, tzmap);
    {
        auto ds = uICAL::DateStamp("20191105T175555");

        std::list<uICAL::TZ_ptr> tzList;

        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("NZDT", tzmap));
        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("Pacific/Auckland", tzmap));

        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("CET", tzmap));
        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("Europe/Paris", tzmap));

        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("EST", tzmap));
        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("America/New_York", tzmap));

        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("CST", tzmap));
        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("America/Chicago", tzmap));

        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("MST", tzmap));
        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("America/Phoenix", tzmap));

        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("PST", tzmap));
        tzList.push_back(uICAL::new_ptr<uICAL::TZ>("America/Los_Angeles", tzmap));

        auto tz = uICAL::new_ptr<uICAL::TZ>("Z");
        auto dt = uICAL::DateTime(ds, tz);

        auto it = tzList.begin();
        auto next = [&]() {
            if (it == tzList.end()) {
                return uICAL::string("END");
            }
            auto newt = uICAL::DateTime(dt.datestamp(*it), *it);
            uICAL::string ret = newt.as_str();
            it++;
            return ret;
        };

        REQUIRE(next() == "20191106T055555NZDT");
        REQUIRE(next() == "20191106T055555NZDT");
        REQUIRE(next() == "20191105T185555CET");
        REQUIRE(next() == "20191105T185555CET");
        REQUIRE(next() == "20191105T125555EST");
        REQUIRE(next() == "20191105T125555EST");
        REQUIRE(next() == "20191105T115555CST");
        REQUIRE(next() == "20191105T115555CST");
        REQUIRE(next() == "20191105T105555MST");
        REQUIRE(next() == "20191105T105555MST");
        REQUIRE(next() == "20191105T095555PST");
        REQUIRE(next() == "20191105T095555PST");
        REQUIRE(next() == "END");
    }
}
