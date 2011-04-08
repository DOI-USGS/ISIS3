#include "ControlNetFileV0002.h"

#include <fstream>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include <QList>

#include "ControlMeasureLogData.h"
#include "ControlNetFileV0002.pb.h"
#include "Filename.h"
#include "iException.h"
#include "Pvl.h"

using namespace google::protobuf;
using namespace google::protobuf::io;
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



  void ControlNetFileV0002::Read(const Pvl &header, const Filename &file) {
    const PvlObject &protoBufferInfo = header.FindObject("ProtoBuffer");
    const PvlObject &protoBufferCore = protoBufferInfo.FindObject("Core");

    BigInt headerStartPos = protoBufferCore["HeaderStartByte"];
    BigInt headerLength = protoBufferCore["HeaderBytes"];

    fstream input(file.Expanded().c_str(), ios::in | ios::binary);
    if (!input.is_open()) {
      iString msg = "Failed to open control network file" + file.fileName();
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    input.seekg(headerStartPos, ios::beg);
    IstreamInputStream inStream(&input);
    CodedInputStream codedInStream(&inStream);
    // max 512MB, warn at 400MB
    codedInStream.SetTotalBytesLimit(1024 * 1024 * 512, 1024 * 1024 * 400);

    // Now stream the rest of the input into the google protocol buffer.
    try {
      int oldLimit = codedInStream.PushLimit(headerLength);
      if (!p_networkHeader->ParseFromCodedStream(&codedInStream)) {
        iString msg = "Failed to read input control net file [" +
            file.fileName() + "]";
        throw iException::Message(iException::Io, msg, _FILEINFO_);
      }

      codedInStream.PopLimit(oldLimit);

      for(int cp = 0; cp < p_networkHeader->pointmessagesizes_size(); cp ++) {
        // We can't re-seek because the proto buffer does weird stuff with
        //   out read position - like it's caching. This would cause problems
        //   if we weren't reading consecutively in the file.
        //input.seekg(curPosition, ios::beg);
        int size = p_networkHeader->pointmessagesizes(cp);
        oldLimit = codedInStream.PushLimit(size);

        ControlPointFileEntryV0002 newPoint;
        newPoint.ParseFromCodedStream(&codedInStream);
        p_controlPoints->append(newPoint);

        codedInStream.PopLimit(oldLimit);
      }
    }
    catch (...) {
      string msg = "Cannot understand binary PB file";
      throw Isis::iException::Message(iException::Io, msg, _FILEINFO_);
    }
  }

  void ControlNetFileV0002::Write(const Filename &file) const {
    // We need to populate ControlNetFileHeaderV0002::pointMessageSizes
    p_networkHeader->clear_pointmessagesizes();
    BigInt pointsSize = 0;
    BigInt numMeasures = 0;
    for(int cpIndex = 0; cpIndex < p_controlPoints->size(); cpIndex ++) {
      numMeasures += p_controlPoints->at(cpIndex).measures_size();
      int size = p_controlPoints->at(cpIndex).ByteSize();
      pointsSize += size;
      p_networkHeader->add_pointmessagesizes(size);
    }

    streampos coreHeaderSize = p_networkHeader->ByteSize();

    const int labelBytes = 65536;
    fstream output(file.Expanded().c_str(),
                   ios::out | ios::trunc | ios::binary);

    char *blankLabel = new char[labelBytes];
    memset(blankLabel, 0, labelBytes);
    output.write(blankLabel, labelBytes);
    delete [] blankLabel;

    streampos startCoreHeaderPos = output.tellp();

    if (!p_networkHeader->SerializeToOstream(&output)) {
      iString msg = "Failed to write output control network file [" +
          file.fileName() + "]";
      throw iException::Message(iException::Io, msg, _FILEINFO_);
    }

    streampos curPosition = startCoreHeaderPos + coreHeaderSize;
    for(int cpIndex = 0; cpIndex < p_controlPoints->size(); cpIndex ++) {
      if(!p_controlPoints->at(cpIndex).IsInitialized()) {
        iString msg = "Failed to write output control network file [" +
            file.fileName() + "] because control points are missing required "
            "fields";
        throw iException::Message(iException::Io, msg, _FILEINFO_);
      }

      if(!p_controlPoints->at(cpIndex).SerializeToOstream(&output)) {
        iString msg = "Failed to write output control network file [" +
            file.fileName() + "] while attempting to write control points";
        throw iException::Message(iException::Io, msg, _FILEINFO_);
      }

      curPosition += p_controlPoints->at(cpIndex).ByteSize();
    }

    Pvl p;
    PvlObject protoObj("ProtoBuffer");

    PvlObject protoCore("Core");
    protoCore.AddKeyword(PvlKeyword("HeaderStartByte",
        iString(startCoreHeaderPos)));
    protoCore.AddKeyword(PvlKeyword("HeaderBytes", iString(coreHeaderSize)));
    protoCore.AddKeyword(PvlKeyword("PointsStartByte",
        iString(startCoreHeaderPos + coreHeaderSize)));
    protoCore.AddKeyword(PvlKeyword("PointsBytes",
        iString(pointsSize)));
    protoObj.AddObject(protoCore);

    PvlGroup netInfo("ControlNetworkInfo");
    netInfo.AddComment("This group is for informational purposes only");
    netInfo += PvlKeyword("NetworkId", p_networkHeader->networkid());
    netInfo += PvlKeyword("TargetName", p_networkHeader->targetname());
    netInfo += PvlKeyword("UserName", p_networkHeader->username());
    netInfo += PvlKeyword("Created", p_networkHeader->created());
    netInfo += PvlKeyword("LastModified", p_networkHeader->lastmodified());
    netInfo += PvlKeyword("Description", p_networkHeader->description());
    netInfo += PvlKeyword("NumberOfPoints", p_controlPoints->size());
    netInfo += PvlKeyword("NumberOfMeasures", numMeasures);
    netInfo += PvlKeyword("Version", "2");
    protoObj.AddGroup(netInfo);

    p.AddObject(protoObj);

    output.seekp(0, ios::beg);
    output << p;
    output << '\n';
    output.close();
  }


  Pvl ControlNetFileV0002::ToPvl() const {
    Pvl pvl;
    pvl.AddObject(PvlObject("ControlNetwork"));
    PvlObject &network = pvl.FindObject("ControlNetwork");

    network += PvlKeyword("NetworkId", p_networkHeader->networkid());
    network += PvlKeyword("TargetName", p_networkHeader->targetname());
    network += PvlKeyword("UserName", p_networkHeader->username());
    network += PvlKeyword("Created", p_networkHeader->created());
    network += PvlKeyword("LastModified", p_networkHeader->lastmodified());
    network += PvlKeyword("Description", p_networkHeader->description());

    // This is the Pvl version we're converting to
    network += PvlKeyword("Version", "2");

    ControlPointFileEntryV0002 binaryPoint;
    foreach(binaryPoint, *p_controlPoints) {
      PvlObject pvlPoint("ControlPoint");

      if(binaryPoint.type() == ControlPointFileEntryV0002::Ground)
        pvlPoint += PvlKeyword("PointType", "Ground");
      else
        pvlPoint += PvlKeyword("PointType", "Tie");

      pvlPoint += PvlKeyword("PointId", binaryPoint.id());
      pvlPoint += PvlKeyword("ChooserName", binaryPoint.choosername());
      pvlPoint += PvlKeyword("DateTime", binaryPoint.datetime());

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
                        binaryPoint.apriorisurfpointsourcefile());

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
                        binaryPoint.aprioriradiussourcefile());

      if(binaryPoint.has_apriorix()) {
        pvlPoint += PvlKeyword("AprioriX", binaryPoint.apriorix(), "meters");
        pvlPoint += PvlKeyword("AprioriY", binaryPoint.aprioriy(), "meters");
        pvlPoint += PvlKeyword("AprioriZ", binaryPoint.aprioriz(), "meters");

        if(binaryPoint.aprioricovar_size()) {
          PvlKeyword matrix("AprioriCovarianceMatrix");
          matrix += binaryPoint.aprioricovar(0);
          matrix += binaryPoint.aprioricovar(1);
          matrix += binaryPoint.aprioricovar(2);
          matrix += binaryPoint.aprioricovar(3);
          matrix += binaryPoint.aprioricovar(4);
          matrix += binaryPoint.aprioricovar(5);
          pvlPoint += matrix;
        }
      }

      if(binaryPoint.latitudeconstrained())
        pvlPoint += PvlKeyword("LatitudeConstrained", "True");

      if(binaryPoint.longitudeconstrained())
        pvlPoint += PvlKeyword("LongitudeConstrained", "True");

      if(binaryPoint.radiusconstrained())
        pvlPoint += PvlKeyword("RadiusConstrained", "True");

      if(binaryPoint.has_adjustedx()) {
        pvlPoint += PvlKeyword("AdjustedX", binaryPoint.adjustedx(), "meters");
        pvlPoint += PvlKeyword("AdjustedY", binaryPoint.adjustedy(), "meters");
        pvlPoint += PvlKeyword("AdjustedZ", binaryPoint.adjustedz(), "meters");

        if(binaryPoint.adjustedcovar_size()) {
          PvlKeyword matrix("AdjustedCovarianceMatrix");
          matrix += binaryPoint.adjustedcovar(0);
          matrix += binaryPoint.adjustedcovar(1);
          matrix += binaryPoint.adjustedcovar(2);
          matrix += binaryPoint.adjustedcovar(3);
          matrix += binaryPoint.adjustedcovar(4);
          matrix += binaryPoint.adjustedcovar(5);
          pvlPoint += matrix;
        }
      }

      for (int j = 0; j < binaryPoint.measures_size(); j++) {
        PvlGroup pvlMeasure("ControlMeasure");
        const ControlPointFileEntryV0002_Measure &
            binaryMeasure = binaryPoint.measures(j);
        pvlMeasure += PvlKeyword("SerialNumber", binaryMeasure.serialnumber());

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

        if(binaryMeasure.has_choosername())
          pvlMeasure += PvlKeyword("ChooserName", binaryMeasure.choosername());

        if(binaryMeasure.has_datetime())
          pvlMeasure += PvlKeyword("DateTime", binaryMeasure.datetime());

        if(binaryMeasure.editlock())
          pvlMeasure += PvlKeyword("EditLock", "True");

        if(binaryMeasure.ignore())
          pvlMeasure += PvlKeyword("Ignore", "True");

        if(binaryMeasure.has_sample())
          pvlMeasure += PvlKeyword("Sample", binaryMeasure.sample());

        if(binaryMeasure.has_line())
          pvlMeasure += PvlKeyword("Line", binaryMeasure.line());

        if (binaryMeasure.has_diameter())
          pvlMeasure += PvlKeyword("Diameter", binaryMeasure.diameter());

        if (binaryMeasure.has_apriorisample())
          pvlMeasure += PvlKeyword("AprioriSample", binaryMeasure.apriorisample());

        if (binaryMeasure.has_aprioriline())
          pvlMeasure += PvlKeyword("AprioriLine", binaryMeasure.aprioriline());

        if (binaryMeasure.has_samplesigma())
          pvlMeasure += PvlKeyword("SampleSigma", binaryMeasure.samplesigma(),
                                   "pixels");

        if (binaryMeasure.has_samplesigma())
          pvlMeasure += PvlKeyword("LineSigma", binaryMeasure.linesigma(),
                                   "pixels");

        if(binaryMeasure.has_sampleresidual())
          pvlMeasure += PvlKeyword("SampleResidual", binaryMeasure.sampleresidual(),
                                   "pixels");

        if(binaryMeasure.has_lineresidual())
          pvlMeasure += PvlKeyword("LineResidual", binaryMeasure.lineresidual(),
                                   "pixels");

        for(int logEntry = 0;
            logEntry < binaryMeasure.log_size();
            logEntry ++) {
          const ControlPointFileEntryV0002_Measure_MeasureLogData &log =
                binaryMeasure.log(logEntry);

          ControlMeasureLogData interpreter(log);
          pvlMeasure += interpreter.ToKeyword();
        }

        if(binaryPoint.has_referenceindex() &&
           binaryPoint.referenceindex() == j)
          pvlMeasure += PvlKeyword("Reference", "True");

        pvlPoint.AddGroup(pvlMeasure);
      }

      network.AddObject(pvlPoint);
    }
    return pvl;
  }
}

