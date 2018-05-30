/* 
 * Project: 	Implementing Block Editor in JavaFX
 * Course: 		IJA 2018
 * Team name: 	Tym xzubri00
 * Members: 
 *		Liscinsky Matus	xlisci02
 *		Zubrik Tomas 	xzubri00
 */
package application;
	
import java.io.File;
import java.io.IOException;
import java.util.Optional;

import application.MainScene.Mode;
import javafx.application.Application;
import javafx.geometry.Insets;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.ButtonBar.ButtonData;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Button;
import javafx.scene.control.Menu;
import javafx.scene.control.MenuBar;
import javafx.scene.control.MenuItem;
import javafx.scene.control.TextInputDialog;
import javafx.scene.control.ToolBar;
import javafx.scene.control.Tooltip;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;


public class Main extends Application {
	static int first_step = 1;
	static Block prev = null;
	public static void main(String[] args) {launch(args);}
	
	MainScene mainscene = new MainScene();
	
	/**
	 *  Main entry point for JavaFX application. The start method is called after the init method has returned.
	 * @param primaryStage The primary stage for this application, onto which the application scene can be set.
	 */
	@Override
	public void start(Stage primaryStage) {
		try {
			StackPane stackPaneLabel = new StackPane();
			
			mainscene.scheme.generateNewID();
			primaryStage.setTitle("Scheme".concat(String.valueOf(mainscene.scheme.getID()).concat(" - Block Editor")));		
			
			//Root node
			BorderPane root = new BorderPane();
			
			//MenuBar
			MenuBar menuBar = new MenuBar();
			menuBar.setPrefHeight(30);
			//Menu 1
			Menu menufile = new Menu("File");
			MenuItem menuitemNew = new MenuItem("New");
			menuitemNew.setOnAction(e->{
				
				if(mainscene.getChildren().isEmpty()) {
					mainscene.scheme.generateNewID();
					primaryStage.setTitle("Scheme".concat(String.valueOf(mainscene.scheme.getID()).concat(" - Block Editor")));
					mainscene.scheme.clearScheme();
					mainscene.clearScene();
				}else {				
					Alert alert = new Alert(AlertType.CONFIRMATION);
					alert.setTitle("Save Scheme");
					alert.setHeaderText("Do you want to save scheme before creating new?");
					ButtonType btnYes = new ButtonType("Yes");
					ButtonType btnNo = new ButtonType("No");
					ButtonType btnCancel = new ButtonType("Cancel", ButtonData.CANCEL_CLOSE);
					alert.getButtonTypes().setAll(btnYes, btnNo, btnCancel);
						
					Optional<ButtonType> result = alert.showAndWait();
					if(result.get() == btnYes) {
						FileChooser fileChooser = new FileChooser();
						fileChooser.setInitialFileName("Scheme".concat(String.valueOf(mainscene.scheme.getID())));
						fileChooser.setTitle("Save Scheme");
						File file = fileChooser.showSaveDialog(primaryStage);
						if(file != null) {
							mainscene.scheme.saveScheme(file.getAbsolutePath());
							mainscene.scheme.clearScheme();
							mainscene.clearScene();	
						}
					}else if(result.get() == btnNo) {
						mainscene.scheme.clearScheme();
						mainscene.clearScene();	
					}
					mainscene.scheme.generateNewID();
					primaryStage.setTitle("Scheme".concat(String.valueOf(mainscene.scheme.getID()).concat(" - Block Editor")));
				}							
			});
			
			MenuItem menuitemOpen = new MenuItem("Open");
			menuitemOpen.setOnAction(e->{
				if(mainscene.getChildren().isEmpty()) {
					mainscene.scheme.clearScheme();
					mainscene.clearScene();
					FileChooser fileChooserLoad = new FileChooser();
					fileChooserLoad.setTitle("Open Scheme");
					File fileLoad = fileChooserLoad.showOpenDialog(primaryStage);
					
					mainscene.scheme.clearScheme();
					mainscene.clearScene();
					
					if(fileLoad != null) {
						try {
							mainscene.scheme.loadScheme(fileLoad.getAbsolutePath());
							primaryStage.setTitle(fileLoad.getName().concat(" - Block Editor"));
						}catch (IOException e1) {
							e1.printStackTrace();
						}
					}	
				}else {	
					Alert alert = new Alert(AlertType.CONFIRMATION);
					alert.setTitle("Save Scheme");
					alert.setHeaderText("Do you want to save scheme before opening another?");
					ButtonType btnYes = new ButtonType("Yes");
					ButtonType btnNo = new ButtonType("No");
					ButtonType btnCancel = new ButtonType("Cancel", ButtonData.CANCEL_CLOSE);
					alert.getButtonTypes().setAll(btnYes, btnNo, btnCancel);
						
					Optional<ButtonType> result = alert.showAndWait();
					if(result.get() == btnYes) {
						FileChooser fileChooser = new FileChooser();
						fileChooser.setTitle("Save Scheme");
						fileChooser.setInitialFileName("Scheme".concat(String.valueOf(mainscene.scheme.getID())));
						File file = fileChooser.showSaveDialog(primaryStage);
						if(file != null) {
							mainscene.scheme.saveScheme(file.getAbsolutePath());
							mainscene.scheme.clearScheme();
							mainscene.clearScene();	
							FileChooser fileChooserLoad = new FileChooser();
							fileChooserLoad.setTitle("Open Scheme");
							File fileLoad = fileChooserLoad.showOpenDialog(primaryStage);
							
							mainscene.scheme.clearScheme();
							mainscene.clearScene();
							
							if(fileLoad != null) {
								try {
									mainscene.scheme.loadScheme(fileLoad.getAbsolutePath());
									primaryStage.setTitle(fileLoad.getName().concat(" - Block Editor"));
								}catch (IOException e1) {
									e1.printStackTrace();
								}
							}	
						}
					}else if(result.get() == btnNo) {
						mainscene.scheme.clearScheme();
						mainscene.clearScene();
						FileChooser fileChooserLoad = new FileChooser();
						fileChooserLoad.setTitle("Open Scheme");
						File fileLoad = fileChooserLoad.showOpenDialog(primaryStage);
							
						mainscene.scheme.clearScheme();
						mainscene.clearScene();
							
						if(fileLoad != null) {
							try {
								mainscene.scheme.loadScheme(fileLoad.getAbsolutePath());
								primaryStage.setTitle(fileLoad.getName().concat(" - Block Editor"));
							}catch (IOException e1) {
								e1.printStackTrace();
							}
						}	
					}		
				}
						
			});
			
			MenuItem menuitemSave = new MenuItem("Save");
			menuitemSave.setOnAction(e->{
				FileChooser fileChooser = new FileChooser();
				fileChooser.setTitle("Save Scheme");
				fileChooser.setInitialFileName("Scheme".concat(String.valueOf(mainscene.scheme.getID())));
				File file = fileChooser.showSaveDialog(primaryStage);		
				if(file != null) {
					mainscene.scheme.saveScheme(file.getAbsolutePath());
				}
			});
			
			MenuItem menuitemExit = new MenuItem("Exit");
			menuitemExit.setOnAction(e->{
				if(mainscene.getChildren().isEmpty()) {
					mainscene.scheme.clearScheme();
					mainscene.clearScene();
				}else {	
					Alert alert = new Alert(AlertType.CONFIRMATION);
					alert.setTitle("Save Scheme");
					alert.setHeaderText("Do you want to save scheme before exit?");
					ButtonType btnYes = new ButtonType("Yes");
					ButtonType btnNo = new ButtonType("No");
					ButtonType btnCancel = new ButtonType("Cancel", ButtonData.CANCEL_CLOSE);
					alert.getButtonTypes().setAll(btnYes, btnNo, btnCancel);
						
					Optional<ButtonType> result = alert.showAndWait();
					if(result.get() == btnYes) {
						FileChooser fileChooser = new FileChooser();
						fileChooser.setTitle("Save Scheme");
						fileChooser.setInitialFileName("Scheme".concat(String.valueOf(mainscene.scheme.getID())));
						File file = fileChooser.showSaveDialog(primaryStage);
						if(file != null) {
							mainscene.scheme.saveScheme(file.getAbsolutePath());
							mainscene.scheme.clearScheme();
							mainscene.clearScene();	
							System.exit(0);
						}
					}else if(result.get() == btnNo) {
						System.exit(0);	
					}		
				}
			});
			
			//Menu 2
			Menu menuabout = new Menu("Help");
			MenuItem menuitemHelp = new MenuItem("About");
			menuitemHelp.setOnAction(e->{
				Alert alert = new Alert(AlertType.INFORMATION);
        		alert.setTitle("Information about application");
        		alert.setHeaderText("Block Editor for basic complex numbers operations");
        		alert.setContentText("Usage: \n"
        			+ "    1. Choose operation from left toolbar.\n"
        			+ "    2. Choose mode from top toolbar (insert LINE, MOVE blocks).\n"
        			+ "    3. Click RUN to calculate scheme.\n"
        			+ "    4. Click STEP to start debugging (stepping calculation).\n");
        		alert.showAndWait();
			});
			menufile.getItems().addAll(menuitemNew, menuitemOpen, menuitemSave, menuitemExit);
			menuabout.getItems().addAll(menuitemHelp);
			menuBar.getMenus().addAll(menufile, menuabout);
					
			//BorderPane under menu
			BorderPane content = new BorderPane();
			
			//Toolbar with buttons
			ToolBar toolbar = new ToolBar();
			toolbar.setOpaqueInsets(new Insets(1,1,1,1));
			toolbar.setPrefHeight(45);
			
			//Toolbar Buttons
			Button btnDelete = new Button();
			setButtonPropertiesToolbar(btnDelete, "DELETE Mode ", "delete.png");
			btnDelete.setOnAction(e->{mainscene.mode = Mode.delete;});
			
			Button btnMove = new Button();
			setButtonPropertiesToolbar(btnMove, "MOVE Mode ", "pointer.png");
			btnMove.setOnAction(e->{mainscene.mode = Mode.move;});
			
			Button btnLine = new Button();
			setButtonPropertiesToolbar(btnLine, "LINE INSERT Mode ", "linepointer.png");
			btnLine.setOnAction(e->{mainscene.mode = Mode.insertLine;});
			
			Button btnStep = new Button();
			setButtonPropertiesToolbar(btnStep, "Start stepping calculation", "step.png");
			btnStep.setOnAction(e->{
				if(first_step == 1) {
					mainscene.scheme.clearPorts();
					askForValues();
					first_step = 0;
				}
				if(prev != null) {
					prev.blockitem.setStyle(""
					+ "-fx-border-color: black; "
					+ "-fx-border-width: 2; "
					+ "-fx-border-style: solid;"
					+ "-fx-background-color: white;");
				}
				Block tmp = mainscene.scheme.run(false);
				mainscene.updateTooltips();
				
				if(tmp != null) {
					prev = tmp;
					tmp.blockitem.setStyle(""
					+ "-fx-border-color: darkcyan; "
					+ "-fx-border-width: 2; "
					+ "-fx-border-style: dashed;"
					+ "-fx-background-color: white;");
				}else {
					first_step = 1;
					prev = null;
				}
			});

			Button btnRun = new Button();
			setButtonPropertiesToolbar(btnRun, "Run calculation", "run.png");
			btnRun.setOnAction(e->{
				mainscene.scheme.clearPorts();
				askForValues();
				mainscene.scheme.run(true);
				mainscene.updateTooltips();
			});

			toolbar.getItems().addAll(btnDelete, btnMove, btnLine, btnRun, btnStep);
			
			VBox tools = new VBox();
			tools.setPrefWidth(100);
			Button btnAdd = new Button();
			setButtonPropertiesVBox(btnAdd, "Adder", "add.png");
			btnAdd.setOnAction(e->{				
				mainscene.blocktype = MainScene.BlockType.add;
				mainscene.mode = Mode.move;
			});
			
			Button btnSub = new Button();
			setButtonPropertiesVBox(btnSub, "Subtractor", "sub.png");
			btnSub.setOnAction(e->{				
				mainscene.blocktype = MainScene.BlockType.sub;
				mainscene.mode = Mode.move;
			});
			
			Button btnMul = new Button();
			setButtonPropertiesVBox(btnMul, "Multiplier", "mul.png");
			btnMul.setOnAction(e->{				
				mainscene.blocktype = MainScene.BlockType.mul;
				mainscene.mode = Mode.move;
			});
			
			Button btnDiv = new Button();
			setButtonPropertiesVBox(btnDiv, "Divider", "div.png");
			btnDiv.setOnAction(e->{				
				mainscene.blocktype = MainScene.BlockType.div;
				mainscene.mode = Mode.move;
			});
			
			Button btnGetReal = new Button();
			setButtonPropertiesVBox(btnGetReal, "Get real part from complex number", "getReal.png");
			btnGetReal.setOnAction(e->{				
				mainscene.blocktype = MainScene.BlockType.getreal;
				mainscene.mode = Mode.move;
			});
			
			Button btnGetIm = new Button();
			setButtonPropertiesVBox(btnGetIm, "Get imaginary part from complex number", "getIm.png");
			btnGetIm.setOnAction(e->{				
				mainscene.blocktype = MainScene.BlockType.getim;
				mainscene.mode = Mode.move;
			});
			
			Button btnPolarForm = new Button();
			setButtonPropertiesVBox(btnPolarForm, "Get polar form from complex number", "polarForm.png");
			btnPolarForm.setOnAction(e->{				
				mainscene.blocktype = MainScene.BlockType.polar;
				mainscene.mode = Mode.move;
			});
			
			Button btnComplexFromNumbers = new Button();
			setButtonPropertiesVBox(btnComplexFromNumbers, "Get complex form from real and imaginary part", "complex.png");
			btnComplexFromNumbers.setOnAction(e->{				
				mainscene.blocktype = MainScene.BlockType.complexnums;
				mainscene.mode = Mode.move;
			});
			
			Button btnComplexFromMagnitude = new Button();
			setButtonPropertiesVBox(btnComplexFromMagnitude, "Get complex form from polar form", "complex.png");
			btnComplexFromMagnitude.setOnAction(e->{				
				mainscene.blocktype = MainScene.BlockType.complexmag;
				mainscene.mode = Mode.move;
			});
			
			tools.getChildren().addAll(
					btnAdd, btnSub, btnMul, btnDiv, btnGetReal,
					btnGetIm, btnPolarForm, btnComplexFromNumbers, btnComplexFromMagnitude);
			
			for(Node n:tools.getChildren()) {
				VBox.setMargin(n, new Insets(5,5,0,5));
			}
				
			root.setTop(menuBar);
			root.setCenter(content);
			
			content.setTop(toolbar);
			content.setLeft(tools);
			content.setCenter(mainscene);
			content.setBottom(stackPaneLabel);
					
			//Mouse events allowed
			mainscene.insertBlock();			
			
			Scene scene = new Scene(root, 1000, 600);
			primaryStage.setScene(scene);
			primaryStage.show();
		} catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	/**
	 *  Set toolbar button properties(width, height, tooltip text, image on block).
	 * @param b Button
	 * @param tptext Tooltip text
	 * @param imagename Image on block representing operation
	 */
	public void setButtonPropertiesToolbar(Button b, String tptext, String imagename) {
		Tooltip tp = new Tooltip(tptext);
		Tooltip.install(b, tp);
		setButtonSize(b, 30);
		Image image = new Image(this.getClass().getClassLoader().getResourceAsStream(imagename));
		b.setGraphic(new ImageView(image));
	}
	
	/**
	 *  Set left bar button properties(width, height, tooltip text, image on block).
	 * @param b Button
	 * @param tptext Tooltip text
	 * @param imagename Image on block representing operation
	 */
	public void setButtonPropertiesVBox(Button b, String tptext, String imagename) {
		Tooltip tp = new Tooltip(tptext);
		Tooltip.install(b, tp);
		b.setPrefSize(100, 40);
		Image image = new Image(this.getClass().getClassLoader().getResourceAsStream(imagename));
		b.setGraphic(new ImageView(image));
	}
	
	/**
	 *  Set button properties: width and height.
	 * @param b Button
	 * @param size Height and width of button
	 */
	public void setButtonSize(Button b, int size) {
		b.setPrefSize(size, size);
		b.setMinSize(size, size);
		b.setMaxSize(size, size);
	}
	
	/**
	 *  Ask for values from user when button RUN or STEP is clicked. 
	 *  Select certain block and port with color border and fill to which the value is being set.
	 */
	public void askForValues() {
		int count=0;
		PortItem pitem;
		for(Block b : mainscene.scheme.Blocks){
				b.blockitem.setStyle(""
				+ "-fx-border-color: purple; "
				+ "-fx-border-width: 2; "
				+ "-fx-border-style: dotted;"
				+ "-fx-background-color: white;");
			count=0;
			for(Port p : b.inPorts) {
				count++;
			    if(p.connected_to == null){
			    	if(count==1) {
			    		pitem = b.blockitem.inport1;
			    		b.blockitem.inport1.setFill(Color.TOMATO);
			    	}else {
			    		pitem = b.blockitem.inport2;
			    		b.blockitem.inport2.setFill(Color.TOMATO);
			    	}
		            for(String key : p.keys) {
		             	TextInputDialog dialog = new TextInputDialog("1");
		               	dialog.setTitle("Insert Value");
		               	dialog.setHeaderText("Enter "+key+" value:");
		              	dialog.setContentText(key);
				                	
		               	double value;
		               	while(true) {
			               	Optional<String>result = dialog.showAndWait();
			               	if(result.isPresent()) {
			            		
				               	try {
				              		value = Double.parseDouble(result.get());
				               		break;
				               	}catch (Exception x){
				               		Alert alert = new Alert(AlertType.ERROR);
				               		alert.setTitle("Error occured");
				               		alert.setHeaderText("Value must be double!");
				               		alert.showAndWait();
				               	}
			               	}
		               	}            	
		               	p.m.put(key, value);              	
		            }
		            pitem.setFill(Color.BLACK);
			    }
			}
			    b.blockitem.setStyle(""
			    + "-fx-border-color: black; "
				+ "-fx-border-width: 2; "
				+ "-fx-border-style: solid;"
				+ "-fx-background-color: white;");
		}	    
	}
}
