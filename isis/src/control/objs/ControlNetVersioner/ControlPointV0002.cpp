#include "ControlPointV0002.h"

#include <QString>

#include "ControlPointV0001.h"
#include "Pvl.h"

using namespace std;

namespace Isis {

  /**
   * Create a ControlPointV0002 object from a protobuf version 1 control point message.
   *
   * @param pointData The protobuf message from a control net file.
   */
  ControlPointV0002::ControlPointV0002(
          QSharedPointer<ControlNetFileProtoV0001_PBControlPoint> pointData)
   : m_pointData(pointData) {

   }


  /**
   * Create a ControlPointV0002 object from a version 2 control point Pvl object
   *
   * @param pointObject The control point and its measures in a Pvl object
   */
  ControlPointV0002::ControlPointV0002(const Pvl &pointObject)
   : m_pointData(new ControlNetFileProtoV0001_PBControlPoint) {

    // Copy over strings, doubles, and bools
    copy(pointObject, "PointId",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_id);
    copy(pointObject, "ChooserName",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_choosername);
    copy(pointObject, "DateTime",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_datetime);
    copy(pointObject, "AprioriXYZSourceFile",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_apriorisurfpointsourcefile);
    copy(pointObject, "AprioriRadiusSourceFile",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_aprioriradiussourcefile);
    copy(pointObject, "JigsawRejected",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_jigsawrejected);
    copy(pointObject, "EditLock",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_editlock);
    copy(pointObject, "Ignore",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_ignore);
    copy(pointObject, "AprioriX",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_apriorix);
    copy(pointObject, "AprioriY",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_aprioriy);
    copy(pointObject, "AprioriZ",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_aprioriz);
    copy(pointObject, "AdjustedX",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_adjustedx);
    copy(pointObject, "AdjustedY",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_adjustedy);
    copy(pointObject, "AdjustedZ",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_adjustedz);
    copy(pointObject, "LatitudeConstrained",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_latitudeconstrained);
    copy(pointObject, "LongitudeConstrained",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_longitudeconstrained);
    copy(pointObject, "RadiusConstrained",
         m_pointData, &ControlNetFileProtoV0001_PBControlPoint::set_radiusconstrained);

    // Copy enumerated values

    // The control point type names were changed between version 3 and version 4.
    // In version 3, the types are ground, tie, and constrained
    // In version 4, these were changed to fixed, free, and constrained respectively.
    // The protobuf file version was not changed, fixed and free were simply added to the
    // enumeration and the old names were flagged as obsolete.
    if (pointObject["PointType"][0] == "Ground") {
      m_pointData->set_type(ControlNetFileProtoV0001_PBControlPoint::Ground);
    }
    else if (pointObject["PointType"][0] == "Tie") {
      m_pointData->set_type(ControlNetFileProtoV0001_PBControlPoint::Tie);
    }
    else {
      QString msg = "Invalid ControlPoint type [" + pointObject["PointType"][0] + "].";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if (pointObject.hasKeyword("AprioriXYZSource")) {
      QString source = pointObject["AprioriXYZSource"][0];

      if (source == "None") {
        m_pointData->set_apriorisurfpointsource(ControlNetFileProtoV0001_PBControlPoint::None);
      }
      else if (source == "User") {
        m_pointData->set_apriorisurfpointsource(ControlNetFileProtoV0001_PBControlPoint::User);
      }
      else if (source == "AverageOfMeasures") {
        m_pointData->set_apriorisurfpointsource(ControlNetFileProtoV0001_PBControlPoint::AverageOfMeasures);
      }
      else if (source == "Reference") {
        m_pointData->set_apriorisurfpointsource(ControlNetFileProtoV0001_PBControlPoint::Reference);
      }
      else if (source == "Basemap") {
        m_pointData->set_apriorisurfpointsource(ControlNetFileProtoV0001_PBControlPoint::Basemap);
      }
      else if (source == "BundleSolution") {
        m_pointData->set_apriorisurfpointsource(ControlNetFileProtoV0001_PBControlPoint::BundleSolution);
      }
      else {
        QString msg = "Invalid AprioriXYZSource [" + source + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    if (pointObject.hasKeyword("AprioriRadiusSource")) {
      QString source = pointObject["AprioriRadiusSource"][0];

      if (source == "None") {
        m_pointData->set_aprioriradiussource(ControlNetFileProtoV0001_PBControlPoint::None);
      }
      else if (source == "User") {
        m_pointData->set_aprioriradiussource(ControlNetFileProtoV0001_PBControlPoint::User);
      }
      else if (source == "AverageOfMeasures") {
        m_pointData->set_aprioriradiussource(ControlNetFileProtoV0001_PBControlPoint::AverageOfMeasures);
      }
      else if (source == "Ellipsoid") {
        m_pointData->set_aprioriradiussource(ControlNetFileProtoV0001_PBControlPoint::Ellipsoid);
      }
      else if (source == "DEM") {
        m_pointData->set_aprioriradiussource(ControlNetFileProtoV0001_PBControlPoint::DEM);
      }
      else if (source == "BundleSolution") {
        m_pointData->set_aprioriradiussource(ControlNetFileProtoV0001_PBControlPoint::BundleSolution);
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
      ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure measure;

      // Copy strings, booleans, and doubles
      copy(group, "SerialNumber",
           measure, &ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::set_serialnumber);
      copy(group, "ChooserName",
           measure, &ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::set_choosername);
      copy(group, "DateTime",
           measure, &ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::set_datetime);
      copy(group, "Diameter",
           measure, &ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::set_diameter);
      copy(group, "EditLock",
           measure, &ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::set_editlock);
      copy(group, "Ignore",
           measure, &ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::set_ignore);
      copy(group, "JigsawRejected",
           measure, &ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::set_jigsawrejected);
      copy(group, "AprioriSample",
           measure, &ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::set_apriorisample);
      copy(group, "AprioriLine",
           measure, &ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::set_aprioriline);
      copy(group, "SampleSigma",
           measure, &ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::set_samplesigma);
      copy(group, "LineSigma",
           measure, &ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::set_linesigma);

      // The sample, line, sample residual, and line residual are nested in another structure
      // inside the measure, so they cannot be copied with the conenience methods.
      if (group.hasKeyword("Sample")) {
        double value = toDouble(group["Sample"][0]);
        measure.measurement().set_sample(value);
      }
      if (group.hasKeyword("Line")) {
        double value = toDouble(group["Line"][0]);
        measure.measurement().set_line(value);
      }
      if (group.hasKeyword("SampleResidual")) {
        double value = toDouble(group["SampleResidual"][0]);
        measure.measurement().set_sampleresidual(value);
      }
      if (group.hasKeyword("LineResidual")) {
        double value = toDouble(group["LineResidual"][0]);
        measure.measurement().set_lineresidual(value);
      }


      if (group.hasKeyword("Reference")) {
        if (group["Reference"][0].toLower() == "true") {
          m_pointData->set_referenceindex(groupIndex);
        }
        group.deleteKeyword("Reference");
      }

      QString type = group["MeasureType"][0].toLower();
      if (type == "candidate") {
        measure.set_type(ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::Candidate);
      }
      else if (type == "manual") {
        measure.set_type(ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::Manual);
      }
      else if (type == "registeredpixel") {
        measure.set_type(ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::RegisteredPixel);
      }
      else if (type == "registeredsubpixel") {
        measure.set_type(ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::RegisteredSubPixel);
      }
      else {
        throw IException(IException::Io,
                         "Unknown measure type [" + type + "]",
                         _FILEINFO_);
      }
      group.deleteKeyword("MeasureType");

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


  /**
   * Create a version 2 control point from a version 1 control point. The two versions actually
   * store the same values, so all this does is copy the internal protobuf object.
   *
   * @param oldPoint The old version 1 control point.
   */
  ControlPointV0002::ControlPointV0002(ControlPointV0001 &oldPoint)
   : m_pointData(oldPoint->pointData()) {

  }


  /**
   * Access the protobuf control point data. If there is not internal point data then
   * default point data is returned. Note that default point data may be missing required
   * fields.
   *
   * @return @b const ControlNetFileProtoV0001_PBControlPoint& A constant reference to the internal
   *                                                           control point data. There is no
   *                                                           guarantee that the point data is
   *                                                           fully initialized.
   */
  const ControlNetFileProtoV0001_PBControlPoint &ControlPointV0002::pointData() {
      if (!m_pointData) {
        m_pointData.reset(new ControlNetFileProtoV0001_PBControlPoint);
      }

      return *m_pointData;
  }


  /**
   * This convenience method takes a boolean value from a PvlKeyword and copies it into a version 1
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control point that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param point[out] The version 1 protobuf representation of the control point that the value
   *                   will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0002::copy(PvlContainer &container,
                               QString keyName,
                               QSharedPointer<ControlNetFileProtoV0001_PBControlPoint> point,
                               void (ControlNetFileProtoV0001_PBControlPoint::*setter)(bool)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    container.deleteKeyword(keyName);
    value = value.toLower();

    if (value == "true" || value == "yes")
      (point->*setter)(true);
  }


  /**
   * This convenience method takes a double value from a PvlKeyword and copies it into a version 1
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control point that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param point[out] The version 1 protobuf representation of the control point that the value
   *                   will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0002::copy(PvlContainer &container,
                               QString keyName,
                               QSharedPointer<ControlNetFileProtoV0001_PBControlPoint> &point,
                               void (ControlNetFileProtoV0001_PBControlPoint::*setter)(double)) {

    if (!container.hasKeyword(keyName))
      return;

    double value = toDouble(container[keyName][0]);
    container.deleteKeyword(keyName);
    (point->*setter)(value);
  }


  /**
   * This convenience method takes a string value from a PvlKeyword and copies it into a version 1
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control point that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param point[out] The version 1 protobuf representation of the control point that the value
   *                   will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0002::copy(PvlContainer &container,
                               QString keyName,
                               QSharedPointer<ControlNetFileProtoV0001_PBControlPoint> &point,
                               void (ControlNetFileProtoV0001_PBControlPoint::*setter)(const std::string&)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    container.deleteKeyword(keyName);
    (point->*setter)(value);
  }


  /**
   * This convenience method takes a boolean value from a PvlKeyword and copies it into a version 1
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control measure that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param[out] measure The version 1 protobuf representation of the control measure that the
   *                     value will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0002::copy(PvlContainer &container,
                               QString keyName,
                               ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure &measure,
                               void (ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::*setter)(bool)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    container.deleteKeyword(keyName);
    value = value.toLower();

    if (value == "true" || value == "yes")
      (measure.*setter)(true);
  }


  /**
   * This convenience method takes a double value from a PvlKeyword and copies it into a version 1
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control measure that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param[out] measure The version 1 protobuf representation of the control measure that the
   *                     value will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0002::copy(PvlContainer &container,
                               QString keyName,
                               ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure &measure,
                               void (ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::*setter)(double)) {

    if (!container.hasKeyword(keyName))
      return;

    double value = toDouble(container[keyName][0]);
    container.deleteKeyword(keyName);
    (measure.*setter)(value);
  }


  /**
   * This convenience method takes a string value from a PvlKeyword and copies it into a version 1
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control measure that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param[out] measure The version 1 protobuf representation of the control measure that the
   *                     value will be into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0002::copy(PvlContainer &container,
                               QString keyName,
                               ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure &measure,
                               void (ControlNetFileProtoV0001_PBControlPoint::PBControlMeasure::*setter)
                                      (const std::string &)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    container.deleteKeyword(keyName);
    (measure.*set)(value);
  }
}
