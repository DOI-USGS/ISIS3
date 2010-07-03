#ifndef NumericalApproximation_h
#define NumericalApproximation_h

#include <string>
#include <vector>
#include <map>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include "iException.h"

using namespace std;

namespace Isis {
  /**                                          
   * @brief NumericalApproximation provides various numerical 
   *        analysis methods of interpolation, extrapolation and
   *        approximation of a tabulated set of @a x, @a y data.
   *  
   * This class contains a merged version of the Isis3 classes 
   * @b DataInterp and @b NumericalMethods.  In addition, 
   * some methods from @b AtmosModel were moved to this class, the
   * @a CubicNeighborhood interpolation type was 
   * adapted from the IDL routine called @b interpol (using the 
   * "/spline" keyword), and several differentiation and 
   * integration approximation methods were created. 
   *  
   * NumericalApproximation contains 9 types of data interpolation 
   * routines.  These routines act on x, y pairs of data points. 
   * The following forms of data interpolation are supported: 
   * 
   * <table>
   *   <tr>
   *     <th>Name</th>
   *     <th>Type (enum)</th>
   *     <th>MinPoints</th>
   *     <th>Description</th>
   *   </tr>
   *   <tr>
   *     <td>Linear</td>
   *     <td>@a Linear</td>
   *     <td>2</td>
   *     <td>
   *        Linear interpolation approximates a curve by
   *        concatinating line segments between known data        
   *        points.  This results in a continuous curve with a
   *        discontinuities of the derivative at the known data
   *        points.  This interpolation type uses the GSL
   *        library routines.
   *        
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Polynomial</td>
   *     <td>@a Polynomial</td>
   *     <td>3</td>
   *     <td>
   *        Computes a polynomial interpolation.  This method
   *        should only be used for interpolating small numbers
   *        of points because polynomial interpolation introduces
   *        large oscillations, even for well-behaved datasets.
   *        The number of terms in the interpolating polynomial
   *        is equal to the number of points.  This interpolation
   *        type uses the GSL library routines.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Neville's Algorithm for Polynomial Interpolation</td>
   *     <td>@a PolynomialNeville</td>
   *     <td>3</td>
   *     <td>
   *     Polynomial interpolation using Neville's algorithm. This
   *     first fits a polynomial of degree 0 through each point.
   *     On the second iteration, the polynomial of adjoining
   *     indices are combined to fit through pairs of points.
   *     This process repeats until a pyramid of approximations is
   *     reached. This is based on the Newton form of the
   *     interpolating polynomial and the recursion relation of
   *     the divided differences.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Natural Cubic Spline</td>
   *     <td>@a CubicNatural</td>
   *     <td>3</td>
   *     <td>
   *       Cubic spline with natural boundary conditions.  The
   *       resulting curve is piecewise cubic on each interval
   *       with matching first and second derivatives at the
   *       supplied data points.  The second derivative is chosen
   *       to be zero at the endpoints, i.e. the first and last
   *       points of the data set. This is also refered to as
   *       free boundary condtions.  This interpolation type uses
   *       the GSL
   *        library routines.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Clamped Cubic Spline</td>
   *     <td>@a CubicClamped</td>
   *     <td>3</td>
   *     <td>
   *     Cubic Spline interpolation with clamped boundary
   *     conditions.  The resulting curve is piecewise cubic on
   *     each interval.  For this type of boundary condition to
   *     hold, it is necessary to have either the values of the
   *     derivative at the endpoints or an accurate approximation
   *     to those values. In general, clamped boundary conditions
   *     lead to more accurate approximations since they include
   *     more information about the function.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Periodic Natural Cubic Spline</td>
   *     <td>@a CubicNatPeriodic</td>
   *     <td>2</td>
   *     <td>
   *       Cubic spline with periodic boundary conditions.  The
   *       resulting curve is piecewise cubic on each interval
   *       with matching first and second derivatives at the
   *       supplied data points.  The derivatives at the first
   *       and last points are also matched.  Note that the last
   *       point in the data must have the same y-value as the
   *       first point, otherwise the resulting periodic
   *       interpolation will have a discontinuity at the
   *       boundary.  This interpolation type uses the GSL
   *       library routines.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Cubic Spline about 4-point Neighborhood</td>
   *     <td>@a CubicNeighborhood</td>
   *     <td>4</td>
   *     <td>
   *       Cubic Spline interpolation using 4-pt Neighborhoods
   *       with natural boundary conditions. This interpolation
   *       type is adapted from the IDL interpol.pro application
   *       using the "/spline" keyword on an irregular grid.
   *       This type of cubic spline fits a natural cubic spline
   *       to the 4-point neighborhood of known data points
   *       surrounding the @a x value at which we wish to
   *       evaluate. For example, suppose {@a x<sub>0</sub>,
   *       @a x<sub>1</sub>, ..., @a x<sub><I>n</I></sub>} is the
   *       array of known domain values in the data set and @f$
   *       x_i \leq u < x_{i+1} @f$ for some @e i such that
   *       @f$ 0 \leq i \leq n @f$, then to evaluate the
   *       @a y value at @e u, this method of interpolation
   *       evaluates the ordinary cubic spline consisting of the
   *       data set {@a x<sub>i-1</sub>, @a x<sub>i</sub>,
   *       @a x<sub>i+1</sub>, @a x<sub>i+2</sub>} at
   *       @e u.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Cubic Spline using Hermite cubic polynomial</td>
   *     <td>@a CubicHermite</td>
   *     <td>2</td>
   *     <td>
   *       Cubic Spline interpolation using the Hermite cubic
   *       polynomial (also called the unique Hermite
   *       interpolating fundamental polynomial of degree 3).
   *       This method requires the input of the slopes of the
   *       tangent lines of the curve (i.e. velocities, or first
   *       derivatives) at each of the data points.  This ensures
   *       that the approximation has the same "shape" as the
   *       curve at the known data points. This interpolation type
   *       uses piecewise Hermite cubic polynomials for each
   *       interval, (@a x<sub>0</sub>, @a x<sub>1</sub>), using
   *       the known data points, (@a
   *       x<sub>0</sub>,@a f(@a x<sub>0</sub>)) and
   *       (@a x<sub>1</sub>,@a f(@a x<sub>1</sub>)), and their
   *       derivatives,
   *       @a f '(@a x<sub>0</sub>) and @a f '(@a
   *       x<sub>1</sub>). The Hermite cubic polynomial is defined
   *       @f[ H_3(x) = 
   *       f(x_0)h_0(x)+f(x_1)h_1(x)
   *       +f\prime(x_0)\hat{h}_0(x)+f\prime(x_1)\hat{h}_1(x)
   *       @f]
   *       where
   *       @f$ h_k(x) = 
   *       [1-2(x-x_k)L\prime_k(x_k)](L_k(x))^2
   *       @f$
   *       and
   *       @f$ \hat{h}_k(x) = 
   *       (x-x_k)(L_k(x))^2
   *       @f$
   *       for the kth-Lagrange coefficient polynomials of degree
   *       n = 1,
   *       @f$ L_0(x) = \frac{x- x_1}{x_0-x_1}@f$ and 
   *       @f$ L_1(x) = \frac{x- x_0}{x_1-x_0}@f$.
   *       
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Akima</td>
   *     <td>@a Akima</td>
   *     <td>5</td>
   *     <td>
   *       Non-rounded Akima spline with natural boundary
   *       conditions.  This method uses non-rounded corner
   *       algorithm of Wodicka. This interpolation type uses the
   *       GSL library routines.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Periodic Akima</td>
   *     <td>@a AkimaPeriodic</td>
   *     <td>5</td>
   *     <td>
   *       Non-rounded Akima spline with periodic boundary
   *       conditions.  This method uses non-rounded corner
   *       algorithm of Wodicka.  This interpolation type uses
   *       the GSL library routines.
   *     </td>
   *   </tr>
   * </table>
   *  
   *  
   * Numerical analysis approximation methods for differentiating 
   * and integrating unknown functions represented by a data set 
   * are implemented by using the interpolations on the data set 
   * created with a NumericalApproximation object. The Lagrange polynomial,
   *       @f[ L(x) = 
   *       \sum_{k=0}^{n} \left(f(x_k) \prod_{i=0,i \neq k}^{n}
   *            \frac{x- x_i}{x_k-x_i}\right)
   *       @f]
   * is used to determine formulas for numerical differentiation 
   * and integration.
   * @n
   * @b Numerical Differentiation 
   * @n 
   * The differentiation methods use difference formulas to
   * approximate first and second derivatives at a domain value 
   * for a given a set of known (@a x,@a y) data points. Once all 
   * of the data points are added, a Isis::NumericalApproximation 
   * interpolation is used on the dataset to estimate the 
   * function, @f$f:x \to y@f$, mapping @a x values to 
   * corresponding @a y values. Then, the derivative of 
   * @e f evaluated at @a x<sub>0</sub> is approximated. 
   * To do so, a uniform step size of @a h is chosen to 
   * determine the distance between @a x<sub>i</sub> and @a 
   * x<sub>i+1</sub>. 
   * <table>
   *   <caption> Differentiation methods require the parameters
   *     @a a (domain value at which the derivative will be
   *     evaluated), @a n (number of points used in the difference
   *     formula), and @a h (step size of points). Note: @a a
   *     is denoted @a x<sub>0</sub> in the above formulas.
   *   </caption>
   *   <tr>
   *     <th>Numerical Differentiation Type</th>
   *     <th>Difference Formulas</th>
   *     <th>Methods Available</th>
   *     <th>Description</th>
   *   </tr>
   *   <tr>
   *     <td>Backward Difference </td>
   *     <td>
   *       <UL> 
   *         <LI> 2-point backward difference.
   *           @f[
   *                f\prime(x_0) \approx \frac{1}{h}[f(x_0) - f(x_0 - h)]
   *           @f]
   *         <LI> 3-point backward difference.
   *           @f[
   *                f\prime(x_0) \approx \frac{1}{2h}[3f(x_0) - 4f(x_0 - h) +
   *                f(x_0 - 2h)]
   *           @f]
   *         <LI> 3-point backward second difference.
   *           @f[
   *                f\prime\prime(x_0) \approx \frac{1}{h^2}[f(x_0)
   *                - 2f(x_0 - h) + f(x_0 - 2h)]
   *           @f]
   *       </UL>
   *     </td>
   *     <td>
   *       <UL>
   *         <LI>BackwardFirstDifference()
   *         <LI>BackwardSecondDifference()
   *       </UL>
   *     </td>
   *     <td>
   *       Backward difference formulas use a uniform step-size
   *       moving in the negative direction from the
   *       given @a x<sub>0</sub>. These formulas are derived
   *       by differentiating the Lagrange polynomials for
   *       @a x<sub>i</sub> = @a x<sub>0</sub> - @a ih
   *       and evaluating them at@a x<sub>0</sub>.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Forward Difference </td>
   *     <td>
   *       <UL> 
   *         <LI> 2-point forward difference.
   *           @f[
   *                f\prime(x_0) \approx \frac{1}{h}[f(x_0 + h) - f(x_0)]
   *           @f]
   *         <LI> 3-point forward difference.
   *           @f[
   *                f\prime(x_0) \approx \frac{1}{2h}[-f(x_0 + 2h) +
   *                4f(x_0 + h) - 3f(x_0)]
   *           @f]
   *         <LI> 3-point forward second difference.
   *           @f[
   *                f\prime\prime(x_0) \approx \frac{1}{h^2}[f(x_0 +
   *                2h) - 2f(x_0 + h) + f(x_0)]
   *           @f]
   *       </UL>
   *     </td>
   *     <td>
   *       <UL>
   *         <LI>ForwardFirstDifference()
   *         <LI>ForwardSecondDifference()
   *       </UL>
   *     </td>
   *     <td>
   *       Forward difference formulas use a uniform step-size
   *       moving in the positive direction from
   *       @a x<sub>0</sub>. These formulas are derived by
   *       differentiating the Lagrange polynomials for
   *       @a x<sub>i</sub> = @a x<sub>0</sub> + @a ih
   *       and evaluating them at@a x<sub>0</sub>.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Center Difference </td>
   *     <td>
   *       <UL> 
   *         <LI> 3-point center difference.
   *           @f[
   *                f\prime(x_0) \approx \frac{1}{2h}[f(x_0 + h) -
   *                f(x_0 - h)]
   *           @f]
   *         <LI> 5-point center difference.
   *           @f[
   *                f\prime(x_0) \approx \frac{1}{12h}[-f(x_0 + 2h) +
   *                8f(x_0 +h) - 8f(x_0 - h) + f(x_0 - 2h)]
   *           @f]
   *         <LI> 3-point center second difference.
   *           @f[
   *                f\prime\prime(x_0) \approx \frac{1}{h^2}[f(x_0 + h)
   *                - 2f(x_0) + f(x_0 - h)]
   *           @f]
   *         <LI> 5-point center second difference.
   *           @f[
   *                f\prime\prime(x_0) \approx \frac{1}{12h^2}[-f(x_0 +
   *                2h) + 16f(x_0 +h) - 30f(x_0) + 16f(x_0 - h) - f(x_0
   *                - 2h)]
   *           @f]
   *       </UL>
   *     </td>
   *     <td>
   *       <UL>
   *         <LI>CenterFirstDifference()
   *         <LI>CenterSecondDifference()
   *       </UL>
   *     </td>
   *     <td>
   *       Center difference formulas use a uniform step-size
   *       moving in both directions from @a x<sub>0</sub> so this
   *       point stays centered. These formulas are derived by
   *       differentiating the Lagrange polynomials for
   *       @a x<sub>i</sub> = @a x<sub>0</sub>
   *       + (@e i - (@a n - 1)/2)@a h and evaluating them
   *       at @a x<sub>0</sub>.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>GSL Differentiation </td>
   *     <td> Unknown </td>
   *     <td>
   *       <UL>
   *         <LI>GslFirstDerivative()
   *         <LI>GslSecondDerivative()
   *       </UL>
   *     </td>
   *     <td>
   *       No documentation was found about the algorithms used
   *       for these methods.  They may only be called when
   *       interpolation type is one of the following: @a Linear,
   *       @a Polynomial, @a CubicNatural, @a CubicNatPeriodic, @a Akima, or
   *       @a AkimaPeriodic.
   *     </td>
   *   </tr>
   * </table> 
   * @n  
   * @b Numerical Integration
   * @n 
   * The integration methods were derived using 
   * @b Newton-Cotes, or @a quadrature, formulas for 
   * approximating integrals given a set of known (@a x,@a y) 
   * data points.  The @a x values may be irregularly spaced, but 
   * must be unique and added to the dataset in ascending order. 
   * Once all of the data points are added, a 
   * Isis::NumericalApproximation interpolation is used on 
   * the dataset to estimate the function, @f$f:x \to y@f$, 
   * mapping @a x values to corresponding @a y values. 
   * Then, the integral of @e f on the interval from @a a 
   * to @a b is approximated. To do so, an algorithm for creating 
   * a composite formula by applying the original formula to 
   * segments that share a boundary point is used. The 
   * NumericalApproximation::InterpType chosen for 
   * interpolation computation seems to have little affect on the 
   * error between the actual integral and the return values of 
   * the integration methods. The BoolesRule() method varies the 
   * most in error. Although errors are not high for any of the 
   * interpolation types, 
   * @a CubicNatural spline interpolations seem to 
   * have the smallest error most often with BoolesRule(). For any 
   * other numerical integration method, the errors appear to be 
   * identical for any NumericalApproximation::InterpType except 
   * @a Polynomial, which usually has a slightly larger 
   * error than the other interpolation types. Note: A portion of 
   * this algorithm is derived from the IDL function int_tabulated 
   * for Boole's Method. 
   * <table>
   *   <caption> Integration methods require the parameters @a a,
   *     @a b (interval of domain over which to integrate).
   *   </caption>
   *   <tr>
   *     <th>Numerical Integration Type</th>
   *     <th>Integration Formulas</th>
   *     <th>Methods Available</th>
   *     <th>Description</th>
   *   </tr>
   *   <tr>
   *     <td>Trapezoidal Rule</td>
   *     <td>
   *       <UL> 
   *         <LI> 2-point Newton-Cotes trapezoidal rule:
   *            @f[
   *           \int_{a}^b f(x)dx \approx \frac{h}{2}[f(a) + f(b)]
   *            @f]
   *         where @e h = @a b - @a a.
   *       </UL>
   *     </td>
   *     <td>
   *       <UL>
   *         <LI>TrapezoidalRule()
   *       </UL>
   *     </td>
   *     <td>
   *       The 2-point closed rule, known as the trapezoidal
   *       rule, uses straight line segments between known data
   *       points to estimate the area under the curve (integral).
   *       This Newton-Cotes formula uses a uniform step-size of
   *       @e h = @a b - @a a and is derived by integrating the
   *       Lagrange polynomials over the closed interval [@a a,
   *       @a a+h] for @a x<sub>i</sub> =
   *       @a x<sub>0</sub> + @e ih.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Simpson's Rule, or Simpson's 3-Point Rule </td>
   *     <td>
   *       <UL> 
   *         <LI> 3-point Newton-Cotes, Simpson's Rule
   *           @f[
   *             \int_{a}^b f(x)dx \approx \frac{h}{3}[f(a) +
   *               4f(a+h) + f(a+2h)]
   *           @f]
   *           where @e h = (@a b - @a a)/2.
   *       </UL>
   *     </td>
   *     <td>
   *       <UL>
   *         <LI>Simpsons3PointRule()
   *       </UL>
   *     </td>
   *     <td>
   *       The 3-point closed rule, known as Simpson's Rule or
   *       Simpson's 3-Point Rule, uses parabolic arcs between
   *       known data points to estimate the area under the curve
   *       (integral). This Newton-Cotes formula uses a uniform
   *       step-size of @e h = (@a b - @a a)/2 and is derived by
   *       integrating the Lagrange polynomials over the closed
   *       interval [@a a, @a a+2h] for @a x<sub>i</sub> =
   *       @a x<sub>0</sub> + @e ih.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Simpson's 3/8 Rule, or Simpson's 4-Point Rule </td>
   *     <td>
   *       <UL> 
   *         <LI> 4-point Newton-Cotes, Simpson's 3/8 Rule
   *           @f[
   *             \int_{a}^b f(x)dx \approx \frac{3h}{8}[f(a) +
   *                 3f(a+h) + 3f(a+2h) + f(a+3h)]
   *           @f]
   *           where @e h = (@a b - @a a)/3.
   *       </UL>
   *     </td>
   *     <td>
   *       <UL>
   *         <LI>Simpsons4PointRule()
   *       </UL>
   *     </td>
   *     <td>
   *       The 4-point closed rule, known as Simpson's 3/8 Rule or
   *       Simpson's 4-Point Rule, uses cubic curves between 
   *       known data points to estimate the area under the curve
   *       (integral). This Newton-Cotes formula uses a uniform
   *       step-size of @e h = (@a b - @a a)/3 and is derived by
   *       integrating the Lagrange polynomials over the closed
   *       interval [@a a, @a a+3h] for @a x<sub>i</sub> =
   *       @a x<sub>0</sub> + @e ih.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Boole's Rule </td>
   *     <td>
   *       <UL> 
   *         <LI> 5-point Newton-Cotes, Boole's Rule
   *           @f[
   *             \int_{a}^b f(x)dx \approx \frac{2h}{45}[7f(a) +
   *                 32f(a+h) + 12f(a+2h) + 32f(a+3h) + 7f(a+4h)]
   *           @f]
   *           where @e h = (@a b - @a a)/4. 
   *       </UL>
   *     </td>
   *     <td>
   *       <UL>
   *         <LI>BoolesRule()
   *       </UL>
   *     </td>
   *     <td>
   *       The 5-point closed rule, known as Boole's Rule, uses
   *       quartic curves between known data points to estimate
   *       the area under the curve (integral). This Newton-Cotes
   *       formula uses a uniform step-size of @e h = (@a b -
   *       @a a)/4 and is derived by integrating the Lagrange
   *       polynomials over the closed interval [@a a, @a a+4h]
   *       for @a x<sub>i</sub> = @a x<sub>0</sub> +
   *       @a ih.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Refinements of Extended Trapezoidal Rule </td>
   *     <td>
   *       <UL> 
   *         <LI> Extended closed trapezoidal rule
   *           @f[
   *             \int_{a}^b f(x)dx \approx h[\frac12 f(x_0) +
   *                 f(x_1) + ... + f(x_{n-2} + \frac12
   *                 f(x_{n-1})]
   *           @f]
   *           where @e h = (@a b - @a a)/4, @a x<sub>0</sub> =
   *           @a a, and @a x<sub>n-1</sub> = @a b.
   *       </UL>
   *     </td>
   *     <td>
   *       <UL>
   *         <LI>RefineExtendedTrap()
   *       </UL>
   *     </td>
   *     <td>
   *       The extended (or composite) trapezoidal rule can be
   *       used to with a series of refinements to approximate the
   *       integral.  The first stage of refinement returns the
   *       ordinary trapezoidal estimate of the integral.
   *       Subsequent stages will improve the accuracy, where, for
   *       the @a n<sup>th</sup> stage 2<sup>@a n-2</sup> interior
   *       points are added.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>Romberg's Method </td>
   *     <td>
   *       <UL>
   *         <LI> Romberg's Method for Integration </td>
   *       </UL>
   *     <td>
   *       <UL>
   *         <LI>RombergsMethod()
   *       </UL>
   *     </td>
   *     <td>
   *       Romberg's Method is a potent numerical integration
   *       tool. It uses the extended trapezoidal rule and
   *       Neville's algorithm for polynomial interpolation.
   *     </td>
   *   </tr>
   *   <tr>
   *     <td>GSL Integration </td>
   *     <td> Unknown </td>
   *     <td>
   *       <UL>
   *         <LI>GslIntegral()
   *       </UL>
   *     </td>
   *     <td>
   *       No documentation was found about the algorithm used for
   *       this methods.  They may only be called when
   *       interpolation type is one of the following: @a Linear,
   *       @a Polynomial, @a CubicNatural, @a CubicNatPeriodic, @a Akima, or
   *       @a AkimaPeriodic.
   *     </td>
   *   </tr>
   * </table> 
   *  
   *  
   * Below is an example demonstating the use of this class:
   * @code
   *    inline double f(double x) { return (x + cos ((x * x))); }
   * 
   *    NumericalApproximation
   *    spline(NumericalApproximation::CubicClamped);
   *    for (int x = 0; x < 200 ; x++) {
   *      spline.addPoint(x, f(x));
   *    }
   *    spline.SetCubicClampedEndptDeriv(0,0);
   *    double yinterp = spline.Evaluate(50.7);
   *    double yextrap =
   *        spline.Evaluate(201,NumericalApproximation::Extrapolate);
   *    double deriv = spline.CenterFirstDifference(10);
   *    double integ =
   *        spline.RombergsMethod(spline.DomainMinimum(),spline.DomainMaximum());
   * @endcode
   * 
   * To compute the same data set using the Akima spline, just use the following:
   * @code 
   *    spline.Reset(NumericalApproximation::Akima); 
   *    double yinterp = spline.Evaluate(50.7);
   *    double yextrap =
   *        spline.Evaluate(201,NumericalApproximation::NearestEndpoint);
   *    double deriv = spline.CenterFirstDifference(10);
   *    double integ =
   *        spline.RombergsMethod(spline.DomainMinimum(),spline.DomainMaximum()); 
   * @endcode
   * 
   * <h1>Caveats</h1>
   * When using this class, there are some important details that 
   * require consideration.  Several interpolation types of this 
   * class use the GSL library. In addition, the GSL library 
   * default error handling scheme has implications when used in 
   * C++ objects.  The default behavior is to terminate the 
   * application when an error occurs.  Options for developers are 
   * to turn off error trapping within the GSL, which means users 
   * of the library must do their own error checking, or a 
   * specific handler can be specified to replace the default 
   * mode.  Neither option is well suited in object-oriented 
   * implementations because they apply globally, to all users of 
   * the library. The approach used here is to turn off error 
   * handling and require users to handle their own error 
   * checking.  This is not entirely safe as any user that does 
   * not follow this strategy could change this behavior at 
   * runtime.  Other options should be explored. An additional 
   * option is that the default behavior can be altered prior to 
   * building GSL but this is also problematic since this behavior 
   * cannot be guaranteed universally. 
   *  
   * For these types, interpolation is strictly adhered to and 
   * extrapolation is not handled well. If the input to be 
   * evaluated is outside of the 
   * minimum and maximum @a x values of the original data 
   * points, then the @a y value corresponding to the 
   * @a x value that is nearest to that input is returned. 
   * However, for sufficiently close values, the clamped cubic and 
   * polynomial Neville's types are fairly accurate in their 
   * extrapolation techniques.  All differentiation and 
   * integration methods throw an error if the value passed is 
   * outside of the domain. 
   * 
   * @ingroup Math                      
   * @author 2008-11-05 Janet Barrett, Kris Becker, K Teal 
   *         Thompson, Jeannie Walldren
   * @internal 
   *   @history 1999-08-11 K Teal Thompson - Original version of 
   *            NumericalMethods subroutines in Isis2.
   *   @history 2006-06-14 Kris Becker - Created DataInterp class
   *   @history 2007-02-20 Janet Barrett - Created NumericalMethods 
   *            class from Isis2 subroutines
   *   @history 2007-02-20 Janet Barrett - Created AtmosModel class 
   *            from Isis2 subroutines
   *   @history 2008-06-18 Christopher Austin - Fixed documentation 
   *            for DataInterp
   *   @history 2008-06-18 Steven Koechle - Updated NumericalMethods 
   *            unitTest.
   *   @history 2008-11-05 Jeannie Walldren - Merged DataInterp 
   *            class, NumericalMethods class, and methods from
   *            AtmosModel.  Modified methods from various classes
   *            to be able to function similarly. Added InterpType
   *            @a CubicNeighborhood, difference formula methods, and
   *            integration methods. Created new unitTest.
   *   @history 2008-11-12 Jeannie Walldren - Fixed documentation.
   *   @history 2008-12-18 Jeannie Walldren - Added address
   *            operator (&) to input variables of type vector,
   *            NumericalApproximation::InterpType, and
   *            NumericalApproximation::ExtrapType in
   *            Constructors, AddData(), Evaluate(),
   *            ValueToExtrapolate(), EvaluateCubicNeighborhood(),
   *   @history 2009-01-26 Jeannie Walldren - Fixed error
   *            documentation.
   *   @history 2009-02-12 Jeannie Walldren - Fixed documentation
   *            to include weblinks.
   *   @history 2009-07-17 Steven Lambright - Added algorithm include,
   *            removed macro MAX
   *   @history 2009-06-10 Jeannie Walldren - Added interpolation
   *            type CubicHermite.  This involved modifying some
   *            methods, as well as adding variable p_fprimeOfx
   *            and methods AddCubicHermiteDeriv() and
   *            EvaluateCubicHermite().  Added Contains() method.
   *   @history 2009-07-02 Jeannie Walldren - Replaced Hermite
   *            interpolation algorithm with simpler version and
   *            added methods EvaluateCubicHermiteFirstDeriv and
   *            EvaluateCubicHermiteSecDeriv
   *   @history 2009-07-09 Debbie Cook - Finished Jeannie's
   *            modifications.
   *   @history 2009-08-03 Jeannie Walldren - Clean up code,
   *            documentation and check in changes from
   *            2009-06-10, 2009-07-02, 2009-07-09
   *  
   */                                                                       

  class NumericalApproximation {
    public:
      /**
       * This enum defines the types of interpolation supported in this class
       */
      enum InterpType { Linear,               //!< Linear interpolation.
                        Polynomial,           //!< Polynomial interpolation.
                        PolynomialNeville,    //!< Polynomial interpolation using Neville's algorithm.
                        CubicNatural,         //!< Cubic Spline interpolation with natural boundary conditions.  
                        CubicClamped,         //!< Cubic Spline interpolation with clamped boundary conditions.  
                        CubicNatPeriodic,     //!< Cubic Spline interpolation with periodic boundary conditions.
                        CubicNeighborhood,    //!< Cubic Spline interpolation using 4-pt Neighborhoods with natural boundary conditions.
                        CubicHermite,         //!< Cubic Spline interpolation using the Hermite cubic polynomial.
                        Akima,                //!< Non-rounded Akima Spline interpolation with natural boundary conditions.  
                        AkimaPeriodic         //!< Non-rounded Akima Spline interpolation with periodic boundary conditions.  
      };
      
      // CONSTRUCTORS
      NumericalApproximation(const InterpType &itype=CubicNatural) throw (iException &);
      NumericalApproximation(unsigned int n, double *x, double *y, 
                             const InterpType &itype=CubicNatural) throw (iException &);
      NumericalApproximation(const vector <double> &x, const vector <double> &y, 
                             const InterpType &itype=CubicNatural)throw (iException &);
      NumericalApproximation(const NumericalApproximation &dint);
      // ASSIGNMENT OPERATOR 
      NumericalApproximation &operator=(const NumericalApproximation &numApMeth);
      // DESTRUCTOR
      virtual ~NumericalApproximation ();


      // ACCESSOR METHODS FOR OBJECT PROPERTIES
      string Name() const;
     /**
      * @brief Returns the enumerated type of interpolation chosen
      * 
      * This method can be selected after all the points are added in the
      * interpolation by using Compute() method.  
      * Note that this prints out as an integer representaion of the 
      * enumerated type: 
      *   <UL>
      *     <LI> Linear             = 0
      *     <LI> Polynomial         = 1
      *     <LI> PolynomialNeville  = 2
      *     <LI> CubicNatural       = 3
      *     <LI> CubicClamped       = 4
      *     <LI> CubicNatPeriodic   = 5
      *     <LI> CubicNeighborhood  = 6
      *     <LI> CubicHermite       = 7
      *     <LI> Akima              = 8
      *     <LI> AkimaPeriodic      = 9
      *    </UL>
      * 
      * @return NumericalApproximation::InterpType Currently assigned
      *         interpolation type
      */
      inline InterpType   InterpolationType() { return (p_itype); }
      int MinPoints();
      int MinPoints(InterpType itype) throw (iException &);

      // ADD DATA TO OBJECT
      void AddData(const double x, const double y);
      void AddData(unsigned int n, double *x, double *y);
      void AddData(const vector <double> &x, const vector <double> &y) throw (iException &);
      void SetCubicClampedEndptDeriv(const double yp1, const double ypn) throw (iException &);
      void AddCubicHermiteDeriv(unsigned int n, double *fprimeOfx) throw (iException &);
      void AddCubicHermiteDeriv(const vector <double> &fprimeOfx) throw (iException &);
      void AddCubicHermiteDeriv(const double fprimeOfx) throw (iException &);

      //ACCESSOR METHODS AFTER DATA IS ENTERED
      double DomainMinimum();
      double DomainMaximum();
      bool Contains(double x);
     /**
      * Returns the number of the coordinates added to the data set.
      * 
      * @return @b unsigned @b int Size of data set.
      */
      inline unsigned int Size() { return(p_x.size()); }
      
       /**
       * This enum defines the manner in which a value outside of the 
       * domain should be handled if passed to the Evaluate() method. 
       */
      enum ExtrapType { ThrowError,     //!< Evaluate() throws an error if @a a is outside of the domain.
                        Extrapolate,    //!< Evaluate() attempts to extrapolate if @a a is outside of the domain.
                                        //!< This is only valid for NumericalApproximation::InterpType @a CubicClamped or @a PolynomialNeville 
                                        //!< and the result will be accurate only if sufficiently close to the domain boundary.
                        NearestEndpoint //!< Evaluate() returns the y-value of the nearest endpoint if @a a is outside of the domain.
      };
      // INTERPOLATION AND EXTRAPOLATION RESULTS
      double          Evaluate (const double          a, const ExtrapType &etype=ThrowError) throw (iException &);
      vector <double> Evaluate (const vector <double> &a, const ExtrapType &etype=ThrowError) throw (iException &);
      vector <double> PolynomialNevilleErrorEstimate() throw (iException &);
      vector <double> CubicClampedSecondDerivatives() throw (iException &);
      double EvaluateCubicHermiteFirstDeriv(const double a);
      double EvaluateCubicHermiteSecDeriv(const double a);

      // DIFFERENTIATION METHODS
      double GslFirstDerivative(const double a) throw (iException &);
      double BackwardFirstDifference(const double a, const unsigned int n=3, 
                                     const double h=0.1) throw (iException &);
      double ForwardFirstDifference(const double a, const unsigned int n=3, 
                                    const double h=0.1) throw (iException &);
      double CenterFirstDifference(const double a, const unsigned int n=5, 
                                   const double h=0.1) throw (iException &);

      double GslSecondDerivative(const double a) throw (iException &);
      double BackwardSecondDifference(const double a, const unsigned int n=3, 
                                      const double h=0.1) throw (iException &);
      double ForwardSecondDifference(const double a, const unsigned int n=3, 
                                     const double h=0.1) throw (iException &);
      double CenterSecondDifference(const double a, const unsigned int n=5, 
                                    const double h=0.1) throw (iException &);

      // INTERGRATION METHODS
      double GslIntegral(const double a, const double b) throw (iException &);
      double TrapezoidalRule(const double a, const double b);
      double Simpsons3PointRule(const double a, const double b);
      double Simpsons4PointRule(const double a, const double b);
      double BoolesRule(const double a, const double b);
      double RefineExtendedTrap (double a, double b, double s, unsigned int n) throw (iException &);
      double RombergsMethod (double a, double b) throw (iException &);

      // ASSIGNMENT OPERATORS
      void Reset();
      void Reset(InterpType itype) throw (iException &);
      void SetInterpType(InterpType itype) throw (iException &);

    protected:
      // == CLASS VARIABLES
      // VARIABLES FOR ALL INTERP TYPES
      InterpType        p_itype;                            //!< Interpolation type
      vector<double>    p_x;                                //!< List of X values                                                                           
      vector<double>    p_y;                                //!< List of Y values                                                     
      bool              p_dataValidated;                    //!< Flag variable to determine whether ValidateDataSet() has been called
      // GSL INTERP VARIABLES
      typedef const gsl_interp_type * InterpFunctor;        //!< GSL Interpolation specs                                                 
      typedef map<InterpType, InterpFunctor> FunctorList;   //!< Set up a std::map of GSL interpolator functors.  List of function types
      typedef FunctorList::const_iterator FunctorConstIter; //!< GSL Iterator
      gsl_interp_accel *p_acc;                              //!< Lookup accelorator                                                 
      gsl_spline       *p_interp;                           //!< Currently active interpolator                                      
      FunctorList       p_interpFunctors;                   //!< Maintains list of interpolator options                                                     
      // CUBIC CLAMPED VARIABLES
      bool              p_clampedEndptsSet;                 //!< Flag variable to determine whether SetCubicClampedEndptDeriv() has been called after all data was added for @a CubicClamped interpolation.
      bool              p_clampedComputed;                  //!< Flag variable to determine whether ComputeCubicClamped() has been called.
      double            p_clampedDerivFirstPt;              //!< First derivative of first x-value, p_x[0].  This is only used for the @a CubicClamped interpolation type.
      double            p_clampedDerivLastPt;               //!< First derivative of last x-value, p_x[n-1].  This is only used for the @a CubicClamped interpolation type.
      vector<double>    p_clampedSecondDerivs;              //!< List of second derivatives evaluated at p_x values.  This is only used for the @a CubicClamped interpolation type.
      // POLYNOMIAL NEVILLE VARIABLES
      vector <double>   p_polyNevError;                     //!< Estimate of error for interpolation evaluated at x.  This is only used for the @a PolynomialNeville interpolation type. 91 taken from AtmosModel.
      // CUBIC HERMITE VARIABLES
      vector<double>    p_fprimeOfx;                        //!< List of first derivatives corresponding to each x value in the data set (i.e. each value in p_x)


      // == PROTECTED METHODS
      // CREATING, DESTROYING OBJECT
      void Init(InterpType itype);
      bool GslInterpType(InterpType itype) const; 
      void GslAllocation(unsigned int npoints);
      void GslDeallocation();
      InterpFunctor GslFunctor(InterpType itype) const throw (iException &);
      // VERIFICATION METHODS 
      void GslIntegrityCheck(int gsl_status, char *src, 
                             int line) throw (iException &);
      void ValidateDataSet() throw (iException &);
      bool InsideDomain(const double a);
      // COMPUTATION AND EVALUATION METHODS
      bool   GslComputed() const;
      void   ComputeGsl() throw (iException &);
      void   ComputeCubicClamped() throw (iException &);
      double ValueToExtrapolate(const double a, const ExtrapType &etype);
      double          EvaluateCubicNeighborhood(const double a);
      vector <double> EvaluateCubicNeighborhood(const vector <double> &a, const ExtrapType &etype);
      double          EvaluateCubicClamped(const double a) throw (iException &);
      double          EvaluateCubicHermite(const double a) throw (iException &);
      double          EvaluatePolynomialNeville(const double a) throw (iException &);
      vector <double> EvaluateForIntegration(const double a, const double b, 
                                                const unsigned int n) throw (iException &);
      int FindIntervalLowerIndex(const double a);
      // STANDARDIZE ERRORS
      void ReportException(iException::errType type, const string &method, 
                       const string &message, char *filesrc, 
                       int lineno) const throw (iException &);
  };
};

#endif
