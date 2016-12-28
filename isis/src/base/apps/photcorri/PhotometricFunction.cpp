/**
 * @file
 * $Revision$
 * $Date$
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
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

#include "Angle.h"
#include "Camera.h"
#include "DbProfile.h"
#include "PhotometricFunction.h"
#include "PvlObject.h"

using namespace std;

namespace Isis {

  /**
   * @brief Construct from PVL and Cube file
   *
   * @author Kris Becker - 2/21/2010
   *
   * @param pvl Photometric parameter files
   * @param cube Input cube file
   */
  PhotometricFunction::PhotometricFunction ( PvlObject &pvl, Cube &cube , bool useCamera) {
    if (useCamera)
    _camera = cube.camera();
    
    m_useDem = false;
    m_incidence = 0;
    m_emission = 0;
    m_phase = 0;
  }
  /**
   * @brief Compute photometric DN at given line/sample/band
   *
   * This routine applies the photometric angles to the equation
   * and returns the calibration coefficient at the given  cube
   * location.
   *
   * The return parameter is the photometric standard/photometric
   * correction coefficient at the given pixel location.
   *
   * @author Kris Becker - 2/21/2010
   *
   * @param line   Line of cube image to compute photometry
   * @param sample Sample of cube image to compute photometry
   * @param band   Band of cube image to compute photometry
   *
   * @return double Photometric correction at cube loation
   */
  double PhotometricFunction::Compute ( const double &line, const double &sample, int band, 
    bool useDem) const {
    // Update band if necessary
    if (_camera->Band() != band) {      
      _camera->SetBand(band);
    }
    if (!_camera->SetImage(sample, line))
      return (Null);
      // calculate photometric angles
      double i = _camera->IncidenceAngle();
      double e = _camera->EmissionAngle();
      double g = _camera->PhaseAngle();
      bool success = true;

      if (m_useDem) {
        Angle phase, incidence, emission;
        _camera->LocalPhotometricAngles(phase, incidence, emission, success);
        if (success) {
          g = phase.degrees();
          i = incidence.degrees();
          e = emission.degrees();
        }
      }

      if ( !success || i < MinimumIncidenceAngle() || i > MaximumIncidenceAngle() || 
      e < MinimumEmissionAngle() || e > MaximumEmissionAngle() || g < MinimumPhaseAngle() || 
      g > MaximumPhaseAngle())
         return (Null);
         return photometry(i, e, g, band);
      }

  /**
   * @brief Apply Lommel Seeliger, Rolo, Minnaert, and McEwen 
   *
   * Short function dispatched for each line to apply the Lommel 
   * Seeliger, Rolo, Minnaert and McEwen photometric correction 
   * functions. 
   *
   * @author kbecker (2/20/2010)
   * 
   * @internal
   *   @history 2016-10-07 Makayla Shepherd & Ian Humphrey - Moved this function from photcorri to
   *                           photcorri
   *
   * @param in Buffer containing input data
   * @param out Buffer of photometrically corrected data
   */
  void PhotometricFunction::operator()( Buffer &in, Buffer &out ) const {

    for (int i = 0; i < in.size(); i++) {
      //  Don't correct special pixels
      if (IsSpecial(in[i])) {
        out[i] = in[i];
      }
      else {
      // Get correction and test for validity
        double ph = photometry(m_incidence, m_emission, m_phase, 
                                   in.Band(i)) / Compute(in.Line(i), in.Sample(i), 
                                                                   in.Band(i), m_useDem);
        out[i] = (IsSpecial(ph) ? Null : in[i] * ph);
      }
    }
    return;
  }

  /**
   * Sets the m_useDem member variable
   * 
   * @author 2016-10-07 Makayla Shepherd & Ian Humphrey
   * 
   * @param bool Boolean that sets the member variable
   */     
  void PhotometricFunction::setUseDem(bool useDem) {
    m_useDem = useDem;
  }

  /**
    * Gets the m_useDem member variable
    * 
    * @author 2016-10-07 Makayla Shepherd & Ian Humphrey 
    */  
     
  bool PhotometricFunction::useDem() const {
    return m_useDem;
  }

  /**
    * Sets the m_incidence member variable
    * 
    * @author 2016-10-07 Makayla Shepherd & Ian Humphrey
    * 
    * @param double Double that sets the member variable 
    */  
     
  void PhotometricFunction::setIncidence(double incidence) {
    m_incidence = incidence;
  }

  /**
    * Gets the m_incidence member variable
    * 
    * @author 2016-10-07 Makayla Shepherd & Ian Humphrey 
    */
  double PhotometricFunction::incidence() const {
    return m_incidence;
  }

  /**
    * Sets the m_incidence member variable
    * 
    * @author 2016-10-07 Makayla Shepherd & Ian Humphrey
    * 
    * @param double Double that sets the member variable 
    */  
     
  void PhotometricFunction::setEmission(double emission) {
    m_emission = emission;
  }

  /**
    * Gets the m_emission member variable
    * 
    * @author 2016-10-07 Makayla Shepherd & Ian Humphrey 
    */
  double PhotometricFunction::emission() const {
   return m_emission;
  }

  /**
    * Sets the m_phase member variable
    * 
    * @author 2016-10-07 Makayla Shepherd & Ian Humphrey
    * 
    * @param double Double that sets the member variable
    */   
  void PhotometricFunction::setPhase(double phase) {
    m_phase = phase;
  }

  /**
    * Gets the m_phase member variable
    * 
    * @author 2016-10-07 Makayla Shepherd & Ian Humphrey 
    */  
    
  double PhotometricFunction::phase() const {
    return m_phase;
  }
   
} // namespace Isis


