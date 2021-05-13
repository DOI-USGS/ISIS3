/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Spectel.h"

namespace Isis{

  /**
   * @brief Constructs an empty Spectel 
   * 
   * @author 2015-06-11 Stuart Sides 
   * 
   */
  Spectel::Spectel():
      Pixel(0, 0, 0, Isis::Null) {
    m_center = Isis::Null;
    m_width = Isis::Null; 
  }


  /**
   * @brief Constructs a Spectel using its l,s,b coordinates, its 
   *        DN value, and its wavelength.
   * 
   * @author 2015-05-08 Kristin Berry (5/8/2015)
   * 
   * @param line spectel line coordinate
   * @param sample spectel sample coordinate
   * @param band spectel band coordinate
   * @param DN spectel value
   * @param center center wavelength of spectel 
   * @param width wavelength width of spectel 
   */
  Spectel::Spectel(int sample, int line, int band, double DN, double center, double width):
     Pixel(sample, line, band, DN) {
    m_center = center;
    m_width = width; 
  }

  /**
   * @brief Constructs a Spectel, given a Pixel, center 
   *        wavelength, and width. 
   * 
   * @author 2015-08-09 Kristin Berry
   * 
   * @param pixel Pixel used to create the Spectel
   * @param center Center wavelength of the Spectel
   * @param width Width of the Spectel
   */
  Spectel::Spectel(Pixel pixel, double center, double width) : Pixel(pixel) {
    m_center = center;
    m_width = width; 
  }


  /**
   * @brief Constructs a Spectel, given a Spectel
   * 
   * @author 2015-08-06 Kristin Berry
   * 
   * @param spectel Spectel to copy 
   */
  Spectel::Spectel(const Spectel& spectel) : Pixel(spectel.sample(), 
                                                   spectel.line(), spectel.band(), spectel.DN()) {
    m_center = spectel.centerWavelength();
    m_width = spectel.filterWidth();
  }


  //! Default destructor
  Spectel::~Spectel(){
  }


  //! Copy assignment operator
  Spectel &Spectel::operator=(const Spectel& other) {
    Pixel::operator=(other); 
    m_center = other.centerWavelength();
    m_width = other.filterWidth(); 
    return *this; 
  }


  /**
   * Gets central wavelength of spectel. 
   *  
   * @author kberry (5/11/2015)
   * 
   * @return double center wavelength of spectel
   */
   double Spectel::centerWavelength() const {
     return m_center;
   }


  /**
   * Gets wavelength width associated with spectel. 
   * 
   * @author kberry (5/11/2015)
   * 
   * @return double wavelength width of spectel
   */
  double Spectel::filterWidth() const {
    return m_width; 
  }
}
