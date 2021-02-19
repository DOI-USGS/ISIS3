#ifndef LightTimeCorrectionState_h
#define LightTimeCorrectionState_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
