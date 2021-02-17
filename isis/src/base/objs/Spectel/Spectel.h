#ifndef Spectel_h
#define Spectel_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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


