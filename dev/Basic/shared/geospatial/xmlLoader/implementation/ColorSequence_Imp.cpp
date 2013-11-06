//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

#include "geo10-pimpl.hpp"

using namespace sim_mob::xml;


void sim_mob::xml::ColorSequence_t_pimpl::pre ()
{
	model.second.clear();
	model.first = sim_mob::InvalidTrafficLightType;
}

std::pair<sim_mob::TrafficLightType, std::vector<std::pair<sim_mob::TrafficColor,int> > > sim_mob::xml::ColorSequence_t_pimpl::post_ColorSequence_t ()
{

	return model;
}

void sim_mob::xml::ColorSequence_t_pimpl::TrafficLightType (sim_mob::TrafficLightType value)
{
	model.first = value;
//	std::cout << "TrafficLightType: " <<value << std::endl;
}

void sim_mob::xml::ColorSequence_t_pimpl::ColorDuration (std::pair<sim_mob::TrafficColor,int> value)
{
	model.second.push_back(value);
}


