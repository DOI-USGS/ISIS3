#include "ControlPoint.h"
#include "SpecialPixel.h"
#include "CameraFocalPlaneMap.h"
#include "CameraDistortionMap.h"
#include "CameraDetectorMap.h"
#include "iString.h"
#include "PvlObject.h"
#include "ControlMeasure.h"
#include "CameraGroundMap.h"
#include "SerialNumberList.h"
#include "Cube.h"

namespace Isis {
  /**
   * Construct a control point 
   *
   */
  ControlPoint::ControlPoint() : p_invalid(false) {
    SetId("");
    SetType(Tie);
    SetUniversalGround(Isis::Null, Isis::Null, Isis::Null);
    SetIgnore(false);
    SetHeld(false);
  }

  /**
   * Construct a control point with given Id
   *
   * @param id Control Point Id
   */
  ControlPoint::ControlPoint(const std::string &id) : p_invalid(false) {
    SetId(id);
    SetType(Tie);
    SetUniversalGround(Isis::Null, Isis::Null, Isis::Null);
    SetIgnore(false);
    SetHeld(false);
  }

  /**
   * Loads the PvlObject into a ControlPoint
   *
   * @param p PvlObject containing ControlPoint information
   * @param forceBuild Forces invalid Control Measures to be added to this Control
   *                   Point
   *
   * @throws Isis::iException::User - Invalid Point Type
   * @throws Isis::iException::User - Unable to add ControlMeasure to ControlPoint
   *
   * @history 2008-06-18  Tracie Sucharski/Jeannie Walldren, Fixed bug with
   *                              checking for "True" vs "true", change to
   *                              lower case for comparison.
   */
  void ControlPoint::Load(PvlObject &p, bool forceBuild) {
    SetId(p["PointId"]);
    if(p.HasKeyword("Latitude")) {
      SetUniversalGround(p["Latitude"], p["Longitude"], p["Radius"]);
    }
    if((std::string)p["PointType"] == "Ground") SetType(Ground);
    else if((std::string)p["PointType"] == "Tie") SetType(Tie);
    else {
      std::string msg = "Invalid Point Type, [" + (std::string)p["PointType"] + "]";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    if(p.HasKeyword("Held")) {
      iString held = (std::string)p["Held"];
      if(held.DownCase() == "true") SetHeld(true);
    }
    if(p.HasKeyword("Ignore")) {
      iString ignore = (std::string)p["Ignore"];
      if(ignore.DownCase() == "true") SetIgnore(true);
    }
    for(int g = 0; g < p.Groups(); g++) {
      try {
        if(p.Group(g).IsNamed("ControlMeasure")) {
          ControlMeasure cm;
          cm.Load(p.Group(g));
          Add(cm, forceBuild);
        }
      }
      catch(iException &e) {
        std::string msg = "Unable to add Control Measure to ControlPoint [" +
                          Id() + "]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
  }

  /**
   * Creates a PvlObject from the ControlPoint
   *
   * @return The PvlObject created
   *
   * @throws Isis::iException::Programmer - Invalid Point Enumeration
   * @internal
   *   @history 2009-10-13 Jeannie Walldren - Added detail to
   *            error message.
   */
  PvlObject ControlPoint::CreatePvlObject() {
    PvlObject p("ControlPoint");
    if(p_type == Ground) {
      p += PvlKeyword("PointType", "Ground");
    }
    else if(p_type == Tie) {
      p += PvlKeyword("PointType", "Tie");
    }
    else {
      std::string msg = "Invalid Point Enumeration, [" + iString(p_type) + "] for ControlPoint [" + Id() + "].";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    p += PvlKeyword("PointId", Id());
    if(p_latitude != Isis::Null && p_longitude != Isis::Null && p_radius != Isis::Null) {
      p += PvlKeyword("Latitude", p_latitude);
      p += PvlKeyword("Longitude", p_longitude);
      p += PvlKeyword("Radius", p_radius);
    }
    if(p_held == true) {
      p += PvlKeyword("Held", "True");
    }
    if(p_ignore == true) {
      p += PvlKeyword("Ignore", "True");
    }

    for(int g = 0; g < Size(); g++) {
      p.AddGroup(this->operator[](g).CreatePvlGroup());
    }

    return p;
  }

  /**
   * Add a measurement to the control point
   *
   * @param measure The ControlMeasure to add
   * @param forceBuild Forces the Control Measure to be added reguardless of
   *                   validity
   * @internal
   *   @history 2009-10-13 Jeannie Walldren - Added detail to
   *            error message.
   */
  void ControlPoint::Add(const ControlMeasure &measure, bool forceBuild) {
    for(int i = 0; i < Size(); i++) {
      if(this->operator[](i).CubeSerialNumber() == measure.CubeSerialNumber()) {
        if(forceBuild) {
          p_invalid |= true;
          break;
        }
        else {
          std::string msg = "The SerialNumber is not unique. A measure with serial number [";
          msg += measure.CubeSerialNumber() + "] already exists for ControlPoint [" + Id() + "].";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }
      }
    }
    p_measures.push_back(measure);
  }

  /**
   * Remove a measurement from the control point
   *
   * @param index The index of the control point to delete
   */
  void ControlPoint::Delete(int index) {
    p_measures.erase(p_measures.begin() + index);

    // Check if the control point is still invalid or not
    if(p_invalid) {
      p_invalid = false;
      for(int i = 0; i < Size() && !p_invalid; i++) {
        for(int j = i + 1; j < Size() && !p_invalid; j++) {
          if(this->operator[](i).CubeSerialNumber() == this->operator[](j).CubeSerialNumber()) {
            p_invalid = true;
          }
        }
      }

    }
  }

  /**
   *  Return the measurement for the given serial number
   *
   *  @param serialNumber The serial number
   *
   *  @return The ControlMeasure corresponding to the give serial number
   * @internal
   *   @history 2009-10-13 Jeannie Walldren - Added detail to
   *            error message.
   */
  ControlMeasure &ControlPoint::operator[](const std::string &serialNumber) {
    for(int m = 0; m < this->Size(); m++) {
      if(this->operator[](m).CubeSerialNumber() == serialNumber) {
        return this->operator [](m);
      }
    }
    std::string msg = "Requested measurement serial number [" + serialNumber + "] ";
    msg += "does not exist in ControlPoint [" + Id() + "].";
    throw iException::Message(iException::User, msg, _FILEINFO_);

  }

  /**
   *  Return the measurement for the given serial number
   *
   *  @param serialNumber The serial number
   *
   *  @return The ControlMeasure corresponding to the give serial number
   * @internal
   *   @history 2009-10-13 Jeannie Walldren - Added detail to
   *            error message.
   */
  const ControlMeasure &ControlPoint::operator[](const std::string &serialNumber) const {
    for(int m = 0; m < this->Size(); m++) {
      if(this->operator[](m).CubeSerialNumber() == serialNumber) {
        return this->operator [](m);
      }
    }
    std::string msg = "Requested measurement serial number [" + serialNumber + "] ";
    msg += "does not exist in ControlPoint [" + Id() + "].";
    throw iException::Message(iException::User, msg, _FILEINFO_);

  }

  /**
   *  Return true if given serial number exists in point
   *
   *  @param serialNumber  The serial number
   *  @return True if point contains serial number, false if not
   */
  bool ControlPoint::HasSerialNumber(std::string &serialNumber) const {
    for(int m = 0; m < this->Size(); m++) {
      if(this->operator[](m).CubeSerialNumber() == serialNumber) {
        return true;
      }
    }
    return false;

  }


  /**
   *  Obtain a string representation of a given PointType
   *
   *  @returns A string representation of type
   *
   *  @throws iException::Programmer When unable to translate type
   * @internal
   *   @history 2009-10-13 Jeannie Walldren - Added detail to
   *            error message.
   *   @history 2010-06-04 Eric Hyer - removed parameter
   */
  const std::string ControlPoint::PointTypeToString() const {
    std::string str = "";
    switch(p_type) {
      case Ground:
        str = "Ground";
        break;
      case Tie:
        str = "Tie";
        break;
      default:
        str = "Unable to translate PointType [" + iString(p_type)
              + "] inside PointTypeToString for ControlPoint [" + Id() + "].";
        throw iException::Message(iException::Programmer, str, _FILEINFO_);
    }

    return str;
  }


  /**
   * Set the ground coordinate of a control point
   *
   * @param lat     planetocentric latitude in degrees
   * @param lon     planetocentric longitude in degrees
   * @param radius  radius at coordinate in meters
   */
  void ControlPoint::SetUniversalGround(double lat, double lon, double radius) {
    p_latitude = lat;
    p_longitude = lon;
    p_radius = radius;
  }

  //! Return the average error of all measurements
  double ControlPoint::AverageError() const {
    double cerr = 0.0;
    int count = 0;
    for(int i = 0; i < (int)p_measures.size(); i++) {
      if(p_measures[i].Ignore()) continue;
      if(p_measures[i].Type() == ControlMeasure::Unmeasured) continue;
      cerr += p_measures[i].ErrorMagnitude();
      count++;
    }

    if(count == 0) return 0.0;
    return cerr / (double) count;
  }


  /**
   * Return true if there is a Reference measure, otherwise return false
   *
   * @todo  ??? Check for more than one reference measure ???
   *          Should print error, this check should also go in
   *          ReferenceIndex.
   */
  bool ControlPoint::HasReference() {

    if(p_measures.size() == 0) {
      std::string msg = "There are no ControlMeasures in the ControlPoint [" + Id() + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // Return true if reference measure is found
    for(unsigned int i = 0; i < p_measures.size(); i++) {
      if(p_measures[i].IsReference()) return true;
    }
    return false;
  }

  /**
  * Return the index of the reference measurement
  * if none is specified, return the first measured CM
  *
  * @return The PvlObject created
  */
  int ControlPoint::ReferenceIndex() {
    if(p_measures.size() == 0) {
      std::string msg = "There are no ControlMeasures in the ControlPoint [" + Id() + "]";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    // Return the first ControlMeasure that is a reference
    for(unsigned int i = 0; i < p_measures.size(); i++) {
      if(p_measures[i].IsReference()) return i;
    }

    // Or return the first measured ControlMeasure
    for(unsigned int i = 0; i < p_measures.size(); i++) {
      if(p_measures[i].IsMeasured()) return i;
    }

    std::string msg = "There are no Measured ControlMeasures in the ControlPoint [" + Id() + "]";
    throw iException::Message(iException::Programmer, msg, _FILEINFO_);
  }

  /**
   * Returns a Reference Index of the Control Point. If none then returns the
   * first measure as Reference. If there are no measures then returns -1;
   *
   * @author Sharmila Prasad (5/11/2010)
   *
   * @return int
   */
  int ControlPoint::ReferenceIndexNoException(void) {
    if(p_measures.size() == 0) {
      return -1;
    }

    // Return the first ControlMeasure that is a reference
    for(unsigned int i = 0; i < p_measures.size(); i++) {
      if(p_measures[i].IsReference()) return i;
    }

    return 0;
  }
  
  /**
   * Returns the Universal Latitude of the Reference Measure
   * Returns Isis::Null if Camera is NULL 
   *  
   * @author Sharmila Prasad (8/31/2010)
   * 
   * @param pCamera 
   * 
   * @return double 
   */
  double ControlPoint::LatitudeByReference(Camera *pCamera)
  {    
    if(pCamera != NULL) {
      ControlMeasure & cMeasure = p_measures[ReferenceIndex()];
      pCamera->SetImage(cMeasure.Sample(), cMeasure.Line());
      return pCamera->UniversalLatitude();
    }
    return Isis::Null;
  }
  
  /**
   * Returns the Universal Longitude of the Reference Measure. 
   * Returns Isis::Null if Camera is NULL 
   * 
   * @author Sharmila Prasad (8/31/2010)
   * 
   * @param pCamera 
   * 
   * @return double 
   */
  double ControlPoint::LongitudeByReference(Camera *pCamera)
  {    
    if(pCamera != NULL) {
      ControlMeasure & cMeasure = p_measures[ReferenceIndex()];
      pCamera->SetImage(cMeasure.Sample(), cMeasure.Line());
      return pCamera->UniversalLongitude();
    }
    return Isis::Null;
  }

  /**
   * Returns the Radius of the Reference Measure. 
   * Returns Isis::Null if Camera is NULL 
   * 
   * @author Sharmila Prasad (8/31/2010)
   * 
   * @param pCamera 
   * 
   * @return double 
   */
  double ControlPoint::RadiusByReference(Camera *pCamera)
  {    
    if(pCamera != NULL) {
      ControlMeasure & cMeasure = p_measures[ReferenceIndex()];
      pCamera->SetImage(cMeasure.Sample(), cMeasure.Line());
      return pCamera->LocalRadius();
    }
    return Isis::Null;
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
   *            error message.
   */
  void ControlPoint::ComputeApriori() {
    // Should we ignore the point altogether?
    if(Ignore()) return;

    // Don't goof with ground points.  The lat/lon is what it is ... if
    // it exists!
    if(Type() == Ground) {
      if(p_latitude == Isis::Null ||
          p_longitude == Isis::Null ||
          p_radius == Isis::Null) {
        std::string msg = "ControlPoint [" + Id() + "] is a ground point ";
        msg += "and requires lat/lon/radius";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
      // Don't return until after the FocalPlaneMeasures have been set
      //      return;
    }

    // A held point is basically a ground point.  So don't mess with it either
    if(Held()) {
      if(p_latitude == Isis::Null &&
          p_longitude == Isis::Null &&
          p_radius == Isis::Null) {
        std::string msg = "ControlPoint [" + Id() + "] is held and ";
        msg += "requires lat/lon/radius";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
      // Don't return until after the FocalPlaneMeasures have been set
      //      return;
    }

    double lat = 0.0;
    double lon = 0.0;
    double rad = 0.0;
    int goodMeasures = 0;
    double baselon = 180.;

    // Loop for each measure and compute the sum of the lat/lon/radii
    for(int j = 0; j < (int)p_measures.size(); j++) {
      ControlMeasure &m = p_measures[j];
      if(m.Type() == ControlMeasure::Unmeasured) {
        // TODO: How do we deal with unmeasured measures
      }
      else if(m.Ignore()) {
        // TODO: How do we deal with ignored measures
      }
      else {
        Camera *cam = m.Camera();
        if(cam == NULL) {
          std::string msg = "The Camera must be set prior to calculating apriori";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }
        if(cam->SetImage(m.Sample(), m.Line())) {
          goodMeasures++;
          lat += cam->UniversalLatitude();

          // Deal with longitude wrapping
          double wraplon = WrapLongitude(cam->UniversalLongitude(), baselon);
          lon += wraplon;
          baselon = wraplon;
          rad += cam->LocalRadius();
          double x = cam->DistortionMap()->UndistortedFocalPlaneX();
          double y = cam->DistortionMap()->UndistortedFocalPlaneY();
          m.SetFocalPlaneMeasured(x, y);
          m.SetMeasuredEphemerisTime(cam->EphemerisTime());
        }
        else {
          // JAA: Don't stop if we know the lat/lon.  The SetImage may fail
          // but the FocalPlane measures have been set
          if(Type() == ControlPoint::Ground || Held()) continue;

          // TODO: What do we do
          std::string msg = "Cannot compute lat/lon for ControlPoint [" +
                            Id() + "], measure [" + m.CubeSerialNumber() + "]";
          throw iException::Message(iException::User, msg, _FILEINFO_);

          // m.SetFocalPlaneMeasured(?,?);
        }
      }
    }

    // Don't update the lat/lon for held or ground points
    if(Held()) return;
    if(Type() == ControlPoint::Ground) return;

    // Did we have any measures?
    if(goodMeasures == 0) {
      std::string msg = "ControlPoint [" + Id() + "] has no measures which ";
      msg += "project to latitude/longitude";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // Compute the averages
    lat = lat / goodMeasures;
    lon = lon / goodMeasures;
    if(lon < 0)  lon += 360.;
    rad = rad / goodMeasures;

    SetUniversalGround(lat, lon, rad);
  }


  /**
   * This method computes the errors for a point.
   *
   * @history 2008-07-17  Tracie Sucharski,  Added ptid and measure serial
   *                            number to the unable to map to surface error.
   */

  void ControlPoint::ComputeErrors() {
    if(Ignore()) return;

    double lat = UniversalLatitude();
    double lon = UniversalLongitude();
    double rad = Radius();

    // Loop for each measure to compute the error
    for(int j = 0; j < (int)p_measures.size(); j++) {
      ControlMeasure &m = p_measures[j];
      if(m.Ignore()) continue;
      if(m.Type() == ControlMeasure::Unmeasured) continue;

      // TODO:  Should we use crater diameter?
      Camera *cam = m.Camera();
      cam->SetImage(m.Sample(), m.Line());
      // Map the lat/lon/radius of the control point through the Spice of the
      // measurement sample/line to get the computed sample/line.  This must be
      // done manually because the camera will compute a new time for line scanners,
      // instead of using the measured time.
      // First compute the look vector in body-fixed coordinates
      std::vector<double> look(3);
      double cudx, cudy;
      cam->GroundMap()->GetXY(lat, lon, rad, &cudx, &cudy);
      m.SetFocalPlaneComputed(cudx, cudy);

      CameraFocalPlaneMap *fpmap = m.Camera()->FocalPlaneMap();

      if(cam->GetCameraType()  !=  Isis::Camera::Radar) {
        // Now things get tricky.  We want to produce errors in pixels not mm
        // but some of the camera maps could fail.  One that won't is the
        // FocalPlaneMap which takes x/y to detector s/l.  We will bypass the
        // distortion map and have residuals in undistorted pixels.
        if(!fpmap->SetFocalPlane(m.FocalPlaneComputedX(), m.FocalPlaneComputedY())) {
          std::string msg = "Sanity check #1 for ControlPoint [" +
                            Id() + "], ControlMeasure [" + m.CubeSerialNumber() + "]";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
          // This error shouldn't happen but check anyways
        }
      }

      else {
        // For radar, we can't skip the "distortion map" because it really converts
        //  slant range to ground range.
        // Convert slant range/ doppler shift x/y to ground range x/y
        m.Camera()->DistortionMap()->SetUndistortedFocalPlane(cudx, cudy);

        // Convert ground range x/y to detector position
        double focalPlaneX = m.Camera()->DistortionMap()->FocalPlaneX();
        double focalPlaneY = m.Camera()->DistortionMap()->FocalPlaneY();

        if(!fpmap->SetFocalPlane(focalPlaneX, focalPlaneY)) {
          std::string msg = "Sanity check #1 for ControlPoint [" +
                            Id() + "], ControlMeasure [" + m.CubeSerialNumber() + "]";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }
      }
      double cuSamp = fpmap->DetectorSample();
      double cuLine = fpmap->DetectorLine();

      if(cam->GetCameraType()  !=  Isis::Camera::Radar) {
        // Again we will bypass the distortion map and have residuals in undistorted pixels.
        if(!fpmap->SetFocalPlane(m.FocalPlaneMeasuredX(), m.FocalPlaneMeasuredY())) {
          std::string msg = "Sanity check #2 for ControlPoint [" +
                            Id() + "], ControlMeasure [" + m.CubeSerialNumber() + "]";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
          // This error shouldn't happen but check anyways
        }
      }
      else {
        // In the radar case we can't skip this step since it is really converting slant range to ground range
        m.Camera()->DistortionMap()->SetUndistortedFocalPlane(m.FocalPlaneMeasuredX(), m.FocalPlaneMeasuredY());
        double focalPlaneX = m.Camera()->DistortionMap()->FocalPlaneX();
        double focalPlaneY = m.Camera()->DistortionMap()->FocalPlaneY();

        if(!fpmap->SetFocalPlane(focalPlaneX, focalPlaneY)) {
          std::string msg = "Sanity check #2 for ControlPoint [" +
                            Id() + "], ControlMeasure [" + m.CubeSerialNumber() + "]";
          throw iException::Message(iException::Programmer, msg, _FILEINFO_);
        }
      }
      double muSamp = fpmap->DetectorSample();
      double muLine = fpmap->DetectorLine();

      // The units are in detector sample/lines.  We will apply the instrument
      // summing mode to get close to real pixels.  Note however we are in
      // undistorted pixels
      CameraDetectorMap *cdmap = m.Camera()->DetectorMap();
      double sampError = muSamp - cuSamp;
      double lineError = muLine - cuLine;

      if(cam->GetCameraType()  != Isis::Camera::Radar) {
        sampError /= cdmap->SampleScaleFactor();
        lineError /=   cdmap->LineScaleFactor();
      }
      m.SetError(sampError, lineError);
    }
    return;
  }

  /**
   * Return the maximum error magnitude of the measures in the point.
   * Ignored and unmeasured measures will not be included.
   */
  double ControlPoint::MaximumError() const {
    double maxError = 0.0;
    if(Ignore()) return maxError;

    for(int j = 0; j < (int) p_measures.size(); j++) {
      if(p_measures[j].Ignore()) continue;
      if(p_measures[j].Type() == ControlMeasure::Unmeasured) continue;

      double dErr = p_measures[j].ErrorMagnitude(); 
      if(dErr > maxError) {
        maxError = dErr;
      }
    }
    return maxError;
  }

  /**
   * Return the minimum error magnitude of the measures in the point.
   * Ignored and Unmeasured measures will not be included
   * 
   * @author Sharmila Prasad (8/26/2010)
   * 
   * @return double 
   */
  double ControlPoint::MinimumError() const
  {
    double dMinError = VALID_MAX4;
    if(Ignore()) return dMinError;

    for(int j = 0; j < (int) p_measures.size(); j++) {
      if(p_measures[j].Ignore()) continue;
      if(p_measures[j].Type() == ControlMeasure::Unmeasured) continue;

      double dErr = p_measures[j].ErrorMagnitude();
      if(dErr < dMinError) {
        dMinError = dErr;
      }
    }
    return dMinError;
  }

  /**
   * Get the Minimum ErrorLine for the Control Point
   * 
   * @author Sharmila Prasad (8/26/2010)
   * 
   * @return double 
   */
  double ControlPoint::MinimumErrorLine()
  {
    double dMinError = VALID_MAX4;
    if(Ignore()) return dMinError;

    for(int j = 0; j < (int) p_measures.size(); j++) {
      if(p_measures[j].Ignore()) continue;
      if(p_measures[j].Type() == ControlMeasure::Unmeasured) continue;
      
      double dErr = p_measures[j].LineError();
      if(dErr < dMinError) {
        dMinError = dErr;
      }
    }
    return dMinError;
  }
  
  /**
   * Get the Minimum ErrorSample for the Control Point
   * 
   * @author Sharmila Prasad (8/26/2010)
   * 
   * @return double 
   */
  double ControlPoint::MinimumErrorSample()
  {
    double dMinError = VALID_MAX4;
    if(Ignore()) return dMinError;

    for(int j = 0; j < (int) p_measures.size(); j++) {
      if(p_measures[j].Ignore()) continue;
      if(p_measures[j].Type() == ControlMeasure::Unmeasured) continue;
      
      double dErr = p_measures[j].SampleError();
      if(dErr < dMinError) {
        dMinError = dErr;
      }
    }
    return dMinError;
  }
      
  /**
   * Get the Maximum ErrorLine for the Control Point
   * 
   * @author Sharmila Prasad (8/26/2010)
   * 
   * @return double 
   */
  double ControlPoint::MaximumErrorLine()
  {
    double dMaxError = 0.0;
    if(Ignore()) return dMaxError;

    for(int j = 0; j < (int) p_measures.size(); j++) {
      if(p_measures[j].Ignore()) continue;
      if(p_measures[j].Type() == ControlMeasure::Unmeasured) continue;

      double dErr = p_measures[j].LineError(); 
      if(dErr > dMaxError) {
        dMaxError = dErr;
      }
    }
    return dMaxError;
  }
      
  /**
   * Get the Maximum ErrorSample for the Control Point
   * 
   * @author Sharmila Prasad (8/26/2010)
   * 
   * @return double 
   */
  double ControlPoint::MaximumErrorSample()
  {
    double dMaxError = 0.0;
    if(Ignore()) return dMaxError;

    for(int j = 0; j < (int) p_measures.size(); j++) {
      if(p_measures[j].Ignore()) continue;
      if(p_measures[j].Type() == ControlMeasure::Unmeasured) continue;

      double dErr = p_measures[j].SampleError(); 
      if(dErr > dMaxError) {
        dMaxError = dErr;
      }
    }
    return dMaxError;
  }

  /**
   * Wraps the input longitude toward a base longitude
   *
   * @param  lon     Input longitude to be wrapped
   * @param  baselon Longitude to compare
   * @return The wrapped longitude
   */
  double ControlPoint::WrapLongitude(double lon, double baselon) {
    double diff = baselon - lon;

    if(diff <= 180.  &&  diff >= -180.) {  // No wrap needed
      return lon;
    }
    else if(diff > 180.) {
      return (lon + 360.);
    }
    else {  // (diff < -180.)
      return (lon - 360.);
    }
  }


  /**
   * Returns the number of non-ignored control measures
   *
   * @return Number of valid control measures
   */
  int ControlPoint::NumValidMeasures() {
    int size = 0;
    for(int cm = 0; cm < Size(); cm ++) {
      if(!p_measures[cm].Ignore()) size ++;
    }
    return size;
  }

  /**
   * Copy Constructor
   *
   * @author Sharmila Prasad (5/11/2010)
   *
   * @param pPoint
   *
   * @return ControlPoint&
   */
  ControlPoint &ControlPoint::operator= (const Isis::ControlPoint &pPoint) {
    p_id        = pPoint.p_id;
    p_type      = pPoint.p_type;
    p_ignore    = pPoint.p_ignore;
    p_held      = pPoint.p_held;
    p_latitude  = pPoint.p_latitude;
    p_longitude = pPoint.p_longitude;
    p_radius    = pPoint.p_radius;
    p_invalid   = pPoint.p_invalid;

    //!< List of Control Measures
    for(int i = 0; i < Size(); i++) {
      p_measures[i] = pPoint.p_measures[i];
    }

    return *this;
  }

  /**
   * Compare two Control Points for inequality
   *
   * @author Sharmila Prasad (4/20/2010)
   *
   * @param pPoint
   *
   * @return bool
   */
  bool ControlPoint::operator != (const Isis::ControlPoint &pPoint) const {
    return !(*this == pPoint);
  }

  /**
   * Compare two Control Points for equality
   *
   * @author Sharmila Prasad (4/20/2010)
   *
   * @param pPoint to be compared against
   *
   * @return bool
   */
  bool ControlPoint::operator == (const Isis::ControlPoint &pPoint) const {
    if(pPoint.Size() != Size() || pPoint.p_id != p_id || pPoint.p_type != p_type ||
        pPoint.p_ignore != p_ignore || pPoint.p_held != p_held || pPoint.p_latitude != p_latitude ||
        pPoint.p_longitude != p_longitude || pPoint.p_radius != p_radius || pPoint.p_invalid != p_invalid) {

      return false;
    }

    for(int i = 0; i < Size(); i++) {
      if(pPoint.p_measures[i] != p_measures[i]) {
        return false;
      }
    }

    return true;
  }
}
