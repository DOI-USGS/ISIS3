#ifndef ControlNetVersioner_h
#define ControlNetVersioner_h
/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2009/07/15 17:33:52 $
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

#include <string>

#include <QList>
#include <QSharedPointer>

#include "ControlPoint.h"

class QString;

namespace Isis {
  class FileName;
  class Progress;
  class Pvl;
  class PvlContainer;
  class PvlObject;

  /**
   * @brief Handle Various Control Network Versions
   *
   * This class is used to read any and all control networks.
   *
   * All publicly released versions of binary control networks should be
   *   supported if possible.
   *
   *                         ControlNetVersioner::Read
   *                        /                         \
   *                    [If Pvl]                    [If Binary]
   *                      /                             \
   *                  Pvl::Read                         |
   *                     |          [If not latest]     |
   *              Update To Latest <-- ToPvl --- ControlNetFileV????::Read
   *                     |                              |  [If latest]
   *                     |                              |
   *                 Latest Pvl  ------------> Latest ControlNetFile
   *                                                    |
   *                                                    |
   *                                             Isis::ControlNet
   *
   *
   * We used to have the 4 conversions:
   *   Pvl    -> Isis::ControlNet
   *   Binary -> Isis::ControlNet
   *   Pvl    <- Isis::ControlNet
   *   Binary <- Isis::ControlNet
   *
   * But maintaining these causes us to need the old ControlNet code around.
   *   These conversions are used instead:
   *   Pvl    -> Binary           *Latest version only
   *   Binary -> Pvl              *All versions
   *   Binary -> Isis::ControlNet *Latest version only
   *   Binary <- Isis::ControlNet *Latest version only
   *
   * The log data classes are still used to understand what log data is what,
   *   so these classes must remain backwards compatible. Otherwise all of the
   *   versioning code is here. I encourage the use of log data to avoid needing
   *   to make any changes in this code.
   *
   * The reason the update cycle is only in Pvl form is because of how much
   *   simpler and less error-prone the code is to convert between versions
   *   in a generic file format. You don't need to do things like
   *   new.setNetworkId(old.getNetworkId()) in Pvl. Hopefully the speed cost
   *   is not significant enough to need an update cycle in binary form. It is
   *   a one-time cost per network, timed at about 5 minutes for our currently
   *   largest network (120MB protocol buffer file).
   *
   * This class is the reason Isis::ControlNet only need to work with the latest
   *   version. Also, we only need 1-way conversions for old file formats
   *   (ControlNetFile::ToPvl).
   *
   * If you want to change the Pvl format, you must update the following:
   *     Update LATEST_PVL_VERSION
   *     Write ConvertVersionAToVersionB
   *     Update ReadPvlNetwork
   *     Update LatestPvlToBinary
   *
   * If you want to change the Binary format, you must update the following:
   *     Update LATEST_BINARY_VERSION
   *     Write ControlNetFileV????
   *     Update ControlNetFile.h
   *     Update ReadBinaryNetwork
   *     Update LatestPvlToBinary
   *
   * @ingroup ControlNetwork
   *
   * @author 2011-04-05 Steven Lambright
   *
   * @internal
   *   @history 2011-04-14 Steven Lambright - Improved handling of V1 networks
   *   @history 2011-05-02 Tracie Sucharski - If Held=True exists in input net,
   *                           set PointType=Ground.
   *   @history 2011-05-02 Debbie A. Cook - Created pvl version 3 which added
   *                           point type of constrained.
   *   @history 2012-08-14 Steven Lambright - Simplified error handling of invalid
   *                           target names in V1 networks (V1->V2 code).
   *   @history 2012-11-22 Debbie A. Cook - Changed to use TProjection instead of Projection.
   *                           References #775.
   *   @history 2013-03-13 Steven Lambright and Stuart Sides - Added support for more V1 Pvl
   *                           networks (specifically, isis3.2.1 hijitreg output networks with
   *                           measures that lack Sample/Line and are set to unmeasured). Fixes
   *                           #1554.
   *   @history 2016-04-22 Jeannie Backer - Updated error message in
   *                           ConvertVersion1ToVersion2() to make it specific to this class
   *                           call. This was done to reduce redundancy since the original
   *                           message for this error was very similar to the caught exception
   *                           to which it is appended. References #3892
   *  @history 2017-12-11 Jeannie Backer & Jesse Mapel - Created class skeleton for refactor.
   */
  class ControlNetVersioner {
    public:
      ControlNetVersioner(QSharedPointer<ControlNet> net);
      ControlNetVersioner(const FileName netFile);
      ~ControlNetVersioner();

      QString netId() const;
      QString targetName() const;
      QString creationDate() const;
      QString lastModificationDate() const;
      QString description() const;
      QString userName() const;

      QSharedPointer<ControlPoint> takeFirstPoint();

      void write(FileName netFile);
      Pvl &toPvl();

    private:
      // These three methods are private for safety reasons.
      // TODO write a better reason. JAM
      ControlNetVersioner();
      ControlNetVersioner(const ControlNetVersioner &other);
      ControlNetVersioner &operator=(const ControlNetVersioner &other);

      void read(const FileName netFile);

      void readPvl(const Pvl &network);
      void readPvlV0001(const Pvl &network);
      void readPvlV0002(const Pvl &network);
      void readPvlV0003(const Pvl &network);
      void readPvlV0004(const Pvl &network);

      void readProtobuf(const Pvl &header, const FileName netFile);
      void readProtobufV0001(const FileName netFile);
      void readProtobufV0002(const FileName netFile);
      void readProtobufV0007(const FileName netFile);

      QSharedPointer<ControlPoint> createPointFromV0001(const ControlPointV0001 point);
      QSharedPointer<ControlPoint> createPointFromV0002(const ControlPointV0002 point);
      QSharedPointer<ControlPoint> createPointFromV0003(const ControlPointV0003 point);
      QSharedPointer<ControlPoint> createPointFromV0004(const ControlPointV0004 point);
      QSharedPointer<ControlPoint> createPointFromV0005(const ControlPointV0005 point);
      QSharedPointer<ControlPoint> createPointFromV0006(const ControlPointV0006 point);
      QSharedPointer<ControlPoint> createPointFromV0007(const ControlPointV0007 point);

      void createHeaderFromV0001(const ControlNetHeaderV0001);
      void createHeaderFromV0002(const ControlNetHeaderV0002);
      void createHeaderFromV0003(const ControlNetHeaderV0003);
      void createHeaderFromV0004(const ControlNetHeaderV0004);
      void createHeaderFromV0005(const ControlNetHeaderV0005);
      void createHeaderFromV0006(const ControlNetHeaderV0006);
      void createHeaderFromV0007(const ControlNetHeaderV0007);

      void writeHeader(ZeroCopyInputStream *fileStream);
      void writeFirstPoint(ZeroCopyInputStream *fileStream);

      ControlNetHeaderV0007 m_header; /**< Header containing information about
                                           the whole network.*/
      QList< QSharedPointer<ControlPoint> > m_points; /**< ControlPoints that are
                                                           read in from a file or
                                                           ready to be written out
                                                           to a file.*/

     struct ControlNetHeaderV0001 {

     }
     struct ControlNetHeaderV0002 {

     }
     struct ControlNetHeaderV0003 {

     }
     struct ControlNetHeaderV0004 {

     }
     struct ControlNetHeaderV0005 {

     }
     struct ControlNetHeaderV0006 {

     }
     struct ControlNetHeaderV0007 {

     }
  };
}

#endif
