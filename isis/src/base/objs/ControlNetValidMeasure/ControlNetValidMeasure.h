#ifndef _ControlNetValidMeasure_h_
#define _ControlNetValidMeasure_h_

#include "CubeManager.h"
#include "iString.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Progress.h"
#include "SerialNumberList.h"

#include <string.h>
/**
 * @file
 * $Revision: $
 * $Date: $
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
  
  class Camera;
  class Cube;
  class ControlNet;
  
   /**
    * @brief ControlNetValidMeasure class
    *
    * Base class to validate all the common Control Network options
    * specific to Control Network applications.
    *
    * @see cnetref autoseed etc.
    *
    * @internal
    *   @history 2010-05-11 Sharmila Prasad - Original Version
    *   @history 2010-06-21 Sharmila Prasad - Remove references to UniversalGroundMap and Cubes
    *                                         use CubeManager instead
    *   @history 2010-06-23 Sharmila Prasad - Added Pixels/Meters from the edge options
    *                                         and Validate Standard Options
    *   @history 2010-09-16 Sharmila Prasad - Renamed to ControlNetValidMeasure for uniformity with
    *                                         other ControlNet Classes
    */

  class ControlNetValidMeasure {
    public:
      ControlNetValidMeasure(Pvl *pvl = 0);
      ControlNetValidMeasure(Pvl &pvl);

      void InitStdOptions(void);         //!< Initialize the Standard Options
      void InitStdOptionsGroup(void);    //!< Initialize the Standard Options Pvl Group with no DefFile

      virtual ~ControlNetValidMeasure();

      void Parse(Pvl &pvlDef);           //!< Parse the DefFile for Standard Options

      //! Get the Pvl Log file
      virtual Pvl &GetLogPvl(void) {
        return mPvlLog;
      };

      //! Virtual Function to get better references for a Control Network based on Criteria
      virtual void FindCnetRef(const ControlNet &pOrigNet, ControlNet &pNewNet) {};

      bool ValidEmissionAngle(double pdEmissionAngle);                   //!< Validate whether the Emission Angle is in the set Range
      bool ValidIncidenceAngle(double pdIncidenceAngle);                 //!< Validate whether the Incidence Angle is in the set Range
      bool ValidDnValue(double pdDnValue);                               //!< Validate whether the DN Value is in the set Range
      bool ValidResolution(double pdResolution);                         //!< Validate whether the Resolution is in the set Range
      bool ValidLatLon(Isis::Camera *pCamera, int piSample, int piLine); //!< Validate the Lat/Lon

      //! Get the Standard Options Pvl Group
      PvlGroup &GetStdOptions(void) {
        return mStdOptionsGrp;
      };

      //! Get the Statistics Pvl Grp
      PvlGroup &GetStatistics(void) {
        return mStatisticsGrp;
      };

      //! Get the option MinDN
      double GetMinDN(void) {
        return mdMinDN;
      };

      //! Get the option MaxDN
      double GetMaxDN(void) {
        return mdMaxDN;
      };

      //! Get the option MinEmissionAngle
      double GetMinEmissionAngle(void) {
        return mdMinEmissionAngle;
      };

      //! Get the option MaxEmissionAngle
      double GetMaxEmissionAngle(void) {
        return mdMaxEmissionAngle;
      };

      //! Get the option MinIncidenceAngle
      double GetMinIncidenceAngle(void) {
        return mdMinIncidenceAngle;
      };

      //! Get the option MaxIncidenceAngle
      double GetMaxIncidenceAngle(void) {
        return mdMaxIncidenceAngle;
      };

      //! Get the option PixelsFromEdge
      double GetPixelsFromEdge(void) {
        return miPixelsFromEdge;
      };

      //! Get the option MetersFromEdge
      double GetMetersFromEdge(void) {
        return mdMetersFromEdge;
      };

      //! API to display location in the form "Sample,Line"
      iString LocationString(double pdSample, double pdLine) const {
        return iString((int)pdSample) + "," + iString((int)pdLine);
      };

      //! Test for a point to be user defined number of pixels from the edge
      bool PixelsFromEdge(int piSample, int piLine, Cube *pCube);

      //! Test for a point to be user defined number of meters from the edge
      bool MetersFromEdge(int piSample, int piLine, Cube *pCube);

      //! Validate Standard options to pick a reference based on a particular criteria
      bool ValidStandardOptions(double pdSample, double pdLine, Cube *pCube, PvlGroup *pMeasureGrp = NULL);

    protected:
      void ValidatePvlDN(void);                             //!< Validate PVL Min & Max DN Standard Options
      void ValidatePvlEmissionAngle(void);                  //!< Validate PVL Min & Max EmissionAngle Standard Options
      void ValidatePvlIncidenceAngle(void);                 //!< Validate PVL Min & Max IncidenceAngle Standard Options
      void ValidatePvlResolution(void);                     //!< Validate PVL Min & Max Resolution Standard Options
      void ValidatePvlFromEdge(void);                       //!< Validate and read Pixels and Meters from Edge Standard Options
      void ReadSerialNumbers(std::string psSerialNumfile);  //!< Read the Serial Numbers from the file and open assocaited cubes

      double mdMinDN;                     //!< Standard Option MinDN
      double mdMaxDN;                     //!< Standard Option MaxDN
      double mdMinResolution;             //!< Standard Option MinResolution
      double mdMaxResolution;             //!< Standard Option MaxResolution
      double mdMinEmissionAngle;          //!< Standard Option MinEmissionAngle
      double mdMaxEmissionAngle;          //!< Standard Option MaxEmissionAngle
      double mdMinIncidenceAngle;         //!< Standard Option MinIncidenceAngle
      double mdMaxIncidenceAngle;         //!< Standard Option MaxIncidenceAngle
      double mdMetersFromEdge;            //!< Standard Option MeteresFromEdge
      int miPixelsFromEdge;               //!< Standard Option PixelsFromEdge

      double mdEmissionAngle;             //!< Store current Measure's Emission Angle
      double mdIncidenceAngle;            //!< Store current Measure's Incidence Angle
      double mdResolution;                //!< Store current Measure's Resolution
      double mdDnValue;                   //!< Store current Measure's DN Value

      PvlGroup mPvlOpGrp;                 //!< Pvl Operator Group
      PvlGroup mStdOptionsGrp;            //!< Pvl Standard Options Group
      PvlGroup mStatisticsGrp;            //!< Pvl output Statistics Group
      Pvl mPvlLog;                        //!< Pvl Log - containing all the processing log
      Progress mStatus;                   //!< Monitor the Progress/status of the application running
      CubeManager mCubeMgr;               //!< CubeManager to open and read cubes
      SerialNumberList mSerialNumbers;    //!< Serial numbers lists read from the list file
  };
};
#endif

