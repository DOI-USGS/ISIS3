/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2009/09/01 17:53:05 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "ControlMeasure.h"

#include <QList>
#include <QStringList>

#include "Application.h"
#include "Camera.h"
#include "ControlMeasureLogData.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlCubeGraphNode.h"
#include "iString.h"
#include "iTime.h"
#include "ControlNetFileV0002.pb.h"
#include "SpecialPixel.h"

using namespace std;

namespace Isis {
  /**
   * Create a new control measure and initialize it to nulls and zeros.
   */
  ControlMeasure::ControlMeasure() {
    InitializeToNull();

    p_serialNumber = new iString;
    p_chooserName = new iString;
    p_dateTime = new iString;
    p_loggedData = new QVector<ControlMeasureLogData>();

    p_measureType = Candidate;
    p_editLock = false;
    p_jigsawRejected = false;
    p_ignore = false;

    p_sample = 0.0;
    p_line = 0.0;
  }


  /**
   * Converts the protocol buffer version of the measure into a real
   *   ControlMeasure
   *
   * @param other The control measure to copy all of the values from
   */
  ControlMeasure::ControlMeasure(
      const ControlPointFileEntryV0002_Measure &protoBuf) {
    InitializeToNull();

    p_serialNumber = new iString(protoBuf.serialnumber());
    p_chooserName = new iString(protoBuf.choosername());
    p_dateTime = new iString(protoBuf.datetime());
    p_loggedData = new QVector<ControlMeasureLogData>();

    switch (protoBuf.type()) {
      case ControlPointFileEntryV0002_Measure::Candidate:
        p_measureType = ControlMeasure::Candidate;
        break;
      case ControlPointFileEntryV0002_Measure::Manual:
        p_measureType = ControlMeasure::Manual;
        break;
      case ControlPointFileEntryV0002_Measure::RegisteredPixel:
        p_measureType = ControlMeasure::RegisteredPixel;
        break;
      case ControlPointFileEntryV0002_Measure::RegisteredSubPixel:
        p_measureType = ControlMeasure::RegisteredSubPixel;
        break;
    }

    p_editLock = protoBuf.editlock();
    p_jigsawRejected = protoBuf.jigsawrejected();
    p_ignore = protoBuf.ignore();
    p_sample = protoBuf.sample();
    p_line = protoBuf.line();
//    ground = protoBuf.???

    if (protoBuf.has_diameter())
      p_diameter = protoBuf.diameter();

    if (protoBuf.has_apriorisample())
      p_aprioriSample = protoBuf.apriorisample();

    if (protoBuf.has_aprioriline())
      p_aprioriLine = protoBuf.aprioriline();

    if (protoBuf.has_samplesigma())
      p_sampleSigma = protoBuf.samplesigma();

    if (protoBuf.has_linesigma())
      p_lineSigma = protoBuf.linesigma();

    if (protoBuf.has_sampleresidual())
      p_sampleResidual = protoBuf.sampleresidual();

    if (protoBuf.has_lineresidual())
      p_lineResidual = protoBuf.lineresidual();

    for (int dataEntry = 0;
        dataEntry < protoBuf.log_size();
        dataEntry ++) {
      ControlMeasureLogData logEntry(protoBuf.log(dataEntry));
      p_loggedData->push_back(logEntry);
    }
  }


  /**
   * Copy the other control measure exactly.
   *
   * @param other The control measure to copy all of the values from
   */
  ControlMeasure::ControlMeasure(const ControlMeasure &other) {
    InitializeToNull();

    p_serialNumber = new iString(*other.p_serialNumber);
    p_chooserName = new iString(*other.p_chooserName);
    p_dateTime = new iString(*other.p_dateTime);

    p_loggedData = new QVector<ControlMeasureLogData>(*other.p_loggedData);

    p_measureType = other.p_measureType;
    p_editLock = other.p_editLock;
    p_jigsawRejected = other.p_jigsawRejected;
    p_ignore = other.p_ignore;
    p_sample = other.p_sample;
    p_line = other.p_line;
    p_diameter = other.p_diameter;
    p_aprioriSample = other.p_aprioriSample;
    p_aprioriLine = other.p_aprioriLine;
    p_sampleSigma = other.p_sampleSigma;
    p_lineSigma = other.p_lineSigma;
    p_sampleResidual = other.p_sampleResidual;
    p_lineResidual = other.p_lineResidual;
    p_camera = other.p_camera;
    associatedCSN = other.associatedCSN;
  }


  //! initialize pointers and other data to NULL
  void ControlMeasure::InitializeToNull() {
    p_serialNumber = NULL;
    p_chooserName = NULL;
    p_dateTime = NULL;
    p_loggedData = NULL;

    p_diameter = Null;
    p_aprioriSample = Null;
    p_aprioriLine = Null;
    p_sampleSigma = Null;
    p_lineSigma = Null;
    p_sampleResidual = Null;
    p_lineResidual = Null;

    p_camera = NULL;
    p_focalPlaneMeasuredX = Null;
    p_focalPlaneMeasuredY = Null;
    p_focalPlaneComputedX = Null;
    p_focalPlaneComputedY = Null;

    parentPoint = NULL;
    associatedCSN = NULL;
  }


  /**
   * Free the memory allocated by a control
   */
  ControlMeasure::~ControlMeasure() {
    if (p_serialNumber) {
      delete p_serialNumber;
      p_serialNumber = NULL;
    }

    if (p_chooserName) {
      delete p_chooserName;
      p_chooserName = NULL;
    }

    if (p_dateTime) {
      delete p_dateTime;
      p_dateTime = NULL;
    }

    if (p_loggedData) {
      delete p_loggedData;
      p_loggedData = NULL;
    }

    associatedCSN = NULL;
  }


  ControlMeasure::Status ControlMeasure::SetAprioriLine(double aprioriLine) {
    if (IsEditLocked())
      return MeasureLocked;
    MeasureModified();
    p_aprioriLine = aprioriLine;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetAprioriSample(
    double aprioriSample) {
    if (IsEditLocked())
      return MeasureLocked;
    MeasureModified();
    p_aprioriSample = aprioriSample;
    return Success;
  }


  /**
   * @brief Set pointer to camera associated with a measure
   *
   * This method is used to set a pointer to the camera associated
   * with a ControlMeasure.
   *
   * @param *camera  Pointer to camera
   *           
   * @return Status Success
   *
   * @internal
   *   @history 2011-07-01 Debbie A. Cook  Removed editLock check
   *
   */
  ControlMeasure::Status ControlMeasure::SetCamera(Isis::Camera *camera) {
    p_camera = camera;
    return Success;
  }


  /**
   * @brief Set cube serial number
   *
   * This method is used to set the serial number of the cube.  That is,
   * the coordinate was selected from a cube with this unique serial
   * number
   *
   * @param sn  Serial number of the cube where the coordinate was
   *            selected
   * @return Status Success or MeasureLocked
   *
   */
  ControlMeasure::Status ControlMeasure::SetCubeSerialNumber(iString newSerialNumber) {
    if (IsEditLocked())
      return MeasureLocked;
    *p_serialNumber = newSerialNumber;
    return Success;
  }


  //! Set chooser name to a user who last changed the coordinate
  ControlMeasure::Status ControlMeasure::SetChooserName() {
    if (IsEditLocked())
      return MeasureLocked;
    *p_chooserName = "";
    return Success;
  }


  //! Set the chooser name to an application that last changed the coordinate
  ControlMeasure::Status ControlMeasure::SetChooserName(iString name) {
    if (IsEditLocked())
      return MeasureLocked;
    *p_chooserName = name;
    return Success;
  }


  /**
   * @brief Set the coordinate of the measurement
   *
   * @param sample  Sample coordinate of the measurement
   * @param line    Line coordinate of the measurement,
   */
  ControlMeasure::Status ControlMeasure::SetCoordinate(double sample,
      double line) {
    return SetCoordinate(sample, line, GetType());
  }


  /**
    * @brief Set the coordinate of the measurement
    *
    * @param sample  Sample coordinate of the measurement
    * @param line    Line coordinate of the measurement
    * @param type    The type of the coordinate
    */
  ControlMeasure::Status ControlMeasure::SetCoordinate(double sample,
      double line, MeasureType type) {
    if (IsEditLocked())
      return MeasureLocked;
    MeasureModified();
    p_sample = sample;
    p_line = line;
    SetType(type);
    return Success;
  }


  //! Date Time - Creation Time
  ControlMeasure::Status ControlMeasure::SetDateTime() {
    if (IsEditLocked())
      return MeasureLocked;
    *p_dateTime = Application::DateTime();
    return Success;
  }


  //! Set date/time the coordinate was last changed to specified date/time
  ControlMeasure::Status ControlMeasure::SetDateTime(iString datetime) {
    if (IsEditLocked())
      return MeasureLocked;
    *p_dateTime = datetime;
    return Success;
  }


  /**
   * @brief Set the crater diameter at the coordinate
   *
   * This method sets the crater diameter at the coordinate.  If
   * left unset a diameter of 0 is assumed which implies no crater
   *
   * @param diameter  The diameter of the crater in pixels
   */
  ControlMeasure::Status ControlMeasure::SetDiameter(double diameter) {
    if (IsEditLocked())
      return MeasureLocked;
    MeasureModified();
    p_diameter = diameter;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetEditLock(bool editLock) {
    p_editLock = editLock;
    return Success;
  }


  /**
   * Set the focal plane x/y for the measured line/sample.  This is a convenience
   * method for the BundleAdjustment class to avoid having to go redo the calculation.
   *
   * @param *camera  Pointer to camera
   *           
   * @return Status Success
   *
   * @internal
   *   @history 2011-07-09 Debbie A. Cook  Removed editLock check for jigsaw
   *
   */
  ControlMeasure::Status ControlMeasure::SetFocalPlaneMeasured(double x,
      double y) {
    p_focalPlaneMeasuredX = x;
    p_focalPlaneMeasuredY = y;
    return Success;
  }


  /**
   * Set the computed focal plane x/y for the apriori lat/lon.  This is a convenience
   * method for the BundleAdjustment class to avoid having to go redo the calculation.
   *
   * @param *camera  Pointer to camera
   *           
   * @return Status Success
   *
   * @internal
   *   @history 2011-07-14 Debbie A. Cook  Removed editLock check for jigsaw
   *
   */
  ControlMeasure::Status ControlMeasure::SetFocalPlaneComputed(double x,
      double y) {
    p_focalPlaneComputedX = x;
    p_focalPlaneComputedY = y;
    return Success;
  }




  /**
   * @brief Set "jigsaw" rejected flag for a measure
   *
   * This method is used to set the "jigsaw"-rejected flag for
   * the current measure.  It should only be used by jigsaw.
   *
   * @param *reject  rejected flag
   *           
   * @return Status Success
   *
   * @internal
   *   @history 2011-07-01 Debbie A. Cook  Removed editLock check
   *
   */
  ControlMeasure::Status ControlMeasure::SetRejected(bool reject) {
    MeasureModified();
    p_jigsawRejected = reject;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetIgnored(bool newIgnoreStatus) {
    if (IsEditLocked())
      return MeasureLocked;

    bool oldStatus = p_ignore;
    p_ignore = newIgnoreStatus;

    // only update if there was a change in status
    if (oldStatus != p_ignore) {
      MeasureModified();
      if (parentPoint && !parentPoint->IsIgnored() && parentPoint->Parent()) {
        ControlNet * cnet = parentPoint->Parent();
        p_ignore ? cnet->measureIgnored(this) : cnet->measureUnIgnored(this);
        cnet->emitNetworkStructureModified();
      }
    }

    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetLineSigma(double lineSigma) {
    if (IsEditLocked())
      return MeasureLocked;
    MeasureModified();
    p_lineSigma = lineSigma;
    return Success;
  }


  /**
   * Set the BundleAdjust Residual of the coordinate.
   *   ***Warning:  This method should only be used by BundleAdjust
   *                and its applications.
   * 
   * @param sampResidual  Sample Residual
   * @param lineResidual  Line Residual
   * 
   * @internal
   *   @history 2011-07-01 Debbie A. Cook  Removed editLock check to
   *                         allow the residuals of locked points
   *                         to be reported.
   */
  ControlMeasure::Status ControlMeasure::SetResidual(double sampResidual,
      double lineResidual) {
    MeasureModified();
    p_sampleResidual = sampResidual;
    p_lineResidual   = lineResidual;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetSampleSigma(double sampleSigma) {
    if (IsEditLocked())
      return MeasureLocked;
    MeasureModified();
    p_sampleSigma = sampleSigma;
    return Success;
  }


  //! Set how the coordinate was obtained
  ControlMeasure::Status ControlMeasure::SetType(MeasureType type) {
    if (IsEditLocked())
      return MeasureLocked;
    MeasureModified();
    p_measureType = type;
    return Success;
  }


  /**
   * This adds or updates the log data information associated with data's type.
   *
   * In most cases, this is what you want to use to assign log data.
   */
  void ControlMeasure::SetLogData(ControlMeasureLogData data) {
    if (!data.IsValid()) {
      iString msg = "Cannot set log data with invalid information stored in "
          "the ControlMeasureLogData";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if (HasLogData(data.GetDataType()))
      UpdateLogData(data);
    else
      p_loggedData->append(data);
  }


  /**
   * This deletes log data of the specified type. If none exist, this does
   *   nothing.
   *
   * @param dataType A ControlMeasureLogData::NumericLogDataType
   */
  void ControlMeasure::DeleteLogData(long dataType) {
    for (int i = p_loggedData->size()-1; i >= 0; i--) {
      ControlMeasureLogData logDataEntry = p_loggedData->at(i);

      if (logDataEntry.GetDataType() == dataType)
        p_loggedData->remove(i);
    }
  }


  /**
   * Get the value of the log data with the specified type as a variant. This
   *   should work for all types of log data.
   */
  QVariant ControlMeasure::GetLogValue(long dataType) const {
    for (int i = 0; i < p_loggedData->size(); i++) {
      const ControlMeasureLogData &logDataEntry = p_loggedData->at(i);

      if (logDataEntry.GetDataType() == dataType)
        return logDataEntry.GetValue();
    }

    return QVariant();
  }


  /**
   * Test if we have a valid log data value of the specified type
   *
   * @param dataType A ControlMeasureLogData::NumericLogDataType
   */
  bool ControlMeasure::HasLogData(long dataType) const {
    for (int i = 0; i < p_loggedData->size(); i++) {
      const ControlMeasureLogData &logDataEntry = p_loggedData->at(i);

      if (logDataEntry.GetDataType() == dataType)
        return true;
    }

    return false;
  }


  /**
   * This updates existing log data information associated with data's type. If
   *   none exist, an error is thrown.
   *
   * @see SetLogData
   */
  void ControlMeasure::UpdateLogData(ControlMeasureLogData newLogData) {
    bool updated = false;

    for (int i = 0; i < p_loggedData->size(); i++) {
      ControlMeasureLogData logDataEntry = p_loggedData->at(i);

      if (logDataEntry.GetDataType() == newLogData.GetDataType()) {
        (*p_loggedData)[i] = newLogData;
        updated = true;
      }
    }

    if (!updated) {
      iString msg = "Unable to update the log data for [" +
          newLogData.DataTypeToName(newLogData.GetDataType()) + "] because this"
          " control measure does not have log data for this value. Please use "
          "SetLogData instead";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  double ControlMeasure::GetAprioriLine() const {
    return p_aprioriLine;
  }


  double ControlMeasure::GetAprioriSample() const {
    return p_aprioriSample;
  }


  Isis::Camera *ControlMeasure::Camera() const {
    return p_camera;
  }


  //! Return the chooser name
  iString ControlMeasure::GetChooserName() const {
    if (*p_chooserName != "") {
      return *p_chooserName;
    }
    else {
      return Filename(Application::Name()).Name();
    }
  }


  //! Return the serial number of the cube containing the coordinate
  iString ControlMeasure::GetCubeSerialNumber() const {
    return *p_serialNumber;
  }


  //! Return the date/time the coordinate was last changed
  iString ControlMeasure::GetDateTime() const {
    if (*p_dateTime != "") {
      return *p_dateTime;
    }
    else {
      return Application::DateTime();
    }
  }


  //! Return the diameter of the crater in pixels (0 implies no crater)
  double ControlMeasure::GetDiameter() const {
    return p_diameter;
  }


  /**
   * @brief Return value for p_editLock or implicit lock on reference measure
   *
   * This method returns p_editLock unless the measure is a reference measure.
   * In the case of a reference measure the value of the parent point's
   * editLock is returned.  An editLock on a control point implicitly locks
   * the points reference measure as well.
   *
   * @return value of p_editLock
   *
   * @internal
   *   @history 2011-07-05 Debbie A. Cook  Added check for implicit lock on
   *                        the reference measure of the parent's reference
   *                        measure.
   *
   */
  bool ControlMeasure::IsEditLocked() const {
    // Check to see if this measure is the reference measure of the parent
    if (parentPoint != NULL  &&  parentPoint->IsEditLocked()  &&
        this == parentPoint->GetRefMeasure())
      return true;
    return p_editLock;
  }


  double ControlMeasure::GetFocalPlaneComputedX() const {
    return p_focalPlaneComputedX;
  }


  double ControlMeasure::GetFocalPlaneComputedY() const {
    return p_focalPlaneComputedY;
  }


  double ControlMeasure::GetFocalPlaneMeasuredX() const {
    return p_focalPlaneMeasuredX;
  }


  double ControlMeasure::GetFocalPlaneMeasuredY() const {
    return p_focalPlaneMeasuredY;
  }


  bool ControlMeasure::IsIgnored() const {
    return p_ignore;
  }


  bool ControlMeasure::IsRejected() const {
    return p_jigsawRejected;
  }


  bool ControlMeasure::IsMeasured() const {
    return p_measureType != Candidate;
  }


  bool ControlMeasure::IsRegistered() const {
    return (p_measureType == RegisteredPixel ||
        p_measureType == RegisteredSubPixel);
  }

  bool ControlMeasure::IsStatisticallyRelevant(DataField field) const {
    bool relevant = false;
    bool validField = false;

    switch (field) {
      case AprioriLine:
      case AprioriSample:
      case ChooserName:
      case CubeSerialNumber:
      case Coordinate:
      case Diameter:
      case FocalPlaneMeasured:
      case FocalPlaneComputed:
      case SampleResidual:
      case LineResidual:
      case SampleSigma:
      case LineSigma:
        relevant = true;
        validField = true;
        break;

      case DateTime:
      case EditLock:
      case Ignore:
      case Rejected:
      case Type:
        validField = true;
        break;
    }

    if (!validField) {
      iString msg = "Cannot test IsStatisticallyRelevant on Measure Data ["
          + iString(field) + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return relevant;
  }


  double ControlMeasure::GetLine() const {
    return p_line;
  }


  double ControlMeasure::GetLineResidual() const {
    return p_lineResidual;
  }


  double ControlMeasure::GetLineSigma() const {
    return p_lineSigma;
  }


  //! Return Residual magnitude
  double ControlMeasure::GetResidualMagnitude() const {
    if(IsSpecial(p_lineResidual) || IsSpecial(p_sampleResidual))
      return Null;

    double dist = (p_lineResidual * p_lineResidual) +
                  (p_sampleResidual * p_sampleResidual);

    return sqrt(dist);
  }


  double ControlMeasure::GetSample() const {
    return p_sample;
  }


  double ControlMeasure::GetSampleResidual() const {
    return p_sampleResidual;
  }


  double ControlMeasure::GetSampleSigma() const {
    return p_sampleSigma;
  }


  ControlMeasure::MeasureType ControlMeasure::GetType() const {
    return p_measureType;
  }


  QString ControlMeasure::GetPointId() const {
    if (parentPoint == NULL) {
      iString msg = "Measure has no containing point";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    return parentPoint->GetId();
  }


  double ControlMeasure::GetSampleShift() const {
    return p_sample - p_aprioriSample;
  }


  double ControlMeasure::GetLineShift() const {
    return p_line - p_aprioriLine;
  }


  double ControlMeasure::GetPixelShift() const {
    return sqrt(pow(GetSampleShift(), 2) + pow(GetLineShift(), 2));
  }


  ControlMeasureLogData ControlMeasure::GetLogData(long dataType) const {
    int foundIndex = 0;
    ControlMeasureLogData::NumericLogDataType typedDataType =
      (ControlMeasureLogData::NumericLogDataType)dataType;

    while (foundIndex < p_loggedData->size()) {
      const ControlMeasureLogData &logData = p_loggedData->at(foundIndex);
      if (logData.GetDataType() == typedDataType) {
        return logData;
      }

      foundIndex ++;
    }

    return ControlMeasureLogData(typedDataType);
  }


  //! One Getter to rule them all
  double ControlMeasure::GetMeasureData(iString data) const {
    if (data == "SampleResidual")
      return p_sampleResidual;
    else if (data == "LineResidual")
      return p_lineResidual;
    else if (data == "Type")
      return p_measureType;
    else if (data == "IsMeasured")
      return IsMeasured();
    else if (data == "IsRegistered")
      return IsRegistered();
    else if (data == "Ignore")
      return p_ignore;
    else {
      iString msg = data + " passed to GetMeasureData but is invalid";
      throw Isis::iException::Message(Isis::iException::Programmer, msg,
          _FILEINFO_);
    }
  }


  //! Returns a list of all valid options to pass to GetMeasureData
  QVector< iString > ControlMeasure::GetMeasureDataNames() {
    QVector< iString > names;

    names.push_back("SampleResidual");
    names.push_back("LineResidual");
    names.push_back("Type");
    names.push_back("IsMeasured");
    names.push_back("IsRegistered");
    names.push_back("Ignore");

    return names;
  }

  /**
   * Data accessor method, provides access to string representations of all
   * variable values and names.
   *
   * @return A QList containing QStringLists, the QStringLists contain name
   *         value pairs such that element 0 is the name and element 1 is
   *         the value of the variable.
   */
  QList< QStringList > ControlMeasure::PrintableClassData() const {
    QList< QStringList > data;
    QStringList qsl;

    qsl << "AprioriLine" << QString::number(p_aprioriLine);
    data.append(qsl);
    qsl.clear();

    qsl << "AprioriSample" << QString::number(p_aprioriSample);
    data.append(qsl);
    qsl.clear();

    qsl << "ChooserName" << *p_chooserName;
    data.append(qsl);
    qsl.clear();

    qsl << "CubeSerialNumber" << *p_serialNumber;
    data.append(qsl);
    qsl.clear();

    qsl << "DateTime" << *p_dateTime;
    data.append(qsl);
    qsl.clear();

    qsl << "Line" << QString::number(p_line);
    data.append(qsl);
    qsl.clear();

    qsl << "LineResidual" << QString::number(p_lineResidual);
    data.append(qsl);
    qsl.clear();

    qsl << "LineSigma" << QString::number(p_lineSigma);
    data.append(qsl);
    qsl.clear();

    qsl << "Sample" << QString::number(p_sample);
    data.append(qsl);
    qsl.clear();

    qsl << "SampleResidual" << QString::number(p_sampleResidual);
    data.append(qsl);
    qsl.clear();

    qsl << "SampleSigma" << QString::number(p_sampleSigma);
    data.append(qsl);
    qsl.clear();

    qsl << "ResidualMagnitude" << QString::number(GetResidualMagnitude());
    data.append(qsl);
    qsl.clear();

    qsl << "MeasureType" << GetMeasureTypeString();
    data.append(qsl);
    qsl.clear();

    return data;
  }


  /**
   * @param str The string to get a MeasureType from
   *
   * @returns A Measure Type given a string
   */
  ControlMeasure::MeasureType ControlMeasure::StringToMeasureType(QString str) {

    iString err = "String [";
    err += iString(str) + "] can not be converted to a MeasureType";

    str = str.toLower();
    MeasureType measureType;
    if (str == "candidate")
      measureType = ControlMeasure::Candidate;
    else
      if (str == "manual")
        measureType = ControlMeasure::Manual;
      else
        if (str == "registeredpixel")
          measureType = ControlMeasure::RegisteredPixel;
        else
          if (str == "registeredsubpixel")
            measureType = ControlMeasure::RegisteredSubPixel;
          else
            throw iException::Message(iException::Programmer, err, _FILEINFO_);

    return measureType;
  }


  /**
   * Return the String Control Measure type
   *
   * @return string - Measure Type
   *
   * @author Sharmila Prasad (10/1/2010)
   * @internal
   *   @history 2010-10-28 Mackenzie Boyd - Changed name and made static,
   *                                        added exception.
   *   @history 2010-12-08 Tracie Sucharski - Added measure type of Ground.
   */
  iString ControlMeasure::MeasureTypeToString(MeasureType type) {
    iString sPrintable;

    switch (type) {
      case ControlMeasure::Candidate:
        sPrintable = "Candidate";
        break;

      case ControlMeasure::Manual:
        sPrintable = "Manual";
        break;

      case ControlMeasure::RegisteredPixel:
        sPrintable = "RegisteredPixel";
        break;

      case ControlMeasure::RegisteredSubPixel:
        sPrintable = "RegisteredSubPixel";
        break;
    }

    if (sPrintable == "") {
      iString msg = "Measure type [" + iString(type) + "] cannot be converted "
          "to a string";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return sPrintable;
  }

  /**
   * Obtain a string representation of the MeasureType
   *
   * @return A string representation of the MeasureType
   */
  iString ControlMeasure::GetMeasureTypeString() const {
    return MeasureTypeToString(p_measureType);
  }

  //! Assignment operator
  const ControlMeasure &ControlMeasure::operator=(const ControlMeasure &other) {
    if (this == &other)
      return *this;

    if (p_serialNumber) {
      delete p_serialNumber;
      p_serialNumber = NULL;
    }
    if (p_chooserName) {
      delete p_chooserName;
      p_chooserName = NULL;
    }
    if (p_dateTime) {
      delete p_dateTime;
      p_dateTime = NULL;
    }
    if (p_loggedData) {
      delete p_loggedData;
      p_loggedData = NULL;
    }

    parentPoint = NULL;

    p_serialNumber = new iString;
    p_chooserName = new iString;
    p_dateTime = new iString;
    p_loggedData = new QVector<ControlMeasureLogData>();

    *p_serialNumber = *other.p_serialNumber;
    *p_chooserName = *other.p_chooserName;
    *p_dateTime = *other.p_dateTime;
    *p_loggedData = *other.p_loggedData;

    p_measureType = other.p_measureType;
    p_editLock = other.p_editLock;
    p_ignore = other.p_ignore;
    p_sample = other.p_sample;
    p_line = other.p_line;
    p_diameter = other.p_diameter;
    p_aprioriSample = other.p_aprioriSample;
    p_aprioriLine = other.p_aprioriLine;
    p_sampleSigma = other.p_sampleSigma;
    p_lineSigma = other.p_lineSigma;
    p_sampleResidual = other.p_sampleResidual;
    p_lineResidual = other.p_lineResidual;
    p_camera = other.p_camera;
    p_focalPlaneMeasuredX = other.p_focalPlaneMeasuredX;
    p_focalPlaneMeasuredY = other.p_focalPlaneMeasuredY;
    p_focalPlaneComputedX = other.p_focalPlaneComputedX;
    p_focalPlaneComputedY = other.p_focalPlaneComputedY;
    associatedCSN = other.associatedCSN;

    return *this;
  }


  /**
   * Compare 2 Control Measures for inequality
   *
   * @author sprasad (4/20/2010)
   *
   * @param pMeasure
   *
   * @return bool
   */
  bool ControlMeasure::operator!=(const Isis::ControlMeasure &pMeasure) const {
    return !(*this == pMeasure);
  }


  /**
   * Check for Control Measures equality
   *
   * @author sprasad (4/20/2010)
   *
   * history 2010-06-24 Tracie Sucharski, Added new keywords
   *
   * @param pMeasure - Control Measure to be compared against
   *
   * @return bool
   */
  bool ControlMeasure::operator==(const Isis::ControlMeasure &pMeasure) const {
    return pMeasure.p_measureType == p_measureType &&
        *pMeasure.p_serialNumber == *p_serialNumber &&
        pMeasure.p_chooserName == p_chooserName &&
        pMeasure.p_dateTime == p_dateTime &&
        pMeasure.p_editLock == p_editLock &&
        pMeasure.p_ignore == p_ignore &&
        pMeasure.p_sample == p_sample &&
        pMeasure.p_line == p_line &&
        pMeasure.p_diameter == p_diameter &&
        pMeasure.p_aprioriSample == p_aprioriSample &&
        pMeasure.p_aprioriLine == p_aprioriLine &&
        pMeasure.p_sampleSigma ==  p_sampleSigma &&
        pMeasure.p_lineSigma ==  p_lineSigma &&
        pMeasure.p_sampleResidual == p_sampleResidual &&
        pMeasure.p_lineResidual == p_lineResidual &&
        pMeasure.p_focalPlaneMeasuredX == p_focalPlaneMeasuredX &&
        pMeasure.p_focalPlaneMeasuredY == p_focalPlaneMeasuredY &&
        pMeasure.p_focalPlaneComputedX == p_focalPlaneComputedX &&
        pMeasure.p_focalPlaneComputedY == p_focalPlaneComputedY;
  }


  ControlPointFileEntryV0002_Measure ControlMeasure::ToProtocolBuffer() const {
    ControlPointFileEntryV0002_Measure protoBufMeasure;

    protoBufMeasure.set_serialnumber(GetCubeSerialNumber());
    switch (GetType()) {
      case ControlMeasure::Candidate:
        protoBufMeasure.set_type(ControlPointFileEntryV0002_Measure::Candidate);
        break;
      case ControlMeasure::Manual:
        protoBufMeasure.set_type(ControlPointFileEntryV0002_Measure::Manual);
        break;
      case ControlMeasure::RegisteredPixel:
        protoBufMeasure.set_type(ControlPointFileEntryV0002_Measure::RegisteredPixel);
        break;
      case ControlMeasure::RegisteredSubPixel:
        protoBufMeasure.set_type(ControlPointFileEntryV0002_Measure::RegisteredSubPixel);
        break;
    }

    if (GetChooserName() != "") {
      protoBufMeasure.set_choosername(GetChooserName());
    }
    if (GetDateTime() != "") {
      protoBufMeasure.set_datetime(GetDateTime());
    }
    if (IsEditLocked())
      protoBufMeasure.set_editlock(true);

    if (IsIgnored())
      protoBufMeasure.set_ignore(true);

    if (IsRejected())
      protoBufMeasure.set_jigsawrejected(true);

    if (GetSample() != 0.)
      protoBufMeasure.set_sample(GetSample());

    if(GetLine() != 0.)
      protoBufMeasure.set_line(GetLine());

    if (GetSampleResidual() != Isis::Null)
      protoBufMeasure.set_sampleresidual(GetSampleResidual());

    if (GetLineResidual() != Isis::Null)
      protoBufMeasure.set_lineresidual(GetLineResidual());

    if (GetDiameter() != Isis::Null)
      protoBufMeasure.set_diameter(GetDiameter());

    if (GetAprioriSample() != Isis::Null)
      protoBufMeasure.set_apriorisample(GetAprioriSample());

    if (GetAprioriLine() != Isis::Null)
      protoBufMeasure.set_aprioriline(GetAprioriLine());

    if (GetSampleSigma() != Isis::Null)
      protoBufMeasure.set_samplesigma(GetSampleSigma());

    if (GetLineSigma() != Isis::Null)
      protoBufMeasure.set_linesigma(GetLineSigma());

    ControlMeasureLogData logEntry;
    foreach(logEntry, *p_loggedData) {
      *protoBufMeasure.add_log() = logEntry.ToProtocolBuffer();
    }

    return protoBufMeasure;
  }


  void ControlMeasure::MeasureModified() {
    *p_dateTime = "";
    *p_chooserName = "";
  }
}
