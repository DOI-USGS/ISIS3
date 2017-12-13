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


  void ControlNetVersioner::read(const FileName netFile) {

  }


  void ControlNetVersioner::readPvl(const Pvl &network) {

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

    ControlPointV0002 newPoint;
    newPoint.container = point.container;

    if (newPoint.container.hasKeyword("Held")
        && newPoint.container["Held"][0] == "True") {
      newPoint.container["PointType"] = "Ground";
    }

    if (newPoint.container.hasKeyword("AprioriLatLonSource")) {
      newPoint.container["AprioriLatLonSource"].setName("AprioriXYZSource");
    }

    if (newPoint.container.hasKeyword("AprioriLatLonSourceFile")) {
      newPoint.container["AprioriLatLonSourceFile"].setName("AprioriXYZSourceFile");
    }

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

    if (newPoint.container.hasKeyword("X")) {
      newPoint.container["X"].setName("AdjustedX");
    }

    if (newPoint.container.hasKeyword("Y")) {
      newPoint.container["Y"].setName("AdjustedY");
    }

    if (newPoint.container.hasKeyword("Z")) {
      newPoint.container["Z"].setName("AdjustedZ");
    }

    if (newPoint.container.hasKeyword("AprioriSigmaLatitude")
        || newPoint.container.hasKeyword("AprioriSigmaLongitude")
        || newPoint.container.hasKeyword("AprioriSigmaRadius")) {
      double sigmaLat = 10000.0;
      double sigmaLon = 10000.0;
      double sigmaRad = 10000.0;

      if (newPoint.container.hasKeyword("AprioriSigmaLatitude")) {
        if (toDouble(newPoint.container["AprioriSigmaLatitude"][0]) > 0
            && toDouble(newPoint.container["AprioriSigmaLatitude"][0]) < sigmaLat) {
          sigmaLat = newPoint.container["AprioriSigmaLatitude"];
        }

        newPoint.container += PvlKeyword("LatitudeConstrained", "True");
      }

      if (newPoint.container.hasKeyword("AprioriSigmaLongitude")) {
        if (toDouble(newPoint.container["AprioriSigmaLongitude"][0]) > 0
            && toDouble(newPoint.container["AprioriSigmaLongitude"][0]) < sigmaLon) {
          sigmaLon = newPoint.container["AprioriSigmaLongitude"];
        }

        newPoint.container += PvlKeyword("LongitudeConstrained", "True");
      }

      if (newPoint.container.hasKeyword("AprioriSigmaRadius")) {
        if (toDouble(newPoint.container["AprioriSigmaRadius"][0]) > 0
            && toDouble(newPoint.container["AprioriSigmaRadius"][0]) < sigmaRad) {
          sigmaRad = newPoint.container["AprioriSigmaRadius"];
        }

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

    if (newPoint.container.hasKeyword("AdjustedSigmaLatitude")
        || newPoint.container.hasKeyword("AdjustedSigmaLongitude")
        || newPoint.container.hasKeyword("AdjustedSigmaRadius")) {
      double sigmaLat = 10000.0;
      double sigmaLon = 10000.0;
      double sigmaRad = 10000.0;

      if (newPoint.container.hasKeyword("AdjustedSigmaLatitude")) {
        if (toDouble(newPoint.container["AdjustedSigmaLatitude"][0]) > 0
            && toDouble(newPoint.container["AdjustedSigmaLatitude"][0]) < sigmaLat) {
          sigmaLat = newPoint.container["AdjustedSigmaLatitude"];
        }
      }

      if (newPoint.container.hasKeyword("AdjustedSigmaLongitude")) {
        if (toDouble(newPoint.container["AdjustedSigmaLongitude"][0]) > 0
            && toDouble(newPoint.container["AdjustedSigmaLongitude"][0]) < sigmaLon) {
          sigmaLon = newPoint.container["AdjustedSigmaLongitude"];
        }
      }

      if (newPoint.container.hasKeyword("AdjustedSigmaRadius")) {
        if (toDouble(newPoint.container["AdjustedSigmaRadius"][0]) > 0
            && toDouble(newPoint.container["AdjustedSigmaRadius"][0]) < sigmaRad) {
          sigmaRad = newPoint.container["AdjustedSigmaRadius"];
        }
      }

      SurfacePoint adjustedSurfacePoint;
      adjustedSurfacePoint.SetRadii(equatorialRadius, equatorialRadius, polarRadius);

      adjustedSurfacePoint.SetRectangular(Displacement(newPoint.container["AdjustedX"], 
                                                       Displacement::Meters),
                                          Displacement(newPoint.container["AdjustedY"], 
                                                       Displacement::Meters),
                                          Displacement(newPoint.container["AdjustedZ"], 
                                                       Displacement::Meters));

      adjustedSurfacePoint.SetSphericalSigmasDistance(Distance(sigmaLat, Distance::Meters),
                                                      Distance(sigmaLon, Distance::Meters),
                                                      Distance(sigmaRad, Distance::Meters));

      PvlKeyword adjustedCovarMatrix("AdjustedCovarianceMatrix");
      adjustedCovarMatrix += toString(adjustedSurfacePoint.GetRectangularMatrix()(0, 0));
      adjustedCovarMatrix += toString(adjustedSurfacePoint.GetRectangularMatrix()(0, 1));
      adjustedCovarMatrix += toString(adjustedSurfacePoint.GetRectangularMatrix()(0, 2));
      adjustedCovarMatrix += toString(adjustedSurfacePoint.GetRectangularMatrix()(1, 1));
      adjustedCovarMatrix += toString(adjustedSurfacePoint.GetRectangularMatrix()(1, 2));
      adjustedCovarMatrix += toString(adjustedSurfacePoint.GetRectangularMatrix()(2, 2));

      newPoint.container += adjustedCovarMatrix;
    }

    if (newPoint.container.hasKeyword("ApostCovarianceMatrix")) {
      newPoint.container["ApostCovarianceMatrix"].setName("AdjustedCovarianceMatrix");
    }

    if (!newPoint.container.hasKeyword("LatitudeConstrained")) {
      if (newPoint.container.hasKeyword("AprioriCovarianceMatrix")) {
        newPoint.container += PvlKeyword("LatitudeConstrained", "True");
      }
      else {
        newPoint.container += PvlKeyword("LatitudeConstrained", "False");
      }
    }

    if (!newPoint.container.hasKeyword("LongitudeConstrained")) {
      if (newPoint.container.hasKeyword("AprioriCovarianceMatrix")) {
        newPoint.container += PvlKeyword("LongitudeConstrained", "True");
      }
      else {
        newPoint.container += PvlKeyword("LongitudeConstrained", "False");
      }
    }

    if (!newPoint.container.hasKeyword("RadiusConstrained")) {
      if (newPoint.container.hasKeyword("AprioriCovarianceMatrix")) {
        newPoint.container += PvlKeyword("RadiusConstrained", "True");
      }
      else {
        newPoint.container += PvlKeyword("RadiusConstrained", "False");
      }
    }

    // Delete anything that has no value...
    for (int cpKeyIndex = 0; cpKeyIndex < newPoint.container.keywords(); cpKeyIndex ++) {
      if (newPoint.container[cpKeyIndex][0] == "") {
        newPoint.container.deleteKeyword(cpKeyIndex);
      }
    }

    for (int measureIndex = 0; measureIndex < newPoint.container.groups(); measureIndex ++) {
      PvlGroup &measure = newPoint.container.group(measureIndex);

      // Estimated => Candidate
      if (measure.hasKeyword("MeasureType")) {
        QString type = measure["MeasureType"][0].toLower();

        if (type == "estimated"
            || type == "unmeasured") {
          if (type == "unmeasured") {
            bool hasSampleLine = false;

            try {
              toDouble(measure["Sample"][0]);
              toDouble(measure["Line"][0]);
              hasSampleLine = true;
            }
            catch (...) {
            }

            if (!hasSampleLine) {
              measure.addKeyword(PvlKeyword("Sample", "0.0"), PvlContainer::Replace);
              measure.addKeyword(PvlKeyword("Line", "0.0"), PvlContainer::Replace);
              measure.addKeyword(PvlKeyword("Ignore", toString(true)), PvlContainer::Replace);
            }
          }

          measure["MeasureType"] = "Candidate";
        }
        else if (type == "automatic"
                 || type == "validatedmanual"
                 || type == "automaticpixel") {
          measure["MeasureType"] = "RegisteredPixel";
        }
        else if (type == "validatedautomatic" 
                 || type == "automaticsubpixel") {
          measure["MeasureType"] = "RegisteredSubPixel";
        }
      }

      if (measure.hasKeyword("ErrorSample")) {
        measure["ErrorSample"].setName("SampleResidual");
      }

      if (measure.hasKeyword("ErrorLine")) {
        measure["ErrorLine"].setName("LineResidual");
      }

      // Delete some extraneous values we once printed
      if (measure.hasKeyword("SampleResidual")
          && toDouble(measure["SampleResidual"][0]) == 0.0) {
        measure.deleteKeyword("SampleResidual");
      }

      if (measure.hasKeyword("LineResidual")
          && toDouble(measure["LineResidual"][0]) == 0.0) {
        measure.deleteKeyword("LineResidual");
      }

      if (measure.hasKeyword("Diameter")
          && toDouble(measure["Diameter"][0]) == 0.0) {
        measure.deleteKeyword("Diameter");
      }

      if (measure.hasKeyword("ErrorMagnitude")) {
        measure.deleteKeyword("ErrorMagnitude");
      }

      if (measure.hasKeyword("ZScore")) {
        measure.deleteKeyword("ZScore");
      }

      // Delete anything that has no value...
      for (int measureKeyIndex = 0; measureKeyIndex < measure.keywords(); measureKeyIndex ++) {
        if (measure[measureKeyIndex][0] == "") {
          measure.deleteKeyword(measureKeyIndex);
        }
      }
    } // end measure loop


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

    ControlPointV0003 newPoint;
    newPoint.container = point.container;

    if (newPoint.container.hasKeyword("AprioriCovarianceMatrix")
        || newPoint.container.hasKeyword("AdjustedCovarianceMatrix")) {

      newPoint.container["PointType"] = "Constrained";

    }

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

    ControlPointV0004 newPoint;
    newPoint.container = point.container;
    if (newPoint.container["PointType"][0] == "Ground") {
      newPoint.container["PointType"] = "Fixed";
    }
    if (newPoint.container["PointType"][0] == "Tie") {
      newPoint.container["PointType"] = "Free";
    }

    return createPoint(newPoint);

  }


  /**
   * Create a pointer to a latest version ControlPoint from an 
   * object in a V0004 control net file. This method converts a 
   * ControlPointV0004 to the latest ControlPontV#### version 
   * and uses the latest versioned point to construct and fill an 
   * Isis::ControlPoint. 
   *
   * @param point The versioned control point to be updated.
   *  
   * @return The latest version ControlPoint constructed from the 
   *         given point.
   */
  QSharedPointer<ControlPoint> ControlNetVersioner::createPoint(const ControlPointV0004 point) {
    ControlPointV0006 newPoint;

    copy(point.container, "PointId",
         newPoint, &ControlPointV0006::set_id);
    copy(point.container, "ChooserName",
         newPoint, &ControlPointV0006::set_choosername);
    copy(point.container, "DateTime",
         newPoint, &ControlPointV0006::set_datetime);
    copy(point.container, "AprioriXYZSourceFile",
         newPoint, &ControlPointV0006::set_apriorisurfpointsourcefile);
    copy(point.container, "AprioriRadiusSourceFile",
         newPoint, &ControlPointV0006::set_aprioriradiussourcefile);
    copy(point.container, "JigsawRejected",
         newPoint, &ControlPointV0006::set_jigsawrejected);
    copy(point.container, "EditLock",
         newPoint, &ControlPointV0006::set_editlock);
    copy(point.container, "Ignore",
         newPoint, &ControlPointV0006::set_ignore);
    copy(point.container, "AprioriX",
         newPoint, &ControlPointV0006::set_apriorix);
    copy(point.container, "AprioriY",
         newPoint, &ControlPointV0006::set_aprioriy);
    copy(point.container, "AprioriZ",
         newPoint, &ControlPointV0006::set_aprioriz);
    copy(point.container, "AdjustedX",
         newPoint, &ControlPointV0006::set_adjustedx);
    copy(point.container, "AdjustedY",
         newPoint, &ControlPointV0006::set_adjustedy);
    copy(point.container, "AdjustedZ",
         newPoint, &ControlPointV0006::set_adjustedz);
    copy(point.container, "LatitudeConstrained",
         newPoint, &ControlPointV0006::set_latitudeconstrained);
    copy(point.container, "LongitudeConstrained",
         newPoint, &ControlPointV0006::set_longitudeconstrained);
    copy(point.container, "RadiusConstrained",
         newPoint, &ControlPointV0006::set_radiusconstrained);

    if (point.container["PointType"][0] == "Fixed")
      newPoint.set_type(ControlPointV0006::Fixed);
    else if (point.container["PointType"][0] == "Constrained")
      newPoint.set_type(ControlPointV0006::Constrained);
    else
      newPoint.set_type(ControlPointV0006::Free);

    if (point.container.hasKeyword("AprioriXYZSource")) {
      IString source = point.container["AprioriXYZSource"][0];

      if (source == "None") {
        newPoint.set_apriorisurfpointsource(ControlPointV0006::None);
      }
      else if (source == "User") {
        newPoint.set_apriorisurfpointsource(ControlPointV0006::User);
      }
      else if (source == "AverageOfMeasures") {
        newPoint.set_apriorisurfpointsource(
            ControlPointV0006::AverageOfMeasures);
      }
      else if (source == "Reference") {
        newPoint.set_apriorisurfpointsource(
            ControlPointV0006::Reference);
      }
      else if (source == "Basemap") {
        newPoint.set_apriorisurfpointsource(
            ControlPointV0006::Basemap);
      }
      else if (source == "BundleSolution") {
        newPoint.set_apriorisurfpointsource(
            ControlPointV0006::BundleSolution);
      }
      else {
        IString msg = "Invalid AprioriXYZSource [" + source + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    if (point.container.hasKeyword("AprioriRadiusSource")) {
      IString source = point.container["AprioriRadiusSource"][0];

      if (source == "None") {
        newPoint.set_aprioriradiussource(ControlPointV0006::None);
      }
      else if (source == "User") {
        newPoint.set_aprioriradiussource(ControlPointV0006::User);
      }
      else if (source == "AverageOfMeasures") {
        newPoint.set_aprioriradiussource(ControlPointV0006::AverageOfMeasures);
      }
      else if (source == "Ellipsoid") {
        newPoint.set_aprioriradiussource(ControlPointV0006::Ellipsoid);
      }
      else if (source == "DEM") {
        newPoint.set_aprioriradiussource(ControlPointV0006::DEM);
      }
      else if (source == "BundleSolution") {
        newPoint.set_aprioriradiussource(ControlPointV0006::BundleSolution);
      }
      else {
        std::string msg = "Invalid AprioriRadiusSource, [" + source + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    if (point.container.hasKeyword("AprioriCovarianceMatrix")) {
      PvlKeyword &matrix = point.container["AprioriCovarianceMatrix"];

      newPoint.add_aprioricovar(toDouble(matrix[0]));
      newPoint.add_aprioricovar(toDouble(matrix[1]));
      newPoint.add_aprioricovar(toDouble(matrix[2]));
      newPoint.add_aprioricovar(toDouble(matrix[3]));
      newPoint.add_aprioricovar(toDouble(matrix[4]));
      newPoint.add_aprioricovar(toDouble(matrix[5]));
    }

    if (point.container.hasKeyword("AdjustedCovarianceMatrix")) {
      PvlKeyword &matrix = point.container["AdjustedCovarianceMatrix"];

      newPoint.add_adjustedcovar(toDouble(matrix[0]));
      newPoint.add_adjustedcovar(toDouble(matrix[1]));
      newPoint.add_adjustedcovar(toDouble(matrix[2]));
      newPoint.add_adjustedcovar(toDouble(matrix[3]));
      newPoint.add_adjustedcovar(toDouble(matrix[4]));
      newPoint.add_adjustedcovar(toDouble(matrix[5]));
    }

    //  Process Measures
    for (int groupIndex = 0; groupIndex < point.container.groups(); groupIndex ++) {
      PvlGroup &group = point.container.group(groupIndex);
      ControlMeasureV0006 measure;

      copy(group, "SerialNumber",
           measure, &ControlMeasureV0006::set_serialnumber);
      copy(group, "ChooserName",
           measure, &ControlMeasureV0006::set_choosername);
      copy(group, "Sample",
           measure, &ControlMeasureV0006::set_sample);
      copy(group, "Line",
           measure, &ControlMeasureV0006::set_line);
      copy(group, "SampleResidual",
           measure, &ControlMeasureV0006::set_sampleresidual);
      copy(group, "LineResidual",
           measure, &ControlMeasureV0006::set_lineresidual);
      copy(group, "DateTime",
           measure, &ControlMeasureV0006::set_datetime);
      copy(group, "Diameter",
           measure, &ControlMeasureV0006::set_diameter);
      copy(group, "EditLock",
           measure, &ControlMeasureV0006::set_editlock);
      copy(group, "Ignore",
           measure, &ControlMeasureV0006::set_ignore);
      copy(group, "JigsawRejected",
           measure, &ControlMeasureV0006::set_jigsawrejected);
      copy(group, "AprioriSample",
           measure, &ControlMeasureV0006::set_apriorisample);
      copy(group, "AprioriLine",
           measure, &ControlMeasureV0006::set_aprioriline);
      copy(group, "SampleSigma",
           measure, &ControlMeasureV0006::set_samplesigma);
      copy(group, "LineSigma",
           measure, &ControlMeasureV0006::set_linesigma);

      if (group.hasKeyword("Reference")) {
        if (group["Reference"][0].toLower() == "true")
          newPoint.set_referenceindex(groupIndex);

        group.deleteKeyword("Reference");
      }

      QString type = group["MeasureType"][0].toLower();
      if (type == "candidate")
        measure.set_type(ControlMeasureV0006::Candidate);
      else if (type == "manual")
        measure.set_type(ControlMeasureV0006::Manual);
      else if (type == "registeredpixel")
        measure.set_type(ControlMeasureV0006::RegisteredPixel);
      else if (type == "registeredsubpixel")
        measure.set_type(ControlMeasureV0006::RegisteredSubPixel);
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

      *newPoint.add_measures() = measure;
    }

    if (!newPoint.IsInitialized()) {
      IString msg = "There is missing required information in the control "
          "points or measures";
      throw IException(IException::Io, msg, _FILEINFO_);
    }




    return createPoint(newPoint);

  }


  /**
   * Create a pointer to a latest version ControlPoint from a 
   * V0006 control net file. 
   *
   * @param point The versioned control point to be updated.
   *  
   * @return The latest version ControlPoint constructed from the 
   *         given point.
   */
  QSharedPointer<ControlPoint> ControlNetVersioner::createPoint(const ControlPointV0006 point) {


    QSharedPointer<ControlPoint> controlPoint = new QSharedPointer<ControlPoint>(point.id().c_str());
    controlPoint->SetChooserName(point.chooserName().c_str());
    controlPoint->SetDateTime(point.dateTime().c_str());

    // setting point type
    ControlPoint::PointType pointType;
    switch (point.type()) {
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
        QString msg = "Unable to create ControlPoint [" + point.id().c_str() + "] from file. "
                      "Type enumeration [" + toString((int)(point.type())) + "] is invalid.";
        throw IException(IException::Programmer, msg, _FILEINFO_);
    }
    controlPoint->SetType(pointType);

    controlPoint->SetIgnored(point.ignore());
    controlPoint->SetRejected(point.jigsawrejected());

    // setting apriori radius information
    if (point.has_aprioriradiussource()) {
      switch (point.aprioriradiussource()) {
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

        // case ControlPointFileEntryV0002_AprioriSource_Reference:
        // case ControlPointFileEntryV0002_AprioriSource_Basemap:
        //  break;
        default:
          //throw error???
      }
      controlPoint->SetAprioriRadiusSource(aprioriRadiusSource);
    }

    if (point.has_aprioriradiussourcefile()) {
      controlPoint->SetAprioriRadiusSourceFile(point.aprioriradiussourcefile().c_str());
    }

    // setting apriori surf pt information
    if (point.has_apriorisurfpointsource()) {
      switch (point.apriorisurfpointsource()) {
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

        // case ControlPointFileEntryV0002_AprioriSource_Ellipsoid:
        // case ControlPointFileEntryV0002_AprioriSource_DEM:
        //   break;
        default:
          //throw error???
      }
      controlPoint->SetAprioriSurfacePointSource(aprioriSurfacePointSource);
    }

    if (point.has_apriorisurfpointsourcefile()) {
      controlPoint->SetAprioriSurfacePointSourceFile(point.apriorisurfpointsourcefile().c_str());
    }

    if (point.has_apriorix()
        && point.has_aprioriy()
        && point.has_aprioriz()) {

      SurfacePoint aprioriSurfacePoint(Displacement(point.apriorix(), Displacement::Meters),
                                       Displacement(point.aprioriy(), Displacement::Meters),
                                       Displacement(point.aprioriz(), Displacement::Meters));
      if (point.aprioricovar_size() > 0) {
        LinearAlgebra::UpperSymmetricMatrix aprioriCovarianceMatrix;
        aprioriCovarianceMatrix.resize(3);
        aprioriCovarianceMatrix.clear();
        aprioriCovarianceMatrix(0, 0) = point.aprioricovar(0);
        aprioriCovarianceMatrix(0, 1) = point.aprioricovar(1);
        aprioriCovarianceMatrix(0, 2) = point.aprioricovar(2);
        aprioriCovarianceMatrix(1, 1) = point.aprioricovar(3);
        aprioriCovarianceMatrix(1, 2) = point.aprioricovar(4);
        aprioriCovarianceMatrix(2, 2) = point.aprioricovar(5);
        aprioriSurfacePoint.SetRectangularMatrix(aprioriCovarianceMatrix);
// ??? TODO
#if 0
        if (Displacement(aprioriCovarianceMatrix(0, 0), Displacement::Meters).isValid() 
            || Displacement(aprioriCovarianceMatrix(1, 1), Displacement::Meters).isValid()) {
        
          if (point.latitudeconstrained()) {
            constraintStatus.set(LatitudeConstrained);
          }
          if (point.longitudeconstrained()) {
            constraintStatus.set(LongitudeConstrained);
          }
          if (point.radiusconstrained()) {
            constraintStatus.set(RadiusConstrained);
          }
        
        }
        else if (Displacement(aprioriCovarianceMatrix(2, 2), Displacement::Meters).isValid()) {
        
          if (point.latitudeconstrained()) {
            constraintStatus.set(LatitudeConstrained);
          }
          if (point.radiusconstrained()) {
            constraintStatus.set(RadiusConstrained);
          }
        
        }
#endif
      }

      controlPoint->SetAprioriSurfacePoint(point.aprioriSurfacePoint);
    }

    // setting adj surf pt information
    if (point.has_adjustedx()
        && point.has_adjustedy()
        && point.has_adjustedz()) {

      SurfacePoint adjustedSurfacePoint(Displacement(point.adjustedx(), Displacement::Meters),
                                        Displacement(point.adjustedy(), Displacement::Meters),
                                        Displacement(point.adjustedz(), Displacement::Meters));

      if (point.adjustedcovar_size() > 0) {
        LinearAlgebra::UpperSymmetricMatrix adjustedCovarianceMatrix;
        adjustedCovarianceMatrix.resize(3);
        adjustedCovarianceMatrix.clear();
        adjustedCovarianceMatrix(0, 0) = point.adjustedcovar(0);
        adjustedCovarianceMatrix(0, 1) = point.adjustedcovar(1);
        adjustedCovarianceMatrix(0, 2) = point.adjustedcovar(2);
        adjustedCovarianceMatrix(1, 1) = point.adjustedcovar(3);
        adjustedCovarianceMatrix(1, 2) = point.adjustedcovar(4);
        adjustedCovarianceMatrix(2, 2) = point.adjustedcovar(5);
        adjustedSurfacePoint.SetRectangularMatrix(adjustedCovarianceMatrix);
      }

      controlPoint->SetAdjustedSurfacePoint(point.adjustedSurfacePoint);
    }

    if (m_header.has_targetname()) {
      try {
        // attempt to get target radii values... 
        PvlGroup pvlRadii = Target::radiiGroup(m_header.targetname().c_str());
        Distance equatorialRadius(pvlRadii["EquatorialRadius"], Distance::Meters);
        Distance polarRadius(pvlRadii["PolarRadius"], Distance::Meters);
        if (equatorialRadius.isValid() && polarRadius.isValid()) {
          aprioriSurfacePoint.SetRadii(equatorialRadius, equatorialRadius, polarRadius);
          adjustedSurfacePoint.SetRadii(equatorialRadius, equatorialRadius, polarRadius);
        }

       }
       catch (IException &e) {
         // do nothing
       }
    }

    // adding measure information
    for (int m = 0 ; m < point.measures_size(); m++) {
      QSharedPointer<ControlMeasure> measure = createMeasure(point.measures(m));
      controlPoint->AddMeasure(measure);
    }

    if (point.has_referenceindex()) {
      controlPoint->SetRefMeasure(point.referenceindex());
    }

    // Set edit lock last
    controlPoint.SetEditLock(point.editLock);
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
  QSharedPointer<ControlMeasure> ControlNetVersioner::createMeasure(const ControlMeasureV0006 measure) {
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
        // throw error???
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


  void ControlNetVersioner::writeHeader(ZeroCopyInputStream *fileStream) {

  }


  void ControlNetVersioner::writeFirstPoint(ZeroCopyInputStream *fileStream) {

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
