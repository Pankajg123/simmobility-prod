package sim_mob.vis.simultion;

import java.awt.Graphics2D;

import sim_mob.vis.controls.DrawableAgent;
import sim_mob.vis.controls.DrawableItem;
import sim_mob.vis.network.basic.ScaledPoint;


/**
 * One agent's tick within the simulation
 */
public abstract class AgentTick implements DrawableAgent {
	protected ScaledPoint pos;
	public ScaledPoint getPos() { return pos; }
	
}
