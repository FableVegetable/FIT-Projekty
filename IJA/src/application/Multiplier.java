/* 
 * Project:     Implementing Block Editor in JavaFX
 * Course:      IJA 2018
 * Team name:   Tym xzubri00
 * Members: 
 *      Liscinsky Matus xlisci02
 *      Zubrik Tomas    xzubri00
 */
package application;
public class Multiplier extends ArithmBlock{
    
    /**
     * Constructor of Multiplier block, calls super constructor
     */
    public Multiplier() {
    	super("multiplier");
    }
    
    /**
     * Function multiplies 2 complex numbers from input ports and writes result to ouput port
     */
    public void Execute() {
    	Complex num1 = new Complex(this.inPorts.get(0).m.get("real"),this.inPorts.get(0).m.get("im"));
    	Complex num2 = new Complex(this.inPorts.get(1).m.get("real"),this.inPorts.get(1).m.get("im"));
    	
    	Complex result = num1.times(num2);
        this.outPorts.get(0).setComplex(result.re(), result.im());
    }
}
