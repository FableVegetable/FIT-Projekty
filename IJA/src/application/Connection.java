/* 
 * Project:     Implementing Block Editor in JavaFX
 * Course:      IJA 2018
 * Team name:   Tym xzubri00
 * Members: 
 *      Liscinsky Matus xlisci02
 *      Zubrik Tomas    xzubri00
 */
package application;
import java.util.Vector;

public class Connection {
    Port output;
    Port input;
    Link link;

    /**
     * Constructor of Connection
     * @param  output Output port
     * @param  input  Input port
     */
    public Connection(Port output, Port input) {
        this.output=output;
        this.input=input;

        if(this.input == null || this.output == null){
            System.exit(1);
        }
        
        this.input.connected_to=this.output;
    }

    /**
     * Function check compatibility of connected ports
     * @return 0 if ports are compatible, -1 otherwise
     */
    public int checkCompatibility(){

        Vector<String> keys1 = this.output.keys;
        Vector<String> keys2 = this.input.keys;

        if(keys1.size() == keys2.size())
        	if(keys1.equals(keys2))
                return 0;
        return -1;
    }

    
}
