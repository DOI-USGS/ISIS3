#include "ControlNetFileV0001.h"

#include <fstream>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include "ControlNetFileV0001.pb.h"
#include "ControlMeasureLogData.h"
#include "FileName.h"
#include "IException.h"
#include "Progress.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "PvlKeyword.h"

using namespace google::protobuf;
using namespace google::protobuf::io;
using namespace std;

namespace Isis {
  ControlNetFileV0001::ControlNetFileV0001() {
    p_network = new ControlNetFileProtoV0001;
    p_logData = new ControlNetLogDataProtoV0001;
  }


  ControlNetFileV0001::~ControlNetFileV0001() {
    delete p_network;
    delete p_logData;
  }


  void ControlNetFileV0001::Read(const Pvl &head, const FileName &file) {
    const PvlObject &protoBufferInfo = head.FindObject("ProtoBuffer");
    const PvlObject &protoBufferCore = protoBufferInfo.FindObject("Core");

    BigInt coreStartPos = protoBufferCore["StartByte"];
    BigInt coreLength = protoBufferCore["Bytes"];

    fstream input(file.expanded().c_str(), ios::in | ios::binary);
    if (!input.is_open()) {
      iString msg = "Failed to open PB file" + file.name();
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    input.seekg(coreStartPos, ios::beg);
    IstreamInputStream inStream(&input);
    CodedInputStream codedInStream(&inStream);
    codedInStream.PushLimit(coreLength);
    // max 512MB, warn at 400MB
    codedInStream.SetTotalBytesLimit(1024 * 1024 * 512, 1024 * 1024 * 400);

    // Now stream the rest of the input into the google protocol buffer.
    try {
      if (!p_network->ParseFromCodedStream(&codedInStream)) {
        iString msg = "Failed to read input PB file " + file.name();
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }
    catch (IException &e) {
      string msg = "Cannot parse binary PB file";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }
    catch (...) {
      string msg = "Cannot parse binary PB file";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    const PvlObject &logDataInfo = protoBufferInfo.FindObject("LogData");
    BigInt logStartPos = logDataInfo["StartByte"];
    BigInt logLength = logDataInfo["Bytes"];

    input.clear();
    input.seekg(logStartPos, ios::beg);
    IstreamInputStream logInStream(&input);
    CodedInputStream codedLogInStream(&logInStream);
    codedLogInStream.PushLimit(logLength);
    // max 512MB, warn at 400MB
    codedLogInStream.SetTotalBytesLimit(1024 * 1024 * 512, 1024 * 1024 * 400);

    // Now stream the rest of the input into the google protocol buffer.
    try {
      if (!p_logData->ParseFromCodedStream(&codedLogInStream)) {
        iString msg = "Failed to read log data in PB file [" + file.name() + "]";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }
    catch (...) {
      string msg = "Cannot parse binary PB file's log data";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }


  Pvl ControlNetFileV0001::ToPvl() const {
    Pvl pvl;
    pvl.AddObject(PvlObject("ControlNetwork"));
    PvlObject &network = pvl.FindObject("ControlNetwork");

    network += PvlKeyword("NetworkId", p_network->networkid());
    network += PvlKeyword("TargetName", p_network->targetname());
    network += PvlKeyword("UserName", p_network->username());
    network += PvlKeyword("Created", p_network->created());
    network += PvlKeyword("LastModified", p_network->lastmodified());
    network += PvlKeyword("Description", p_network->description());

    // This is the Pvl version we're converting to
    network += PvlKeyword("Version", "1");

    for (int i = 0; i < p_network->points_size(); i++) {
      const ControlNetFileProtoV0001_PBControlPoint &binaryPoint =
          p_network->points(i);
      PvlObject pvlPoint("ControlPoint");

      if(binaryPoint.type() == ControlNetFileProtoV0001_PBControlPoint::Ground)
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
        case ControlNetFileProtoV0001_PBControlPoint::None:
          break;
        case ControlNetFileProtoV0001_PBControlPoint::User:
          pvlPoint += PvlKeyword("AprioriXYZSource", "User");
          break;
        case ControlNetFileProtoV0001_PBControlPoint::AverageOfMeasures:
          pvlPoint += PvlKeyword("AprioriXYZSource", "AverageOfMeasures");
          break;
        case ControlNetFileProtoV0001_PBControlPoint::Reference:
          pvlPoint += PvlKeyword("AprioriXYZSource", "Reference");
          break;
        case ControlNetFileProtoV0001_PBControlPoint::Basemap:
          pvlPoint += PvlKeyword("AprioriXYZSource", "Basemap");
          break;
        case ControlNetFileProtoV0001_PBControlPoint::BundleSolution:
          pvlPoint += PvlKeyword("AprioriXYZSource", "BundleSolution");
          break;
        case ControlNetFileProtoV0001_PBControlPoint::Ellipsoid:
        case ControlNetFileProtoV0001_PBControlPoint::DEM:
          break;
      }

      if (binaryPoint.has_apriorisurfpointsourcefile())
        pvlPoint += PvlKeyword("AprioriXYZSourceFile",
                        binaryPoint.apriorisurfpointsourcefile());

      switch (binaryPoint.aprioriradiussource()) {
        case ControlNetFileProtoV0001_PBControlPoint::None:
          break;
        case ControlNetFileProtoV0001_PBControlPoint::User:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "User");
          break;
        case ControlNetFileProtoV0001_PBControlPoint::AverageOfMeasures:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "AverageOfMeasures");
          break;
        case ControlNetFileProtoV0001_PBControlPoint::Reference:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "Reference");
          break;
        case ControlNetFileProtoV0001_PBControlPoint::Basemap:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "Basemap");
          break;
        case ControlNetFileProtoV0001_PBControlPoint::BundleSolution:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "BundleSolution");
          break;
        case ControlNetFileProtoV0001_PBControlPoint::Ellipsoid:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "Ellipsoid");
          break;
        case ControlNetFileProtoV0001_PBControlPoint::DEM:
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

      if(binaryPoint.latitudeconstrained() &&
         (binaryPoint.aprioricovar_size() || binaryPoint.adjustedcovar_size()))
        pvlPoint += PvlKeyword("LatitudeConstrained", "True");

      if(binaryPoint.longitudeconstrained() &&
         (binaryPoint.aprioricovar_size() || binaryPoint.adjustedcovar_size()))
        pvlPoint += PvlKeyword("LongitudeConstrained", "True");

      if(binaryPoint.radiusconstrained() &&
         (binaryPoint.aprioricovar_size() || binaryPoint.adjustedcovar_size()))
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
        const ControlNetFileProtoV0001_PBControlPoint_PBControlMeasure &
            binaryMeasure = binaryPoint.measures(j);
        pvlMeasure += PvlKeyword("SerialNumber", binaryMeasure.serialnumber());

        switch(binaryMeasure.type()) {
          case ControlNetFileProtoV0001_PBControlPoint_PBControlMeasure_MeasureType_Candidate:
            pvlMeasure += PvlKeyword("MeasureType", "Candidate");
            break;
          case ControlNetFileProtoV0001_PBControlPoint_PBControlMeasure_MeasureType_Manual:
            pvlMeasure += PvlKeyword("MeasureType", "Manual");
            break;
          case ControlNetFileProtoV0001_PBControlPoint_PBControlMeasure_MeasureType_RegisteredPixel:
            pvlMeasure += PvlKeyword("MeasureType", "RegisteredPixel");
            break;
          case ControlNetFileProtoV0001_PBControlPoint_PBControlMeasure_MeasureType_RegisteredSubPixel:
            pvlMeasure += PvlKeyword("MeasureType", "RegisteredSubPixel");
            break;
        }

        pvlMeasure += PvlKeyword("ChooserName", binaryMeasure.choosername());
        pvlMeasure += PvlKeyword("DateTime", binaryMeasure.datetime());

        if(binaryMeasure.editlock())
          pvlMeasure += PvlKeyword("EditLock", "True");

        if(binaryMeasure.ignore())
          pvlMeasure += PvlKeyword("Ignore", "True");

        if(binaryMeasure.has_measurement()) {
          pvlMeasure += PvlKeyword("Sample", binaryMeasure.measurement().sample());
          pvlMeasure += PvlKeyword("Line", binaryMeasure.measurement().line());

          if (binaryMeasure.measurement().has_sampleresidual())
            pvlMeasure += PvlKeyword("SampleResidual",
                binaryMeasure.measurement().sampleresidual(), "pixels");

          if (binaryMeasure.measurement().has_lineresidual())
            pvlMeasure += PvlKeyword("LineResidual",
                binaryMeasure.measurement().lineresidual(), "pixels");
        }

        if (binaryMeasure.has_diameter())
          pvlMeasure += PvlKeyword("Diameter", binaryMeasure.diameter());

        if (binaryMeasure.has_apriorisample())
          pvlMeasure += PvlKeyword("AprioriSample", binaryMeasure.apriorisample());

        if (binaryMeasure.has_aprioriline())
          pvlMeasure += PvlKeyword("AprioriLine", binaryMeasure.aprioriline());

        if (binaryMeasure.has_samplesigma())
          pvlMeasure += PvlKeyword("SampleSigma", binaryMeasure.samplesigma());

        if (binaryMeasure.has_samplesigma())
          pvlMeasure += PvlKeyword("LineSigma", binaryMeasure.linesigma());

         for (int logEntry = 0;
             logEntry <
               p_logData->points(i).measures(j).loggedmeasuredata_size();
             logEntry ++) {
           const ControlNetLogDataProtoV0001_Point_Measure_DataEntry &log =
               p_logData->points(i).measures(j).loggedmeasuredata(logEntry);

           try {
             ControlMeasureLogData interpreter(log);
             pvlMeasure += interpreter.ToKeyword();
           }
           catch(IException &) {
           }
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
