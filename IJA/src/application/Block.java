/* 
 * Project:     Implementing Block Editor in JavaFX
 * Course:      IJA 2018
 * Team name:   Tym xzubri00
 * Members: 
 *      Liscinsky Matus xlisci02
 *      Zubrik Tomas    xzubri00
 */
package application;
import java.util.HashMap;
import java.util.Vector;

public abstract class Block {
	String name;
	Vector<Port> inPorts = new Vector <Port>();
	Vector<Port> outPorts = new Vector <Port> ();
	BlockItem blockitem;
	double [] position = new double[2];

    /**
     * Function loads values on input ports, which are connected to another block
     */
	void Load() {

	    for (Port in : this.inPorts) {
	        if(in.connected_to != null){
	            in.m = new HashMap<String, Double> (in.connected_to.m);
	        }
	    }
	}
    
    /**
     * Function adds port to vector of input ports
     * @param p Port to be added
     */
	void addInPort(Port p) {
		this.inPorts.addElement(p);
	}

    /**
     * Function adds port to vector of output ports
     * @param p Port to be added
     */
    void addOutPort(Port p) {
    	this.outPorts.addElement(p);
    }
    
    /**
     * Function sets the position property of block
     * @param x x position
     * @param y y position
     */
    void setPosition(int x, int y) {
    	 this.position[0]=x;
    	 this.position[1]=y;
    }
    
    /**
     * Abstract method that executes calculation
     */
    public abstract void Execute();
    
}