/* 
 * Project: 	Implementing Block Editor in JavaFX
 * Course: 		IJA 2018
 * Team name: 	Tym xzubri00
 * Members: 
 *		Liscinsky Matus	xlisci02
 *		Zubrik Tomas 	xzubri00
 */
package application;

import javafx.scene.paint.Paint;
import javafx.scene.shape.Circle;

public class PortItem extends Circle{

	/**
	 * Port constructor represented as circle.
	 * @param x X start coordinate
	 * @param y Y start coordinate
	 * @param radius Radius of port
	 * @param color Color of port
	 */
	public PortItem (double x, double y, double radius, Paint color) {
		super(x,y,radius,color);	
	}
	
}
