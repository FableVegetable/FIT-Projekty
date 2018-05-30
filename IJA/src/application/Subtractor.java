/* 
 * Project:     Implementing Block Editor in JavaFX
 * Course:      IJA 2018
 * Team name:   Tym xzubri00
 * Members: 
 *      Liscinsky Matus xlisci02
 *      Zubrik Tomas    xzubri00
 */
package application;

public class Subtractor extends ArithmBlock{
    
    /**
     * Constructor of Subtractor block, calls super constructor
     */
    public Subtractor() {
    	super("subtractor");
    }
    /**
     * Function subtract two complex numbers and writes the result to output port 
     */
    public void Execute() {
    	Complex num1 = new Complex(this.inPorts.get(0).m.get("real"),this.inPorts.get(0).m.get("im"));
    	Complex num2 = new Complex(this.inPorts.get(1).m.get("real"),this.inPorts.get(1).m.get("im"));
    	
    	Complex result = num1.minus(num2);
        this.outPorts.get(0).setComplex(result.re(), result.im());
    }
}

