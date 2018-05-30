/* 
 * Project:     Implementing Block Editor in JavaFX
 * Course:      IJA 2018
 * Team name:   Tym xzubri00
 * Members: 
 *      Liscinsky Matus xlisci02
 *      Zubrik Tomas    xzubri00
 */
package application;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Vector;

public class Scheme {
    int id;
    static int idGenerator=1;
	Vector<Block> Blocks;
    Vector<Connection> Connections;
    MainScene mainscene;
    
    /**
     * Constructor of scheme
     */
    public Scheme() {
    	Blocks = new Vector <Block> ();
    	Connections = new Vector <Connection> ();
    }
    
    /**
     * Function adds block to vector of Blocks in scheme
     * @param b Block to be added into scheme
     */
    public void addBlock(Block b) {
    	this.Blocks.addElement(b);
    }
    
    /**
     * Function adds connection to vector of Connections in scheme
     * @param c Connection to be added into scheme
     */
    public void addConnection(Connection c) {
    	this.Connections.addElement(c);
    }

    /**
     * Function saves information about scheme to the file
     * @param filename Path to the file
     */
    public void saveScheme(String filename) { // TODO
    	try {
	    	FileWriter fstream = new FileWriter(filename);
	    	BufferedWriter out = new BufferedWriter(fstream);
	    	for(Block b: this.Blocks) {
	    		out.write(b.blockitem.type.toString().concat(" "));
	    		out.write(String.valueOf(b.position[0]).concat(" "));
	    		out.write(String.valueOf(b.position[1]).concat("\n"));
	    	}
	    	out.write("#\n");
	    	for(Connection c: this.Connections) {
	    		out.write(String.valueOf(this.getIndexOfBlock(c.output.in_block)).concat(" "));
	    		out.write(String.valueOf(this.getIndexOfBlock(c.input.in_block)).concat(" "));
	    		if(c.input.in_block.inPorts.size() == 1) {
	    			out.write("45\n");
	    		}
	    		else if(c.input.in_block.inPorts.get(0) == c.input) {
	    			out.write("25\n");
	    		}else {
	    			out.write("65\n");
	    		}
	    	}
	    	out.close();
	    	
    	} catch (Exception e) {
    		System.err.println("error filewriter scheme");
    	}
    }

    /**
     * Function loads the scheme from file
     * @param  filename    Path to the file
     * @throws IOException Input-Output exception
     */
    public void loadScheme(String filename) throws IOException { 
    	 File file = new File(filename);
    	 BufferedReader reader = null;
    	 reader = new BufferedReader(new FileReader(file));
    	 String text = null;
    	 
    	 String line;
    	 String [] parsedLine;
    	 
    	 
    	  boolean hashTagFound=false;
    	 // repeat until all lines are read
    	 while ((text = reader.readLine()) != null) {
    		 //contents.append(text).append(System.getProperty("line.separator"));
    		 line = text;
    		 parsedLine = line.split(" ");
    		 
    		 if(text.contains("#")) {
    			 hashTagFound = true;
    			 continue;
    		 }
    		 if(!hashTagFound) {
    			 mainscene.insertBlockOnLoad(MainScene.BlockType.valueOf(parsedLine[0]), Double.parseDouble(parsedLine[1]), Double.parseDouble(parsedLine[2]));
    		 }else {
    			 mainscene.insertLineOnLoad(Integer.parseInt(parsedLine[0]), Integer.parseInt(parsedLine[1]), Double.parseDouble(parsedLine[2]));
    		 }
    		 
    		 System.out.print(line);
 
    		 
    	 }
    	 reader.close();
    }

    /**
     * Function gets index of block in vector of blocks in scheme
     * @param  b Block 
     * @return   index fo block in vector of blocks in scheme
     */
    public int getIndexOfBlock(Block b) {
    	int index = 0;
        for(Block t : this.Blocks){
            if(t == b)
                return index;
            index++;
        }
        return -1;
    }
    
    /**
     * Function calculate transitive closure of matrix
     * @param  graph matrix
     * @param  n     number of rows | columns of matrix (has to be square matrix)
     * @return       0 if matrix of transitive closure does not contain 1 on diagonal, -1 otherwise
     */
    public int transitiveClosure(int [] graph, int n) {
    	int[][] reach = new int[n][n];
    	int i, j, k;

        for(int a = 0; a < n; a++){
            for(int b = 0; b < n; b++){
                reach[a][b] = graph[a*n+b];
            }
        }
            
        for (k = 0; k < n; k++){
                // Pick all vertices as source one by one
        	for (i = 0; i < n; i++){
                    // Pick all vertices as destination for the
                    // above picked source
        		for (j = 0; j < n; j++){
                        // If vertex k is on a path from i to j,
                        // then make sure that the value of reach[i][j] is 1
        			if( reach[i][j] ==1 || (reach[i][k] == 1 && reach[k][j] == 1))
        				reach[i][j] = 1;
                    else 
                      	reach[i][j] = 0;
                    if(i == j && reach[i][j] == 1)
                        return -1;
                }
            }
        }
        return 0;
    }
    
    /**
     * Function check if cycle was created in scheme
     * @return 0 if cycle was not detected, -1 otherwise
     */
    public int checkForCycle() {
        int n = this.Blocks.size();
        int [] matrix = new int [n*n];
        for(Connection c : this.Connections){
            int input_idx = getIndexOfBlock(c.input.in_block);
            int output_idx = getIndexOfBlock(c.output.in_block);
            matrix[output_idx * n + input_idx] = 1;
       }
       int result = transitiveClosure(matrix, n);
       return result;
    }
    
    /**
     * Funtion calculate one block or all the blocks of scheme
     * @param  recursive defines whether all scheme blocks will be executed or not
     * @return block that has been executed
     */
    public Block run(boolean recursive) {
    	 // obmedzujuca podmienka
        int cond = 1;
        
        for(Block b : this.Blocks){
            for (Port p : b.outPorts){
                if(p.m.isEmpty())
                    cond = 0;
            }
        }
        if(cond == 1)
            return null;
        int flag;
        for(Block b : this.Blocks){
            flag = 0;
			 
            for (Port p : b.inPorts){
                b.Load(); // vzdy sa pokusi nacitat input z outputu
            	           	
                if (p.m.size() == 0){
                    flag=1; break;
                }
            }
            boolean emptyOutput = b.outPorts.get(0).m.isEmpty();
            if(flag == 0 && emptyOutput){ // ak vsetky vstupne porty naplnene vykona sa
                b.Execute();
                System.out.println("vykonal sa " + b.name);
                if(!recursive)
                    return b;
            }
        }
        if(recursive)
            run(true);
       return null;
    }
    
    /**
     * Function generates new id for scheme
     */
    public void generateNewID() {
    	this.id = idGenerator++;
    	
    }
    
    /**
     * Function gets the id of scheme
     * @return Id of scheme
     */
    public int getID() {
    	return id;
    }
    
    /**
     * Function clears all the ports of blocks in scheme
     */
    public void clearPorts() {
        for(Block b : this.Blocks){
            for (Port p : b.outPorts){
                p.m.clear();
            }
            for (Port p : b.inPorts){
                p.m.clear();
            }
        }
    }

    /**
     * Function removes the connection from scheme
     * @param c Connection to be removed
     */
    public void removeConnection(Connection c) {
    	c.input.connected_to=null;
    	this.Connections.remove(c);
    }
    
    /**
     * Function removes all Connections of Block from scheme
     * @param b Block
     */
    @SuppressWarnings("unchecked")
    public void removeConnectionsOfBlock(Block b) {
    	Vector <Connection> help = new Vector <Connection> ();
    	help = (Vector<Connection>) this.Connections.clone();

       for(Connection c : help){
           if(c.input.in_block == b || c.output.in_block == b)
               removeConnection(c);
       }
    }
    
    /**
     * Function removes the block and its connections from scheme
     * @param b [description]
     */
    public void removeBlock(Block b) {
    	removeConnectionsOfBlock(b);
    	this.Blocks.remove(b);
    }
    
    /**
     * Function removes all blocks and Connections from scheme
     */
    @SuppressWarnings("unchecked")
    public void clearScheme() {
    	Vector <Block> help = new Vector <Block> ();
    	help = (Vector<Block>) this.Blocks.clone();
    	for(Block b : help) {
    		removeBlock(b);
    	}
    }
    	
}
