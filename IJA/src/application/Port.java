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
import java.util.HashMap;
import java.util.Map;

public class Port {
	HashMap <String, Double> m;
	Vector <String> keys;
	Port connected_to;
	Block in_block ;

	/**
	 * Constructor of Port
	 */
	public Port() {
		this.connected_to=null;
		m = new HashMap <String,Double> ();
		keys = new Vector <String> ();
	}

	/**
	 * Function sets complex number on port
	 * @param real      real part
	 * @param imaginary imaginary part
	 */
	void setComplex(double real, double imaginary) {
		this.setReal(real);
		this.setImaginary(imaginary);
	}
	 
	/**
	 * Function adds type key in vector of keys 
	 * @param key Key to be added in vector
	 */
	void addTypeKey(String key) {
		this.keys.addElement(key);
	}
	 
	/**
	 * Function sets real part of complex number on port
	 * @param real Real part of complex number
	 */
	void setReal(double real) {
		this.m.put("real", real);
	}
	 
	/**
	 * Function sets imaginary part of complex number on port
	 * @param imaginary Imaginary part of complex number
	 */
	void setImaginary(double imaginary) {
		this.m.put("im", imaginary);
	}
	 
	/**
	 * Function sets complex number in polar form on port
	 * @param magnitude   magnitude of complex number
	 * @param phase_angle phase angle of complex number
	 */
	void setPolar(double magnitude, double phase_angle) {
		this.m.put("magnitude", magnitude);
		this.m.put("angle", phase_angle);
	}
	 
	/**
	 * Function gets information about map of port
	 * @return Formatted String with information about map
	 */
	String mapToString(){
		String result = new String();
		    if (this.m.isEmpty()){
		        result = result.concat("empty");
		        return result;
		    }
		    for (Map.Entry<String, Double> entry : this.m.entrySet()) {
		    	String key = entry.getKey();
		        Double value = entry.getValue();
		        
		        result = result.concat("  ");
		        result = result.concat(key);
		        result = result.concat(": ");
		        result = result.concat(value.toString());
		        result = result.concat("\n");
		    }
		    return result.substring(0, result.length()-1);
	}
}
