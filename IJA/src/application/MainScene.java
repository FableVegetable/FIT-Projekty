/* 
 * Project: 	Implementing Block Editor in JavaFX
 * Course: 		IJA 2018
 * Team name: 	Tym xzubri00
 * Members: 
 *		Liscinsky Matus	xlisci02
 *		Zubrik Tomas 	xzubri00
 */
package application;

import javafx.geometry.Bounds;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.control.Tooltip;
import javafx.scene.control.Alert;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.layout.AnchorPane;

public class MainScene extends AnchorPane{
	
	public enum BlockType {
		add, sub, mul, div, getreal, getim, polar, complexnums, complexmag, nth
	}
	
	public enum Mode{
		insertItem, insertLine, move, delete
	}
	
	double xcoor = 0;
	double ycoor = 0;
	Scheme scheme;
	BlockType blocktype;
	Mode mode;
	Link l = new Link();
	BlockItem Selected_block; /* NEW */
	
	/**
	  *  MainScene constructor.
	  */
	public MainScene() {
		scheme = new Scheme();
		scheme.mainscene = this;
		this.blocktype = BlockType.nth;
		this.mode = Mode.move;
		drawLine();
	}
	
	/**
	 *  Insert block on load with certain coordinates.
	 * @param type Type of block
	 * @param x Initial coordinate x
	 * @param y Initial coordinate y
	 */
	public void insertBlockOnLoad(BlockType type, double x, double y) {
		BlockItem block = new BlockItem(type);
		block.b.blockitem = block;
		this.getChildren().add(block);
		block.setLayoutX(x);
		block.setLayoutY(y);
		block.allowDragging();
		scheme.addBlock(block.b);
		block.b.position[0] = x;
		block.b.position[1] = y;
		this.updateTooltips();
		this.blocktype = BlockType.nth;
	}
	
	/**
	 *  Insert new block on coordinates where mouse was clicked.
	 */
	public void insertBlock() {
		this.setOnMouseClicked(e->{				
			if(this.blocktype != BlockType.nth) {
				BlockItem block = new BlockItem(this.blocktype);
				block.b.blockitem = block;
				this.getChildren().add(block);
				block.relocate(e.getX() - 40, e.getY() - 45);				
				block.allowDragging();
				scheme.addBlock(block.b);
				block.b.position[0] = block.getLayoutX();
				block.b.position[1] = block.getLayoutY();
				this.updateTooltips();	
				this.blocktype = BlockType.nth;
			}
		});
	}
	
	/**
	 *  Update tooltips value on ports and links between blocks.
	 */
	public void updateTooltips() {
    	for(Block b: this.scheme.Blocks) {
    		boolean first = true;
    		for(Port p: b.inPorts) {
    			Tooltip tp = new Tooltip(p.mapToString());
    			if (first) {
    				Tooltip.install(p.in_block.blockitem.inport1, tp);
    				first=false;
    			}
    			else 
        			Tooltip.install(p.in_block.blockitem.inport2, tp);
    		}
    		for(Port p: b.outPorts) {
    			Tooltip tp = new Tooltip(p.mapToString());
    			Tooltip.install(p.in_block.blockitem.outport, tp);
    		}
    	}
    	for(Connection c : this.scheme.Connections) {
    		Tooltip tp = new Tooltip(c.output.mapToString());
    		Tooltip.install(c.link, tp);
    	}
    }
	
	/**
	 *  Update tooltips value on ports and links between blocks.
	 * @param indexOut Index of destination block in vector blocks.
	 * @param indexIn Index of sourceblock in vector blocks.
	 * @param portY Y position of input port. 
	 */
	public void insertLineOnLoad(int indexOut, int indexIn, double portY) {
		Block srcblock = this.scheme.Blocks.get(indexOut);
		Block destblock = this.scheme.Blocks.get(indexIn);

		Link newlink = new Link(
				srcblock.blockitem.getLayoutX() + 80, srcblock.blockitem.getLayoutY() + 45, 
				destblock.blockitem.getLayoutX(), destblock.blockitem.getLayoutY() + portY);
				
		destblock.blockitem.reservedPorts.set((portY == 65) ? 1 : 0, true);
		srcblock.blockitem.reservedPorts.set(2, true);
	
		newlink.destblock = destblock.blockitem;
		newlink.srcblock = srcblock.blockitem;
		newlink.destblock.connectedLinks.add(newlink);
		newlink.srcblock.connectedLinks.add(newlink);		
		newlink.con = new Connection(
		newlink.srcblock.b.outPorts.get(0),
		newlink.destblock.b.inPorts.get((portY == 65) ? 1 : 0));
		newlink.con.link = newlink;
		this.scheme.addConnection(newlink.con);
		this.getChildren().add(newlink);
	}
	
	/**
	 *  Draw connection between source and destination block on release mouse event.
	 */
	public void drawLine() {

		this.setOnMousePressed(e->{
			if(this.mode == Mode.insertLine) {
				xcoor = e.getX();
				ycoor = e.getY();
				l.setStartX(e.getX());
				l.setStartY(e.getY());	
				l.setEndX(l.getStartX());
				l.setEndY(l.getStartY());
				Node node;
				node = (Node)e.getSource();				
				if(node instanceof Parent) {
					
					BlockItem blockptr;
					for(Node n : this.getChildren()) {
						try {
							blockptr = (BlockItem) n;
							Bounds box = blockptr.localToScene(node.getBoundsInLocal());
							
							if(isInBlock(xToLocal(e.getX()), yToLocal(e.getY()),box.getMinX(), box.getMinY())){
								if(blockptr.reservedPorts.get(2) == false) {
									l.srcblock = blockptr;
									xcoor = box.getMinX()-20;
									ycoor = box.getMinY()-30;
								}else{
									l.srcblock = null;
								}
								break;
							}
						}catch(Exception x) {
							l.srcblock = null;
						}
					}		
				}
				this.getChildren().add(l);
			}
		});
		
		this.setOnMouseDragged(e-> {
			if(this.mode == Mode.insertLine) {
				l.setEndX(e.getX());
				l.setEndY(e.getY());			
			}
		});
		
		this.setOnMouseReleased(e-> {
			if(this.mode == Mode.insertLine) {
				this.getChildren().remove(l);
				
				if(l.srcblock != null) {					
					Node node;
					node = (Node)e.getSource();				
					if(node instanceof Parent) {
						
						BlockItem blockptr;
						for(Node n : this.getChildren()) {
							try {
								blockptr = (BlockItem) n;
								Bounds box = blockptr.localToScene(node.getBoundsInLocal());
								
								if(isInBlock(xToLocal(e.getX()), yToLocal(e.getY()),box.getMinX(), box.getMinY())){
									if(l.srcblock != blockptr)
									{
										if(blockptr.reservedPorts.get(1) == null) {
											if(blockptr.reservedPorts.get(0) == false) {
												Link newlink = new Link(xcoor, ycoor, box.getMinX()-100, box.getMinY()-32);
												
												Tooltip tooltip = new Tooltip("new tooltip");
												Tooltip.install(newlink, tooltip);
												
												blockptr.reservedPorts.set(0, true);
												l.destblock = blockptr;
												l.srcblock.reservedPorts.set(2, true);
												
												l.destblock.connectedLinks.add(newlink);
												l.srcblock.connectedLinks.add(newlink);
												
												newlink.destblock = l.destblock;
												newlink.srcblock = l.srcblock;
												
												newlink.con = new Connection(
														newlink.srcblock.b.outPorts.get(0),
														newlink.destblock.b.inPorts.get(0));
												
												newlink.con.link = newlink;												
												
												this.scheme.addConnection(newlink.con);
												this.getChildren().add(newlink);
												
												this.updateTooltips();
												
												/* change */
												if(newlink.con.checkCompatibility() == -1 || this.scheme.checkForCycle() == -1) {
													if( this.scheme.checkForCycle() == -1)
														showCycleError();
													else
														showCompatibiltyError();
													blockptr.reservedPorts.set(0, false);
													l.srcblock.reservedPorts.set(2, false);
													
													l.destblock.connectedLinks.remove(newlink);
													l.srcblock.connectedLinks.remove(newlink);
													this.scheme.removeConnection(newlink.con);
													this.getChildren().remove(newlink);
												}
											}
										}else {
											
											if(blockptr.reservedPorts.get(0) == false) {
												Link newlink = new Link(xcoor, ycoor, box.getMinX()-100, box.getMinY()-52);
																					
												Tooltip tooltip = new Tooltip("new tooltip");
												Tooltip.install(newlink, tooltip);
												
												
												blockptr.reservedPorts.set(0, true);
												l.destblock = blockptr;
												l.srcblock.reservedPorts.set(2, true);
												
												l.destblock.connectedLinks.add(newlink);
												l.srcblock.connectedLinks.add(newlink);
												
												newlink.destblock = l.destblock;
												newlink.srcblock = l.srcblock;
												
												
												newlink.con = new Connection(
														newlink.srcblock.b.outPorts.get(0),
														newlink.destblock.b.inPorts.get(0));
												
												newlink.con.link = newlink;
												
												
												this.scheme.addConnection(newlink.con);
												
												
												
												this.getChildren().add(newlink);
												
												
												this.updateTooltips();

												/* change */
												if(newlink.con.checkCompatibility() == -1 || this.scheme.checkForCycle() == -1) {
													if( this.scheme.checkForCycle() == -1)
														showCycleError();
													else
														showCompatibiltyError();
													blockptr.reservedPorts.set(0, false);
													l.srcblock.reservedPorts.set(2, false);
													
													l.destblock.connectedLinks.remove(newlink);
													l.srcblock.connectedLinks.remove(newlink);
													this.scheme.removeConnection(newlink.con);
													this.getChildren().remove(newlink);
												}
												
											}
											else if(blockptr.reservedPorts.get(1) == false) {
												Link newlink = new Link(xcoor, ycoor, box.getMinX()-100, box.getMinY()-12);
												
												Tooltip tooltip = new Tooltip("new tooltip");
												Tooltip.install(newlink, tooltip);
												
												
												blockptr.reservedPorts.set(1, true);
												l.destblock = blockptr;
												l.srcblock.reservedPorts.set(2, true);
												
												l.destblock.connectedLinks.add(newlink);
												l.srcblock.connectedLinks.add(newlink);
												
												newlink.destblock = l.destblock;
												newlink.srcblock = l.srcblock;
												
												newlink.con = new Connection(
														newlink.srcblock.b.outPorts.get(0),
														newlink.destblock.b.inPorts.get(1));
												
												newlink.con.link = newlink;
												
												
												this.scheme.addConnection(newlink.con);
												
												this.getChildren().add(newlink);
												this.updateTooltips();
												/* change */
												if(newlink.con.checkCompatibility() == -1 || this.scheme.checkForCycle() == -1) {
													if( this.scheme.checkForCycle() == -1)
														showCycleError();
													else
														showCompatibiltyError();
													blockptr.reservedPorts.set(0, false);
													l.srcblock.reservedPorts.set(2, false);
													
													l.destblock.connectedLinks.remove(newlink);
													l.srcblock.connectedLinks.remove(newlink);
													this.scheme.removeConnection(newlink.con);
													this.getChildren().remove(newlink);
												}
											}
										}
									}else {
										l.srcblock = null;
										l.destblock = null;
									}
									break;
								}
							}catch(Exception x) {
								
							}
						}		
					}
				}
			}
		});

	}
	
	/**
	 *  Show error message when cycle in scheme has been detected.
	 */
	public void showCycleError() {
		Alert alert = new Alert(AlertType.ERROR);
		alert.setTitle("Error Detected");
		alert.setHeaderText("Cycle in the scheme has been detected!");
		alert.showAndWait();
	}
	
	/**
	 *  Show error message when incompatible ports have been connected in scheme
	 */
	public void showCompatibiltyError() {
		Alert alert = new Alert(AlertType.ERROR);
		alert.setTitle("Error Detected");
		alert.setHeaderText("Ports with uncompatible types have been connected!");
		alert.showAndWait();
	}
	
	/**
	 *  Clear scene. Delete all blocks.
	 */
	public void clearScene() {
		this.getChildren().clear();
	}
	
	/**
	 *  Modify x coordinate to MainScene window.
	 *  @param x X coordinate to modify.
	 *  @return New value of x coordinate.
	 */
	public double xToLocal(double x) {
		return x + 100;
	}
	
	/**
	 *  Modify y coordinate to MainScene window.
	 *  @param y Y coordinate to modify.
	 *  @return New value of y coordinate.
	 */
	public double yToLocal(double y) {
		return y + 80;
	}
	
	/**
	 *  Modify x coordinate to MainScene window.
	 *  @param x X coordinate of mouse position
	 *  @param y Y coordinate of mouse position
	 *  @param minx Left top X coordinate of block in scheme
	 *  @param miny Left top Y coordinate of block in scheme
	 *  @return True if line starts in block. False if does not.
	 */
	public boolean isInBlock(double x, double y, double minx, double miny) {
		return (x > minx && x < (minx + 80) && y > miny && y < (miny + 90));
	}
}
