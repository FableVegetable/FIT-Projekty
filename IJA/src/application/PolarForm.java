/* 
 * Project: 	Implementing Block Editor in JavaFX
 * Course: 		IJA 2018
 * Team name: 	Tym xzubri00
 * Members: 
 *		Liscinsky Matus	xlisci02
 *		Zubrik Tomas 	xzubri00
 */
package application;
public class PolarForm extends Block {

	/**
	 * Constructor of PolarForm block
	 */
	public PolarForm() {
		this.name="polarForm";
	    Port p1 = new Port();
	    p1.addTypeKey("real");
	    p1.addTypeKey("im");
	    p1.in_block=this;
	    this.addInPort(p1);

	    Port out = new Port();
	    out.addTypeKey("magnitude");
	    out.addTypeKey("angle");
	    out.in_block=this;
	    this.addOutPort(out);
	}
	
	/**
	 * Function transforms complex number from input port to polar form and writes it to output port
	 */
	public void Execute() {
		Complex complexNum = new Complex(inPorts.get(0).m.get("real"),inPorts.get(0).m.get("im"));
		
		double magnitude = complexNum.abs();
		double phase_angle = complexNum.phase();
		
	    this.outPorts.get(0).setPolar(magnitude, phase_angle);
	}
}
