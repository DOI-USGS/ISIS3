#ifndef _CnetRefByEmission_h_
#define _CnetRefByEmission_h_

#include "ControlNetValidMeasure.h"

/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2010/06/23 22:29:14 $
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

namespace Isis {
  /**
  * @brief Find a Reference in Control Point with the least Emission Angle
  *
  * This class is used find a Reference in Control Point with the least Emission Angle
  * after it has passed all the validity test for Dn, Emission & Incidence Angles
  * and Resolution. Processes the entire points in the Control Network
  *
  * @ingroup ControlNetwork
  *
  * @author 2010-06-10 Sharmila Prasad
  *
  * @see ControlNetValidMeasure, CnetRefByIncidence, CnetRefByResolution
  *
  * @internal
  *   @history 2010-06-10 Sharmila Prasad - Original version
  *   @history 2010-06-21 Sharmila Prasad - Remove references to UniversalGroundMap and Cubes
  *                                         use CubeManager instead
  *   @history 2010-06-23 Sharmila Prasad - Use ControlNetValidMeasure's Validate Standard Options &
  *                                         Std Options Pixels/Meters from Edge
  *   @history 2010-09-16 Sharmila Prasad - Renamed CnetValidMeasure to ControlNetValidMeasure
  */

  class Pvl;
  class ControlNet;

  class CnetRefByEmission : public ControlNetValidMeasure {
    public:
      //! Constructor
      CnetRefByEmission(Pvl *pPvlDef, std::string msSerialNumfile);
      virtual ~CnetRefByEmission() {};

      //! Get the Log updated by this object
      virtual Pvl &GetLogPvl(void) {
        return mPvlLog;
      };

      //! Given Control Net, find the reference based on Emission Angle
      virtual void FindCnetRef(const ControlNet &pOrigNet, ControlNet &pNewNet);
  };
};
#endif

