#ifndef ControlPointList_h
#define ControlPointList_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
   *            and std::string. Fixes #5259.
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
