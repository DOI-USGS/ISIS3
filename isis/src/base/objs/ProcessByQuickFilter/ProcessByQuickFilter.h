#ifndef FilterProcess_h
#define FilterProcess_h
/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2006/12/15 15:58:39 $
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
 
#include "Process.h"
#include "Buffer.h"
#include "QuickFilter.h"

namespace Isis {
 /**                                                                       
  * @brief Process cubes using a Filter Object                
  *                                                                        
  * This class processes an entire cube using an Filter object. That is, it 
  * walks a Filter object line-by-line over an input cube. This allows for the 
  * development of programs which do spatial filters such as highpass, lowpass, 
  * and sharpen. Understanding the Filter class is essential in order to utilize 
  * this class. This class expects the user to define an NxM boxcar size. Using 
  * that information, a Filter object is created and loaded with the proper cube 
  * data in order to walk the NxM boxcar through the entire cube in a very 
  * efficient manner. Currently it is required that the following parameters be
  * available in the application XML file: 
  *   LINES - Defines the height of the boxcar to convolve over the cube 
  *   SAMPLES - Defines the width of the boxcar to convoled over the cube 
  *   MINIMUM - Defines the minimum number of pixels in the boxcar in order for 
  *             statistics to be computed (see Filter class)          
  *   LOW - Defines minimum valid pixel value to be included in statistics 
  *         (see Filter class) 
  *   HIGH - Defines maximum valid pixel value to be included in statistics 
  *          (see Filter class) 
  *                                                               
  * If you would like to see ProcessByQuickFilter being used in implementation, 
  * see sharpen.cpp                                               
  *                                                                        
  * @ingroup HighLevelCubeIO                                                  
  *                                                                        
  * @author 2003-03-31 Jeff Anderson                                                                                                                                                   
  *                                                                        
  * @internal                                                              
  *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeology... 
  *                                      isis.astrogeology...
  *   @history 2003-06-02 Jeff Anderson - Fixed a bug where line unfolding at 
  *                                       the bottom of the cube was always
  *                                        using band 1
  *   @history 2003-08-28 Jeff Anderson - Added SetFilterParameters method
  *   @history 2005-02-08 Elizabeth Ribelin - Modified file to support Doxygen 
  *                                           documentation    
  *   @history 2006-12-15 Jeff Anderson - Fixed bug for images with 1 line
  */                                                                       
  class ProcessByQuickFilter : public Isis::Process {
  
    public:
      ProcessByQuickFilter ();
  
      void StartProcess (void funct(Isis::Buffer &in, Isis::Buffer &out, Isis::QuickFilter &filter));
      void SetFilterParameters(int samples, int lines,
                               double low=-DBL_MAX, double high=DBL_MAX, 
                               int minimum=0);
  
    private:
      bool p_getParametersFromUser; /**<Flag to indicate whether or not to get 
                                        parameters from the user*/ 
      int p_boxcarSamples;          /**<Number of samples in the boxcar. 
                                        Must be odd.*/
      int p_boxcarLines;            /**<Number of lines in the boxcar. 
                                        Must be odd.*/
      int p_minimum;                /**<Minimum number of valid pixels in the 
                                        sample-by-line boxcar in order for
                                        statistical computations to be valid. 
                                        Defaults to 0 */
      double p_low;                 /**<Minimum valid pixel value to include in 
                                        statistical computations of the boxcar.
                                        Defaults to DBL_MAX */
      double p_high;                /**<Maximum valid pixel value to include in 
                                        statistical computations of the boxcar. 
                                         Defaults to DBL_MAX */
  
      void GetFilterParameters ();
  };
};

#endif
