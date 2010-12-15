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
#include "iString.h"
#include "iTime.h"
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

    p_measureType = Candidate;
    p_editLock = false;
    p_jigsawRejected = false;
    p_ignore = false;

    p_sample = 0.0;
    p_line = 0.0;

  }


  /**
   * Copy the other control measure exactly.
   *
   * @param other The control measure to copy all of the values from
   */
  ControlMeasure::ControlMeasure(const ControlMeasure & other) {
    InitializeToNull();

    p_serialNumber = new iString(*other.p_serialNumber);
    p_chooserName = new iString(*other.p_chooserName);
    p_dateTime = new iString(*other.p_dateTime);

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
  }


  //! initialize pointers and other data to NULL
  void ControlMeasure::InitializeToNull() {
    p_serialNumber = NULL;
    p_chooserName = NULL;
    p_dateTime = NULL;

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
  }


  /**
   * Free the memory allocated by a control measure.
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
    *p_serialNumber = p["SerialNumber"][0];

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

    //  Check for old Reference keyword
    if (p.HasKeyword("Reference")) {
      iString reference = p["Reference"][0];
      if (reference.DownCase() == "true") p_measureType = Reference;
    }

    if (p.HasKeyword("ChooserName"))
      *p_chooserName = p["ChooserName"][0];

    if (p.HasKeyword("DateTime"))
      *p_dateTime = p["DateTime"][0];

    if (p.HasKeyword("EditLock")) {
      iString lock = p["EditLock"][0];
      if (lock.DownCase() == "true") p_editLock = true;
    }

    if (p.HasKeyword("JigsawRejected")) {
      iString reject = p["JigsawRejected"][0];
      if (reject.DownCase() == "true") p_jigsawRejected = true;
    }

    if (p.HasKeyword("Ignore")) {
      iString ignore = p["Ignore"][0];
      if (ignore.DownCase() == "true") p_ignore = true;
    }

    p_sample = p["Sample"];
    p_line = p["Line"];
    if (p.HasKeyword("Diameter")) p_diameter = p["Diameter"];

    if (p.HasKeyword("AprioriSample")) p_aprioriSample = p["AprioriSample"];
    if (p.HasKeyword("AprioriLine")) p_aprioriLine = p["AprioriLine"];

    if (p.HasKeyword("SampleSigma")) p_sampleSigma = p["SampleSigma"];
    if (p.HasKeyword("LineSigma")) p_lineSigma = p["LineSigma"];

    if (p.HasKeyword("SampleResidual")) p_sampleResidual = p["SampleResidual"];
    if (p.HasKeyword("LineResidual")) p_lineResidual = p["LineResidual"];
    //  Old keywords ErrorSample, ErrorLine, Error Magnitude
    if (p.HasKeyword("ErrorSample")) p_sampleResidual = p["ErrorSample"];
    if (p.HasKeyword("ErrorLine")) p_lineResidual = p["ErrorLine"];
  }


  ControlMeasure::Status ControlMeasure::SetAprioriLine(
      double aprioriLine) {
    if (p_editLock) return MeasureLocked;
    MeasureModified();
    p_aprioriLine = aprioriLine;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetAprioriSample(
      double aprioriSample) {
    if (p_editLock) return MeasureLocked;
    MeasureModified();
    p_aprioriSample = aprioriSample;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetCamera (Isis::Camera *camera) {
    if (p_editLock) return MeasureLocked;
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
    if (p_editLock) return MeasureLocked;
    *p_serialNumber = newSerialNumber;
    return Success;
  }


  //! Set chooser name to a user who last changed the coordinate
  ControlMeasure::Status ControlMeasure::SetChooserName() {
    if (p_editLock) return MeasureLocked;
    *p_chooserName = "";
    return Success;
  }


  //! Set the chooser name to an application that last changed the coordinate
  ControlMeasure::Status ControlMeasure::SetChooserName(iString name) {
    if (p_editLock) return MeasureLocked;
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
    return SetCoordinate(sample, line, Type());
  }


  /**
    * @brief Set the coordinate of the measurement
    *
    * @param sample  Sample coordinate of the measurement
    * @param line    Line coordinate of the measurement
    * @param type    The type of the coordinate
    */
  ControlMeasure::Status ControlMeasure::SetCoordinate(double sample,
                                                       double line,
                                                       MeasureType type) {
    if (p_editLock) return MeasureLocked;
    MeasureModified();
    p_sample = sample;
    p_line = line;
    SetType(type);
    return Success;
  }


  //! Date Time - Creation Time
  ControlMeasure::Status ControlMeasure::SetDateTime() {
    if (p_editLock) return MeasureLocked;
    *p_dateTime = Application::DateTime();
    return Success;
  }


  //! Set date/time the coordinate was last changed to specified date/time
  ControlMeasure::Status ControlMeasure::SetDateTime(iString datetime) {
    if (p_editLock) return MeasureLocked;
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
    if (p_editLock) return MeasureLocked;
    MeasureModified();
    p_diameter = diameter;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetEditLock(bool editLock) {
    p_editLock = editLock;
    return Success;
  }


  //! Set the focal plane x/y for the measured line/sample
  ControlMeasure::Status ControlMeasure::SetFocalPlaneMeasured(double x, double y) {
    if (p_editLock) return MeasureLocked;
    p_focalPlaneMeasuredX = x;
    p_focalPlaneMeasuredY = y;
    return Success;
  }


  //! Set the focal plane x/y for the computed (apriori) lat/lon
  ControlMeasure::Status ControlMeasure::SetFocalPlaneComputed(double x, double y) {
    if (p_editLock) return MeasureLocked;
    p_focalPlaneComputedX = x;
    p_focalPlaneComputedY = y;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetRejected(bool reject) {
    if (p_editLock) return MeasureLocked;
    MeasureModified();
    p_jigsawRejected = reject;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetIgnore(bool ignore) {
    if (p_editLock) return MeasureLocked;
    MeasureModified();
    p_ignore = ignore;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetLineSigma(double lineSigma) {
    if (p_editLock) return MeasureLocked;
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
    if (p_editLock) return MeasureLocked;
    MeasureModified();
    p_sampleResidual = sampResidual;
    p_lineResidual   = lineResidual;
    return Success;
  }


  ControlMeasure::Status ControlMeasure::SetSampleSigma(double sampleSigma) {
    if (p_editLock) return MeasureLocked;
    MeasureModified();
    p_sampleSigma = sampleSigma;
    return Success;
  }


  //! Set how the coordinate was obtained
  ControlMeasure::Status ControlMeasure::SetType(MeasureType type) {
    if (p_editLock) return MeasureLocked;
    MeasureModified();
    p_measureType = type;
    return Success;
  }


  double ControlMeasure::AprioriLine() const {
    return p_aprioriLine;
  }


  double ControlMeasure::AprioriSample() const {
    return p_aprioriSample;
  }


  Isis::Camera *ControlMeasure::Camera() const {
    return p_camera;
  }


  //! Return the chooser name
  iString ControlMeasure::ChooserName() const {
    if(*p_chooserName != "") {
      return *p_chooserName;
    }
    else {
      return Application::Name();
    }
  }


  //! Return the serial number of the cube containing the coordinate
  iString ControlMeasure::CubeSerialNumber() const {
    return *p_serialNumber;
  }


  //! Return the date/time the coordinate was last changed
  iString ControlMeasure::DateTime() const {
    if(*p_dateTime != "") {
      return *p_dateTime;
    }
    else {
      return Application::DateTime();
    }
  }


  //! Return the diameter of the crater in pixels (0 implies no crater)
  double ControlMeasure::Diameter() const {
    return p_diameter;
  }


  bool ControlMeasure::EditLock() const {
    return p_editLock;
  }


  double ControlMeasure::FocalPlaneComputedX() const {
    return p_focalPlaneComputedX;
  }


  double ControlMeasure::FocalPlaneComputedY() const {
    return p_focalPlaneComputedY;
  }


  double ControlMeasure::FocalPlaneMeasuredX() const {
    return p_focalPlaneMeasuredX;
  }


  double ControlMeasure::FocalPlaneMeasuredY() const {
    return p_focalPlaneMeasuredY;
  }


  bool ControlMeasure::Ignore() const {
    return p_ignore;
  }


  bool ControlMeasure::IsRejected() const {
    return p_jigsawRejected;
  }


  bool ControlMeasure::IsMeasured () const {
    return p_measureType != Candidate;
  }


  bool ControlMeasure::IsRegistered () const {
    return (p_measureType == RegisteredPixel ||
            p_measureType == RegisteredSubPixel);
  }

  bool ControlMeasure::IsGround () const {
    return p_measureType == Ground;
  }

/*
  bool ControlMeasure::IsStatisticallyRelevant(MeasureData field) const {
    bool relevant = false;
    bool validField = false;
    
    switch(field) {
      case ComputerEphemerisTime:
      case EditLock:
      case Ignore:
      case IsMeasured:
      case IsRegistered:
      case LineResidual:
      case LineSigma:
      case ResidualMagnitude:
      case SampleResidual:
      case SampleSigma:
        relevant = true;
        validField = true;

      case NumMeasureDataFields:
    }

    if(!validField) {
      iString msg = "Cannot test IsStatisticallyRelevant on Measure Data ["
          + iString(field) + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }
*/

  double ControlMeasure::Line() const {
    return p_line;
  }


  double ControlMeasure::LineResidual() const {
    return p_lineResidual;
  }


  double ControlMeasure::LineSigma() const {
    return p_lineSigma;
  }


  //! Return Residual magnitude
  double ControlMeasure::ResidualMagnitude() const {
    double dist = (p_lineResidual * p_lineResidual) + (p_sampleResidual * p_sampleResidual);
    return sqrt(dist);
  }


  double ControlMeasure::Sample() const {
    return p_sample;
  }


  double ControlMeasure::SampleResidual() const {
    return p_sampleResidual;
  }


  double ControlMeasure::SampleSigma() const {
    return p_sampleSigma;
  }


  ControlMeasure::MeasureType ControlMeasure::Type () const {
    return p_measureType;
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

    qsl << "ResidualMagnitude" << QString::number(ResidualMagnitude());
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
        throw iException::Message(iException::Programmer,msg,_FILEINFO_);
        break;
    }

    p += PvlKeyword("ChooserName", ChooserName());
    p += PvlKeyword("DateTime", DateTime());

    if (p_editLock == true) p += PvlKeyword("EditLock", "True");
    if (p_ignore == true) p += PvlKeyword("Ignore", "True");

    p += PvlKeyword("Sample", p_sample);
    p += PvlKeyword("Line",   p_line);
    if (p_diameter != Isis::Null) p += PvlKeyword("Diameter", p_diameter);
    if (p_aprioriSample != Isis::Null) {
      p += PvlKeyword("AprioriSample", p_aprioriSample);
    }
    if (p_aprioriLine != Isis::Null) {
      p += PvlKeyword("AprioriLine", p_aprioriLine);
    }

    if (p_sampleSigma != Isis::Null) {
      p += PvlKeyword("SampleSigma", p_sampleSigma,"pixels");
    }
    if (p_lineSigma != Isis::Null) {
      p += PvlKeyword("LineSigma", p_lineSigma,"pixels");
    }
    if (p_sampleResidual != Isis::Null) {
      p += PvlKeyword("SampleResidual", p_sampleResidual,"pixels");
    }
    if (p_lineResidual != Isis::Null) {
      p += PvlKeyword("LineResidual", p_lineResidual,"pixels");
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
        sPrintable ="Reference";
        break;

      case ControlMeasure::Candidate:
        sPrintable ="Candidate";
        break;

      case ControlMeasure::Manual:
        sPrintable ="Manual";
        break;

      case ControlMeasure::RegisteredPixel:
        sPrintable ="RegisteredPixel";
        break;

      case ControlMeasure::RegisteredSubPixel:
        sPrintable ="RegisteredSubPixel";
        break;

      case ControlMeasure::Ground:
        sPrintable ="Ground";
        break;
    }

    if(sPrintable == "") {
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

    p_serialNumber = new iString;
    p_chooserName = new iString;
    p_dateTime = new iString;

    *p_serialNumber = *other.p_serialNumber;
    *p_chooserName = *other.p_chooserName;
    *p_dateTime = *other.p_dateTime;

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
  bool ControlMeasure::operator==(const Isis::ControlMeasure & pMeasure) const {
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


  void ControlMeasure::MeasureModified() {
    *p_dateTime = "";
    *p_chooserName = "";
  }
}
