#ifndef Spectel_h
#define Spectel_h

/**
 * @file                                                                  
 * $Revision: 6129 $ 
 * $Date: 2015-04-02 10:42:32 -0700 (Thu, 02 Apr 2015) $ 
 * $Id: CalculatorStrategy.h 6129 2015-04-02 17:42:32Z jwbacker@GS.DOI.NET $
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

#include "Pixel.h"

namespace Isis {
  /**
   * @brief Stores information about a "Spectral pixel" or 
   *        spectel.
   * 
   * @author 2015-05-11 Kristin Berry (5/11/2015)
   *  
   * @internal
   *  @history 2015-06-09 Stuart Sides - Added empty constructor and constness
   *  @history 2015-08-04 Kristin Berry - Added copy constructor,
   *           copy assignment operator, virtual destructor, and a
   *           constructor that takes a Pixel, center, and width. 
   */
  class Spectel : public Isis::Pixel {
    public:
      Spectel();
      Spectel(int sample, int line, int band, double DN, double center, double width);
      Spectel(Pixel pixel, double center, double width);
      Spectel(const Spectel& spectel);
      virtual ~Spectel();
      
      Spectel &operator=(const Spectel& other); 
      
      double centerWavelength() const;
      double filterWidth() const; 

    private:
      //! Center wavelength associated with pixel
      double m_center;
      //! Wavelength width (FWHM) associated with pixel 
      double m_width; 
  };
}

#endif


