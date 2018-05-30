/* 
 * Project: 	Implementing Block Editor in JavaFX
 * Course: 		IJA 2018
 * Team name: 	Tym xzubri00
 * Members: 
 *		Liscinsky Matus	xlisci02
 *		Zubrik Tomas 	xzubri00
 */
package application;
public class GetReal extends Block{
	
	/**
	 * Constructor of GetReal block
	 */
	public GetReal() {
		this.name="getReal";

	    Port p1 = new Port();
	    p1.in_block=this;
	    p1.addTypeKey("real");
	    p1.addTypeKey("im");
	    this.addInPort(p1);

	    Port out = new Port();
	    out.addTypeKey("real");
	    out.in_block=this;
	    this.addOutPort(out);
	}

	/**
	 * Function gets real part of complex number and writes it to output port
	 */
	public void Execute() {
		double real = this.inPorts.get(0).m.get("real");
	    this.outPorts.get(0).setReal(real);	
	}
    
}
