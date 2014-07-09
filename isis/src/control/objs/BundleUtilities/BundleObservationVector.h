#ifndef BundleObservationVector_h
#define BundleObservationVector_h
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

#include <QMap>
#include <QVector>

#include "BundleImage.h"

namespace Isis {

  class BundleObservation;
  class BundleSettings;

  class BundleObservationVector : public QVector < BundleObservation* > {

    public:
      BundleObservationVector();
      ~BundleObservationVector();

      BundleObservation* addnew(BundleImage* image, QString observationNumber,
                                QString instrumentId, BundleSettings& bundleSettings);

      int numberPositionParameters();
      int numberPointingParameters();
      int numberParameters();

      BundleObservation* getObservationByCubeSerialNumber(QString cubeSerialNumber);

      bool initializeExteriorOrientation();

  private:
      QMap<QString, BundleObservation*> m_observationNumberToObservationMap; //!< map between
                                                                             //!< observation # and
                                                                             //!< ptr to observation

      QMap<QString, BundleObservation*> m_imageSerialToObservationMap;       //!< map between
                                                                             //!< image serial # &
                                                                             //!< vector index
  };
}

#endif // BundleObservationVector_h
