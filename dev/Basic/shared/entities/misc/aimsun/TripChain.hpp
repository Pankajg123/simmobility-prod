/* Copyright Singapore-MIT Alliance for Research and Technology */

#pragma once

#include <vector>
#include <map>
#include <set>

#include "util/LangHelpers.hpp"
#include "util/DailyTime.hpp"

#ifndef SIMMOB_DISABLE_MPI
#include "partitions/PackageUtils.hpp"
#include "partitions/UnPackageUtils.hpp"
#endif

namespace aimsun
{

/**
 * Aimsun class to read from database.
 * Hoping to be able to remove this and load data directly into corresponding sim_mob classes.
 * \author Harish L
 */
class TripChainItem {
public:
	sim_mob::TripChainItem::ItemType itemType;
	int sequenceNumber;
	sim_mob::DailyTime startTime;
	sim_mob::DailyTime endTime;
	int entityID;
	std::string description;
	sim_mob::aimsun::Node* location;
	sim_mob::TripChainItem::LocationType locationType;
	bool isPrimary;
	bool isFlexible;
	bool isMandatory;
	sim_mob::DailyTime EndTime;
	sim_mob::TripChainItem::LocationType tripfromLocationType;
	sim_mob::TripChainItem::LocationType triptoLocationType;
	sim_mob::aimsun::Node* fromLocation;
	sim_mob::TripChainItem::LocationType fromLocationType;
    sim_mob::aimsun::Node* toLocation;
    sim_mob::TripChainItem::LocationType toLocationType;
    int tripID;
	std::string mode;
	bool isPrimaryMode;
	std::string ptLineId; //Public transit (bus or train) line identifier.

    //Temporaries for SOCI conversion
	int tmp_subTripID;
	int tmp_tripfromLocationNodeID;
	int tmp_triptoLocationNodeID;
	int tmp_fromLocationNodeID;
	std::string tmp_fromlocationType;
	int tmp_toLocationNodeID;
	std::string tmp_tolocationType;
	std::string tmp_startTime;
	std::string tmp_endTime;
	int tmp_activityID;
	int tmp_locationID;
	std::string  tmp_locationType;

	unsigned int getSequenceNumber() const {
		return sequenceNumber;
	}

	void setSequenceNumber(unsigned int sequenceNumber) {
		this->sequenceNumber = sequenceNumber;
	}
};

}