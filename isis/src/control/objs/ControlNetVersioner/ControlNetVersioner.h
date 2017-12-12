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

#include <QString>

#include <QList>
#include <QSharedPointer>
#include <QVector>

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
   *  @history 2017-12-11 Jesse Mapel - Added VersionedControlNetHeaders.
   *  @history 2017-12-13 Jeannie Backer - Added VersionedControlPoints.
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

      struct ControlPointV0001 {
        PvlContainer container;
      }
      typedef ControlPointV0002 ControlPointV0001;
      typedef ControlPointV0003 ControlPointV0001;
      typedef ControlPointV0004 ControlPointV0001;
      typedef ControlPointV0005 ControlPointFileEntryV0002;//??? what to do???
      typedef ControlPointV0006 ControlPointFileEntryV0002;
      typedef ControlPointV0007 ControlPointFileEntryV0002;

      typedef ControlMeasureV0006 ControlPointV0006::Measure;//???

      // Private ControlNetHeader structs for versioning
      // TODO Document these for doxygen. JAM
      struct ControlNetHeaderV0001 {
        QString networkID;
        QString targetName;
        QString created;
        QString lastModified;
        QString description;
        QString userName;
      };
      typedef ControlNetHeaderV0002 ControlNetHeaderV0001;
      typedef ControlNetHeaderV0003 ControlNetHeaderV0001;
      typedef ControlNetHeaderV0004 ControlNetHeaderV0001;
      typedef ControlNetHeaderV0005 ControlNetHeaderV0001;
      typedef ControlNetHeaderV0006 ControlNetHeaderV0001;
      typedef ControlNetHeaderV0007 ControlNetHeaderV0001;

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

      QSharedPointer<ControlPoint> createPoint(const ControlPointV0001 point);
      QSharedPointer<ControlPoint> createPoint(const ControlPointV0002 point);
      QSharedPointer<ControlPoint> createPoint(const ControlPointV0003 point);
      QSharedPointer<ControlPoint> createPoint(const ControlPointV0004 point);
      // v5 dne???
      QSharedPointer<ControlPoint> createPoint(const ControlPointV0006 point);
      QSharedPointer<ControlMeasure> createMeasure(const ControlMeasureV0006 measure);
      void copy(PvlContainer &container,
                QString keyName,
                ControlPointFileEntryV0002 &point,
                void (ControlPointFileEntryV0002::*setter)(bool));
      void copy(PvlContainer &container,
                QString keyName,
                ControlPointFileEntryV0002 &point,
                void (ControlPointFileEntryV0002::*setter)(double));
      void copy(PvlContainer &container,
                QString keyName,
                ControlPointFileEntryV0002 &point,
                void (ControlPointFileEntryV0002::*setter)(const std::string&));
      void copy(PvlContainer &container,
                QString keyName,
                ControlPointFileEntryV0002::Measure &measure,
                void (ControlPointFileEntryV0002::Measure::*setter)(bool));
      void copy(PvlContainer &container,
                QString keyName,
                ControlPointFileEntryV0002::Measure &measure,
                void (ControlPointFileEntryV0002::Measure::*setter)(double));
      void copy(PvlContainer &container,
                QString keyName,
                ControlPointFileEntryV0002::Measure &measure,
                void (ControlPointFileEntryV0002::Measure::*setter)(const std::string &));

      void setHeader(const ControlNetHeaderV0001 header);

      void writeHeader(ZeroCopyInputStream *fileStream);
      void writeFirstPoint(ZeroCopyInputStream *fileStream);

      ControlNetHeaderV0007 m_header; /**< Header containing information about
                                           the whole network.*/
      QList< QSharedPointer<ControlPoint> > m_points; /**< ControlPoints that are
                                                           read in from a file or
                                                           ready to be written out
                                                           to a file.*/

  };
}

#endif
#if 0
      struct ControlPointV0004 {
        QString id;
        QString chooserName;
        QString datetime;
        int     type;
        bool    editLock;
        bool    ignored;
        bool    jigsawRejected;

        QString aprioriRadiusSource;
        QString aprioriRadiusSourceFile;

        QString aprioriSurfacePointSource;
        QString aprioriSurfacePointSourceFile; // apriorixyzsourcefile
        // SurfacePoint aprioriSurfacePoint;   // apriorixyzsource
        double     aprioriX; // <meters>
        double     aprioriY; // <meters>
        double     aprioriZ; // <meters>
        bool       latitudeConstrained;
        bool       longitudeConstrained;
        bool       radiusConstrained;
        QVector<double> aprioriCovarianceMatrix;

        // SurfacePoint adjustedSurfacePoint;
        double adjustedX;
        double adjustedY;
        double adjustedZ;
        QVector<double> adjustedCovarianceMatrix;

        struct CPV4Measure {
          QString serialnumber;
          int     type;
          double  sample;
          double  line;
          double  sampleResidual;
          double  lineResidual;
          QString choosername;
          QString datetime;
          bool    editLock;
          bool    ignore;
          bool    jigsawRejected;
          double  diameter;
          double apriorisample;
          double aprioriline;
          double samplesigma;
          double linesigma;
        };

        QList <CPV4Measure> measureList;
        int   referenceIndex;
      }
#endif
