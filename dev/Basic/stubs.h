#pragma once

#include <vector>
#include <iostream>

#include "simple_classes.h"
#include "entities/Agent.hpp"
#include "workers/Worker.hpp"



//Function stubs
void loadUserConf(std::vector<Agent>& agents, std::vector<Region>& regions,
		          std::vector<TripChain>& trips, std::vector<ChoiceSet>& chSets,
		          std::vector<Vehicle>& vehicles)
{
	for (size_t i=0; i<20; i++)
		agents.push_back(Agent(i));
	for (size_t i=0; i<5; i++)
		regions.push_back(Region(i));
	for (size_t i=0; i<6; i++)
		trips.push_back(TripChain(i));
	for (size_t i=0; i<15; i++)
		chSets.push_back(ChoiceSet(i));
	for (size_t i=0; i<10; i++)
		vehicles.push_back(Vehicle(i));
	std::cout <<"Configuration file loaded." <<std::endl;
}
void setConfiguration() {
	std::cout <<"Server is configured." <<std::endl;
}
void loadNetwork() {
	std::cout <<"Network has been loaded." <<std::endl;
}
void loadSingleTripChain(const Agent* const ag, const TripChain* const tc) {
	trivial(tc->id);
}
void createSingleAgent(const Agent* const ag) {
	trivial(ag->getId()); //Trivial. Presumably, we'd set an agent's other properties here.
}
void createSingleChoiceSet(ChoiceSet* const cs, unsigned int newID) {
	cs->id = newID;
}
void createSingleVehicle(Vehicle* const v, unsigned int newID) {
	v->id = newID;
}
void updateSingleRegionSignals(Region& r) {
	for (size_t i=0; i<r.signals.size(); i++) {
		r.signals[i].id = r.signals[i].id; //Trivial. Will update signal's other properties.
	}
}
void updateSingleShortestPath(Agent& a) {
	trivial(a.getId()); //Trivial. Will update shortest path later.
}



//Example of using a Worker with a functional pointer instead of sub-classing.
void load_trip_chain(Worker* wk)
{
	//Using functional pointers instead of inheritance means we have to cast from void*
	for (std::vector<void*>::iterator it=wk->getEntities().begin(); it!=wk->getEntities().end(); it++) {
		TripChain* tc = (TripChain*)(*it);
		loadSingleTripChain(NULL, tc);   //At the moment, no way to link from agents to trip chains.
	}

	//Signal "done"
	wk->active.set(false);
}

void load_agents(Worker* wk)
{
	for (std::vector<void*>::iterator it=wk->getEntities().begin(); it!=wk->getEntities().end(); it++) {
		Agent* ag = (Agent*)(*it);
		createSingleAgent(ag);   //At the moment, no way to link from agents to trip chains.
	}

	//Signal "done"
	wk->active.set(false);
}

void load_choice_sets(Worker* wk)
{
	for (std::vector<void*>::iterator it=wk->getEntities().begin(); it!=wk->getEntities().end(); it++) {
		ChoiceSet* cs = (ChoiceSet*)(*it);
		createSingleChoiceSet(cs, cs->id);   //At the moment, no way to link from agents to trip chains.
	}

	//Signal "done"
	wk->active.set(false);
}

void load_vehicles(Worker* wk)
{
	for (std::vector<void*>::iterator it=wk->getEntities().begin(); it!=wk->getEntities().end(); it++) {
		Vehicle* vh = (Vehicle*)(*it);
		createSingleVehicle(vh, vh->id);   //At the moment, no way to link from agents to trip chains.
	}

	//Signal "done"
	wk->active.set(false);
}





//Quick double-check
bool checkIDs(const std::vector<Agent>& agents, const std::vector<TripChain>& trips, const std::vector<ChoiceSet>& choiceSets, const std::vector<Vehicle>& vehicles) {
	std::string error = "";
	for (size_t i=0; i<agents.size(); i++) {
		if (agents[i].getId() != i)
			error = "Agent ID";
	}
	for (size_t i=0; i<trips.size(); i++) {
		if (trips[i].id != i)
			error = "Trip Chain ID";
	}
	for (size_t i=0; i<choiceSets.size(); i++) {
		if (choiceSets[i].id != i)
			error = "Choice Set ID";
	}
	for (size_t i=0; i<vehicles.size(); i++) {
		if (vehicles[i].id != i)
			error = "Vehicle ID";
	}

	if (error.empty())
		return true;
	else {
		std::cout <<"Error, invalid " <<error <<std::endl;
		return false;
	}
}





