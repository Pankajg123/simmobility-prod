/* Copyright Singapore-MIT Alliance for Research and Technology */

#include "TripChain.hpp"
#include <algorithm>

using std::string;
using namespace sim_mob;

sim_mob::TripChainItem::TripChainItem(int entId, string type, DailyTime start,
		DailyTime end, unsigned int seqNumber) :
		entityID(entId), itemType(getItemType(type)), startTime(start), endTime(
				end), sequenceNumber(seqNumber) {
}

sim_mob::TripChainItem::TripChainItem() {
}

sim_mob::TripChainItem::~TripChainItem() {
}

sim_mob::Activity::Activity() : TripChainItem(){
}

sim_mob::Activity::~Activity(){
}

sim_mob::Trip::Trip() : TripChainItem() {
}

sim_mob::Trip::Trip(int entId, std::string type, unsigned int seqNumber,
		DailyTime start, DailyTime end, int tripId, Node* from,
		std::string fromLocType, Node* to, std::string toLocType) :
		TripChainItem(entId, type, start, end, seqNumber), tripID(tripId), fromLocation(
				from), fromLocationType(getLocationType(fromLocType)), toLocation(
				to), toLocationType(getLocationType(toLocType)) {
}

sim_mob::Trip::~Trip() {
}

sim_mob::SubTrip::SubTrip() {
}

sim_mob::SubTrip::SubTrip(int entId, std::string type, unsigned int seqNumber,
		DailyTime start, DailyTime end, Node* from,
		std::string fromLocType, Node* to, std::string toLocType, Trip* parent, std::string mode,
		bool isPrimary, std::string ptLineId) : Trip(entId, type, seqNumber, start, end, 0, from, fromLocType, to, toLocType),
		parentTrip(parent), mode(mode) , isPrimaryMode(isPrimary), ptLineId(ptLineId){
}

sim_mob::SubTrip::~SubTrip() {
}

TripChainItem::LocationType sim_mob::TripChainItem::getLocationType(
		string locType) {
	locType.erase(remove_if(locType.begin(), locType.end(), isspace),
			locType.end());
	if (locType == "building") {
		return TripChainItem::LT_BUILDING;
	} else if (locType == "node") {
		return TripChainItem::LT_NODE;
	} else if (locType == "link") {
		return TripChainItem::LT_LINK;
	} else if (locType == "stop") {
		return TripChainItem::LT_PUBLIC_TRANSIT_STOP;
	} else {
		throw std::runtime_error("Unexpected location type.");
	}
}

TripChainItem::ItemType sim_mob::TripChainItem::getItemType(
		std::string itemType) {
	itemType.erase(remove_if(itemType.begin(), itemType.end(), isspace),
			itemType.end());
	if (itemType == "Activity") {
		return IT_ACTIVITY;
	} else if (itemType == "Trip") {
		return IT_TRIP;
	} else {
		throw std::runtime_error("Unknown trip chain item type.");
	}
}

void sim_mob::Trip::addSubTrip(sim_mob::SubTrip* aSubTrip) {
	subTrips.push_back(aSubTrip);
}
