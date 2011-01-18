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
#include "ControlPoint.h"
#include "ControlSerialNumber.h"
#include "iString.h"
#include "iTime.h"
#include "PBControlNetIO.pb.h"
#include "PBControlNetLogData.pb.h"
#include "SpecialPixel.h"


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
    const PBControlNet_PBControlPoint_PBControlMeasure &protoBuf) {
    Init(protoBuf);
  }


  /**
   * Converts the protocol buffer version of the measure into a real
   *   ControlMeasure
   *
   * @param other The control measure to copy all of the values from
   */
  ControlMeasure::ControlMeasure(
    const PBControlNet_PBControlPoint_PBControlMeasure &protoBuf,
    const PBControlNetLogData_Point_Measure &logData) {
    Init(protoBuf);

    for (int dataEntry = 0;
         dataEntry < logData.loggedmeasuredata_size();
         dataEntry ++) {
      ControlMeasureLogData logEntry(logData.loggedmeasuredata(dataEntry));
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
    parentPoint = other.parentPoint;
    associatedSN = other.associatedSN;
  }


  void ControlMeasure::Init(
    const PBControlNet_PBControlPoint_PBControlMeasure &protoBuf) {
    InitializeToNull();

    p_serialNumber = new iString(protoBuf.serialnumber());
    p_chooserName = new iString(protoBuf.choosername());
    p_dateTime = new iString(protoBuf.datetime());
    p_loggedData = new QVector<ControlMeasureLogData>();

    switch (protoBuf.type()) {
      case PBControlNet_PBControlPoint_PBControlMeasure::Reference:
        p_measureType = ControlMeasure::Reference;
        break;
      case PBControlNet_PBControlPoint_PBControlMeasure::Candidate:
        p_measureType = ControlMeasure::Candidate;
        break;
      case PBControlNet_PBControlPoint_PBControlMeasure::Manual:
        p_measureType = ControlMeasure::Manual;
        break;
      case PBControlNet_PBControlPoint_PBControlMeasure::RegisteredPixel:
        p_measureType = ControlMeasure::RegisteredPixel;
        break;
      case PBControlNet_PBControlPoint_PBControlMeasure::RegisteredSubPixel:
        p_measureType = ControlMeasure::RegisteredSubPixel;
        break;
      case PBControlNet_PBControlPoint_PBControlMeasure::Ground:
        p_measureType = ControlMeasure::RegisteredSubPixel;
        break;
    }

    p_editLock = protoBuf.editlock();
    p_jigsawRejected = protoBuf.jigsawrejected();
    p_ignore = protoBuf.ignore();
    p_sample = protoBuf.measurement().sample();
    p_line = protoBuf.measurement().line();

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

    if (protoBuf.measurement().has_sampleresidual())
      p_sampleResidual = protoBuf.measurement().sampleresidual();

    if (protoBuf.measurement().has_lineresidual())
      p_lineResidual = protoBuf.measurement().lineresidual();
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
    associatedSN = NULL;
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

    if (associatedSN) {
      associatedSN->RemoveMeasure(GetPointId());
    }
  }


  /**
    * Convert from PvlGroup to ControlMeasure. This requires the following
    *   keywords in the PvlGroup:
    *     SerialNumber = one element, any value
    *     MeasureType = one of (Reference,Candidate,Manual,RegisteredPixel,
    *                           RegisteredSubPixel, Estimated, Unmeasured,
    *                           Automatic, ValidatedManual, ValidatedAutomatic,
    *                           AutomaticPixel, AutomaticSubPixel, Ground)
    *     Sample = one element, numeric value
    *     Line = one element, numeric value
    *
    * Other keywords that are used include: Reference, ChooserName, DateTime,
    *   EditLock, AprioriSample, AprioriLine, SampleSigma, LineSigma,
    *   SampleResidual, LineResidual, ErrorSample, ErrorLine, Ignore
    *
    * @param p PvlGroup containing ControlMeasure information
    *
    * @throws Isis::iException::User - Invalid Measure Type
    *
    * @internal
    *   @history 2010-12-08 Tracie Sucharski - Added measure type of Ground.
    *
    */
  void ControlMeasure::Load(PvlGroup &p) {
    PvlGroup unknownKeywords(p);
    *p_serialNumber = p["SerialNumber"][0];
    unknownKeywords.DeleteKeyword("SerialNumber");

    if (p["MeasureType"][0] == "Reference") {
      p_measureType = Reference;
    }
    else if (p["MeasureType"][0] == "Candidate") {
      p_measureType = Candidate;
    }
    else if (p["MeasureType"][0] == "Manual") {
      p_measureType = Manual;
    }
    else if (p["MeasureType"][0] == "RegisteredPixel") {
      p_measureType = RegisteredPixel;
    }
    else if (p["MeasureType"][0] == "RegisteredSubPixel") {
      p_measureType = RegisteredSubPixel;
    }
    else if (p["MeasureType"][0] == "Ground") {
      p_measureType = Ground;
    }
    //  Backwards compatiability for old keyword values
    else if (p["MeasureType"][0] == "Estimated") {
      p_measureType = Candidate;
    }
    else if (p["MeasureType"][0] == "Unmeasured") {
      p_measureType = Candidate;
    }
    else if (p["MeasureType"][0] == "Automatic" ||
             p["MeasureType"][0] == "ValidatedManual") {
      p_measureType = RegisteredPixel;
    }
    else if (p["MeasureType"][0] == "ValidatedAutomatic") {
      p_measureType = RegisteredSubPixel;
    }
    //  Intermediate versions that might still be out there
    else if (p["MeasureType"][0] == "AutomaticPixel") {
      p_measureType = RegisteredPixel;
    }
    else if (p["MeasureType"][0] == "AutomaticSubPixel") {
      p_measureType = RegisteredSubPixel;
    }
    else {
      iString msg = "Invalid Measure Type, [" + p["MeasureType"][0] + "]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    unknownKeywords.DeleteKeyword("MeasureType");

    //  Check for old Reference keyword
    if (p.HasKeyword("Reference")) {
      iString reference = p["Reference"][0];
      if (reference.DownCase() == "true")
        p_measureType = Reference;
      unknownKeywords.DeleteKeyword("Reference");
    }

    if (p.HasKeyword("ChooserName")) {
      *p_chooserName = p["ChooserName"][0];
      unknownKeywords.DeleteKeyword("ChooserName");
    }

    if (p.HasKeyword("DateTime")) {
      *p_dateTime = p["DateTime"][0];
      unknownKeywords.DeleteKeyword("DateTime");
    }

    if (p.HasKeyword("EditLock")) {
      iString lock = p["EditLock"][0];
      if (lock.DownCase() == "true")
        p_editLock = true;
      unknownKeywords.DeleteKeyword("EditLock");
    }

    if (p.HasKeyword("JigsawRejected")) {
      iString reject = p["JigsawRejected"][0];
      if (reject.DownCase() == "true")
        p_jigsawRejected = true;
      unknownKeywords.DeleteKeyword("JigsawRejected");
    }

    if (p.HasKeyword("Ignore")) {
      iString ignore = p["Ignore"][0];
      if (ignore.DownCase() == "true")
        p_ignore = true;
      unknownKeywords.DeleteKeyword("Ignore");
    }

    p_sample = p["Sample"];
    unknownKeywords.DeleteKeyword("Sample");
    p_line = p["Line"];
    unknownKeywords.DeleteKeyword("Line");

    if (p.HasKeyword("Diameter")) {
      p_diameter = p["Diameter"];
      unknownKeywords.DeleteKeyword("Diameter");
    }

    if (p.HasKeyword("AprioriSample")) {
      p_aprioriSample = p["AprioriSample"];
      unknownKeywords.DeleteKeyword("AprioriSample");
    }

    if (p.HasKeyword("AprioriLine")) {
      p_aprioriLine = p["AprioriLine"];
      unknownKeywords.DeleteKeyword("AprioriLine");
    }

    if (p.HasKeyword("SampleSigma")) {
      p_sampleSigma = p["SampleSigma"];
      unknownKeywords.DeleteKeyword("SampleSigma");
    }

    if (p.HasKeyword("LineSigma")) {
      p_lineSigma = p["LineSigma"];
      unknownKeywords.DeleteKeyword("LineSigma");
    }

    if (p.HasKeyword("SampleResidual")) {
      p_sampleResidual = p["SampleResidual"];
      unknownKeywords.DeleteKeyword("SampleResidual");
    }

    if (p.HasKeyword("LineResidual")) {
      p_lineResidual = p["LineResidual"];
      unknownKeywords.DeleteKeyword("LineResidual");
    }

    //  Old keywords ErrorSample, ErrorLine, Error Magnitude
    if (p.HasKeyword("ErrorSample")) {
      p_sampleResidual = p["ErrorSample"];
      unknownKeywords.DeleteKeyword("ErrorSample");
    }

    if (p.HasKeyword("ErrorLine")) {
      p_lineResidual = p["ErrorLine"];
      unknownKeywords.DeleteKeyword("ErrorLine");
    }

    // Try to interpret remaining keywords as log data
    for (int unknownKeyIndex = 0; unknownKeyIndex < unknownKeywords.Keywords();
         unknownKeyIndex ++) {
      ControlMeasureLogData logEntry(unknownKeywords[unknownKeyIndex]);

      if (logEntry.IsValid()) {
        p_loggedData->append(logEntry);
      }
    }
  }


  ControlMeasure::Status ControlMeasure::SetAprioriLine(double aprioriLine) {
    if (p_editLock)
      return MeasureLocked;
    MeasureModified();
    p_aprioriLine = aprioriLine;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetAprioriSample(
    double aprioriSample) {
    if (p_editLock)
      return MeasureLocked;
    MeasureModified();
    p_aprioriSample = aprioriSample;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetCamera(Isis::Camera *camera) {
    if (p_editLock)
      return MeasureLocked;
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
    if (p_editLock)
      return MeasureLocked;
    *p_serialNumber = newSerialNumber;
    return Success;
  }


  //! Set chooser name to a user who last changed the coordinate
  ControlMeasure::Status ControlMeasure::SetChooserName() {
    if (p_editLock)
      return MeasureLocked;
    *p_chooserName = "";
    return Success;
  }


  //! Set the chooser name to an application that last changed the coordinate
  ControlMeasure::Status ControlMeasure::SetChooserName(iString name) {
    if (p_editLock)
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
    if (p_editLock)
      return MeasureLocked;
    MeasureModified();
    p_sample = sample;
    p_line = line;
    SetType(type);
    return Success;
  }


  //! Date Time - Creation Time
  ControlMeasure::Status ControlMeasure::SetDateTime() {
    if (p_editLock)
      return MeasureLocked;
    *p_dateTime = Application::DateTime();
    return Success;
  }


  //! Set date/time the coordinate was last changed to specified date/time
  ControlMeasure::Status ControlMeasure::SetDateTime(iString datetime) {
    if (p_editLock)
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
    if (p_editLock)
      return MeasureLocked;
    MeasureModified();
    p_diameter = diameter;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetEditLock(bool editLock) {
    p_editLock = editLock;
    return Success;
  }


  //! Set the focal plane x/y for the measured line/sample
  ControlMeasure::Status ControlMeasure::SetFocalPlaneMeasured(double x,
      double y) {
    if (p_editLock)
      return MeasureLocked;
    p_focalPlaneMeasuredX = x;
    p_focalPlaneMeasuredY = y;
    return Success;
  }


  //! Set the focal plane x/y for the computed (apriori) lat/lon
  ControlMeasure::Status ControlMeasure::SetFocalPlaneComputed(double x,
      double y) {
    if (p_editLock)
      return MeasureLocked;
    p_focalPlaneComputedX = x;
    p_focalPlaneComputedY = y;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetRejected(bool reject) {
    if (p_editLock)
      return MeasureLocked;
    MeasureModified();
    p_jigsawRejected = reject;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetIgnore(bool ignore) {
    if (p_editLock)
      return MeasureLocked;
    MeasureModified();
    p_ignore = ignore;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetLineSigma(double lineSigma) {
    if (p_editLock)
      return MeasureLocked;
    MeasureModified();
    p_lineSigma = lineSigma;
    return Success;
  }


  /**
    * @brief Set the Residual of the coordinate
    *
    * @param sampResidual  Sample Residual
    * @param lineResidual  Line Residual
    */
  ControlMeasure::Status ControlMeasure::SetResidual(double sampResidual,
      double lineResidual) {
    if (p_editLock)
      return MeasureLocked;
    MeasureModified();
    p_sampleResidual = sampResidual;
    p_lineResidual   = lineResidual;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetSampleSigma(double sampleSigma) {
    if (p_editLock)
      return MeasureLocked;
    MeasureModified();
    p_sampleSigma = sampleSigma;
    return Success;
  }


  //! Set how the coordinate was obtained
  ControlMeasure::Status ControlMeasure::SetType(MeasureType type) {
    if (p_editLock)
      return MeasureLocked;
    MeasureModified();
    p_measureType = type;
    return Success;
  }


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


  void ControlMeasure::DeleteLogData(long dataType) {
    for (int i = p_loggedData->size(); i >= 0; i--) {
      ControlMeasureLogData logDataEntry = p_loggedData->at(i);

      if (logDataEntry.GetDataType() == dataType)
        p_loggedData->remove(i);
    }
  }


  bool ControlMeasure::HasLogData(long dataType) const {
    for (int i = 0; i < p_loggedData->size(); i++) {
      const ControlMeasureLogData &logDataEntry = p_loggedData->at(i);

      if (logDataEntry.GetDataType() == dataType)
        return true;
    }

    return false;
  }


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
      return Application::Name();
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


  bool ControlMeasure::IsEditLocked() const {
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

  bool ControlMeasure::IsGround() const {
    return p_measureType == Ground;
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
    double dist = (p_lineResidual * p_lineResidual) + (p_sampleResidual * p_sampleResidual);
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


  void ControlMeasure::ConnectControlSN(ControlSerialNumber *sn) {
    associatedSN = sn;
  }


  void ControlMeasure::DisconnectControlSN() {
    associatedSN = NULL;
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

    qsl << "MeasureType" << MeasureTypeString();
    data.append(qsl);
    qsl.clear();

    return data;
  }


  /**
   * Sets up and returns a PvlGroup for the ControlMeasure
   *
   * @return The PvlGroup for the ControlMeasure
   *
   * @throws Isis::iException::Programmer - Invalid Measure Enumeration
   *
   * @internal
   *   @history 2010-12-08 Tracie Sucharski - Added measure type of Ground.
   *
   */
  PvlGroup ControlMeasure::CreatePvlGroup() {
    PvlGroup p("ControlMeasure");
    p += PvlKeyword("SerialNumber", *p_serialNumber);

    switch (p_measureType) {
      case Reference:
        p += PvlKeyword("MeasureType", "Reference");
        break;
      case Candidate:
        p += PvlKeyword("MeasureType", "Candidate");
        break;
      case Manual:
        p += PvlKeyword("MeasureType", "Manual");
        break;
      case RegisteredPixel:
        p += PvlKeyword("MeasureType", "RegisteredPixel");
        break;
      case RegisteredSubPixel:
        p += PvlKeyword("MeasureType", "RegisteredSubPixel");
        break;
      case Ground:
        p += PvlKeyword("MeasureType", "Ground");
        break;
      default:
        iString msg = "Invalid Measure Enumeration, [" + iString(p_measureType)
                      + "]";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        break;
    }

    p += PvlKeyword("ChooserName", GetChooserName());
    p += PvlKeyword("DateTime", GetDateTime());

    if (IsEditLocked())
      p += PvlKeyword("EditLock", "True");
    if (IsIgnored())
      p += PvlKeyword("Ignore", "True");

    p += PvlKeyword("Sample", p_sample);
    p += PvlKeyword("Line",   p_line);
    if (p_diameter != Isis::Null)
      p += PvlKeyword("Diameter", p_diameter);
    if (p_aprioriSample != Isis::Null) {
      p += PvlKeyword("AprioriSample", p_aprioriSample);
    }
    if (p_aprioriLine != Isis::Null) {
      p += PvlKeyword("AprioriLine", p_aprioriLine);
    }

    if (p_sampleSigma != Isis::Null) {
      p += PvlKeyword("SampleSigma", p_sampleSigma, "pixels");
    }
    if (p_lineSigma != Isis::Null) {
      p += PvlKeyword("LineSigma", p_lineSigma, "pixels");
    }
    if (p_sampleResidual != Isis::Null) {
      p += PvlKeyword("SampleResidual", p_sampleResidual, "pixels");
    }
    if (p_lineResidual != Isis::Null) {
      p += PvlKeyword("LineResidual", p_lineResidual, "pixels");
    }

    for (int logEntry = 0; logEntry < p_loggedData->size(); logEntry ++) {
      p += p_loggedData->at(logEntry).ToKeyword();
    }

    return p;
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
      case ControlMeasure::Reference:
        sPrintable = "Reference";
        break;

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

      case ControlMeasure::Ground:
        sPrintable = "Ground";
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
  iString ControlMeasure::MeasureTypeString() const {
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
      delete p_chooserName;
      p_chooserName = NULL;
    }
    if (p_loggedData) {
      delete p_loggedData;
      p_loggedData = NULL;
    }

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
    parentPoint = other.parentPoint;
    associatedSN = other.associatedSN;

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


  PBControlNet_PBControlPoint_PBControlMeasure
  ControlMeasure::ToProtocolBuffer() const {
    PBControlNet_PBControlPoint_PBControlMeasure protoBufMeasure;

    protoBufMeasure.set_serialnumber(GetCubeSerialNumber());
    switch (GetType()) {
      case ControlMeasure::Reference:
        protoBufMeasure.set_type(PBControlNet_PBControlPoint_PBControlMeasure::Reference);
        break;
      case ControlMeasure::Candidate:
        protoBufMeasure.set_type(PBControlNet_PBControlPoint_PBControlMeasure::Candidate);
        break;
      case ControlMeasure::Manual:
        protoBufMeasure.set_type(PBControlNet_PBControlPoint_PBControlMeasure::Manual);
        break;
      case ControlMeasure::RegisteredPixel:
        protoBufMeasure.set_type(PBControlNet_PBControlPoint_PBControlMeasure::RegisteredPixel);
        break;
      case ControlMeasure::RegisteredSubPixel:
        protoBufMeasure.set_type(PBControlNet_PBControlPoint_PBControlMeasure::RegisteredSubPixel);
        break;
      case ControlMeasure::Ground:
        protoBufMeasure.set_type(PBControlNet_PBControlPoint_PBControlMeasure::RegisteredSubPixel);
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

    if (GetSample() != 0. && GetLine() != 0.) {
      PBControlNet_PBControlPoint_PBControlMeasure::PBMeasure *m = protoBufMeasure.mutable_measurement();
      m->set_sample(GetSample());
      m->set_line(GetLine());
      if (GetSampleResidual() != Isis::Null) {
        m->set_sampleresidual(GetSampleResidual());
      }
      if (GetLineResidual() != Isis::Null) {
        m->set_lineresidual(GetLineResidual());
      }
    }

    if (GetDiameter() != Isis::Null)
      protoBufMeasure.set_diameter(GetDiameter());
    if (GetAprioriSample() != Isis::Null) {
      protoBufMeasure.set_apriorisample(GetAprioriSample());
    }
    if (GetAprioriLine() != Isis::Null) {
      protoBufMeasure.set_aprioriline(GetAprioriLine());
    }
    if (GetSampleSigma() != Isis::Null) {
      protoBufMeasure.set_samplesigma(GetSampleSigma());
    }
    if (GetLineSigma() != Isis::Null) {
      protoBufMeasure.set_linesigma(GetLineSigma());
    }

    return protoBufMeasure;
  }


  PBControlNetLogData_Point_Measure ControlMeasure::GetLogProtocolBuffer()
  const {
    PBControlNetLogData_Point_Measure protoBufLog;
    ControlMeasureLogData logDataEntry;

    foreach(logDataEntry, *p_loggedData) {
      *protoBufLog.add_loggedmeasuredata() = logDataEntry.ToProtocolBuffer();
    }

    return protoBufLog;
  }


  void ControlMeasure::MeasureModified() {
    *p_dateTime = "";
    *p_chooserName = "";
  }
}
