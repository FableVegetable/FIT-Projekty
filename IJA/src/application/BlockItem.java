/* 
 * Project: 	Implementing Block Editor in JavaFX
 * Course: 		IJA 2018
 * Team name: 	Tym xzubri00
 * Members: 
 *		Liscinsky Matus	xlisci02
 *		Zubrik Tomas 	xzubri00
 */
package application;

import java.util.Vector;

import application.MainScene.BlockType;
import application.MainScene.Mode;
import javafx.scene.Cursor;
import javafx.scene.control.Label;
import javafx.scene.control.Tooltip;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;


public class BlockItem extends Pane{
	
	PortItem inport1;
	PortItem inport2;
	PortItem outport;
	Label blockname;
	MainScene.BlockType type;
	Vector<Boolean>reservedPorts = new Vector <Boolean> ();
	int numOfPorts = 2;
	Vector<Link>connectedLinks = new Vector <Link> ();
	Block b;
	
	/**
	 * Constructor of block that represents block in main scene.
	 * @param type Type of block depending on operation
	 */
	public BlockItem(BlockType type){
		
		this.setStyle(""
		        + "-fx-border-color: black; "
		        + "-fx-border-width: 2; "
		        + "-fx-border-style: solid;"
		        + "-fx-background-color: white;");
		this.setPrefSize(80, 90);
		this.type = type;
		
		String imagename="";
		switch(type) {
			case add:
				b = new Adder();
				imagename = "add.png";
				break;
			case sub:
				b = new Subtractor();
				imagename = "sub.png";
				break;
			case mul:
				b = new Multiplier();
				imagename = "mul.png";
				break;
			case div:
				b = new Divider();
				imagename = "div.png";
				break;
			case getreal:
				b = new GetReal();
				imagename = "getReal.png";
				break;
			case getim:
				b = new GetImaginary();
				imagename = "getIm.png";
				break;
			case polar:
				b = new PolarForm();
				imagename = "polarForm.png";
				break;
			case complexnums:
				b = new CreateComplex();
				imagename = "complex.png";
				break;
			case complexmag:
				b = new PolarToComplex();
				imagename = "complex.png";
				break;
			case nth:
				imagename="";
				break;		
		}
		
		Image image = new Image(this.getClass().getClassLoader().getResourceAsStream(imagename));
		ImageView imageview = new ImageView(image);
		imageview.setLayoutX(20);
		imageview.setLayoutY(25);
				
		if(	type == BlockType.add || type == BlockType.sub || type == BlockType.mul || 
			type == BlockType.div ||  type == BlockType.complexnums) {
			inport1 = new PortItem(0, 25, 7, Color.BLACK);
			Tooltip tpport1 = new Tooltip("Inport1");			
			Tooltip.install(inport1, tpport1);
			
			inport2 = new PortItem(0, 65, 7, Color.BLACK);
			Tooltip tpport2 = new Tooltip("Inport2");
			Tooltip.install(inport2, tpport2);
			
			outport = new PortItem(80, 45,7, Color.BLACK);
			Tooltip tpport3 = new Tooltip("Outport");
			Tooltip.install(outport, tpport3);
			
			reservedPorts.add(false);
			reservedPorts.add(false);
			reservedPorts.add(false);
			
			this.getChildren().addAll(inport1, inport2, outport, imageview);
			
		}else {
		
			inport1 = new PortItem(0, 45, 7, Color.BLACK);
			outport = new PortItem(80, 45,7, Color.BLACK);
			
			reservedPorts.add(false);
			reservedPorts.add(null);
			reservedPorts.add(false);
			
			this.getChildren().addAll(inport1, outport, imageview);
		}	
	}
	
	/**
	 * Allow dragging block objects in the scene.
	 */
	public void allowDragging() {
		
		final Delta dragDelta = new Delta();
		
			this.setOnMouseClicked(e -> {
				if(((MainScene)this.getParent()).mode == Mode.insertLine) {	
				}else if(((MainScene)this.getParent()).mode == Mode.delete) {
					((MainScene)this.getParent()).scheme.removeBlock(this.b);
					for(Link l: this.connectedLinks) {
						((MainScene)this.getParent()).getChildren().remove(l);
					}
					((MainScene)this.getParent()).getChildren().remove(this);
				}
	        });
		 
			this.setOnMousePressed(e -> {
				if(((MainScene)this.getParent()).mode == Mode.move) {
		            if (e.isPrimaryButtonDown()) {
		            	this.getScene().setCursor(Cursor.DEFAULT);
		            }
		            dragDelta.x = e.getX();
		            dragDelta.y = e.getY();
		            this.getScene().setCursor(Cursor.MOVE);
				}
		
	        });
			this.setOnMouseReleased(e -> {
				if(((MainScene)this.getParent()).mode == Mode.move) {
					if (!e.isPrimaryButtonDown()) {
						this.getScene().setCursor(Cursor.DEFAULT);
						this.b.position[0] = this.getLayoutX();
						this.b.position[1] = this.getLayoutY();
					}
				}
	        });
			this.setOnMouseDragged(e->{
				if(((MainScene)this.getParent()).mode == Mode.move) {
					
					this.setLayoutX(this.getLayoutX() + e.getX() - dragDelta.x);
					this.setLayoutY(this.getLayoutY() + e.getY() - dragDelta.y);
					updateLinkPosition(this, this.getLayoutX() + e.getX() - dragDelta.x, this.getLayoutY() + e.getY() - dragDelta.y);
				}
	        });
	}
	
	/**
	 * Update link position while dragging the block.
	 * @param block Dragged block.
	 * @param x X coordinate for updating link position
	 * @param y Y coordinate for updating link position
	 */
	public void updateLinkPosition(BlockItem block, double x, double y) {
		
		if(block.reservedPorts.get(1) == null) {
			for(Link l : block.connectedLinks) {
				if(block == l.destblock) {				
					l.setEndX(x);
					l.setEndY(y+45);					
				}else if(block == l.srcblock) {
					l.setStartX(x+80);
					l.setStartY(y+45);	
				}
			}	
		}else {
			boolean first=true;
			for(Link l : block.connectedLinks) {
				if(block == l.destblock) {				
					l.setEndX(x);
					l.setEndY( (first==true) ? y+25  : y+65 );
					first=false;
					
				}else if(block == l.srcblock) {
					l.setStartX(x+80);
					l.setStartY(y+45);	
				}
			}
		}
	}
		
	private class Delta {
	    public double x;
	    public double y;
	}
}
