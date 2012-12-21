#ifndef SpacecraftPosition_h
#define SpacecraftPosition_h
/**
 * @file
 * $Revision$
 * $Date$
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
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

#include <string>
#include <vector>
#include "Distance.h"
#include "LightTimeCorrectionState.h"
#include "SpicePosition.h"

namespace Isis {

/**
 * @brief Provides swap observer/target and improved light time correction
 *  
 * The process by which ISIS has determined the position of the spacecraft 
 * w.r.t a target body is by utilizing the NAIF spkez_c/spkezp_c routines. 
 * Recently it has been determined that the parameters for observer (or 
 * spacecraft) and target (typically a planet) has been swapped.  This results 
 * in a slightly different location of the s/c.  This class provides 
 * programmers with a way to swap these parameters at runtime by providing a 
 * different instantiation option. 
 *  
 * This implementation was chosen to mostly hide this option as it the full 
 * impact of this chage is still being evaluated for all supported instruments 
 * in ISIS. 
 *  
 * This implementation provides the ability to swap observer/target parameters 
 * selectively as deemed appropriate by API developers.  See the Spice class 
 * for how this class is being utilized. 
 *  
 * In addition, this class provides the ability to correct for stellar 
 * aberration and light time to the target body surface (via a reimplementation 
 * of SetEphemerisSpiceTime()).  This mostly fixes the problem of accurate 
 * light time correction.  What remains is applying this fix on a per pixel 
 * basis.  It is most accurate at the subspacecraft lat/lon point on the target
 * body surface.   
 *  
 * @see Spice 
 * @see LightTimeCorrectionState
 * @see SpicePosition 
 *  
 * @ingroup SpiceInstrumentsAndCameras 
 *  
 * @author 2012-10-11 Kris Becker
 *  
 * @internal  
 * @history 2012-10-31 Kris Becker - New class implements swapping of 
 *          observer/target and light time correction to surface.  Fixes
 *          (mostly) #0909, #1136 and #1223.
 * @history 2012-11-01 Kris Becker - Revised parameter order to computeStateVector
 *                                   to match comments.  References #1136.          
 * @history 2012-12-04 Kris Becker - Corrected documentation 
 */
  class SpacecraftPosition : public SpicePosition {
    public:

      SpacecraftPosition(int targetCode, int observerCode,
                         const LightTimeCorrectionState &ltState = LightTimeCorrectionState(),
                         const Distance &radius = Distance(0.0, Distance::Meters));


      // destructor
      virtual ~SpacecraftPosition() {   }

      double getRadiusLightTime() const;
      static double getDistanceLightTime(const Distance &distance);

      virtual void SetAberrationCorrection(const QString &correction);
      virtual QString GetAberrationCorrection() const;

      virtual void SetEphemerisTimeSpice();

      const LightTimeCorrectionState &getLightTimeState() const;

    private:
      LightTimeCorrectionState m_abcorr;   //!< Light time correction state
      Distance                 m_radius;   //!< Radius of target
  };


}  // Isis namespace
#endif 
