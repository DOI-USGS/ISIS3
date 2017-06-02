/**
 * @file
 * $Revision: 4928 $
 * $Date: 2012-12-21 10:44:12 -0700 (Fri, 21 Dec 2012) $
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
#include "SpacecraftPosition.h"

#include <cfloat>
#include <iomanip>

#include <QVector>

#include "Constants.h"
#include "Distance.h"
#include "EndianSwapper.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "iTime.h"
#include "Longitude.h"
#include "NaifStatus.h"

using namespace std;

namespace Isis {

  /**
   * @brief constructor for swapping observer/target parameters 
   *  
   * This constructor utlizes a protected constructor in the SpicePosition class 
   * specially designed to handle this option.  Passing false into this 
   * constructor results in preexisting behavior.  True will swap observer/target 
   * when determining the s/c position. 
   *  
   * It is critical that the targetCode and observerCode be of the same exact 
   * order as they are in preexisting code.  This is consistent with the current 
   * SpicePosition constructor with an additional boolean parameter that 
   * indicates to treat the targetCode as the observer and the observerCode as 
   * the target. 
   * 
   * @author 2012-10-11 Kris Becker
   * 
   * @param targetCode         NAIF code for target
   * @param observerCode       NAIF code for observer
   * @param swapObserverTarget Boolean to specify swap
   */
  SpacecraftPosition::SpacecraftPosition(int targetCode, int observerCode,
                                         const LightTimeCorrectionState &ltState,
                                         const Distance &radius) : 
                                         SpicePosition(targetCode, observerCode,
                                                       ltState.isObserverTargetSwapped()) { 
    m_abcorr = ltState;
    m_radius = radius;
    return;
  }



/**
 * @brief Returns the time it takes for light to travel the radius of the 
 *        target
 *  
 *  This method returns the time in seconds it takes to travel the distance of
 *  the radius of the target body.  This is a function of the Distance
 *  parameter provided at the time this object was constructed.
 *  
 * @author 2012-10-23 Kris Becker
 * 
 * @return double Time in seconds it takes light to travel the distance of the 
 *         radius provided/set in the object
 */
  double SpacecraftPosition::getRadiusLightTime() const {
    return (m_radius.kilometers()/clight_c());
  }

/**
 * @brief Returns the time it takes for light to travel a given distance 
 *  
 *  
 * @author 2012-10-23 Kris Becker
 *  
 * @param  radius Distance to compute light time travel for
 * @return double Time in seconds it takes light to travel given distance 
 *        
 */
  double SpacecraftPosition::getDistanceLightTime(const Distance &distance) {
    return (distance.kilometers()/clight_c());
  }


/**
 * @brief Set aberration correction value for determining positions
 * 
 * @author 2012-10-29 Kris Becker
 * 
 * @param correction Type of stellar aberration correction to apply
 */
  void SpacecraftPosition::SetAberrationCorrection(const QString &correction) {
    SpicePosition::SetAberrationCorrection(correction);  // Checks for validity
    m_abcorr.setAberrationCorrection(correction);
  }


/**
 * @brief Returns the stellr aberration correction applied
 * 
 * @author 2012-10-29 Kris Becker
 */
  QString SpacecraftPosition::GetAberrationCorrection() const {
    return (m_abcorr.getAberrationCorrection());
  }

/**
 * @brief Determine accurate position of target w.r.t. observer 
 *  
 * This method computes the position of the target w.r.t. the observer with 
 * additional specialized light time position of target and to a more accurate 
 * correction to the surface. 
 *  
 * NAIF routines are used to compute state vectors that have optional light 
 * time correction applied.  However, this uses the center of the body as the 
 * reference  
 * 
 * @author 2012-10-29 Kris Becker 
 *  
 * @internal
 *   @history 2012-11-01 Kris Becker - Revised parameters to computeStateVector
 *                                     to match comments.  References #1136.
 */
  void SpacecraftPosition::SetEphemerisTimeSpice() {

    // Both light time correction and surface light time correction *must* be requested
    // in order to invoke the algorithm below, otherwise we can call the pre-existing
    // implementation as it handles swap and light time adjustments as requested. 
    // The algorithm below only additionally handles light time surface correction.
    if ( !(m_abcorr.isLightTimeCorrected() && m_abcorr.isLightTimeToSurfaceCorrected()) ) {
      SpicePosition::SetEphemerisTimeSpice();
      return;
    }

    //////////////////////////////////////////////////////////////////////
    //  Proceed with applying light time corrections to surface.  The
    //  steps to make this more accurate is as follows:
    // 
    //  1)  Compute vector from observer to target to acquire the light
    //      time correction (in seconds)
    // 
    //  2)  Acquire the vector from the solar system barycenter (SSB) to
    //      the spacecraft at the specified time.
    // 
    //  3)  Acquire the vector from the SSB to the target less the light
    //      time from 1) adding back in the time it takes for light to
    //      travel the radius of the target.
    // 
    //  4)  Compute the vector state of the target from the observer
    //      buy subtracting the result of 2) from 3).
    //////////////////////////////////////////////////////////////////////

    SpiceDouble state[6], lt;
    bool hasVelocity;
    const SpiceInt ssbCode(0);

    //  1)  get observer/target light time
    computeStateVector(getAdjustedEphemerisTime(), getTargetCode(), getObserverCode(),
                       "J2000", GetAberrationCorrection(), state, hasVelocity, lt);

    //  2)  get SSB to s/c
    SpiceDouble ssbObs[6], ssbObs_lt;
    bool dummy;
    computeStateVector(getAdjustedEphemerisTime(), getObserverCode(), ssbCode,
                       "J2000", "NONE", ssbObs, dummy, ssbObs_lt);

    // 3) get adjusted target position from SSB
    double ltAdj_et = getAdjustedEphemerisTime() - lt + getRadiusLightTime();
    SpiceDouble ssbTarg[6], ssbTarg_lt;
    computeStateVector(ltAdj_et, getTargetCode(), ssbCode,
                       "J2000", "NONE", ssbTarg, dummy, ssbTarg_lt);

    // 4) compute target to observer
    (void) vsubg_c(ssbTarg, ssbObs, 6, state);

    // Store vector and light time correction results
    setStateVector(state, hasVelocity);
    setLightTime(ltAdj_et);
    return;
  }


/**
 * @brief Return the state of light time correction parameters
 * 
 * @author Kris Becker - 10/23/2012
 * 
 * @return LightTimeCorrectionState Light time state
 */
  const LightTimeCorrectionState &SpacecraftPosition::getLightTimeState() const {
    return (m_abcorr);
  }


}  // namespace Isis
