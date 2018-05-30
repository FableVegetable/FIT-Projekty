/* 
 * Project: 	Implementing Block Editor in JavaFX
 * Course: 		IJA 2018
 * Team name: 	Tym xzubri00
 * Members: 
 *		Liscinsky Matus	xlisci02
 *		Zubrik Tomas 	xzubri00
 */
package application;
public class PolarToComplex extends Block{
	
	/**
	 * Constructor of PolarToComplex block
	 */
	public PolarToComplex()
	{
	    this.name = "polarToComplex";
	    Port p1 = new Port();

	    p1.in_block=this;
	    p1.addTypeKey("magnitude");
	    p1.addTypeKey("angle");

	    this.addInPort(p1);

	    Port out = new Port();
	    out.in_block=this;
	    out.addTypeKey("real");
	    out.addTypeKey("im");

	    this.addOutPort(out);
	}

	/**
	 * Function transforms complex number in polar form to cartesian form and writes it to output port
	 */
	public void Execute(){
		double magnitude = this.inPorts.get(0).m.get("magnitude");
	    double angle = this.inPorts.get(0).m.get("angle");

	    Complex num = Complex.polarToCartesian(magnitude, angle);
	    this.outPorts.get(0).setComplex(num.re(), num.im());

	}
}
