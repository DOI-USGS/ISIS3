#include "ControlNetFileV0002.h"

#include <fstream>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <QList>
#include <QDebug>

#include "ControlMeasureLogData.h"
#include "ControlNetFileV0002.pb.h"
#include "FileName.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifStatus.h"
#include "Pvl.h"
#include "SurfacePoint.h"
#include "Target.h"

using namespace google::protobuf;
using namespace google::protobuf::io;
using boost::numeric::ublas::symmetric_matrix;
using boost::numeric::ublas::upper;
using namespace std;

namespace Isis {
  ControlNetFileV0002::ControlNetFileV0002() {
    p_networkHeader = new ControlNetFileHeaderV0002;
    p_controlPoints = new QList<ControlPointFileEntryV0002>;
  }


  ControlNetFileV0002::~ControlNetFileV0002() {
    delete p_networkHeader;
    delete p_controlPoints;
  }



  /**
   * Reads binary version 2
   *
   * @internal
   * @history 2011-05-02 Debbie A. Cook - Converted to read constrained
   *                     point type
   *
   */
  void ControlNetFileV0002::Read(const Pvl &header, const FileName &file) {
    const PvlObject &protoBufferInfo = header.findObject("ProtoBuffer");
    const PvlObject &protoBufferCore = protoBufferInfo.findObject("Core");

    BigInt headerStartPos = protoBufferCore["HeaderStartByte"];
    BigInt headerLength = protoBufferCore["HeaderBytes"];

    fstream input(file.expanded().toLatin1().data(), ios::in | ios::binary);
    if (!input.is_open()) {
      IString msg = "Failed to open control network file" + file.name();
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    input.seekg(headerStartPos, ios::beg);
    streampos filePos = input.tellg();
    IstreamInputStream headerInStream(&input);
    CodedInputStream headerCodedInStream(&headerInStream);
    // max 512MB, warn at 400MB
    headerCodedInStream.SetTotalBytesLimit(1024 * 1024 * 512,
                                           1024 * 1024 * 400);

    // Now stream the rest of the input into the google protocol buffer.
    try {
      filePos += headerLength;
      int oldLimit = headerCodedInStream.PushLimit(headerLength);
      if (!p_networkHeader->ParseFromCodedStream(&headerCodedInStream)) {
        IString msg = "Failed to read input control net file [" +
            file.name() + "]";
        throw IException(IException::Io, msg, _FILEINFO_);
      }
      headerCodedInStream.PopLimit(oldLimit);

      // Without closing and re-opening the protocol buffers break... no clue
      //   why other than it's got some static data around keeping track
      //   maybe. We need to do this for it to reset it's idea of the total
      //   bytes though. Doing it every time is too expensive - so we're going
      //   to just do it periodically.
      IstreamInputStream *pointInStream = NULL;
      CodedInputStream *pointCodedInStream = NULL;

      for (int cp = 0; cp < p_networkHeader->pointmessagesizes_size(); cp ++) {
        if (cp % 50000 == 0 && pointCodedInStream && pointInStream) {
          delete pointCodedInStream;
          pointCodedInStream = NULL;

          delete pointInStream;
          pointInStream = NULL;
        }

        if (pointInStream == NULL) {
          input.close();
          input.open(file.expanded().toLatin1().data(), ios::in | ios::binary);
          input.seekg(filePos, ios::beg);

          pointInStream = new IstreamInputStream(&input);
          pointCodedInStream = new CodedInputStream(pointInStream);
          // max 512MB, warn at 400MB
          pointCodedInStream->SetTotalBytesLimit(1024 * 1024 * 512,
                                                 1024 * 1024 * 400);
        }

        int size = p_networkHeader->pointmessagesizes(cp);
        oldLimit = pointCodedInStream->PushLimit(size);

        filePos += size;
        ControlPointFileEntryV0002 newPoint;
        newPoint.ParseFromCodedStream(pointCodedInStream);

        if (newPoint.type() == ControlPointFileEntryV0002::obsolete_Tie ||
            newPoint.type() == ControlPointFileEntryV0002::obsolete_Ground) {
          if (newPoint.aprioricovar_size())
            newPoint.set_type(ControlPointFileEntryV0002::Constrained);
        }

        p_controlPoints->append(newPoint);
        pointCodedInStream->PopLimit(oldLimit);
      }

      if (pointCodedInStream) {
        delete pointCodedInStream;
        pointCodedInStream = NULL;
      }

      if (pointInStream) {
        delete pointInStream;
        pointInStream = NULL;
      }
    }
    catch (...) {
      string msg = "Cannot understand binary PB file";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  }

  void ControlNetFileV0002::Write(const FileName &file) const {
    // We need to populate ControlNetFileHeaderV0002::pointMessageSizes
    p_networkHeader->clear_pointmessagesizes();
    BigInt pointsSize = 0;
    BigInt numMeasures = 0;
    for (int cpIndex = 0; cpIndex < p_controlPoints->size(); cpIndex ++) {
      numMeasures += p_controlPoints->at(cpIndex).measures_size();
      int size = p_controlPoints->at(cpIndex).ByteSize();
      pointsSize += size;
      p_networkHeader->add_pointmessagesizes(size);
    }

    streampos coreHeaderSize = p_networkHeader->ByteSize();

    const int labelBytes = 65536;
    fstream output(file.expanded().toLatin1().data(),
                   ios::out | ios::trunc | ios::binary);

    char *blankLabel = new char[labelBytes];
    memset(blankLabel, 0, labelBytes);
    output.write(blankLabel, labelBytes);
    delete [] blankLabel;

    streampos startCoreHeaderPos = output.tellp();

    if (!p_networkHeader->SerializeToOstream(&output)) {
      IString msg = "Failed to write output control network file [" +
          file.name() + "]";
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    streampos curPosition = startCoreHeaderPos + coreHeaderSize;
    for (int cpIndex = 0; cpIndex < p_controlPoints->size(); cpIndex ++) {
      if (!p_controlPoints->at(cpIndex).IsInitialized()) {
        IString msg = "Failed to write output control network file [" +
            file.name() + "] because control points are missing required "
            "fields";
        throw IException(IException::Io, msg, _FILEINFO_);
      }

      if (!p_controlPoints->at(cpIndex).SerializeToOstream(&output)) {
        IString msg = "Failed to write output control network file [" +
            file.name() + "] while attempting to write control points";
        throw IException(IException::Io, msg, _FILEINFO_);
      }

      curPosition += p_controlPoints->at(cpIndex).ByteSize();
    }

    Pvl p;
    PvlObject protoObj("ProtoBuffer");

    PvlObject protoCore("Core");
    protoCore.addKeyword(PvlKeyword("HeaderStartByte",
                         toString((BigInt) startCoreHeaderPos)));
    protoCore.addKeyword(PvlKeyword("HeaderBytes", toString((BigInt) coreHeaderSize)));
    protoCore.addKeyword(PvlKeyword("PointsStartByte",
        toString((BigInt) ( startCoreHeaderPos + coreHeaderSize))));
    protoCore.addKeyword(PvlKeyword("PointsBytes",
        toString(pointsSize)));
    protoObj.addObject(protoCore);

    PvlGroup netInfo("ControlNetworkInfo");
    netInfo.addComment("This group is for informational purposes only");
    netInfo += PvlKeyword("NetworkId", p_networkHeader->networkid().c_str());
    netInfo += PvlKeyword("TargetName", p_networkHeader->targetname().c_str());
    netInfo += PvlKeyword("UserName", p_networkHeader->username().c_str());
    netInfo += PvlKeyword("Created", p_networkHeader->created().c_str());
    netInfo += PvlKeyword("LastModified", p_networkHeader->lastmodified().c_str());
    netInfo += PvlKeyword("Description", p_networkHeader->description().c_str());
    netInfo += PvlKeyword("NumberOfPoints", toString(p_controlPoints->size()));
    netInfo += PvlKeyword("NumberOfMeasures", toString(numMeasures));
    netInfo += PvlKeyword("Version", "2");
    protoObj.addGroup(netInfo);

    p.addObject(protoObj);

    output.seekp(0, ios::beg);
    output << p;
    output << '\n';
    output.close();
  }


  /**
   * Converts binary control net version 2 to pvl version 3
   *
   * @internal
   *   @history 2011-05-02 Debbie A. Cook - Converted to version pvl 3
   *                           instead of 2
   *   @history 2011-05-09 Tracie Sucharski - Add comments for printing apriori
   *                           and adjusted values as lat/lon/radius, and sigmas.
   *   @history 2011-05-16 Tracie Sucharski - Before trying to get radii, make
   *                           sure network has a TargetName.  If not, do not add
   *                           lat/lon/radius comments for SurfacePoints.
   *   @history 2011-06-07 Tracie Sucharski/Debbie A. Cook - Point Type changes
   *                           Ground ----> Fixed
   *                           Tie    ----> Free
   *
   */
  Pvl ControlNetFileV0002::toPvl() const {
    Pvl pvl;
    pvl.addObject(PvlObject("ControlNetwork"));
    PvlObject &network = pvl.findObject("ControlNetwork");

    network += PvlKeyword("NetworkId", p_networkHeader->networkid().c_str());
    network += PvlKeyword("TargetName", p_networkHeader->targetname().c_str());
    network += PvlKeyword("UserName", p_networkHeader->username().c_str());
    network += PvlKeyword("Created", p_networkHeader->created().c_str());
    network += PvlKeyword("LastModified", p_networkHeader->lastmodified().c_str());
    network += PvlKeyword("Description", p_networkHeader->description().c_str());

    // This is the Pvl version we're converting to
    network += PvlKeyword("Version", "3");

    //  Get Target Radii from naif kernel
    PvlGroup pvlRadii;
    QString target = (QString)network.findKeyword("TargetName",Pvl::Traverse);
    if (target != "") {
      try {
        NaifStatus::CheckErrors();
        pvlRadii = Target::radiiGroup(target);
      }
      catch (IException) {
        // leave pvlRadii empty if target is not recognized by NAIF 
      }
    }

    ControlPointFileEntryV0002 binaryPoint;
    foreach(binaryPoint, *p_controlPoints) {
      PvlObject pvlPoint("ControlPoint");

      if (binaryPoint.type() == ControlPointFileEntryV0002::Fixed) {
        pvlPoint += PvlKeyword("PointType", "Fixed");
      }
      else if (binaryPoint.type() == ControlPointFileEntryV0002::Constrained) {
        pvlPoint += PvlKeyword("PointType", "Constrained");
      }
      else {
        pvlPoint += PvlKeyword("PointType", "Free");
      }

      pvlPoint += PvlKeyword("PointId", binaryPoint.id().c_str());
      pvlPoint += PvlKeyword("ChooserName", binaryPoint.choosername().c_str());
      pvlPoint += PvlKeyword("DateTime", binaryPoint.datetime().c_str());

      if (binaryPoint.editlock()) {
        pvlPoint += PvlKeyword("EditLock", "True");
      }

      if (binaryPoint.ignore()) {
        pvlPoint += PvlKeyword("Ignore", "True");
      }

      switch (binaryPoint.apriorisurfpointsource()) {
        case ControlPointFileEntryV0002::None:
          break;
        case ControlPointFileEntryV0002::User:
          pvlPoint += PvlKeyword("AprioriXYZSource", "User");
          break;
        case ControlPointFileEntryV0002::AverageOfMeasures:
          pvlPoint += PvlKeyword("AprioriXYZSource", "AverageOfMeasures");
          break;
        case ControlPointFileEntryV0002::Reference:
          pvlPoint += PvlKeyword("AprioriXYZSource", "Reference");
          break;
        case ControlPointFileEntryV0002::Basemap:
          pvlPoint += PvlKeyword("AprioriXYZSource", "Basemap");
          break;
        case ControlPointFileEntryV0002::BundleSolution:
          pvlPoint += PvlKeyword("AprioriXYZSource", "BundleSolution");
          break;
        case ControlPointFileEntryV0002::Ellipsoid:
        case ControlPointFileEntryV0002::DEM:
          break;
      }

      if (binaryPoint.has_apriorisurfpointsourcefile())
        pvlPoint += PvlKeyword("AprioriXYZSourceFile",
                        binaryPoint.apriorisurfpointsourcefile().c_str());

      switch (binaryPoint.aprioriradiussource()) {
        case ControlPointFileEntryV0002::None:
          break;
        case ControlPointFileEntryV0002::User:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "User");
          break;
        case ControlPointFileEntryV0002::AverageOfMeasures:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "AverageOfMeasures");
          break;
        case ControlPointFileEntryV0002::Reference:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "Reference");
          break;
        case ControlPointFileEntryV0002::Basemap:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "Basemap");
          break;
        case ControlPointFileEntryV0002::BundleSolution:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "BundleSolution");
          break;
        case ControlPointFileEntryV0002::Ellipsoid:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "Ellipsoid");
          break;
        case ControlPointFileEntryV0002::DEM:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "DEM");
          break;
      }

      if (binaryPoint.has_aprioriradiussourcefile())
        pvlPoint += PvlKeyword("AprioriRadiusSourceFile",
                        binaryPoint.aprioriradiussourcefile().c_str());

      if (binaryPoint.has_apriorix()) {
        pvlPoint += PvlKeyword("AprioriX", toString(binaryPoint.apriorix()), "meters");
        pvlPoint += PvlKeyword("AprioriY", toString(binaryPoint.aprioriy()), "meters");
        pvlPoint += PvlKeyword("AprioriZ", toString(binaryPoint.aprioriz()), "meters");

        // Get surface point, convert to lat,lon,radius and output as comment
        SurfacePoint apriori;
        apriori.SetRectangular(
                Displacement(binaryPoint.apriorix(),Displacement::Meters),
                Displacement(binaryPoint.aprioriy(),Displacement::Meters),
                Displacement(binaryPoint.aprioriz(),Displacement::Meters));
        pvlPoint.findKeyword("AprioriX").addComment("AprioriLatitude = " +
                                 toString(apriori.GetLatitude().degrees()) +
                                 " <degrees>");
        pvlPoint.findKeyword("AprioriY").addComment("AprioriLongitude = " +
                                 toString(apriori.GetLongitude().degrees()) +
                                 " <degrees>");
        pvlPoint.findKeyword("AprioriZ").addComment("AprioriRadius = " +
                                 toString(apriori.GetLocalRadius().meters()) +
                                 " <meters>");

        if (binaryPoint.aprioricovar_size()) {
          PvlKeyword matrix("AprioriCovarianceMatrix");
          matrix += toString(binaryPoint.aprioricovar(0));
          matrix += toString(binaryPoint.aprioricovar(1));
          matrix += toString(binaryPoint.aprioricovar(2));
          matrix += toString(binaryPoint.aprioricovar(3));
          matrix += toString(binaryPoint.aprioricovar(4));
          matrix += toString(binaryPoint.aprioricovar(5));
          pvlPoint += matrix;

          if (pvlRadii.hasKeyword("EquatorialRadius")) {
            apriori.SetRadii(
                         Distance(pvlRadii["EquatorialRadius"],Distance::Meters),
                         Distance(pvlRadii["EquatorialRadius"],Distance::Meters),
                         Distance(pvlRadii["PolarRadius"],Distance::Meters));
            symmetric_matrix<double, upper> covar;
            covar.resize(3);
            covar.clear();
            covar(0, 0) = binaryPoint.aprioricovar(0);
            covar(0, 1) = binaryPoint.aprioricovar(1);
            covar(0, 2) = binaryPoint.aprioricovar(2);
            covar(1, 1) = binaryPoint.aprioricovar(3);
            covar(1, 2) = binaryPoint.aprioricovar(4);
            covar(2, 2) = binaryPoint.aprioricovar(5);
            apriori.SetRectangularMatrix(covar);
            QString sigmas = "AprioriLatitudeSigma = " +
                             toString(apriori.GetLatSigmaDistance().meters()) +
                             " <meters>  AprioriLongitudeSigma = " +
                             toString(apriori.GetLonSigmaDistance().meters()) +
                             " <meters>  AprioriRadiusSigma = " +
                             toString(apriori.GetLocalRadiusSigma().meters()) +
                             " <meters>";
            pvlPoint.findKeyword("AprioriCovarianceMatrix").addComment(sigmas);
          }
        }
      }

      if (binaryPoint.latitudeconstrained())
        pvlPoint += PvlKeyword("LatitudeConstrained", "True");

      if (binaryPoint.longitudeconstrained())
        pvlPoint += PvlKeyword("LongitudeConstrained", "True");

      if (binaryPoint.radiusconstrained())
        pvlPoint += PvlKeyword("RadiusConstrained", "True");

      if (binaryPoint.has_adjustedx()) {
        pvlPoint += PvlKeyword("AdjustedX", toString(binaryPoint.adjustedx()), "meters");
        pvlPoint += PvlKeyword("AdjustedY", toString(binaryPoint.adjustedy()), "meters");
        pvlPoint += PvlKeyword("AdjustedZ", toString(binaryPoint.adjustedz()), "meters");

        // Get surface point, convert to lat,lon,radius and output as comment
        SurfacePoint adjusted;
        adjusted.SetRectangular(
                Displacement(binaryPoint.adjustedx(),Displacement::Meters),
                Displacement(binaryPoint.adjustedy(),Displacement::Meters),
                Displacement(binaryPoint.adjustedz(),Displacement::Meters));
        pvlPoint.findKeyword("AdjustedX").addComment("AdjustedLatitude = " +
                                 toString(adjusted.GetLatitude().degrees()) +
                                 " <degrees>");
        pvlPoint.findKeyword("AdjustedY").addComment("AdjustedLongitude = " +
                                 toString(adjusted.GetLongitude().degrees()) +
                                 " <degrees>");
        pvlPoint.findKeyword("AdjustedZ").addComment("AdjustedRadius = " +
                                 toString(adjusted.GetLocalRadius().meters()) +
                                 " <meters>");

        if (binaryPoint.adjustedcovar_size()) {
          PvlKeyword matrix("AdjustedCovarianceMatrix");
          matrix += toString(binaryPoint.adjustedcovar(0));
          matrix += toString(binaryPoint.adjustedcovar(1));
          matrix += toString(binaryPoint.adjustedcovar(2));
          matrix += toString(binaryPoint.adjustedcovar(3));
          matrix += toString(binaryPoint.adjustedcovar(4));
          matrix += toString(binaryPoint.adjustedcovar(5));
          pvlPoint += matrix;

          if (pvlRadii.hasKeyword("EquatorialRadius")) {
            adjusted.SetRadii(
                         Distance(pvlRadii["EquatorialRadius"],Distance::Meters),
                         Distance(pvlRadii["EquatorialRadius"],Distance::Meters),
                         Distance(pvlRadii["PolarRadius"],Distance::Meters));
            symmetric_matrix<double, upper> covar;
            covar.resize(3);
            covar.clear();
            covar(0, 0) = binaryPoint.adjustedcovar(0);
            covar(0, 1) = binaryPoint.adjustedcovar(1);
            covar(0, 2) = binaryPoint.adjustedcovar(2);
            covar(1, 1) = binaryPoint.adjustedcovar(3);
            covar(1, 2) = binaryPoint.adjustedcovar(4);
            covar(2, 2) = binaryPoint.adjustedcovar(5);
            adjusted.SetRectangularMatrix(covar);
            QString sigmas = "AdjustedLatitudeSigma = " +
                             toString(adjusted.GetLatSigmaDistance().meters()) +
                             " <meters>  AdjustedLongitudeSigma = " +
                             toString(adjusted.GetLonSigmaDistance().meters()) +
                             " <meters>  AdjustedRadiusSigma = " +
                             toString(adjusted.GetLocalRadiusSigma().meters()) +
                             " <meters>";
            pvlPoint.findKeyword("AdjustedCovarianceMatrix").addComment(sigmas);
          }
        }
      }

      for (int j = 0; j < binaryPoint.measures_size(); j++) {
        PvlGroup pvlMeasure("ControlMeasure");
        const ControlPointFileEntryV0002_Measure &
            binaryMeasure = binaryPoint.measures(j);
        pvlMeasure += PvlKeyword("SerialNumber", binaryMeasure.serialnumber().c_str());

        switch(binaryMeasure.type()) {
          case ControlPointFileEntryV0002_Measure_MeasureType_Candidate:
            pvlMeasure += PvlKeyword("MeasureType", "Candidate");
            break;
          case ControlPointFileEntryV0002_Measure_MeasureType_Manual:
            pvlMeasure += PvlKeyword("MeasureType", "Manual");
            break;
          case ControlPointFileEntryV0002_Measure_MeasureType_RegisteredPixel:
            pvlMeasure += PvlKeyword("MeasureType", "RegisteredPixel");
            break;
          case ControlPointFileEntryV0002_Measure_MeasureType_RegisteredSubPixel:
            pvlMeasure += PvlKeyword("MeasureType", "RegisteredSubPixel");
            break;
        }

        if (binaryMeasure.has_choosername())
          pvlMeasure += PvlKeyword("ChooserName", binaryMeasure.choosername().c_str());

        if (binaryMeasure.has_datetime())
          pvlMeasure += PvlKeyword("DateTime", binaryMeasure.datetime().c_str());

        if (binaryMeasure.editlock())
          pvlMeasure += PvlKeyword("EditLock", "True");

        if (binaryMeasure.ignore())
          pvlMeasure += PvlKeyword("Ignore", "True");

        if (binaryMeasure.has_sample())
          pvlMeasure += PvlKeyword("Sample", toString(binaryMeasure.sample()));

        if (binaryMeasure.has_line())
          pvlMeasure += PvlKeyword("Line", toString(binaryMeasure.line()));

        if (binaryMeasure.has_diameter())
          pvlMeasure += PvlKeyword("Diameter", toString(binaryMeasure.diameter()));

        if (binaryMeasure.has_apriorisample())
          pvlMeasure += PvlKeyword("AprioriSample", toString(binaryMeasure.apriorisample()));

        if (binaryMeasure.has_aprioriline())
          pvlMeasure += PvlKeyword("AprioriLine", toString(binaryMeasure.aprioriline()));

        if (binaryMeasure.has_samplesigma())
          pvlMeasure += PvlKeyword("SampleSigma", toString(binaryMeasure.samplesigma()),
                                   "pixels");

        if (binaryMeasure.has_samplesigma())
          pvlMeasure += PvlKeyword("LineSigma", toString(binaryMeasure.linesigma()),
                                   "pixels");

        if (binaryMeasure.has_sampleresidual())
          pvlMeasure += PvlKeyword("SampleResidual", toString(binaryMeasure.sampleresidual()),
                                   "pixels");

        if (binaryMeasure.has_lineresidual())
          pvlMeasure += PvlKeyword("LineResidual", toString(binaryMeasure.lineresidual()),
                                   "pixels");

        if (binaryMeasure.has_jigsawrejected()) {
         pvlMeasure += PvlKeyword("JigsawRejected", toString(binaryMeasure.jigsawrejected()));
        }

        for (int logEntry = 0;
            logEntry < binaryMeasure.log_size();
            logEntry ++) {
          const ControlPointFileEntryV0002_Measure_MeasureLogData &log =
                binaryMeasure.log(logEntry);

          ControlMeasureLogData interpreter(log);
          pvlMeasure += interpreter.ToKeyword();
        }

        if (binaryPoint.has_referenceindex() &&
           binaryPoint.referenceindex() == j)
          pvlMeasure += PvlKeyword("Reference", "True");

        pvlPoint.addGroup(pvlMeasure);
      }

      network.addObject(pvlPoint);
    }
    return pvl;
  }
}

