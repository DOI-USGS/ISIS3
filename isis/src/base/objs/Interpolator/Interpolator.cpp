/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/11/06 15:53:53 $                                                                 
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

#include <string>
#include "iException.h"
#include "Interpolator.h"

using namespace std;
namespace Isis {
/**                                                                       
 * Constructs an empty Interpolator object.                                       
 */                                                                       
  Interpolator::Interpolator () {
    Init ();
  }
  
/**
 * Constructs an Interpolator object, and sets the type of interpolation.
 * 
 * @param type The type of interpolation to be performed. (NearestNeighbor,
 * BiLinear or CubicConvulsion) 
 * @see Interpolator.h
 */
  Interpolator::Interpolator (const interpType &type) {
    Init ();
    p_type = type;
  }
  
  //! Destroys the Interpolator object.
  Interpolator::~Interpolator () {}
  
  //! Initializes the object data members.
  void Interpolator::Init () {
    p_type = None;
  }
  
/**                                                                       
 * Performs an interpolation on the data according to the parameters set in the 
 * constructor. If the buffer contains special pixel values then the current 
 * interpolation is abandoned and the next lower type of interpolation is 
 * attempted. The order from highest to lowest is (Cubic Convolution, Bi-linear 
 * and Nearest Neighbor).                                                        
 *                                                                        
 * @param isamp The exact sample position being interpolated within the image.
 * @param iline The exact line position being interpolated within the image.
 * @param buf[] The buffer of data to be interpolated. The buffer is assumed
 * to be the correct dimensions for the interpolator.
 *                                                                        
 * @internal
 * @todo Add a @return statement to this method.
 */                                                                         
  double Interpolator::Interpolate(const double isamp, const double iline,
                                       const double buf[]) {
    
    switch (p_type) {
      case None: {
        string message = "Interpolator type not set";
        throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
        break;
      }
      case NearestNeighborType:
        return NearestNeighbor (isamp, iline, buf);
        break;
      case BiLinearType:
        return BiLinear (isamp, iline, buf);
        break;
      case CubicConvolutionType:
        return CubicConvolution (isamp, iline, buf);
        break;
    }
  
    string message = "Invalid interpolator";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }
/**                                                                       
 * Sets the type of interpolation. (NearestNeighbor, BiLinear, CubicConvulsion).
 * @see Interpolator.h
 *                                                                        
 * @param type The type of interpolation to be set.                                                        
 */                                                                         
  void Interpolator::SetType (const interpType &type) {
    p_type = type;
  }
  
  
/**
 * Performs a nearest-neighbor interpolation on the buffer data.
 * 
 * @param isamp The input sample/column/x coordinate.
 * @param iline The input line/row/y coordinate.
 * @param buf[] The data buffer to be interpolated. For nearest-neighbor
 * there is only one value, the pixel of interest.
 * 
 * @internal
 * @todo Add a @return tag to this method.
 */
  double Interpolator::NearestNeighbor (const double isamp, const double iline,
                                            const double buf[]) {
    return buf[0];
  }
  
  
/**
 * Performs a bi-linear interpolation on the buffer data.
 * 
 * @param isamp The input sample/column/x coordinate.
 * @param iline The input line/row/y coordinate.
 * @param buf[] The data buffer to be interpolated. For bi-linear there are four
 * values with the pixel of interest in the upper left corner of a 2x2 window.
 * 
 * @internal
 * @todo Add a @return tag to this method.
 */
    double Interpolator::BiLinear (const double isamp, const double iline,
                                     const double buf[]) {
  
    // Get the fractional portions of the sample and line coordinates
    double j = int (isamp);   
    double k = int (iline);
    double a = isamp - j;
    double b = iline - k;
  
    // If any of the 4 pixels are special pixels, drop down to a nearest neighbor
    for (int i=0; i<4; i++) {
      if (Isis::IsSpecial(buf[i])) {
        return NearestNeighbor (isamp, iline, 
                                &buf[(int)(a + 0.5) + 2 * (int)(b+0.5)]);
        break;
      }
    }
  
    // Otherwise do the bilinear
    return (1.0-a) * (1.0-b) * buf[0] +
           a * (1.0-b) * buf[1] +
           (1.0-a) * b * buf[2] + 
           a * b * buf[3];
  
  }
  
/**
 * Performs a cubic-convulsion interpolation on the buffer data.
 * 
 * @param isamp The input sample/column/x coordinate.
 * @param iline The input line/row/y coordinate.
 * @param buf[] The data buffer to be interpolated. For cubic-convultion
 * there are sixteen values with the pixel of interest in the second row and
 * second column of a 4x4 window.
 * 
 * @internal
 * @todo Add a @return tag to this method.
 */
  double Interpolator::CubicConvolution (const double isamp,
                                             const double iline,
                                             const double buf[]) {
  
    // If any of the 16 pixels are special pixels, drop down to a bilinear
    for (int i=0; i<16; i++) {
      if (Isis::IsSpecial(buf[i])) {
        double tbuf[4] = {buf[5], buf[6], buf[9], buf[10]};
        return BiLinear (isamp, iline, tbuf);
        break;
      }
    }
  
    double j = int (isamp);
    double k = int (iline);
    double a = isamp - j;
    double b = iline - k;
  
    /**
     * This algorithm has been checked extensively, and is correctly coded! 
     *  
     * This algorithm works by modeling the picture locally with a polynomial 
     * surface, which means DNs less than all inputs or greater than all inputs are 
     * possible. 
     */
    return  -b * (1.0-b)*(1.0-b) * (-a * (1.0-a)*(1.0-a) * buf[0] +
                                   (1.0 - 2.0 * a*a + a*a*a) * buf[1] + 
                                   a * (1.0 + a - a*a) * buf[2] - 
                                   a*a * (1.0 - a) * buf[3]) +
          
           (1.0 - 2.0 * b*b + b*b*b) * (-a * (1.0-a)*(1.0-a) * buf[4] +                 
                                        (1.0 - 2.0 * a*a + a*a*a) * buf[5] +
                                        a * (1.0 + a - a*a) * buf[6] -        
                                        a*a * (1.0 - a) * buf[7]) +
  
           b * (1.0 + b - b*b) * (-a * (1.0-a)*(1.0-a) * buf[8] +         
                                  (1.0 - 2.0 * a*a + a*a*a) * buf[9] +
                                  a * (1.0 + a - a*a) * buf[10] -        
                                  a*a * (1.0 - a) * buf[11]) +
  
           b*b * (b-1.0) * (-a * (1.0-a)*(1.0-a) * buf[12] +           
                            (1.0 - 2.0 * a*a + a*a*a) * buf[13] +
                            a * (1.0 + a - a*a) * buf[14] -        
                            a*a * (1.0 - a) * buf[15]);
  
  }
  
/**                                                                       
 * Returns the number of samples needed by the interpolator.                                                          
 *                                                                                                                                               
 * @return The number of samples needed.                                      
 */                                                                         
  int Interpolator::Samples () {
    
    switch (p_type) {
      case None: {
        string message = "Interpolator type not set";
        throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
        break;
      }
      case NearestNeighborType:
        return 1;
        break;
      case BiLinearType:
        return 2;
        break;
      case CubicConvolutionType:
        return 4;
        break;
    }
  
    string message = "Invalid interpolator";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }
  
/**                                                                       
 * Returns the number of lines needed by the interpolator.                                                          
 *                                                                                                                                               
 * @return The number of lines needed.                                      
 */   
  int Interpolator::Lines () {
    
    switch (p_type) {
      case None: {
        string message = "Interpolator type not set";
        throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
        break;
      }
      case NearestNeighborType:
        return 1;
        break;
      case BiLinearType:
        return 2;
        break;
      case CubicConvolutionType:
        return 4;
        break;
    }
  
    string message = "Invalid interpolator";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }
  
/**                                                                       
 * Returns the sample coordinate of the center pixel in the buffer for the
 * interpolator.
 *                                                                                                                                               
 * @return The sample coordinate of the center pixel.                                       
 */                                                                         
  double Interpolator::HotSample () {
    
    switch (p_type) {
      case None: {
        string message = "Interpolator type not set";
        throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
        break;
      }
      // To get the correct pixel for NN you have to round the sample
      case NearestNeighborType:
        return -0.5;
        break;
      // To get the correct pixel for BL you have to truncate the sample
      case BiLinearType:
        return 0.0;
        break;
      // To get the correct pixel for CC you have to truncate the sample
      case CubicConvolutionType:
        return 1.0;
        break;
    }
  
    string message = "Invalid interpolator";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }
  
/**                                                                       
 * Returns the line coordinate of the center pixel in the buffer for the
 * interpolator.
 *                                                                                                                                               
 * @return The line coordinate of the center pixel.                                       
 */  
  double Interpolator::HotLine () {
    
    switch (p_type) {
      case None: {
        string message = "Interpolator type not set";
        throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
        break;
      }
      // To get the correct pixel for NN you have to round the line
      case NearestNeighborType:
        return -0.5;
        break;
      // To get the correct pixel for BL you have to truncate the line
      case BiLinearType:
        return 0.0;
        break;
      // To get the correct pixel for CC you have to truncate the line
      case CubicConvolutionType:
        return 1.0;
        break;
    }
  
    string message = "Invalid interpolator";
    throw Isis::iException::Message(Isis::iException::Programmer,message, _FILEINFO_);
  }
}
