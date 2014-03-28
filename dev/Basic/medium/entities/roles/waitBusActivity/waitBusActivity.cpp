//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)



#include "waitBusActivity.hpp"
#include "waitBusActivityFacets.hpp"
#include "entities/Person.hpp"

using std::vector;
using namespace sim_mob;

namespace sim_mob {

namespace medium {

sim_mob::medium::WaitBusActivity::WaitBusActivity(Agent* parent, MutexStrategy mtxStrat,
		sim_mob::medium::WaitBusActivityBehavior* behavior,
		sim_mob::medium::WaitBusActivityMovement* movement) :
		sim_mob::Role(behavior, movement, parent, "WaitBusActivity_"),waitingTimeAtBusStop(0){

}

Role* sim_mob::medium::WaitBusActivity::clone(Person* parent) const {

	WaitBusActivityBehavior* behavior = new WaitBusActivityBehavior(parent);
	WaitBusActivityMovement* movement = new WaitBusActivityMovement(parent);
	WaitBusActivity* waitBusActivity = new WaitBusActivity(parent, parent->getMutexStrategy(),	behavior, movement);
	behavior->setParentWaitBusActivity(waitBusActivity);
	movement->setParentWaitBusActivity(waitBusActivity);
	return waitBusActivity;
}

void sim_mob::medium::WaitBusActivity::increaseWaitingTime(unsigned int ms){
	waitingTimeAtBusStop += ms;
}


}

}

