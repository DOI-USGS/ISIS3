/**                                                                       
 * @file                                                                  
 * $Revision: 1.1.1.1 $                                                             
 * $Date: 2006/10/31 23:18:07 $                                                                 
 *                                                                        
 *   Unless noted otherwise, the portions of Isis written by the USGS are 
 *   public domain. See individual third-party library and package descriptions 
 *   for intellectual property information, user agreements, and related  
 *   information.                                                         
 *                                                                        
 *   Although Isis has been used by the USGS, no warranty, expressed or   
 *   implied, is made by the USGS as to the accuracy and functioning of such 
 *   software and related material nor shall the fact of distribution     
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.                                        
 *                                                                        
 *   For additional information, launch                                   
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html                
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.                                    
 */                                                                       
#ifndef Interpolator_h
#define Interpolator_h

#include "SpecialPixel.h"

namespace Isis {
/**                                                                       
 * @brief Pixel interpolator.                
 *                                                                        
 * This class is used for interpolating buffers of pixel data. It is usually 
 * associated with spacial translation, rotations and scaling in geometric
 * warping algorithums (i.e., rubber sheeting). When special-pixel values are
 * found in the data buffer the current interpolator is abandonded and the next
 * lower interpolator is used instead (i.e.; if Cubic-convolution can not be 
 * performed then a bi-linear is used and if the bi-linear can not be performed 
 * then nearest-neighbor will be used.             
 *                                                                        
 * @ingroup Geometry                                                  
 *                                                                        
 * @author 2002-10-09 Stuart Sides                                                                                  
 *                                                                        
 * @internal                                                              
 *   @todo This class needs an example.                                                               
 *   @history 2003-02-12 Stuart Sides Fixed misspelling of object name in XML
 *   file. (Note: XML files no longer used in documentation.)
 *   @history 2003-05-16 Stuart Sides modified schema from 
 *   astrogeology...isis.astrogeology.                                              
 */                                                                       
  class Interpolator {
    public:
      /**
       * The interpolator type, including: None, Nearest Neighbor, BiLinear
       * or Cubic Convultion.
       */
      enum interpType {
        None=0, 
        NearestNeighborType=1, 
        BiLinearType=2, 
        CubicConvolutionType=4
      };
  
    private:
      /**
       * The type of interpolation to be performed. (NearestNeighbor, BiLinear
       * or CubicConvultion
       */
      interpType p_type;
      

      void Init ();
  

      double NearestNeighbor (const double isamp, const double iline,
                              const double buf[]);
  
      // Do a bilinear interpolation on the buf
      double BiLinear (const double isamp, const double iline,
                       const double buf[]);
  
      // Do a cubic convolution on the buf
      double CubicConvolution (const double isamp, const double iline,
                               const double buf[]);
  
  
    public:
      // Constructores / destructores
      Interpolator ();
      Interpolator (const interpType &type);
      ~Interpolator ();
  
  
      // Perform the interpolation with the current settings
      double Interpolate (const double isamp, const double iline,
                          const double buf[]);
  
  
      // Set the type of interpolation
      void SetType (const interpType &type);
  
  
      // Return sizes needed by the interpolator
      int Samples ();
      int Lines();
  
  
      // Return the hot spot (center pixel) of the interpolator zero based
      double HotSample();
      double HotLine();
  };
};
#endif
