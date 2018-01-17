#include "ControlPointV0005.h"

#include <QString>

#include "ControlMeasureLogData.h"
#include "ControlPointFileEntryV0005.pb.h"
#include "ControlPointV0003.h"
#include "IException.h"
#include "PvlObject.h"
#include "PvlContainer.h"

using namespace std;

namespace Isis {

  /**
   * Create a ControlPointV0005 object from a protobuf version 5 control point message.
   *
   * @param pointData The protobuf message from a control net file.
   */
  ControlPointV0005::ControlPointV0005(QSharedPointer<ControlPointFileEntryV0005> pointData)
   : m_pointData(pointData) {

   }


  /**
   * Create a ControlPointV0005 object from a version 4 control point Pvl object
   *
   * @param pointObject The control point and its measures in a Pvl object
   */
  ControlPointV0005::ControlPointV0005(PvlObject &pointObject)
   : m_pointData(new ControlPointFileEntryV0005) {

    // Copy over strings, doubles, and bools
    copy(pointObject, "PointId",
         m_pointData, &ControlPointFileEntryV0005::set_id);
    copy(pointObject, "ChooserName",
         m_pointData, &ControlPointFileEntryV0005::set_choosername);
    copy(pointObject, "DateTime",
         m_pointData, &ControlPointFileEntryV0005::set_datetime);
    copy(pointObject, "AprioriXYZSourceFile",
         m_pointData, &ControlPointFileEntryV0005::set_apriorisurfpointsourcefile);
    copy(pointObject, "AprioriRadiusSourceFile",
         m_pointData, &ControlPointFileEntryV0005::set_aprioriradiussourcefile);
    copy(pointObject, "JigsawRejected",
         m_pointData, &ControlPointFileEntryV0005::set_jigsawrejected);
    copy(pointObject, "EditLock",
         m_pointData, &ControlPointFileEntryV0005::set_editlock);
    copy(pointObject, "Ignore",
         m_pointData, &ControlPointFileEntryV0005::set_ignore);
    copy(pointObject, "AprioriX",
         m_pointData, &ControlPointFileEntryV0005::set_apriorix);
    copy(pointObject, "AprioriY",
         m_pointData, &ControlPointFileEntryV0005::set_aprioriy);
    copy(pointObject, "AprioriZ",
         m_pointData, &ControlPointFileEntryV0005::set_aprioriz);
    copy(pointObject, "AdjustedX",
         m_pointData, &ControlPointFileEntryV0005::set_adjustedx);
    copy(pointObject, "AdjustedY",
         m_pointData, &ControlPointFileEntryV0005::set_adjustedy);
    copy(pointObject, "AdjustedZ",
         m_pointData, &ControlPointFileEntryV0005::set_adjustedz);
    copy(pointObject, "LatitudeConstrained",
         m_pointData, &ControlPointFileEntryV0005::set_latitudeconstrained);
    copy(pointObject, "LongitudeConstrained",
         m_pointData, &ControlPointFileEntryV0005::set_longitudeconstrained);
    copy(pointObject, "RadiusConstrained",
         m_pointData, &ControlPointFileEntryV0005::set_radiusconstrained);

    // Copy enumerated values

    // The control point type names were changed between version 3 and version 4.
    // In version 3, the types are ground, tie, and constrained
    // In version 4, these were changed to fixed, free, and constrained respectively.
    // The protobuf file version was not changed, fixed and free were simply added to the
    // enumeration and the old names were flagged as obsolete.
    if (pointObject["PointType"][0] == "Fixed" ||
        pointObject["PointType"][0] == "Ground") {
      m_pointData->set_type(ControlPointFileEntryV0005::Fixed);
    }
    else if (pointObject["PointType"][0] == "Constrained") {
      m_pointData->set_type(ControlPointFileEntryV0005::Constrained);
    }
    else if (pointObject["PointType"][0] == "Free" ||
             pointObject["PointType"][0] == "Tie") {
      m_pointData->set_type(ControlPointFileEntryV0005::Free);
    }
    else {
      QString msg = "Invalid ControlPoint type [" + pointObject["PointType"][0] + "].";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if (pointObject.hasKeyword("AprioriXYZSource")) {
      QString source = pointObject["AprioriXYZSource"][0];

      if (source == "None") {
        m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0005::None);
      }
      else if (source == "User") {
        m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0005::User);
      }
      else if (source == "AverageOfMeasures") {
        m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0005::AverageOfMeasures);
      }
      else if (source == "Reference") {
        m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0005::Reference);
      }
      else if (source == "Basemap") {
        m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0005::Basemap);
      }
      else if (source == "BundleSolution") {
        m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0005::BundleSolution);
      }
      else {
        QString msg = "Invalid AprioriXYZSource [" + source + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    if (pointObject.hasKeyword("AprioriRadiusSource")) {
      QString source = pointObject["AprioriRadiusSource"][0];

      if (source == "None") {
        m_pointData->set_aprioriradiussource(ControlPointFileEntryV0005::None);
      }
      else if (source == "User") {
        m_pointData->set_aprioriradiussource(ControlPointFileEntryV0005::User);
      }
      else if (source == "AverageOfMeasures") {
        m_pointData->set_aprioriradiussource(ControlPointFileEntryV0005::AverageOfMeasures);
      }
      else if (source == "Ellipsoid") {
        m_pointData->set_aprioriradiussource(ControlPointFileEntryV0005::Ellipsoid);
      }
      else if (source == "DEM") {
        m_pointData->set_aprioriradiussource(ControlPointFileEntryV0005::DEM);
      }
      else if (source == "BundleSolution") {
        m_pointData->set_aprioriradiussource(ControlPointFileEntryV0005::BundleSolution);
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
      ControlPointFileEntryV0005_Measure measure;

      // Copy strings, booleans, and doubles
      copy(group, "SerialNumberIndex",
           measure, &ControlPointFileEntryV0005_Measure::set_serialnumberindex);
      copy(group, "ChooserName",
           measure, &ControlPointFileEntryV0005_Measure::set_choosername);
      copy(group, "Sample",
           measure, &ControlPointFileEntryV0005_Measure::set_sample);
      copy(group, "Line",
           measure, &ControlPointFileEntryV0005_Measure::set_line);
      copy(group, "SampleResidual",
           measure, &ControlPointFileEntryV0005_Measure::set_sampleresidual);
      copy(group, "LineResidual",
           measure, &ControlPointFileEntryV0005_Measure::set_lineresidual);
      copy(group, "DateTime",
           measure, &ControlPointFileEntryV0005_Measure::set_datetime);
      copy(group, "Diameter",
           measure, &ControlPointFileEntryV0005_Measure::set_diameter);
      copy(group, "EditLock",
           measure, &ControlPointFileEntryV0005_Measure::set_editlock);
      copy(group, "Ignore",
           measure, &ControlPointFileEntryV0005_Measure::set_ignore);
      copy(group, "JigsawRejected",
           measure, &ControlPointFileEntryV0005_Measure::set_jigsawrejected);
      copy(group, "AprioriSample",
           measure, &ControlPointFileEntryV0005_Measure::set_apriorisample);
      copy(group, "AprioriLine",
           measure, &ControlPointFileEntryV0005_Measure::set_aprioriline);
      copy(group, "SampleSigma",
           measure, &ControlPointFileEntryV0005_Measure::set_samplesigma);
      copy(group, "LineSigma",
           measure, &ControlPointFileEntryV0005_Measure::set_linesigma);

      if (group.hasKeyword("Reference")) {
        if (group["Reference"][0].toLower() == "true") {
          m_pointData->set_referenceindex(groupIndex);
        }
        group.deleteKeyword("Reference");
      }

      QString type = group["MeasureType"][0].toLower();
      if (type == "candidate") {
        measure.set_type(ControlPointFileEntryV0005_Measure::Candidate);
      }
      else if (type == "manual") {
        measure.set_type(ControlPointFileEntryV0005_Measure::Manual);
      }
      else if (type == "registeredpixel") {
        measure.set_type(ControlPointFileEntryV0005_Measure::RegisteredPixel);
      }
      else if (type == "registeredsubpixel") {
        measure.set_type(ControlPointFileEntryV0005_Measure::RegisteredSubPixel);
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

          ControlPointFileEntryV0005_Measure_MeasureLogData protoBufDataEntry;

          protoBufDataEntry.set_doubledatatype(interpreter.GetDataType());
          protoBufDataEntry.set_doubledatavalue(interpreter.GetNumericalValue());

          *measure.add_log() = protoBufDataEntry;
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
   * Create a ControlPointV0005 object from a PvlControlPointV0003 object
   *
   * @param oldPoint The PvlControlPointV0003 that will be upgraded to V0003.
   * @param[in,out] serialNumbers The list of serial numbers for the cubes in the control net.
   *                              This is used to convert the serial number in the V0003 control
   *                              point into an index. If the serial number for a measure in the
   *                              control point is not in this list, it will be appended to it.
   */
  ControlPointV0005::ControlPointV0005(ControlPointV0003 &oldPoint,
                                       QList<QString> &serialNumbers)
   : m_pointData(new ControlPointFileEntryV0005) {
    ControlPointFileEntryV0002 oldPointData = oldPoint.pointData();
    if ( !oldPointData.IsInitialized() ) {
      QString msg = "version 2 control point is not fully initialized.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Copy over POD values
    if ( oldPointData.has_id() ) {
      m_pointData->set_id( oldPointData.id() );
    }
    if ( oldPointData.has_choosername() ) {
      m_pointData->set_choosername( oldPointData.choosername() );
    }
    if ( oldPointData.has_datetime() ) {
      m_pointData->set_datetime( oldPointData.datetime() );
    }
    if ( oldPointData.has_apriorisurfpointsourcefile() ) {
      m_pointData->set_apriorisurfpointsourcefile( oldPointData.apriorisurfpointsourcefile() );
    }
    if ( oldPointData.has_aprioriradiussourcefile() ) {
      m_pointData->set_aprioriradiussourcefile( oldPointData.aprioriradiussourcefile() );
    }
    if ( oldPointData.has_jigsawrejected() ) {
      m_pointData->set_jigsawrejected( oldPointData.jigsawrejected() );
    }
    if ( oldPointData.has_editlock() ) {
      m_pointData->set_editlock( oldPointData.editlock() );
    }
    if ( oldPointData.has_ignore() ) {
      m_pointData->set_ignore( oldPointData.ignore() );
    }
    if ( oldPointData.has_apriorix() ) {
      m_pointData->set_apriorix( oldPointData.apriorix() );
    }
    if ( oldPointData.has_aprioriy() ) {
      m_pointData->set_aprioriy( oldPointData.aprioriy() );
    }
    if ( oldPointData.has_aprioriz() ) {
      m_pointData->set_aprioriz( oldPointData.aprioriz() );
    }
    if ( oldPointData.has_adjustedx() ) {
      m_pointData->set_adjustedx( oldPointData.adjustedx() );
    }
    if ( oldPointData.has_adjustedy() ) {
      m_pointData->set_adjustedy( oldPointData.adjustedy() );
    }
    if ( oldPointData.has_adjustedz() ) {
      m_pointData->set_adjustedz( oldPointData.adjustedz() );
    }
    if ( oldPointData.has_latitudeconstrained() ) {
      m_pointData->set_latitudeconstrained( oldPointData.latitudeconstrained() );
    }
    if ( oldPointData.has_longitudeconstrained() ) {
      m_pointData->set_longitudeconstrained( oldPointData.longitudeconstrained() );
    }
    if ( oldPointData.has_radiusconstrained() ) {
      m_pointData->set_radiusconstrained( oldPointData.radiusconstrained() );
    }
    if ( oldPointData.has_referenceindex() ) {
      m_pointData->set_referenceindex( oldPointData.referenceindex() );
    }

    // Copy over enumerated values

    // The only point types in V0002 are ground and tie.
    // So, convert ground and tie to their V0003 values, fixed and free respectively.
    // Later check if the point is constrained.
    if ( oldPointData.has_type() ) {
      ControlPointFileEntryV0002_PointType pointType = oldPointData.type();
      switch (pointType) {
        case ControlPointFileEntryV0002_PointType_obsolete_Tie:
        case ControlPointFileEntryV0002_PointType_Free:
          m_pointData->set_type(ControlPointFileEntryV0005_PointType_Free);
          break;
        case ControlPointFileEntryV0002_PointType_Constrained:
          m_pointData->set_type(ControlPointFileEntryV0005_PointType_Constrained);
          break;
        case ControlPointFileEntryV0002_PointType_obsolete_Ground:
        case ControlPointFileEntryV0002_PointType_Fixed:
          m_pointData->set_type(ControlPointFileEntryV0005_PointType_Fixed);
          break;
        default:
          QString msg = "Unknown control point type.";
          throw IException(IException::User, msg, _FILEINFO_);
          break;
      }
    }

    if ( oldPointData.has_apriorisurfpointsource() ) {
      ControlPointFileEntryV0002_AprioriSource surfacePointSource;
      surfacePointSource = oldPointData.apriorisurfpointsource();
      switch (surfacePointSource) {
        case ControlPointFileEntryV0002_AprioriSource_None:
          m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0005_AprioriSource_None);
          break;
        case ControlPointFileEntryV0002_AprioriSource_User:
          m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0005_AprioriSource_User);
          break;
        case ControlPointFileEntryV0002_AprioriSource_AverageOfMeasures:
          m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0005_AprioriSource_AverageOfMeasures);
          break;
        case ControlPointFileEntryV0002_AprioriSource_Reference:
          m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0005_AprioriSource_Reference);
          break;
        case ControlPointFileEntryV0002_AprioriSource_Basemap:
          m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0005_AprioriSource_Basemap);
          break;
        case ControlPointFileEntryV0002_AprioriSource_BundleSolution:
          m_pointData->set_apriorisurfpointsource(ControlPointFileEntryV0005_AprioriSource_BundleSolution);
          break;
        default:
          QString msg = "Unknown apriori surface point source type.";
          throw IException(IException::User, msg, _FILEINFO_);
          break;
      }
    }

    if ( oldPointData.has_aprioriradiussource() ) {
      ControlPointFileEntryV0002_AprioriSource radiusSource;
      radiusSource = oldPointData.aprioriradiussource();
      switch (radiusSource) {
        case ControlPointFileEntryV0002_AprioriSource_None:
          m_pointData->set_aprioriradiussource(ControlPointFileEntryV0005_AprioriSource_None);
          break;
        case ControlPointFileEntryV0002_AprioriSource_User:
          m_pointData->set_aprioriradiussource(ControlPointFileEntryV0005_AprioriSource_User);
          break;
        case ControlPointFileEntryV0002_AprioriSource_AverageOfMeasures:
          m_pointData->set_aprioriradiussource(ControlPointFileEntryV0005_AprioriSource_AverageOfMeasures);
          break;
        case ControlPointFileEntryV0002_AprioriSource_Ellipsoid:
          m_pointData->set_aprioriradiussource(ControlPointFileEntryV0005_AprioriSource_Ellipsoid);
          break;
        case ControlPointFileEntryV0002_AprioriSource_DEM:
          m_pointData->set_aprioriradiussource(ControlPointFileEntryV0005_AprioriSource_DEM);
          break;
        case ControlPointFileEntryV0002_AprioriSource_BundleSolution:
          m_pointData->set_aprioriradiussource(ControlPointFileEntryV0005_AprioriSource_BundleSolution);
          break;
        default:
          QString msg = "Unknown apriori radius source type.";
          throw IException(IException::User, msg, _FILEINFO_);
          break;
      }
    }

    // Copy the array values

    // If there is a covariance matrix, then the point is constrained
    if (oldPointData.aprioricovar_size() > 0) {
      m_pointData->add_aprioricovar( oldPointData.aprioricovar(0) );
      m_pointData->add_aprioricovar( oldPointData.aprioricovar(1) );
      m_pointData->add_aprioricovar( oldPointData.aprioricovar(2) );
      m_pointData->add_aprioricovar( oldPointData.aprioricovar(3) );
      m_pointData->add_aprioricovar( oldPointData.aprioricovar(4) );
      m_pointData->add_aprioricovar( oldPointData.aprioricovar(5) );
    }

    if (oldPointData.adjustedcovar_size() > 0) {
      m_pointData->add_adjustedcovar( oldPointData.adjustedcovar(0) );
      m_pointData->add_adjustedcovar( oldPointData.adjustedcovar(1) );
      m_pointData->add_adjustedcovar( oldPointData.adjustedcovar(2) );
      m_pointData->add_adjustedcovar( oldPointData.adjustedcovar(3) );
      m_pointData->add_adjustedcovar( oldPointData.adjustedcovar(4) );
      m_pointData->add_adjustedcovar( oldPointData.adjustedcovar(5) );
    }

    // Copy the measures
    for (int i = 0; i < oldPointData.measures_size(); i++) {
      ControlPointFileEntryV0005_Measure *newMeasure = m_pointData->add_measures();
      ControlPointFileEntryV0002_Measure oldMeasure;
      oldMeasure = oldPointData.measures(i);

      // Convert the serial number to an index into the serial number list.
      // If the serial number is not in the list, append it.
      if ( oldMeasure.has_serialnumber() ) {
        // TODO this may be slow, QList::indexOf is a linear search through the list
        int serialNumberIndex = serialNumbers.indexOf(
                                    QString::fromStdString(oldMeasure.serialnumber()) );
        // If !serialNumbers.contains(oldMeasure.serialnumber()), then serialNumberIndex = -1
        if ( serialNumberIndex < 0 ) {
          serialNumbers.append( QString::fromStdString( oldMeasure.serialnumber() ) );
          serialNumberIndex = serialNumbers.size() - 1;
        }
        newMeasure->set_serialnumberindex(serialNumberIndex);
      }
      // Copy over POD values
      if ( oldMeasure.has_choosername() ) {
        newMeasure->set_choosername( oldMeasure.choosername() );
      }
      if ( oldMeasure.has_datetime() ) {
        newMeasure->set_datetime( oldMeasure.datetime() );
      }
      if ( oldMeasure.has_diameter() ) {
        newMeasure->set_diameter( oldMeasure.diameter() );
      }
      if ( oldMeasure.has_editlock() ) {
        newMeasure->set_editlock( oldMeasure.editlock() );
      }
      if ( oldMeasure.has_ignore() ) {
        newMeasure->set_ignore( oldMeasure.ignore() );
      }
      if ( oldMeasure.has_jigsawrejected() ) {
        newMeasure->set_jigsawrejected( oldMeasure.jigsawrejected() );
      }
      if ( oldMeasure.has_apriorisample() ) {
        newMeasure->set_apriorisample( oldMeasure.apriorisample() );
      }
      if ( oldMeasure.has_aprioriline() ) {
        newMeasure->set_aprioriline( oldMeasure.aprioriline() );
      }
      if ( oldMeasure.has_samplesigma() ) {
        newMeasure->set_samplesigma( oldMeasure.samplesigma() );
      }
      if ( oldMeasure.has_linesigma() ) {
        newMeasure->set_linesigma( oldMeasure.linesigma() );
      }
      if ( oldMeasure.has_sample() ) {
        newMeasure->set_sample( oldMeasure.sample() );
      }
      if ( oldMeasure.has_line() ) {
        newMeasure->set_line( oldMeasure.line() );
      }
      if ( oldMeasure.has_sampleresidual() ) {
        newMeasure->set_sampleresidual( oldMeasure.sampleresidual() );
      }
      if ( oldMeasure.has_lineresidual() ) {
        newMeasure->set_lineresidual( oldMeasure.lineresidual() );
      }

      // Copy over the enumerated values
      if ( oldMeasure.has_type() ) {
        ControlPointFileEntryV0002_Measure_MeasureType oldType;
        oldType = oldMeasure.type();
        switch (oldType) {
          case ControlPointFileEntryV0002_Measure_MeasureType_Candidate:
            newMeasure->set_type(ControlPointFileEntryV0005_Measure_MeasureType_Candidate);
            break;
          case ControlPointFileEntryV0002_Measure_MeasureType_Manual:
            newMeasure->set_type(ControlPointFileEntryV0005_Measure_MeasureType_Manual);
            break;
          case ControlPointFileEntryV0002_Measure_MeasureType_RegisteredPixel:
            newMeasure->set_type(ControlPointFileEntryV0005_Measure_MeasureType_RegisteredPixel);
            break;
          case ControlPointFileEntryV0002_Measure_MeasureType_RegisteredSubPixel:
            newMeasure->set_type(ControlPointFileEntryV0005_Measure_MeasureType_RegisteredSubPixel);
            break;
          default:
          QString msg = "Invalid measure type";
          throw IException(IException::User, msg, _FILEINFO_);
          break;
        }

        // Copy over any log data
        for (int j = 0; j < oldMeasure.log_size(); j++) {
          ControlPointFileEntryV0005_Measure_MeasureLogData newLogData;
          ControlPointFileEntryV0002_Measure_MeasureLogData oldLogData = oldMeasure.log(j);

          if ( oldLogData.has_doubledatatype() ) {
            newLogData.set_doubledatatype( oldLogData.doubledatatype() );
          }
          if ( oldLogData.has_doubledatavalue() ) {
            newLogData.set_doubledatavalue( oldLogData.doubledatavalue() );
          }
          if ( oldLogData.has_booldatatype() ) {
            newLogData.set_booldatatype( oldLogData.booldatatype() );
          }
          if ( oldLogData.has_booldatavalue() ) {
            newLogData.set_booldatavalue( oldLogData.booldatavalue() );
          }

          *newMeasure->add_log() = newLogData;
        }

        // Check that all the required fields in the measure are filled
        if ( !newMeasure->IsInitialized() ) {
          QString msg = "Measure file entry at index [" + toString(i)
                        + "] is missing required fields.";
          throw IException(IException::User, msg, _FILEINFO_);
        }
      }
    }

    // Check that all fo the required fields in the point are filled
    if ( !m_pointData->IsInitialized() ) {
      QString msg = "Control point file entry is missing required fields.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }


  /**
   * Access the protobuf control point data. If there is not internal point data then
   * default point data is returned. Note that default point data may be missing required
   * fields.
   *
   * @return @b const ControlPointFileEntryV0005& A constant reference to the internal control
   *                                              point data. There is no guarantee that the point
   *                                              data is fully initialized.
   */
  const ControlPointFileEntryV0005 &ControlPointV0005::pointData() {
    if (!m_pointData) {
      m_pointData.reset(new ControlPointFileEntryV0005);
    }

    return *m_pointData;
  }


  /**
   * This convenience method takes a boolean value from a PvlKeyword and copies it into a version 5
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control point that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param point[out] The version 5 protobuf representation of the control point that the value
   *                   will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0005::copy(PvlContainer &container,
                               QString keyName,
                               QSharedPointer<ControlPointFileEntryV0005> point,
                               void (ControlPointFileEntryV0005::*setter)(bool)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    container.deleteKeyword(keyName);
    value = value.toLower();

    if (value == "true" || value == "yes")
      (point.data()->*setter)(true);
  }


  /**
   * This convenience method takes a double value from a PvlKeyword and copies it into a version 5
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control point that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param point[out] The version 5 protobuf representation of the control point that the value
   *                   will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0005::copy(PvlContainer &container,
                               QString keyName,
                               QSharedPointer<ControlPointFileEntryV0005> point,
                               void (ControlPointFileEntryV0005::*setter)(double)) {

    if (!container.hasKeyword(keyName))
      return;

    double value = toDouble(container[keyName][0]);
    container.deleteKeyword(keyName);
    (point.data()->*setter)(value);
  }


  /**
   * This convenience method takes a string value from a PvlKeyword and copies it into a version 5
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control point that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param point[out] The version 5 protobuf representation of the control point that the value
   *                   will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0005::copy(PvlContainer &container,
                               QString keyName,
                               QSharedPointer<ControlPointFileEntryV0005> point,
                               void (ControlPointFileEntryV0005::*setter)(const std::string&)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    container.deleteKeyword(keyName);
    (point.data()->*setter)(value.toLatin1().data());
  }


  /**
   * This convenience method takes a boolean value from a PvlKeyword and copies it into a version 5
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control measure that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param[out] measure The version 5 protobuf representation of the control measure that the
   *                     value will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0005::copy(PvlContainer &container,
                               QString keyName,
                               ControlPointFileEntryV0005_Measure &measure,
                               void (ControlPointFileEntryV0005_Measure::*setter)(bool)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    container.deleteKeyword(keyName);
    value = value.toLower();

    if (value == "true" || value == "yes")
      (measure.*setter)(true);
  }


  /**
   * This convenience method takes an int value from a PvlKeyword and copies it into a version 5
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control measure that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param[out] measure The version 5 protobuf representation of the control measure that the
   *                     value will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0005::copy(PvlContainer &container,
                               QString keyName,
                               ControlPointFileEntryV0005_Measure &measure,
                               void (ControlPointFileEntryV0005_Measure::*setter)(int)) {

    if (!container.hasKeyword(keyName))
      return;

    int value = toInt(container[keyName][0]);
    container.deleteKeyword(keyName);
    (measure.*setter)(value);
  }


  /**
   * This convenience method takes a double value from a PvlKeyword and copies it into a version 5
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control measure that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param[out] measure The version 5 protobuf representation of the control measure that the
   *                     value will be copied into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0005::copy(PvlContainer &container,
                               QString keyName,
                               ControlPointFileEntryV0005_Measure &measure,
                               void (ControlPointFileEntryV0005_Measure::*setter)(double)) {

    if (!container.hasKeyword(keyName))
      return;

    double value = toDouble(container[keyName][0]);
    container.deleteKeyword(keyName);
    (measure.*setter)(value);
  }


  /**
   * This convenience method takes a string value from a PvlKeyword and copies it into a version 5
   * protobuf field.
   *
   * If the keyword doesn't exist, this does nothing.
   *
   * @param container The PvlContainer representation of the control measure that contains the
   *                  PvlKeyword.
   * @param keyName The name of the keyword to be copied.
   * @param[out] measure The version 5 protobuf representation of the control measure that the
   *                     value will be into.
   * @param setter The protobuf mutator method that sets the value of the field in the protobuf
   *               representation.
   */
  void ControlPointV0005::copy(PvlContainer &container,
                               QString keyName,
                               ControlPointFileEntryV0005_Measure &measure,
                               void (ControlPointFileEntryV0005_Measure::*setter)
                                      (const std::string &)) {

    if (!container.hasKeyword(keyName))
      return;

    QString value = container[keyName][0];
    container.deleteKeyword(keyName);
    (measure.*setter)(value.toLatin1().data());
  }
}
