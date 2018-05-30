/* 
 * Project:     Implementing Block Editor in JavaFX
 * Course:      IJA 2018
 * Team name:   Tym xzubri00
 * Members: 
 *      Liscinsky Matus xlisci02
 *      Zubrik Tomas    xzubri00
 */
package application;

public class Complex {
    private final double re;   // the real part
    private final double im;   // the imaginary part

    /**
     * Function creates a new object with the given real and imaginary parts
     * @param  real real part of complex number
     * @param  imag imaginary part of complex number
     */
    public Complex(double real, double imag) {
        re = real;
        im = imag;
    }

    /**
     * Function calculate magnitude of complex number
     * @return Magnitude of complex number
     */
    public double abs() {
        return Math.hypot(re, im);
    }

    /**
     * Function calculate angle normalized to be between -pi and pi
     * @return Angle of complex number
     */
    public double phase() {
        return Math.atan2(im, re);
    }


    /**
     * Function adds up two complex numbers
     * @param  b second addend
     * @return   New Complex object whose value is (this + b)
     */
    public Complex plus(Complex b) {
        Complex a = this;             
        double real = a.re + b.re;
        double imag = a.im + b.im;
        return new Complex(real, imag);
    }

    /**
     * Function subtracts two complex numbers 
     * @param  b second number
     * @return   New Complex object whose value is (this - b)
     */
    public Complex minus(Complex b) {
        Complex a = this;
        double real = a.re - b.re;
        double imag = a.im - b.im;
        return new Complex(real, imag);
    }

    /**
     * Function multiplies two complex numbers
     * @param  b second number
     * @return   New Complex object whose value is (this * b)
     */
    public Complex times(Complex b) {
        Complex a = this;
        double real = a.re * b.re - a.im * b.im;
        double imag = a.re * b.im + a.im * b.re;
        return new Complex(real, imag);
    }
    
    public double re() { return re; }
    public double im() { return im; }

    /**
     * Function divides two complex numbers
     * @param  b divisor
     * @return   New Complex object whose value is (this / b)
     */
    public Complex divides(Complex b) {
        Complex a = this;
        return a.times(b.reciprocal());
    }
    
    /**
     * Function calculates reciprocal of complex number
     * @return New complex number as recoprocal of complex number
     */
    public Complex reciprocal() {
        double scale = re*re + im*im;
        return new Complex(re / scale, -im / scale);
    }
    
    /**
     * Function transform complex number in polar form to cartesian form
     * @param  magnitude magnitude of complex number
     * @param  phase     phase/angle  of complex number
     * @return           complex number in cartesian form
     */
    public static Complex polarToCartesian(double magnitude, double phase){
    	double real = Math.cos(phase) * magnitude;
    	double imaginary = Math.sin(phase) * magnitude;
    	return new Complex(real,imaginary);
    }
}