#ifndef ControlNetDiff_h
#define ControlNetDiff_h

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
