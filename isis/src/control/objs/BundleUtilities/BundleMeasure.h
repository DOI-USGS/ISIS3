#ifndef BundleMeasure_h
#define BundleMeasure_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QSharedPointer>

namespace Isis {
  class BundleControlPoint;
  class BundleImage;
  class BundleObservation;
  class BundleObservationSolveSettings;
  class Camera;
  class ControlMeasure;

/**
   * @brief A container class for a ControlMeasure.
   *
   * This class is used as a wrapper around a ControlMeasure to provide the necessary information
   * for BundleAdjust. This class can be used to get the parent bundle observation solve settings
   * for observation mode adjustment.
   *
   * Note that a BundleMeasure should be created from a non-ignored ControlMeasure.
   *
   * @ingroup ControlNetworks
   *
   * @author 2014-07-09 Ken Edmundson
   *
   * @internal
   *   @history 2015-02-20 Jeannie Backer - Added assignment operator. Brought closer to Isis
   *                           coding standards.
   *   @history 2016-07-14 Ian Humphrey - Updated documentation and coding standards in preparation
   *                           to merge into trunk. Updated unit test for BundleMeasure. Fixes
   *                           #4145, #4077.
   *   @history 2016-08-03 Jesse Mapel - Changed parent observation to a QSharedPointer.
   *                           Added error throws to observationSolveSettings and observationIndex
   *                           if calling when parent observation has not been set.  Fixes #4150.
   *   @history 2016-08-15 Ian Humphrey - Added sampleResidual(), lineResidual(),
   *                           residualMagnitude(), focalPlaneComputedX(), and
   *                           focalPlaneComputedY(). Modified isRejected() and camera() accessors
   *                           to be const. Updated unit test for these methods. References #4201.
   *   @history 2016-08-15 Jesse Mapel - Changed parent BundleImage to a QSharedPointer and added
   *                           a mutator.  Added wrapper methods for several ControlMeasure
   *                           methods.  Added typedef for BundleMeasureQsp.  Fixes #4159.
   *   @history 2016-08-18 Jesse Mapel - Changed to no longer inherit from QObject.  Fixes #4192.
   *   @history 2016-08-25 Adam Paquette - Updated documentation. Fixes #4299.
   */

  class BundleMeasure {

    public:
      // constructor
      BundleMeasure(ControlMeasure *controlMeasure, BundleControlPoint *bundleControlPoint);

      // copy constructor
      BundleMeasure(const BundleMeasure &src);

      // destructor
      ~BundleMeasure();

      BundleMeasure &operator=(const BundleMeasure &src);
      void setParentObservation(QSharedPointer<BundleObservation> observation);
      void setParentImage(QSharedPointer<BundleImage> image);
      void setRejected(bool reject);

      void setImage();
      void setNormalsPositionBlockIndex(int index);
      void setNormalsPointingBlockIndex(int index);
      void setFocalPlaneResidualsMillimeters();
      void setSigma(double sigma);
      bool isRejected() const;
      Camera *camera() const;
      BundleControlPoint *parentControlPoint();
      QSharedPointer<BundleImage> parentBundleImage();
      QSharedPointer<BundleObservation> parentBundleObservation();
      const QSharedPointer<BundleObservationSolveSettings> observationSolveSettings();

      double sample() const;
      double sampleResidual() const;
      double line() const;
      double lineResidual() const;
      double sigma() const;
      double weight() const;
      double weightSqrt() const;
      double residualMagnitude() const;
      double xFocalPlaneResidual() const;
      double yFocalPlaneResidual() const;
      QString cubeSerialNumber() const;
      double focalPlaneComputedX() const;
      double focalPlaneComputedY() const;
      double focalPlaneMeasuredX() const;
      double focalPlaneMeasuredY() const;
      int observationIndex() const;
      int positionNormalsBlockIndex() const;
      int pointingNormalsBlockIndex() const;

    private:
      ControlMeasure *m_controlMeasure;         /**< Contained control measure **/
      BundleControlPoint *m_parentControlPoint; /**< Parent bundle control point that contains this
                                                     bundle control measure **/
      QSharedPointer<BundleImage> m_parentBundleImage; /**< Parent image of this bundle control measure **/
      QSharedPointer<BundleObservation> m_parentObservation; /**< Parent bundle observation **/

      int m_normalsPositionBlockIndex; //!< block index into normal equations
      int m_normalsPointingBlockIndex; //!< block index into normal equations

      double m_xFocalPlaneResidual;    //!< x focal plane residual in mm
      double m_yFocalPlaneResidual;    //!< y focal plane residual in mm

      double m_sigma;                  //!< measure uncertainty in mm
      double m_weightSqrt;             //!< sqrt of measure weight
  };
  //! Definition for BundleMeasureQsp, a shared pointer to a BundleMeasure.
  typedef QSharedPointer<BundleMeasure> BundleMeasureQsp;
}

#endif // BundleMeasure_h
