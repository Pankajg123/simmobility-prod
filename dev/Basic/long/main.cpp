//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

/*
 * file main.cpp
 * Empty file for the (future) long-term simulation
 * \author Pedro Gandola
 */

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <unistd.h>

#include "GenConfig.h"
#include "tinyxml.h"

#include "entities/roles/RoleFactory.hpp"
#include "conf/simpleconf.hpp"
#include "workers/Worker.hpp"
#include "workers/WorkGroup.hpp"
#include "entities/AuraManager.hpp"
#include "agent/LT_Agent.hpp"
#include "database/dao/HouseholdDao.hpp"
#include "database/dao/IndividualDao.hpp"
#include "DatabaseHelper.h"
#include "unit-tests/DaoTests.hpp"

using std::cout;
using std::endl;
using std::vector;
using std::string;

using namespace sim_mob;
using namespace sim_mob::long_term;

//Current software version.
const string SIMMOB_VERSION = string(
        SIMMOB_VERSION_MAJOR) + ":" + SIMMOB_VERSION_MINOR;

//Start time of program
timeval start_time;


// id, distance to CDB, size (m2), owner Id.
int TEST_UNITS [][4] = {
    {1, 345, 2152, 2},
    {2, 96, 1707, 2},
    {3, 275, 1590, 4},
    {4, 235, 2463, 4},
    {5, 44, 2121, 6},
    {6, 36, 1817, 6},
    {7, 9, 2347, 8},
    {8, 9, 1850, 8},
    {9, 150, 1943, 10},
    {10, 178, 1551, 10},
    {11, 216, 2003, 12},
    {12, 295, 1582, 12},
    {13, 324, 1944, 14},
    {14, 9, 1988, 14},
    {15, 51, 2470, 16},
    {16, 198, 1941, 16},
    {17, 146, 2242, 18},
    {18, 118, 1890, 18},
    {19, 252, 2096, 20},
    {20, 315, 1721, 20},
    {21, 342, 2304, 22},
    {22, 122, 1596, 22},
    {23, 323, 2476, 24},
    {24, 312, 2317, 24},
    {25, 334, 2095, 26},
    {26, 101, 2417, 26},
    {27, 355, 1947, 28},
    {28, 147, 2219, 28},
    {29, 271, 2183, 30},
    {30, 268, 2288, 30}
};

int TEST_HH [][4] = {
    {21, 251, 1505, 1},
    {27, 336, 1389, 2},
    {26, 326, 1368, 3},
    {10, 138, 1274, 4},
    {4, 41, 1583, 5},
    {19, 245, 1991, 6},
    {22, 264, 1680, 7},
    {6, 81, 1334, 8},
    {3, 35, 1204, 9},
    {9, 121, 1287, 10},
    {29, 343, 1501, 11},
    {7, 84, 1808, 12},
    {12, 156, 1663, 13},
    {14, 196, 1934, 14},
    {28, 340, 1818, 15},
    {16, 228, 1467, 16},
    {17, 234, 1981, 17},
    {1, 1, 1360, 18},
    {15, 225, 1505, 19},
    {18, 234, 1950, 20},
    {24, 305, 1659, 21},
    {2, 12, 1715, 22},
    {25, 314, 1773, 23},
    {20, 249, 1945, 24},
    {23, 288, 1226, 25},
    {5, 49, 1500, 26},
    {8, 119, 1752, 27},
    {30, 359, 1413, 28},
    {13, 184, 1837, 29},
    {11, 145, 1948, 30}
};

float UNIT_FIXED_COST = 1.0f;

//SIMOBILITY TEST PARAMS
#define MAX_ITERATIONS 1
#define TICK_STEP 1
#define DAYS 365
#define WORKERS 4
#define DATA_SIZE 30


void perform_main() {
    
    LogOut("Starting SimMobility, version " << SIMMOB_VERSION << endl);

    // Milliseconds step (Application crashes if this is 0).
    ConfigParams::GetInstance().baseGranMS = TICK_STEP;
    ConfigParams::GetInstance().totalRuntimeTicks = DAYS;
    ConfigParams::GetInstance().defaultWrkGrpAssignment =
            WorkGroup::ASSIGN_ROUNDROBIN;

    unit_tests::DaoTests tests;
    tests.TestHouseholdDao();
    tests.TestIndividualDao();
    tests.TestBuildingTypeDao();
    tests.TestBuildingDao();
    
    //Work Group specifications
    /*WorkGroup* agentWorkers = WorkGroup::NewWorkGroup(WORKERS, DAYS, TICK_STEP);
    WorkGroup::InitAllGroups();
    agentWorkers->initWorkers(nullptr);
    
    HousingMarket market;
    agentWorkers->assignAWorker(&market);
    //create all units.
    list<LT_Agent*> agents;
    //create all households.
    for (int i = 0; i < DATA_SIZE; i++) {
        LT_Agent* hh = new LT_Agent((TEST_HH[i][0]), &market,
                TEST_HH[i][1], TEST_HH[i][2]);
        //add agents units.
        for (int j = 0; j < DATA_SIZE; j++) {
            if (TEST_UNITS[j][3] == TEST_HH[i][0]) {
                Unit* unit = new Unit((TEST_UNITS[j][0]), true,
                        UNIT_FIXED_COST, TEST_UNITS[j][1], TEST_UNITS[j][2]);
                hh->AddUnit(unit);
            }
        }
        agents.push_back(hh);
        agentWorkers->assignAWorker(hh);
    }
    //Start work groups and all threads.
    WorkGroup::StartAllWorkGroups();

    LogOut("Started all workgroups." << endl);
    for (unsigned int currTick = 0; currTick < DAYS; currTick++) {
        LogOut("Time: " << currTick << endl);
        WorkGroup::WaitAllGroups();
    }

    LogOut("Finalizing workgroups: " << endl);
    WorkGroup::FinalizeAllWorkGroups();
    
    LogOut("Destroying agents: " << endl);
    //destroy all agents.
    for (list<LT_Agent*>::iterator itr = agents.begin(); 
            itr != agents.end(); itr++) {
        LT_Agent* ag = *(itr);
        safe_delete_item(ag);
    }
    agents.clear();*/
}

int main(int argc, char* argv[]) {
    Logger::log_init("");
    time_t now;
    time_t end;

    //get start time of the simulation.
    time(&now);
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        LogOut("Simulation #:  " << (i + 1) << endl);
        perform_main();
    }
    //get start time of the simulation.
    time(&end);
    double diffTime = difftime(end, now);
    LogOut("Long-term simulation complete. In " << diffTime << " seconds."
            << endl);
    LogOut("#################### FINISED WITH SUCCESS ####################" << endl);
    Logger::log_done();
    return 0;
}