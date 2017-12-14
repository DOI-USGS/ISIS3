#include "ControlPointV0005.h"

#include <QString>

#include "ControlPointV0004.h"
#include "Pvl.h"

using namespace std;

namespace Isis {

  /**
   * Create a ControlPointV0005 object from a protobuf version 2 control point message.
   *
   * @param pointData The protobuf message from a control net file.
   */
  ControlPointV0005::ControlPointV0005(QSharedPointer<ControlPointFileEntryV0002> pointData)
   : m_pointData(pointData) {

   }


  /**
   * Create a ControlPointV0005 object from a version 4 control point Pvl object
   *
   * @param pointObject The control point and its measures in a Pvl object
   */
  ControlPointV0005::ControlPointV0005(const Pvl &pointObject)
   : m_pointData(new ControlPointFileEntryV0002) {

    // Copy over strings, doubles, and bools
    copy(pointObject, "PointId",
         m_pointData, &ControlPointFileEntryV0002::set_id);
    copy(pointObject, "ChooserName",
         m_pointData, &ControlPointFileEntryV0002::set_choosername);
    copy(pointObject, "DateTime",
         m_pointData, &ControlPointFileEntryV0002::set_datetime);
    copy(pointObject, "AprioriXYZSourceFile",
         m_pointData, &ControlPointFileEntryV0002::set_apriorisurfpointsourcefile);
    copy(pointObject, "AprioriRadiusSourceFile",
         m_pointData, &ControlPointFileEntryV0002::set_aprioriradiussourcefile);
    copy(pointObject, "JigsawRejected",
         m_pointData, &ControlPointFileEntryV0002::set_jigsawrejected);
    copy(pointObject, "EditLock",
         m_pointData, &ControlPointFileEntryV0002::set_editlock);
    copy(pointObject, "Ignore",
         m_pointData, &ControlPointFileEntryV0002::set_ignore);
    copy(pointObject, "AprioriX",
         m_pointData, &ControlPointFileEntryV0002::set_apriorix);
    copy(pointObject, "AprioriY",
         m_pointData, &ControlPointFileEntryV0002::set_aprioriy);
    copy(pointObject, "AprioriZ",
         m_pointData, &ControlPointFileEntryV0002::set_aprioriz);
    copy(pointObject, "AdjustedX",
         m_pointData, &ControlPointFileEntryV0002::set_adjustedx);
    copy(pointObject, "AdjustedY",
         m_pointData, &ControlPointFileEntryV0002::set_adjustedy);
    copy(pointObject, "AdjustedZ",
         m_pointData, &ControlPointFileEntryV0002::set_adjustedz);
    copy(pointObject, "LatitudeConstrained",
         m_pointData, &ControlPointFileEntryV0002::set_latitudeconstrained);
    copy(pointObject, "LongitudeConstrained",
         m_pointData, &ControlPointFileEntryV0002::set_longitudeconstrained);
    copy(pointObject, "RadiusConstrained",
         m_pointData, &ControlPointFileEntryV0002::set_radiusconstrained);

    // Copy enumerated values
    if (pointObject["PointType"][0] == "Fixed") {
      m_pointData->set_type(ControlPointFileEntryV0002::Fixed);
    }
    else if (pointObject["PointType"][0] == "Constrained") {
      m_pointData->set_type(ControlPointFileEntryV0002::Constrained);
    }
    else {
      m_pointData->set_type(ControlPointFileEntryV0002::Free);
    }

    if (pointObject.hasKeyword("AprioriXYZSource")) {
      QString source = pointObject["AprioriXYZSource"][0];

      if (source == "None") {
        m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0002::None);
      }
      else if (source == "User") {
        m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0002::User);
      }
      else if (source == "AverageOfMeasures") {
        m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0002::AverageOfMeasures);
      }
      else if (source == "Reference") {
        m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0002::Reference);
      }
      else if (source == "Basemap") {
        m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0002::Basemap);
      }
      else if (source == "BundleSolution") {
        m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0002::BundleSolution);
      }
      else {
        QString msg = "Invalid AprioriXYZSource [" + source + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    if (pointObject.hasKeyword("AprioriRadiusSource")) {
      QString source = pointObject["AprioriRadiusSource"][0];

      if (source == "None") {
        m_pointData->set_aprioriradiussource(ControlPointFileEntryV0002::None);
      }
      else if (source == "User") {
        m_pointData->set_aprioriradiussource(ControlPointFileEntryV0002::User);
      }
      else if (source == "AverageOfMeasures") {
        m_pointData->set_aprioriradiussource(ControlPointFileEntryV0002::AverageOfMeasures);
      }
      else if (source == "Ellipsoid") {
        m_pointData->set_aprioriradiussource(ControlPointFileEntryV0002::Ellipsoid);
      }
      else if (source == "DEM") {
        m_pointData->set_aprioriradiussource(ControlPointFileEntryV0002::DEM);
      }
      else if (source == "BundleSolution") {
        m_pointData->set_aprioriradiussource(ControlPointFileEntryV0002::BundleSolution);
      }
      else {
        QString msg = "Invalid AprioriRadiusSource, [" + source + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    // Copy array values
    if (pointObject.hasKeyword("AprioriCovarianceMatrix")) {
      PvlKeyword &matrix = pointObject["AprioriCovarianceMatrix"];

      m_pointData->add_aprioricovar(toDouble(matrix[0]));
      m_pointData->add_aprioricovar(toDouble(matrix[1]));
      m_pointData->add_aprioricovar(toDouble(matrix[2]));
      m_pointData->add_aprioricovar(toDouble(matrix[3]));
      m_pointData->add_aprioricovar(toDouble(matrix[4]));
      m_pointData->add_aprioricovar(toDouble(matrix[5]));
    }

    if (pointObject.hasKeyword("AdjustedCovarianceMatrix")) {
      PvlKeyword &matrix = pointObject["AdjustedCovarianceMatrix"];

      m_pointData->add_adjustedcovar(toDouble(matrix[0]));
      m_pointData->add_adjustedcovar(toDouble(matrix[1]));
      m_pointData->add_adjustedcovar(toDouble(matrix[2]));
      m_pointData->add_adjustedcovar(toDouble(matrix[3]));
      m_pointData->add_adjustedcovar(toDouble(matrix[4]));
      m_pointData->add_adjustedcovar(toDouble(matrix[5]));
    }

    //  Process Measures
    for (int groupIndex = 0; groupIndex < pointObject.groups(); groupIndex ++) {
      PvlGroup &group = pointObject.group(groupIndex);
      ControlPointFileEntryV0002::Measure measure;

      // Copy strings, booleans, and doubles
      copy(group, "SerialNumber",
           measure, &ControlPointFileEntryV0002::Measure::set_serialnumber);
      copy(group, "ChooserName",
           measure, &ControlPointFileEntryV0002::Measure::set_choosername);
      copy(group, "Sample",
           measure, &ControlPointFileEntryV0002::Measure::set_sample);
      copy(group, "Line",
           measure, &ControlPointFileEntryV0002::Measure::set_line);
      copy(group, "SampleResidual",
           measure, &ControlPointFileEntryV0002::Measure::set_sampleresidual);
      copy(group, "LineResidual",
           measure, &ControlPointFileEntryV0002::Measure::set_lineresidual);
      copy(group, "DateTime",
           measure, &ControlPointFileEntryV0002::Measure::set_datetime);
      copy(group, "Diameter",
           measure, &ControlPointFileEntryV0002::Measure::set_diameter);
      copy(group, "EditLock",
           measure, &ControlPointFileEntryV0002::Measure::set_editlock);
      copy(group, "Ignore",
           measure, &ControlPointFileEntryV0002::Measure::set_ignore);
      copy(group, "JigsawRejected",
           measure, &ControlPointFileEntryV0002::Measure::set_jigsawrejected);
      copy(group, "AprioriSample",
           measure, &ControlPointFileEntryV0002::Measure::set_apriorisample);
      copy(group, "AprioriLine",
           measure, &ControlPointFileEntryV0002::Measure::set_aprioriline);
      copy(group, "SampleSigma",
           measure, &ControlPointFileEntryV0002::Measure::set_samplesigma);
      copy(group, "LineSigma",
           measure, &ControlPointFileEntryV0002::Measure::set_linesigma);

      if (group.hasKeyword("Reference")) {
        if (group["Reference"][0].toLower() == "true")
          m_pointData->set_referenceindex(groupIndex);
      }

      QString type = group["MeasureType"][0].toLower();
      if (type == "candidate") {
        measure.set_type(ControlPointFileEntryV0002::Measure::Candidate);
      }
      else if (type == "manual") {
        measure.set_type(ControlPointFileEntryV0002::Measure::Manual);
      }
      else if (type == "registeredpixel") {
        measure.set_type(ControlPointFileEntryV0002::Measure::RegisteredPixel);
      }
      else if (type == "registeredsubpixel") {
        measure.set_type(ControlPointFileEntryV0002::Measure::RegisteredSubPixel);
      }
      else {
        throw IException(IException::Io,
                         "Unknown measure type [" + type + "]",
                         _FILEINFO_);
        }

      for (int key = 0; key < group.keywords(); key++) {
        ControlMeasureLogData interpreter(group[key]);
        if (!interpreter.IsValid()) {
          QString msg = "Unhandled or duplicate keywords in control measure ["
                        + group[key].name() + "]";
          throw IException(IException::Programmer, msg, _FILEINFO_);
        }
        else {
          *measure.add_log() = interpreter.ToProtocolBuffer();
        }
      }

      *m_pointData->add_measures() = measure;
    }

    if (!m_pointData->IsInitialized()) {
      QString msg = "There is missing required information in the control "
                    "points or measures";
      throw IException(IException::Io, msg, _FILEINFO_);
    }
  }


  // TODO finish this once Version 4 is created
  ControlPointV0005::ControlPointV0005(const ControlPointV0004 &oldPoint);


  /**
   * Create a PvlObject representation of the control point.
   *
   * @return @b PvlObject A PvlObject containing all of the point's information and measures.
   */
  PvlObject ControlPointV0005::toPvl() {
    PvlObject pvlPoint("ControlPoint");

    if (m_pointData->type() == ControlPointFileEntryV0002::Fixed) {
      pvlPoint += PvlKeyword("PointType", "Fixed");
    }
    else if (m_pointData->type() == ControlPointFileEntryV0002::Constrained) {
      pvlPoint += PvlKeyword("PointType", "Constrained");
    }
    else {
      pvlPoint += PvlKeyword("PointType", "Free");
    }

    pvlPoint += PvlKeyword("PointId", m_pointData->id().c_str());
    pvlPoint += PvlKeyword("ChooserName", m_pointData->choosername().c_str());
    pvlPoint += PvlKeyword("DateTime", m_pointData->datetime().c_str());

    if (m_pointData->editlock()) {
      pvlPoint += PvlKeyword("EditLock", "True");
    }

    if (m_pointData->ignore()) {
      pvlPoint += PvlKeyword("Ignore", "True");
    }

    switch (m_pointData->apriorisurfpointsource()) {
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

    if (m_pointData->has_apriorisurfpointsourcefile()) {
      pvlPoint += PvlKeyword("AprioriXYZSourceFile",
                      m_pointData->apriorisurfpointsourcefile().c_str());
    }

    switch (m_pointData->aprioriradiussource()) {
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

    if (m_pointData->has_aprioriradiussourcefile()) {
      pvlPoint += PvlKeyword("AprioriRadiusSourceFile",
                      m_pointData->aprioriradiussourcefile().c_str());
    }

    if (m_pointData->has_apriorix()) {
      pvlPoint += PvlKeyword("AprioriX", toString(m_pointData->apriorix()), "meters");
      pvlPoint += PvlKeyword("AprioriY", toString(m_pointData->aprioriy()), "meters");
      pvlPoint += PvlKeyword("AprioriZ", toString(m_pointData->aprioriz()), "meters");

      // Get surface point, convert to lat,lon,radius and output as comment
      SurfacePoint apriori;
      apriori.SetRectangular(
              Displacement(m_pointData->apriorix(),Displacement::Meters),
              Displacement(m_pointData->aprioriy(),Displacement::Meters),
              Displacement(m_pointData->aprioriz(),Displacement::Meters));
      pvlPoint.findKeyword("AprioriX").addComment("AprioriLatitude = " +
                               toString(apriori.GetLatitude().degrees()) +
                               " <degrees>");
      pvlPoint.findKeyword("AprioriY").addComment("AprioriLongitude = " +
                               toString(apriori.GetLongitude().degrees()) +
                               " <degrees>");
      pvlPoint.findKeyword("AprioriZ").addComment("AprioriRadius = " +
                               toString(apriori.GetLocalRadius().meters()) +
                               " <meters>");

      if (m_pointData->aprioricovar_size()) {
        PvlKeyword matrix("AprioriCovarianceMatrix");
        matrix += toString(m_pointData->aprioricovar(0));
        matrix += toString(m_pointData->aprioricovar(1));
        matrix += toString(m_pointData->aprioricovar(2));
        matrix += toString(m_pointData->aprioricovar(3));
        matrix += toString(m_pointData->aprioricovar(4));
        matrix += toString(m_pointData->aprioricovar(5));
        pvlPoint += matrix;

        if (pvlRadii.hasKeyword("EquatorialRadius")) {
          apriori.SetRadii(
                       Distance(pvlRadii["EquatorialRadius"],Distance::Meters),
                       Distance(pvlRadii["EquatorialRadius"],Distance::Meters),
                       Distance(pvlRadii["PolarRadius"],Distance::Meters));
          symmetric_matrix<double, upper> covar;
          covar.resize(3);
          covar.clear();
          covar(0, 0) = m_pointData->aprioricovar(0);
          covar(0, 1) = m_pointData->aprioricovar(1);
          covar(0, 2) = m_pointData->aprioricovar(2);
          covar(1, 1) = m_pointData->aprioricovar(3);
          covar(1, 2) = m_pointData->aprioricovar(4);
          covar(2, 2) = m_pointData->aprioricovar(5);
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

    if (m_pointData->latitudeconstrained()) {
      pvlPoint += PvlKeyword("LatitudeConstrained", "True");
    }

    if (m_pointData->longitudeconstrained()) {
      pvlPoint += PvlKeyword("LongitudeConstrained", "True");
    }

    if (m_pointData->radiusconstrained()) {
      pvlPoint += PvlKeyword("RadiusConstrained", "True");
    }

    if (m_pointData->has_adjustedx()) {
      pvlPoint += PvlKeyword("AdjustedX", toString(m_pointData->adjustedx()), "meters");
      pvlPoint += PvlKeyword("AdjustedY", toString(m_pointData->adjustedy()), "meters");
      pvlPoint += PvlKeyword("AdjustedZ", toString(m_pointData->adjustedz()), "meters");

      // Get surface point, convert to lat,lon,radius and output as comment
      SurfacePoint adjusted;
      adjusted.SetRectangular(
              Displacement(m_pointData->adjustedx(),Displacement::Meters),
              Displacement(m_pointData->adjustedy(),Displacement::Meters),
              Displacement(m_pointData->adjustedz(),Displacement::Meters));
      pvlPoint.findKeyword("AdjustedX").addComment("AdjustedLatitude = " +
                               toString(adjusted.GetLatitude().degrees()) +
                               " <degrees>");
      pvlPoint.findKeyword("AdjustedY").addComment("AdjustedLongitude = " +
                               toString(adjusted.GetLongitude().degrees()) +
                               " <degrees>");
      pvlPoint.findKeyword("AdjustedZ").addComment("AdjustedRadius = " +
                               toString(adjusted.GetLocalRadius().meters()) +
                               " <meters>");

      if (m_pointData->adjustedcovar_size()) {
        PvlKeyword matrix("AdjustedCovarianceMatrix");
        matrix += toString(m_pointData->adjustedcovar(0));
        matrix += toString(m_pointData->adjustedcovar(1));
        matrix += toString(m_pointData->adjustedcovar(2));
        matrix += toString(m_pointData->adjustedcovar(3));
        matrix += toString(m_pointData->adjustedcovar(4));
        matrix += toString(m_pointData->adjustedcovar(5));
        pvlPoint += matrix;

        if (pvlRadii.hasKeyword("EquatorialRadius")) {
          adjusted.SetRadii(
                       Distance(pvlRadii["EquatorialRadius"],Distance::Meters),
                       Distance(pvlRadii["EquatorialRadius"],Distance::Meters),
                       Distance(pvlRadii["PolarRadius"],Distance::Meters));
          symmetric_matrix<double, upper> covar;
          covar.resize(3);
          covar.clear();
          covar(0, 0) = m_pointData->adjustedcovar(0);
          covar(0, 1) = m_pointData->adjustedcovar(1);
          covar(0, 2) = m_pointData->adjustedcovar(2);
          covar(1, 1) = m_pointData->adjustedcovar(3);
          covar(1, 2) = m_pointData->adjustedcovar(4);
          covar(2, 2) = m_pointData->adjustedcovar(5);
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

    for (int j = 0; j < m_pointData->measures_size(); j++) {
      PvlGroup pvlMeasure("ControlMeasure");
      const ControlPointFileEntryV0002_Measure &
          binaryMeasure = m_pointData->measures(j);
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

      if (binaryMeasure.has_choosername()) {
        pvlMeasure += PvlKeyword("ChooserName", binaryMeasure.choosername().c_str());
      }

      if (binaryMeasure.has_datetime()) {
        pvlMeasure += PvlKeyword("DateTime", binaryMeasure.datetime().c_str());
      }

      if (binaryMeasure.editlock()) {
        pvlMeasure += PvlKeyword("EditLock", "True");
      }

      if (binaryMeasure.ignore()) {
        pvlMeasure += PvlKeyword("Ignore", "True");
      }

      if (binaryMeasure.has_sample()) {
        pvlMeasure += PvlKeyword("Sample", toString(binaryMeasure.sample()));
      }

      if (binaryMeasure.has_line()) {
        pvlMeasure += PvlKeyword("Line", toString(binaryMeasure.line()));
      }

      if (binaryMeasure.has_diameter()) {
        pvlMeasure += PvlKeyword("Diameter", toString(binaryMeasure.diameter()));
      }

      if (binaryMeasure.has_apriorisample()) {
        pvlMeasure += PvlKeyword("AprioriSample", toString(binaryMeasure.apriorisample()));
      }

      if (binaryMeasure.has_aprioriline()) {
        pvlMeasure += PvlKeyword("AprioriLine", toString(binaryMeasure.aprioriline()));
      }

      if (binaryMeasure.has_samplesigma()) {
        pvlMeasure += PvlKeyword("SampleSigma", toString(binaryMeasure.samplesigma()),
                                 "pixels");
      }

      if (binaryMeasure.has_samplesigma()) {
        pvlMeasure += PvlKeyword("LineSigma", toString(binaryMeasure.linesigma()),
                                 "pixels");
      }

      if (binaryMeasure.has_sampleresidual()) {
        pvlMeasure += PvlKeyword("SampleResidual", toString(binaryMeasure.sampleresidual()),
                                 "pixels");
      }

      if (binaryMeasure.has_lineresidual()) {
        pvlMeasure += PvlKeyword("LineResidual", toString(binaryMeasure.lineresidual()),
                                 "pixels");
      }

      if (binaryMeasure.has_jigsawrejected()) {
        pvlMeasure += PvlKeyword("JigsawRejected", toString(binaryMeasure.jigsawrejected()));
      }

      for (int logEntry = 0;
           logEntry < binaryMeasure.log_size();
           logEntry ++) {
        ControlPointFileEntryV0002_Measure_MeasureLogData &log = binaryMeasure.log(logEntry);

        ControlMeasureLogData interpreter(log);
        pvlMeasure += interpreter.ToKeyword();
      }

      if (binaryPoint.has_referenceindex() &&
          binaryPoint.referenceindex() == j) {
        pvlMeasure += PvlKeyword("Reference", "True");
      }

      pvlPoint.addGroup(pvlMeasure);
    }

    return pvlPoint;
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
  void ControlPointV0005::copy(PvlContainer &container,
                               QString keyName,
                               QSharedPointer<ControlPointFileEntryV0002> point,
                               void (ControlPointFileEntryV0002::*setter)(bool)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    value = value.toLower();

    if (value == "true" || value == "yes")
      (point->*setter)(true);
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
  void ControlPointV0005::copy(PvlContainer &container,
                               QString keyName,
                               QSharedPointer<ControlPointFileEntryV0002> &point,
                               void (ControlPointFileEntryV0002::*setter)(double)) {

    if (!container.hasKeyword(keyName))
      return;

    double value = toDouble(container[keyName][0]);
    (point->*setter)(value);
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
  void ControlPointV0005::copy(PvlContainer &container,
                               QString keyName,
                               QSharedPointer<ControlPointFileEntryV0002> &point,
                               void (ControlPointFileEntryV0002::*setter)(const std::string&)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    (point->*setter)(value);
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
  void ControlPointV0005::copy(PvlContainer &container,
                               QString keyName,
                               ControlPointFileEntryV0002::Measure &measure,
                               void (ControlPointFileEntryV0002::Measure::*setter)(bool)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
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
  void ControlPointV0005::copy(PvlContainer &container,
                               QString keyName,
                               ControlPointFileEntryV0002::Measure &measure,
                               void (ControlPointFileEntryV0002::Measure::*setter)(double)) {

    if (!container.hasKeyword(keyName))
      return;

    double value = toDouble(container[keyName][0]);
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
  void ControlPointV0005::copy(PvlContainer &container,
                               QString keyName,
                               ControlPointFileEntryV0002::Measure &measure,
                               void (ControlPointFileEntryV0002::Measure::*setter)
                                      (const std::string &)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    (measure.*set)(value);
  }
}
