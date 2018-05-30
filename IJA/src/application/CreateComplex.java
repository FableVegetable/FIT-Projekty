/* 
 * Project: 	Implementing Block Editor in JavaFX
 * Course: 		IJA 2018
 * Team name: 	Tym xzubri00
 * Members: 
 *		Liscinsky Matus	xlisci02
 *		Zubrik Tomas 	xzubri00
 */
package application;
public class CreateComplex extends Block {
	
	/**
	 * Constructor of CreateComplex block
	 */
	public CreateComplex() {
		this.name = "createComplex";
	    Port p1 = new Port();
	    Port p2 = new Port();
	
	    p1.in_block=this;
	    p1.addTypeKey("real");
	
	    p2.in_block=this;
	    p2.addTypeKey("im");
	
	    this.addInPort(p1);
	    this.addInPort(p2);
	
	    Port out = new Port();
	    out.in_block=this;
	    out.addTypeKey("real");
	    out.addTypeKey("im");
	
	    this.addOutPort(out);
	}
	
	/**
	 * Function create complex number from real and imaginary parts and writes to output port
	 */
	public void Execute(){
	    double real = this.inPorts.get(0).m.get("real");
	    double imaginary = this.inPorts.get(1).m.get("im");
	    this.outPorts.get(0).setComplex(real, imaginary);
	}
	
	
	
}
