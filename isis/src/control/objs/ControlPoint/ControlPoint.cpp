#include "IsisDebug.h"
#include "ControlPoint.h"

#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <QHash>
#include <QString>
#include <QStringList>

#include "Application.h"
#include "CameraDetectorMap.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "CameraGroundMap.h"
#include "ControlNet.h"
#include "Cube.h"
#include "iString.h"
#include "Latitude.h"
#include "Longitude.h"
#include "PBControlNetIO.pb.h"
#include "PBControlNetLogData.pb.h"
#include "PvlObject.h"
#include "SerialNumberList.h"
#include "SpecialPixel.h"
#include "Statistics.h"

using boost::numeric::ublas::symmetric_matrix;
using boost::numeric::ublas::upper;
using namespace std;

namespace Isis {
  /**
   * Construct a control point
   *
   * @author tsucharski (5/5/2010)
   *
   */
  ControlPoint::ControlPoint() : invalid(false) {
    measures = NULL;
    cubeSerials = NULL;

    measures = new QHash< QString, ControlMeasure * >;
    cubeSerials = new QStringList;

    type = Tie;
    dateTime = "";
    editLock = false;
    ignore = false;
    jigsawRejected = false;
    referenceExplicitlySet = false;
    aprioriSurfacePointSource = SurfacePointSource::None;
    aprioriRadiusSource = RadiusSource::None;
    parentNetwork = NULL;
    referenceMeasure = NULL;
  }

  ControlPoint::ControlPoint(const ControlPoint &other) {
    measures = NULL;
    cubeSerials = NULL;
    referenceMeasure = NULL;

    measures = new QHash< QString, ControlMeasure * >;
    cubeSerials = new QStringList;

    QHashIterator< QString, ControlMeasure * > i(*other.measures);
    while (i.hasNext()) {
      i.next();
      ControlMeasure *newMeasure = new ControlMeasure(*i.value());
      if (other.referenceMeasure == i.value())
        referenceMeasure = newMeasure;
      QString newSerial = newMeasure->GetCubeSerialNumber();
      newMeasure->parentPoint = this;
      measures->insert(newSerial, newMeasure);
      cubeSerials->append(newSerial);
    }

    if (referenceMeasure == NULL && cubeSerials->size() != 0)
      referenceMeasure = measures->value(cubeSerials->at(0));

    parentNetwork = other.parentNetwork;
    id = other.id;
    chooserName = other.chooserName;
    dateTime = other.dateTime;
    type = other.type;
    invalid = other.invalid;
    editLock = other.editLock;
    jigsawRejected = other.jigsawRejected;
    referenceExplicitlySet = other.referenceExplicitlySet;
    ignore = other.ignore;
    aprioriSurfacePointSource = other.aprioriSurfacePointSource;
    aprioriSurfacePointSourceFile = other.aprioriSurfacePointSourceFile;
    aprioriRadiusSource = other.aprioriRadiusSource;
    aprioriRadiusSourceFile = other.aprioriRadiusSourceFile;
    aprioriSurfacePoint = other.aprioriSurfacePoint;
    surfacePoint = other.surfacePoint;
    numberOfRejectedMeasures = other.numberOfRejectedMeasures;
  }

  ControlPoint::ControlPoint(const PBControlNet_PBControlPoint &protoBufPt) {
    measures = NULL;
    cubeSerials = NULL;
    referenceMeasure = NULL;

    measures = new QHash< QString, ControlMeasure * >;
    cubeSerials = new QStringList;
    Init(protoBufPt);

    for (int m = 0 ; m < protoBufPt.measures_size() ; m++) {
      // Create a PControlMeasure and fill in it's info.
      // with the values from the input file.
      ControlMeasure *measure = new ControlMeasure(protoBufPt.measures(m));
      AddMeasure(measure);
    }

    if (protoBufPt.has_referenceindex()) {
      referenceExplicitlySet = true;

      referenceMeasure =
        (*measures)[cubeSerials->at(protoBufPt.referenceindex())];
    }
    else {
      referenceExplicitlySet = false;
    }
  }


  ControlPoint::ControlPoint(const PBControlNet_PBControlPoint &protoBufPt,
      const PBControlNetLogData_Point &logProtoBuf) {
    measures = NULL;
    cubeSerials = NULL;
    referenceMeasure = NULL;

    measures = new QHash< QString, ControlMeasure * >;
    cubeSerials = new QStringList;
    Init(protoBufPt);

    for (int m = 0 ; m < protoBufPt.measures_size() ; m++) {
      // Create a PControlMeasure and fill in it's info.
      // with the values from the input file.
      ControlMeasure *measure =
        new ControlMeasure(protoBufPt.measures(m), logProtoBuf.measures(m));

      AddMeasure(measure);
    }

    if (protoBufPt.has_referenceindex()) {
      referenceExplicitlySet = true;

      referenceMeasure =
        (*measures)[cubeSerials->at(protoBufPt.referenceindex())];
    }
    else {
      referenceExplicitlySet = false;
    }
  }


  /**
   * Construct a control point with given Id
   *
   * @param id Control Point Id
   */
  ControlPoint::ControlPoint(const iString &newId) : invalid(false) {
    parentNetwork = NULL;
    measures = NULL;
    referenceMeasure = NULL;
    measures = new QHash< QString, ControlMeasure * >;
    cubeSerials = new QStringList;

    id = newId;
    type = Tie;
    editLock = false;
    jigsawRejected = false;
    referenceExplicitlySet = false;
    ignore = false;
    aprioriSurfacePointSource = SurfacePointSource::None;
    aprioriRadiusSource = RadiusSource::None;
  }


  /**
   * This destroys the current instance and cleans up any and all allocated
   *    memory.
   */
  ControlPoint::~ControlPoint() {
    if (measures != NULL) {
      QList< QString > keys = measures->keys();
      for (int i = 0; i < keys.size(); i++) {
        delete(*measures)[keys[i]];
        (*measures)[keys[i]] = NULL;
      }

      delete measures;
      measures = NULL;
    }

    if (cubeSerials) {
      delete cubeSerials;
      cubeSerials = NULL;
    }

    referenceMeasure = NULL;
  }

  /**
  * Loads the PvlObject into a ControlPoint
  *
  * @param p PvlObject containing ControlPoint information
  * @param forceBuild Allows invalid Control Measures to be added to this
  *                   Control Point
  *
  * @throws Isis::iException::User - Invalid Point Type
  * @throws Isis::iException::User - Unable to add ControlMeasure to Control
  *                                  Point
  *
  * @history 2008-06-18  Tracie Sucharski/Jeannie Walldren, Fixed bug with
  *                         checking for "True" vs "true", change to
  *                         lower case for comparison.
  * @history 2009-12-29  Tracie Sucharski - Added new ControlPoint information.
  * @history 2010-01-13  Tracie Sucharski - Changed from Set methods to simply
  *                         setting private variables to increase speed?
  * @history 2010-07-30  Tracie Sucharski, Updated for changes made after
  *                         additional working sessions for Control network
  *                         design.
  * @history 2010-09-01  Tracie Sucharski, Add checks for AprioriLatLonSource
  *                         AprioriLatLonSourceFile.  If there are
  *                         AprioriSigmas,but no AprioriXYZ, use the XYZ values.
  * @history 2010-09-15 Tracie Sucharski, It was decided after mtg with
  *                         Debbie, Stuart, Ken and Tracie that ControlPoint
  *                         will only function with x/y/z, not lat/lon/radius.
  *                         It will be the responsibility of the application
  *                         or class using ControlPoint to set up a
  *                         SurfacePoint object to do conversions between x/y/z
  *                         and lat/lon/radius.
  *                         So... remove all conversion methods from this
  *                         class.
  *                         It was also decided that when importing old
  *                         networks that contain Sigmas, the sigmas will not
  *                         be imported , due to conflicts with the units of
  *                         the sigmas,we cannot get accurate x,y,z sigams from
  *                         the lat,lon,radius sigmas without the covariance
  *                         matrix.
  * @history 2010-09-28 Tracie Sucharski, Added back the conversion methods
  *                         from lat,lon,radius to x,y,z only for the point,
  *                         since that is what most applications need.
  * @history 2010-12-02 Debbie A. Cook, Added units to
  *                         SurfacePoint.SetSpherical calls.
  */
  void ControlPoint::Load(PvlObject &p) {
    id = (std::string) p["PointId"];
    if ((std::string)p["PointType"] == "Ground") {
      type = Ground;
    }
    else if ((std::string)p["PointType"] == "Tie") {
      type = Tie;
    }
    else {
      std::string msg = "Invalid Point Type, [" + (std::string)p["PointType"] +
          "]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    if (p.HasKeyword("Ignore")) {
      iString ignoreStr = (std::string)p["Ignore"];
      if (ignoreStr.DownCase() == "true")
        ignore = true;
    }
    if (p.HasKeyword("AprioriXYZSource")) {
      if ((std::string)p["AprioriXYZSource"] == "None") {
        aprioriSurfacePointSource = SurfacePointSource::None;
      }
      else if ((std::string)p["AprioriXYZSource"] == "User") {
        aprioriSurfacePointSource = SurfacePointSource::User;
      }
      else if ((std::string)p["AprioriXYZSource"] == "AverageOfMeasures") {
        aprioriSurfacePointSource = SurfacePointSource::AverageOfMeasures;
      }
      else if ((std::string)p["AprioriXYZSource"] == "Reference") {
        aprioriSurfacePointSource = SurfacePointSource::Reference;
      }
      else if ((std::string)p["AprioriXYZSource"] == "Basemap") {
        aprioriSurfacePointSource = SurfacePointSource::Basemap;
      }
      else if ((std::string)p["AprioriXYZSource"] == "BundleSolution") {
        aprioriSurfacePointSource = SurfacePointSource::BundleSolution;
      }
      else {
        std::string msg = "Invalid AprioriXYZSource, [" +
            (std::string)p["AprioriXYZSource"] + "]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
    if (p.HasKeyword("AprioriXYZSourceFile")) {
      aprioriSurfacePointSourceFile = (std::string)p["AprioriXYZSourceFile"];
    }

    //  Look for AprioriLatLonSource.  These keywords may exist in old nets.
    if (p.HasKeyword("AprioriLatLonSource")) {
      if ((std::string)p["AprioriLatLonSource"] == "None") {
        aprioriSurfacePointSource = SurfacePointSource::None;
      }
      else if ((std::string)p["AprioriLatLonSource"] == "User") {
        aprioriSurfacePointSource = SurfacePointSource::User;
      }
      else if ((std::string)p["AprioriLatLonSource"] == "AverageOfMeasures") {
        aprioriSurfacePointSource = SurfacePointSource::AverageOfMeasures;
      }
      else if ((std::string)p["AprioriLatLonSource"] == "Reference") {
        aprioriSurfacePointSource = SurfacePointSource::Reference;
      }
      else if ((std::string)p["AprioriLatLonSource"] == "Basemap") {
        aprioriSurfacePointSource = SurfacePointSource::Basemap;
      }
      else if ((std::string)p["AprioriLatLonSource"] == "BundleSolution") {
        aprioriSurfacePointSource = SurfacePointSource::BundleSolution;
      }
      else {
        std::string msg = "Invalid AprioriXYZSource, [" +
            (std::string)p["AprioriXYZSource"] + "]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
    if (p.HasKeyword("AprioriLatLonSourceFile")) {
      aprioriSurfacePointSourceFile = p["AprioriLatLonSourceFile"][0];
    }

    if (p.HasKeyword("AprioriRadiusSource")) {
      if ((std::string)p["AprioriRadiusSource"] == "None") {
        aprioriRadiusSource = RadiusSource::None;
      }
      else if ((std::string)p["AprioriRadiusSource"] == "User") {
        aprioriRadiusSource = RadiusSource::User;
      }
      else if ((std::string)p["AprioriRadiusSource"] == "AverageOfMeasures") {
        aprioriRadiusSource = RadiusSource::AverageOfMeasures;
      }
      else if ((std::string)p["AprioriRadiusSource"] == "Ellipsoid") {
        aprioriRadiusSource = RadiusSource::Ellipsoid;
      }
      else if ((std::string)p["AprioriRadiusSource"] == "DEM") {
        aprioriRadiusSource = RadiusSource::DEM;
      }
      else if ((std::string)p["AprioriRadiusSource"] == "BundleSolution") {
        aprioriRadiusSource = RadiusSource::BundleSolution;
      }
      else {
        std::string msg = "Invalid AprioriRadiusSource, [" +
            (std::string)p["AprioriRadiusSource"] + "]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
    if (p.HasKeyword("AprioriRadiusSourceFile")) {
      aprioriRadiusSourceFile = (std::string)p["AprioriRadiusSourceFile"];
    }

    if (p.HasKeyword("AprioriX") &&
        p.HasKeyword("AprioriY") &&
        p.HasKeyword("AprioriZ")) {
      aprioriSurfacePoint.SetRectangular(
        Displacement(p["AprioriX"], Displacement::Meters),
        Displacement(p["AprioriY"], Displacement::Meters),
        Displacement(p["AprioriZ"], Displacement::Meters));
    }

    //  Look for AprioriLatitude/Longitude/Radius.  These keywords may
    //  exist in old nets.  Convert to x/y/z.
    else if (p.HasKeyword("AprioriLatitude") &&
        p.HasKeyword("AprioriLongitude") &&
        p.HasKeyword("AprioriRadius")) {
      aprioriSurfacePoint.SetSpherical(
        Latitude(p["AprioriLatitude"], Angle::Degrees),
        Longitude(p["AprioriLongitude"], Angle::Degrees),
        Distance(p["AprioriRadius"], Distance::Meters));
    }

    if (p.HasKeyword("X") && p.HasKeyword("Y") && p.HasKeyword("Z")) {
      surfacePoint.SetRectangular(
        Displacement(p["X"], Displacement::Meters),
        Displacement(p["Y"], Displacement::Meters),
        Displacement(p["Z"], Displacement::Meters));
    }

    // Look for Latitude/Longitude/Radius.  These keywords may exist in old
    // nets.  Convert to x/y/z.
    else if (p.HasKeyword("Latitude") && p.HasKeyword("Longitude") &&
        p.HasKeyword("Radius")) {
      surfacePoint.SetSpherical(
        Latitude(p["Latitude"], Angle::Degrees),
        Longitude(p["Longitude"], Angle::Degrees),
        Distance(p["Radius"], Distance::Meters));
    }

    if (p.HasKeyword("AprioriCovarianceMatrix")) {
      PvlKeyword &matrix = p["AprioriCovarianceMatrix"];
      symmetric_matrix<double, upper> aprioriCovariance;
      aprioriCovariance.resize(3);
      aprioriCovariance.clear();

      aprioriCovariance(0, 0) = matrix[0];
      aprioriCovariance(0, 1) = matrix[1];
      aprioriCovariance(0, 2) = matrix[2];
      aprioriCovariance(1, 1) = matrix[3];
      aprioriCovariance(1, 2) = matrix[4];
      aprioriCovariance(2, 2) = matrix[5];

      aprioriSurfacePoint.SetRectangularMatrix(aprioriCovariance);
    }

    if (p.HasKeyword("ApostCovarianceMatrix")) {
      PvlKeyword &matrix = p["ApostCovarianceMatrix"];

      symmetric_matrix<double, upper> apostCovariance;
      apostCovariance.resize(3);
      apostCovariance.clear();

      apostCovariance(0, 0) = matrix[0];
      apostCovariance(0, 1) = matrix[1];
      apostCovariance(0, 2) = matrix[2];
      apostCovariance(1, 1) = matrix[3];
      apostCovariance(1, 2) = matrix[4];
      apostCovariance(2, 2) = matrix[5];

      surfacePoint.SetRectangularMatrix(apostCovariance);
    }

    if (p.HasKeyword("ChooserName"))
      chooserName = p["ChooserName"][0];
    if (p.HasKeyword("DateTime"))
      dateTime = p["DateTime"][0];
    if (p.HasKeyword("JigsawRejected")) {
      iString reject = p["JigsawRejected"][0];
      if (reject.DownCase() == "true")
        jigsawRejected = true;
    }

    //  Process Measures
    for (int g = 0; g < p.Groups(); g++) {
      try {
        PvlGroup &measureGroup = p.Group(g);
        if (measureGroup.IsNamed("ControlMeasure")) {
          ControlMeasure *cm = new ControlMeasure;
          cm->Load(measureGroup);
          AddMeasure(cm);
          try {
            if (measureGroup.FindKeyword("Reference")[0].UpCase() == "TRUE") {
              SetRefMeasure(cm);
            }
          }
          catch (iException &e) {
            e.Clear();
          }
        }
      }
      catch (iException &e) {
        iString msg = "Unable to add Control Measure to ControlPoint [" +
            GetId() + "]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }

    if (p.HasKeyword("EditLock")) {
      iString locked = (std::string)p["EditLock"];
      if (locked.DownCase() == "true")
        editLock = true;
    }
  }


  /**
   * Add a measurement to the control point, taking ownership of the measure in
   * the process.
   *
   * @param measure The ControlMeasure to add
   */
  void ControlPoint::Add(ControlMeasure *measure) {
    PointModified();
    AddMeasure(measure);
  }

  void ControlPoint::AddMeasure(ControlMeasure *measure) {
    // Make sure measure is unique
    foreach(ControlMeasure * m, measures->values()) {
      if (m->GetCubeSerialNumber() == measure->GetCubeSerialNumber()) {
        iString msg = "The SerialNumber is not unique. A measure with "
            "serial number [" + measure->GetCubeSerialNumber() + "] already "
            "exists for ControlPoint [" + GetId() + "]";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
    }

    if (!measures->size()) {
      ASSERT(referenceMeasure == NULL);
      referenceMeasure = measure;
    }

    measure->parentPoint = this;
    QString newSerial = measure->GetCubeSerialNumber();
    measures->insert(newSerial, measure);
    cubeSerials->append(newSerial);

    // notify parent network if we have one
    if (parentNetwork)
      parentNetwork->MeasureAdded(measure);
  }


  /**
   * Throws an exception if none of the point's measures have the given serial
   * number.  It is common to ensure that a measure exists before taking some
   * action.
   *
   * @param sn The serial number of the measure to validate
   */
  void ControlPoint::ValidateMeasure(iString serialNumber) const {
    if (!measures->contains(serialNumber)) {
      iString msg = "No measure with serial number [" + serialNumber +
          "] is owned by this point";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Remove a measurement from the control point, deleting reference measure
   * is allowed.
   *
   * @param serialNumber The serial number of the measure to delete
   */
  void ControlPoint::Delete(iString serialNumber) {
    ValidateMeasure(serialNumber);
    ControlMeasure *cm = (*measures)[serialNumber];

    // remove measure from the point's data structures
    measures->remove(serialNumber);
    cubeSerials->removeAt(cubeSerials->indexOf(serialNumber));

    // update the reference measure
    if (cubeSerials->size()) {
      if (referenceMeasure == cm) {
        referenceMeasure = (*measures)[cubeSerials->at(0)];
        referenceExplicitlySet = false;
      }
    }
    else {
      referenceMeasure = NULL;
    }

    // notify parent network of the change
    if (parentNetwork)
      parentNetwork->MeasureDeleted(cm);

    delete cm;
    cm = NULL;

    PointModified();
  }


  /**
   * Remove a measurement from the control point, deleting reference measure
   * is allowed.
   *
   * @param measure The measure to delete
   */
  void ControlPoint::Delete(ControlMeasure *measure) {
    ASSERT(measure);
    Delete(measure->GetCubeSerialNumber());
  }


  /**
   * Remove a measurement from the control point, deleting reference measure
   * is allowed.
   *
   * @param index The index of the control measure to delete
   */
  void ControlPoint::Delete(int index) {
    if (index < 0 || index >= cubeSerials->size()) {
      iString msg = "index [" + iString(index) + "] out of bounds";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    Delete(cubeSerials->at(index));
  }


  /**
   * Reset all the Apriori info to defaults
   *
   * @author Sharmila Prasad (10/22/2010)
   */
  ControlPoint::Status ControlPoint::ResetApriori() {
    if (IsEditLocked())
      return PointLocked;

    aprioriSurfacePointSource = SurfacePointSource::None;
    aprioriSurfacePointSourceFile    = "";
    aprioriRadiusSource     = RadiusSource::None;
    aprioriRadiusSourceFile = "";

    aprioriSurfacePoint = SurfacePoint();

    return Success;
  }


  /**
   * Get a control measure based on its cube's serial number.
   *
   * @param serialNumber serial number of measure to get
   * @returns control measure with matching serial number
   */
  ControlMeasure *ControlPoint::GetMeasure(iString serialNumber) {
    ValidateMeasure(serialNumber);
    return (*measures)[serialNumber];
  }


  /**
   * Get a control measure based on its cube's serial number.
   *
   * @param serialNumber serial number of measure to get
   * @returns const control measure with matching serial number
   */
  const ControlMeasure *ControlPoint::GetMeasure(iString serialNumber) const {
    ValidateMeasure(serialNumber);
    return measures->value(serialNumber);
  }


  const ControlMeasure *ControlPoint::GetMeasure(int index) const {
    if (index < 0 || index >= cubeSerials->size()) {
      iString msg = "Index [" + iString(index) + "] out of range";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return GetMeasure(cubeSerials->at(index));
  }


  ControlMeasure *ControlPoint::GetMeasure(int index) {
    if (index < 0 || index >= cubeSerials->size()) {
      iString msg = "Index [" + iString(index) + "] out of range";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return GetMeasure(cubeSerials->at(index));
  }


  /**
   * Get the reference control measure.
   *
   * @returns const reference measure for this point
   */
  const ControlMeasure *ControlPoint::GetRefMeasure() const {
    if (referenceMeasure == NULL) {
      iString msg = "Control point [" + GetId() + "] has no reference measure!";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return referenceMeasure;
  }


  ControlMeasure *ControlPoint::GetRefMeasure() {
    if (referenceMeasure == NULL) {
      iString msg = "Control point [" + GetId() + "] has no reference measure!";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return referenceMeasure;
  }


  /**
   * Set the point's chooser name. This will be lost if any attributes relating
   *   to this point is later changed and the current user will be set. This is
   *   one of the 'last modified attributes' referred to in other comments.
   *
   * @param name The username of the person who last modified this control point
   */
  ControlPoint::Status ControlPoint::SetChooserName(iString name) {
    if (editLock)
      return PointLocked;
    chooserName = name;
    return Success;
  }


  /**
   * Set the point's last modified time. This will be lost if any attributes
   *   relating to this point are later changed and the current time will be
   *   set. This is one of the 'last modified attributes' referred to in other
   *   comments.
   *
   * @param newDateTime The date and time this control point was last modified
   */
  ControlPoint::Status ControlPoint::SetDateTime(iString newDateTime) {
    if (editLock)
      return PointLocked;
    dateTime = newDateTime;
    return Success;
  }


  /**
   * Set the EditLock state. If edit lock is on, then most attributes relating
   *   to this point are not modifiable. Edit lock is like "Don't modify my
   *   attributes, but you can still modify my measures' attributes". The
   *   reference measure is implicitely edit locked if the point is edit locked.
   *
   * @param lock True to enable edit lock, false to disable it and allow the
   *   point to be modified.
   */
  ControlPoint::Status ControlPoint::SetEditLock(bool lock) {
    editLock = lock;
    return Success;
  }


  /**
   * Set the jigsawRejected state. If IsRejected is true, then this point should be
   *   ignored until the next iteration in the bundle adjustement.  BundleAdjust
   *   decides when to reject or accept a point. The initial IsRejected state of
   *   a measure is false.
   *
   * @param reject True to reject a measure, false to include it in the adjustment
   */
  ControlPoint::Status ControlPoint::SetRejected(bool reject) {
    jigsawRejected = reject;
    return Success;
  }


  /**
   * Sets the Id of the control point
   *
   * @param id Control Point Id
   *
   * @return  (int) status Success or PointLocked
   */
  ControlPoint::Status ControlPoint::SetId(iString newId) {
    if (editLock)
      return PointLocked;
    id = newId;
    return Success;
  }


  /**
   * Set the point's reference measure
   *
   * @param cm The new reference measure
   */
  ControlPoint::Status ControlPoint::SetRefMeasure(ControlMeasure *cm) {
    if (editLock)
      return PointLocked;

    ASSERT(cm);
    referenceExplicitlySet = true;
    referenceMeasure = cm;
    return Success;
  }


  /**
   * Set the point's reference measure
   *
   * @param index The index of the new reference measure
   */
  ControlPoint::Status ControlPoint::SetRefMeasure(int index) {
    if (editLock)
      return PointLocked;

    if (index < 0 || index >= cubeSerials->size()) {
      iString msg = "Index [";
      msg += index + "] out of range";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    referenceExplicitlySet = true;
    referenceMeasure = (*measures)[cubeSerials->at(index)];
    return Success;
  }


  /**
   * Set the points reference measure
   *
   * @param sn The serial number of the new reference measure
   */
  ControlPoint::Status ControlPoint::SetRefMeasure(iString sn) {
    if (editLock)
      return PointLocked;

    if (!cubeSerials->contains(sn)) {
      iString msg = "Point [" + id + "] has no measure with serial number [" +
          sn + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    referenceExplicitlySet = true;
    referenceMeasure = (*measures)[sn];
    return Success;
  }


  /**
   * Set whether to ignore or use control point
   *
   * @param newIgnoreStatus True to ignore this Control Point, False to
   *                        un-ignore
   */
  ControlPoint::Status ControlPoint::SetIgnored(bool newIgnoreStatus) {
    if (editLock)
      return PointLocked;
    PointModified();
    ignore = newIgnoreStatus;
    return Success;
  }


  /**
   * Set or update the surface point relating to this control point. This is the
   *   point on the surface of the planet that the measures are tied to. This
   *   updates the last modified attributes of this point.
   *
   * @param newSurfacePoint The point on the target's surface the measures are
   *                        tied to
   */
  ControlPoint::Status ControlPoint::SetSurfacePoint(
    SurfacePoint newSurfacePoint) {
    if (editLock)
      return PointLocked;
    PointModified();
    surfacePoint = newSurfacePoint;
    return Success;
  }


  /**
   * Updates the control point's type. This updates the last modified attributes
   *   of this point.
   *
   * @see PointType
   *
   * @param newType The new type this control point should be
   */
  ControlPoint::Status ControlPoint::SetType(PointType newType) {
    if (type != Ground && type != Tie) {
      iString msg = "Invalid Point Enumeration, [" + iString(type) + "], for "
          "Control Point [" + GetId() + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if (editLock)
      return PointLocked;
    PointModified();
    type = newType;
    return Success;
  }


  /**
   * This updates the source of the radius of the apriori surface point.
   *
   * @see RadiusSource::Source
   *
   * @param source Where the radius came from
   */
  ControlPoint::Status ControlPoint::SetAprioriRadiusSource(
    RadiusSource::Source source) {
    if (editLock)
      return PointLocked;
    PointModified();
    aprioriRadiusSource = source;
    return Success;
  }


  /**
   * This updates the filename of the DEM that the apriori radius came from. It
   *   doesn't really make sense to call this unless the RadiusSource is DEM.
   *
   * @see RadiusSource::Source
   *
   * @param source Where the radius came from
   */
  ControlPoint::Status ControlPoint::SetAprioriRadiusSourceFile(
    iString sourceFile) {
    if (editLock)
      return PointLocked;
    PointModified();
    aprioriRadiusSourceFile = sourceFile;
    return Success;
  }


  /**
   * This updates the apriori surface point.
   *
   * @see SetAprioriRadiusSource
   * @see SetAprioriRadiusSourceFile
   * @see SetAprioriPointSource
   * @see SetAprioriPointSourceFile
   * @see aprioriSurfacePoint
   *
   * @param aprioriSP The apriori surface point to remember
   */
  ControlPoint::Status ControlPoint::SetAprioriSurfacePoint(
    SurfacePoint aprioriSP) {
    if (editLock)
      return PointLocked;
    PointModified();
    aprioriSurfacePoint = aprioriSP;
    return Success;
  }


  /**
   * This updates the source of the surface point
   *
   * @see SurfacePointSource::Source
   *
   * @param source Where the surface point came from
   */
  ControlPoint::Status ControlPoint::SetAprioriSurfacePointSource(
    SurfacePointSource::Source source) {
    if (editLock)
      return PointLocked;
    PointModified();
    aprioriSurfacePointSource = source;
    return Success;
  }


  /**
   * This updates the filename of where the apriori surface point came from.
   *
   * @see RadiusSource::Source
   *
   * @param sourceFile Where the surface point came from
   */
  ControlPoint::Status ControlPoint::SetAprioriSurfacePointSourceFile(
    iString sourceFile) {
    if (editLock)
      return PointLocked;
    PointModified();
    aprioriSurfacePointSourceFile = sourceFile;
    return Success;
  }


  /**
   * This method computes the apriori lat/lon for a point.  It computes this
   * by determining the average lat/lon of all the measures.  Note that this
   * does not change held, ignored, or ground points.  Also, it does not
   * use unmeasured or ignored measures when computing the lat/lon.
   * @internal
   *   @history 2008-06-18  Tracie Sucharski/Jeannie Walldren,
   *                               Changed error messages for
   *                               Held/Ground points.
   *   @history 2009-10-13 Jeannie Walldren - Added detail to
   *                               error message.
   *   @history 2010-11-29 Tracie Sucharski - Remove call to ControlMeasure::
   *                               SetMeasuredEphemerisTime, the values were
   *                               never used. so these methods were removed
   *                               from ControlMeasure and the call was removed
   *                               here.
   *   @history 2010-12-02 Debbie A. Cook - Added units to SetRectangular
   *                               calls since default is meters and units
   *                               are km.
   *
   * @return Status Success or PointLocked
   */
  ControlPoint::Status ControlPoint::ComputeApriori() {

    if (editLock)
      return PointLocked;
    // Should we ignore the point altogether?
    if (IsIgnored())
      return Failure;

    PointModified();

    // Don't goof with ground points.  The lat/lon is what it is ... if
    // it exists!
    if (GetType() == Ground) {
      if (!surfacePoint.Valid()) {
        iString msg = "ControlPoint [" + GetId() + "] is a ground point ";
        msg += "and requires x/y/z";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
      // Don't return until after the FocalPlaneMeasures have been set
      //      return;
    }

    double xB = 0.0;
    double yB = 0.0;
    double zB = 0.0;
    int goodMeasures = 0;

    // Loop for each measure and compute the sum of the lat/lon/radii
    for (int j = 0; j < cubeSerials->size(); j++) {
      ControlMeasure *m = GetMeasure(j);
      if (!m->IsMeasured()) {
        // TODO: How do we deal with unmeasured measures
      }
      else if (m->IsIgnored()) {
        // TODO: How do we deal with ignored measures
      }
      else {
        Camera *cam = m->Camera();
        if (cam == NULL) {
          iString msg = "The Camera must be set prior to calculating apriori";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }
        if (cam->SetImage(m->GetSample(), m->GetLine())) {
          goodMeasures++;
          double pB[3];
          cam->Coordinate(pB);
          xB += pB[0];
          yB += pB[1];
          zB += pB[2];

          double x = cam->DistortionMap()->UndistortedFocalPlaneX();
          double y = cam->DistortionMap()->UndistortedFocalPlaneY();
          m->SetFocalPlaneMeasured(x, y);
        }
        else {
          // JAA: Don't stop if we know the lat/lon.  The SetImage may fail
          // but the FocalPlane measures have been set
          if (GetType() == Ground)
            continue;

          // TODO: What do we do
          iString msg = "Cannot compute lat/lon/radius (x/y/z) for "
              "ControlPoint [" + GetId() + "], measure [" +
              m->GetCubeSerialNumber() + "]";
          throw iException::Message(iException::User, msg, _FILEINFO_);

          // m->SetFocalPlaneMeasured(?,?);
        }
      }
    }

    // Don't update the x/y/z for ground points
    if (GetType() == Ground)
      return Success;

    // Did we have any measures?
    if (goodMeasures == 0) {
      iString msg = "ControlPoint [" + GetId() + "] has no measures which "
          "project to lat/lon/radius (x/y/z)";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // Compute the averages
    aprioriSurfacePoint.SetRectangular(
      Displacement((xB / goodMeasures), Displacement::Kilometers),
      Displacement((yB / goodMeasures), Displacement::Kilometers),
      Displacement((zB / goodMeasures), Displacement::Kilometers));

    SetAprioriSurfacePointSource(SurfacePointSource::AverageOfMeasures);
    SetAprioriRadiusSource(RadiusSource::AverageOfMeasures);

    return Success;
  }


  /**
   * This method computes the residuals for a point.
   *
   * @history 2008-07-17 Tracie Sucharski,  Added ptid and measure serial
   *                            number to the unable to map to surface error.
   * @history 2009-12-06 Tracie Sucharski, Renamed from ComputeErrors
   * @history 2010-08-05 Tracie Sucharski, Changed lat/lon/radius to x/y/z
   * @history 2010-12-10 Debbie A. Cook,  Revised error calculation for radar
   *                            because it was always reporting line errors=0.
   */
  ControlPoint::Status ControlPoint::ComputeResiduals() {
    if (editLock)
      return PointLocked;
    if (IsIgnored())
      return Failure;

    PointModified();

    // Loop for each measure to compute the error
    QList<QString> keys = measures->keys();
    for (int j = 0; j < keys.size(); j++) {
      ControlMeasure *m = (*measures)[keys[j]];
      if (m->IsIgnored())
        continue;
      if (!m->IsMeasured())
        continue;

      // TODO:  Should we use crater diameter?
      Camera *cam = m->Camera();
      cam->SetImage(m->GetSample(), m->GetLine());

      double cuSamp;
      double cuLine;
      CameraFocalPlaneMap *fpmap = m->Camera()->FocalPlaneMap();

      if (cam->GetCameraType()  !=  Isis::Camera::Radar) {

        // Map the lat/lon/radius of the control point through the Spice of the
        // measurement sample/line to get the computed sample/line.  This must be
        // done manually because the camera will compute a new time for line scanners,
        // instead of using the measured time.
        double cudx, cudy;
        cam->GroundMap()->GetXY(GetSurfacePoint(), &cudx, &cudy);
        m->SetFocalPlaneComputed(cudx, cudy);

        // Now things get tricky.  We want to produce errors in pixels not mm
        // but some of the camera maps could fail.  One that won't is the
        // FocalPlaneMap which takes x/y to detector s/l.  We will bypass the
        // distortion map and have residuals in undistorted pixels.
        if (!fpmap->SetFocalPlane(m->GetFocalPlaneComputedX(), m->GetFocalPlaneComputedY())) {
          iString msg = "Sanity check #1 for ControlPoint [" + GetId() +
              "], ControlMeasure [" + m->GetCubeSerialNumber() + "]";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
          // This error shouldn't happen but check anyways
        }

        cuSamp = fpmap->DetectorSample();
        cuLine = fpmap->DetectorLine();
      }

      else {
        // For radar we can't map through the current Spice, because y in the
        // focal plane is doppler shift.  Line is calculated from time.  If
        // we hold time and the Spice, we'll get the same sample/line as
        // measured
        double lat = GetSurfacePoint().GetLatitude().GetDegrees();
        double lon = GetSurfacePoint().GetLatitude().GetDegrees();
        double rad = GetSurfacePoint().GetLocalRadius().GetMeters();
        if (!cam->SetUniversalGround(lat, lon, rad)) {
          std::string msg = "ControlPoint [" +
              GetId() + "], ControlMeasure [" +
              m->GetCubeSerialNumber() + "]" +
              " does not map into image";
          throw iException::Message(iException::User, msg, _FILEINFO_);
        }

        cuSamp = cam->Sample();
        cuLine = cam->Line();
      }

      double muSamp;
      double muLine;

      if (cam->GetCameraType()  !=  Isis::Camera::Radar) {
        // Again we will bypass the distortion map and have residuals in undistorted pixels.
        if (!fpmap->SetFocalPlane(m->GetFocalPlaneMeasuredX(), m->GetFocalPlaneMeasuredY())) {
          iString msg = "Sanity check #2 for ControlPoint [" + GetId() +
              "], ControlMeasure [" + m->GetCubeSerialNumber() + "]";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
          // This error shouldn't happen but check anyways
        }
        muSamp = fpmap->DetectorSample();
        muLine = fpmap->DetectorLine();
      }
      else {
        muSamp = m->GetSample();
        muLine = m->GetLine();
      }

      // The units are in detector sample/lines.  We will apply the instrument
      // summing mode to get close to real pixels.  Note however we are in
      // undistorted pixels
      double sampResidual = muSamp - cuSamp;
      double lineResidual = muLine - cuLine;
      m->SetResidual(sampResidual, lineResidual);
    }

    return Success;
  }


  iString ControlPoint::GetChooserName() const {
    if (chooserName != "") {
      return chooserName;
    }
    else {
      return Filename(Application::Name()).Name();
    }
  }


  iString ControlPoint::GetDateTime() const {
    if (dateTime != "") {
      return dateTime;
    }
    else {
      return Application::DateTime();
    }
  }


  bool ControlPoint::IsEditLocked() const {
    return editLock;
  }


  bool ControlPoint::IsRejected() const {
    return jigsawRejected;
  }


  /**
   * Return the Id of the control point
   *
   * @return Control Point Id
   */
  iString ControlPoint::GetId() const {
    return id;
  }


  bool ControlPoint::IsIgnored() const {
    return ignore;
  }


  bool ControlPoint::IsValid() const {
    return !invalid;
  }


  bool ControlPoint::IsInvalid() const {
    return invalid;
  }


  /**
   *  Obtain a string representation of a given PointType
   *
   *  @param type PointType to convert to a string
   *
   *  @returns A string representation of type
   */
  iString ControlPoint::PointTypeToString(PointType pointType) {
    iString str;

    switch (pointType) {
      case Ground:
        str = "Ground";
        break;
      case Tie:
        str = "Tie";
        break;
    }

    return str;
  }

  /**
   * Obtain a string representation of the PointType
   *
   * @return A string representation of the PointType
   */
  iString ControlPoint::GetPointTypeString() const {
    return PointTypeToString(type);
  }

  /**
   *  Obtain a string representation of a given RadiusSource
   *
   *  @param source RadiusSource to convert to string
   *
   *  @returns A string representation of RadiusSource
   */
  iString ControlPoint::RadiusSourceToString(RadiusSource::Source source) {
    iString str;

    switch (source) {
      case RadiusSource::None:
        str = "None";
        break;
      case RadiusSource::User:
        str = "User";
        break;
      case RadiusSource::AverageOfMeasures:
        str = "AverageOfMeasures";
        break;
      case RadiusSource::Ellipsoid:
        str = "Ellipsoid";
        break;
      case RadiusSource::DEM:
        str = "DEM";
        break;
      case RadiusSource::BundleSolution:
        str = "BundleSolution";
        break;
    }

    return str;
  }

  /**
   * Obtain a string representation of the RadiusSource
   *
   * @return A string representation of the RadiusSource
   */
  iString ControlPoint::GetRadiusSourceString() const {
    return RadiusSourceToString(aprioriRadiusSource);
  }

  /**
   *  Obtain a string representation of a given SurfacePointSource
   *
   *  @param souce SurfacePointSource to get a string representation of
   *
   *  @returns A string representation of SurfacePointSource
   */
  iString ControlPoint::SurfacePointSourceToString(SurfacePointSource::Source source) {
    iString str;

    switch (source) {
      case SurfacePointSource::None:
        str = "None";
        break;
      case SurfacePointSource::User:
        str = "User";
        break;
      case SurfacePointSource::AverageOfMeasures:
        str = "AverageOfMeasures";
        break;
      case SurfacePointSource::Reference:
        str = "Reference";
        break;
      case SurfacePointSource::Basemap:
        str = "Basemap";
        break;
      case SurfacePointSource::BundleSolution:
        str = "BundleSolution";
        break;
    }

    return str;
  }

  /**
   * Obtain a string representation of the SurfacePointSource
   *
   * @return A string representation of the SurfacePointSource
   */
  iString ControlPoint::GetSurfacePointSourceString() const {
    return SurfacePointSourceToString(aprioriSurfacePointSource);
  }

  SurfacePoint ControlPoint::GetSurfacePoint() const {
    return surfacePoint;
  }


  ControlPoint::PointType ControlPoint::GetType() const {
    return type;
  }



  bool ControlPoint::IsGround() const {
    return (type == Ground);
  }


  SurfacePoint ControlPoint::GetAprioriSurfacePoint() const {
    return aprioriSurfacePoint;
  }


  ControlPoint::RadiusSource::Source ControlPoint::GetAprioriRadiusSource()
      const {
    return aprioriRadiusSource;
  }


  iString ControlPoint::GetAprioriRadiusSourceFile() const {
    return aprioriRadiusSourceFile;
  }

  ControlPoint::SurfacePointSource::Source
  ControlPoint::GetAprioriSurfacePointSource() const {
    return aprioriSurfacePointSource;
  }


  iString ControlPoint::GetAprioriSurfacePointSourceFile() const {
    return aprioriSurfacePointSourceFile;
  }


  int ControlPoint::GetNumMeasures() const {
    return measures->size();
  }


  /**
   *
   * @return Number of valid control measures
   */
  int ControlPoint::GetNumValidMeasures() const {
    int size = 0;
    QList<QString> keys = measures->keys();
    for (int cm = 0; cm < keys.size(); cm++) {
      if (!(*measures)[keys[cm]]->IsIgnored())
        size++;
    }
    return size;
  }


  /**
   * Returns the number of locked control measures
   *
   * @return Number of locked control measures
   */
  int ControlPoint::GetNumLockedMeasures() const {
    int size = 0;
    QList<QString> keys = measures->keys();
    for (int cm = 0; cm < keys.size(); cm++) {
      if ((*measures)[keys[cm]]->IsEditLocked())
        size++;
    }
    return size;
  }


  /**
   *  Return true if given serial number exists in point
   *
   *  @param serialNumber  The serial number
   *  @return True if point contains serial number, false if not
   */
  bool ControlPoint::HasSerialNumber(iString serialNumber) const {
    return cubeSerials->contains(serialNumber);
  }


  /**
   * @returns true Returns true if SetRefMeasure has ever been set on this
   *               point.
   */
  bool ControlPoint::IsReferenceExplicit() const {
    return referenceExplicitlySet;
  }


  /**
   * @returns The cube serial number of the reference measure
   */
  QString ControlPoint::GetReferenceSN() const {
    if (referenceMeasure == NULL) {
      iString msg = "There is no reference measure set in the ControlPoint [" +
          GetId() + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return referenceMeasure->GetCubeSerialNumber();
  }


  /**
   * @param cm The control measure to find the index of
   * @param throws Throws an exception on failure instead of returning -1.
   *               Be aware that by default this is true!
   *
   * @returns The index of the passed in measure, or -1 on failure if throws
   *          is false.
   */
  int ControlPoint::IndexOf(ControlMeasure *cm, bool throws) const {
    ASSERT(cm);
    return IndexOf(cm->GetCubeSerialNumber(), throws);
  }


  /**
   * @param sn The serial number of the control measure to find the index of
   * @param throws Throws an exception on failure instead of returning -1.
   *               Be aware that by default this is true!
   *
   * @returns The index of the measure with serial number matching sn,
   *          or -1 on failure if throws is false.
   */
  int ControlPoint::IndexOf(iString sn, bool throws) const {
    int index = cubeSerials->indexOf(sn);

    if (throws && index == -1) {
      iString msg = "ControlMeasure [" + sn + "] does not exist in point [" +
          id + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return index;
  }


  /**
   * @param throws Throws an exception on failure instead of returning -1.
   *               Be aware that by default this is true!
   *
   * @returns The index of the reference measure, or -1 if no measures exist
   * in the point (A point with at least one measure ALWAYS has a reference
   * measure.
   */
  int ControlPoint::IndexOfRefMeasure() const {
    if (!referenceMeasure) {
      iString msg = "There is no reference measure for point [" + id + "]."
          "  This also means of course that the point is empty!";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    int index = cubeSerials->indexOf(referenceMeasure->GetCubeSerialNumber());
    ASSERT(index != -1)

    return index;
  }


  /**
   * This function will call a given method on every control measure that
   * this point has.
   *
   * @param statFunc The function to use for data collection
   *
   * @returns The gathered statistic
   */
  Statistics ControlPoint::GetStatistic(
    double(ControlMeasure::*statFunc)() const) const {
    Statistics stats;
    foreach(ControlMeasure * cm, *measures) {
      if (!cm->IsIgnored() && cm->GetType() == ControlMeasure::Candidate)
        stats.AddData((cm->*statFunc)());
    }

    return stats;
  }


  QList< QString > ControlPoint::GetCubeSerialNumbers() const {
    return *cubeSerials;
  }


  /**
   * Creates a PvlObject from the ControlPoint
   *
   * @return The PvlObject created
   *
   */
  PvlObject ControlPoint::ToPvlObject() const {
    PvlObject p("ControlPoint");

    p += PvlKeyword("PointType", GetPointTypeString());

    p += PvlKeyword("PointId", id);
    p += PvlKeyword("ChooserName", GetChooserName());
    p += PvlKeyword("DateTime", GetDateTime());

    if (editLock == true) {
      p += PvlKeyword("EditLock", "True");
    }

    if (ignore == true) {
      p += PvlKeyword("Ignore", "True");
    }

    switch (aprioriSurfacePointSource) {
      case SurfacePointSource::None:
        break;
      case SurfacePointSource::User:
        p += PvlKeyword("AprioriXYZSource", "User");
        break;
      case SurfacePointSource::AverageOfMeasures:
        p += PvlKeyword("AprioriXYZSource", "AverageOfMeasures");
        break;
      case SurfacePointSource::Reference:
        p += PvlKeyword("AprioriXYZSource", "Reference");
        break;
      case SurfacePointSource::Basemap:
        p += PvlKeyword("AprioriXYZSource", "Basemap");
        break;
      case SurfacePointSource::BundleSolution:
        p += PvlKeyword("AprioriXYZSource", "BundleSolution");
        break;
      default:
        break;
    }

    if (!aprioriSurfacePointSourceFile.empty()) {
      p += PvlKeyword("AprioriXYZSourceFile", aprioriSurfacePointSourceFile);
    }

    if (aprioriRadiusSource != RadiusSource::None) {
      p += PvlKeyword("AprioriRadiusSource", GetRadiusSourceString());
    }

    if (!aprioriRadiusSourceFile.empty()) {
      p += PvlKeyword("AprioriRadiusSourceFile", aprioriRadiusSourceFile);
    }

    if (aprioriSurfacePoint.Valid()) {
      const SurfacePoint &apriori = aprioriSurfacePoint;

      p += PvlKeyword("AprioriX", apriori.GetX().GetMeters(), "meters");
      p += PvlKeyword("AprioriY", apriori.GetY().GetMeters(), "meters");
      p += PvlKeyword("AprioriZ", apriori.GetZ().GetMeters(), "meters");

      symmetric_matrix<double, upper> covar = apriori.GetRectangularMatrix();
      if (covar(0, 0) != 0. || covar(1, 1) != 0. || covar(2, 2) != 0.) {
        PvlKeyword matrix("AprioriCovarianceMatrix");
        matrix += covar(0, 0);
        matrix += covar(0, 1);
        matrix += covar(0, 2);
        matrix += covar(1, 1);
        matrix += covar(1, 2);
        matrix += covar(2, 2);
        p += matrix;
      }
    }

    if (surfacePoint.Valid()) {
      const SurfacePoint &point = surfacePoint;

      p += PvlKeyword("X", point.GetX().GetMeters(), "meters");
      p += PvlKeyword("Y", point.GetY().GetMeters(), "meters");
      p += PvlKeyword("Z", point.GetZ().GetMeters(), "meters");

      symmetric_matrix<double, upper> covar = point.GetRectangularMatrix();
      if (covar(0, 0) != 0. || covar(1, 1) != 0. ||
          covar(2, 2) != 0.) {
        PvlKeyword matrix("ApostCovarianceMatrix");
        matrix += covar(0, 0);
        matrix += covar(0, 1);
        matrix += covar(0, 2);
        matrix += covar(1, 1);
        matrix += covar(1, 2);
        matrix += covar(2, 2);
        p += matrix;
      }
    }

    for (int i = 0; i < cubeSerials->size(); i++) {
      p.AddGroup((*measures)[cubeSerials->at(i)]->CreatePvlGroup());

      if(IsReferenceExplicit() &&
         referenceMeasure == (*measures)[cubeSerials->at(i)]) {
        p.Group(p.Groups() - 1).AddKeyword(
            PvlKeyword("Reference", "True"));
      }
    }

    return p;
  }

  /**
   *  Same as GetMeasure (provided for convenience)
   *
   *  @param serialNumber Cube serial number of desired control measure
   *
   *  @returns const version of the measure which has the provided serial number
   */
  const ControlMeasure *ControlPoint::operator[](iString serialNumber) const {
    return GetMeasure(serialNumber);
  }


  /**
   *  Same as GetMeasure (provided for convenience)
   *
   *  @param serialNumber Cube serial number of desired control measure
   *
   *  @returns The measure which has the provided serial number
   */
  ControlMeasure *ControlPoint::operator[](iString serialNumber) {
    return GetMeasure(serialNumber);
  }


  /**
   *  Same as GetMeasure (provided for convenience)
   *
   *  @param index If there are n measures, the measure returned will be the
   *               ith measure added to the point
   *
   *  @returns const version of the measure which has the provided serial number
   */
  const ControlMeasure *ControlPoint::operator[](int index) const {
    return GetMeasure(index);
  }


  /**
   *  Same as GetMeasure (provided for convenience)
   *
   *  @param index If there are n measures, the measure returned will be the
   *               ith measure added to the point
   *
   *  @returns The measure which has the provided serial number
   */
  ControlMeasure *ControlPoint::operator[](int index) {
    return GetMeasure(index);
  }


  /**
   * Compare two Control Points for inequality
   *
   * @param other The other point to compare this one to
   *
   * @returns true if the two points are not equal, and false otherwise
   */
  bool ControlPoint::operator!=(const ControlPoint &other) const {
    return !(*this == other);
  }


  /**
   * Compare two Control Points for equality
   *
   * @param other The other point to compare to
   *
   * @returns true if the two points are equal, and false otherwise
   */
  bool ControlPoint::operator==(const ControlPoint &other) const {
    return other.GetNumMeasures() == GetNumMeasures() &&
        other.id == id &&
        other.type == type &&
        other.chooserName == chooserName &&
        other.editLock == editLock &&
        other.ignore == ignore &&
        other.aprioriSurfacePointSource  == aprioriSurfacePointSource &&
        other.aprioriSurfacePointSourceFile
        == aprioriSurfacePointSourceFile &&
        other.aprioriRadiusSource  == aprioriRadiusSource &&
        other.aprioriRadiusSourceFile  == aprioriRadiusSourceFile &&
        other.aprioriSurfacePoint == aprioriSurfacePoint &&
        other.surfacePoint == surfacePoint &&
        other.invalid == invalid &&
        other.measures == measures;
  }


  /**
   *
   * @param pPoint
   *
   * @return ControlPoint&
   */
  const ControlPoint &ControlPoint::operator=(ControlPoint other) {

    if (measures) {
      QList< QString > keys = measures->keys();
      for (int i = 0; i < keys.size(); i++) {
        delete(*measures)[keys[i]];
        (*measures)[keys[i]] = NULL;
      }

      delete measures;
      measures = NULL;
    }

    if (cubeSerials) {
      delete cubeSerials;
      cubeSerials = NULL;
    }

    measures = new QHash< QString, ControlMeasure * >;
    cubeSerials = new QStringList;

    QHashIterator< QString, ControlMeasure * > i(*other.measures);
    while (i.hasNext()) {
      i.next();
      ControlMeasure *newMeasure = new ControlMeasure;
      *newMeasure = *i.value();
      if (other.referenceMeasure == i.value())
        referenceMeasure = newMeasure;
      QString newSerial = newMeasure->GetCubeSerialNumber();
      newMeasure->parentPoint = this;
      measures->insert(newSerial, newMeasure);
      cubeSerials->append(newSerial);
    }

    if (referenceMeasure == NULL && cubeSerials->size() != 0)
      referenceMeasure = measures->value(cubeSerials->at(0));

    id             = other.id;
    chooserName    = other.chooserName;
    dateTime       = other.dateTime;
    type           = other.type;
    invalid        = other.invalid;
    editLock       = other.editLock;
    jigsawRejected = other.jigsawRejected;
    referenceExplicitlySet = other.referenceExplicitlySet;
    ignore         = other.ignore;
    aprioriSurfacePointSource      = other.aprioriSurfacePointSource;
    aprioriSurfacePointSourceFile  = other.aprioriSurfacePointSourceFile;
    aprioriRadiusSource            = other.aprioriRadiusSource;
    aprioriRadiusSourceFile        = other.aprioriRadiusSourceFile;
    aprioriSurfacePoint            = other.aprioriSurfacePoint;
    surfacePoint = other.surfacePoint;
    numberOfRejectedMeasures = other.numberOfRejectedMeasures;

    return *this;
  }


  void ControlPoint::Init(const PBControlNet_PBControlPoint &protoBufPt) {
    id = protoBufPt.id();
    dateTime = "";
    aprioriSurfacePointSource = SurfacePointSource::None;
    aprioriRadiusSource = RadiusSource::None;

    chooserName = protoBufPt.choosername();
    dateTime = protoBufPt.datetime();
    editLock = protoBufPt.editlock();

    parentNetwork = NULL;

    switch (protoBufPt.type()) {
      case PBControlNet_PBControlPoint_PointType_Tie:
        type = Tie;
        break;
      case PBControlNet_PBControlPoint_PointType_Ground:
        type = Ground;
        break;
    }

    ignore = protoBufPt.ignore();
    jigsawRejected = protoBufPt.jigsawrejected();

    // Read apriori keywords
    if (protoBufPt.has_apriorixyzsource()) {
      switch (protoBufPt.apriorixyzsource()) {
        case PBControlNet_PBControlPoint_AprioriSource_None:
          aprioriSurfacePointSource = SurfacePointSource::None;
          break;

        case PBControlNet_PBControlPoint_AprioriSource_User:
          aprioriSurfacePointSource = SurfacePointSource::User;
          break;

        case PBControlNet_PBControlPoint_AprioriSource_AverageOfMeasures:
          aprioriSurfacePointSource = SurfacePointSource::AverageOfMeasures;
          break;

        case PBControlNet_PBControlPoint_AprioriSource_Reference:
          aprioriSurfacePointSource = SurfacePointSource::Reference;
          break;

        case PBControlNet_PBControlPoint_AprioriSource_Basemap:
          aprioriSurfacePointSource = SurfacePointSource::Basemap;
          break;

        case PBControlNet_PBControlPoint_AprioriSource_BundleSolution:
          aprioriSurfacePointSource = SurfacePointSource::BundleSolution;
          break;

        case PBControlNet_PBControlPoint_AprioriSource_Ellipsoid:
        case PBControlNet_PBControlPoint_AprioriSource_DEM:
          break;
      }
    }

    if (protoBufPt.has_apriorixyzsourcefile()) {
      aprioriSurfacePointSourceFile = protoBufPt.apriorixyzsourcefile();
    }

    if (protoBufPt.has_aprioriradiussource()) {
      switch (protoBufPt.aprioriradiussource()) {
        case PBControlNet_PBControlPoint_AprioriSource_None:
          aprioriRadiusSource = RadiusSource::None;
          break;
        case PBControlNet_PBControlPoint_AprioriSource_User:
          aprioriRadiusSource = RadiusSource::User;
          break;
        case PBControlNet_PBControlPoint_AprioriSource_AverageOfMeasures:
          aprioriRadiusSource = RadiusSource::AverageOfMeasures;
          break;
        case PBControlNet_PBControlPoint_AprioriSource_Ellipsoid:
          aprioriRadiusSource = RadiusSource::Ellipsoid;
          break;
        case PBControlNet_PBControlPoint_AprioriSource_DEM:
          aprioriRadiusSource = RadiusSource::DEM;
          break;
        case PBControlNet_PBControlPoint_AprioriSource_BundleSolution:
          aprioriRadiusSource = RadiusSource::BundleSolution;
          break;

        case PBControlNet_PBControlPoint_AprioriSource_Reference:
        case PBControlNet_PBControlPoint_AprioriSource_Basemap:
          break;
      }
    }

    if (protoBufPt.has_aprioriradiussourcefile()) {
      aprioriRadiusSourceFile = protoBufPt.aprioriradiussourcefile();
    }

    if (protoBufPt.has_apriorix() && protoBufPt.has_aprioriy() &&
        protoBufPt.has_aprioriz()) {
      SurfacePoint apriori(
        Displacement(protoBufPt.apriorix(), Displacement::Meters),
        Displacement(protoBufPt.aprioriy(), Displacement::Meters),
        Displacement(protoBufPt.aprioriz(), Displacement::Meters));

      if (protoBufPt.aprioricovar_size() > 0) {
        symmetric_matrix<double, upper> covar;
        covar.resize(3);
        covar.clear();
        covar(0, 0) = protoBufPt.aprioricovar(0);
        covar(0, 1) = protoBufPt.aprioricovar(1);
        covar(0, 2) = protoBufPt.aprioricovar(2);
        covar(1, 1) = protoBufPt.aprioricovar(3);
        covar(1, 2) = protoBufPt.aprioricovar(4);
        covar(2, 2) = protoBufPt.aprioricovar(5);
        apriori.SetRectangularMatrix(covar);
      }

      aprioriSurfacePoint = apriori;
    }

    if (protoBufPt.has_x() && protoBufPt.has_y() && protoBufPt.has_z()) {
      SurfacePoint apost(Displacement(protoBufPt.x(), Displacement::Meters),
          Displacement(protoBufPt.y(), Displacement::Meters),
          Displacement(protoBufPt.z(), Displacement::Meters));

      if (protoBufPt.apostcovar_size() > 0) {
        symmetric_matrix<double, upper> covar;
        covar.resize(3);
        covar.clear();
        covar(0, 0) = protoBufPt.aprioricovar(0);
        covar(0, 1) = protoBufPt.aprioricovar(1);
        covar(0, 2) = protoBufPt.aprioricovar(2);
        covar(1, 1) = protoBufPt.aprioricovar(3);
        covar(1, 2) = protoBufPt.aprioricovar(4);
        covar(2, 2) = protoBufPt.aprioricovar(5);
        apost.SetRectangularMatrix(covar);
      }

      surfacePoint = apost;
    }
  }


  void ControlPoint::PointModified() {
    dateTime = "";
  }


  //! Initialize the number of rejected measures to 0
  void ControlPoint::ZeroNumberOfRejectedMeasures()

  {
    numberOfRejectedMeasures = 0;
  }


  /**
   * Set (update) the number of rejected measures for the control point
   *
   * @param numRejected    The number of rejected measures
   *
   */
  void ControlPoint::SetNumberOfRejectedMeasures(int numRejected) {
    numberOfRejectedMeasures = numRejected;
  }


  /**
   * Get the number of rejected measures on the control point
   *
   * @return The number of rejected measures on this control point
   *
   */
  int ControlPoint::GetNumberOfRejectedMeasures() const {
    return numberOfRejectedMeasures;
  }


  PBControlNet_PBControlPoint ControlPoint::ToProtocolBuffer() const {
    PBControlNet_PBControlPoint pbPoint;

    pbPoint.set_id(GetId());
    switch (GetType()) {
      case ControlPoint::Tie:
        pbPoint.set_type(PBControlNet_PBControlPoint::Tie);
        break;
      case ControlPoint::Ground:
        pbPoint.set_type(PBControlNet_PBControlPoint::Ground);
        break;
    }

    if (!GetChooserName().empty()) {
      pbPoint.set_choosername(GetChooserName());
    }
    if (!GetDateTime().empty()) {
      pbPoint.set_datetime(GetDateTime());
    }
    if (IsEditLocked())
      pbPoint.set_editlock(true);
    if (IsIgnored())
      pbPoint.set_ignore(true);
    if (IsRejected())
      pbPoint.set_jigsawrejected(true);

    if (referenceMeasure && referenceExplicitlySet) {
      pbPoint.set_referenceindex(IndexOfRefMeasure());
    }

    switch (GetAprioriSurfacePointSource()) {
      case ControlPoint::SurfacePointSource::None:
        break;
      case ControlPoint::SurfacePointSource::User:
        pbPoint.set_apriorixyzsource(PBControlNet_PBControlPoint_AprioriSource_User);
        break;
      case ControlPoint::SurfacePointSource::AverageOfMeasures:
        pbPoint.set_apriorixyzsource(PBControlNet_PBControlPoint_AprioriSource_AverageOfMeasures);
        break;
      case ControlPoint::SurfacePointSource::Reference:
        pbPoint.set_apriorixyzsource(PBControlNet_PBControlPoint_AprioriSource_Reference);
        break;
      case ControlPoint::SurfacePointSource::Basemap:
        pbPoint.set_apriorixyzsource(PBControlNet_PBControlPoint_AprioriSource_Basemap);
        break;
      case ControlPoint::SurfacePointSource::BundleSolution:
        pbPoint.set_apriorixyzsource(PBControlNet_PBControlPoint_AprioriSource_BundleSolution);
        break;
      default:
        break;
    }
    if (!GetAprioriSurfacePointSourceFile().empty()) {
      pbPoint.set_apriorixyzsourcefile(GetAprioriSurfacePointSourceFile());
    }
    switch (GetAprioriRadiusSource()) {
      case ControlPoint::RadiusSource::None:
        break;
      case ControlPoint::RadiusSource::User:
        pbPoint.set_aprioriradiussource(PBControlNet_PBControlPoint_AprioriSource_User);
        break;
      case ControlPoint::RadiusSource::AverageOfMeasures:
        pbPoint.set_aprioriradiussource(PBControlNet_PBControlPoint_AprioriSource_AverageOfMeasures);
        break;
      case ControlPoint::RadiusSource::Ellipsoid:
        pbPoint.set_aprioriradiussource(PBControlNet_PBControlPoint_AprioriSource_Ellipsoid);
        break;
      case ControlPoint::RadiusSource::DEM:
        pbPoint.set_aprioriradiussource(PBControlNet_PBControlPoint_AprioriSource_DEM);
        break;
      case ControlPoint::RadiusSource::BundleSolution:
        pbPoint.set_aprioriradiussource(PBControlNet_PBControlPoint_AprioriSource_BundleSolution);
        break;
      default:
        break;
    }
    if (!GetAprioriRadiusSourceFile().empty()) {
      pbPoint.set_aprioriradiussourcefile(GetAprioriRadiusSourceFile());
    }

    if (GetAprioriSurfacePoint().Valid()) {
      SurfacePoint apriori = GetAprioriSurfacePoint();
      pbPoint.set_apriorix(apriori.GetX().GetMeters());
      pbPoint.set_aprioriy(apriori.GetY().GetMeters());
      pbPoint.set_aprioriz(apriori.GetZ().GetMeters());

      symmetric_matrix< double, upper > covar = apriori.GetRectangularMatrix();
      if (covar(0, 0) != 0. || covar(0, 1) != 0. ||
          covar(0, 2) != 0. || covar(1, 1) != 0. ||
          covar(1, 2) != 0. || covar(2, 2) != 0.) {
        pbPoint.add_aprioricovar(covar(0, 0));
        pbPoint.add_aprioricovar(covar(0, 1));
        pbPoint.add_aprioricovar(covar(0, 2));
        pbPoint.add_aprioricovar(covar(1, 1));
        pbPoint.add_aprioricovar(covar(1, 2));
        pbPoint.add_aprioricovar(covar(2, 2));
      }
    }


    if (GetSurfacePoint().Valid()) {
      SurfacePoint apost = GetSurfacePoint();
      pbPoint.set_x(apost.GetX().GetMeters());
      pbPoint.set_y(apost.GetY().GetMeters());
      pbPoint.set_z(apost.GetZ().GetMeters());

      symmetric_matrix< double, upper > covar = apost.GetRectangularMatrix();
      if (covar(0, 0) != 0. || covar(0, 1) != 0. ||
          covar(0, 2) != 0. || covar(1, 1) != 0. ||
          covar(1, 2) != 0. || covar(2, 2) != 0.) {
        pbPoint.add_apostcovar(covar(0, 0));
        pbPoint.add_apostcovar(covar(0, 1));
        pbPoint.add_apostcovar(covar(0, 2));
        pbPoint.add_apostcovar(covar(1, 1));
        pbPoint.add_apostcovar(covar(1, 2));
        pbPoint.add_apostcovar(covar(2, 2));
      }
    }

    //  Process all measures in the point
    for (int i = 0; i < cubeSerials->size(); i++)
      *pbPoint.add_measures() = (*measures)[cubeSerials->at(i)]->ToProtocolBuffer();

    return pbPoint;
  }


  PBControlNetLogData_Point ControlPoint::GetLogProtocolBuffer() const {
    PBControlNetLogData_Point protoBufLog;

    for (int i = 0; i < cubeSerials->size(); i++)
      *protoBufLog.add_measures() =
        (*measures)[cubeSerials->at(i)]->GetLogProtocolBuffer();

    return protoBufLog;
  }
}
