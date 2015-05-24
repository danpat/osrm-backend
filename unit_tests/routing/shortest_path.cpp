/*

Copyright (c) 2014, Project OSRM contributors
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list
of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "../../routing_algorithms/shortest_path.hpp"
#include "../../server/data_structures/internal_datafacade.hpp"
#include "../../data_structures/query_edge.hpp"
#include "../../include/osrm/server_paths.hpp"
#include "../../include/osrm/json_container.hpp"
#include "../../plugins/viaroute.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>

#include <osrm/coordinate.hpp>

#include <iostream>

struct ArgsFixture {
   ArgsFixture(): argc(boost::unit_test::framework::master_test_suite().argc),
           argv(boost::unit_test::framework::master_test_suite().argv){}
   int argc;
   char **argv;
};


BOOST_FIXTURE_TEST_SUITE(shortest_path, ArgsFixture)




BOOST_AUTO_TEST_CASE(all_necessary_test)
{

    std::string base_string("../unit_tests/data/montevideo.osrm");

    // If supplied, use the first command-line parameter as the OSRM
    // filename to load
    if (argc > 1)
    {
      base_string = argv[1];
    }

    ServerPaths server_paths;
    server_paths["hsgrdata"] = base_string + ".hsgr";
    server_paths["nodesdata"] = base_string + ".nodes";
    server_paths["edgesdata"] = base_string + ".edges";
    server_paths["geometries"] = base_string + ".geometry";
    server_paths["ramindex"] = base_string + ".ramIndex";
    server_paths["fileindex"] = base_string + ".fileIndex";
    server_paths["namesdata"] = base_string + ".names";
    server_paths["timestamp"] = base_string + ".timestamp";

    InternalDataFacade<QueryEdge::EdgeData> query_data_facade(server_paths);
    ViaRoutePlugin<InternalDataFacade<QueryEdge::EdgeData>> via_route(&query_data_facade);

    // http://localhost:5000/viaroute?z=17&output=json&jsonp=OSRM.JSONP.callbacks.route&checksum=1800904781&loc=-34.871296,-56.149937&hint=xwMAABkNAABsAAAAEwAAABgAAAAAAAAAIAAAADIEAAAAAAAAAOjr_U44p_wAABEA&loc=-34.871685,-56.147415&hint=DwYAAP8GAAAuAQAAMAAAABEAAAAAAAAAAAAAAP____8AAAAAe-br_SlCp_wAABEA&instructions=true
    // http://localhost:5000/viaroute?z=17&output=json&jsonp=OSRM.JSONP.callbacks.route&checksum=1800904781&loc=-34.871685,-56.147415&hint=DwYAAP8GAAAuAQAAMAAAABEAAAAAAAAAAAAAAP____8AAAAAe-br_SlCp_wAABEA&loc=-34.871296,-56.149937&hint=xwMAABkNAABsAAAAEwAAABgAAAAAAAAAIAAAADIEAAAAAAAAAOjr_U44p_wAABEA&instructions=true

    RouteParameters params1;
    params1.setZoomLevel(17);
    params1.addCoordinate({-34.871296,-56.149937});
    params1.addCoordinate({-34.871685,-56.147415});
    params1.setInstructionFlag(true);

    osrm::json::Object result1;
    via_route.HandleRequest(params1, result1);

    RouteParameters params2;
    params2.setZoomLevel(17);
    // Note: coordinates are reversed here
    params2.addCoordinate({-34.871685,-56.147415});
    params2.addCoordinate({-34.871296,-56.149937});
    params2.setInstructionFlag(true);

    osrm::json::Object result2;
    via_route.HandleRequest(params2, result2);

    BOOST_CHECK_EQUAL(334, result1["route_summary"].get<osrm::json::Object>()["total_distance"].get<osrm::json::Number>().value);
    BOOST_CHECK_EQUAL(334, result2["route_summary"].get<osrm::json::Object>()["total_distance"].get<osrm::json::Number>().value);

}

BOOST_AUTO_TEST_SUITE_END()
