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
  class MeasureValidationResults;
  class ControlMeasure;
  
   /**
    * @brief ControlNetValidMeasure class
    *
    * Base class to validate all the common Control Network options
    * specific to Control Network applications.
    *
    * @see cnetref autoseed etc.
    *
    * @internal
    *  @history 2010-05-11 Sharmila Prasad - Original Version
    *  @history 2010-06-21 Sharmila Prasad - Remove references to UniversalGroundMap & 
    *                                        Cubes use CubeManager instead
    *  @history 2010-06-23 Sharmila Prasad - Added Pixels/Meters from the edge options
    *                                        and Validate Standard Options
    *  @history 2010-09-16 Sharmila Prasad - Renamed to ControlNetValidMeasure for  
    *                                        uniformity with other ControlNet Classes
    *  @history 2010-10-14 Sharmila Prasad - Use only a single copy of Control Net
    *  @history 2010-11-10 Sharmila Prasad - Change group name of DefFile from "Operator" to
    *                                        "ValidMeasure"
    *  @history 2011-05-17 Sharmila Prasad - Added Sample, Line Residuals and Residual Magnitude
    *                                        for validation
    *  @history 2011-05-19 Sharmila Prasad - Flag to indicate whether Camera is required, to increase
    *                                        the processing speed
    *  @history 2011-06-06 Sharmila Prasad - Process the options only is specified in the DefFile to
    *                                        improve run time
    */

  class ControlNetValidMeasure {
    public:
      ControlNetValidMeasure(Pvl *pvl = 0);
      ControlNetValidMeasure(Pvl &pvl);

      //! Initialize the Standard Options
      void InitStdOptions(void);         
      
      //! Initialize the Standard Options Pvl Group with no DefFile
      void InitStdOptionsGroup(void);    

      virtual ~ControlNetValidMeasure();

      //! Parse the DefFile for Standard Options
      void Parse(Pvl &pvlDef);

      //! Get the Pvl Log file
      virtual Pvl &GetLogPvl(void) {
        return mPvlLog;
      };

      //! Virtual Function to get better references for a Control Network based on Criteria
      virtual void FindCnetRef(ControlNet &pNewNet) {};

      //! Validate whether the Emission Angle is in the set Range
      bool ValidEmissionAngle(double pdEmissionAngle);
      
      //! Validate whether the Incidence Angle is in the set Range
      bool ValidIncidenceAngle(double pdIncidenceAngle);
      
      //! Validate whether the DN Value is in the set Range
      bool ValidDnValue(double pdDnValue);
      
      //! Validate whether the Resolution is in the set Range
      bool ValidResolution(double pdResolution);
      
      //! Validate whether the Residuals are within the set Tolerance
      bool ValidResidualTolerances(double pdSampleResidual, double pdLineResidual, 
                     double pdResidualMagnitude, MeasureValidationResults & pResults);

      bool ValidShiftTolerances(double sampleShift, double lineShift,
          double pixelShift, MeasureValidationResults &results);
      
      //! Validate the Lat/Lon
      bool ValidLatLon(Isis::Camera *pCamera, int piSample, int piLine); 

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
      MeasureValidationResults ValidStandardOptions(const ControlMeasure *pMeasure,
          Cube *pCube, PvlGroup *pMeasureGrp = NULL);

      //! Validate Standard options to pick a reference based on a particular criteria
      MeasureValidationResults ValidStandardOptions(const ControlMeasure *pMeasure,
          Cube *pCube, Camera *camera, PvlGroup *pMeasureGrp = NULL);
      
      //! Validate Standard options to pick a reference based on a particular criteria
      MeasureValidationResults ValidStandardOptions(double pSample, double pLine, 
          const ControlMeasure *pMeasure, Cube *pCube, PvlGroup *pMeasureGrp = NULL);

      MeasureValidationResults ValidStandardOptions(double pSample, double pLine, 
          const ControlMeasure *pMeasure, Cube *pCube, Camera *measureCamera,
          PvlGroup *pMeasureGrp = NULL);
      
      //! Validate Standard options to pick a reference based on a particular criteria
      MeasureValidationResults ValidStandardOptions(double pSample, double pLine, 
                                          Cube *pCube, PvlGroup *pMeasureGrp = NULL);

      bool IsCubeRequired() {
        return IsCameraRequired() || mbValidateDN || mbValidateFromEdge;
      }

      /**
       * API to get status of CameraRequired flag
       * 
       * @author Sharmila Prasad (5/19/2011)
       * 
       * @return bool 
       */
      bool IsCameraRequired() {
        return mbCameraRequired;
      }

    protected:
      //! Validate PVL Min & Max DN Standard Options
      void ValidatePvlDN(void);
      
      //! Validate PVL Min & Max EmissionAngle Standard Options
      void ValidatePvlEmissionAngle(void);
      
      //! Validate PVL Min & Max IncidenceAngle Standard Options
      void ValidatePvlIncidenceAngle(void);
      
      //! Validate PVL Min & Max Resolution Standard Options
      void ValidatePvlResolution(void);
      
      //! Validate and read Pixels and Meters from Edge Standard Options
      void ValidatePvlFromEdge(void);
      
      //! Validate Pvl Sample, Line, Residual Magnitude Tolerances
      void ValidatePvlResidualTolerances(void);

      void ValidatePvlShiftTolerances();
      
      //! Read the Serial Numbers from the file and open assocaited cubes
      void ReadSerialNumbers(std::string psSerialNumfile);
      
      /**
       * Set the CameraRequired Flag. This flag indicates whether a camera is required 
       * to Validate a Control Measure. Camera is required to get Emission, Incidence 
       * angles and Resolution 
       * 
       * @author Sharmila Prasad (5/19/2011)
       * 
       * @param pbFlag 
       */
      void SetCameraRequiredFlag(bool pbFlag){
        mbCameraRequired = pbFlag;
      }
      
      double mdMinDN;                  //!< Standard Option MinDN
      double mdMaxDN;                  //!< Standard Option MaxDN
      double mdMinResolution;          //!< Standard Option MinResolution
      double mdMaxResolution;          //!< Standard Option MaxResolution
      double mdMinEmissionAngle;       //!< Standard Option MinEmissionAngle
      double mdMaxEmissionAngle;       //!< Standard Option MaxEmissionAngle
      double mdMinIncidenceAngle;      //!< Standard Option MinIncidenceAngle
      double mdMaxIncidenceAngle;      //!< Standard Option MaxIncidenceAngle
      double mdMetersFromEdge;         //!< Standard Option MeteresFromEdge
      int miPixelsFromEdge;            //!< Standard Option PixelsFromEdge
      double mdSampleResTolerance;     //!< Standard Option Sample Residual
      double mdLineResTolerance;       //!< Standard Option Line Residual
      double mdResidualTolerance;      //!< Standard Option Residual Magnitude

      double m_sampleShiftTolerance;   //!< Standard Option Sample Shift
      double m_lineShiftTolerance;     //!< Standard Option Line Shift
      double m_pixelShiftTolerance;    //!< Standard Option Pixel Shift

      double mdEmissionAngle;          //!< Store current Measure's Emission Angle
      double mdIncidenceAngle;         //!< Store current Measure's Incidence Angle
      double mdResolution;             //!< Store current Measure's Resolution
      double mdDnValue;                //!< Store current Measure's DN Value
      double mdSampleResidual;         //!< Store current Measure's Sample Residual
      double mdLineResidual;           //!< Store current Measure's Line Residual
      double mdResidualMagnitude;      //!< Store current Measure's Residual Magnitude

      double m_sampleShift;            //!< Store current Measure's Sample Shift
      double m_lineShift;              //!< Store current Measure's Line Shift
      double m_pixelShift;             //!< Store current Measure's Pixel Shift

      PvlGroup mPvlOpGrp;              //!< Pvl Operator Group
      PvlGroup mStdOptionsGrp;         //!< Pvl Standard Options Group
      PvlGroup mStatisticsGrp;         //!< Pvl output Statistics Group
      Pvl mPvlLog;                     //!< Pvl Log of all the processing
      Progress mStatus;                //!< Monitor the status of the app
      CubeManager mCubeMgr;            //!< CubeManager to open and read cubes
      SerialNumberList mSerialNumbers; //!< Serial numbers list

      bool mbCameraRequired;           //!< To improve speed, flag to indicate if 
                                       //!< Camera needs to be opened
      bool mbValidateDN;               //!< Check if DN needs to be Validated
      bool mbValidateFromEdge;         //!< Check if Pixels/Meters from edge needs to be Validated
  };
};
#endif

