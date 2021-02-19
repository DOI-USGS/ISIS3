#ifndef ControlNetVersioner_h
#define ControlNetVersioner_h

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
   * <h3>Overview</h3>
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
   * <h3>Reading Control Network Files</h3>
   *
   * The read routine is as follows:
   * <ol>
   *   <li>Read the Pvl file header</li>
   *   <li>Determine if the network is stored in Pvl or protobuf format</li>
   *   <li>Determine the version of the network</li>
   *   <li>Read in the general ControlNet information such as network
   *        description, last modification date, etc. For Pvl networks, this
   *        information is in a PvlObject at the start of the network. For
   *        protobuf objects this information is stored in a header protobuf
   *        message after the Pvl file header.</li>
   *   <li>For each control point do the following:
   *   <ol type="a">
   *     <li>Read the control point into the appropriate ControlPointV####
   *          object.</li>
   *     <li>If the ControlPointV#### object is not the most recent version,
   *          upgrade it to the latest version.</li>
   *     <li>Convert the final ControlPointV#### object into a ControlPoint
   *          object and store it in the ControlNetVersioner.</li>
   *   </ol>
   *   </li>
   * </ol>
   *
   * Once the ControlNet file is read into the ControlNetVersioner, the
   *   ControlPoints can be accessed through the takeFirstPoint method. This
   *   will remove the first ControlPoint stored in the ControlNetVersioner and
   *   give it to the caller. At this point, the caller is given ownership of
   *   the ControlPoint and is expected to delete it when finished. General
   *   information about the control network can be accessed directly from the
   *   ControlNetVersioner.
   *
   * <h3>Writing Control Network Files</h3>
   *
   * The protobuf file write routine is as follows:
   * <ol>
   *   <li>Copy the general ControlNet information such as network description,
   *        last modification date, etc. into the ControlNetVersioner.</li>
   *   <li>Copy the pointers to the ControlPoints and store them in the
   *        ControlNetVersioner. The ControlNetVersioner does not assume
   *        ownership of the ControlPoints when it does this. The ControlNet
   *        or what the ControlNet got the points from retains ownership.</li>
   *   <li>Write a 65536 byte blank header to the file.</li>
   *   <li>Write the general ControlNet information to a protobuf message header
   *        after the blank header.</li>
   *   <li>For each control point do the following:
   *   <ol type="a">
   *     <li>Convert the control point into a protobuf message.</li>
   *     <li>Write the size of the protobuf message to the file.</li>
   *     <li>Write the protobuf message to the file.</li>
   *   </ol>
   *   </li>
   *   <li>Write a Pvl header into the original blank header at the start of the
   *        file. This header contains: a flag indicating the network is a
   *        protobuf formatted network, the version of the format, the byte
   *        offset and size of the protobuf header, the byte offset and
   *        size of the block of protobuf control points, and general
   *        information about the control network.</li>
   * </ol>
   *
   * Once the ControlNetVersioner is initialized from a file or a ControlNet,
   *   a Pvl formatted version of the control network can be created by the
   *   toPvl method. This will always output the control network in the latest
   *   Pvl format. From here, the Pvl network can be written to a file with
   *   Pvl::write(filename).
   *
   * <h3>Modifying the Control Network File Format</h3>
   *
   * If the control network file format is changed the following changes need
   *   to be made to ControlNetVersioner and its supporting classes:
   *<ul>
   * <li>
   * New containers need to be added to interface with the new format. These
   *   containers should try to match the format of the data in the file, then
   *   the versioner will convert from that format to ControlNet, ControlPoint,
   *   ControlMeasure, and ControlMeasureLogData. General information about the
   *   control network should be stored in the ControlNetHeaderV#### structs.
   *   Data for control points should be stored in ControlPointV#### objects.
   * </li>
   * <li>
   * If a new control point container was created, code needs to be added to
   *   create ControlPoint objects from them. A new createPoint method that
   *   takes the new container should be added to do this. The createMeasure
   *   method should also be changed to create ControlMeasures from the new
   *   containers.
   * </li>
   * <li>
   * If a new header container was created, code needs to be added to store its
   *   information in the ControlNetVersioner. A new createHeader method that
   *   takes the new header container should be added to do this.
   * </li>
   * <li>
   * New code needs to be added to update the previous containers to the new
   *   containers. Updating control point containers should happen in the new
   *   ControlPointV#### container class. It should have a constructor that
   *   takes a container for the previous version. Then, the createPoint method
   *   for the previous version needs to be changed to create a new container
   *   with this and then call the createPoint method for the new version.
   *   Updating header containers should happen in the createHeader method that
   *   takes the previous version. If the headers become more complicated, this
   *   may need to change to match how control point containers are updated.
   * </li>
   * <li>
   * New methods need to be added to read the new file format. Methods for
   *   reading Pvl formatted files and protobuf formatted files need to be
   *   added; they should match the naming convention of readPvlV#### and
   *   readProtobufV#### respectively.
   * </li>
   * <li>
   * New methods need to be added to write out the new file format. The write
   *   method should be changed to write out the new protobuf format. If
   *   a new header container is added, the writeHeader method should be
   *   changed to write the new protobuf header to the file. If a new control
   *   point container is added, the writeFirstPoint method should be changed
   *   to write a new protobuf control point to the file.
   * </li>
   * <li>
   * Update the documentation on this class under the <b>Control Network File
   *   Format History</b> heading. This should include a description of the new
   *   file format and how it differs from the previous version.
   * </li>
   *</ul>
   *
   * <h3>Control Network File Format History</h3>
   *
   * Prior to the creation of this versioning class, which was released with
   *   ISIS 3.2.2, all control network files were Pvl formatted text files.
   *   Reading and writing these files was handled by the ControlNet, ControlPoint,
   *   and ControlMeasure classes. As the file format was changed, those
   *   classes were modified to account for the new format. Because of this,
   *   the history of the control network file format prior to ISIS 3.2.2
   *   is not well documented.
   *
   * The following are descriptions of the different control network file
   *   format versions that are currently supported. Each description also
   *   describes how that version differs from the previous.
   *
   * <b>Version 1</b>
   *
   *   This version maintains backwards compatibility with all files created
   *   prior to versioning. If a control network file does not have a version
   *   flag, then it is assumed to be a version 1 file. Because this version
   *   supports all files created prior to ISIS 3.2.2, there is no standardized
   *   Pvl format associated with it.
   *
   *   Originally, there was no version 1 binary format. When version 2 was
   *   added, version 1 was changed to use the version 2 binary format.
   *
   * <b>Version 2</b>
   *
   *   This version was the first to have a standardized format. The following
   *   were standardized with this format:
   *   <ul>
   *     <li>The Held flag was replaced by point types. Points could be either
   *     Tie points or Ground points. Points that were previously flagged as
   *     Held were changed to Ground points.</li>
   *     <li>A posteriori was replaced with adjusted in several keyword
   *     names.</li>
   *     <li>The a priori and adjusted ground points were changed from
   *     (latitude, longitude, radius) format to body fixed (X, Y, Z) format.
   *     </li>
   *     <li>Ground point sigmas were replaced with covariance matrices.</li>
   *     <li>Latitude, longitude, and radius constrained flags were added.</li>
   *     <li>Estimated measures were renamed to Candidate measures.</li>
   *     <li>Unmeasured measures were renamed to Candidate measures, had their
   *     line and sample set to 0, and were flagged as ignored.</li>
   *     <li>Automatic, ValidatedManual and AutomaticPixel measures were
   *     renamed to RegisteredPixel measures.</li>
   *     <li>ValidatedAutomatic and AutomaticSubPixel measures were renamed to
   *     RegisteredSubPixel measures.</li>
   *     <li>ErrorSample and ErrorLine were renamed to SampleResidual and
   *     LineResidual respectively.</li>
   *     <li>Diameter, ZScore, and ErrorMagnitude were no longer saved in
   *     measures.</li>
   *   </ul>
   *
   *   Version 2 was the first version to support a binary protobuf format.
   *   Version 1 was retroactively changed to use the version 2 binary format.
   *   Version 2 binary control network files consist of three parts:
   *   <ol>
   *     <li>
   *     <em>Pvl File Header:</em> The file starts with a Pvl formatted header
   *     that contains offsets to the binary components of the file and the
   *     version number of the file. This header may also contain general
   *     information about the control network that is only for user reference
   *     is not used when reading the file.
   *     </li>
   *     <li>
   *     <em>Protobuf Core:</em> After the Pvl header is the protobuf core that
   *     contains the majority of the network data. This is a hierarchical
   *     structure with general network information such as the network
   *     description at the top level. Below that is the control point
   *     information. The lowest level contains the control measure information.
   *     This structure is defined by <em>ControlNetFileProtoV0001.proto</em>.
   *     </li>
   *     <li>
   *     <em>Protobuf Log Data:</em> The final component of the file contains
   *     the control measure log data. This is structured the same as the
   *     protobuf core. So, the log data for the i<sup>th</sup> measure in the
   *     j<sup>th</sup> point in the core is in the i<sup>th</sup> measure of
   *     the j<sup>th</sup> point in this structure. This structure is defined
   *     by <em>ControlNetLogDataProtoV0001.proto</em>.
   *     </li>
   *   </ol>
   *
   * <b>Version 3</b>
   *
   *   This version was created to avoid file size limits imposed by the
   *   version 2 binary file format. Protobuf messages are limited to 2GB for
   *   security reasons. So, version 2 binary files can only contain 2GB of
   *   information in their Protobuf Core. In version 3, the Protobuf Core was
   *   changed from a single message to a header message and individual
   *   messages for each control point. This way, the Protobuf Core could
   *   contain an arbitrary amount of information as long as any single point
   *   does not exceed 2GB. At the same time, control measure log data was
   *   moved into the Protobuf Core so that each measure contains its own log
   *   data.
   *
   *   Version 3 binary control network files are formatted as follows:
   *   <ul>
   *     <li>
   *     <em>pvl File Header:</em> The Pvl Header in version 3 binary files
   *     is the same as the Pvl Header in version 2 binary files, except it has
   *     offsets to the Protobuf Header and Protobuf Core instead of the
   *     Protobuf Core and Protobuf Log Data.
   *     </li>
   *     <li>
   *     <em>Protobuf Header:</em> The binary component of version 3 binary
   *     control network files starts with a protobuf message header that
   *     contains general information about the network and the size of each
   *     control point's protobuf message. The size of each control point
   *     message is required to parse the Protobuf Core because protobuf
   *     messages are not self-delimiting. This structure is defined by
   *     <em>ControlNetFileHeaderV0002.proto</em>.
   *     </li>
   *     <li>
   *     <em>Protobuf Core:</em> Immediately after the Protobuf Header is
   *     the Protobuf Core which contains all of the control point and control
   *     measure information. This is structured as consecutive protobuf
   *     messages where each message contains all of the information for a
   *     control point and its control measures. Because protobuf messages
   *     are not self-delimiting, the size of each message must be known prior
   *     to parsing the Protobuf Core. The control point messages are defined
   *     by <em>ControlPointFileEntryV0002.proto</em>.
   *     </li>
   *   </ul>
   *
   *   Version 3 also further differentiated control point types. Control
   *   points that had their latitude, longitude, and/or radius constrained
   *   were changed from Tie points to Constrained points.
   *
   * <b>Version 4</b>
   *
   *   This version was created when Ground and Tie control points were renamed
   *   to Fixed and Free respectively. Version 4 Pvl control network files are
   *   identical to version 3 Pvl control network files, except for the new
   *   control point type values. When version 4 was created, the .proto file
   *   that defined control point protobuf messages was modified to allow for
   *   the new control point type names and the old names were flagged as
   *   deprecated. So, version 3 and version 4 binary control network files are
   *   formatted exactly the same.
   *
   * <b>Version 5</b>
   *
   *   This version was created to allow for progressive reading and writing of
   *   binary control network files. Previous versions required the entire
   *   contents of binary control network files to be read into memory before
   *   the ControlNet, ControlPoint, and ControlMeasure objects could be
   *   created. Version 5 was created to allow binary control network files
   *   to be read one control point at a time. Similarly, previous versions
   *   required all of the information in the control network to be copied into
   *   protobuf structures before any of it could be written to a file.
   *
   *   Version 5 Pvl control network files are identical to version 4 Pvl
   *   control network files.
   *
   *   Version 5 binary control network files are formatted the same as version
   *   4 binary control network files except for how they store the sizes of
   *   the control point messages in the Protobuf Core. In a version 5 binary
   *   control network file, each control point message is prepended by an
   *   unsigned, 32 bit, LSB, integer (c++ uint32_t) that contains the size of
   *   the message. This design was modeled after the delimited read and write
   *   functionality in the Java protobuf library. Additionally, the Protobuf
   *   Header contains the number of control points in the network instead of
   *   a list of all the control point message sizes. The structure of the
   *   Protobuf Header is defined by <em>ControlNetFileHeaderV0005.proto</em>.
   *   The structure of the protobuf messages in the Protobuf Core is defined
   *   by <em>ControlPointFileEntryV0002.proto</em>, the same as in version 4.
   *
   *   Starting with version 5, the naming scheme for .proto files was changed
   *   to use the same version number as the control net file format. So, the
   *   new .proto file defining the Protobuf Header was named
   *   <em>ControlNetFileHeaderV0005.proto</em> instead of
   *   <em>ControlNetFileHeaderV0003.proto</em>.
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
   *   @history 2017-12-20 Jesse Mapel - Made read and createPoint methods match new
   *                           ControlPointV#### classes.
   *   @history 2017-12-20 Jeannie Backer - Updated toPvl and write methods to get surface point
   *                           information from the ControlPoint.
   *   @history 2018-01-03 Jesse Mapel - Updated class documentation.
   *   @history 2018-01-04 Adam Goins - Updated read/write methods to read/write protobuf messages
   *                           correctly.
   *   @history 2018-01-12 Adam Goins - Added the ControlPoint radii to the header to avoid
   *                           Target::GetRadii calls to speed up createPoint().
   *   @history 2018-01-12 Adam Goins - Added Progress during reads.
   *   @history 2018-01-24 Jesse Mapel - Fixed c++11 build warnings.
   *
   *   @history 2018-01-27 Jesse Mapel - Fixed some documentation formatting. Added a section
   *                           describing the different file format versions.
   *   @history 2018-01-30 Adam Goins - Ensured point sizes are written/read as lsb by using
   *                           EndianSwapper.
   *   @history 2018-02-25 Debbie A. Cook - Generalized calls to
   *                           ControlPoint::IsLatitudeConstrained to IsCoord1Constained
   *                           and added or updated a few comments. *** TODO *** make sure
   *                           the new methods are fully functional for either coordinate type
   *                           once the new header keyword is added.
   *   @history 2018-03-28 Adam Goins - Added targetRadii groups to the header. Changed the
   *                           versioner to write these values out in a targetRadii group for
   *                           both binary V0005 and PvlV0005 networks. Fixes #5361.
   *   @history 2018-04-05 Adam Goins - Added hasTargetRadii() and targetRadii() to the versioner
   *                           so that these values can be grabbed from a ControlNet on read.
   *                           Also Fixes #5361.
   *   @history 2018-06-01 Debbie A. Cook - (added to BundleXYZ 2018-02-25)
   *                           Generalized calls to ControlPoint::IsLatitudeConstrained to
   *                           IsCoord1Constained and added or updated a few comments.
   *                           *** TODO *** make sure the new methods are fully functional
   *                           for either coordinate type once the new header keyword is added.
   *
   *   @history 2018-07-03 Jesse Mapel - Removed target radii from versioner. References #5457.
   */
  class ControlNetVersioner {

    public:
      ControlNetVersioner(ControlNet *net);
      ControlNetVersioner(const FileName netFile, Progress *progress=NULL);
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
      // These three methods are private to ensure proper memory management
      //! Default constructor. Intentially un-implemented.
      ControlNetVersioner();
      /**
       * Copy constructor. Intentially un-implemented.
       *
       * @param other The other ControlNetVersioner to create a copy of.
       */
      ControlNetVersioner(const ControlNetVersioner &other);
      /**
       * Asssignment operator. Intentially un-implemented.
       *
       * @param other The other ControlNetVersione to assign from.
       *
       * @return @b ControlNetVersioner& A reference to this after assignment.
       */
      ControlNetVersioner &operator=(const ControlNetVersioner &other);

      // Private ControlNetHeader structs for versioning
      /**
       * Versioned container for general information about a control network.
       *
       * @ingroup ControlNetwork
       *
       * @author 2017-12-27 Jesse Mapel
       *
       * @internal
       *   @history 2017-12-27 Jesse Mapel - Original Version
       */
      struct ControlNetHeaderV0001 {
        //! The ID/Name of the control network
        QString networkID;
        //! The NAIF name of the target body
        QString targetName;
        //! The date and time of the control network's creation
        QString created;
        //! The date and time of the control network's last modification
        QString lastModified;
        //! The text description of the control network
        QString description;
        //! The name of the user or program that last modified the control network
        QString userName;
      };

      //! Typedef for consistent naming of containers for version 2
      typedef ControlNetHeaderV0001 ControlNetHeaderV0002;
      //! Typedef for consistent naming of containers for version 3
      typedef ControlNetHeaderV0001 ControlNetHeaderV0003;
      //! Typedef for consistent naming of containers for version 4
      typedef ControlNetHeaderV0001 ControlNetHeaderV0004;
      //! Typedef for consistent naming of containers for version 5
      typedef ControlNetHeaderV0001 ControlNetHeaderV0005;

      //! Typedef for consistent naming of containers for version 4
      typedef ControlPointV0003 ControlPointV0004;
      //! Typedef for consistent naming of containers for version 5
      typedef ControlPointV0003 ControlPointV0005;

      void read(const FileName netFile, Progress *progress=NULL);

      void readPvl(const Pvl &network, Progress *progress=NULL);
      void readPvlV0001(const PvlObject &network, Progress *progress=NULL);
      void readPvlV0002(const PvlObject &network, Progress *progress=NULL);
      void readPvlV0003(const PvlObject &network, Progress *progress=NULL);
      void readPvlV0004(const PvlObject &network, Progress *progress=NULL);
      void readPvlV0005(const PvlObject &network, Progress *progress=NULL);

      void readProtobuf(const Pvl &header, const FileName netFile, Progress *progress=NULL);
      void readProtobufV0001(const Pvl &header, const FileName netFile, Progress *progress=NULL);
      void readProtobufV0002(const Pvl &header, const FileName netFile, Progress *progress=NULL);
      void readProtobufV0005(const Pvl &header, const FileName netFile, Progress *progress=NULL);

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
