/* 
 * Project: 	Implementing Block Editor in JavaFX
 * Course: 		IJA 2018
 * Team name: 	Tym xzubri00
 * Members: 
 *		Liscinsky Matus	xlisci02
 *		Zubrik Tomas 	xzubri00
 */
package application;
public class GetImaginary extends Block{
	
	/**
	 * Constructor of GetImaginary block
	 */
	public GetImaginary() {
		this.name="getIm";

	    Port p1 = new Port();
	    p1.in_block=this;
	    p1.addTypeKey("real");
	    p1.addTypeKey("im");
	    this.addInPort(p1);

	    Port out = new Port();
	    out.addTypeKey("im");
	    out.in_block=this;
	    this.addOutPort(out);
	}

	/**
	 * Function gets imaginary part of complex number and writes it to output port
	 */
	public void Execute() {
		double imaginary = this.inPorts.get(0).m.get("im");
	    this.outPorts.get(0).setImaginary(imaginary);	
	}
    
}
