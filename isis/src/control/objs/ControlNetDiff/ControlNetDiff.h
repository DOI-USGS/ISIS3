#ifndef ControlNetDiff_h
#define ControlNetDiff_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QString>

template< typename A, typename B > class QMap;
template< typename T > class QSet;

namespace Isis {
  class ControlNet;
  class FileName;
  class Pvl;
  class PvlContainer;
  class PvlKeyword;
  class PvlObject;

  /**
   * @brief Compares two Control Networks and reports their differences
   *
   * This class opens two Control Networks from Filenames and returns their
   * differences as a PVL structure.  It is generally stateless, able to compare
   * multiple networks in succession without needing to reset anything.
   * However, the tolerances that are added are persistent.
   *
   * Differences will be reported as PvlKeywords with two to three values.  The
   * first value is from the first network, the second from the second network,
   * and the third the failed tolerance (if provided).  Control Point PVL
   * Objects and Control Measure PVL Groups will only be reported if there are
   * differences, or they appear in only one of the two networks.  The number of
   * points and measures will similarly only be reported if they differ.  The
   * tolerances are stored in a PVL structure containing an IgnoreKeys group
   * with keywords to ignore completely, and a Tolerances group to ignore if the
   * (numerical) values are different within the given tolerance.
   *
   * @ingroup ControlNetwork
   *
   * @author 2012-04-25 Travis Addair
   *
   * @internal
   *   @history 2012-04-26 Travis Addair - Added documentation.
   *   @history 2018-01-02 Kristin Berry - Modified to use ControlNetVersioner instead of
   *                           LatestControlNetFile.
   *
   */
  class ControlNetDiff {
    public:
      ControlNetDiff();
      explicit ControlNetDiff(Pvl &diffFile);
      virtual ~ControlNetDiff();

      void addTolerances(Pvl &diffFile);
      Pvl compare(FileName &net1Name, FileName &net2Name);


    protected:
      void compare(PvlObject &point1Pvl, PvlObject &point2Pvl, PvlObject &report);
      void compareGroups(PvlContainer &g1, PvlContainer &g2, PvlObject &report);
      void compare(PvlKeyword &k1, PvlKeyword &k2, PvlContainer &report);

      void diff(QString name, PvlObject &o1, PvlObject &o2, PvlContainer &report);
      void diff(QString name, QString v1, QString v2, PvlContainer &report);
      PvlKeyword makeKeyword(QString name, QString v1, QString v2);

      void diff(QString name, double v1, double v2, double tol, PvlContainer &report);
      PvlKeyword makeKeyword(QString name, double v1, double v2, double tol);

      void addUniquePoint(QString label, QString v1, QString v2, PvlObject &parent);
      void addUniqueMeasure(QString label, QString v1, QString v2, PvlObject &parent);


    private:
      void init();

    private:
      //! The map of tolerances going from keyword name to tolerance value
      QMap<QString, double> *m_tolerances;

      //! The set of keywords to ignore by name
      QSet<QString> *m_ignoreKeys;
  };
}

#endif
