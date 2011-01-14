#ifndef ControlSerialNumber_h
#define ControlSerialNumber_h

/**
 * @file
 * $Revision: 1.8 $
 * $Date: 2008/06/18 18:54:11 $
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

#include "SerialNumber.h"

template < typename T > class QList;

namespace Isis {

  class ControlMeasure;

  /**
   * @brief Serial Number with added functionality for Control Networks
   *
   * This class is extends the Serial Number class in order to directly point
   * between its associated measures in a Control Network and back.
   *
   * @ingroup ControlNetwork
   *
   * @author 2011-01-14 Travis Addair and Christopher Austin
   *
   * @see ControlPoint ControlMeasure
   *
   * @internal
   *   @history 2011-01-14 Travis Addair and Christopher Austin - original
   *                                                              version
   *
   */

  class ControlSerialNumber : SerialNumber {
    public:
      ControlSerialNumber();
      virtual ~ControlSerialNumber();

      void AddMeasure(ControlMeasure * measure);
      void RemoveMeasure(ControlMeasure * measure);
      void RemoveMeasure(int index);

    private:
      QList<ControlMeasure *> * measures;

  }; // End of Class
}; // End of namespace

#endif
