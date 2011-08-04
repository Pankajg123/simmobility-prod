/* Copyright Singapore-MIT Alliance for Research and Technology */

#pragma once

#include <bitset>


namespace sim_mob
{

/**
 * A lane ...
 *
 * When the StreetDirectory creates a Lane, it needs to set up the rules for that lane.  Since
 * the bitset constructor will set the bits to false, the StreetDirectory would only need to
 * set up the rules which are to be true.
 *   \code
 *   // A road with 3 lanes for vehicle traffic.
 *   // Any vehicle on the middle lane can filter to the left lane only
 *   // if it is a bus or the driver is not law-abiding.
 *   // Vehicles on the right lane may turn right even though the signal
 *   // is red.
 *
 *   Lane* leftLane = new Lane;
 *   leftLane.is_vehicle_lane(true);
 *   leftLane.can_go_straight(true);
 *   leftLane.can_turn_left(true);
 *   leftLane.can_change_lane_right(true);
 *   leftLane.is_whole_day_bus_lane(true);
 *
 *   Lane* centerLane = new Lane;
 *   centerLane.is_vehicle_lane(true);
 *   centerLane.can_go_straight(true);
 *   centerLane.can_change_lane_left(true);
 *   centerLane.can_change_lane_right(true);
 *
 *   Lane* rightLane = new Lane;
 *   rightLane.is_vehicle_lane(true);
 *   rightLane.can_turn_right(true);
 *   rightLane.can_change_lane_left(true);
 *   rightLane.can_turn_on_red_signal(true);
 *   \endcode
 */
class Lane {
private:
    /**
     * Lane movement rules.
     *
     */
    enum LANE_MOVEMENT_RULES {
        // Let the compiler assigns value to each enum item.

	//Who can drive here.
	IS_BICYCLE_LANE,
	IS_PEDESTRIAN_LANE,
	IS_VEHICLE_LANE,
	IS_HIGH_OCCUPANCY_VEHICLE_LANE,

	//Will likely be moved to Intersection or Link
	IS_U_TURN_ALLOWED,

	//Lane changing rules, traffic light turning
        CAN_GO_STRAIGHT,
        CAN_TURN_LEFT,
        CAN_TURN_RIGHT,
	CAN_CHANGE_LANE_LEFT,
	CAN_CHANGE_LANE_RIGHT,
	CAN_TURN_ON_RED_SIGNAL,

	//Parking rules
	CAN_STOP_HERE,
	CAN_FREELY_PARK_HERE,

	//General driving restrictions
	IS_STANDARD_BUS_LANE,
	IS_WHOLE_DAY_BUS_LANE,
	IS_ROAD_SHOULDER,

        // Leave this as the last item, it is used as the template parameter of the
        // rules_ data member.
        MAX_LANE_MOVEMENT_RULES
    };


public:
        /** Return true if vehicles can go straight on this lane.  */
	bool can_go_straight() const { return rules_.test(CAN_GO_STRAIGHT); }

        /** Return true if vehicles can turn left from this lane.  */
	bool can_turn_left() const { return rules_.test(CAN_TURN_LEFT); }

        /** Return true if vehicles can turn right from this lane.  */
	bool can_turn_right() const { return rules_.test(CAN_TURN_RIGHT); }

        /** Return true if vehicles can turn from this lane even when the signal is red.  */
	bool can_turn_on_red_signal() const { return rules_.test(CAN_TURN_ON_RED_SIGNAL); }

        /**
         * Return true if vehicles can move to the adjacent lane on the left.
         *
         * Vehicles are unable to move to the adjacent left lane for several reasons:
         *   - there is no lane on the left.
         *   - the adjacent left lane is a raised pavement road divider.
         *   - the adjacent left lane is a road divider with railings.
         *   - the adjacent left lane is a sidewalk.
         *
         * Even if vehicles are allowed to move to the adjacent left lane, the lane could be
         * a designated bus lane or road shoulder.
         */
	bool can_change_lane_left() const { return rules_.test(CAN_CHANGE_LANE_LEFT); }

        /**
         * Return true if vehicles can move to the adjacent lane on the right.
         *
         * Vehicles are unable to move to the adjacent right lane for several reasons:
         *   - there is no lane on the right.
         *   - the adjacent right lane is a raised pavement road divider.
         *   - the adjacent right lane is a road divider with railings.
         *   - the adjacent right lane is a sidewalk.
         *
         * Even if vehicles are allowed to move to the adjacent right lane, the lane could be
         * a designated bus lane or road shoulder.
         */
	bool can_change_lane_right() const { return rules_.test(CAN_CHANGE_LANE_RIGHT); }

        /** Return true if this lane is a designated road shoulder.  */
	bool is_road_shoulder() const { return rules_.test(IS_ROAD_SHOULDER); }

        /** Return true if this lane is reserved for cyclists.  */
	bool is_bicycle_lane() const { return rules_.test(IS_BICYCLE_LANE); }

        /** Return true if this lane is reserved for pedestrians.  Duh, it's a sidewalk.  */
	bool is_pedestrian_lane() const { return rules_.test(IS_PEDESTRIAN_LANE); }

        /** Return true if this lane is reserved for vehicle traffic.  */
	bool is_vehicle_lane() const { return rules_.test(IS_VEHICLE_LANE); }

	/** Return true if this lane is reserved for buses during bus lane operation hours.  */
	bool is_standard_bus_lane() const { return rules_.test(IS_STANDARD_BUS_LANE); }

        /** Return true if this lane is reserved for buses for the whole day.  */
	bool is_whole_day_bus_lane() const { return rules_.test(IS_WHOLE_DAY_BUS_LANE); }

        /**
         * Return true if this lane is reserved for high-occupancy vehicles.
         *
         * A bus lane (standard or whole-day) is reserved for buses.  But a high-occupancy-vehicle
         * lane can be used by both buses and car-pools.  */
	bool is_high_occupancy_vehicle_lane() const { return rules_.test(IS_HIGH_OCCUPANCY_VEHICLE_LANE); }

        /**
         * Return true if agents can park their vehicles on this lane.
         *
         * The agent may have to pay to park on this lane.
         */
	bool can_freely_park_here() const { return rules_.test(CAN_FREELY_PARK_HERE); }

        /**
         * Return true if agents can stop their vehicles on this lane temporarily.
         *
         * The agent may stop their vehicles for passengers to board or alight the vehicle
         * or for loading and unloading purposes.
         */
	bool can_stop_here() const { return rules_.test(CAN_STOP_HERE); }

	/** Return true if vehicles can make a U-turn on this lane.  */
	bool is_u_turn_allowed() const { return rules_.test(IS_U_TURN_ALLOWED); }


private:
        friend class StreetDirectory;

        /** If \c value is true, vehicles can go straight on this lane.  */
        void can_go_straight(bool value) { rules_.set(CAN_GO_STRAIGHT, value); }

        /** If \c value is true, vehicles can turn left from this lane.  */
        void can_turn_left(bool value) { rules_.set(CAN_TURN_LEFT, value); }

        /** If \c value is true, vehicles can turn right from this lane.  */
        void can_turn_right(bool value) { rules_.set(CAN_TURN_RIGHT, value); }

        /** If \c value is true, vehicles can turn from this lane even when the signal is red.  */
	void can_turn_on_red_signal(bool value) { rules_.set(CAN_TURN_ON_RED_SIGNAL, value); }

        /** If \c value is true, vehicles can move to the adjacent lane on the left.  */
	void can_change_lane_left(bool value) { rules_.set(CAN_CHANGE_LANE_LEFT, value); }

        /** If \c value is true, vehicles can move to the adjacent lane on the right.  */
	void can_change_lane_right(bool value) { rules_.set(CAN_CHANGE_LANE_RIGHT, value); }

        /** If \c value is true, this lane is a designated road shoulder.  */
	void is_road_shoulder(bool value) { rules_.set(IS_ROAD_SHOULDER, value); }

        /** If \c value is true, this lane is reserved for cyclists.  */
	void is_bicycle_lane(bool value) { rules_.set(IS_BICYCLE_LANE, value); }

        /** If \c value is true, this lane is reserved for pedestrians.  */
	void is_pedestrian_lane(bool value) { rules_.set(IS_PEDESTRIAN_LANE, value); }

        /** If \c value is true, this lane is reserved for vehicle traffic.  */
	void is_vehicle_lane(bool value) { rules_.set(IS_VEHICLE_LANE, value); }

        /** If \c value is true, this lane is reserved for buses during bus lane operation hours.  */
	void is_standard_bus_lane(bool value) { rules_.set(IS_STANDARD_BUS_LANE, value); }

        /** If \c value is true, this lane is reserved for buses for the whole day.  */
	void is_whole_day_bus_lane(bool value) { rules_.set(IS_WHOLE_DAY_BUS_LANE, value); }

        /** If \c value is true, this lane is reserved for high-occupancy vehicles.  */
	void is_high_occupancy_vehicle_lane(bool value) { rules_.set(IS_HIGH_OCCUPANCY_VEHICLE_LANE, value); }

        /** If \c value is true, agents can park their vehicles on this lane.  */
	void can_freely_park_here(bool value) { rules_.set(CAN_FREELY_PARK_HERE, value); }

        /** If \c value is true, agents can stop their vehicles on this lane temporarily.  */
	void can_stop_here(bool value) { rules_.set(CAN_STOP_HERE, value); }

        /** If \c value is true, vehicles can make a U-turn on this lane.  */ 
	void is_u_turn_allowed(bool value) { rules_.set(IS_U_TURN_ALLOWED, value); }

private:
	std::bitset<MAX_LANE_MOVEMENT_RULES> rules_;



};





}
