
#ifndef ObservationNumber_h
#define ObservationNumber_h

/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2008/05/09 16:30:19 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <string>

#include <QMutex>

#include "SerialNumber.h"
#include "SerialNumberList.h"

namespace Isis {
  class Pvl;
  class PvlGroup;
  class Cube;

  /**
   * @brief Serial Number composer
   *
   * A Serial Number is a unique identification for some object such as an
   * Isis cube. A Serial Number for a specific object must be repeatable. This
   * class is intended to be used to create all Serial Numbers for Isis.
   *
   * @ingroup ControlNetworks
   *
   * @author  2008-??-?? Christopher Austin
   *
   * @internal
   *
   *  @todo This is only a temporary version. The code needs to be modified
   *  to use a PVL file to determine which keywords to use to create the
   *  Serial Number
   *
   *  @history 2008-01-08 Christpher Austin - Original Version,
   *           a derivative of the previous SerialNumber class
   *
   *  @history 2008-05-09 Steven Lambright - Optimized the
   *           FindObservationTranslation method
   *  @history 2018-07-11 Ian Humphrey - Made FindObservationTranslation thread-safe. Added a
   *                          static mutex for the FindObservationTranslation() method. This
   *                          faciliates adding a QString observationNumber member to the Image
   *                          class and calling ObservationNumber::Compose() within the Image
   *                          constructors. When qmos or ipce opens a list of images, it uses an
   *                          ImageReader, which uses a QtConcurrent mapped function to load its
   *                          images. Since the FindObservationTranslation() method declares its
   *                          static local variables within in, this could cause thread collisions
   *                          on these static variables when ObservationNumbers are being composed
   *                          for the Images being conucrrently opened by ImageReader. Added a
   *                          QMutexLoccker to the FindObservationTranslation() method to auto-lock
   *                          and unlock the mutex, which prevents multiple threads from colliding
   *                          on the static variables. References #5206.
   *
   */
  class ObservationNumber : public Isis::SerialNumber {
    public:
      ObservationNumber();

      virtual ~ObservationNumber();

      static QString Compose(Pvl &label, bool def2filename = false);

      static QString Compose(Cube &cube, bool def2filename = false);

      static QString Compose(const QString &filename, bool def2filename = false);

      std::vector<QString> PossibleSerial(const QString &on, SerialNumberList &list);

    private:

      static PvlGroup FindObservationTranslation(Pvl &label);

      /**
       * Static mutex for preventing threads from colliding on the static variables in
       * FindObservationTranslation().
       */
      static QMutex m_mutex;

  }; // End of Class
}; // End of namespace

#endif
