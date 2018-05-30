/* 
 * Project: 	Implementing Block Editor in JavaFX
 * Course: 		IJA 2018
 * Team name: 	Tym xzubri00
 * Members: 
 *		Liscinsky Matus	xlisci02
 *		Zubrik Tomas 	xzubri00
 */
package application;

public abstract class ArithmBlock extends Block {
	
	/**
	 * Universal constructor for blocks with arithmetic operations
	 * @param  name name of block
	 */
	public ArithmBlock(String name) {
		this.name = name;

	    Port p1 = new Port();
	    Port p2 = new Port();
	    p1.in_block=this;
	    p1.addTypeKey("real");
	    p1.addTypeKey("im");

	    p2.in_block=this;
	    p2.addTypeKey("real");
	    p2.addTypeKey("im");

	    this.addInPort(p1);
	    this.addInPort(p2);

	    Port out = new Port();
	    out.in_block=this;
	    out.addTypeKey("real");
	    out.addTypeKey("im");

	    this.addOutPort(out);
	}
}
