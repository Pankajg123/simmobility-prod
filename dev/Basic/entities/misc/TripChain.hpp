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

namespace sim_mob {

//Forward declarations
class Node;
class SubTrip;

/**
 * Base class for elements in a trip chain.
 * \author Harish L
 */
class TripChainItem {
public:
	//Type of location of this trip chain item.
	enum LocationType {
		LT_BUILDING, LT_NODE, LT_LINK, LT_PUBLIC_TRANSIT_STOP
	};

	//Type of this trip chain item.
	enum ItemType {
		IT_TRIP, IT_ACTIVITY
	};

	int entityID;
	ItemType itemType;
	unsigned int sequenceNumber;
	sim_mob::DailyTime startTime;
	sim_mob::DailyTime endTime;

	TripChainItem();
	TripChainItem(int entId, std::string type,
			DailyTime start, DailyTime end, unsigned int seqNumber);
	virtual ~TripChainItem();
	static LocationType getLocationType(std::string locType);
	static ItemType getItemType(std::string itemType);
};

/**
 * An activity within a trip chain. Has a location and a description.
 * \author Seth N. Hetu
 * \author Harish L
 */
class Activity: public sim_mob::TripChainItem {
public:
	std::string description;
	sim_mob::Node* location;
	TripChainItem::LocationType locationType;
	bool isPrimary;
	bool isFlexible;
	bool isMandatory;

	Activity();
	virtual ~Activity();
};

/**
 * \author Seth N. Hetu
 * \author Harish
 */
class Trip: public sim_mob::TripChainItem {
public:
	int tripID;
	sim_mob::Node* fromLocation;
	TripChainItem::LocationType fromLocationType;
	sim_mob::Node* toLocation;
	TripChainItem::LocationType toLocationType;

	Trip();
	Trip(int entId, std::string type, unsigned int seqNumber, DailyTime start,
			DailyTime end, int tripId, Node* from, std::string fromLocType,
			Node* to, std::string toLocType);
	virtual ~Trip();
	void addSubTrip(sim_mob::SubTrip* aSubTrip);

	std::vector<SubTrip*> getSubTrips() const {
		return subTrips;
	}

	void setSubTrips(const std::vector<SubTrip*>& subTrips) {
		this->subTrips = subTrips;
	}

private:
	std::vector<SubTrip*> subTrips;
};

/**
 * \author Harish
 */
class SubTrip: public sim_mob::Trip {
public:
	sim_mob::Trip* parentTrip;
	std::string mode;
	bool isPrimaryMode;
	std::string ptLineId; //Public transit (bus or train) line identifier.

	SubTrip();
	SubTrip(int entId, std::string type, unsigned int seqNumber,
			DailyTime start, DailyTime end, Node* from,
			std::string fromLocType, Node* to, std::string toLocType, Trip* parent, std::string mode,
			bool isPrimary, std::string ptLineId);
	virtual ~SubTrip();
};

}
