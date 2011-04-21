#include "ControlNetVersioner.h"

#include <string>

#include "ControlNetFile.h"
#include "ControlNetFileV0001.h"
#include "ControlNetFileV0002.h"
#include "ControlNetFileV0002.pb.h"
#include "ControlMeasureLogData.h"
#include "Distance.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifStatus.h"
#include "Progress.h"
#include "Projection.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "SurfacePoint.h"

using namespace std;

namespace Isis {
  /**
   * Read the control network from disk. This will always return the network in
   *   its "latest version" binary form. Generally this will only be called by
   *   ControlNet but a conversion from binary to pvl can make use out of this
   *   also.
   *
   * @param networkFilename The filename of the cnet to be read
   *
   */
  LatestControlNetFile *ControlNetVersioner::Read(
      const Filename &networkFilename) {
    try {
      Pvl network(networkFilename.Expanded());

      if(network.HasObject("ProtoBuffer")) {
        return ReadBinaryNetwork(network, networkFilename);
      }
      else if(network.HasObject("ControlNetwork")) {
        return ReadPvlNetwork(network);
      }
      else {
        iString msg = "Could not determine the control network file type";
        throw iException::Message(iException::Io, msg, _FILEINFO_);
      }
    }
    catch(iException &e) {
      iString msg = "Reading the control network [" + networkFilename.fileName()
          + "] failed";
      throw iException::Message(iException::Io, msg, _FILEINFO_);
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
  void ControlNetVersioner::Write(const Filename &file,
      const LatestControlNetFile &fileData, bool pvl) {
    if(pvl) {
      fileData.ToPvl().Write(file.Expanded());
    }
    else {
      fileData.Write(file);
    }
  }


  /**
   * This interprets a Pvl network of any version. Since we already have the
   *   Pvl in memory (we need it to figure out if it is a Pvl network) it
   *   does not actually call Pvl::Read.
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
    PvlObject &network = pvl.FindObject("ControlNetwork");

    if(!network.HasKeyword("Version"))
      network += PvlKeyword("Version", 1);

    int version = network["Version"][0];

    while(version != LATEST_PVL_VERSION) {
      int previousVersion = version;

      switch(version) {
        case 1:
          ConvertVersion1ToVersion2(network);
          break;

        default:
          iString msg = "The Pvl file version [" + iString(version) + "] is not"
              " supported";
          throw iException::Message(iException::Io, msg, _FILEINFO_);
      }

      version = network["Version"][0];

      if(version == previousVersion) {
        iString msg = "Cannot update from version [" + iString(version) + "] "
            "to any other version";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
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
  LatestControlNetFile *ControlNetVersioner::LatestPvlToBinary(
      PvlObject &network) {
    LatestControlNetFile *latest = new LatestControlNetFile;

    ControlNetFileHeaderV0002 &header = latest->GetNetworkHeader();

    header.set_networkid(network.FindKeyword("NetworkId"));
    header.set_targetname(network.FindKeyword("TargetName"));
    header.set_created(network.FindKeyword("Created"));
    header.set_lastmodified(network.FindKeyword("LastModified"));
    header.set_description(network.FindKeyword("Description"));
    header.set_username(network.FindKeyword("UserName"));
    header.add_pointmessagesizes(0); // Just to pass the "IsInitialized" test

    if(!header.IsInitialized()) {
      iString msg = "There is missing required information in the network "
          "header";
      throw iException::Message(iException::Io, msg, _FILEINFO_);
    }

    QList<ControlPointFileEntryV0002> &points = latest->GetNetworkPoints();

    for(int objectIndex = 0; objectIndex < network.Objects(); objectIndex ++) {
      ControlPointFileEntryV0002 point;
      PvlObject &object = network.Object(objectIndex);

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

      point.set_type( (object["PointType"][0] == "Ground") ?
          ControlPointFileEntryV0002::Ground : ControlPointFileEntryV0002::Tie);

      if (object.HasKeyword("AprioriXYZSource")) {
        iString source = object["AprioriXYZSource"][0];

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
          iString msg = "Invalid AprioriXYZSource [" + source + "]";
          throw iException::Message(iException::User, msg, _FILEINFO_);
        }
      }

      if (object.HasKeyword("AprioriRadiusSource")) {
        iString source = object["AprioriRadiusSource"][0];

        if (source == "None") {
          point.set_aprioriradiussource(ControlPointFileEntryV0002::None);
        }
        else if (source == "User") {
          point.set_aprioriradiussource(ControlPointFileEntryV0002::User);
        }
        else if (source == "AverageOfMeasures") {
          point.set_aprioriradiussource(
              ControlPointFileEntryV0002::AverageOfMeasures);
        }
        else if (source == "Ellipsoid") {
          point.set_aprioriradiussource(ControlPointFileEntryV0002::Ellipsoid);
        }
        else if (source == "DEM") {
          point.set_aprioriradiussource(ControlPointFileEntryV0002::DEM);
        }
        else if (source == "BundleSolution") {
          point.set_aprioriradiussource(
              ControlPointFileEntryV0002::BundleSolution);
        }
        else {
          std::string msg = "Invalid AprioriRadiusSource, [" + source + "]";
          throw iException::Message(iException::User, msg, _FILEINFO_);
        }
      }

      if (object.HasKeyword("AprioriCovarianceMatrix")) {
        PvlKeyword &matrix = object["AprioriCovarianceMatrix"];

        point.add_aprioricovar(matrix[0]);
        point.add_aprioricovar(matrix[1]);
        point.add_aprioricovar(matrix[2]);
        point.add_aprioricovar(matrix[3]);
        point.add_aprioricovar(matrix[4]);
        point.add_aprioricovar(matrix[5]);
      }

      if(object.HasKeyword("AdjustedCovarianceMatrix")) {
        PvlKeyword &matrix = object["AdjustedCovarianceMatrix"];

        point.add_adjustedcovar(matrix[0]);
        point.add_adjustedcovar(matrix[1]);
        point.add_adjustedcovar(matrix[2]);
        point.add_adjustedcovar(matrix[3]);
        point.add_adjustedcovar(matrix[4]);
        point.add_adjustedcovar(matrix[5]);
      }

      //  Process Measures
      for (int groupIndex = 0; groupIndex < object.Groups(); groupIndex ++) {
        PvlGroup &group = object.Group(groupIndex);
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

        if(group.HasKeyword("Reference")) {
          if(group["Reference"][0].DownCase() == "true")
            point.set_referenceindex(groupIndex);

          group.DeleteKeyword("Reference");
        }

        iString type = group["MeasureType"][0].DownCase();
        if(type == "candidate")
          measure.set_type(ControlPointFileEntryV0002::Measure::Candidate);
        else if(type == "manual")
          measure.set_type(ControlPointFileEntryV0002::Measure::Manual);
        else if(type == "registeredpixel")
          measure.set_type(
              ControlPointFileEntryV0002::Measure::RegisteredPixel);
        else if(type == "registeredsubpixel")
          measure.set_type(
              ControlPointFileEntryV0002::Measure::RegisteredSubPixel);
        else
          throw iException::Message(iException::Io,
                                    "Unknown measure type [" + type + "]",
                                    _FILEINFO_);
        group.DeleteKeyword("MeasureType");

        for(int key = 0; key < group.Keywords(); key++) {
          ControlMeasureLogData interpreter(group[key]);
          if(!interpreter.IsValid()) {
            iString msg = "Unhandled or duplicate keywords in control measure ["
                + group[key].Name() + "]";
            throw iException::Message(iException::Programmer, msg, _FILEINFO_);
          }
          else {
            *measure.add_log() = interpreter.ToProtocolBuffer();
          }
        }

        *point.add_measures() = measure;
      }

      if(!point.IsInitialized()) {
        iString msg = "There is missing required information in the control "
            "points or measures";
        throw iException::Message(iException::Io, msg, _FILEINFO_);
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
  LatestControlNetFile *ControlNetVersioner::ReadBinaryNetwork(
      const Pvl &header, const Filename &filename) {
    // Find the binary cnet version by any means necessary
    int version = 1;

    const PvlObject &protoBuf = header.FindObject("ProtoBuffer");
    const PvlGroup &netInfo = protoBuf.FindGroup("ControlNetworkInfo");

    if(netInfo.HasKeyword("Version"))
      version = netInfo["Version"][0];

    // Okay, let's instantiate the correct ControlNetFile for this version
    ControlNetFile *cnetFile;
    switch(version) {
      case 1:
        cnetFile = new ControlNetFileV0001;
        break;

      case 2:
        cnetFile = new ControlNetFileV0002;
        break;

      default:
        iString msg = "The binary file version [" + iString(version) + "] is "
            "not supported";
        throw iException::Message(iException::Io, msg, _FILEINFO_);
    }

    // Now read and update as necessary
    cnetFile->Read(header, filename);

    if(version != LATEST_BINARY_VERSION) {
      Pvl pvl(cnetFile->ToPvl());

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
  void ControlNetVersioner::ConvertVersion1ToVersion2(
      PvlObject &network) {
    network["Version"] = 2;

    // Really... Projection::TargetRadii should be making this call
    NaifStatus::CheckErrors();

    bool error = false;
    PvlGroup radii;
    try {
      radii = Projection::TargetRadii(network["TargetName"]);
    }
    catch(iException &e) {
      error = true;
    }

    // If we had an error then the errors aren't flushed
    bool errorsFlushed = false;
    while(!errorsFlushed) {
      try {
        NaifStatus::CheckErrors();
        errorsFlushed = true;
      }
      catch(iException &e) {
        error = true;
      }
    }

    if(error) {
      iString msg = "The target name is not recognized";
      throw iException::Message(iException::Io, msg, _FILEINFO_);
    }

    Distance equatorialRadius(radii["EquatorialRadius"], Distance::Meters);
    Distance polarRadius(radii["PolarRadius"], Distance::Meters);

    for(int cpIndex = 0; cpIndex < network.Objects(); cpIndex ++) {
      PvlObject &cp = network.Object(cpIndex);

      if(cp.HasKeyword("AprioriLatLonSource"))
        cp["AprioriLatLonSource"].SetName("AprioriXYZSource");

      if(cp.HasKeyword("AprioriLatLonSourceFile"))
        cp["AprioriLatLonSourceFile"].SetName("AprioriXYZSourceFile");

      if(cp.HasKeyword("AprioriLatitude")) {
        SurfacePoint apriori(
            Latitude(cp["AprioriLatitude"][0], Angle::Degrees),
            Longitude(cp["AprioriLongitude"][0], Angle::Degrees),
            Distance(cp["AprioriRadius"][0], Distance::Meters));

        cp += PvlKeyword("AprioriX", apriori.GetX().GetMeters(), "meters");
        cp += PvlKeyword("AprioriY", apriori.GetY().GetMeters(), "meters");
        cp += PvlKeyword("AprioriZ", apriori.GetZ().GetMeters(), "meters");
      }

      if(cp.HasKeyword("Latitude")) {
        SurfacePoint adjusted(
            Latitude(cp["Latitude"][0], Angle::Degrees),
            Longitude(cp["Longitude"][0], Angle::Degrees),
            Distance(cp["Radius"][0], Distance::Meters));

        cp += PvlKeyword("AdjustedX", adjusted.GetX().GetMeters(), "meters");
        cp += PvlKeyword("AdjustedY", adjusted.GetY().GetMeters(), "meters");
        cp += PvlKeyword("AdjustedZ", adjusted.GetZ().GetMeters(), "meters");

        if(!cp.HasKeyword("AprioriLatitude")) {
          cp += PvlKeyword("AprioriX", adjusted.GetX().GetMeters(), "meters");
          cp += PvlKeyword("AprioriY", adjusted.GetY().GetMeters(), "meters");
          cp += PvlKeyword("AprioriZ", adjusted.GetZ().GetMeters(), "meters");
        }
      }

      if(cp.HasKeyword("X"))
        cp["X"].SetName("AdjustedX");

      if(cp.HasKeyword("Y"))
        cp["Y"].SetName("AdjustedY");

      if(cp.HasKeyword("Z"))
        cp["Z"].SetName("AdjustedZ");

      if(cp.HasKeyword("AprioriSigmaLatitude") ||
         cp.HasKeyword("AprioriSigmaLongitude") ||
         cp.HasKeyword("AprioriSigmaRadius")) {
        double sigmaLat = 10000.0;
        double sigmaLon = 10000.0;
        double sigmaRad = 10000.0;

        if(cp.HasKeyword("AprioriSigmaLatitude")) {
          if((double)cp["AprioriSigmaLatitude"][0] > 0 &&
             (double)cp["AprioriSigmaLatitude"][0] < sigmaLat)
            sigmaLat = cp["AprioriSigmaLatitude"];

          cp += PvlKeyword("LatitudeConstrained", "True");
        }

        if(cp.HasKeyword("AprioriSigmaLongitude")) {
          if((double)cp["AprioriSigmaLongitude"][0] > 0 &&
             (double)cp["AprioriSigmaLongitude"][0] < sigmaLon)
            sigmaLon = cp["AprioriSigmaLongitude"];

          cp += PvlKeyword("LongitudeConstrained", "True");
        }

        if(cp.HasKeyword("AprioriSigmaRadius")) {
          if((double)cp["AprioriSigmaRadius"][0] > 0 &&
             (double)cp["AprioriSigmaRadius"][0] < sigmaRad)
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
        aprioriCovarMatrix += tmp.GetRectangularMatrix()(0, 0);
        aprioriCovarMatrix += tmp.GetRectangularMatrix()(0, 1);
        aprioriCovarMatrix += tmp.GetRectangularMatrix()(0, 2);
        aprioriCovarMatrix += tmp.GetRectangularMatrix()(1, 1);
        aprioriCovarMatrix += tmp.GetRectangularMatrix()(1, 2);
        aprioriCovarMatrix += tmp.GetRectangularMatrix()(2, 2);

        cp += aprioriCovarMatrix;
      }

      if(cp.HasKeyword("AdjustedSigmaLatitude") ||
         cp.HasKeyword("AdjustedSigmaLongitude") ||
         cp.HasKeyword("AdjustedSigmaRadius")) {
        double sigmaLat = 10000.0;
        double sigmaLon = 10000.0;
        double sigmaRad = 10000.0;

        if(cp.HasKeyword("AdjustedSigmaLatitude")) {
          if((double)cp["AdjustedSigmaLatitude"][0] > 0 &&
             (double)cp["AdjustedSigmaLatitude"][0] < sigmaLat)
            sigmaLat = cp["AdjustedSigmaLatitude"];
        }

        if(cp.HasKeyword("AdjustedSigmaLongitude")) {
          if((double)cp["AdjustedSigmaLongitude"][0] > 0 &&
             (double)cp["AdjustedSigmaLongitude"][0] < sigmaLon)
            sigmaLon = cp["AdjustedSigmaLongitude"];
        }

        if(cp.HasKeyword("AdjustedSigmaRadius")) {
          if((double)cp["AdjustedSigmaRadius"][0] > 0 &&
             (double)cp["AdjustedSigmaRadius"][0] < sigmaRad)
            sigmaRad = cp["AdjustedSigmaRadius"];
        }

        SurfacePoint tmp;
        tmp.SetRadii(equatorialRadius, equatorialRadius, polarRadius);
        tmp.SetRectangular(
            Displacement(cp["AdjustedX"], Displacement::Meters),
            Displacement(cp["AdjustedY"], Displacement::Meters),
            Displacement(cp["AdjustedZ"], Displacement::Meters));
        tmp.SetSphericalSigmasDistance(
          Distance(sigmaLat, Distance::Meters),
          Distance(sigmaLon, Distance::Meters),
          Distance(sigmaRad, Distance::Meters));

        PvlKeyword adjustedCovarMatrix("AdjustedCovarianceMatrix");
        adjustedCovarMatrix += tmp.GetRectangularMatrix()(0, 0);
        adjustedCovarMatrix += tmp.GetRectangularMatrix()(0, 1);
        adjustedCovarMatrix += tmp.GetRectangularMatrix()(0, 2);
        adjustedCovarMatrix += tmp.GetRectangularMatrix()(1, 1);
        adjustedCovarMatrix += tmp.GetRectangularMatrix()(1, 2);
        adjustedCovarMatrix += tmp.GetRectangularMatrix()(2, 2);

        cp += adjustedCovarMatrix;
      }

      if(cp.HasKeyword("ApostCovarianceMatrix"))
        cp["ApostCovarianceMatrix"].SetName("AdjustedCovarianceMatrix");

      if(!cp.HasKeyword("LatitudeConstrained")) {
        if(cp.HasKeyword("AprioriCovarianceMatrix"))
          cp += PvlKeyword("LatitudeConstrained", "True");
        else
          cp += PvlKeyword("LatitudeConstrained", "False");
      }

//      if(cp.HasKeyword("AprioriCovarianceMatrix") ||
//         cp.HasKeyword("AdjustedCovarianceMatrix"))
//        cp["PointType"] = "Ground";

      if(!cp.HasKeyword("LongitudeConstrained")) {
        if(cp.HasKeyword("AprioriCovarianceMatrix"))
          cp += PvlKeyword("LongitudeConstrained", "True");
        else
          cp += PvlKeyword("LongitudeConstrained", "False");
      }

      if(!cp.HasKeyword("RadiusConstrained")) {
        if(cp.HasKeyword("AprioriCovarianceMatrix"))
          cp += PvlKeyword("RadiusConstrained", "True");
        else
          cp += PvlKeyword("RadiusConstrained", "False");
      }

      // Delete anything that has no value...
      for(int cpKeyIndex = 0; cpKeyIndex < cp.Keywords(); cpKeyIndex ++) {
        if(cp[cpKeyIndex][0] == "") {
          cp.DeleteKeyword(cpKeyIndex);
        }
      }

      for(int cmIndex = 0; cmIndex < cp.Groups(); cmIndex ++) {
        PvlGroup &cm = cp.Group(cmIndex);

        // Estimated => Candidate
        if(cm.HasKeyword("MeasureType")) {
          iString type = cm["MeasureType"][0];
          type.DownCase();

          if(type == "estimated" || type == "unmeasured") {
            cm["MeasureType"] = "Candidate";
          }
          else if(type == "automatic" || type == "validatedmanual" ||
                  type == "automaticpixel") {
            cm["MeasureType"] = "RegisteredPixel";
          }
          else if(type == "validatedautomatic" ||
                  type == "automaticsubpixel") {
            cm["MeasureType"] = "RegisteredSubPixel";
          }
        }

        if(cm.HasKeyword("ErrorSample"))
          cm["ErrorSample"].SetName("SampleResidual");

        if(cm.HasKeyword("ErrorLine"))
          cm["ErrorLine"].SetName("LineResidual");

        // Delete some extraneous values we once printed
        if(cm.HasKeyword("SampleResidual") &&
           (double)cm["SampleResidual"][0] == 0.0)
          cm.DeleteKeyword("SampleResidual");

        if(cm.HasKeyword("LineResidual") &&
           (double)cm["LineResidual"][0] == 0.0)
          cm.DeleteKeyword("LineResidual");

        if(cm.HasKeyword("Diameter") &&
           (double)cm["Diameter"][0] == 0.0)
          cm.DeleteKeyword("Diameter");

        if(cm.HasKeyword("ErrorMagnitude"))
          cm.DeleteKeyword("ErrorMagnitude");

        if(cm.HasKeyword("ZScore"))
          cm.DeleteKeyword("ZScore");

        // Delete anything that has no value...
        for(int cmKeyIndex = 0; cmKeyIndex < cm.Keywords(); cmKeyIndex ++) {
          if(cm[cmKeyIndex][0] == "") {
            cm.DeleteKeyword(cmKeyIndex);
          }
        }
      }
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
      iString keyName, ControlPointFileEntryV0002 &point,
      void (ControlPointFileEntryV0002::*setter)(bool)) {
    if(!container.HasKeyword(keyName))
      return;

    iString value = container[keyName][0];
    container.DeleteKeyword(keyName);
    value.DownCase();
    if(value == "true" || value == "yes")
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
      iString keyName, ControlPointFileEntryV0002 &point,
      void (ControlPointFileEntryV0002::*setter)(double)) {
    if(!container.HasKeyword(keyName))
      return;

    double value = container[keyName][0];
    container.DeleteKeyword(keyName);
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
      iString keyName, ControlPointFileEntryV0002 &point,
      void (ControlPointFileEntryV0002::*setter)(const std::string&)) {
    if(!container.HasKeyword(keyName))
      return;

    iString value = container[keyName][0];
    container.DeleteKeyword(keyName);
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
  void ControlNetVersioner::Copy(PvlContainer &container, iString keyName,
      ControlPointFileEntryV0002::Measure &measure,
      void (ControlPointFileEntryV0002::Measure::*setter)(bool)) {
    if(!container.HasKeyword(keyName))
      return;

    iString value = container[keyName][0];
    container.DeleteKeyword(keyName);
    value.DownCase();
    if(value == "true" || value == "yes")
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
  void ControlNetVersioner::Copy(PvlContainer &container, iString keyName,
      ControlPointFileEntryV0002::Measure &measure,
      void (ControlPointFileEntryV0002::Measure::*setter)(double)) {
    if(!container.HasKeyword(keyName))
      return;

    double value = container[keyName][0];
    container.DeleteKeyword(keyName);
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
  void ControlNetVersioner::Copy(PvlContainer &container, iString keyName,
      ControlPointFileEntryV0002::Measure &measure,
      void (ControlPointFileEntryV0002::Measure::*set)(const std::string &)) {
    if(!container.HasKeyword(keyName))
      return;

    iString value = container[keyName][0];
    container.DeleteKeyword(keyName);
    (measure.*set)(value);
  }
}
