/* 
 * Project:     Implementing Block Editor in JavaFX
 * Course:      IJA 2018
 * Team name:   Tym xzubri00
 * Members: 
 *      Liscinsky Matus xlisci02
 *      Zubrik Tomas    xzubri00
 */
package application;
public class Divider extends ArithmBlock{
    /**
     * Constructor of Divider block, calls super constructor
     */
    public Divider() {
    	super("divider");
    }
    
    /**
     * Function calculates division of two complex numbers, then writes result on output port
     */
    public void Execute() {
    	Complex num1 = new Complex(this.inPorts.get(0).m.get("real"),this.inPorts.get(0).m.get("im"));
    	Complex num2 = new Complex(this.inPorts.get(1).m.get("real"),this.inPorts.get(1).m.get("im"));
    	
    	Complex result = num1.divides(num2);
        this.outPorts.get(0).setComplex(result.re(), result.im());
    }
}
