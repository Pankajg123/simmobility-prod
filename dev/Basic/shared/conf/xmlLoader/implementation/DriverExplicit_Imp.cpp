#include "conf1-pimpl.hpp"

using namespace sim_mob::conf;

#include <stdexcept>
#include <iostream>

using std::string;
using std::pair;



void sim_mob::conf::driver_explicit_pimpl::pre ()
{
	model = sim_mob::DriverSpec();

}

sim_mob::DriverSpec sim_mob::conf::driver_explicit_pimpl::post_driver_explicit ()
{
	return model;
}

void sim_mob::conf::driver_explicit_pimpl::property (const std::pair<std::string, std::string>& value)
{
	model.properties[value.first] = value.second;
}

void sim_mob::conf::driver_explicit_pimpl::originPos (const ::std::string& value)
{
	std::pair<uint32_t, uint32_t> pt = parse_point(value);
	model.origin = sim_mob::Point2D(pt.first, pt.second);
}

void sim_mob::conf::driver_explicit_pimpl::destPos (const ::std::string& value)
{
	std::pair<uint32_t, uint32_t> pt = parse_point(value);
	model.dest = sim_mob::Point2D(pt.first, pt.second);
}

void sim_mob::conf::driver_explicit_pimpl::startTime (const ::std::string& value)
{
	model.startTimeMs = parse_start_time_ms(value);
}

void sim_mob::conf::driver_explicit_pimpl::startFrame (int value)
{
	//We convert based on the "base_units", which have/will be/been validated elsewhere.
	model.startTimeMs = value * config->simulation().baseGranularity.ms();
}
