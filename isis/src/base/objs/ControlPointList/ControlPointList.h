#ifndef ControlPointList_h
#define ControlPointList_h
/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2010/06/28 17:15:01 $
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
 *   $ISISROOT/doc/documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <string>
#include <vector>
#include <QList>
#include <QStringList>
#include "Pvl.h"

namespace Isis {
  /**
   * @brief Control Point List  generator
   *
   * Create a list of Control Points from a file with control
   * points ids
   *
   * @ingroup ControlNetworks
   *
   * @author  2009-08-11 Sharmila Prasad
   *
   * @internal
   *
   * @history 2009-08-11 Sharmila Prasad Original Version
   */

  class ControlPointList {
    public:
      ControlPointList(const std::string &psFileName);
      virtual ~ControlPointList();

      std::string ControlPointId(int piIndex);
      int ControlPointIndex(const std::string &psCpId);

      bool HasControlPoint(const std::string &psCpId);

      int Size() const;

      void RegisterStatistics(Pvl &pcPvlLog);

    private:
      QStringList mqCpList;
      std::vector<bool> mbFound;	 // holds one to one correspondence with "mqCpList" on
      // whether the point was valid
  };
};

#endif
