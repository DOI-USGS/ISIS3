#ifndef LightTimeCorrectionState_h
#define LightTimeCorrectionState_h
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

#include <QString>

namespace Isis {

  class Spice;

/**
 * @brief Provides interface to user configurable Light Time correction feature 
 *  
 * This class provides a comprehensive container for the state of the 
 * observer/target swapping to correct a long term issue that applied the wrong 
 * order of observer/target to the NAIF spkez_c/spkezp_c routines that provide 
 * body state vectors. 
 *  
 * In addition, it also contains parameters that provide determination of the 
 * type and extent of stellar aberration and light time correction to the 
 * surface of the target body.  This is a recent addition to address accuracy 
 * issues in determining these vectors. 
 *  
 * @see Spice 
 * @see SpacecraftPosition 
 * @see SpicePosition 
 *  
 * @ingroup SpiceInstrumentsAndCameras 
 *  
 * @author 2012-10-11 Kris Becker 
 *  
 * @internal 
 * @history 2012-10-31 Kris Becker - New class iprovides support for swapping
 *          of observer/target and light time correction to surface. Fixes
 *          (mostly) #0909, #1136 and #1223.
 * @history 2012-11-01 Kris Becker - Fixed isLightTimeCorrected() as it was
 *          returning exactly opposite what it should be.  References #1136.
 * @history 2012-12-04 Kris Becker - Corrected documentation 
 * @history 2015-07-21 Kristin Berry - Added NaifStatus::CheckErrors() to see if any NAIF errors 
 *          were signaled. References #2248.
 *                          
 */
  class LightTimeCorrectionState  {
    public:

      LightTimeCorrectionState();
      LightTimeCorrectionState(int ikCode, Spice *spice);

     // destructor
      virtual ~LightTimeCorrectionState() {  }

      bool operator==(const LightTimeCorrectionState &state) const;

      bool checkAberrationCorrection(int ikCode, Spice *spice);
      void setAberrationCorrection(const QString &correction);
      QString getAberrationCorrection() const;
      bool checkSpkKernelsForAberrationCorrection();
      bool isLightTimeCorrected() const;

      bool checkObserverTargetSwap(int ikCode, Spice *spice);
      bool isObserverTargetSwapped() const;
      void setSwapObserverTarget();
      void setNoSwapObserverTarget();

      bool checkLightTimeToSurfaceCorrect(int ikCode, Spice *spice);
      bool isLightTimeToSurfaceCorrected() const;
      void setCorrectLightTimeToSurface();
      void setNoCorrectLightTimeToSurface();

    private:
      QString m_abcorr;
      bool m_swapObserverTarget;
      bool m_sc_to_surf_ltcorr;

      void setDefaultState();
  };
}  // Isis namespace
#endif 
