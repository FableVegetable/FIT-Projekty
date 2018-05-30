/* 
 * Project: 	Implementing Block Editor in JavaFX
 * Course: 		IJA 2018
 * Team name: 	Tym xzubri00
 * Members: 
 *		Liscinsky Matus	xlisci02
 *		Zubrik Tomas 	xzubri00
 */
package application;

import javafx.scene.shape.Line;

public class Link extends Line {
	
	Connection con;
	BlockItem destblock=null, srcblock=null;
	boolean forbiddenOp = false;
	
	/**
	 * Link constructor that creates empty line with no start, no end.
	 */
	public Link() {
		super();
		this.setStrokeWidth(2);
	}
	
	/**
	 * Link constructor with start and end position.
	 * @param startX Starting X coordinate
	 * @param startY Starting Y coordinate
	 * @param endX Ending X coordinate
	 * @param endY Ending Y coordinate
	 */
	public Link(double startX, double startY, double endX, double endY) {
		super(startX, startY, endX, endY);
		this.setStrokeWidth(2);
	}
}
