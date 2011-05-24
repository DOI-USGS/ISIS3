#ifndef _CnetRefByResolution_H_
#define _CnetRefByResolution_H_

#include "ControlNetValidMeasure.h"

/**
 * @file
 * $Revision: 1.5 $
 * $Date:
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
  * @brief Find a Reference in Control Point based on Resolution
  *
  * This class is used find a Reference in Control Point based on Resolution
  * after it has passed all the validity test for Dn, Emission & Incidence Angles
  * and Resolution. Processes the entire points in the Control Network
  *
  * Resolution choices could be based on highest, lowest Resolutions or Measure
  * whose resolution is closest to mean of all the resolutions, Measure whose
  * resolution is closest to user defined value or Measure whose resolution
  * is in a given range
  *
  * @ingroup ControlNetwork
  *
  * @author 2010-06-10 Sharmila Prasad
  *
  * @see ControlNetValidMeasure, CnetRefByEmission, CnetRefByIncidence
  *
  * @internal
  *   @history 2010-06-10 Sharmila Prasad - Original version
  *   @history 2010-06-21 Sharmila Prasad - Remove references to UniversalGroundMap and Cubes
  *                                         use CubeManager instead
  *   @history 2010-06-23 Sharmila Prasad - Use ControlNetValidMeasure's Validate Standard Options &
  *                                         Std Options Pixels/Meters from Edge
  *   @history 2010-09-16 Sharmila Prasad - Renamed CnetValidMeasure to ControlNetValidMeasure
  *   @history 2010-10-06 Sharmila Prasad - Modified to incorporate Binary Control Network
  *   @history 2010-10-15 Sharmila Prasad - Use single copy of Control Net in FindCnetRef()
  *   @history 2010-10-22 Sharmila Prasad - Reset apriori for source==Reference
  *   @history 2010-12-29 Sharmila Prasad - Modified for new ControlNet API's
  *                                         (UpdatePoint, UpdateMeasure)
   *  @history 2011-01-19 Christopher Austin - Altered to compile with the new
  *                                             Control redesign.
  *   @history 2011-02-24 Sharmila Prasad - Fixed segmentation fault
  */

  /**
   * Enumeration containing different Resolution Types
   */
  enum ResolutionType {Low, High, Mean, Nearest, Range};

  class Pvl;
  class ControlNet;
  class ControlPoint;

  class CnetRefByResolution : public ControlNetValidMeasure {
    public:
      //! Constructor
      CnetRefByResolution(Pvl *pPvlDef, std::string psSerialNumfile, ResolutionType peType = Low,
          double pdResValue = 0, double pdMinRes = 0, double pdMaxRes = 0);
      virtual ~CnetRefByResolution() {};

      //! Get the Log updated by this object
      virtual Pvl &GetLogPvl(void) {
        return mPvlLog;
      };

      //! Get the Mean of all the Resolution in a Point
      double GetMeanResolution(void);

      //! Given Control Net, find the reference based on Resolution
      virtual void FindCnetRef(ControlNet &pNewNet);

    private:
      //! Get the Reference based on Resolution type
      int GetReferenceByResolution(ControlPoint *pNewPoint);

      std::vector<double> mdResVector; //!< Store the Resolutions of all Measures in a Point
      ResolutionType meType;           //!< Resolution Type - Low, Mean, High, Value, Range
      double mdResValue;               //!< Resolution value for Type=Value
      double mdMinRes;                 //!< Min Resolution for Type=Range
      double mdMaxRes;                 //!< Max Resolution for Type=Range
  };
};
#endif

