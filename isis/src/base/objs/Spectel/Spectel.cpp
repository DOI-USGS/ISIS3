/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2009/01/07 18:33:38 $
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
