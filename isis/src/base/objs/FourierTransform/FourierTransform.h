#ifndef FourierTransform_h
#define FourierTransform_h
/**
 * @file
 * $Revision: 1.1.1.1 $
 * $Date: 2006/10/31 23:18:07 $
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch 
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website, 
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */ 
 
#include <complex>
#include <vector>
#include "Constants.h"

namespace Isis
{
 /**                                                                       
  * @brief Fourier Transform class                
  * 
  *     This class is used to apply a Fourier transform to a vector of complex
  * data as well as the inverse Fourier transform. Applying the Fourier
  * transform on data in the spatial domain will convert it to data in the 
  * Fourier (or frequency) domain. The inverse transform takes data
  * from the frequency domain to the spatial.
  *                                                                
  * If you would like to see FourierTransform being used
  *         in implementation, see fft.cpp or ifft.cpp.
  *                                                                        
  * @ingroup Math and Statistics
  * 
  * @author Jacob Danton - 2005-11-28
  *                                                                                                                                                                                      
  * @internal                                                                                                                           
  */
    class FourierTransform
    {
    public:
	FourierTransform ();
	~FourierTransform ();
    std::vector< std::complex<double> > Transform(std::vector< std::complex<double> > input);
    std::vector< std::complex<double> > Inverse(std::vector< std::complex<double> > input);
    bool IsPowerOfTwo(int n);
    int lg(int n);
    int BitReverse (int n, int x);
    int NextPowerOfTwo (int n);
    }; 
}

#endif
