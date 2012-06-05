/* Copyright Singapore-MIT Alliance for Research and Technology */

/*
 * Signal.hpp
 *
 *  Created on: 2011-7-18
 *      Author: xrm
 *      Autore: vahid
 */

#pragma once

//If we're not using the "new signal" flag, just forward this header file to the old location.
//  This allows us to simply include "entities/signal/Signal.hpp" without reservation.
#include "GenConfig.h"
#ifndef SIMMOB_NEW_SIGNAL
#include "entities/Signal.hpp"
#else
#include <map>
#include <vector>
#include "entities/Agent.hpp"
#include "metrics/Length.hpp"
#include "util/SignalStatus.hpp"
#include "entities/LoopDetectorEntity.hpp"
#include "SplitPlan.hpp"
#include "Phase.hpp"
#include "Cycle.hpp"
#include "Offset.hpp"
#include "defaults.hpp"


#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/mem_fun.hpp>


namespace sim_mob
{
using boost::multi_index::multi_index_container;
using boost::multi_index::ordered_non_unique;
using boost::multi_index::ordered_unique;
using boost::multi_index::indexed_by;
using boost::multi_index::member;
using boost::multi_index::random_access;
using boost::multi_index::mem_fun;
// Forwared declarations.
class Node;
class Lane;
class Crossing;
class Link;

typedef struct
{
    sim_mob::Link* LinkTo;
    sim_mob::Link* LinkFrom;
    mutable TrafficColor currColor;//can change it directly as it is not a member of any key and it is mutable
}linkToLink_signal;

typedef multi_index_container<
		linkToLink_signal,
		indexed_by<
		random_access<>
//    ,ordered_unique<
//      composite_key<
//      sim_mob::Link*,
//        member<linkToLink_signal,sim_mob::Link*,&linkToLink_signal::LinkTo>,
//        member<linkToLink_signal,sim_mob::Link*,&linkToLink_signal::LinkFrom>
//      >
//    >
  >
> linkToLink_ck_C;

#ifndef SIMMOB_DISABLE_MPI
class PackageUtils;
class UnPackageUtils;
#endif
//Link and crossing of an intersection/traffic signal
struct LinkAndCrossing
{
	LinkAndCrossing(int id_,sim_mob::Link const * link_,sim_mob::Crossing const * crossing_,double angle_):
		id(id_),
		link(link_),
		crossing(crossing_),
		angle(angle_)
	{}
	size_t id;         //index for backward compatibility (setupindexMaps()
	double angle;         //index for backward compatibility (setupindexMaps()
	sim_mob::Link const * link;
	sim_mob::Crossing const * crossing;
};


typedef multi_index_container<
		LinkAndCrossing, indexed_by<
	 random_access<>															//0
    ,ordered_unique<member<LinkAndCrossing, size_t , &LinkAndCrossing::id> >//1
	,ordered_unique<member<LinkAndCrossing, sim_mob::Link const * , &LinkAndCrossing::link> >//2
	,ordered_non_unique<member<LinkAndCrossing, double , &LinkAndCrossing::angle> >//3
	,ordered_non_unique<member<LinkAndCrossing, sim_mob::Crossing const * , &LinkAndCrossing::crossing> >//4
   >
> LinkAndCrossingC;//Link and Crossing Container(multi index)
typedef nth_index<LinkAndCrossingC, 2>::type LinkAndCrossingByLink;
typedef nth_index<LinkAndCrossingC, 3>::type LinkAndCrossingByAngle;
typedef nth_index<LinkAndCrossingC, 4>::type LinkAndCrossingByCrossing;

typedef LinkAndCrossingByAngle::reverse_iterator LinkAndCrossingIterator;
typedef LinkAndCrossingByCrossing::iterator SignalCrossingIterator;

class Signal  : public sim_mob::Agent {

public:

	/*--------Initialization----------*/
	void initializeSignal();
	void setSplitPlan(sim_mob::SplitPlan);
	void setCycleLength(sim_mob::Cycle);
	Signal(Node const & node, const MutexStrategy& mtxStrat, int id=-1);
    static Signal const & signalAt(Node const & node, const MutexStrategy& mtxStrat);
    void addSignalSite(centimeter_t xpos, centimeter_t ypos,std::string const & typeCode, double bearing);
    void findIncomingLanes();
    void findSignalLinks();
    void findSignalLinksAndCrossings();
    /*links are sorted in order of their ascending angle with respect to the whole coordinate
     */
//    LinkAndCrossingIterator LinkAndCrossings_begin()const { return LinkAndCrossings_.get<3>().rbegin(); }
//    LinkAndCrossingIterator LinkAndCrossings_end()const { return LinkAndCrossings_.get<3>().rend(); }
//    LinkAndCrossingByLink &getLinkAndCrossingsByLink() {return LinkAndCrossings_.get<2>();}
    LinkAndCrossingByLink const & getLinkAndCrossingsByLink() const {return LinkAndCrossings_.get<2>();}
//    const std::vector<sim_mob::Link const *> & getSignalLinks() const;
    LoopDetectorEntity const & loopDetector() const { return loopDetector_; }


	/*--------Updation----------*/
	void updateSignal(double DS[]);
	void updateTrafficLights();
	void updatecurrSplitPlan();
	void updateOffset();
	virtual Entity::UpdateStatus update(frame_t frameNumber);
	void newCycleUpdate();
	bool updateCurrCycleTimer(frame_t frameNumber);


	/*--------Split Plan----------*/
	void startSplitPlan();
//	void setnextSplitPlan(double DS[]);
	int getcurrSplitPlanID();
	int getnextSplitPlanID();
	sim_mob::SplitPlan & getPlan();

//	std::vector<double> getNextSplitPlan();
//	std::vector<double> getCurrSplitPlan();


	/*--------Phase----------*/
	int getcurrPhase();
	int getphaseCounter(){return phaseCounter;}


	/*--------Degree of Saturation----------*/
	double computeDS();
	double LaneDS(const LoopDetectorEntity::CountAndTimePair& ctPair,double total_g);
	void calProDS_MaxProDS(std::vector<double> &proDS,std::vector<double>  &maxproDS);


	/*--------Miscellaneous----------*/
	Node  const & getNode() const { return node_; }
	void frame_output(frame_t frameNumber);
	int fmin_ID(const  std::vector<double>  maxproDS);
	///Return the loggable representation of this Signal.
	std::string toString() const { return strRepr; }
	unsigned int getSignalId()  {return TMP_SignalID;}

	/*--------The cause of this Module----------*/
    TrafficColor getDriverLight(Lane const & fromLane, Lane const & toLane) const ;
	TrafficColor getPedestrianLight(Crossing const & crossing) const;

	typedef multi_index_container<
			sim_mob::Signal *, indexed_by<
		 random_access<>															//0
	    ,ordered_unique<mem_fun<Signal, unsigned int ,&Signal::getSignalId> >//1
	   >
	> all_signals;

	static Signal::all_signals all_signals_;
	static const double updateInterval;

    void updateIndicators();

private:

    unsigned int TMP_SignalID;//todo change the name to withouth TMP

    /* Fixed time or adaptive control */
    int signalAlgorithm;
    /*-------------------------------------------------------------------------
     * -------------------Geo Spatial indicators--------------------------------
     * ------------------------------------------------------------------------*/
    /*The node associated with this traffic Signal */
    sim_mob::Node const & node_;
    //todo check whether we realy need it? (this container and the function filling it)
    /*check done! only Density vector needs it for its size!!! i.e a count for the number of lines would also do
     * the job. I don't think we need this but I am not ommitting it until I check wether it will be usefull for the loop detector
     * (how much usful)
     * else, no need to store so many lane pointers unnecessarily
     */
    std::vector<sim_mob::Lane const *>  IncomingLanes_;//The data for this vector is generated
    //used (probabely in createloopdetectors()

    LinkAndCrossingC LinkAndCrossings_;
    std::vector<sim_mob::Link const *>  SignalLinks;//The data for this vector is generated

    /*-------------------------------------------------------------------------
     * -------------------split plan Indicators--------------------------------
     * ------------------------------------------------------------------------*/
    /*
     * the split plan(s) used in this traffic signal are represented using this single variable
     * This variable has two main tasks
     * 1-hold plans' phase information and different combinations of phase time shares(choiceSet)
     * 2-decides/outputs/selects the next split plan(choiceSet combination) based on the the inputted DS
     */
    sim_mob::SplitPlan plan_;

	std::vector<double> currSplitPlan;//a chunk in "choiceSet" container,Don't think I will need it anymore coz job is distributed to a different class
	int currSplitPlanID;//Don't think I will need it anymore
	int phaseCounter;//Don't think I will need it anymore coz apparently currCycleTimer will replace it
	double currCycleTimer;//The amount of time passed since the current cycle started.(in millisecond)

    /*-------------------------------------------------------------------------
     * -------------------Density Indicators-----------------------------------
     * ------------------------------------------------------------------------*/
     /* -donna what this is, still change it to vector-----
     * update 1: it is used as an argument in updateSignal
     * update 2: probabely this is the DS at each lane(curent assumption) */
    std::vector<double> Density;
    //so far this value is used to store the max value in the above(Density) vector
    double DS_all;

    /*-------------------------------------------------------------------------
     * -------------------Cycle Length Indicators------------------------------
     * ------------------------------------------------------------------------*/
    sim_mob::Cycle cycle_;
	//previous,current and next cycle length
	double currCL;
	int currPhaseID;
	sim_mob::Phase currPhase;

	bool isNewCycle; //indicates whether operations pertaining to a new cycle should be performed

    /*-------------------------------------------------------------------------
     * -------------------Offset Indicators------------------------------
     * ------------------------------------------------------------------------*/
	//current and next Offset
	sim_mob::Offset offset_;
	double currOffset;




	//String representation, so that we can retrieve this information at any time.
	std::string strRepr;
//	sim_mob::Shared<SignalStatus> buffered_TC;

//	//private override for friends ;)
//	static Signal & signal_at(Node const & node, const MutexStrategy& mtxStrat);
	friend class DatabaseLoader;
protected:
        LoopDetectorEntity loopDetector_;

protected:
        void setupIndexMaps();
        void outputToVisualizer(frame_t frameNumber);

#ifndef SIMMOB_DISABLE_MPI
public:
    virtual void pack(PackageUtils& packageUtil){}
    virtual void unpack(UnPackageUtils& unpackageUtil){}

	virtual void packProxy(PackageUtils& packageUtil);
	virtual void unpackProxy(UnPackageUtils& unpackageUtil);
#endif
//	static std::vector< std::vector<double> > SplitPlan;
};//class Signal
typedef nth_index_iterator<Signal::all_signals, 0>::type all_signals_Iterator;
typedef nth_index_const_iterator<Signal::all_signals, 0>::type all_signals_const_Iterator;


}//namespace sim_mob
#endif
