#ifndef BundleImage_h
#define BundleImage_h

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

#include "SerialNumber.h"

namespace Isis {

  class BundleObservation;
  class Camera;

  /**
   * This class hold image information that BundleAdjust needs to run correctly.
   * 
   * @author 2014-05-22 Ken Edmundson
   *
   * @internal
   *   @history 2014-05-22 Ken Edmundson - Original Version
   *   @history 2014-07-17 Kimberly Oyama - Updated to better meet coding standards.
   *   
   */
  class BundleImage : QObject {

    Q_OBJECT

  public:
    // constructor
    BundleImage(Camera* camera, QString serialNumber, QString fileName);

    // destructor
   ~BundleImage();

    // copy constructor
    BundleImage(const BundleImage &src);

    // mutators
    void setParentObservation(BundleObservation* parentObservation);

    // accessors
    Camera* camera();
    BundleObservation* parentObservation();
    QString serialNumber();
    QString fileName();

    private:
      Camera* m_camera;
      BundleObservation* m_parentObservation; //!< parent BundleObservation
      QString m_serialNumber;
      QString m_fileName;      
  };
}

#endif // BundleImage_h
