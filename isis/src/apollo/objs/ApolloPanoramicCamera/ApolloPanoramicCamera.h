#ifndef ApolloPanoramicCamera_h
#define ApolloPanoramicCamera_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */                                                                     

#include "LineScanCamera.h"

namespace Isis {
  class PvlGroup;
    /**
     * @brief Apollo Panoramic Camera
     *
     * Description: Geometric camera model for the Apollo Panoramic Camera
     *
     * @ingroup Apollo
     *
     * @author 2011-09-19 Orrin Thomas
     *
     * @internal
     *   @history 2011-09-19 Orrin Thomas - Original version
     *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis
     *                           coding standards. References #972.
     *   @history 2012-07-10 Orrin Thomas - Updated to current coding standards
     *   @history 2015-09-01 Ian Humphrey and Makayla Shepherd - Added new data members and
     *                           methods to get spacecraft and instrument names.
     *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument
     *                           members and methods and removed implementation of these methods
     *                           since Camera now handles this. References #2335. Fixed
     *                           indentation.
     *   @history 2017-06-28 Makayla Shepherd - Updated documentation. References #4807.
     */
    class ApolloPanoramicCamera : public LineScanCamera {
    public:
      ApolloPanoramicCamera(Cube &lab);

      //! Destorys the ApolloPanoramicCamera object
      ~ApolloPanoramicCamera() {};

      /**
      * CK frame ID -  - Instrument Code
      *
      * @return @b int The appropriate instrument code for the "Camera-matrix"
      *         Kernel Frame ID
      */
      //this sensor was used on multiple missions so it is necessary to check which Apollo
      virtual int CkFrameId() const {return m_CkFrameId; }

      /**
      * CK Reference ID - J2000
      *
      * @return @b int The appropriate instrument code for the "Camera-matrix"
      *         Kernel Reference ID
      */
      virtual int CkReferenceId() const { return (1); }

      /**
      *  SPK Reference ID - J2000
      *
      * @return @b int The appropriate instrument code for the Spacecraft
      *         Kernel Reference ID
      */
      virtual int SpkReferenceId() const { return (1); }

      /**
       * Returns residual summary statistics from interior orientation as a PvlGroup
       *
       * @return PvlGroup Residuals report.
       */
      PvlGroup intOriResidualsReport();

      /**
       * Max interior orientation residual vector length, accesor
       *
       * @return double Residual max
       */
      double intOriResidualMax()   const { return m_residualMax; }

      /**
       * Mean (average) of interior orientation residual vector length, accesor
       *
       * @return double Residual mean
       */
      double intOriResidualMean()  const { return m_residualMean; }

      /**
       * Standard deviation of interior orientation residual vector length, accesor
       *
       * @return double Residual standard deviation
       */
      double intOriResidualStdev() const { return m_residualStdev; }

    private:
      //! Max interior orientation residual vector length
      double m_residualMean;

      //! Mean (average) of interior orientation residual vector length
      double m_residualMax;

      //! Standard deviation of interior orientation residual vector length
      double m_residualStdev;

      //! CK "Camera Matrix" kernel frame ID
      int m_CkFrameId;
    };
};

#endif
