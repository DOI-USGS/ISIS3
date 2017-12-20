#include "ControlNetVersioner.h"

#include <string>

#include <QDebug>

#include "ControlNetFile.h"
#include "ControlNetFileV0001.h"
#include "ControlNetFileV0002.h"
#include "ControlNetFileHeaderV0002.pb.h"
#include "ControlNetFileHeaderV0005.pb.h"
#include "ControlPointFileEntryV0002.pb.h"
#include "ControlMeasureLogData.h"
#include "Distance.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Latitude.h"
#include "LinearAlgebra.h"
#include "Longitude.h"
#include "NaifStatus.h"
#include "Progress.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "PvlObject.h"
#include "SurfacePoint.h"
#include "Target.h"

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

using namespace google::protobuf::io;
using namespace std;

namespace Isis {

  ControlNetVersioner::ControlNetVersioner(QSharedPointer<ControlNet> net) {
    // Populate the internal list of points.
    for (int i = 0; i < net.GetNumPoints(); i++) {
        m_points.add( QSharedPointer<ControlPoint>( net.GetPoints().at(i) ) );
    }

    ControlNetHeaderV0001 header;

    header.networkID = net.GetNetworkId();
    header.targetName = net.GetTarget();
    header.created = net.CreatedDate();
    header.lastModified = net.GetLastModified();
    header.description = net.Description();
    header.userName = net.GetUserName();
    createHeader(header);
  }


  ControlNetVersioner::ControlNetVersioner(const FileName netFile) {
    read(netFile);
  }


  ControlNetVersioner::~ControlNetVersioner() {

  }


  /**
   * Returns the ID for the network.
   *
   * @return @b QString The network ID as a string
   */
  QString ControlNetVersioner::netId() const {
    return m_header->networkID;
  }


  /**
   * Returns the target for the network.
   *
   * @return @b QString The target name as a string
   */
  QString ControlNetVersioner::targetName() const {
    return m_header->targetName;
  }


  /**
   * Returns the date and time that the network was created
   *
   * @return @b QString The date and time the network was created as a string
   */
  QString ControlNetVersioner::creationDate() const {
    return m_header->created;
  }


  /**
   * Returns the date and time of the last modification to the network.
   *
   * @return @b QString The date and time of the last modfication as a string
   */
  QString ControlNetVersioner::lastModificationDate() const {
    return m_header->lastModified;
  }


  /**
   * Returns the network's description.
   *
   * @return @b QString A description of the network.
   */
  QString ControlNetVersioner::description() const {
    return m_header->description;
  }


  /**
   * Returns the name of the last person or program to modify the network.
   *
   * @retrun @b QString The name of the last person or program to modify the network.
   */
  QString ControlNetVersioner::userName() const {
    return m_header->userName;
  }


  QSharedPointer<ControlPoint> ControlNetVersioner::takeFirstPoint() {

  }





  /**
   * Generates a Pvl file from the currently stored control points and header.
   *
   * @return Pvl& The Pvl version of the network
   */
  Pvl &ControlNetVersioner::toPvl(){
    Pvl pvl;
    pvl.addObject(PvlObject("ControlNetwork"));
    PvlObject &network = pvl.findObject("ControlNetwork");

    network += PvlKeyword("NetworkId", m_header.networkid().c_str());
    network += PvlKeyword("TargetName", m_header.targetname().c_str());
    network += PvlKeyword("UserName", m_header.username().c_str());
    network += PvlKeyword("Created", m_header.created().c_str());
    network += PvlKeyword("LastModified", m_header.lastmodified().c_str());
    network += PvlKeyword("Description", m_header.description().c_str());
    // optionally add username to output?

    // This is the Pvl version we're converting to
    network += PvlKeyword("Version", "5");

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

    ControlPoint controlPoint;
    foreach(controlPoint, *m_points) {
      PvlObject pvlPoint("ControlPoint");

      if (controlPoint.GetType() == ControlPoint::Fixed) {
        pvlPoint += PvlKeyword("PointType", "Fixed");
      }
      else if (controlPoint.GetType() == ControlPoint::Constrained) {
        pvlPoint += PvlKeyword("PointType", "Constrained");
      }
      else {
        pvlPoint += PvlKeyword("PointType", "Free");
      }

      pvlPoint += PvlKeyword("PointId", controlPoint.GetId());
      pvlPoint += PvlKeyword("ChooserName", controlPoint.GetChooserName());
      pvlPoint += PvlKeyword("DateTime", controlPoint.GetDateTime());

      if (controlPoint.IsEditLocked()) {
        pvlPoint += PvlKeyword("EditLock", "True");
      }
      if (controlPoint.IsIgnored()) {
        pvlPoint += PvlKeyword("Ignore", "True");
      }

      switch (controlPoint.GetAprioriSurfPointSource()) {
        case ControlPoint::SurfacePointSouce::None:
          break;
        case ControlPoint::SurfacePointSouce::User:
          pvlPoint += PvlKeyword("AprioriXYZSource", "User");
          break;
        case ControlPoint::SurfacePointSouce::AverageOfMeasures:
          pvlPoint += PvlKeyword("AprioriXYZSource", "AverageOfMeasures");
          break;
        case ControlPoint::SurfacePointSouce::Reference:
          pvlPoint += PvlKeyword("AprioriXYZSource", "Reference");
          break;
        case ControlPoint::SurfacePointSouce::Basemap:
          pvlPoint += PvlKeyword("AprioriXYZSource", "Basemap");
          break;
        case ControlPoint::SurfacePointSouce::BundleSolution:
          pvlPoint += PvlKeyword("AprioriXYZSource", "BundleSolution");
          break;
        case ControlPoint::RadiusSource::Ellipsoid:
        case ControlPoint::RadiusSource::DEM:
          break;
      }

      if (controlPoint.HasAprioriSurfacePointSourceFile()) {
        pvlPoint += PvlKeyword("AprioriXYZSourceFile",
                        controlPoint.GetAprioriSurfacePointSourceFile());
      }

      switch (controlPoint.GetAprioriRadiusSource()) {
        case ControlPoint::RadiusSource::None:
          break;
        case ControlPoint::RadiusSource::User:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "User");
          break;
        case ControlPoint::RadiusSource::AverageOfMeasures:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "AverageOfMeasures");
          break;
        case ControlPoint::RadiusSource::Reference:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "Reference");
          break;
        case ControlPoint::RadiusSource::Basemap:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "Basemap");
          break;
        case ControlPoint::RadiusSource::BundleSolution:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "BundleSolution");
          break;
        case ControlPoint::RadiusSource::Ellipsoid:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "Ellipsoid");
          break;
        case ControlPoint::RadiusSource::DEM:
          pvlPoint += PvlKeyword("AprioriRadiusSource", "DEM");
          break;
      }


      if (controlPoint.HasAprioriRadiusSourceFile()) {
        pvlPoint += PvlKeyword("AprioriRadiusSourceFile",
                        protobufPoint.GetAprioriRadiusSourceFile());
        }

      if (controlPoint.HasAprioriCoordinates()) {
        pvlPoint += PvlKeyword("AprioriX", toString(controlPoint.GetAprioriX()), "meters");
        pvlPoint += PvlKeyword("AprioriY", toString(controlPoint.GetAprioriY()), "meters");
        pvlPoint += PvlKeyword("AprioriZ", toString(controlPoint.GetAprioriZ()), "meters");

        // Get surface point, convert to lat,lon,radius and output as comment
        SurfacePoint apriori;
        apriori.SetRectangular(
                Displacement(controlPoint.GetAprioriX(),Displacement::Meters),
                Displacement(controlPoint.GetAprioriY(),Displacement::Meters),
                Displacement(controlPoint.GetAprioriZ(),Displacement::Meters));
        pvlPoint.findKeyword("AprioriX").addComment("AprioriLatitude = " +
                                 toString(apriori.GetLatitude().degrees()) +
                                 " <degrees>");
        pvlPoint.findKeyword("AprioriY").addComment("AprioriLongitude = " +
                                 toString(apriori.GetLongitude().degrees()) +
                                 " <degrees>");
        pvlPoint.findKeyword("AprioriZ").addComment("AprioriRadius = " +
                                 toString(apriori.GetLocalRadius().meters()) +
                                 " <meters>");

        // FIXME: None of Covariance matrix information is available directly from ControlPoint in the API
        if (controlPoint.aprioricovar_size()) { // DNE
          PvlKeyword matrix("AprioriCovarianceMatrix");
          matrix += toString(controlPoint.aprioricovar(0)); // DNE
          matrix += toString(controlPoint.aprioricovar(1)); // DNE
          matrix += toString(controlPoint.aprioricovar(2)); // DNE
          matrix += toString(controlPoint.aprioricovar(3)); // DNE
          matrix += toString(controlPoint.aprioricovar(4)); // DNE
          matrix += toString(controlPoint.aprioricovar(5)); // DNE
          pvlPoint += matrix;

          if (pvlRadii.hasKeyword("EquatorialRadius")) {
            apriori.SetRadii(
                         Distance(pvlRadii["EquatorialRadius"],Distance::Meters),
                         Distance(pvlRadii["EquatorialRadius"],Distance::Meters),
                         Distance(pvlRadii["PolarRadius"],Distance::Meters));
            symmetric_matrix<double, upper> covar;
            covar.resize(3);
            covar.clear();
            covar(0, 0) = controlPoint.aprioricovar(0); // DNE
            covar(0, 1) = controlPoint.aprioricovar(1); // DNE
            covar(0, 2) = controlPoint.aprioricovar(2); // DNE
            covar(1, 1) = controlPoint.aprioricovar(3); // DNE
            covar(1, 2) = controlPoint.aprioricovar(4); // ""
            covar(2, 2) = controlPoint.aprioricovar(5); // ""
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

      if (controlPoint.IsLatitudeConstrained()) {
        pvlPoint += PvlKeyword("LatitudeConstrained", "True");
      }

      if (controlPoint.IsLongitudeConstrained()) {
        pvlPoint += PvlKeyword("LongitudeConstrained", "True");
      }

      if (controlPoint.IsRadiusConstrained()) {
        pvlPoint += PvlKeyword("RadiusConstrained", "True");
      }

      if (controlPoint.HasAdjustedX()) {
        pvlPoint += PvlKeyword("AdjustedX", toString(controlPoint.AdjustedX()), "meters");
        pvlPoint += PvlKeyword("AdjustedY", toString(controlPoint.AdjustedY()), "meters");
        pvlPoint += PvlKeyword("AdjustedZ", toString(controlPoint.AdjustedZ()), "meters");

        // Get surface point, convert to lat,lon,radius and output as comment
        SurfacePoint adjusted;
        adjusted.SetRectangular(
                Displacement(controlPoint.AdjustedX(),Displacement::Meters),
                Displacement(controlPoint.adjustedY(),Displacement::Meters),
                Displacement(controlPoint.adjustedZ(),Displacement::Meters));
        pvlPoint.findKeyword("AdjustedX").addComment("AdjustedLatitude = " +
                                 toString(adjusted.GetLatitude().degrees()) +
                                 " <degrees>");
        pvlPoint.findKeyword("AdjustedY").addComment("AdjustedLongitude = " +
                                 toString(adjusted.GetLongitude().degrees()) +
                                 " <degrees>");
        pvlPoint.findKeyword("AdjustedZ").addComment("AdjustedRadius = " +
                                 toString(adjusted.GetLocalRadius().meters()) +
                                 " <meters>");

        if (controlPoint.AdjustedCovarSize()) { // DNE
          PvlKeyword matrix("AdjustedCovarianceMatrix");
          matrix += toString(controlPoint.AdjustedCovar(0));
          matrix += toString(controlPoint.AdjustedCovar(1));
          matrix += toString(controlPoint.AdjustedCovar(2));
          matrix += toString(controlPoint.AdjustedCovar(3));
          matrix += toString(controlPoint.AdjustedCovar(4));
          matrix += toString(controlPoint.AdjustedCovar(5));
          pvlPoint += matrix;

          if (pvlRadii.hasKeyword("EquatorialRadius")) {
            adjusted.SetRadii(
                         Distance(pvlRadii["EquatorialRadius"],Distance::Meters),
                         Distance(pvlRadii["EquatorialRadius"],Distance::Meters),
                         Distance(pvlRadii["PolarRadius"],Distance::Meters));
            symmetric_matrix<double, upper> covar;
            covar.resize(3);
            covar.clear();
            covar(0, 0) = controlPoint.AdjustedCovar(0);
            covar(0, 1) = controlPoint.AdjustedCovar(1);
            covar(0, 2) = controlPoint.AdjustedCovar(2);
            covar(1, 1) = controlPoint.AdjustedCovar(3);
            covar(1, 2) = controlPoint.AdjustedCovar(4);
            covar(2, 2) = controlPoint.AdjustedCovar(5);
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

      for (int j = 0; j < controlPoint.GetNumMeasures(); j++) {
        PvlGroup pvlMeasure("ControlMeasure");
        const ControlMeasure &
            controlMeasure = controlPoint.GetMeasures(j);
        pvlMeasure += PvlKeyword("SerialNumber", controlMeasure.GetCubeSerialNumber());

        switch(controlMeasure.GetType()) {
          case ControlMeasure::Candidate:
            pvlMeasure += PvlKeyword("MeasureType", "Candidate");
            break;
          case ControlMeasure::Manual:
            pvlMeasure += PvlKeyword("MeasureType", "Manual");
            break;
          case ControlMeasure::RegisteredPixel:
            pvlMeasure += PvlKeyword("MeasureType", "RegisteredPixel");
            break;
          case ControlMeasure::RegisteredSubPixel:
            pvlMeasure += PvlKeyword("MeasureType", "RegisteredSubPixel");
            break;
        }

        if (controlMeasure.HasChooserName()) {
          pvlMeasure += PvlKeyword("ChooserName", controlMeasure.GetChooserName());
        }

        if (controlMeasure.HasDateTime()) {
          pvlMeasure += PvlKeyword("DateTime", controlMeasure.GetDateTime());
        }

        if (controlMeasure.IsEditLocked()) {
          pvlMeasure += PvlKeyword("EditLock", "True");
        }

        if (controlMeasure.IsIgnored()) {
          pvlMeasure += PvlKeyword("Ignore", "True");
        }

        if (controlMeasure.HasSample()) {
          pvlMeasure += PvlKeyword("Sample", toString(controlMeasure.GetSample());
        }

        if (controlMeasure.HasLine()) {
          pvlMeasure += PvlKeyword("Line", toString(controlMeasure.GetLine()));
        }

        if (controlMeasure.HasDiameter()) {
          pvlMeasure += PvlKeyword("Diameter", toString(controlMeasure.GetDiameter()));
        }

        if (controlMeasure.HasAprioriSample()) {
          pvlMeasure += PvlKeyword("AprioriSample", toString(controlMeasure.GetAprioriSample()));
        }

        if (controlMeasure.HasAprioriLine()) {
          pvlMeasure += PvlKeyword("AprioriLine", toString(controlMeasure.GetAprioriLine()));
        }

        if (controlMeasure.HasSampleSigma()) {
          pvlMeasure += PvlKeyword("SampleSigma", toString(controlMeasure.GetSampleSigma()),
                                   "pixels");
        }

        if (controlMeasure.HasLineSigma()) {
          pvlMeasure += PvlKeyword("LineSigma", toString(controlMeasure.GetLineSigma()),
                                   "pixels");
        }

        if (controlMeasure.HasSampleResidual()) {
          pvlMeasure += PvlKeyword("SampleResidual", toString(controlMeasure.GetSampleResidual())
                                   "pixels");
        }

        if (controlMeasure.HasLineResidual()) {
          pvlMeasure += PvlKeyword("LineResidual", toString(controlMeasure.GetLineResidual()),
                                   "pixels");
        }

        pvlMeasure += PvlKeyword("JigsawRejected", toString(controlMeasure.IsJigsawRejected()));

        for (int logEntry = 0;
            logEntry < controlMeasure.LogSize(); // DNE?
            logEntry ++) {
          const ControlMeasureLogData &log =
                controlMeasure.GetLogData(logEntry); // Not sure this is right.

          ControlMeasureLogData interpreter(log);
          pvlMeasure += interpreter.ToKeyword();
        }

        if (controlPoint.HasRefMeasure() &&
           controlPoint.IndexOfRefMeasure() == j) {
          pvlMeasure += PvlKeyword("Reference", "True");
        }
        pvlPoint.addGroup(pvlMeasure);
      }
      network.addObject(pvlPoint);
    }
    return pvl;
  }


  /**
   * Read a control network file and prepare the data to be converted into
   * a control network.
   *
   * @param netFile The control network file to read.
   */
  void ControlNetVersioner::read(const FileName netFile) {
    try {
      const Pvl &network(netFile.expanded());

      if (network.hasObject("ProtoBuffer")) {
        readProtobuf(network, netFile);
      }
      else if (network.hasObject("ControlNetwork")) {
        readPvl(network);
      }
      else {
        QString msg = "Could not determine the control network file type";
        throw IException(IException::Io, msg, _FILEINFO_);
      }
    }
    catch (IException &e) {
      QString msg = "Reading the control network [" + netFile.name()
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
      const PvlObject &controlNetwork = network.findObject("ControlNetwork");

      int version = 1

      if (controlNetwork.hasKeyword("Version")) {
        version = toInt(controlNetwork["Version"][0]);
      }

      switch (version) {
        case 1:
          readPvlV0001(controlNetwork);
          break;

        case 2:
          readPvlV0002(controlNetwork);
          break;

        case 3:
          readPvlV0003(controlNetwork);
          break;

        case 4:
          readPvlV0004(controlNetwork);
          break;

        case 5:
          readPvlV0005(controlNetwork);
          break;

        default:
          QString msg = "The Pvl file version [" + toString(version)
                        + "] is not supported";
          throw IException(IException::Unknown, msg, _FILEINFO_);
      }
  }


  /**
   * read a version 1 Pvl control network and convert the data into control points.
   *
   * @param network The control network PvlObject.
   */
  void ControlNetVersioner::readPvlV0001(const PvlObject &network) {
    // initialize the header
    try {
      ControlNetHeaderV0001 header;
      header.networkID = network.findKeyword("NetworkId")[0];
      header.targetName = network.findKeyword("TargetName")[0];
      header.created = network.findKeyword("Created")[0];
      header.lastModified = network.findKeyword("LastModified")[0];
      header.description = network.findKeyword("Description")[0];
      header.userName = network.findKeyword("UserName")[0];
      createHeader(header);
    }
    catch (IException &e) {
      QString msg = "Missing required header information.";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    // initialize the control points
    for (int objectIndex = 0; objectIndex < network.objects(); objectIndex ++) {
      try {
        PvlObject &pointObject = network.object(objectIndex);
        ControlPointV0001 point(pointObject, header.targetName);
        m_points.append( createPoint(point) );
      }
      catch (IException &e) {
        QString msg = "Failed to initialize control point at index ["
                      + toString(objectIndex) + "].";
        throw IException(e, IException::Io, msg, _FILEINFO_);
      }
    }
  }


  /**
   * read a version 2 Pvl control network and convert the data into control points.
   *
   * @param network The control network PvlObject.
   */
  void ControlNetVersioner::readPvlV0002(const PvlObject &network) {
    // initialize the header
    try {
      ControlNetHeaderV0002 header;
      header.networkID = network.findKeyword("NetworkId")[0];
      header.targetName = network.findKeyword("TargetName")[0];
      header.created = network.findKeyword("Created")[0];
      header.lastModified = network.findKeyword("LastModified")[0];
      header.description = network.findKeyword("Description")[0];
      header.userName = network.findKeyword("UserName")[0];
      createHeader(header);
    }
    catch (IException &e) {
      QString msg = "Missing required header information.";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    // initialize the control points
    for (int objectIndex = 0; objectIndex < network.objects(); objectIndex ++) {
      try {
        PvlObject &pointObject = network.object(objectIndex);
        ControlPointV0002 point(pointObject);
        m_points.append( createPoint(point) );
      }
      catch (IException &e) {
        QString msg = "Failed to initialize control point at index ["
                      + toString(objectIndex) + "].";
        throw IException(e, IException::Io, msg, _FILEINFO_);
      }
    }
  }


  /**
   * read a version 3 Pvl control network and convert the data into control points.
   *
   * @param network The control network PvlObject.
   */
  void ControlNetVersioner::readPvlV0003(const PvlObject &network) {
    // initialize the header
    try {
      ControlNetHeaderV0003 header;
      header.networkID = network.findKeyword("NetworkId")[0];
      header.targetName = network.findKeyword("TargetName")[0];
      header.created = network.findKeyword("Created")[0];
      header.lastModified = network.findKeyword("LastModified")[0];
      header.description = network.findKeyword("Description")[0];
      header.userName = network.findKeyword("UserName")[0];
      createHeader(header);
    }
    catch (IException &e) {
      QString msg = "Missing required header information.";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    // initialize the control points
    for (int objectIndex = 0; objectIndex < network.objects(); objectIndex ++) {
      try {
        PvlObject &pointObject = network.object(objectIndex);
        ControlPointV0003 point(pointObject);
        m_points.append( createPoint(point) );
      }
      catch (IException &e) {
        QString msg = "Failed to initialize control point at index ["
                      + toString(objectIndex) + "].";
        throw IException(e, IException::Io, msg, _FILEINFO_);
      }
    }
  }


  /**
   * read a version 4 Pvl control network and convert the data into control points.
   *
   * @param network The control network PvlObject.
   */
  void ControlNetVersioner::readPvlV0004(const PvlObject &network) {
    // initialize the header
    try {
      ControlNetHeaderV0004 header;
      header.networkID = network.findKeyword("NetworkId")[0];
      header.targetName = network.findKeyword("TargetName")[0];
      header.created = network.findKeyword("Created")[0];
      header.lastModified = network.findKeyword("LastModified")[0];
      header.description = network.findKeyword("Description")[0];
      header.userName = network.findKeyword("UserName")[0];
      createHeader(header);
    }
    catch (IException &e) {
      QString msg = "Missing required header information.";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    // initialize the control points
    for (int objectIndex = 0; objectIndex < network.objects(); objectIndex ++) {
      try {
        PvlObject &pointObject = network.object(objectIndex);
        ControlPointV0004 point(pointObject);
        m_points.append( createPoint(point) );
      }
      catch (IException &e) {
        QString msg = "Failed to initialize control point at index ["
                      + toString(objectIndex) + "].";
        throw IException(e, IException::Io, msg, _FILEINFO_);
      }
    }
  }


  /**
   * read a version 5 Pvl control network and convert the data into control points.
   *
   * @param network The control network PvlObject.
   */
  void ControlNetVersioner::readPvlV0005(const PvlObject &network) {
    // initialize the header
    try {
      ControlNetHeaderV0005 header;
      header.networkID = network.findKeyword("NetworkId")[0];
      header.targetName = network.findKeyword("TargetName")[0];
      header.created = network.findKeyword("Created")[0];
      header.lastModified = network.findKeyword("LastModified")[0];
      header.description = network.findKeyword("Description")[0];
      header.userName = network.findKeyword("UserName")[0];
      createHeader(header);
    }
    catch (IException &e) {
      QString msg = "Missing required header information.";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    // initialize the control points
    for (int objectIndex = 0; objectIndex < network.objects(); objectIndex ++) {
      try {
        PvlObject &pointObject = network.object(objectIndex);
        ControlPointV0005 point(pointObject);
        m_points.append( createPoint(point) );
      }
      catch (IException &e) {
        QString msg = "Failed to initialize control point at index ["
                      + toString(objectIndex) + "].";
        throw IException(e, IException::Io, msg, _FILEINFO_);
      }
    }
  }


  /**
   * Read a protobuf control network and prepare the data to be converted into a
   * control network.
   *
   * @param header The Pvl network header that contains the version number.
   * @param netFile The filename of the control network file.
   */
  void ControlNetVersioner::readProtobuf(const Pvl &header, const FileName netFile) {
    int version = 1;

    const PvlObject &protoBuf = header.findObject("ProtoBuffer");
    const PvlGroup &netInfo = protoBuf.findGroup("ControlNetworkInfo");

    if (netInfo.hasKeyword("Version")) {
      version = toInt(netInfo["Version"][0]);
    }

    switch (version) {
      case 1:
        readProtobufV0001(header, netFile);
        break;

      case 2:
        readProtobufV0002(header, netFile);
        break;

      case 5:
        readProtobufV0005(header, netFile);
        break;

      default:
        QString msg = "The Protobuf file version [" + toString(version)
                      + "] is not supported";
        throw IException(IException::Io, msg, _FILEINFO_);
    }
  }


  /**
   * Read a protobuf version 1 control network and prepare the data to be
   *  converted into a control network.
   *
   * @param netFile The filename of the control network file.
   */
  void ControlNetVersioner::readProtobufV0001(const Pvl &header, const FileName netFile) {
    const PvlObject &protoBufferInfo = header.findObject("ProtoBuffer");
    const PvlObject &protoBufferCore = protoBufferInfo.findObject("Core");

    BigInt coreStartPos = protoBufferCore["StartByte"];
    BigInt coreLength = protoBufferCore["Bytes"];

    fstream input(netFile.expanded().toLatin1().data(), ios::in | ios::binary);
    if (!input.is_open()) {
      QString msg = "Failed to open protobuf file [" + netFile.name() + "].";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    input.seekg(coreStartPos, ios::beg);
    IstreamInputStream inStream(&input);
    CodedInputStream codedInStream(&inStream);
    codedInStream.PushLimit(coreLength);
    // max 512MB, warn at 400MB
    codedInStream.SetTotalBytesLimit(1024 * 1024 * 512, 1024 * 1024 * 400);

    // Now stream the rest of the input into the google protocol buffer.
    ControlNetFileProtoV0001 protoNet;
    try {
      if (!protoNet.ParseFromCodedStream(&codedInStream)) {
        QString msg = "Failed to read input PB file [" + netFile.name() + "].";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }
    catch (IException &e) {
      QString msg = "Cannot parse binary protobuf file";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }
    catch (...) {
      QString msg = "Cannot parse binary PB file";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    const PvlObject &logDataInfo = protoBufferInfo.findObject("LogData");
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
    ControlNetLogDataProtoV0001 protoLogData;
    try {
      if (protoLogData.ParseFromCodedStream(&codedLogInStream)) {
        QString msg = "Failed to read log data in protobuf file [" + netFile.name() + "].";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }
    catch (...) {
      QString msg = "Cannot parse binary protobuf file's log data";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Create the header
    try {
      ControlNetHeaderV0001 header;
      header.networkID = protoNet.networkid().c_str();
      if (protoNet.has_targetname()) {
        header.targetName = protoNet.targetname().c_str();
      }
      else {
        header.targetName = "";
      }
      header.created = protoNet.created().c_str();
      header.lastModified = protoNet.lastmodified().c_str();
      header.description = protoNet.description().c_str();
      header.userName = protoNet.username().c_str();
      createHeader(header);
    }
    catch (IException &e) {
      QString msg = "Failed to parse the header from the protobuf control network file.";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }

    // Create the control points
    for (int i = 0; i < protoNet.points_size(); i++) {
      try {
        QSharedPointer<ControlNetFileProtoV0001_PBControlPoint>
              protoPoint(protoNet.mutable_points.points(i));
        ControlPointV0001 point(protoPoint);
        m_points.append( createPoint(point) );
      }
      catch (IException &e) {
        QString msg = "Failed to convert version 1 protobuf control point at index ["
                      + toString(i) + "] to a ControlPoint.";
        throw IException(e, IException::User, msg, _FILEINFO_);
      }
    }

    // TODO how to parse the version 1 log data?
  }


  /**
   * Read a protobuf version 2 control network and prepare the data to be
   *  converted into a control network.
   *
   * @param netFile The filename of the control network file.
   */
  void ControlNetVersioner::readProtobufV0002(const Pvl &header, const FileName netFile) {
    // read the header protobuf object
    const PvlObject &protoBufferInfo = header.findObject("ProtoBuffer");
    const PvlObject &protoBufferCore = protoBufferInfo.findObject("Core");

    BigInt headerStartPos = protoBufferCore["HeaderStartByte"];
    BigInt headerLength = protoBufferCore["HeaderBytes"];

    fstream input(netFile.expanded().toLatin1().data(), ios::in | ios::binary);
    if (!input.is_open()) {
      IString msg = "Failed to open control network file" + netFile.name();
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    input.seekg(headerStartPos, ios::beg);
    streampos filePos = input.tellg();

    ControlNetFileHeaderV0002 protoHeader;
    try {
      IstreamInputStream headerInStream(&input);
      CodedInputStream headerCodedInStream(&headerInStream);
      // max 512MB, warn at 400MB
      headerCodedInStream.SetTotalBytesLimit(1024 * 1024 * 512,
                                             1024 * 1024 * 400);
      CodedInputStream::Limit oldLimit = headerCodedInStream.PushLimit(headerLength);
      if (!protoHeader.ParseFromCodedStream(&headerCodedInStream)) {
        QString msg = "Failed to parse protobuf header from input control net file ["
                      + netFile.name() + "]";
        throw IException(IException::Io, msg, _FILEINFO_);
      }
      headerCodedInStream.PopLimit(oldLimit);
      filePos += headerLength;
    }
    catch (...) {
      QString msg = "An error occured while reading the protobuf control network header.";
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    // initialize the header from the protobuf header
    try {
      ControlNetHeaderV0004 header;
      header.networkID = protoHeader.networkid().c_str();
      if (protoHeader.has_targetname()) {
        header.targetName = protoHeader.targetname().c_str();
      }
      else {
        header.targetName = "";
      }
      header.created = protoHeader.created().c_str();
      header.lastModified = protoHeader.lastmodified().c_str();
      header.description = protoHeader.description().c_str();
      header.userName = protoHeader.username().c_str();
      createHeader(header);
    }
    catch (IException &e) {
      QString msg = "Missing required header information.";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    // read each protobuf control point and then initialize it
    // For some reason, reading the header causes the input stream to fail so reopen the file
    input.close();
    input.open(file.expanded().toLatin1().data(), ios::in | ios::binary);
    input.seekg(filePos, ios::beg);
    IstreamInputStream pointInStream(&input);
    int numPoints = protoHeader.pointmessagesizes_size();
    for (int pointIndex = 0; pointIndex < numPoints; pointIndex ++) {
      ControlPointFileEntryV0002 newPoint;

      try {
        CodedInputStream pointCodedInStream = CodedInputStream(&pointInStream);
        pointCodedInStream.SetTotalBytesLimit(1024 * 1024 * 512,
                                              1024 * 1024 * 400);
        int pointSize = protoHeader.pointmessagesizes(pointIndex);
        CodedInputStream::Limit oldPointLimit = pointCodedInStream.PushLimit(pointSize);
        newPoint.ParseFromCodedStream(&pointCodedInStream);
        pointCodedInStream.PopLimit(oldPointLimit);
      }
      catch (...) {
        QString msg = "Failed to read protobuf version 2 control point at index ["
                      + toString(pointIndex) + "].";
        throw IException(IException::Io, msg, _FILEINFO_);
      }

      try {
        ControlPointV0004 point(newPoint);
        m_points.append( createPoint(point) );
      }
      catch (IException &e) {
        QString msg = "Failed to convert protobuf version 2 control point at index ["
                      + toString(pointIndex) + "] into a ControlPoint.";
        throw IException(e, IException::Io, msg, _FILEINFO_);
      }
    }
  }


  /**
   * Read a protobuf version 5 control network and prepare the data to be
   *  converted into a control network.
   *
   * @param netFile The filename of the control network file.
   */
  void ControlNetVersioner::readProtobufV0005(const Pvl &header, const FileName netFile) {
    // read the header protobuf object
    const PvlObject &protoBufferInfo = header.findObject("ProtoBuffer");
    const PvlObject &protoBufferCore = protoBufferInfo.findObject("Core");

    BigInt headerStartPos = protoBufferCore["HeaderStartByte"];
    BigInt headerLength = protoBufferCore["HeaderBytes"];
    BigInt pointsStartPos = protoBufferCore["PointsStartByte"];
    BigInt pointsLength = protoBufferCore["PointsBytes"];

    fstream input(netFile.expanded().toLatin1().data(), ios::in | ios::binary);
    if (!input.is_open()) {
      IString msg = "Failed to open control network file" + netFile.name();
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    input.seekg(headerStartPos, ios::beg);
    streampos filePos = input.tellg();

    ControlNetFileHeaderV0005 protoHeader;
    try {
      IstreamInputStream headerInStream(&input);
      CodedInputStream headerCodedInStream(&headerInStream);
      // max 512MB, warn at 400MB
      headerCodedInStream.SetTotalBytesLimit(1024 * 1024 * 512,
                                             1024 * 1024 * 400);
      CodedInputStream::Limit oldLimit = headerCodedInStream.PushLimit(headerLength);
      if (!protoHeader.ParseFromCodedStream(&headerCodedInStream)) {
        QString msg = "Failed to parse protobuf header from input control net file ["
                      + netFile.name() + "]";
        throw IException(IException::Io, msg, _FILEINFO_);
      }
      headerCodedInStream.PopLimit(oldLimit);
      filePos += headerLength;
    }
    catch (...) {
      QString msg = "An error occured while reading the protobuf control network header.";
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    // initialize the header from the protobuf header
    try {
      ControlNetHeaderV0005 header;
      header.networkID = protoHeader.networkid().c_str();
      if (protoHeader.has_targetname()) {
        header.targetName = protoHeader.targetname().c_str();
      }
      else {
        header.targetName = "";
      }
      header.created = protoHeader.created().c_str();
      header.lastModified = protoHeader.lastmodified().c_str();
      header.description = protoHeader.description().c_str();
      header.userName = protoHeader.username().c_str();
      createHeader(header);
    }
    catch (IException &e) {
      QString msg = "Missing required header information.";
      throw IException(e, IException::Io, msg, _FILEINFO_);
    }

    // read each protobuf control point and then initialize it
    // For some reason, reading the header causes the input stream to fail so reopen the file
    input.close();
    input.open(file.expanded().toLatin1().data(), ios::in | ios::binary);
    input.seekg(filePos, ios::beg);
    IstreamInputStream pointInStream(&input);
    while (pointInStream.ByteCount() < pointsLength) {
      ControlPointFileEntryV0002 newPoint;

      try {
        CodedInputStream pointCodedInStream = CodedInputStream(&pointInStream);
        pointCodedInStream.SetTotalBytesLimit(1024 * 1024 * 512,
                                              1024 * 1024 * 400);
        uint32_t size;
        if (!input.ReadVarint32(&size)) {
          // If we can't read another size, then assume at eof
          break;
        }
        CodedInputStream::Limit oldPointLimit = pointCodedInStream.PushLimit(size);
        newPoint.ParseFromCodedStream(&pointCodedInStream);
        pointCodedInStream.PopLimit(oldPointLimit);
      }
      catch (...) {
        QString msg = "Failed to read protobuf version 2 control point at index ["
                      + toString(pointIndex) + "].";
        throw IException(IException::Io, msg, _FILEINFO_);
      }

      try {
        ControlPointV0005 point(newPoint);
        m_points.append( createPoint(point) );
      }
      catch (IException &e) {
        QString msg = "Failed to convert protobuf version 2 control point at index ["
                      + toString(pointIndex) + "] in a ControlPoint.";
        throw IException(e, IException::Io, msg, _FILEINFO_);
      }
    }
  }


  /**
   * Create a pointer to a latest version ControlPoint from an
   * object in a V0001 control net file. This method converts a
   * ControlPointV0001 to the latest ControlPontV#### version
   * and uses the latest versioned point to construct and fill an
   * Isis::ControlPoint.
   *
   * @param point The versioned control point to be updated.
   *
   * @return The latest version ControlPoint constructed from the
   *         given point.
   */
  QSharedPointer<ControlPoint> ControlNetVersioner::createPoint(const ControlPointV0001 point) {

    ControlPointV0002 newPoint(point);
    return createPoint(newPoint);

  }


  /**
   * Create a pointer to a latest version ControlPoint from an
   * object in a V0002 control net file. This method converts a
   * ControlPointV0002 to the latest ControlPontV#### version
   * and uses the latest versioned point to construct and fill an
   * Isis::ControlPoint.
   *
   * @param point The versioned control point to be updated.
   *
   * @return The latest version ControlPoint constructed from the
   *         given point.
   */
  QSharedPointer<ControlPoint> ControlNetVersioner::createPoint(const ControlPointV0002 point) {

    ControlPointV0003 newPoint(point);
    return createPoint(newPoint);

  }


  /**
   * Create a pointer to a latest version ControlPoint from an
   * object in a V0003 control net file. This method converts a
   * ControlPointV0003 to the latest ControlPontV#### version
   * and uses the latest versioned point to construct and fill an
   * Isis::ControlPoint.
   *
   * @param point The versioned control point to be updated.
   *
   * @return The latest version ControlPoint constructed from the
   *         given point.
   */
  QSharedPointer<ControlPoint> ControlNetVersioner::createPoint(const ControlPointV0003 point) {

    ControlPointFileEntryV0002 protoPoint = point.pointData();
    QSharedPointer<ControlPoint> controlPoint =
          new QSharedPointer<ControlPoint>(protoPoint.id().c_str());
    controlPoint->SetChooserName(protoPoint.chooserName().c_str());

    // setting point type
    ControlPoint::PointType pointType;
    switch (protoPoint.type()) {
      case ControlPointFileEntryV0002_PointType_obsolete_Tie:
      case ControlPointFileEntryV0002_PointType_Free:
        pointType = Free;
        break;
      case ControlPointFileEntryV0002_PointType_Constrained:
        pointType = Constrained;
        break;
      case ControlPointFileEntryV0002_PointType_obsolete_Ground:
      case ControlPointFileEntryV0002_PointType_Fixed:
        pointType = Fixed;
        break;
      default:
        QString msg = "Unable to create ControlPoint [" + protoPoint.id().c_str() + "] from file. "
                      "Type enumeration [" + toString((int)(protoPoint.type())) + "] is invalid.";
        throw IException(IException::Programmer, msg, _FILEINFO_);
        break;
    }
    controlPoint->SetType(pointType);

    // get radius values for surface points
    Distance equatorialRadius;
    Distance polarRadius;
    if (m_header.has_targetname()) {
      try {
        // attempt to get target radii values...
        PvlGroup pvlRadii = Target::radiiGroup(m_header.targetname().c_str());
        equatorialRadius.setMeters(pvlRadii["EquatorialRadius"]);
        polarRadius.setMeters(pvlRadii["PolarRadius"]);
       }
       catch (IException &e) {
         // do nothing
       }
    }

    controlPoint->SetIgnored(protoPoint.ignore());
    controlPoint->SetRejected(protoPoint.jigsawrejected());

    // setting apriori radius information
    if (protoPoint.has_aprioriradiussource()) {
      switch (protoPoint.aprioriradiussource()) {
        case ControlPointFileEntryV0002_AprioriSource_None:
          aprioriRadiusSource = ControlPoint::RadiusSource::None;
          break;
        case ControlPointFileEntryV0002_AprioriSource_User:
          aprioriRadiusSource = ControlPoint::RadiusSource::User;
          break;
        case ControlPointFileEntryV0002_AprioriSource_AverageOfMeasures:
          aprioriRadiusSource = ControlPoint::RadiusSource::AverageOfMeasures;
          break;
        case ControlPointFileEntryV0002_AprioriSource_Ellipsoid:
          aprioriRadiusSource = ControlPoint::RadiusSource::Ellipsoid;
          break;
        case ControlPointFileEntryV0002_AprioriSource_DEM:
          aprioriRadiusSource = ControlPoint::RadiusSource::DEM;
          break;
        case ControlPointFileEntryV0002_AprioriSource_BundleSolution:
          aprioriRadiusSource = ControlPoint::RadiusSource::BundleSolution;
          break;

        default:
          QString msg = "Unknown control point apriori radius source.";
          throw IException(IException::User, msg, _FILEINFO_);
          break;
      }
      controlPoint->SetAprioriRadiusSource(aprioriRadiusSource);
    }

    if (protoPoint.has_aprioriradiussourcefile()) {
      controlPoint->SetAprioriRadiusSourceFile(protoPoint.aprioriradiussourcefile().c_str());
    }

    // setting apriori surf pt information
    if (protoPoint.has_apriorisurfpointsource()) {
      switch (protoPoint.apriorisurfpointsource()) {
        case ControlPointFileEntryV0002_AprioriSource_None:
          aprioriSurfacePointSource = ControlPoint::SurfacePointSource::None;
         break;

        case ControlPointFileEntryV0002_AprioriSource_User:
          aprioriSurfacePointSource = ControlPoint::SurfacePointSource::User;
          break;

        case ControlPointFileEntryV0002_AprioriSource_AverageOfMeasures:
          aprioriSurfacePointSource = ControlPoint::SurfacePointSource::AverageOfMeasures;
          break;

        case ControlPointFileEntryV0002_AprioriSource_Reference:
          aprioriSurfacePointSource = ControlPoint::SurfacePointSource::Reference;
          break;

        case ControlPointFileEntryV0002_AprioriSource_Basemap:
          aprioriSurfacePointSource = ControlPoint::SurfacePointSource::Basemap;
          break;

        case ControlPointFileEntryV0002_AprioriSource_BundleSolution:
          aprioriSurfacePointSource = ControlPoint::SurfacePointSource::BundleSolution;
          break;

        default:
          QString msg = "Unknown control point aprioir surface point source.";
          throw IException(IException::User, msg, _FILEINFO_);
          break;
      }

      controlPoint->SetAprioriSurfacePointSource(aprioriSurfacePointSource);
    }

    if (protoPoint.has_apriorisurfpointsourcefile()) {
      controlPoint->SetAprioriSurfacePointSourceFile(protoPoint.apriorisurfpointsourcefile().c_str());
    }

    if (protoPoint.has_apriorix()
        && protoPoint.has_aprioriy()
        && protoPoint.has_aprioriz()) {

      SurfacePoint aprioriSurfacePoint(Displacement(protoPoint.apriorix(), Displacement::Meters),
                                       Displacement(protoPoint.aprioriy(), Displacement::Meters),
                                       Displacement(protoPoint.aprioriz(), Displacement::Meters));
      if (protoPoint.aprioricovar_size() > 0) {
        symmetric_matrix<double, upper> aprioriCovarianceMatrix;
        aprioriCovarianceMatrix.resize(3);
        aprioriCovarianceMatrix.clear();
        aprioriCovarianceMatrix(0, 0) = protoPoint.aprioricovar(0);
        aprioriCovarianceMatrix(0, 1) = protoPoint.aprioricovar(1);
        aprioriCovarianceMatrix(0, 2) = protoPoint.aprioricovar(2);
        aprioriCovarianceMatrix(1, 1) = protoPoint.aprioricovar(3);
        aprioriCovarianceMatrix(1, 2) = protoPoint.aprioricovar(4);
        aprioriCovarianceMatrix(2, 2) = protoPoint.aprioricovar(5);
        aprioriSurfacePoint.SetRectangularMatrix(aprioriCovarianceMatrix);

        // note: setting lat/lon/rad constrained happens when we call SetAprioriSurfacePoint()
        // this method will look at the covar matrix for valid values and set accordingly.

#if 0
        if (Displacement(aprioriCovarianceMatrix(0, 0), Displacement::Meters).isValid()
            || Displacement(aprioriCovarianceMatrix(1, 1), Displacement::Meters).isValid()) {

          if (protoPoint.latitudeconstrained()) {
            constraintStatus.set(LatitudeConstrained);
          }
          if (protoPoint.longitudeconstrained()) {
            constraintStatus.set(LongitudeConstrained);
          }
          if (protoPoint.radiusconstrained()) {
            constraintStatus.set(RadiusConstrained);
          }

        }
        else if (Displacement(aprioriCovarianceMatrix(2, 2), Displacement::Meters).isValid()) {

          if (protoPoint.latitudeconstrained()) {
            constraintStatus.set(LatitudeConstrained);
          }
          if (protoPoint.radiusconstrained()) {
            constraintStatus.set(RadiusConstrained);
          }

        }
#endif
      }

      if (equatorialRadius.isValid() && polarRadius.isValid()) {
        aprioriSurfacePoint.SetRadii(equatorialRadius, equatorialRadius, polarRadius);
      }

      controlPoint->SetAprioriSurfacePoint(aprioriSurfacePoint);
    }

    // setting adj surf pt information
    if (protoPoint.has_adjustedx()
        && protoPoint.has_adjustedy()
        && protoPoint.has_adjustedz()) {

      SurfacePoint adjustedSurfacePoint(Displacement(protoPoint.adjustedx(), Displacement::Meters),
                                        Displacement(protoPoint.adjustedy(), Displacement::Meters),
                                        Displacement(protoPoint.adjustedz(), Displacement::Meters));

      if (protoPoint.adjustedcovar_size() > 0) {
        symmetric_matrix<double, upper> adjustedCovarianceMatrix;
        adjustedCovarianceMatrix.resize(3);
        adjustedCovarianceMatrix.clear();
        adjustedCovarianceMatrix(0, 0) = protoPoint.adjustedcovar(0);
        adjustedCovarianceMatrix(0, 1) = protoPoint.adjustedcovar(1);
        adjustedCovarianceMatrix(0, 2) = protoPoint.adjustedcovar(2);
        adjustedCovarianceMatrix(1, 1) = protoPoint.adjustedcovar(3);
        adjustedCovarianceMatrix(1, 2) = protoPoint.adjustedcovar(4);
        adjustedCovarianceMatrix(2, 2) = protoPoint.adjustedcovar(5);
        adjustedSurfacePoint.SetRectangularMatrix(adjustedCovarianceMatrix);
      }

      if (equatorialRadius.isValid() && polarRadius.isValid()) {
        adjustedSurfacePoint.SetRadii(equatorialRadius, equatorialRadius, polarRadius);
      }

      controlPoint->SetAdjustedSurfacePoint(adjustedSurfacePoint);
    }

    // adding measure information
    for (int m = 0 ; m < protoPoint.measures_size(); m++) {
      QSharedPointer<ControlMeasure> measure = createMeasure(protoPoint.measures(m));
      controlPoint->AddMeasure(measure);
    }

    if (protoPoint.has_referenceindex()) {
      controlPoint->SetRefMeasure(protoPoint.referenceindex());
    }

    // Set DateTime after calling all setters that clear DateTime value
    controlPoint->SetDateTime(protoPoint.dateTime().c_str());
    // Set edit lock last
    controlPoint.SetEditLock(protoPoint.editLock());
    return controlPoint;

  }


  /**
   * Create a pointer to a ControlMeasure from a V0006 file.
   *
   * @param measure The versioned control measure to be created.
   *
   * @return The ControlMeasure constructed from the V0006 version
   *         file.
   */
  QSharedPointer<ControlMeasure> ControlNetVersioner::createMeasure(const ControlPointFileEntryV0002_Measure &measure) {
    QSharedPointer<ControlMeasure> newMeasure = new QSharedPointer<ControlMeasure>();
    newMeasure.SetCubeSerialNumber(QString(measure.serialnumber().c_str()));
    newMeasure.SetChooserName(QString(measure.choosername().c_str()));
    newMeasure.SetDateTime(QString(measure.datetime().c_str()));

    ControlMeasure::MeasureType measureType;
    switch (measure.type()) {
      case ControlPointFileEntryV0002_Measure::Candidate:
        measureType = ControlMeasure::Candidate;
        break;
      case ControlPointFileEntryV0002_Measure::Manual:
        measureType = ControlMeasure::Manual;
        break;
      case ControlPointFileEntryV0002_Measure::RegisteredPixel:
        measureType = ControlMeasure::RegisteredPixel;
        break;
      case ControlPointFileEntryV0002_Measure::RegisteredSubPixel:
        measureType = ControlMeasure::RegisteredSubPixel;
        break;
      default:
        QString msg = "Unknown control measure type.";
        throw IException(IException::User, msg, _FILEINFO_);
        break;
    }
    newMeasure.SetType(measureType);

    newMeasure.SetEditLock(measure.editlock());
    newMeasure.SetRejected(measure.jigsawrejected());
    newMeasure.SetIgnored(measure.ignore());
    newMeasure.SetCoordinate(measure.sample(), measure.line());

    if (measure.has_diameter()) {
      newMeasure.SetDiameter(measure.diameter());
    }

    if (measure.has_apriorisample()) {
      newMeasure.SetAprioriSample(measure.apriorisample());
    }

    if (measure.has_aprioriline()) {
      newMeasure.SetAprioriLine(measure.aprioriline());
    }

    if (measure.has_samplesigma()) {
      newMeasure.SetSampleSigma(measure.samplesigma());
    }

    if (measure.has_linesigma()) {
      newMeasure.SetLineSigma(measure.linesigma());
    }
    if (measure.has_sampleresidual()
        && measure.has_lineresidual()) {
      newMeasure.SetResidual(measure.sampleresidual(), measure.lineresidual());
    }

    for (int i = 0; i < measure.log_size(); i++) {
      ControlMeasureLogData logEntry(measure.log(i));
      newMeasure.SetLogData(logEntry);
    }
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


  /**
   * This will write a control net file object to disk.
   *
   * @param netFile The output filename that will be written to
   *
   */
  void ControlNetVersioner::write(FileName netFile) {
    try {

      const int labelBytes = 65536;
      fstream output(netFile.expanded().toLatin1().data(), ios::out | ios::trunc | ios::binary);
      char *blankLabel = new char[labelBytes];
      memset(blankLabel, 0, labelBytes);
      output.write(blankLabel, labelBytes);
      delete [] blankLabel;

      streampos startCoreHeaderPos = output.tellp();

      OStreamOutputStream* fileStream(output);

      writeHeader(fileStream);

      while ( !m_points.isEmpty() ) {
        writeFirstPoint(fileStream);
      }

      // Insert header at the beginning of the file once writing is done.

      ControlNetFileHeaderV0005 protobufHeader;

      protobufHeader.set_networkid(m_header.networkID);
      protobufHeader.set_targetname(m_header.targetName);
      protobufHeader.set_created(m_header.created);
      protobufHeader.set_lastmodified(m_header.lastModified);
      protobufHeader.set_description(m_header.description);
      protobufHeader.set_username(m_header.userName);

      streampos coreHeaderSize = protobufHeader->ByteSize();

      Pvl p;

      PvlObject protoObj("ProtoBuffer");

      PvlObject protoCore("Core");
      protoCore.addKeyword(PvlKeyword("HeaderStartByte",
                           toString((BigInt) startCoreHeaderPos)));
      protoCore.addKeyword(PvlKeyword("HeaderBytes", toString((BigInt) coreHeaderSize)));

      BigInt pointsStartByte = (BigInt) (startCoreHeaderPos + coreHeaderSize);

      protoCore.addKeyword(PvlKeyword("PointsStartByte", toString(pointsStartByte);

      // Output.gcount() gives us bytes read so far, subtract the bytes that aren't related to points
      // To get the pointsSize.
      BigInt pointsSize = output.gcount() - pointsStartByte;

      protoCore.addKeyword(PvlKeyword("PointsBytes",
                           toString(pointsSize)));
      protoObj.addObject(protoCore);

      PvlGroup netInfo("ControlNetworkInfo");
      netInfo.addComment("This group is for informational purposes only");
      netInfo += PvlKeyword("NetworkId", protobufHeader.get_networkid().c_str());
      netInfo += PvlKeyword("TargetName", protobufHeader.get_targetname().c_str());
      netInfo += PvlKeyword("UserName", protobufHeader.get_username().c_str());
      netInfo += PvlKeyword("Created", protobufHeader.get_created().c_str());
      netInfo += PvlKeyword("LastModified", protobufHeader.get_lastmodified().c_str());
      netInfo += PvlKeyword("Description", protobufHeader.get_description().c_str());
      netInfo += PvlKeyword("NumberOfPoints", toString(m_points.size()));

      // Is there a better way we can get the total number of measures?
      int numMeasures = 0;
      foreach (QSharedPointer<ControlPoint> point, m_points) {
        numMeasures += point->GetNumMeasures();
      }
      netInfo += PvlKeyword("NumberOfMeasures", toString(numMeasures));
      netInfo += PvlKeyword("Version", "5");
      protoObj.addGroup(netInfo);

      p.addObject(protoObj);

      output.seekp(0, ios::beg);
      output << p;
      output << '\n';
      output.close();
    }
    catch () {
      string msg = "Can't write control net file"
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  }

 /**
  * This will read the binary protobuffer control network header to a ZeroCopyOutputStream
  *
  * @param fileStream
  */
  void ControlNetVersioner::writeHeader(ZeroCopyOutputStream *oStream) {

    CodedOutputStream fileStream(oStream);

    // Create the protobuf header using our struct
    ControlNetFileHeaderV0005 protobufHeader;
    protobufHeader.set_networkid(m_header.networkID);
    protobufHeader.set_targetname(m_header.targetName);
    protobufHeader.set_created(m_header.created);
    protobufHeader.set_lastmodified(m_header.lastModified);
    protobufHeader.set_description(m_header.description);
    protobufHeader.set_username(m_header.userName);

    // Write out the header
    if (!protobufHeader->SerializeToCodedStream(&fileStream)) {
      IString msg = "Failed to write output control network file [" +
          file.name() + "]";
      throw IException(IException::Io, msg, _FILEINFO_);
    }

  }


 /**
  * This will write the first control control point to a ZeroCopyOutputStream
  *
  * @param fileStream A pointer to the fileStream that we are writing the point to.
  */
  void ControlNetVersioner::writeFirstPoint(ZeroCopyOutputStream *oStream) {

      CodedOutputStream fileStream(oStream);

      ControlPointFileEntryV0002 protoPoint;
      QSharedPointer<ControlPoint> controlPoint = m_points.takeFirst();

      protoPoint.set_type(controlPoint->getType());

      protoPoint.set_id(controlPoint->GetId());
      protoPoint.set_choosername(controlPoint->GetChooserName());
      protoPoint.set_datetime(controlPoint->GetDateTime());
      protoPoint.set_editlock(controlPoint->IsEditLocked());

      protoPoint.set_ignore(controlPoint->IsIgnored());

      protoPoint.set_apriorisurfpointsource(controlPoint->GetAprioriSurfPointSource());

      if (controlPoint->HasAprioriSurfacePointSourceFile()) {
        protoPoint.set_apriorisurfpointsourcefile(controlPoint->GetAprioriSurfacePointSourceFile());
      }

      // Apriori Surf Point Source ENUM settting
      switch (controlPoint->GetAprioriSurfPointSource()) {
        case ControlPoint::SurfacePointSouce::None:
          protoPoint.set_apriorisurfpointsource(ControlPointFileEntryV0002_AprioriSource_None);
          break;
        case ControlPoint::SurfacePointSource::User:
          protoPoint.set_apriorisurfpointsource(ControlPointFileEntryV0002_AprioriSource_User);
          break;
        case ControlPoint::SurfacePointSource::AverageOfMeasures:
          protoPoint.set_apriorisurfpointsource(ControlPointFileEntryV0002_AprioriSource_AverageOfMeasures);
          break;
        case ControlPoint::SurfacePointSource::Reference:
          protoPoint.set_apriorisurfpointsource(ControlPointFileEntryV0002_AprioriSource_Reference);
          break;
        case ControlPoint::SurfacePointSource::Basemap:
          protoPoint.set_apriorisurfpointsource(ControlPointFileEntryV0002_AprioriSource_Basemap);
          break;
        case ControlPoint::SurfacePointSource::BundleSolution:
          protoPoint.set_apriorisurfpointsource(ControlPointFileEntryV0002_AprioriSource_BundleSolution);
          break;
      }

      // Apriori Radius Point Source ENUM setting
      switch (controlPoint->GetAprioriRadiusSource()) {
        case ControlPoint::RadiusSource::None:
          protoPoint.set_aprioriradiussource(ControlPointFileEntryV0002_AprioriSource_None);
          break;
        case ControlPoint::RadiusSource::User:
          protoPoint.set_aprioriradiussource(ControlPointFileEntryV0002_AprioriSource_User);
          break;
        case ControlPoint::RadiusSource::AverageOfMeasures:
          protoPoint.set_aprioriradiussource(ControlPointFileEntryV0002_AprioriSource_AverageOfMeasures);
          break;
        case ControlPoint::RadiusSource::BundleSolution:
          protoPoint.set_aprioriradiussource(ControlPointFileEntryV0002_AprioriSource_BundleSolution);
          break;
        case ControlPoint::RadiusSource::Ellipsoid:
          protoPoint.set_aprioriradiussource(ControlPointFileEntryV0002_AprioriSource_Ellipsoid);
          break;
        case ControlPoint::RadiusSource::DEM:
          protoPoint.set_aprioriradiussource(ControlPointFileEntryV0002_AprioriSource_DEM);
          break;
      }

      protoPoint.set_aprioriradiussource(controlPoint->GetAprioriRadiusSource());

      if (controlPoint->HasAprioriRadiusSourceFile()) {
        protoPoint.set_aprioriradiussourcefile(protobufPoint.GetAprioriRadiusSourceFile());
      }

      if (controlPoint->HasAprioriCoordinates()) {

        protoPoint.set_apriorix(controlPoint->AprioriX());
        protoPoint.set_aprioriy(controlPoint->AprioriY());
        protoPoint.set_aprioriz(controlPoint->AprioriZ());


        if (controlPoint->AprioriCovarSize()) { // DNE

          // Ensure this is the right way to add these values
          protoPoint.add_aprioricovar(controlPoint->aprioricovar(0)); // DNE
          protoPoint.add_aprioricovar(controlPoint->aprioricovar(1)); // DNE
          protoPoint.add_aprioricovar(controlPoint->aprioricovar(2)); // DNE
          protoPoint.add_aprioricovar(controlPoint->aprioricovar(3)); // DNE
          protoPoint.add_aprioricovar(controlPoint->aprioricovar(4)); // DNE
          protoPoint.add_aprioricovar(controlPoint->aprioricovar(5)); // DNE

          }
        }
      }

      protoPoint.set_latitudeconstrained(controlPoint->IsLatitudeConstrained());
      protoPoint.set_longitudeconstrained(controlPoint->IsLongitudeConstrained());
      protoPoint.set_radiusconstrained(controlPoint->IsRadiusConstrained());


      if (controlPoint->HasAdjustedCoordinates()) {

        protoPoint.set_adjustedx(controlPoint->AdjustedX());
        protoPoint.set_adjustedy(controlPoint->AdjustedY());
        protoPoint.set_adjustedz(controlPoint->AdjustedZ());

        if (controlPoint->AdjustedCovarSize()) { // DNE
          protoPoint.add_adjustedcovar(controlPoint->AdjustedCovar(0));
          protoPoint.add_adjustedcovar(controlPoint->AdjustedCovar(1));
          protoPoint.add_adjustedcovar(controlPoint->AdjustedCovar(2));
          protoPoint.add_adjustedcovar(controlPoint->AdjustedCovar(3));
          protoPoint.add_adjustedcovar(controlPoint->AdjustedCovar(4));
          protoPoint.add_adjustedcovar(controlPoint->AdjustedCovar(5));
          }
        }
      }

      // Converting Measures
      for (int j = 0; j < controlPoint->GetNumMeasures(); j++) {

        const ControlMeasure &
            controlMeasure = controlPoint->GetMeasure(j);

        ControlPointFileEntryV0002_Measure protoMeasure;

        if (controlPoint->HasRefMeasure() && controlPoint->IndexOfRefMeasure() == j) {
             protoPoint.set_referenceindex(j);
        }

        protoMeasure.set_serialnumber(controlMeasure.GetCubeSerialNumber());

        switch ( controlMeasure.GetType() ) {
            case (ControlMeasure::MeasureType::Canditate) {
                protoMeasure.set_measuretype(ControlPointFileEntryV0002_Measure_MeasureType_Candidate);
                break;
            }
            case (ControlMeasure::MeasureType::Manual) {
                protoMeasure.set_measuretype(ControlPointFileEntryV0002_Measure_MeasureType_Manual);
                break;
            }
            case (ControlMeasure::RegisteredPixel) {
                protoMeasure.set_measuretype(ControlPointFileEntryV0002_Measure_MeasureType_RegisteredPixel);
                break;
            }
            case (ControlMeasure::RegisteredSubPixel) {
                protoMeasure.set_measuretype(ControlPointFileEntryV0002_Measure_MeasureType_RegisteredSubPixel);
                break;
            }
        }

        if (controlMeasure.HasChooserName()) {
          protoMeasure.set_choosername(controlMeasure.GetChooserName());
        }

        if (controlMeasure.HasDateTime()) {
          protoMeasure.set_datetime(controlMeasure.GetDateTime());
        }

        protoMeasure.set_editlock(controlMeasure.IsEditLocked());

        protoMeasure.set_ignore(controlMeasure.IsIgnored());

        if (controlMeasure.HasSample()) {
          protoMeasure.set_sample(controlMeasure.GetSample());
        }

        if (controlMeasure.HasLine()) {
          protoMeasure.set_line(controlMeasure.GetLine()));
        }

        if (controlMeasure.HasDiameter()) {
          protoMeasure.set_diameter(controlMeasure.GetDiameter()));
        }

        if (controlMeasure.HasAprioriSample()) {
          protoMeasure.set_apriorisample(controlMeasure.GetAprioriSample()));
        }

        if (controlMeasure.HasAprioriLine()) {
          protoMeasure.set_aprioriline(controlMeasure.GetAprioriLine()));
        }

        if (controlMeasure.HasSampleSigma()) {
          protoMeasure.set_samplesigma(controlMeasure.GetSampleSigma());
        }

        if (controlMeasure.HasLineSigma()) {
          protoMeasure.set_linesigma(controlMeasure.GetLineSigma());
        }

        if (controlMeasure.HasSampleResidual()) {
          protoMeasure.set_sampleresidual(controlMeasure.GetSampleResidual());
        }

        if (controlMeasure.HasLineResidual()) {
          protoMeasure.set_lineresidual(controlMeasure.GetLineResidual());
        }

        // I removed the if statement because we always initialize jigsawRejected to false
        // in ControlPoint.
        protoMeasure.set_jigsawrejected(controlMeasure.IsJigsawRejected()));


        for (int logEntry = 0;
            logEntry < controlMeasure.LogSize(); // DNE?
            logEntry ++) {

          const ControlMeasureLogData &log =
                controlMeasure.GetLogData(logEntry); // Not sure this is right.

          // These methods might not not exist, we may need to wrap each of These
          // In if/else statements because they're optional values.
          ControlPointFileEntryV0002_Measure_MeasureLogData logData;

          logData.set_doubledatatype(log.GetDoubleDataType());
          logData.set_doubledatavalue(log.GetDoubleDataValue());
          logData.set_booldatatype(log.GetBoolDataType());
          logData.set_booldatavalue(log.getBoolDataValue());

          protoMeasure.add_log(logData);
        }

        if (controlPoint->HasRefMeasure() && controlPoint->IndexOfRefMeasure() == j) {
             protoPoint.set_referenceindex(j);
        }
        protoPoint.add_measure(protoMeasure);
      }

      int msgSize(protoPoint.ByteSize());
      fileStream->WriteVarint32(msgSize);

      if ( !protoPoint.SerializeToCodedStream(fileStream.data()) ) {
        QString err = "Error writing to coded protobuf stream";
        throw IException(IException::Programmer, err, _FILEINFO_);
      }

      // return size of message
      return (msgSize);
  }


// ??? TODO
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
   *   protocol buffer objects. Helper methods copy(...) do most of the work.
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

      if (newPoint.container.hasKeyword("Held") && newPoint.container["Held"][0] == "True")
        newPoint.container["PointType"] = "Ground";

      if (newPoint.container.hasKeyword("AprioriLatLonSource"))
        newPoint.container["AprioriLatLonSource"].setName("AprioriXYZSource");

      if (newPoint.container.hasKeyword("AprioriLatLonSourceFile"))
        newPoint.container["AprioriLatLonSourceFile"].setName("AprioriXYZSourceFile");

      if (newPoint.container.hasKeyword("AprioriLatitude")) {
        SurfacePoint apriori(
            Latitude(toDouble(newPoint.container["AprioriLatitude"][0]), Angle::Degrees),
            Longitude(toDouble(newPoint.container["AprioriLongitude"][0]), Angle::Degrees),
            Distance(toDouble(newPoint.container["AprioriRadius"][0]), Distance::Meters));

        newPoint.container += PvlKeyword("AprioriX", toString(apriori.GetX().meters()), "meters");
        newPoint.container += PvlKeyword("AprioriY", toString(apriori.GetY().meters()), "meters");
        newPoint.container += PvlKeyword("AprioriZ", toString(apriori.GetZ().meters()), "meters");
      }

      if (newPoint.container.hasKeyword("Latitude")) {
        SurfacePoint adjusted(
            Latitude(toDouble(newPoint.container["Latitude"][0]), Angle::Degrees),
            Longitude(toDouble(newPoint.container["Longitude"][0]), Angle::Degrees),
            Distance(toDouble(newPoint.container["Radius"][0]), Distance::Meters));

        newPoint.container += PvlKeyword("AdjustedX", toString(adjusted.GetX().meters()), "meters");
        newPoint.container += PvlKeyword("AdjustedY", toString(adjusted.GetY().meters()), "meters");
        newPoint.container += PvlKeyword("AdjustedZ", toString(adjusted.GetZ().meters()), "meters");

        if (!newPoint.container.hasKeyword("AprioriLatitude")) {
          newPoint.container += PvlKeyword("AprioriX", toString(adjusted.GetX().meters()), "meters");
          newPoint.container += PvlKeyword("AprioriY", toString(adjusted.GetY().meters()), "meters");
          newPoint.container += PvlKeyword("AprioriZ", toString(adjusted.GetZ().meters()), "meters");
        }
      }

      if (newPoint.container.hasKeyword("X"))
        newPoint.container["X"].setName("AdjustedX");

      if (newPoint.container.hasKeyword("Y"))
        newPoint.container["Y"].setName("AdjustedY");

      if (newPoint.container.hasKeyword("Z"))
        newPoint.container["Z"].setName("AdjustedZ");

      if (newPoint.container.hasKeyword("AprioriSigmaLatitude") ||
         newPoint.container.hasKeyword("AprioriSigmaLongitude") ||
         newPoint.container.hasKeyword("AprioriSigmaRadius")) {
        double sigmaLat = 10000.0;
        double sigmaLon = 10000.0;
        double sigmaRad = 10000.0;

        if (newPoint.container.hasKeyword("AprioriSigmaLatitude")) {
          if (toDouble(newPoint.container["AprioriSigmaLatitude"][0]) > 0 &&
              toDouble(newPoint.container["AprioriSigmaLatitude"][0]) < sigmaLat)
            sigmaLat = newPoint.container["AprioriSigmaLatitude"];

          newPoint.container += PvlKeyword("LatitudeConstrained", "True");
        }

        if (newPoint.container.hasKeyword("AprioriSigmaLongitude")) {
          if (toDouble(newPoint.container["AprioriSigmaLongitude"][0]) > 0 &&
              toDouble(newPoint.container["AprioriSigmaLongitude"][0]) < sigmaLon)
            sigmaLon = newPoint.container["AprioriSigmaLongitude"];

          newPoint.container += PvlKeyword("LongitudeConstrained", "True");
        }

        if (newPoint.container.hasKeyword("AprioriSigmaRadius")) {
          if (toDouble(newPoint.container["AprioriSigmaRadius"][0]) > 0 &&
              toDouble(newPoint.container["AprioriSigmaRadius"][0]) < sigmaRad)
            sigmaRad = newPoint.container["AprioriSigmaRadius"];

          newPoint.container += PvlKeyword("RadiusConstrained", "True");
        }

        SurfacePoint tmp;
        tmp.SetRadii(equatorialRadius, equatorialRadius, polarRadius);
        tmp.SetRectangular(
            Displacement(newPoint.container["AprioriX"], Displacement::Meters),
            Displacement(newPoint.container["AprioriY"], Displacement::Meters),
            Displacement(newPoint.container["AprioriZ"], Displacement::Meters));
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

        newPoint.container += aprioriCovarMatrix;
      }

      if (newPoint.container.hasKeyword("AdjustedSigmaLatitude") ||
          newPoint.container.hasKeyword("AdjustedSigmaLongitude") ||
          newPoint.container.hasKeyword("AdjustedSigmaRadius")) {
        double sigmaLat = 10000.0;
        double sigmaLon = 10000.0;
        double sigmaRad = 10000.0;

        if (newPoint.container.hasKeyword("AdjustedSigmaLatitude")) {
          if (toDouble(newPoint.container["AdjustedSigmaLatitude"][0]) > 0 &&
              toDouble(newPoint.container["AdjustedSigmaLatitude"][0]) < sigmaLat)
            sigmaLat = newPoint.container["AdjustedSigmaLatitude"];
        }

        if (newPoint.container.hasKeyword("AdjustedSigmaLongitude")) {
          if (toDouble(newPoint.container["AdjustedSigmaLongitude"][0]) > 0 &&
              toDouble(newPoint.container["AdjustedSigmaLongitude"][0]) < sigmaLon)
            sigmaLon = newPoint.container["AdjustedSigmaLongitude"];
        }

        if (newPoint.container.hasKeyword("AdjustedSigmaRadius")) {
          if (toDouble(newPoint.container["AdjustedSigmaRadius"][0]) > 0 &&
              toDouble(newPoint.container["AdjustedSigmaRadius"][0]) < sigmaRad)
            sigmaRad = newPoint.container["AdjustedSigmaRadius"];
        }

        SurfacePoint tmp;
        tmp.SetRadii(equatorialRadius, equatorialRadius, polarRadius);
        tmp.SetRectangular(Displacement(newPoint.container["AdjustedX"], Displacement::Meters),
                           Displacement(newPoint.container["AdjustedY"], Displacement::Meters),
                           Displacement(newPoint.container["AdjustedZ"], Displacement::Meters));
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

        newPoint.container += adjustedCovarMatrix;
      }

      if (newPoint.container.hasKeyword("ApostCovarianceMatrix"))
        newPoint.container["ApostCovarianceMatrix"].setName("AdjustedCovarianceMatrix");

      if (!newPoint.container.hasKeyword("LatitudeConstrained")) {
        if (newPoint.container.hasKeyword("AprioriCovarianceMatrix"))
          newPoint.container += PvlKeyword("LatitudeConstrained", "True");
        else
          newPoint.container += PvlKeyword("LatitudeConstrained", "False");
      }

      if (!newPoint.container.hasKeyword("LongitudeConstrained")) {
        if (newPoint.container.hasKeyword("AprioriCovarianceMatrix"))
          newPoint.container += PvlKeyword("LongitudeConstrained", "True");
        else
          newPoint.container += PvlKeyword("LongitudeConstrained", "False");
      }

      if (!newPoint.container.hasKeyword("RadiusConstrained")) {
        if (newPoint.container.hasKeyword("AprioriCovarianceMatrix"))
          newPoint.container += PvlKeyword("RadiusConstrained", "True");
        else
          newPoint.container += PvlKeyword("RadiusConstrained", "False");
      }

      // Delete anything that has no value...
      for (int cpKeyIndex = 0; cpKeyIndex < newPoint.container.keywords(); cpKeyIndex ++) {
        if (newPoint.container[cpKeyIndex][0] == "") {
          newPoint.container.deleteKeyword(cpKeyIndex);
        }
      }

      for (int cmIndex = 0; cmIndex < newPoint.container.groups(); cmIndex ++) {
        PvlGroup &cm = newPoint.container.group(cmIndex);

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

     if (newPoint.container.hasKeyword("AprioriCovarianceMatrix") ||
         newPoint.container.hasKeyword("AdjustedCovarianceMatrix"))
       newPoint.container["PointType"] = "Constrained";
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

     if (newPoint.container["PointType"][0] == "Ground") newPoint.container["PointType"] = "Fixed";
     if (newPoint.container["PointType"][0] == "Tie") newPoint.container["PointType"] = "Free";
    }
  }

#endif

  /**
   * This is a convenience method for copying keywords out of the container
   *   and into the ControlPointV0006 for booleans. This operation is
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
  void ControlNetVersioner::copy(PvlContainer &container,
                                 QString keyName,
                                 ControlPointV0006 &point,
                                 void (ControlPointV0006::*setter)(bool)) {

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
   *   and into the ControlPointV0006 for doubles. This operation is
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
  void ControlNetVersioner::copy(PvlContainer &container,
                                 QString keyName,
                                 ControlPointV0006 &point,
                                 void (ControlPointV0006::*setter)(double)) {

    if (!container.hasKeyword(keyName))
      return;

    double value = toDouble(container[keyName][0]);
    container.deleteKeyword(keyName);
    (point.*setter)(value);
  }


  /**
   * This is a convenience method for copying keywords out of the container
   *   and into the ControlPointV0006 for strings. This operation is
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
  void ControlNetVersioner::copy(PvlContainer &container,
                                 QString keyName,
                                 ControlPointV0006 &point,
                                 void (ControlPointV0006::*setter)(const std::string&)) {

    if (!container.hasKeyword(keyName))
      return;

    IString value = container[keyName][0];
    container.deleteKeyword(keyName);
    (point.*setter)(value);
  }


  /**
   * This is a convenience method for copying keywords out of the container
   *   and into the ControlMeasureV0006 for booleans. This
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
  void ControlNetVersioner::copy(PvlContainer &container,
                                 QString keyName,
                                 ControlMeasureV0006 &measure,
                                 void (ControlMeasureV0006::*setter)(bool)) {

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
   *   and into the ControlMeasureV0006 for doubles. This
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
  void ControlNetVersioner::copy(PvlContainer &container,
                                 QString keyName,
                                 ControlMeasureV0006 &measure,
                                 void (ControlMeasureV0006::*setter)(double)) {

    if (!container.hasKeyword(keyName))
      return;

    double value = toDouble(container[keyName][0]);
    container.deleteKeyword(keyName);
    (measure.*setter)(value);
  }


  /**
   * This is a convenience method for copying keywords out of the container
   *   and into the ControlMeasureV0006 for strings. This
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
  void ControlNetVersioner::copy(PvlContainer &container,
                                 QString keyName,
                                 ControlMeasureV0006 &measure,
                                 void (ControlMeasureV0006::*setter)
                                      (const std::string &)) {

    if (!container.hasKeyword(keyName))
      return;

    IString value = container[keyName][0];
    container.deleteKeyword(keyName);
    (measure.*set)(value);
  }
}
