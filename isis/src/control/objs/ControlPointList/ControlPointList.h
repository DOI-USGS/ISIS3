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

#include <QStringList>
#include <QVector>

#include "Pvl.h"

namespace Isis {
  class FileName;

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
   *   @history 2009-08-11 Sharmila Prasad Original Version
   *   @history 2009-10-04 Steven Lambright Constructor now takes a FileName
   *            instead of a string.
   *   @history 2017-12-12 Kristin Berry Updated to use QVector and QString instead of std::vector
   *            and std::string.
   */
  class ControlPointList {
    public:
      ControlPointList(const FileName &psFileName);
      virtual ~ControlPointList();

      QString ControlPointId(int piIndex);
      int ControlPointIndex(const QString &psCpId);

      bool HasControlPoint(const QString &psCpId);

      int Size() const;

      void RegisterStatistics(Pvl &pcPvlLog);

    private:
      QStringList mqCpList;

      //! holds one to one correspondence with "mqCpList" on
      //! whether the point was valid
      QVector<bool> mbFound; 
  };
};

#endif
