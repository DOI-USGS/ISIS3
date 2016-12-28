/**
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
   * Construct from PVL and Cube file.
   *
   *
   * @param pvl Photometric parameter files
   * @param cube Input cube file
   * @param useCamera Indicates whether to use the camera model for the given 
   *                  cube.
   *  
   * @author 2010-02-21 Kris Becker
   */
  PhotometricFunction::PhotometricFunction(PvlObject &pvl, 
                                           Cube &cube , 
                                           bool useCamera) {
    if (useCamera) {
      m_camera = cube.camera();
    }
    
    m_useDem = false;
    m_incidence = 0.0;
    m_emission = 0.0;
    m_phase = 0.0;
  }


  //! Destructor
  PhotometricFunction::~PhotometricFunction() {
  }


  /**
   * Compute photometric DN at given line/sample/band.
   *
   * This routine applies the photometric angles to the equation
   * and returns the calibration coefficient at the given  cube
   * location.
   *
   * The return parameter is the photometric standard/photometric
   * correction coefficient at the given pixel location. 
   *  
   * Returns ISIS Null value if unable to set the given 
   * line/sample. 
   *
   * @param line   Line of cube image to compute photometry
   * @param sample Sample of cube image to compute photometry
   * @param band   Band of cube image to compute photometry
   *
   * @return double Photometric correction at cube loation
   *  
   * @author 2010-02-21 Kris Becker
   */
  double PhotometricFunction::compute(const double &line, 
                                      const double &sample, 
                                      int bandNumber,
                                      bool useDem) const {
    // Update band if necessary
    if (m_camera->Band() != bandNumber) {      
      m_camera->SetBand(bandNumber);
    }
    if (!m_camera->SetImage(sample, line)) {
      return Null;
    }
    // calculate photometric angles
    double incidence = m_camera->IncidenceAngle();
    double emission = m_camera->EmissionAngle();
    double phase = m_camera->PhaseAngle();
    bool success = true;

    if (m_useDem) {
      Angle phaseAngle, incidenceAngle, emissionAngle;
      m_camera->LocalPhotometricAngles(phaseAngle, 
                                      incidenceAngle, 
                                      emissionAngle, 
                                      success);
      if (success) {
        phase = phaseAngle.degrees();
        incidence = incidenceAngle.degrees();
        emission = emissionAngle.degrees();
      }
    }

    if ( !success 
         || incidence < minimumIncidenceAngle() 
         || incidence > maximumIncidenceAngle() 
         || emission < minimumEmissionAngle() 
         || emission > maximumEmissionAngle() 
         || phase < minimumPhaseAngle() 
         || phase > maximumPhaseAngle()) {
       return Null;
    }
    return photometry(incidence, emission, phase, bandNumber);
  }


  /**
   * Apply Lommel Seeliger, Rolo, Minnaert, and McEwen.
   *
   * Short function dispatched for each line to apply the Lommel 
   * Seeliger, Rolo, Minnaert and McEwen photometric correction 
   * functions. 
   *
   * @param in Buffer containing input data
   * @param out Buffer of photometrically corrected data
   *  
   * @author 2010-02-21 Kris Becker
   * @internal
   *   @history 2016-10-07 Makayla Shepherd & Ian Humphrey - Moved this function from photcorri to
   *                           photcorri
   */
  void PhotometricFunction::operator()(Buffer &in, 
                                       Buffer &out) const {

    for (int i = 0; i < in.size(); i++) {
      //  Don't correct special pixels
      if (IsSpecial(in[i])) {
        out[i] = in[i];
      }
      else {
      // Get correction and test for validity
        double ph = photometry(m_incidence, 
                               m_emission, 
                               m_phase, 
                               in.Band(i))
                    / compute(in.Line(i), 
                              in.Sample(i), 
                              in.Band(i), 
                              m_useDem);

        out[i] = (IsSpecial(ph) ? Null : in[i] * ph);
      }
    }
    return;
  }


  /**
   * Gets the name of the algorithm from the given PVL. This value is expected 
   * to be stored in the PhotometricModel object under the Algorithm group's 
   * Name keyword. 
   *  
   * @param pvl A PVL containing a PhotometricModel object, Algorithm group, and 
   *            Name keyword.
   * 
   * @return @b QString The name of this photometric model's algorithm.
   */
  QString PhotometricFunction::algorithmName(const PvlObject &pvl) {
    return pvl.findObject("PhotometricModel")
              .findGroup("Algorithm", Pvl::Traverse)
              .findKeyword("Name")[0];
  }


  /**
   * Sets a camera object to this function class.
   * 
   * @param cam Pointer to a camera.
   */
  void PhotometricFunction::setCamera(Camera *cam) {
    m_camera = cam;
  }


  /**
   * Sets whether to use a DEM for this class. 
   * 
   * @param bool Indicates whether a DEM will be used.
   * 
   * @author 2016-10-07 Makayla Shepherd & Ian Humphrey
   */
  void PhotometricFunction::useDem(bool useDem) {
    m_useDem = useDem;
  }


  /**
    * Sets the incidence angle.
    * 
    * @param angle The incidence angle, in degrees.
    * 
    * @author 2016-10-07 Makayla Shepherd & Ian Humphrey
    */  
  void PhotometricFunction::setIncidence(double angle) {
    m_incidence = angle;
  }


  /**
    * Sets the incidence reference angle.
    * 
    * @param angle The incidence reference angle, in degrees.
    */  
  void PhotometricFunction::setIncidenceReference(double angle) {
    m_incRef = angle;
  }


  /**
    * Sets the minimum incidence angle.
    * 
    * @param angle The value for the minimum incidence angle, in degrees. 
    */  
  void PhotometricFunction::setMinimumIncidenceAngle(double angle) {
    m_minimumIncidenceAngle = angle;
  }


  /**
    * Sets the maximum incidence angle.
    * 
    * @param angle The value for the maximum incidence angle, in degrees. 
    */  
  void PhotometricFunction::setMaximumIncidenceAngle(double angle) {
     m_maximumIncidenceAngle = angle;
  }


  /**
    * Sets the emission angle.
    * 
    * @param angle The emission angle, in degrees,
    * 
    * @author 2016-10-07 Makayla Shepherd & Ian Humphrey
    */
  void PhotometricFunction::setEmission(double angle) {
    m_emission = angle;
  }


  /**
    * Sets the emission reference angle.
    * 
    * @param angle The incidence reference angle, in degrees.
    */  
  void PhotometricFunction::setEmissionReference(double angle) {
    m_emaRef = angle;
  }


  /**
    * Sets the minimum emission angle.
    * 
    * @param angle The value for the minimum emission angle, in degrees. 
    */  
  void PhotometricFunction::setMinimumEmissionAngle(double angle) {
     m_minimumEmissionAngle = angle;
  }


  /**
    * Sets the maximum emission angle.
    * 
    * @param angle The value for the maximum emission angle, in degrees. 
    */  
  void PhotometricFunction::setMaximumEmissionAngle(double angle) {
     m_maximumEmissionAngle = angle;
  }


  /**
    * Sets the phase angle.
    * 
    * @param angle The phase angle, in degrees.
    * 
    * @author 2016-10-07 Makayla Shepherd & Ian Humphrey
    */   
  void PhotometricFunction::setPhase(double angle) {
    m_phase = angle;
  }


  /**
    * Sets the phase reference angle.
    * 
    * @param angle The phase reference angle, in degrees.
    */  
  void PhotometricFunction::setPhaseReference(double angle) {
    m_phaRef = angle;
  }


  /**
    * Sets the minimum phase angle.
    * 
    * @param angle The value for the minimum phase angle, in degrees. 
    */  
  void PhotometricFunction::setMinimumPhaseAngle(double angle) {
     m_minimumPhaseAngle = angle;
  }


  /**
    * Sets the maximum phase angle.
    * 
    * @param angle The value for the maximum phase angle, in degrees. 
    */  
  void PhotometricFunction::setMaximumPhaseAngle(double angle) {        
    m_maximumPhaseAngle = angle;
  }


  /**
    * Indicates whether a DEM will be used. 
    *  
    * @return @b bool True if using a DEM. 
    * 
    * @author 2016-10-07 Makayla Shepherd & Ian Humphrey 
    */  
  bool PhotometricFunction::useDem() const {
    return m_useDem;
  }


  /**
    * Gets the incidence angle. 
    *  
    * @return @b double The value for the incidence angle, in degrees. 
    * 
    * @author 2016-10-07 Makayla Shepherd & Ian Humphrey 
    */
  double PhotometricFunction::incidence() const {
    return m_incidence;
  }


  /**
    * Gets the incidence reference angle.
    * 
    * @return @b double The incidence reference angle, in degrees.
    */  
  double PhotometricFunction::incidenceReference() const {
    return m_incRef;
  }


  /**
    * Gets the minimum incidence angle.
    * 
    * @return @b double The value for the minimum incidence angle, in degrees. 
    */  
  double PhotometricFunction::minimumIncidenceAngle() const {
    return m_minimumIncidenceAngle;
  }


  /**
    * Gets the maximum incidence angle.
    * 
    * @return @b double The value for the maximum incidence angle, in degrees. 
    */  
  double PhotometricFunction::maximumIncidenceAngle() const {
    return m_maximumIncidenceAngle;
  }


  /**
    * Gets the emission angle.
    * 
    * @return @b double The value for the emission angle, in degrees. 
    * 
    * @author 2016-10-07 Makayla Shepherd & Ian Humphrey 
    */
  double PhotometricFunction::emission() const {
   return m_emission;
  }


  /**
    * Gets the emission reference angle.
    * 
    * @return @b double The emission reference angle, in degrees.
    */  
  double PhotometricFunction::emissionReference() const {
    return m_emaRef;
  }


  /**
    * Gets the minimum emission angle.
    * 
    * @return @b double The value for the minimum emission angle, in degrees. 
    */  
  double PhotometricFunction::minimumEmissionAngle() const {
    return m_minimumEmissionAngle;
  }


  /**
    * Gets the maximum emission angle.
    * 
    * @return @b double The value for the maximum emission angle, in degrees. 
    */  
  double PhotometricFunction::maximumEmissionAngle() const {
    return m_maximumEmissionAngle;
  }


  /**
    * Gets the phase angle.
    * 
    * @return @b double The value for the phase angle, in degrees. 
    * 
    * @author 2016-10-07 Makayla Shepherd & Ian Humphrey 
    */  
    
  double PhotometricFunction::phase() const {
    return m_phase;
  }


  /**
    * Gets the phase reference angle.
    * 
    * @return @b double The phase reference angle, in degrees.
    */  
  double PhotometricFunction::phaseReference() const {
    return m_phaRef;
  }


  /**
    * Gets the minimum phase angle.
    * 
    * @return @b double The value for the minimum phase angle, in degrees. 
    */  
  double PhotometricFunction::minimumPhaseAngle() const {
    return m_minimumPhaseAngle;
  }


  /**
    * Gets the maximum phase angle.
    * 
    * @return @b double The value for the maximum phase angle, in degrees. 
    */  
  double PhotometricFunction::maximumPhaseAngle() const {
    return m_maximumPhaseAngle;
  }
 
  /**
    * Gets the normal profile configuration.
    * 
    * @return @b DbProfile The normal parameter profile container. 
    */  
  DbProfile PhotometricFunction::normalProfile() const {
    return m_normProf;
  }
} // namespace Isis


