#include "ControlPointV0003.h"

#include <QString>

#include "ControlPointV0002.h"
#include "Pvl.h"

using namespace std;

namespace Isis {

  /**
   * Create a ControlPointV0003 object from a protobuf version 2 control point message.
   *
   * @param pointData The protobuf message from a control net file.
   */
  ControlPointV0003::ControlPointV0003(QSharedPointer<ControlPointFileEntryV0002> pointData)
   : m_pointData(pointData) {

   }


  /**
   * Create a ControlPointV0003 object from a version 4 control point Pvl object
   *
   * @param pointObject The control point and its measures in a Pvl object
   */
  ControlPointV0003::ControlPointV0003(const Pvl &pointObject)
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

    // The control point type names were changed between version 3 and version 4.
    // In version 3, the types are ground, tie, and constrained
    // In version 4, these were changed to fixed, free, and constrained respectively.
    // The protobuf file version was not changed, fixed and free were simply added to the
    // enumeration and the old names were flagged as obsolete.
    if (pointObject["PointType"][0] == "Fixed" ||
        pointObject["PointType"][0] == "Ground") {
      m_pointData->set_type(ControlPointFileEntryV0002::Fixed);
    }
    else if (pointObject["PointType"][0] == "Constrained") {
      m_pointData->set_type(ControlPointFileEntryV0002::Constrained);
    }
    else if (pointObject["PointType"][0] == "Free" ||
             pointObject["PointType"][0] == "Tie") {
      m_pointData->set_type(ControlPointFileEntryV0002::Free);
    }
    else {
      QString msg = "Invalid ControlPoint type [" + pointObject["PointType"][0] + "].";
      throw IException(IException::User, msg, _FILEINFO_);
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


  // TODO finish this once Version 2 is created
  ControlPointV0003::ControlPointV0003(const ControlPointV0002 &oldPoint);


  /**
   * This convenience method takes a boolean value from a PvlKeyword and copies it into a version 2
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control point that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param point[out] The version 2 protobuf representation of the control point that the value
   *                   will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0003::copy(PvlContainer &container,
                               QString keyName,
                               QSharedPointer<ControlPointFileEntryV0002> point,
                               void (ControlPointFileEntryV0002::*setter)(bool)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    container.deleteKeyword(keyName);
    value = value.toLower();

    if (value == "true" || value == "yes")
      (point->*setter)(true);
  }


  /**
   * This convenience method takes a double value from a PvlKeyword and copies it into a version 2
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control point that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param point[out] The version 2 protobuf representation of the control point that the value
   *                   will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0003::copy(PvlContainer &container,
                               QString keyName,
                               QSharedPointer<ControlPointFileEntryV0002> &point,
                               void (ControlPointFileEntryV0002::*setter)(double)) {

    if (!container.hasKeyword(keyName))
      return;

    double value = toDouble(container[keyName][0]);
    container.deleteKeyword(keyName);
    (point->*setter)(value);
  }


  /**
   * This convenience method takes a string value from a PvlKeyword and copies it into a version 2
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control point that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param point[out] The version 2 protobuf representation of the control point that the value
   *                   will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0003::copy(PvlContainer &container,
                               QString keyName,
                               QSharedPointer<ControlPointFileEntryV0002> &point,
                               void (ControlPointFileEntryV0002::*setter)(const std::string&)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    container.deleteKeyword(keyName);
    (point->*setter)(value);
  }


  /**
   * This convenience method takes a boolean value from a PvlKeyword and copies it into a version 2
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control measure that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param[out] measure The version 2 protobuf representation of the control measure that the
   *                     value will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0003::copy(PvlContainer &container,
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
   * This convenience method takes a double value from a PvlKeyword and copies it into a version 2
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control measure that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param[out] measure The version 2 protobuf representation of the control measure that the
   *                     value will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0003::copy(PvlContainer &container,
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
   * This convenience method takes a string value from a PvlKeyword and copies it into a version 2
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control measure that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param[out] measure The version 2 protobuf representation of the control measure that the
   *                     value will be into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0003::copy(PvlContainer &container,
                               QString keyName,
                               ControlPointFileEntryV0002::Measure &measure,
                               void (ControlPointFileEntryV0002::Measure::*setter)
                                      (const std::string &)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    container.deleteKeyword(keyName);
    (measure.*set)(value);
  }
}
