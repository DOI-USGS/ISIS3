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
#include "ControlPointV0001.h"
#include "ControlPointV0002.h"
#include "ControlPointV0003.h"

class QString;

namespace Isis {
  class FileName;
  class Progress;
  class Pvl;
  class PvlContainer;
  class PvlObject;
  class ControlPointV0001;
  class ControlPointV0002;
  class ControlPointV0003;

  /**
   * @brief Handle various control network file format versions.
   *
   * This class is used to read all versions of control networks and write out
   *   the most recent version in Pvl and protobuf format. When reading a
   *   control net file, the ControlNeVersioner is initialized with the
   *   filename. When writing a control net file or generating a Pvl network,
   *   the ControlNetVersioner is initialized from a ControlNet object.
   *
   * This class exists to isolate the code dealing with control network file
   *   formats. ControlNet can then interface with this class and only has to
   *   work with the current ControlPoint object.
   *
   *
   * The read routine is as follows:
   *   1. Read the Pvl file header.
   *   2. Determine if the network is stored in Pvl or protobuf format
   *   3. Determine the version of the network
   *   4. Read in the general ControlNet information such as network
   *        description, last modification date, etc. For Pvl networks, this
   *        information is in a PvlObject at the start of the network. For
   *        protobuf objects this information is stored in a header protobuf
   *        message after the Pvl file header.
   *   5. For each control point do the following:
   *     a. Read the control point into the appropriate ControlPointV####
   *          object.
   *     b. If the ControlPointV#### object is not the most recent version,
   *          upgrade it to the latest version.
   *     c. Convert the final ControlPointV#### object into a ControlPoint
   *          object and store it in the ControlNetVersioner.
   *
   * Once the ControlNet file is read into the ControlNetVersioner, the
   *   ControlPoints can be accessed through the takeFirstPoint method. This
   *   will remove the first ControlPoint stored in the ControlNetVersioner and
   *   give it to the caller. At this point, the caller is given ownership of
   *   the ControlPoint and is expected to delete it when finished. General
   *   information about the control network can be accessed directly from the
   *   ControlNetVersioner.
   *
   *
   * The protobuf file write routine is as follows:
   *   1. Copy the general ControlNet information such as network description,
   *        last modification date, etc. into the ControlNetVersioner.
   *   2. Copy the pointers to the ControlPoints and store them in the
   *        ControlNetVersioner. The ControlNetVersioner does not assume
   *        ownership of the ControlPoints when it does this. The ControlNet
   *        or what the ControlNet got the points from retains ownership.
   *   3. Write a 65536 byte blank header to the file.
   *   4. Write the general ControlNet information to a protobuf message header
   *        after the blank header.
   *   5. For each control point do the following:
   *     a. Convert the control point into a protobuf message.
   *     b. Write the size of the protobuf message to the file.
   *     c. Write the protobuf message to the file.
   *   6. Write a Pvl header into the original blank header at the start of the
   *        file. This header contains a flag indicating the network is a
   *        protobuf formatted network, the version of the format, the byte
   *        offset and size of the protobuf header, the byte offset and
   *        size of the block of protobuf control points, and general
   *        information about the control network.
   *
   *
   * Once the ControlNetVersioner is initialized from a file or a ControlNet,
   *   a Pvl formatted version of the control network can be created by the
   *   toPvl method. This will always output the control network in the latest
   *   Pvl format. From here, the Pvl network can be written to a file with
   *   Pvl::write(filename).
   *
   *
   * If the control network file format is changed the following changes need
   *   to be made to ControlNetVersioner and its supporting classes:
   *
   * New containers need to be added to interface with the new format. These
   *   containers should try to match the format of the data in the file, then
   *   the versioner will convert from that format to ControlNet, ControlPoint,
   *   ControlMeasure, and ControlMeasureLogData. General information about the
   *   control network should be stored in the ControlNetHeaderV#### structs.
   *   Data for control points should be stored in ControlPointV#### objects.
   *
   * If a new control point container was created, code needs to be added to
   *   create ControlPoint objects from them. A new createPoint method that
   *   takes the new container should be added to do this. The createMeasure
   *   method should also be changed to create ControlMeasures from the new
   *   containers.
   *
   * If a new header container was created, code needs to be added to store its
   *   information in the ControlNetVersioner. A new createHeader method that
   *   takes the new header container should be added to do this.
   *
   * New code needs to be added to update the previous containers to the new
   *   containers. Updating control point containers should happen in the new
   *   ControlPointV#### container class. It should have a constructor that
   *   takes a container for the previous version. Then, the createPoint method
   *   for the previous version needs to be changed to create a new container
   *   with this and then call the createPoint method for the new version.
   *   Updating header containers should happen in the createHeader method that
   *   takes the previous version. If the headers become more complicated, this
   *   may need to change to match how control point containers are updated.
   *
   * New methods need to be added to read the new file format. Methods for
   *   reading Pvl formatted files and protobuf formatted files need to be
   *   added; they should match the naming convention of readPvlV#### and
   *   readProtobufV#### respectively.
   *
   * New methods need to be added to write out the new file format. The write
   *   method should be changed to write out the new protobuf format. If
   *   a new header container is added, the writeHeader method should be
   *   changed to write the new protobuf header to the file. If a new control
   *   point container is added, the writeFirstPoint method should be changed
   *   to write a new protobuf control point to the file.
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
   *   @history 2017-12-11 Jeannie Backer & Jesse Mapel - Created class skeleton for refactor.
   *   @history 2017-12-11 Jesse Mapel - Added VersionedControlNetHeaders.
   *   @history 2017-12-12 Kristin Berry - Added initial toPvl for refactor.
   *   @history 2017-12-12 Jeannie Backer - Added VersionedControlPoints.
   *   @history 2017-12-12 Jeannie Backer - Implemented createPoint() methods.
   *   @history 2017-12-13 Jeannie Backer - Added target radii to createPoint(V0006).
   *   @history 2017-12-18 Adam Goins and Kristin Berry - Added new write() method.
   *   @history 2017-12-19 Kristin Berry - Corrected method names and general cleanup in toPvl and
   *                           write for refactor.
   *   @histroy 2017-12-20 Jesse Mapel - Made read and createPoint methods match new
   *                           ControlPointV#### classes.
   *   @history 2017-12-20 Jeannie Backer - Updated toPvl and write methods to get surface point
   *                           information from the ControlPoint.
   *   @history 2018-01-03 Jesse Mapel - Updated class documentation.
   *   @history 2018-01-04 Adam Goins - Updated read/write methods to read/write protobuf messages
   *                           correctly.
   *   @history 2018-01-12 Adam Goins - Added the ControlPoint radii to the header to avoid
   *                           Target::GetRadii calls to speed up createPoint().
   */
  class ControlNetVersioner {

    public:
      ControlNetVersioner(ControlNet *net);
      ControlNetVersioner(const FileName netFile);
      ~ControlNetVersioner();

      QString netId() const;
      QString targetName() const;
      QString creationDate() const;
      QString lastModificationDate() const;
      QString description() const;
      QString userName() const;

      int numPoints() const;
      ControlPoint *takeFirstPoint();

      void write(FileName netFile);
      Pvl toPvl();

    private:
      // These three methods are private for safety reasons.
      // TODO write a better reason. JAM
      ControlNetVersioner();
      ControlNetVersioner(const ControlNetVersioner &other);
      ControlNetVersioner &operator=(const ControlNetVersioner &other);

      // Private ControlNetHeader structs for versioning
      // TODO Document these for doxygen. JAM
      struct ControlNetHeaderV0001 {
        QString networkID;
        QString targetName;
        QString created;
        QString lastModified;
        QString description;
        QString userName;
        Distance equatorialRadius;
        Distance polarRadius;
      };
      typedef ControlNetHeaderV0001 ControlNetHeaderV0002;
      typedef ControlNetHeaderV0001 ControlNetHeaderV0003;
      typedef ControlNetHeaderV0001 ControlNetHeaderV0004;
      typedef ControlNetHeaderV0001 ControlNetHeaderV0005;

      typedef ControlPointV0003 ControlPointV0004;
      typedef ControlPointV0003 ControlPointV0005;

      void read(const FileName netFile);

      void readPvl(const Pvl &network);
      void readPvlV0001(const PvlObject &network);
      void readPvlV0002(const PvlObject &network);
      void readPvlV0003(const PvlObject &network);
      void readPvlV0004(const PvlObject &network);
      void readPvlV0005(const PvlObject &network);

      void readProtobuf(const Pvl &header, const FileName netFile);
      void readProtobufV0001(const Pvl &header, const FileName netFile);
      void readProtobufV0002(const Pvl &header, const FileName netFile);
      void readProtobufV0005(const Pvl &header, const FileName netFile);

      ControlPoint *createPoint(ControlPointV0001 &point);
      ControlPoint *createPoint(ControlPointV0002 &point);
      ControlPoint *createPoint(ControlPointV0003 &point);

      ControlMeasure *createMeasure(const ControlPointFileEntryV0002_Measure&);

      void createHeader(const ControlNetHeaderV0001 header);

      void writeHeader(std::fstream *output);
      int writeFirstPoint(std::fstream *output);

      ControlNetHeaderV0005 m_header; /**< Header containing information about
                                           the whole network.*/
      QList<ControlPoint *> m_points; /**< ControlPoints that are read in from a file or
                                           ready to be written out to a file.*/
      bool m_ownsPoints; /**< Flag if the versioner owns the control points stored in it.
                             This will be true when the versioner created the points from a file.
                             This will be false when the versioner copied the points from an
                             esiting control network.*/

  };
}
#endif
