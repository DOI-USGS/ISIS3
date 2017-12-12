#include "ControlNetVersioner.h"

#include <string>

#include <QDebug>

#include "ControlNetFile.h"
#include "ControlNetFileV0001.h"
#include "ControlNetFileV0002.h"
#include "ControlNetFileV0002.pb.h"
#include "ControlMeasureLogData.h"
#include "Distance.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifStatus.h"
#include "Progress.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "SurfacePoint.h"
#include "Target.h"

using namespace std;

namespace Isis {

  ControlNetVersioner::ControlNetVersioner(QSharedPointer<ControlNet> net) {

  }


  ControlNetVersioner::ControlNetVersioner(const FileName netFile) {

  }


  ControlNetVersioner::~ControlNetVersioner() {

  }


  QString ControlNetVersioner::netId() const {

  }


  QString ControlNetVersioner::targetName() const {

  }


  QString ControlNetVersioner::creationDate() const {

  }


  QString ControlNetVersioner::lastModificationDate() const {

  }


  QString ControlNetVersioner::description() const {

  }


  QString ControlNetVersioner::userName() const {

  }


  QSharedPointer<ControlPoint> ControlNetVersioner::takeFirstPoint() {

  }


  void ControlNetVersioner::write(FileName netFile) {

  }


  Pvl &ControlNetVersioner::toPvl() {

  }


  /**
   * Read a control network file and prepare the data to be converted into
   * a control network.
   *
   * @param netFile The control network file to read.
   */
  void ControlNetVersioner::read(const FileName netFile) {
    try {
      Pvl network(netFile.expanded());

      if (network.hasObject("ProtoBuffer")) {
        readProtobuf(network, netFile);
      }
      else if (network.hasObject("ControlNetwork")) {
        readPvl(network);
      }
      else {
        IString msg = "Could not determine the control network file type";
        throw IException(IException::Io, msg, _FILEINFO_);
      }
    }
    catch (IException &e) {
      IString msg = "Reading the control network [" + netFile.name()
          + "] failed";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }
  }


  /**
   * Read a Pvl control network and prepare the data to be converted into a
   * control network.
   *
   * @param network The Pvl network data
   */
  void ControlNetVersioner::readPvl(const Pvl &network) {
      PvlObject &controlNetwork = network.findObject("ControlNetwork");

      if (!controlNetwork.hasKeyword("Version"))
        controlNetwork += PvlKeyword("Version", "1");

      int version = toInt(controlNetwork["Version"][0]);

      switch (version) {
        case 1:
          readPvlV0001(controlNetwork);
          break;

        case 2:
          readPvlV0001(controlNetwork);
          break;

        case 3:
          readPvlV0001(controlNetwork);
          break;

        default:
          IString msg = "The Pvl file version [" + toString(version)
                        + "] is not supported";
          throw IException(IException::Unknown, msg, _FILEINFO_);
      }
  }


  void ControlNetVersioner::readPvlV0001(const Pvl &network) {

  }


  void ControlNetVersioner::readPvlV0002(const Pvl &network) {

  }


  void ControlNetVersioner::readPvlV0003(const Pvl &network) {

  }


  void ControlNetVersioner::readPvlV0004(const Pvl &network) {

  }


  void ControlNetVersioner::readProtobuf(const Pvl &header, const FileName netFile) {

  }


  void ControlNetVersioner::readProtobufV0001(const FileName netFile) {

  }


  void ControlNetVersioner::readProtobufV0002(const FileName netFile) {

  }


  void ControlNetVersioner::readProtobufV0007(const FileName netFile) {

  }


  QSharedPointer<ControlPoint> ControlNetVersioner::createPointFromV0001(const ControlPointV0001 point) {

  }


  QSharedPointer<ControlPoint> ControlNetVersioner::createPointFromV0002(const ControlPointV0002 point) {

  }


  QSharedPointer<ControlPoint> ControlNetVersioner::createPointFromV0003(const ControlPointV0003 point) {

  }


  QSharedPointer<ControlPoint> ControlNetVersioner::createPointFromV0004(const ControlPointV0004 point) {

  }


  QSharedPointer<ControlPoint> ControlNetVersioner::createPointFromV0005(const ControlPointV0005 point) {

  }


  QSharedPointer<ControlPoint> ControlNetVersioner::createPointFromV0006(const ControlPointV0006 point) {

  }


  QSharedPointer<ControlPoint> ControlNetVersioner::createPointFromV0007(const ControlPointV0007 point) {

  }


  /**
   * Create the internal header from a V0001 header.
   *
   * The latest version is V0001, so this will check for an old issue with
   * Mars target names and then internalize the header.
   *
   * @param header The V0001 header
   */
  void ControlNetVersioner::createHeader(const ControlNetHeaderV0001 header) {
    m_header = header;

    if (m_header.targetName.startsWith("MRO/")) {
      m_header.targetName = "Mars";
    }
  }


  void ControlNetVersioner::writeHeader(ZeroCopyInputStream *fileStream) {

  }


  void ControlNetVersioner::writeFirstPoint(ZeroCopyInputStream *fileStream) {

  }


// ~~~~~~~~~~~~~~~ BEGIN OLD CONTROLNETVERSIONER CODE ~~~~~~~~~~~~~~~
#if 0

  /**
   * Read the control network from disk. This will always return the network in
   *   its "latest version" binary form. Generally this will only be called by
   *   ControlNet but a conversion from binary to pvl can make use out of this
   *   also.
   *
   * @param networkFileName The filename of the cnet to be read
   *
   */
  LatestControlNetFile *ControlNetVersioner::Read(const FileName &networkFileName) {

    try {
      Pvl network(networkFileName.expanded());

      if (network.hasObject("ProtoBuffer")) {
        return ReadBinaryNetwork(network, networkFileName);
      }
      else if (network.hasObject("ControlNetwork")) {
        return ReadPvlNetwork(network);
      }
      else {
        IString msg = "Could not determine the control network file type";
        throw IException(IException::Io, msg, _FILEINFO_);
      }
    }
    catch (IException &e) {
      IString msg = "Reading the control network [" + networkFileName.name()
          + "] failed";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }
  }


  /**
   * This will write a control net file object to disk.
   *
   * @param file file The output filename that will be written to
   * @param fileData The ControlNetFile representation to write
   * @param pvl True if the output format should be Pvl, false if not
   *
   */
  void ControlNetVersioner::Write(const FileName &file,
      const LatestControlNetFile &fileData, bool pvl) {

    if (pvl) {
      fileData.toPvl().write(file.expanded());
    }
    else {
      fileData.Write(file);
    }
  }


  /**
   * This interprets a Pvl network of any version. Since we already have the
   *   Pvl in memory (we need it to figure out if it is a Pvl network) it
   *   does not actually call Pvl::Read.

    PvlObject &network = pvl.findObject("ControlNetwork");

    if (!network.hasKeyword("Version"))
      network += PvlKeyword("Version", "1");


    PvlObject &network = pvl.findObject("ControlNetwork");

    if (!network.hasKeyword("Version"))
      network += PvlKeyword("Version", "1");

    int version = toInt(network["Version"][0]);

    PvlObject &network = pvl.findObject("ControlNetwork");

    if (!network.hasKeyword("Version"))
      network += PvlKeyword("Version", "1");

    int version = toInt(network["Version"][0]);

    while (version != LATEST_PVL_VERSION) {
      int previousVersion = version;

      switch (version) {
        case 1:
          ConvertVersion1ToVersion2(network);
          break;

        case 2:
          ConvertVersion2ToVersion3(network);
          break;

        case 3:
          ConvertVersion3ToVersion4(network);
          break;

        default:
          IString msg = "The Pvl file version [" + IString(version) + "] is not"
              " supported";
          throw IException(IException::Unknown, msg, _FILEINFO_);
      }

      version = toInt(network["Version"][0]);

      if (version == previousVersion) {
        IString msg = "Cannot update from version [" + IString(version) + "] "
            "to any other version";
          throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }

    return LatestPvlToBinary(network);

    while (version != LATEST_PVL_VERSION) {
      int previousVersion = version;

      switch (version) {
        case 1:
          ConvertVersion1ToVersion2(network);
          break;

        case 2:
          ConvertVersion2ToVersion3(network);
          break;

        case 3:
          ConvertVersion3ToVersion4(network);
          break;

        default:
          IString msg = "The Pvl file version [" + IString(version) + "] is not"
              " supported";
          throw IException(IException::Unknown, msg, _FILEINFO_);
      }

      version = toInt(network["Version"][0]);

      if (version == previousVersion) {
        IString msg = "Cannot update from version [" + IString(version) + "] "
            "to any other version";
          throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }

    return LatestPvlToBinary(network);
    int version = toInt(network["Version"][0]);

    while (version != LATEST_PVL_VERSION) {
      int previousVersion = version;

      switch (version) {
        case 1:
          ConvertVersion1ToVersion2(network);
          break;

        case 2:
          ConvertVersion2ToVersion3(network);
          break;

        case 3:
          ConvertVersion3ToVersion4(network);
          break;

        default:
          IString msg = "The Pvl file version [" + IString(version) + "] is not"
              " supported";
          throw IException(IException::Unknown, msg, _FILEINFO_);
      }

      version = toInt(network["Version"][0]);

      if (version == previousVersion) {
        IString msg = "Cannot update from version [" + IString(version) + "] "
            "to any other version";
          throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }

    return LatestPvlToBinary(network);
   *
   * The update cycle is contained in this method. Old versions of Pvl will be
   *   updated until they reach the latest version and then LatestPvlToBinary
   *   will be called to convert it back to a LatestControlNetFile.
   *
   * To add a new version, you only need to add a case to the switch that
   *   calls a method (ConvertVersionAToVersionB). No other code should be
   *   necessary. ConvertVersionAToVersionB is expected to update the Pvl's
   *   version number.
   *
   * @param pvl The pvl network obtained from Pvl::Read on the input filename
   */
  LatestControlNetFile *ControlNetVersioner::ReadPvlNetwork(Pvl pvl) {

    PvlObject &network = pvl.findObject("ControlNetwork");

    if (!network.hasKeyword("Version"))
      network += PvlKeyword("Version", "1");

    int version = toInt(network["Version"][0]);

    while (version != LATEST_PVL_VERSION) {
      int previousVersion = version;

      switch (version) {
        case 1:
          ConvertVersion1ToVersion2(network);
          break;

        case 2:
          ConvertVersion2ToVersion3(network);
          break;

        case 3:
          ConvertVersion3ToVersion4(network);
          break;

        default:
          IString msg = "The Pvl file version [" + IString(version) + "] is not"
              " supported";
          throw IException(IException::Unknown, msg, _FILEINFO_);
      }

      version = toInt(network["Version"][0]);

      if (version == previousVersion) {
        IString msg = "Cannot update from version [" + IString(version) + "] "
            "to any other version";
          throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }

    return LatestPvlToBinary(network);
  }


  /**
   * Convert a pvl (in the latest version) back to binary (LatestControlNetFile)
   *
   * This does exactly what you think it would do - it copies PvlKeywords into
   *   protocol buffer objects. Helper methods Copy(...) do most of the work.
   *   Any unexpected keywords in the Pvl will cause an exception to be thrown.
   *   Not enough keywords in the Pvl will cause an exception to be thrown.
   *   The returned LatestControlNetFile is guaranteed to have all required
   *   fields.
   *
   * @param network The input PVL Control Network to convert
   */
  LatestControlNetFile *ControlNetVersioner::LatestPvlToBinary(PvlObject &network) {

    LatestControlNetFile *latest = new LatestControlNetFile;

    ControlNetFileHeaderV0002 &header = latest->GetNetworkHeader();

    header.set_networkid(network.findKeyword("NetworkId")[0].toLatin1().data());
    header.set_targetname(network.findKeyword("TargetName")[0].toLatin1().data());
    header.set_created(network.findKeyword("Created")[0].toLatin1().data());
    header.set_lastmodified(network.findKeyword("LastModified")[0].toLatin1().data());
    header.set_description(network.findKeyword("Description")[0].toLatin1().data());
    header.set_username(network.findKeyword("UserName")[0].toLatin1().data());
    header.add_pointmessagesizes(0); // Just to pass the "IsInitialized" test

    if (!header.IsInitialized()) {
      IString msg = "There is missing required information in the network "
          "header";
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    QList<ControlPointFileEntryV0002> &points = latest->GetNetworkPoints();

    for (int objectIndex = 0; objectIndex < network.objects(); objectIndex ++) {
      ControlPointFileEntryV0002 point;
      PvlObject &object = network.object(objectIndex);

      Copy(object, "PointId",
           point, &ControlPointFileEntryV0002::set_id);
      Copy(object, "ChooserName",
           point, &ControlPointFileEntryV0002::set_choosername);
      Copy(object, "DateTime",
           point, &ControlPointFileEntryV0002::set_datetime);
      Copy(object, "AprioriXYZSourceFile",
           point, &ControlPointFileEntryV0002::set_apriorisurfpointsourcefile);
      Copy(object, "AprioriRadiusSourceFile",
           point, &ControlPointFileEntryV0002::set_aprioriradiussourcefile);
      Copy(object, "JigsawRejected",
           point, &ControlPointFileEntryV0002::set_jigsawrejected);
      Copy(object, "EditLock",
           point, &ControlPointFileEntryV0002::set_editlock);
      Copy(object, "Ignore",
           point, &ControlPointFileEntryV0002::set_ignore);
      Copy(object, "AprioriX",
           point, &ControlPointFileEntryV0002::set_apriorix);
      Copy(object, "AprioriY",
           point, &ControlPointFileEntryV0002::set_aprioriy);
      Copy(object, "AprioriZ",
           point, &ControlPointFileEntryV0002::set_aprioriz);
      Copy(object, "AdjustedX",
           point, &ControlPointFileEntryV0002::set_adjustedx);
      Copy(object, "AdjustedY",
           point, &ControlPointFileEntryV0002::set_adjustedy);
      Copy(object, "AdjustedZ",
           point, &ControlPointFileEntryV0002::set_adjustedz);
      Copy(object, "LatitudeConstrained",
           point, &ControlPointFileEntryV0002::set_latitudeconstrained);
      Copy(object, "LongitudeConstrained",
           point, &ControlPointFileEntryV0002::set_longitudeconstrained);
      Copy(object, "RadiusConstrained",
           point, &ControlPointFileEntryV0002::set_radiusconstrained);

      if (object["PointType"][0] == "Fixed")
        point.set_type(ControlPointFileEntryV0002::Fixed);
      else if (object["PointType"][0] == "Constrained")
        point.set_type(ControlPointFileEntryV0002::Constrained);
      else
        point.set_type(ControlPointFileEntryV0002::Free);

      if (object.hasKeyword("AprioriXYZSource")) {
        IString source = object["AprioriXYZSource"][0];

        if (source == "None") {
          point.set_apriorisurfpointsource(ControlPointFileEntryV0002::None);
        }
        else if (source == "User") {
          point.set_apriorisurfpointsource(ControlPointFileEntryV0002::User);
        }
        else if (source == "AverageOfMeasures") {
          point.set_apriorisurfpointsource(
              ControlPointFileEntryV0002::AverageOfMeasures);
        }
        else if (source == "Reference") {
          point.set_apriorisurfpointsource(
              ControlPointFileEntryV0002::Reference);
        }
        else if (source == "Basemap") {
          point.set_apriorisurfpointsource(
              ControlPointFileEntryV0002::Basemap);
        }
        else if (source == "BundleSolution") {
          point.set_apriorisurfpointsource(
              ControlPointFileEntryV0002::BundleSolution);
        }
        else {
          IString msg = "Invalid AprioriXYZSource [" + source + "]";
          throw IException(IException::User, msg, _FILEINFO_);
        }
      }

      if (object.hasKeyword("AprioriRadiusSource")) {
        IString source = object["AprioriRadiusSource"][0];

        if (source == "None") {
          point.set_aprioriradiussource(ControlPointFileEntryV0002::None);
        }
        else if (source == "User") {
          point.set_aprioriradiussource(ControlPointFileEntryV0002::User);
        }
        else if (source == "AverageOfMeasures") {
          point.set_aprioriradiussource(ControlPointFileEntryV0002::AverageOfMeasures);
        }
        else if (source == "Ellipsoid") {
          point.set_aprioriradiussource(ControlPointFileEntryV0002::Ellipsoid);
        }
        else if (source == "DEM") {
          point.set_aprioriradiussource(ControlPointFileEntryV0002::DEM);
        }
        else if (source == "BundleSolution") {
          point.set_aprioriradiussource(ControlPointFileEntryV0002::BundleSolution);
        }
        else {
          std::string msg = "Invalid AprioriRadiusSource, [" + source + "]";
          throw IException(IException::User, msg, _FILEINFO_);
        }
      }

      if (object.hasKeyword("AprioriCovarianceMatrix")) {
        PvlKeyword &matrix = object["AprioriCovarianceMatrix"];

        point.add_aprioricovar(toDouble(matrix[0]));
        point.add_aprioricovar(toDouble(matrix[1]));
        point.add_aprioricovar(toDouble(matrix[2]));
        point.add_aprioricovar(toDouble(matrix[3]));
        point.add_aprioricovar(toDouble(matrix[4]));
        point.add_aprioricovar(toDouble(matrix[5]));
      }

      if (object.hasKeyword("AdjustedCovarianceMatrix")) {
        PvlKeyword &matrix = object["AdjustedCovarianceMatrix"];

        point.add_adjustedcovar(toDouble(matrix[0]));
        point.add_adjustedcovar(toDouble(matrix[1]));
        point.add_adjustedcovar(toDouble(matrix[2]));
        point.add_adjustedcovar(toDouble(matrix[3]));
        point.add_adjustedcovar(toDouble(matrix[4]));
        point.add_adjustedcovar(toDouble(matrix[5]));
      }

      //  Process Measures
      for (int groupIndex = 0; groupIndex < object.groups(); groupIndex ++) {
        PvlGroup &group = object.group(groupIndex);
        ControlPointFileEntryV0002::Measure measure;

        Copy(group, "SerialNumber",
             measure, &ControlPointFileEntryV0002::Measure::set_serialnumber);
        Copy(group, "ChooserName",
             measure, &ControlPointFileEntryV0002::Measure::set_choosername);
        Copy(group, "Sample",
             measure, &ControlPointFileEntryV0002::Measure::set_sample);
        Copy(group, "Line",
             measure, &ControlPointFileEntryV0002::Measure::set_line);
        Copy(group, "SampleResidual",
             measure, &ControlPointFileEntryV0002::Measure::set_sampleresidual);
        Copy(group, "LineResidual",
             measure, &ControlPointFileEntryV0002::Measure::set_lineresidual);
        Copy(group, "DateTime",
             measure, &ControlPointFileEntryV0002::Measure::set_datetime);
        Copy(group, "Diameter",
             measure, &ControlPointFileEntryV0002::Measure::set_diameter);
        Copy(group, "EditLock",
             measure, &ControlPointFileEntryV0002::Measure::set_editlock);
        Copy(group, "Ignore",
             measure, &ControlPointFileEntryV0002::Measure::set_ignore);
        Copy(group, "JigsawRejected",
             measure, &ControlPointFileEntryV0002::Measure::set_jigsawrejected);
        Copy(group, "AprioriSample",
             measure, &ControlPointFileEntryV0002::Measure::set_apriorisample);
        Copy(group, "AprioriLine",
             measure, &ControlPointFileEntryV0002::Measure::set_aprioriline);
        Copy(group, "SampleSigma",
             measure, &ControlPointFileEntryV0002::Measure::set_samplesigma);
        Copy(group, "LineSigma",
             measure, &ControlPointFileEntryV0002::Measure::set_linesigma);

        if (group.hasKeyword("Reference")) {
          if (group["Reference"][0].toLower() == "true")
            point.set_referenceindex(groupIndex);

          group.deleteKeyword("Reference");
        }

        QString type = group["MeasureType"][0].toLower();
        if (type == "candidate")
          measure.set_type(ControlPointFileEntryV0002::Measure::Candidate);
        else if (type == "manual")
          measure.set_type(ControlPointFileEntryV0002::Measure::Manual);
        else if (type == "registeredpixel")
          measure.set_type(ControlPointFileEntryV0002::Measure::RegisteredPixel);
        else if (type == "registeredsubpixel")
          measure.set_type(ControlPointFileEntryV0002::Measure::RegisteredSubPixel);
        else
          throw IException(IException::Io,
                           "Unknown measure type [" + type + "]",
                           _FILEINFO_);
        group.deleteKeyword("MeasureType");

        for (int key = 0; key < group.keywords(); key++) {
          ControlMeasureLogData interpreter(group[key]);
          if (!interpreter.IsValid()) {
            IString msg = "Unhandled or duplicate keywords in control measure ["
                + group[key].name() + "]";
            throw IException(IException::Programmer, msg, _FILEINFO_);
          }
          else {
            *measure.add_log() = interpreter.ToProtocolBuffer();
          }
        }

        *point.add_measures() = measure;
      }

      if (!point.IsInitialized()) {
        IString msg = "There is missing required information in the control "
            "points or measures";
        throw IException(IException::Io, msg, _FILEINFO_);
      }

      points.append(point);
    }

    return latest;
  }


  /**
   * This method is designed to read any and all binary networks. Old versions
   *   will be sent to ReadPvlNetwork.
   *
   * @param header The Pvl at the top of the binary file
   * @param filename The file that contains the binary network
   * @return In-memory representation of the network
   */
  LatestControlNetFile *ControlNetVersioner::ReadBinaryNetwork(const Pvl &header,
                                                               const FileName &filename) {

    // Find the binary cnet version by any means necessary
    int version = 1;

    const PvlObject &protoBuf = header.findObject("ProtoBuffer");
    const PvlGroup &netInfo = protoBuf.findGroup("ControlNetworkInfo");

    if (netInfo.hasKeyword("Version"))
      version = toInt(netInfo["Version"][0]);

    // Okay, let's instantiate the correct ControlNetFile for this version
    ControlNetFile *cnetFile;
    switch (version) {
      case 1:
        cnetFile = new ControlNetFileV0001;
        break;

      case 2:
        cnetFile = new ControlNetFileV0002;
        break;

      default:
        IString msg = "The binary file version [" + IString(version) + "] is "
            "not supported";
        throw IException(IException::Io, msg, _FILEINFO_);
    }

    // Now read and update as necessary
    cnetFile->Read(header, filename);

    if (version != LATEST_BINARY_VERSION) {
      Pvl pvl(cnetFile->toPvl());

      delete cnetFile;
      cnetFile = NULL;

      return ReadPvlNetwork(pvl);
    }
    else {
      return (LatestControlNetFile *)cnetFile;
    }
  }


  /**
   * This converts pvl networks from their implied version 1 to version 2.
   *
   * We're trying to handle all cases of old keywords from over a very long
   *   time in this method, and end up with a consistent set of keywords so
   *   there is no more duplication or confusion about what will be in the Pvl.
   *
   * Future conversions will have similar operations in them but will probably
   *   be smaller/less work.
   *
   * Modify in place to prevent unnecessary memory usage.
   *
   * Version 2 is the first version made inside this versioner. It is the
   *   first time keyword names and values cannot vary.
   *
   * @param network Input is Version 1, must be modified to conform to Version 2
   */
  void ControlNetVersioner::ConvertVersion1ToVersion2(PvlObject &network) {

    network["Version"] = "2";

    // Really... Projection::TargetRadii should be making this call
    NaifStatus::CheckErrors();

    if (QString(network["TargetName"]).startsWith("MRO/")) {
      network["TargetName"] = "Mars";
    }

    PvlGroup radii;
    try {
      radii = Target::radiiGroup(network["TargetName"][0]);
    }
    catch (IException &e) {
      try {
        NaifStatus::CheckErrors();
      }
      catch (IException &) {
      }

      QString msg = "Unable to get convert ControlNet Version 1 to Version 2.";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    Distance equatorialRadius(radii["EquatorialRadius"], Distance::Meters);
    Distance polarRadius(radii["PolarRadius"], Distance::Meters);

    for (int cpIndex = 0; cpIndex < network.objects(); cpIndex ++) {
      PvlObject &cp = network.object(cpIndex);

      if (cp.hasKeyword("Held") && cp["Held"][0] == "True")
        cp["PointType"] = "Ground";

      if (cp.hasKeyword("AprioriLatLonSource"))
        cp["AprioriLatLonSource"].setName("AprioriXYZSource");

      if (cp.hasKeyword("AprioriLatLonSourceFile"))
        cp["AprioriLatLonSourceFile"].setName("AprioriXYZSourceFile");

      if (cp.hasKeyword("AprioriLatitude")) {
        SurfacePoint apriori(
            Latitude(toDouble(cp["AprioriLatitude"][0]), Angle::Degrees),
            Longitude(toDouble(cp["AprioriLongitude"][0]), Angle::Degrees),
            Distance(toDouble(cp["AprioriRadius"][0]), Distance::Meters));

        cp += PvlKeyword("AprioriX", toString(apriori.GetX().meters()), "meters");
        cp += PvlKeyword("AprioriY", toString(apriori.GetY().meters()), "meters");
        cp += PvlKeyword("AprioriZ", toString(apriori.GetZ().meters()), "meters");
      }

      if (cp.hasKeyword("Latitude")) {
        SurfacePoint adjusted(
            Latitude(toDouble(cp["Latitude"][0]), Angle::Degrees),
            Longitude(toDouble(cp["Longitude"][0]), Angle::Degrees),
            Distance(toDouble(cp["Radius"][0]), Distance::Meters));

        cp += PvlKeyword("AdjustedX", toString(adjusted.GetX().meters()), "meters");
        cp += PvlKeyword("AdjustedY", toString(adjusted.GetY().meters()), "meters");
        cp += PvlKeyword("AdjustedZ", toString(adjusted.GetZ().meters()), "meters");

        if (!cp.hasKeyword("AprioriLatitude")) {
          cp += PvlKeyword("AprioriX", toString(adjusted.GetX().meters()), "meters");
          cp += PvlKeyword("AprioriY", toString(adjusted.GetY().meters()), "meters");
          cp += PvlKeyword("AprioriZ", toString(adjusted.GetZ().meters()), "meters");
        }
      }

      if (cp.hasKeyword("X"))
        cp["X"].setName("AdjustedX");

      if (cp.hasKeyword("Y"))
        cp["Y"].setName("AdjustedY");

      if (cp.hasKeyword("Z"))
        cp["Z"].setName("AdjustedZ");

      if (cp.hasKeyword("AprioriSigmaLatitude") ||
         cp.hasKeyword("AprioriSigmaLongitude") ||
         cp.hasKeyword("AprioriSigmaRadius")) {
        double sigmaLat = 10000.0;
        double sigmaLon = 10000.0;
        double sigmaRad = 10000.0;

        if (cp.hasKeyword("AprioriSigmaLatitude")) {
          if (toDouble(cp["AprioriSigmaLatitude"][0]) > 0 &&
              toDouble(cp["AprioriSigmaLatitude"][0]) < sigmaLat)
            sigmaLat = cp["AprioriSigmaLatitude"];

          cp += PvlKeyword("LatitudeConstrained", "True");
        }

        if (cp.hasKeyword("AprioriSigmaLongitude")) {
          if (toDouble(cp["AprioriSigmaLongitude"][0]) > 0 &&
              toDouble(cp["AprioriSigmaLongitude"][0]) < sigmaLon)
            sigmaLon = cp["AprioriSigmaLongitude"];

          cp += PvlKeyword("LongitudeConstrained", "True");
        }

        if (cp.hasKeyword("AprioriSigmaRadius")) {
          if (toDouble(cp["AprioriSigmaRadius"][0]) > 0 &&
              toDouble(cp["AprioriSigmaRadius"][0]) < sigmaRad)
            sigmaRad = cp["AprioriSigmaRadius"];

          cp += PvlKeyword("RadiusConstrained", "True");
        }

        SurfacePoint tmp;
        tmp.SetRadii(equatorialRadius, equatorialRadius, polarRadius);
        tmp.SetRectangular(
            Displacement(cp["AprioriX"], Displacement::Meters),
            Displacement(cp["AprioriY"], Displacement::Meters),
            Displacement(cp["AprioriZ"], Displacement::Meters));
        tmp.SetSphericalSigmasDistance(
          Distance(sigmaLat, Distance::Meters),
          Distance(sigmaLon, Distance::Meters),
          Distance(sigmaRad, Distance::Meters));

        PvlKeyword aprioriCovarMatrix("AprioriCovarianceMatrix");
        aprioriCovarMatrix += toString(tmp.GetRectangularMatrix()(0, 0));
        aprioriCovarMatrix += toString(tmp.GetRectangularMatrix()(0, 1));
        aprioriCovarMatrix += toString(tmp.GetRectangularMatrix()(0, 2));
        aprioriCovarMatrix += toString(tmp.GetRectangularMatrix()(1, 1));
        aprioriCovarMatrix += toString(tmp.GetRectangularMatrix()(1, 2));
        aprioriCovarMatrix += toString(tmp.GetRectangularMatrix()(2, 2));

        cp += aprioriCovarMatrix;
      }

      if (cp.hasKeyword("AdjustedSigmaLatitude") ||
          cp.hasKeyword("AdjustedSigmaLongitude") ||
          cp.hasKeyword("AdjustedSigmaRadius")) {
        double sigmaLat = 10000.0;
        double sigmaLon = 10000.0;
        double sigmaRad = 10000.0;

        if (cp.hasKeyword("AdjustedSigmaLatitude")) {
          if (toDouble(cp["AdjustedSigmaLatitude"][0]) > 0 &&
              toDouble(cp["AdjustedSigmaLatitude"][0]) < sigmaLat)
            sigmaLat = cp["AdjustedSigmaLatitude"];
        }

        if (cp.hasKeyword("AdjustedSigmaLongitude")) {
          if (toDouble(cp["AdjustedSigmaLongitude"][0]) > 0 &&
              toDouble(cp["AdjustedSigmaLongitude"][0]) < sigmaLon)
            sigmaLon = cp["AdjustedSigmaLongitude"];
        }

        if (cp.hasKeyword("AdjustedSigmaRadius")) {
          if (toDouble(cp["AdjustedSigmaRadius"][0]) > 0 &&
              toDouble(cp["AdjustedSigmaRadius"][0]) < sigmaRad)
            sigmaRad = cp["AdjustedSigmaRadius"];
        }

        SurfacePoint tmp;
        tmp.SetRadii(equatorialRadius, equatorialRadius, polarRadius);
        tmp.SetRectangular(Displacement(cp["AdjustedX"], Displacement::Meters),
                           Displacement(cp["AdjustedY"], Displacement::Meters),
                           Displacement(cp["AdjustedZ"], Displacement::Meters));
        tmp.SetSphericalSigmasDistance(Distance(sigmaLat, Distance::Meters),
                                       Distance(sigmaLon, Distance::Meters),
                                       Distance(sigmaRad, Distance::Meters));

        PvlKeyword adjustedCovarMatrix("AdjustedCovarianceMatrix");
        adjustedCovarMatrix += toString(tmp.GetRectangularMatrix()(0, 0));
        adjustedCovarMatrix += toString(tmp.GetRectangularMatrix()(0, 1));
        adjustedCovarMatrix += toString(tmp.GetRectangularMatrix()(0, 2));
        adjustedCovarMatrix += toString(tmp.GetRectangularMatrix()(1, 1));
        adjustedCovarMatrix += toString(tmp.GetRectangularMatrix()(1, 2));
        adjustedCovarMatrix += toString(tmp.GetRectangularMatrix()(2, 2));

        cp += adjustedCovarMatrix;
      }

      if (cp.hasKeyword("ApostCovarianceMatrix"))
        cp["ApostCovarianceMatrix"].setName("AdjustedCovarianceMatrix");

      if (!cp.hasKeyword("LatitudeConstrained")) {
        if (cp.hasKeyword("AprioriCovarianceMatrix"))
          cp += PvlKeyword("LatitudeConstrained", "True");
        else
          cp += PvlKeyword("LatitudeConstrained", "False");
      }

      if (!cp.hasKeyword("LongitudeConstrained")) {
        if (cp.hasKeyword("AprioriCovarianceMatrix"))
          cp += PvlKeyword("LongitudeConstrained", "True");
        else
          cp += PvlKeyword("LongitudeConstrained", "False");
      }

      if (!cp.hasKeyword("RadiusConstrained")) {
        if (cp.hasKeyword("AprioriCovarianceMatrix"))
          cp += PvlKeyword("RadiusConstrained", "True");
        else
          cp += PvlKeyword("RadiusConstrained", "False");
      }

      // Delete anything that has no value...
      for (int cpKeyIndex = 0; cpKeyIndex < cp.keywords(); cpKeyIndex ++) {
        if (cp[cpKeyIndex][0] == "") {
          cp.deleteKeyword(cpKeyIndex);
        }
      }

      for (int cmIndex = 0; cmIndex < cp.groups(); cmIndex ++) {
        PvlGroup &cm = cp.group(cmIndex);

        // Estimated => Candidate
        if (cm.hasKeyword("MeasureType")) {
          QString type = cm["MeasureType"][0].toLower();

          if (type == "estimated" || type == "unmeasured") {
            if (type == "unmeasured") {
              bool hasSampleLine = false;

              try {
                toDouble(cm["Sample"][0]);
                toDouble(cm["Line"][0]);
                hasSampleLine = true;
              }
              catch (...) {
              }

              if (!hasSampleLine) {
                cm.addKeyword(PvlKeyword("Sample", "0.0"), PvlContainer::Replace);
                cm.addKeyword(PvlKeyword("Line", "0.0"), PvlContainer::Replace);
                cm.addKeyword(PvlKeyword("Ignore", toString(true)), PvlContainer::Replace);
              }
            }

            cm["MeasureType"] = "Candidate";
          }
          else if (type == "automatic" ||
                   type == "validatedmanual" ||
                   type == "automaticpixel") {
            cm["MeasureType"] = "RegisteredPixel";
          }
          else if (type == "validatedautomatic" || type == "automaticsubpixel") {
            cm["MeasureType"] = "RegisteredSubPixel";
          }
        }

        if (cm.hasKeyword("ErrorSample"))
          cm["ErrorSample"].setName("SampleResidual");

        if (cm.hasKeyword("ErrorLine"))
          cm["ErrorLine"].setName("LineResidual");

        // Delete some extraneous values we once printed
        if (cm.hasKeyword("SampleResidual") &&
            toDouble(cm["SampleResidual"][0]) == 0.0)
          cm.deleteKeyword("SampleResidual");

        if (cm.hasKeyword("LineResidual") &&
            toDouble(cm["LineResidual"][0]) == 0.0)
          cm.deleteKeyword("LineResidual");

        if (cm.hasKeyword("Diameter") &&
            toDouble(cm["Diameter"][0]) == 0.0)
          cm.deleteKeyword("Diameter");

        if (cm.hasKeyword("ErrorMagnitude"))
          cm.deleteKeyword("ErrorMagnitude");

        if (cm.hasKeyword("ZScore"))
          cm.deleteKeyword("ZScore");

        // Delete anything that has no value...
        for (int cmKeyIndex = 0; cmKeyIndex < cm.keywords(); cmKeyIndex ++) {
          if (cm[cmKeyIndex][0] == "") {
            cm.deleteKeyword(cmKeyIndex);
          }
        }
      }
    }
  }


  /**
   * This converts pvl networks from their version 2 to version 3.
   *
   * Modify in place to prevent unnecessary memory usage.
   *
   * @param network Input is Version 2, must be modified to conform to Version 3
   */
  void ControlNetVersioner::ConvertVersion2ToVersion3(PvlObject &network) {

    network["Version"] = "3";

    for (int cpIndex = 0; cpIndex < network.objects(); cpIndex ++) {
      PvlObject &cp = network.object(cpIndex);

     if (cp.hasKeyword("AprioriCovarianceMatrix") ||
         cp.hasKeyword("AdjustedCovarianceMatrix"))
       cp["PointType"] = "Constrained";
    }
  }


  /**
   * This converts pvl networks from their version 3 to version 4.
   *
   * Modify in place to prevent unnecessary memory usage.
   *
   * @param network Input is Version 3, must be modified to conform to Version 4
   */
  void ControlNetVersioner::ConvertVersion3ToVersion4(PvlObject &network) {

    network["Version"] = "4";

    for (int cpIndex = 0; cpIndex < network.objects(); cpIndex ++) {
      PvlObject &cp = network.object(cpIndex);

     if (cp["PointType"][0] == "Ground") cp["PointType"] = "Fixed";
     if (cp["PointType"][0] == "Tie") cp["PointType"] = "Free";
    }
  }


  /**
   * This is a convenience method for copying keywords out of the container
   *   and into the ControlPointFileEntryV0002 for booleans. This operation is
   *   only necessary for the latest version of the binary so this method needs
   *   to be updated or removed when V0003 comes around.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlObject that represents a control point
   * @param keyName The keyword name inside the PvlObject
   * @param point The protocol buffer point instance to set the value in
   * @param setter The protocol buffer setter method
   */
  void ControlNetVersioner::Copy(PvlContainer &container,
                                 QString keyName,
                                 ControlPointFileEntryV0002 &point,
                                 void (ControlPointFileEntryV0002::*setter)(bool)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    container.deleteKeyword(keyName);
    value = value.toLower();

    if (value == "true" || value == "yes")
      (point.*setter)(true);
  }


  /**
   * This is a convenience method for copying keywords out of the container
   *   and into the ControlPointFileEntryV0002 for doubles. This operation is
   *   only necessary for the latest version of the binary so this method needs
   *   to be updated or removed when V0003 comes around.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlObject that represents a control point
   * @param keyName The keyword name inside the PvlObject
   * @param point The protocol buffer point instance to set the value in
   * @param setter The protocol buffer setter method
   */
  void ControlNetVersioner::Copy(PvlContainer &container,
                                 QString keyName,
                                 ControlPointFileEntryV0002 &point,
                                 void (ControlPointFileEntryV0002::*setter)(double)) {

    if (!container.hasKeyword(keyName))
      return;

    double value = toDouble(container[keyName][0]);
    container.deleteKeyword(keyName);
    (point.*setter)(value);
  }


  /**
   * This is a convenience method for copying keywords out of the container
   *   and into the ControlPointFileEntryV0002 for strings. This operation is
   *   only necessary for the latest version of the binary so this method needs
   *   to be updated or removed when V0003 comes around.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlObject that represents a control point
   * @param keyName The keyword name inside the PvlObject
   * @param point The protocol buffer point instance to set the value in
   * @param setter The protocol buffer setter method
   */
  void ControlNetVersioner::Copy(PvlContainer &container,
                                 QString keyName,
                                 ControlPointFileEntryV0002 &point,
                                 void (ControlPointFileEntryV0002::*setter)(const std::string&)) {

    if (!container.hasKeyword(keyName))
      return;

    IString value = container[keyName][0];
    container.deleteKeyword(keyName);
    (point.*setter)(value);
  }


  /**
   * This is a convenience method for copying keywords out of the container
   *   and into the ControlPointFileEntryV0002::Measure for booleans. This
   *   operation is only necessary for the latest version of the binary so
   *   this method needs to be updated or removed when V0003 comes around.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlObject that represents a control point
   * @param keyName The keyword name inside the PvlObject
   * @param measure The protocol buffer point instance to set the value in
   * @param setter The protocol buffer setter method
   */
  void ControlNetVersioner::Copy(PvlContainer &container,
                                 QString keyName,
                                 ControlPointFileEntryV0002::Measure &measure,
                                 void (ControlPointFileEntryV0002::Measure::*setter)(bool)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    container.deleteKeyword(keyName);
    value = value.toLower();

    if (value == "true" || value == "yes")
      (measure.*setter)(true);
  }


  /**
   * This is a convenience method for copying keywords out of the container
   *   and into the ControlPointFileEntryV0002::Measure for doubles. This
   *   operation is only necessary for the latest version of the binary so
   *   this method needs to be updated or removed when V0003 comes around.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlObject that represents a control point
   * @param keyName The keyword name inside the PvlObject
   * @param measure The protocol buffer point instance to set the value in
   * @param setter The protocol buffer setter method
   */
  void ControlNetVersioner::Copy(PvlContainer &container,
                                 QString keyName,
                                 ControlPointFileEntryV0002::Measure &measure,
                                 void (ControlPointFileEntryV0002::Measure::*setter)(double)) {

    if (!container.hasKeyword(keyName))
      return;

    double value = toDouble(container[keyName][0]);
    container.deleteKeyword(keyName);
    (measure.*setter)(value);
  }


  /**
   * This is a convenience method for copying keywords out of the container
   *   and into the ControlPointFileEntryV0002::Measure for strings. This
   *   operation is only necessary for the latest version of the binary so
   *   this method needs to be updated or removed when V0003 comes around.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlObject that represents a control point
   * @param keyName The keyword name inside the PvlObject
   * @param measure The protocol buffer point instance to set the value in
   * @param set The protocol buffer setter method
   */
  void ControlNetVersioner::Copy(PvlContainer &container,
                                 QString keyName,
                                 ControlPointFileEntryV0002::Measure &measure,
                                 void (ControlPointFileEntryV0002::Measure::*set)
                                      (const std::string &)) {

    if (!container.hasKeyword(keyName))
      return;

    IString value = container[keyName][0];
    container.deleteKeyword(keyName);
    (measure.*set)(value);
  }
#endif
}
