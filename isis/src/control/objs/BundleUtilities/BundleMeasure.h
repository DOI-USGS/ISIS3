#ifndef BundleMeasure_h
#define BundleMeasure_h
/**
 * @file
 * $Revision: 1.20 $
 * $Date: 2014/5/22 01:35:17 $
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

#include <QObject>

namespace Isis {
  /**
   * @brief 
   *  
   * @ingroup ControlNetworks
   *
   * @author 2014-07-09 Ken Edmundson
   *
   * @internal
   *   @history 2015-02-20 Jeannie Backer - Added assignment operator. Brought closer to Isis coding
   *                           standards.
   *
   */

  class BundleControlPoint;
  class BundleImage;
  class BundleObservation;
  class BundleObservationSolveSettings;
  class Camera;
  class ControlMeasure;

  class BundleMeasure : QObject {

    Q_OBJECT

    public:
      // constructor
      BundleMeasure(ControlMeasure *controlMeasure, BundleControlPoint *bundleControlPoint);

      // copy constructor
      BundleMeasure(const BundleMeasure &src);

      // destructor
      ~BundleMeasure();

      BundleMeasure &operator=(const BundleMeasure &src);
      void setParentObservation(BundleObservation *observation);

      bool isRejected();
      Camera *camera();
      BundleControlPoint *parentControlPoint();
      BundleImage *parentBundleImage();
      BundleObservation *parentBundleObservation();
      const BundleObservationSolveSettings *observationSolveSettings();

      double sample() const;
      double line() const;
      QString cubeSerialNumber() const;
      double focalPlaneMeasuredX() const;
      double focalPlaneMeasuredY() const;
      int observationIndex() const;

    private:
      ControlMeasure *m_controlMeasure;
      BundleControlPoint *m_parentControlPoint;
      BundleImage *m_parentBundleImage;
      BundleObservation *m_parentObservation;
  };
}

#endif // BundleMeasure_h
