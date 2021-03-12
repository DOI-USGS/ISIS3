#ifndef _CnetRefByIncidence_h_
#define _CnetRefByIncidence_h_

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */


#include "ControlNetValidMeasure.h"

namespace Isis {
  class ControlNet;
  class Pvl;

  /**
   * @brief Find a Reference in Control Point with the least Incidence Angle
   *
   * This class is used find a Reference in Control Point with the least Incidence Angle
   * after it has passed all the validity test for Dn, Emission & Incidence Angles
   * and Resolution. Processes the entire points in the Control Network
   *
   * @ingroup ControlNetwork
   *
   * @author 2010-06-10 Sharmila Prasad
   *
   * @see ControlNetValidMeasure, CnetRefByEmission, CnetRefByResolution
   *
   * @internal
   *   @history 2010-06-10 Sharmila Prasad - Original version
   *   @history 2010-06-21 Sharmila Prasad - Remove references to UniversalGroundMap and
   *                                         Cubes use CubeManager instead
   *   @history 2010-06-23 Sharmila Prasad - Use ControlNetValidMeasure's Validate Standard
   *                                         Options & Std Options Pixels/Meters from Edge
   *   @history 2010-09-16 Sharmila Prasad - Renamed CnetValidMeasure to ControlNetValidMeasure
   *   @history 2010-10-06 Sharmila Prasad - Modified to incorporate Binary Control Network
   *   @history 2010-10-15 Sharmila Prasad - Use single copy of Control Net in FindCnetRef()
   *   @history 2010-10-22 Sharmila Prasad - Reset apriori for source==Reference
   *   @history 2010-12-29 Sharmila Prasad - Modified for new ControlNet API's
   *                                         (UpdatePoint, UpdateMeasure)
   *   @history 2011-01-19 Christopher Austin - Altered to compile with the new
   *                                            Control redesign.
   *   @history 2011-02-24 Sharmila Prasad - Fixed segmentation fault
   *   @history 2011-06-07 Debbie A. Cook and Tracie Sucharski - Modified point types
   *                                            Ground ------> Fixed
   *                                            Tie----------> Free
   */
  class CnetRefByIncidence : public ControlNetValidMeasure {
    public:
      //! Constructor
      CnetRefByIncidence(Pvl *pPvlDef, QString msSerialNumfile);
      virtual ~CnetRefByIncidence() {};

      //! Get the Log updated by this object
      virtual Pvl &GetLogPvl(void) {
        return mPvlLog;
      };

      //! Given Control Net, find the reference based on Incidence Angle
      virtual void FindCnetRef(ControlNet &pNewNet);

    private:
  };
};
#endif
