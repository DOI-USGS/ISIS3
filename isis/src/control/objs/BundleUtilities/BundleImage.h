#ifndef BundleImage_h
#define BundleImage_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QSharedPointer>

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
   *   @history 2014-02-20 Jeannie Backer - Added assignment operator. Updated
   *                           to better meet coding standards.
   *   @history 2016-06-27 Jesse Mapel - Updated documentation in preparation
   *                           for merging from IPCE to ISIS.  Fixes #4076.
   *   @history 2016-08-03 Jesse Mapel - Changed parent observation to a QSharedPointer.
   *                           Fixes #4150.
   *   @history 2016-08-18 Jesse Mapel - Changed to no longer inherit from QObject.  Fixes #4192.
   *
   */
  class BundleImage {

  public:
    // constructor
    BundleImage(Camera *camera, QString serialNumber, QString fileName);

    // copy constructor
    BundleImage(const BundleImage &other);

    // destructor
   ~BundleImage();

    BundleImage &operator=(const BundleImage &other);

    // mutators
    void setParentObservation(QSharedPointer<BundleObservation> parentObservation);

    // accessors
    Camera *camera();
    QSharedPointer<BundleObservation> parentObservation();
    QString serialNumber();
    QString fileName();

    private:
      Camera *m_camera; //!< The camera model for the image
      QSharedPointer<BundleObservation> m_parentObservation; //!< parent BundleObservation
      QString m_serialNumber; //!< The serial number for the image
      QString m_fileName; //!< The file name of the image
  };

  //!< Definition for a BundleImageQsp, a shared pointer to a BundleImage.
  typedef QSharedPointer<BundleImage> BundleImageQsp;
}

#endif // BundleImage_h
