/**
 * @file
 * $Revision: 1.10 $
 * $Date: 2009/12/28 20:56:01 $
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
#include "Projection.h"

#include <QObject>

#include <cfloat>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <vector>

#include <naif/SpiceUsr.h>

#include "Constants.h"
#include "Displacement.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "Longitude.h"
#include "NaifStatus.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "SpecialPixel.h"
#include "WorldMapper.h"

using namespace std;
namespace Isis {
  /**
   * Constructs an empty Projection object.
   *
   * @param label A PVL object containing map projection labels. These labels
   *            are fully described in the Isis Map Projection Users Guide. A
   *            brief example follows:
   *                   @code
   *                       Group = Mapping
   *                         EquatorialRadius = 3396190.0
   *                         PolarRadius = 3376200.0
   *                         LongitudeDirection = PositiveEast
   *                         LongitudeDomain = 360
   *                         LatitudeType = Planetographic
   *                         MinimumLatitude = 10.8920539924144
   *                         MaximumLatitude = 34.7603960060206
   *                         MinimumLongitude = 219.72432466275
   *                         MaximumLongitude = 236.186050244411
   *                         PixelResolution = 1387.31209461362
   *                         ProjectionName = SimpleCylindrical
   *                         CenterLongitude = 220.0
   *                       EndGroup
   *                     End
   *                   @endcode
   *
   * @throw IException::Unknown - "Projection failed. No target radii 
   *            available through keywords [EquatorialRadius and
   *             PolarRadius] or [TargetName]."
   * @throw IException::Unknown - "Projection failed. Invalid value 
   *            for keyword [EquatorialRadius]. It must be greater than
   *            zero."
   * @throw IException::Unknown - "Projection failed. Invalid value 
   *            for keyword [PolarRadius]. It must be greater than zero."
   * @throw IException::Unknown - "Projection failed. Invalid value 
   *            for keyword [LatitudeType] must be [Planetographic or
   *             Planetocentric]"
   * @throw IException::Unknown - "Projection failed. Invalid value 
   *            for keyword [LongitudeDirection] must be [PositiveWest or
   *            PositiveEast]"
   * @throw IException::Unknown - "Projection failed. Invalid value 
   *            for keyword [LongitudeDomain] must be [180 or 360]"
   * @throw IException::Unknown - "Projection failed. 
   *            [MinimumLatitude] is outside the range of [-90:90]"
   * @throw IException::Unknown - "Projection failed. 
   *            [MaximumLatitude] is outside the range of [-90:90]"
   * @throw IException::Unknown - "Projection failed. 
   *            [MinimumLatitude,MaximumLatitude] are not properly ordered"
   * @throw IException::Unknown - "Projection failed. 
   *            [MinimumLongitude,MaximumLongitude] are not properly ordered"
   * @throw IException::Unknown - "Projection failed. Invalid keyword 
   *            value(s). [EquatorialRadius] must be greater than or equal to
   *            [PolarRadius]"
   * @throw IException::Unknown - "Projection failed. Invalid label 
   *            group [Mapping]"
   * 
   */
  Projection::Projection(Pvl &label) : m_mappingGrp("Mapping") {
    try {
      // Try to read the mapping group
      m_mappingGrp = label.FindGroup("Mapping", Pvl::Traverse);

      // Get the radii from the EquatorialRadius and PolarRadius keywords
      if ((m_mappingGrp.HasKeyword("EquatorialRadius")) &&
          (m_mappingGrp.HasKeyword("PolarRadius"))) {
        m_equatorialRadius = m_mappingGrp["EquatorialRadius"];
        m_polarRadius = m_mappingGrp["PolarRadius"];
      }
      // Get the radii using the "TargetName" keyword and NAIF
      else  if (m_mappingGrp.HasKeyword("TargetName")) {
        PvlGroup radii = TargetRadii((QString)m_mappingGrp["TargetName"]);
        m_equatorialRadius = radii["EquatorialRadius"];
        m_polarRadius = radii["PolarRadius"];
      }
      else {
        QString msg = "Projection failed. No target radii are available "
                      "through keywords [EquatorialRadius and PolarRadius] "
                      "or [TargetName].";
        throw IException(IException::Unknown, msg, _FILEINFO_);
      }

      // Check the radii for validity
      if (m_equatorialRadius <= 0.0) {
        QString msg = "Projection failed. Invalid value for keyword "
                      "[EquatorialRadius]. It must be greater than zero";
        throw IException(IException::Unknown, msg, _FILEINFO_);
      }
      if (m_polarRadius <= 0.0) {
        QString msg = "Projection failed. Invalid value for keyword "
                      "[PolarRadius]. It must be greater than zero";
        throw IException(IException::Unknown, msg, _FILEINFO_);
      }

      // Get the LatitudeType
      if ((QString) m_mappingGrp["LatitudeType"] == "Planetographic") {
        m_latitudeType = Planetographic;
      }
      else if ((QString) m_mappingGrp["LatitudeType"] == "Planetocentric") {
        m_latitudeType = Planetocentric;
      }
      else {
        QString msg = "Projection failed. Invalid value for keyword "
                      "[LatitudeType] must be "
                      "[Planetographic or Planetocentric]";
        throw IException(IException::Unknown, msg, _FILEINFO_);
      }

      // Get the LongitudeDirection
      if (m_mappingGrp["LongitudeDirection"][0].toLower() == "positivewest") {
        m_longitudeDirection = PositiveWest;
      }
      else if (m_mappingGrp["LongitudeDirection"][0].toLower() == "positiveeast") {
        m_longitudeDirection = PositiveEast;
      }
      else {
        QString msg = "Projection failed. Invalid value for keyword "
                      "[LongitudeDirection] must be "
                      "[PositiveWest or PositiveEast]";
        throw IException(IException::Unknown, msg, _FILEINFO_);
      }

      // Get the LongitudeDomain
      if ((QString) m_mappingGrp["LongitudeDomain"] == "360") {
        m_longitudeDomain = 360;
      }
      else if ((QString) m_mappingGrp["LongitudeDomain"] == "180") {
        m_longitudeDomain = 180;
      }
      else {
        QString msg = "Projection failed. Invalid value for keyword "
                      "[LongitudeDomain] must be [180 or 360]";
        throw IException(IException::Unknown, msg, _FILEINFO_);
      }

      // Get the ground range if it exists
      m_groundRangeGood = false;
      if ((m_mappingGrp.HasKeyword("MinimumLatitude")) &&
          (m_mappingGrp.HasKeyword("MaximumLatitude")) &&
          (m_mappingGrp.HasKeyword("MinimumLongitude")) &&
          (m_mappingGrp.HasKeyword("MaximumLongitude"))) {
        m_minimumLatitude  = m_mappingGrp["MinimumLatitude"];
        m_maximumLatitude  = m_mappingGrp["MaximumLatitude"];
        m_minimumLongitude = m_mappingGrp["MinimumLongitude"];
        m_maximumLongitude = m_mappingGrp["MaximumLongitude"];

        if ((m_minimumLatitude < -90.0) || (m_minimumLatitude > 90.0)) {
          QString msg = "Projection failed. "
                        "[MinimumLatitude] of [" + toString(m_minimumLatitude)
                        + "] is outside the range of [-90:90]";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }

        if ((m_maximumLatitude < -90.0) || (m_maximumLatitude > 90.0)) {
          QString msg = "Projection failed. "
                        "[MaximumLatitude] of [" + toString(m_maximumLatitude)
                        + "] is outside the range of [-90:90]";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }

        if (m_minimumLatitude >= m_maximumLatitude) {
          QString msg = "Projection failed. "
                        "[MinimumLatitude,MaximumLatitude] of ["
                        + toString(m_minimumLatitude) + ","
                        + toString(m_maximumLatitude) + "] are not "
                        + "properly ordered";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }

        if (m_minimumLongitude >= m_maximumLongitude) {
          QString msg = "Projection failed. "
                        "[MinimumLongitude,MaximumLongitude] of ["
                        + toString(m_minimumLongitude) + "," 
                        + toString(m_maximumLongitude) + "] are not "
                        + "properly ordered";
          throw IException(IException::Unknown, msg, _FILEINFO_);
        }

        m_groundRangeGood = true;
      }
      else {
        // if no ground range is given, initialize the min/max lat/lon to 0
        m_minimumLatitude  = 0.0;
        m_maximumLatitude  = 0.0;
        m_minimumLongitude = 0.0;
        m_maximumLongitude = 0.0;
      }

      // Get the map rotation
      m_rotation = 0.0;
      if (m_mappingGrp.HasKeyword("Rotation")) {
        m_rotation = m_mappingGrp["Rotation"];
      }


      // Initialize miscellaneous protected data elements
      m_good = false;

      m_pixelResolution = 1.0;
      if (m_mappingGrp.HasKeyword("PixelResolution")) {
        m_pixelResolution = m_mappingGrp["PixelResolution"];
      }

      m_minimumX = DBL_MAX;
      m_maximumX = -DBL_MAX;
      m_minimumY = DBL_MAX;
      m_maximumY = -DBL_MAX;

      if (m_equatorialRadius < m_polarRadius) {
        QString msg = "Projection failed. Invalid keyword value(s). "
                      "[EquatorialRadius] = " + toString(m_equatorialRadius)
                      + " must be greater than or equal to [PolarRadius] = "
                      + toString(m_polarRadius);
        throw IException(IException::Unknown, msg, _FILEINFO_);
      }
      else {
        m_eccentricity = 1.0 -
                         (m_polarRadius * m_polarRadius) /
                         (m_equatorialRadius * m_equatorialRadius);
        m_eccentricity = sqrt(m_eccentricity);
      }

      m_mapper = NULL;

      m_sky = false;
      if (m_mappingGrp.HasKeyword("TargetName")) {
        QString str = m_mappingGrp["TargetName"];
        if (str.toUpper() == "SKY") m_sky = true;
      }

      // initialize the rest of the x,y,lat,lon member variables
      m_latitude = Null;
      m_longitude = Null;
      m_x = Null;
      m_y = Null;
    }
    catch(IException &e) {
      QString msg = "Projection failed.  Invalid label group [Mapping]";
      throw IException(e, IException::Unknown, msg, _FILEINFO_);
    }
  }

  //! Destroys the Projection object
  Projection::~Projection() {
    if(m_mapper) delete m_mapper;
  }

  /**
   * This method determines whether two map projection objects are equal by 
   * comparing the equatorial radius, polar radius, latitude type, longitude 
   * direction, resolution, and projection name. 
   *  
   * @param proj A reference to a Projection object to which this Projection 
   *             will be compared.
   *
   * @return bool Indicates whether the Projection objects are equivalent.
   */
  bool Projection::operator== (const Projection &proj) {
    if (EquatorialRadius() != proj.EquatorialRadius()) return false;
    if (PolarRadius() != proj.PolarRadius()) return false;
    if (IsPlanetocentric() != proj.IsPlanetocentric()) return false;
    if (IsPositiveWest() != proj.IsPositiveWest()) return false;
    if (Resolution() != proj.Resolution()) return false;
    if (Name() != proj.Name()) return false;
    return true;
  }

  /**
   * This method determines whether two map projection objects are not equal. 
   * True is returned if they have at least some differences in the 
   * radii, latitude type, longitude direction, resolution, or projection name. 
   *  
   * @param proj A reference to a Projection object to which this Projection 
   *             will be compared.
   * @return bool Indicates whether the Projection objects are not equivalent. 
   */
  bool Projection::operator!= (const Projection &proj) {
    return !(*this == proj);
  }


  /**
   * Returns true if projection is sky and false if it is land
   *
   * @return bool 
   */
  bool Projection::IsSky() const {
    return m_sky;
  }

  /**
   * This returns the equatorial radius of the target. The radius was
   * obtained from the label during object construction.
   *
   * @return double
   */
  double Projection::EquatorialRadius() const {
    return m_equatorialRadius;
  }

  /**
   * This returns the polar radius of the target. The radius was obtained
   * from the label during object construction.
   *
   * @return double
   */
  double Projection::PolarRadius() const {
    return m_polarRadius;
  }

  /**
   * This returns the eccentricity of the target, 
   *  
   * @f[ 
   * e = \sqrt{1 - \frac{PR^2}{ER^2}} 
   * @f] 
   * where PR is the polar radius and ER is the equatorial radius. Since 
   * polar and equatorial radii are required to be greater than zero, it 
   * follows that @f$ 0 \le e < 1 @f$ .  Note that if the body is 
   * spherical, then PR = ER and so @a e = 0. 
   *
   * @return double
   */
  double Projection::Eccentricity() const {
    return m_eccentricity;
  }

  /**
   * This method returns the local radius in meters at the specified latitude
   * position.  For this method, the local radius is defined as the distance 
   * from the center of the planet to the surface of the planet at the given 
   * latitude. 
   *  
   *  @f[ 
   * LR = \frac{ER*PR}{\sqrt{PR^2 \cos^2(LAT)+ER^2 \sin^2(LAT)}}
   * @f] 
   *  
   * @param latitude A latitude in degrees (assumed to be of the correct
   *                 LatitudeType).
   *  
   * @throw IException::Unknown - "The given latitude is invalid." 
   *  
   * @return double The value for the local radius, in meters, at the given 
   *                latitude.
   */
  double Projection::LocalRadius(double latitude) const {
    if (latitude == Null) {
      throw IException(IException::Unknown, 
                       "Unable to calculate local radius. The given latitude value [" 
                       + IString(latitude) + "] is invalid.", 
                       _FILEINFO_);
    }
    double a = m_equatorialRadius;
    double c = m_polarRadius;
    // to save calculations, if the target is spherical, return the eq. rad
    if (a - c < DBL_EPSILON) {
      return a;
    }
    else {
      double lat = latitude * PI / 180.0;
      return  a * c / sqrt(pow(c * cos(lat), 2) + pow(a * sin(lat), 2));
    }
  }

  /**
   * This method returns the local radius in meters at the current latitude
   * position. This is only usable if the use of SetGround or SetCoordinate was
   * successful.
   *
   * @return double The value for the local radius, in meters, at the current 
   *                latitude.
   */
  double Projection::LocalRadius() const {
    return LocalRadius(m_latitude);
  }

  /**
   * Creates a Pvl Group with keywords TargetName, EquitorialRadius, and 
   * PolarRadius. The values for the radii will be retrieved from the most 
   * recent Target Attitude and Shape Naif kernel available in the Isis data 
   * area. 
   *
   * @param target The name of the body for which the radii will be retrieved.
   *
   * @throw IException::Io - "Could not convert target name to NAIF code."
   *  
   * @return PvlGroup Group named "Mapping" with keywords TargetName, 
   *             EquatorialRadius, and PolarRadius.
   */

  PvlGroup Projection::TargetRadii(QString target) {
    // Convert the target name to a NAIF code
    SpiceInt code;
    SpiceBoolean found;
    bodn2c_c(target.toAscii().data(), &code, &found);
    if (!found) {
      QString msg = "Could not convert target name [" + target +"] to NAIF code";
      throw IException(IException::Io, msg, _FILEINFO_);
    }

    // Load the most recent target attitude and shape kernel for NAIF
    FileName kern("$Base/kernels/pck/pck?????.tpc");
    kern = kern.highestVersion();
    QString kernName(kern.expanded());
    furnsh_c(kernName.toAscii().data());

    // Get the radii from NAIF
    NaifStatus::CheckErrors();
    SpiceInt n;
    SpiceDouble radii[3];
    bodvar_c(code, "RADII", &n, radii);
    unload_c(kernName.toAscii().data());

    try {
      NaifStatus::CheckErrors();
    }
    catch (IException &e) {
      throw IException(e,
                       IException::Unknown,
                       QObject::tr("The target name [%1] does not correspond to a target body "
                                   "with known radii").arg(target),
                       _FILEINFO_);
    }

    PvlGroup mapping("Mapping");
    mapping += PvlKeyword("TargetName",  target);
    mapping += PvlKeyword("EquatorialRadius",  toString(radii[0] * 1000.0), "meters");
    mapping += PvlKeyword("PolarRadius", toString(radii[2] * 1000.0), "meters");

    return mapping;
  }

  /**
   * Convenience method to add the Target Radii information to the given Pvl 
   * Mapping Group. This method 
   *
   * @param cubeLab Pvl labels for the image.
   * @param mapGroup PvlGroup that contains mapping parameters for the
   *                 projection.
   *
   * @return PvlGroup The Mapping Group for the projection with keywords 
   *                 EquatorialRadius and PolarRadius. 
   *  
   */
  PvlGroup Projection::TargetRadii(Pvl &cubeLab, PvlGroup &mapGroup) {
    //Check to see if the mapGroup already has the target radii.
    //If BOTH radii are already in the mapGroup then just return back the map 
    //Group. 
    if (mapGroup.HasKeyword("EquatorialRadius") 
        && mapGroup.HasKeyword("PolarRadius")) {
      return mapGroup;
    }
    //If the mapping group only has one or the other of the radii keywords, then
    //we are going to replace both, so delete which ever one it does have.
    if (mapGroup.HasKeyword("EquatorialRadius") 
        && !mapGroup.HasKeyword("PolarRadius")) {
      mapGroup.DeleteKeyword("EquatorialRadius");
    }
    if (!mapGroup.HasKeyword("EquatorialRadius") 
        && mapGroup.HasKeyword("PolarRadius")) {
      mapGroup.DeleteKeyword("PolarRadius");
    }

    PvlGroup inst = cubeLab.FindGroup("Instrument", Pvl::Traverse);
    QString target = inst["TargetName"];
    PvlGroup radii = Projection::TargetRadii(target);
    //Now INSERT the EquatorialRadius and PolorRadius into the mapGroup pvl.
    mapGroup += PvlKeyword("EquatorialRadius",  
                           radii.FindKeyword("EquatorialRadius")[0], "meters");
    mapGroup += PvlKeyword("PolarRadius", 
                           radii.FindKeyword("PolarRadius")[0], "meters");

    return mapGroup;
  }

  /**
   * This method returns the latitude of true scale. It is a virtual function 
   * and if it is not overriden the default latitude of true scale is 0 (at 
   * the equator). Otherwise it is projection specific. For example, the 
   * center latitude for Mercator, Equidistant, or a parallel for conic 
   * projections. This method is used by the Scale routine to ensure the 
   * local radius is used in the computation. 
   *
   * @return double The latitude where the projection is not distorted.
   */
  double Projection::TrueScaleLatitude() const {
    return 0.0;
  }

  /**
   * This method returns true if the projection is
   *   equatorial cylindrical. In other words, if the
   *   projection is cylindrical and an image projected at 0 is
   *   the same as an image projected at 360.
   *
   *
   * @return bool true if the projection is equatorial cylindrical
   */
  bool Projection::IsEquatorialCylindrical() {
    return false;
  }

  /**
   * This indicates if the latitude type is planetocentric (as opposed to
   * planetographic). The latitude type was obtained from the label during
   * object construction.
   *
   * @return bool
   */
  bool Projection::IsPlanetocentric() const {
    return m_latitudeType == Planetocentric;
  }

  /**
   * This indicates if the latitude type is planetographic (as opposed to
   * planetocentric). The latitude type was obtained from the label during
   * object construction.
   *
   * @return bool
   */
  bool Projection::IsPlanetographic() const {
    return m_latitudeType == Planetographic;
  }

  /**
   * This method converts a planetographic latitude to a planetocentric 
   * latitude. It utilizes the equatorial and polar radii found in the 
   * labels to perform the computation. 
   *
   * @param lat Planetographic latitude to convert.
   *  
   * @see ToPlanetocentric(lat, eRadius, pRadius) 
   *  
   * @return double The latitude, converted to planetocentric.
   */
  double Projection::ToPlanetocentric(const double lat) const {
    return ToPlanetocentric(lat, m_equatorialRadius, m_polarRadius);
  }

  /**
   * This method converts a planetographic latitude to a planetocentric 
   * latitude.
   *
   * @param lat Planetographic latitude to convert.
   * @param eRadius Equatorial radius.
   * @param pRadius Polar radius
   *
   * @throw IException::Unknown - "The given latitude is invalid." 
   *  
   * @return double The latitude, converted to planetocentric.
   */
  double Projection::ToPlanetocentric(const double lat,
                                      double eRadius, double pRadius) {
    if (lat == Null || abs(lat) > 90.0) {
      throw IException(IException::Unknown, 
                       "Unable to convert to Planetocentric. The given latitude value [" 
                       + IString(lat) + "] is invalid.", 
                       _FILEINFO_);
    }
    double mylat = lat;
    if (abs(mylat) < 90.0) {  // So tan doesn't fail
      mylat *= PI / 180.0;
      mylat = atan(tan(mylat) * (pRadius / eRadius) *
                   (pRadius / eRadius));
      mylat *= 180.0 / PI;
    }
    return mylat;
  }

  /**
   * This method converts a planetocentric latitude to a planetographic
   * latitude. It utilizes the equatorial and polar radii found in the 
   * labels to perform the computation. 
   *
   * @param lat Planetocentric latitude to convert.
   *
   * @see ToPlanetographic(lat, eRadius, pRadius)
   *  
   * @return double The latitude, converted to planetographic.
   */
  double Projection::ToPlanetographic(const double lat) const {
    return ToPlanetographic(lat,  m_equatorialRadius, m_polarRadius);
  }

  /**
   * This method converts a planetocentric latitude to a planetographic
   * latitude. It is static so that a projection object does not need to
   * exist.
   *
   * @param lat Planetocentric latitude to convert.
   * @param eRadius Equatorial radius.
   * @param pRadius Polar radius
   *
   * @throw IException::Unknown - "The given latitude is invalid." 
   *  
   * @return double The latitude, converted to planetographic.
   */
  double Projection::ToPlanetographic(double lat,
                                      double eRadius, double pRadius) {
    //Account for double rounding error.
    if (qFuzzyCompare(fabs(lat), 90.0)) {
      lat = qRound(lat);
    }
    if (lat == Null || fabs(lat) > 90.0) {
      throw IException(IException::Unknown, 
                       "Unable to convert to Planetographic. The given latitude value [" 
                       + IString(lat) + "] is invalid.", 
                       _FILEINFO_);
    }
    double mylat = lat;
    if (fabs(mylat) < 90.0) {  // So tan doesn't fail
      mylat *= PI / 180.0;
      mylat = atan(tan(mylat) * (eRadius / pRadius) *
                   (eRadius / pRadius));
      mylat *= 180.0 / PI;
    }
    return mylat;
  }

  /**
   * This method returns the latitude type as a string. It will return either
   * Planetocentric or Planetographic.
   *
   * @return string The latitude type, "Planetocentric" or "Planetographic".
   */
  QString Projection::LatitudeTypeString() const {
    if (m_latitudeType == Planetographic) return "Planetographic";
    return "Planetocentric";
  }

  /**
   * This indicates if the longitude direction type is positive west (as
   * opposed to postive east). The longitude type was obtained from the 
   * label during object construction.
   *
   * @return bool
   */
  bool Projection::IsPositiveEast() const {
    return m_longitudeDirection == PositiveEast;
  }

  /**
   * This indicates if the longitude direction type is positive east (as
   * opposed to postive west). The longitude type was obtained from the 
   * label during object construction.
   *
   * @return bool
   */
  bool Projection::IsPositiveWest() const {
    return m_longitudeDirection == PositiveWest;
  }

  /**
   * This method converts a longitude into the positive east direction.
   *
   * @param lon Longitude to convert into the positive east direction.
   * @param domain Must be an integer value of 180 (for -180 to 180) or 360 (for 
   *            0 to 360).
   *  
   * @throw IException::Unknown - "The given longitude is invalid." 
   * @throw IException::Unknown - "Unable to convert longitude.  Domain is
   *            not 180 or 360."
   *  
   * @return double Longitude value, in positive east direction.
   */
  double Projection::ToPositiveEast(const double lon, const int domain) {
    if (lon == Null) {
      throw IException(IException::Unknown, 
                       "Unable to convert to PositiveEast. The given longitude value [" 
                       + IString(lon) + "] is invalid.", 
                       _FILEINFO_);
    }
    double mylon = lon;

    mylon *= -1;

    if (domain == 360) {
      mylon = To360Domain(mylon);
    }
    else if (domain == 180) {
      mylon = To180Domain(mylon);
    }
    else {
      QString msg = "Unable to convert longitude.  Domain [" + toString(domain) 
                    + "] is not 180 or 360.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    return mylon;
  }

  /**
   * This method converts a longitude into the positive west direction.
   *
   * @param lon Longitude to convert into the positive west direction.
   * @param domain Must be an integer value of 180 (for -180 to 180) or 360 (for 
   *            0 to 360).
   *
   * @throw IException::Unknown - "The given longitude is invalid." 
   * @throw IException::Unknown - "Unable to convert longitude.  Domain is
   *            not 180 or 360."
   *  
   * @return double Longitude value, in positive west direction.
   */
  double Projection::ToPositiveWest(const double lon, const int domain) {
    if (lon == Null) {
      throw IException(IException::Unknown, 
                       "Unable to convert to PositiveWest. The given longitude value [" 
                       + toString(lon) + "] is invalid.", 
                       _FILEINFO_);
    }
    double mylon = lon;

    mylon *= -1;

    if (domain == 360) {
      mylon = To360Domain(mylon);
    }
    else if (domain == 180) {
      mylon = To180Domain(mylon);
    }
    else {
      QString msg = "Unable to convert longitude.  Domain [" + toString(domain)
                    + "] is not 180 or 360.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    return mylon;
  }

  /**
   * This method returns the longitude direction as a string. It will return
   * either PositiveEast or PositiveWest.
   *
   * @return string The longitude direction, "PositiveEast" or 
   *         "PositiveWest".
   */
  QString Projection::LongitudeDirectionString() const {
    if (m_longitudeDirection == PositiveEast) return "PositiveEast";
    return "PositiveWest";
  }

  /**
   * This indicates if the longitude domain is -180 to 180 (as opposed to 0
   * to 360). The longitude domain was obtained from the label during object
   * construction.
   *
   * @return bool
   */
  bool Projection::Has180Domain() const {
    return m_longitudeDomain == 180;
  }

  /**
   * This indicates if the longitude domain is 0 to 360 (as opposed to -180
   * to 180). The longitude domain was obtained from the label during object
   * construction.
   *
   * @return bool
   */
  bool Projection::Has360Domain() const {
    return m_longitudeDomain == 360;
  }

  /**
   * This method converts a longitude into the -180 to 180 domain. It will leave
   * the longitude unchanged if it is already in the domain.
   *
   * @param lon Longitude to convert into the -180 to 180 domain.
   *
   * @throw IException::Unknown - "The given longitude is invalid." 
   *  
   * @return double The longitude, converted to 180 domain.
   */
  double Projection::To180Domain(const double lon) {
    if (lon == Null) {
      throw IException(IException::Unknown, 
                       "Unable to convert to 180 degree domain. The given longitude value [" 
                       + IString(lon) + "] is invalid.", 
                       _FILEINFO_);
    }
    return Isis::Longitude(lon, Angle::Degrees).force180Domain().degrees();
  }

  /**
   * This method converts a longitude into the 0 to 360 domain. It will leave 
   * the longitude unchanged if it is already in the domain.
   *
   * @param lon Longitude to convert into the 0 to 360 domain.
   *
   * @return double The longitude, converted to 360 domain.
   */
  double Projection::To360Domain(const double lon) {
    if (lon == Null) {
      throw IException(IException::Unknown, 
                       "Unable to convert to 360 degree domain. The given longitude value [" 
                       + IString(lon) + "] is invalid.", 
                       _FILEINFO_);
    }
    double result = lon;

    if ( (lon < 0.0 || lon > 360.0) &&
        !qFuzzyCompare(lon, 0.0) && !qFuzzyCompare(lon, 360.0)) {
     result = Isis::Longitude(lon, Angle::Degrees).force360Domain().degrees();
    }

    return result;
  }

  /**
   * This method returns the longitude domain as a string. It will return either
   * 180 or 360.
   *
   * @return string The longitude domain, "180" or "360".
   */
  QString Projection::LongitudeDomainString() const {
    if (m_longitudeDomain == 360) return "360";
    return "180";
  }

  /**
   * This indicates that the labels contained minimum and maximum latitudes
   * and longitudes (e.g., a ground range coverage). If the projection has
   * ground range coverage then the MinimumLatitude, MaximumLatitude,
   * MinimumLongitude, and MaximumLongitude methods can be used. The ground
   * range coverage essentially defines the area of user interest.
   *
   * @return bool
   */
  bool Projection::HasGroundRange() const {
    return m_groundRangeGood;
  }

  /**
   * This returns the minimum latitude of the area of interest. The value 
   * was obtained from the labels during object construction. This method 
   * can only be used if HasGroundRange returns a true.
   *
   * @return double
   */
  double Projection::MinimumLatitude() const {
    return m_minimumLatitude;
  }

  /**
   * This returns the maximum latitude of the area of interest. The value 
   * was obtained from the labels during object construction. This method 
   * can only be used if HasGroundRange returns a true.
   *
   * @return double
   */
  double Projection::MaximumLatitude() const {
    return m_maximumLatitude;
  }

  /**
   * This returns the minimum longitude of the area of interest. The value
   * was obtained from the labels during object construction. This method 
   * can only be used if HasGroundRange returns a true.
   *
   * @return double
   */
  double Projection::MinimumLongitude() const {
    return m_minimumLongitude;
  }

  /**
   * This returns the maximum longitude of the area of interest. The value
   * was obtained from the labels during object construction. This method 
   * can only be used if HasGroundRange returns a true.
   *
   * @return double
   */
  double Projection::MaximumLongitude() const {
    return m_maximumLongitude;
  }

  /**
   * Returns the value of the Rotation keyword from the mapping group. 
   *
   * @return double The rotation of the map.
   */
  double Projection::Rotation() const {
    return m_rotation;
  }

  /**
   * This method is used to set the latitude/longitude (assumed to be of the
   * correct LatitudeType, LongitudeDirection, and LongitudeDomain. The Set
   * forces an attempted calculation of the projection X/Y values. This may or
   * may not be successful and a status is returned as such. Usually this method
   * is overridden in a dervied class, for example, Sinusoidal. If not the
   * default method simply copies lat/lon to x/y which is no projection.
   *
   * @param lat Latitude value to project
   * @param lon Longitude value to project
   *
   * @return bool Indicates whether the method was successful.
   */
  bool Projection::SetGround(const double lat, const double lon) {
    if (lat == Null || lon == Null) {
      m_good = false;
      return m_good;
    }
    else {
      m_latitude = lat;
      m_longitude = lon;
      m_good = true;
      SetComputedXY(lon, lat);
    }
    return m_good;
  }

  /**
   * This method is used to set the projection x/y. The Set forces an attempted
   * calculation of the corresponding latitude/longitude position. This may or
   * may not be successful and a status is returned as such.  Usually this 
   * method is overridden in a dervied class, for example, Sinusoidal. If not 
   * the default method simply copies x/y to  lat/lon to x/y which is no 
   * projection. 
   *
   * @param x X coordinate of the projection in units that are the same as the
   *          radii in the label
   * @param y Y coordinate of the projection in units that are the same as the
   *          radii in the label
   *
   * @return bool Indicates whether the method was successful.
   */
  bool Projection::SetCoordinate(const double x, const double y) {
    if (x == Null || y == Null) {
      m_good = false;
    }
    else {
      m_good = true;
      SetXY(x, y);
      m_latitude = m_y;
      m_longitude = m_x;
    }
    return m_good;
  }

  /**
   * This indicates if the last invocation of SetGround, SetCoordinate,
   * SetUniversalGround, or SetWorld was with successful or not. If there was 
   * success then the Latitude, Longitude, XCoord, YCoord, 
   * UniversalLatitude, UniversalLongitude, WorldX, and WorldY methods can 
   * be utilized. 
   *
   * @return bool True if the last call to SetGround, SetCoordinate, 
   *         SetUniversalGround, or SetWorld was successful.
   */
  bool Projection::IsGood() const {
    return m_good;
  }


  /**
   * This returns a latitude with correct latitude type as specified in the
   * label object. The method can only be used if SetGround, SetCoordinate,
   * SetUniversalGround, or SetWorld return with success. Success can also
   * be checked using the IsGood method.
   *
   * @return double
   */
  double Projection::Latitude() const {
    return m_latitude;
  }

  /**
   * This returns a longitude with correct longitude direction and domain as
   * specified in the label object. The method can only be used if 
   * SetGround, SetCoordinate, SetUniversalGround, or SetWorld return with 
   * success. Success can also be checked using the IsGood method. 
   *
   * @return double
   */
  double Projection::Longitude() const {
    return m_longitude;
  }

  /**
   * This returns the projection X provided SetGround, SetCoordinate,
   * SetUniversalGround, or SetWorld returned with success. Success can also
   * be checked using the IsGood method. The units of X will be in the same .
   * units as the radii obtained from the label.
   *
   * @return double
   */
  double Projection::XCoord() const {
    return m_x;
  }


  /**
   * This returns the projection Y provided SetGround, SetCoordinate,
   * SetUniversalGround, or SetWorld returned with success. Success can also
   * be checked using the IsGood method. The units of Y will be in the same
   * units as the radii obtained from the label.
   *
   * @return double
   */
  double Projection::YCoord() const {
    return m_y;
  }


  /**
   * This method is used to set the latitude/longitude which must be
   * Planetocentric (latitude) and PositiveEast/Domain360 (longitude). The Set
   * forces an attempted calculation of the projection X/Y values. This may or
   * may not be successful and a status is returned as such.
   *
   * @param lat Planetocentric Latitude value to project
   * @param lon PositiveEast, Domain360 Longitude value to project
   *
   * @return bool Indicates whether the method was successful.
   */
  bool Projection::SetUniversalGround(const double lat, const double lon) {
    if (lat == Null || lon == Null) {
      m_good = false;
      return m_good;
    }
    // Deal with the longitude first
    m_longitude = lon;
    if (m_longitudeDirection == PositiveWest) m_longitude = -lon;
    if (m_longitudeDomain == 180) {
      m_longitude = To180Domain(m_longitude);
    }
    else {
      // Do this because longitudeDirection could cause (-360,0)
      m_longitude = To360Domain(m_longitude);
    }

    // Deal with the latitude
    if (m_latitudeType == Planetographic) {
      m_latitude = ToPlanetographic(lat);
    }
    else {
      m_latitude = lat;
    }

    // Now the lat/lon are in user defined coordinates so set them
    return SetGround(m_latitude, m_longitude);
    }

  /**
   * This returns a universal latitude (planetocentric). The method can only be
   * used if SetGround, SetCoordinate, SetUniversalGround, or SetWorld return
   * with success. Success can also be checked using the IsGood method.
   *
   * @return double The universal latitude.
   */
  double Projection::UniversalLatitude() {
    double lat = m_latitude;
    if (m_latitudeType == Planetographic) lat = ToPlanetocentric(lat);
    return lat;
  }

  /**
   * This returns a universal longitude (positive east in 0 to 360 domain). The
   * method can only be used if SetGround, SetCoordinate, SetUniversalGround, or
   * SetWorld return with success. Success can also be checked using the IsGood
   * method.
   *
   * @return double The universal longitude.
   */
  double Projection::UniversalLongitude() {
    double lon = m_longitude;
    if (m_longitudeDirection == PositiveWest) lon = -lon;
    lon = To360Domain(lon);
    return lon;
  }

  /**
   * If desired the programmer can use this method to set a world mapper to
   * be used in the SetWorld, WorldX, and WorldY methods. Mappers typically
   * transform a projection coordinate (x/y) into the desired working
   * coordinate system, for example, cube pixels or inches on a piece of
   * paper. They transform in both directions (world to projection and
   * projection to world). This allows for conversions from line/sample to
   * latitude/longitude and vice versa. This projection will take ownership
   * of the WorldMapper pointer.
   *
   * @param mapper Pointer to the mapper
   */
  void Projection::SetWorldMapper(WorldMapper *mapper) {
    m_mapper = mapper;
  }

  /**
   * This method is used to set a world coordinate. A world coordinate is a
   * different coordinate type that has a one-to-one mapping to the projection
   * coordinate system. For example, mapping pixel samples and lines to
   * projection x's and y's. The Set forces an attempted calculation of the
   * corresponding latitude/longitude position. This may or may not be 
   * successful and a status is returned as such. Note that is only applies if 
   * the Projection object was given an WorldMapper object during construction. 
   * If an WorldMapper was not supplied then SetWorld operates exactly the same 
   * as SetCoordinate (impling that world coordinate and projection coordinate 
   * are identical). 
   *
   * @param worldX World X coordinate in units that are specified by the
   *            WorldMapper object (e.g., pixels, millimeters, etc)
   * @param worldY World Y coordinate in units that are specified by the
   *            WorldMapper object (e.g., pixels, millimeters, etc)
   *
   * @return bool Indicates whether the method was successful.
   */
  bool Projection::SetWorld(const double worldX, const double worldY) {
    double projectionX;
    double projectionY;

    if (m_mapper != NULL) {
      projectionX = m_mapper->ProjectionX(worldX);
      projectionY = m_mapper->ProjectionY(worldY);
    }
    else {
      projectionX = worldX;
      projectionY = worldY;
    }

    return SetCoordinate(projectionX, projectionY);
  }

  /**
   * This returns the world X coordinate provided SetGround, SetCoordinate,
   * SetUniversalGround, or SetWorld returned with success. Success can also be
   * checked using the IsGood method. The units of X will be in the units as
   * specified by the WorldMapper object which was given to the SetWorldMapper
   * method. If a mapper object was not given then world coordinates are the 
   * same as the projection coordinates (i.e., WorldX and XCoord will return the 
   * same value). 
   *
   * @return double The world X coordinate.
   */
  double Projection::WorldX() const {
    if (m_mapper != NULL) {
      return m_mapper->WorldX(m_x);
    }
    else {
      return m_x;
    }
  }

  /**
   * This returns the world Y coordinate provided SetGround, SetCoordinate,
   * SetUniversalGround, or SetWorld returned with success. Success can also be
   * checked using the IsGood method. The units of Y will be in the units as
   * specified by the WorldMapper object which was given to the SetWorldMapper.
   * If a mapper object was not given then world coordinates are the same as the
   * projection coordinates (i.e., WorldY and YCoord will return the same 
   * value).
   *
   * @return double The world Y coordinate.
   */
  double Projection::WorldY() const {
    if (m_mapper != NULL) {
      return m_mapper->WorldY(m_y);
    }
    else {
      return m_y;
    }
  }

  /**
   * This method converts a projection x value to a world x value. For example,
   * if the world coordinate system is an image then this method converts a
   * projection x to a sample position. Note that if SetWorldMapper is not used
   * then this routine simply returns the value of the argument. That is, no
   * mapping occurs.
   *
   * @param projectionX Projection x value in meters
   *
   * @throw IException::Unknown - "The given x-value is invalid." 
   *  
   * @return double The world X coordinate value.
   */
  double Projection::ToWorldX(const double projectionX) const {
    if (projectionX == Null) {
      throw IException(IException::Unknown, 
                       "Unable to convert to world x.  The given x-value [" 
                       + IString(projectionX) + "] is invalid.", 
                       _FILEINFO_);
    }
    if (m_mapper != NULL) {
      return m_mapper->WorldX(projectionX);
    }
    else {
      return projectionX;
    }
  }

  /**
   * This method converts a projection y value to a world y value. For example,
   * if the world coordinate system is an image then this method converts a
   * projection y to a line position. Note that if SetWorldMapper is not used
   * then this routine simply returns the value of the argument. That is, no
   * mapping occurs.
   *
   * @param projectionY Projection y value in meters
   *
   * @throw IException::Unknown - "The given y-value is invalid." 
   *  
   * @return double The world Y coordinate value.
   */
  double Projection::ToWorldY(const double projectionY) const {
    if (projectionY == Null) {
      throw IException(IException::Unknown, 
                       "Unable to convert to world y.  The given y-value [" 
                       + IString(projectionY) + "] is invalid.", 
                       _FILEINFO_);
    }
    if (m_mapper != NULL) {
      return m_mapper->WorldY(projectionY);
    }
    else {
      return projectionY;
    }
  }

  /**
   * This method converts a world x value to a projection x value. For example,
   * if the world coordinate system is an image then this method converts a
   * sample position to a projection x value. Note that if SetWorldMapper is not
   * used then this routine simply returns the value of the argument. That is,
   * no mapping occurs.
   *
   * @param worldX World x coordinate
   *
   * @throw IException::Unknown - "The given x-value is invalid." 
   *  
   * @return double The projection X coordinate value.
   */
  double Projection::ToProjectionX(const double worldX) const {
    if (worldX == Null) {
      throw IException(IException::Unknown, 
                       "Unable to convert to projection x.  The given x-value [" 
                       + IString(worldX) + "] is invalid.", 
                       _FILEINFO_);
    }
    if (m_mapper != NULL) {
      return m_mapper->ProjectionX(worldX);
    }
    else {
      return worldX;
    }
  }

  /**
   * This method converts a world y value to a projection y value. For example,
   * if the world coordinate system is an image then this method converts a line
   * position to a projection y value. Note that if SetWorldMapper is not used
   * then this routine simply returns the value of the argument. That is, no
   * mapping occurs.
   *
   * @param worldY World y coordinate
   *
   * @throw IException::Unknown - "The given y-value is invalid." 
   *  
   * @return double The projection Y coordinate value.
   */
  double Projection::ToProjectionY(const double worldY) const {
    if (worldY == Null) {
      throw IException(IException::Unknown, 
                       "Unable to convert to projection y.  The given y-value [" 
                       + IString(worldY) + "] is invalid.", 
                       _FILEINFO_);
    }
    if (m_mapper != NULL) {
      return m_mapper->ProjectionY(worldY);
    }
    else {
      return worldY;
    }
  }

  /**
   * This method returns the resolution for mapping world coordinates into
   * projection coordinates. For example, if the world coordinate system is an
   * image then this routine returns the number of meters per pixel. Likewise,
   * if the world coordinate system is a piece of paper, it might return the
   * number of meters per inch of paper. If the SetWorldMapper method is not
   * invoked then this method returns 1.0
   *
   * @return double The resolution, in appropriate units.
   */
  double Projection::Resolution() const {
    if (m_mapper != NULL) {
      return m_mapper->Resolution();
    }
    else {
      return 1.0;
    }
  }

  /**
   * This method returns the scale for mapping world coordinates into projection
   * coordinates. For example, if the world coordinate system is an image then
   * this routine returns the number of pixels per degree. Likewise, if the
   * world coordinate system is a piece of paper, it might return the number of
   * inches of paper per degree. If the SetWorldMapper method is not invoked
   * then this method returns 1.0
   *
   * @return double The scale for mapping.
   */
  double Projection::Scale() const {
    if (m_mapper != NULL) {
      double lat = TrueScaleLatitude() * PI / 180.0;
      double a = m_polarRadius * cos(lat);
      double b = m_equatorialRadius * sin(lat);
      double localRadius = m_equatorialRadius * m_polarRadius /
                           sqrt(a * a + b * b);

      return localRadius / m_mapper->Resolution();
    }
    else {
      return 1.0;
    }
  }

  /**
   * Converts the given angle (in degrees) to hours by using the ratio 15 
   * degrees per hour. 
   *
   * @param angle Angle in degrees to be converted to hours
   *
   * @return double The number of hours for the given angle. 
   */
  double Projection::ToHours(double angle) {
    return angle / 15.0;
  }

  /**
   * Converts the given angle (in degrees) to degrees, minutes, seconds.
   * Outputs in the form xxx yym zz.zzzs, for example, 206.291 degrees 
   * is 206 17m 27.6s 
   *
   * @param angle Angle in degrees to be converted to degrees, minutes, seconds
   *
   * @return string The angle in Degrees, minutes, seconds
   */
  QString Projection::ToDMS(double angle) {
    int iangle = (int)angle;
    double mins = abs(angle - iangle) * 60.0;
    int imins = (int)mins;
    double secs = (mins - imins) * 60.0;
    int isecs = (int)secs;
    double frac = (secs - isecs) * 1000.0;
    if (frac >= 1000.0) {
      frac -= 1000.0;
      isecs++;
    }
    if (isecs >= 60) {
      isecs -= 60;
      imins++;
    }
    if (imins >= 60) {
      imins -= 60;
      iangle++;
    }
    stringstream s;
    s << iangle << " " << setw(2) << setfill('0')
      << imins << "m " << setw(2) << setfill('0') << isecs << "."  <<
      setprecision(3) << frac << "s";
    return s.str().c_str();
  }

  /**
   * Converts the given angle (in degrees) to hours, minutes, seconds.
   * Outputs in the form xxh yym zz.zzzs  For example, 206.291 will be
   * 13h 45m 09.84s
   *
   * @param angle Angle in degrees to be converted to hours, minutes, seconds
   *
   * @return string The angle in Hours, minutes, seconds
   */
  QString Projection::ToHMS(double angle) {
    double tangle = angle;
    while (tangle < 0.0) tangle += 360.0;
    while (tangle > 360.0) tangle -= 360.0;
    double hrs = ToHours(tangle);
    int ihrs = (int)(hrs);
    double mins = (hrs - ihrs) * 60.0;
    int imins = (int)(mins);
    double secs = (mins - imins) * 60.0;
    int isecs = (int)(secs);
    double msecs = (secs - isecs) * 1000.0;
    int imsecs = (int)(msecs + 0.5);
    if (imsecs >= 1000) {
      imsecs -= 1000;
      isecs++;
    }
    if (isecs >= 60) {
      isecs -= 60;
      imins++;
    }
    if (imins >= 60) {
      imins -= 60;
      ihrs++;
    }
    stringstream s;
    s << setw(2) << setfill('0') << ihrs << "h " << setw(2) << setfill('0') <<
      imins << "m " << setw(2) << setfill('0') << isecs << "." << imsecs << "s";
    return s.str().c_str();
  }

  /**
   * This protected method is a helper for derived classes.  It
   * takes unrotated x and y values, rotates them using the 
   * rotation angle data member, and stores the results in the
   * current x and y data members. 
   *
   * @param x The unrotated x coordinate.
   * @param y The unrotated y coordinate.
   */
  void Projection::SetComputedXY(double x, double y) {
    if (x == Null || y == Null) {
      m_good = false;
      return;
    }
    if (m_rotation == 0.0) {
      m_x = x;
      m_y = y;
    }
    else {
      double rot = m_rotation * PI / 180.0;
      m_x = x * cos(rot) + y * sin(rot);
      m_y = y * cos(rot) - x * sin(rot);
    }
  }

  /**
   * This protected method is a helper for derived classes.  It
   * takes a rotated x,y and stores them in the current x and y
   * data members. 
   *
   * @param x The rotated x coordinate.
   * @param y The rotated y coordinate.
   */
  void Projection::SetXY(double x, double y) {
    if (x == Null || y == Null) {
      m_good = false;
    }
    m_x = x;
    m_y = y;
    return;
  }

  /**
   * Calculates the unrotated form of current x value.
   *
   * @return double The unrotated x-value.
   */
  double Projection::GetX() const {
    if (m_rotation == 0.0) return m_x;
    double rot = m_rotation * PI / 180.0;
    return m_x * cos(rot) - m_y * sin(rot);
  }

  /**
   * Calculates the unrotated form of the current y value.
   *
   * @return double The unrotated y-value.
   */
  double Projection::GetY() const {
    if (m_rotation == 0.0) return m_y;
    double rot = m_rotation * PI / 180.0;
    return m_y * cos(rot) + m_x * sin(rot);
  }

  /**
   * This method is used to determine the x/y range which completely covers the
   * area of interest specified by the lat/lon range. The latitude/longitude
   * range may be obtained from the labels. This method should not be used if
   * HasGroundRange is false. The purpose of this method is to return the x/y
   * range so it can be used to compute how large a map may need to be. For
   * example, how big a piece of paper is needed or how large of an image needs
   * to be created. This is method and therefore must be written by the derived
   * class (e.g., Sinusoidal). The method may fail as indicated by its return
   * value.
   *
   * 
   * @param &minX Reference to the address where the minimum x 
   *             coordinate value will be written.  The Minimum x projection
   *             coordinate calculated by this method covers the
   *             latitude/longitude range specified in the labels.
   *
   * @param &maxX Reference to the address where the maximum x 
   *             coordinate value will be written.  The Maximum x projection
   *             coordinate calculated by this method covers the
   *             latitude/longitude range specified in the labels.
   *
   * @param &minY Reference to the address where the minimum y 
   *             coordinate value will be written.  The Minimum y projection
   *             coordinate calculated by this method covers the
   *             latitude/longitude range specified in the labels.
   *
   * @param &maxY Reference to the address where the maximum y 
   *             coordinate value will be written.  The Maximum y projection
   *             coordinate calculated by this method covers the
   *             latitude/longitude range specified in the labels.
   * 
   * @return bool Indicates whether the method was able to determine the X/Y 
   *              Range of the projection.  If yes, minX, maxX, minY, maxY will
   *              be set with these values.
   *
   */
  bool Projection::XYRange(double &minX, double &maxX, 
                           double &minY, double &maxY) {
    if (minX == Null || maxX == Null || minY == Null || maxY == Null) {
      return false;
    }
    if (m_groundRangeGood) {
      minX = m_minimumLongitude;
      maxX = m_maximumLongitude;
      minY = m_minimumLatitude;
      maxY = m_maximumLatitude;
      return true;
    }
    return false;
  }

  /**
   * This convience function is established to assist in the development of the
   * XYRange virtual method. It allows the developer to test ground points
   * (lat/lon) to see if they produce a minimum/maximum projection coordinate.
   * For example in Sinusoidal,
   *    @code
   *       bool Sinusoidal::XYRange(double &minX, double &maxX,
   *                                    double &minY, double &maxY) {
   *        // Check the corners of the lat/lon range
   *         XYRangeCheck (m_minimumLatitude,m_minimumLongitude);
   *         XYRangeCheck (m_maximumLatitude,m_minimumLongitude);
   *         XYRangeCheck (m_minimumLatitude,m_maximumLongitude);
   *         XYRangeCheck (m_maximumLatitude,m_maximumLongitude);
   *
   *         // If the latitude crosses the equator check there
   *         if ((m_minimumLatitude < 0.0) && (m_maximumLatitude > 0.0)) {
   *           XYRangeCheck (0.0,m_minimumLongitude);
   *           XYRangeCheck (0.0,m_maximumLongitude);
   *         }
   *
   *         // Make sure everything is ordered
   *         if (m_minimumX >= m_maximumX) return false;
   *         if (m_minimumY >= m_maximumY) return false;
   *
   *         // Return X/Y min/maxs
   *         minX = m_minimumX;
   *         maxX = m_maximumX;
   *         minY = m_minimumY;
   *         maxY = m_maximumY;
   *         return true;
   *      }
   *    @endcode
   *
   *
   * @param latitude Test for min/max projection coordinates at this latitude
   * @param longitude Test for min/max projection coordinates at this longitude
   */
  void Projection::XYRangeCheck(const double latitude, const double longitude) {
    if (latitude == Null || longitude == Null) {
      m_good = false;
      return;
    }
    SetGround(latitude, longitude);
    if (!IsGood()) return;

    if (m_x < m_minimumX) m_minimumX = m_x;
    if (m_x > m_maximumX) m_maximumX = m_x;
    if (m_y < m_minimumY) m_minimumY = m_y;
    if (m_y > m_maximumY) m_maximumY = m_y;
    return;
  }

  /** 
   * This method is used to find the XY range for oblique aspect projections 
   * (non-polar projections) by "walking" around each of the min/max lat/lon. 
   *  
   * @param minX Minimum x projection coordinate which covers the latitude
   *             longitude range specified in the labels.
   * @param maxX Maximum x projection coordinate which covers the latitude
   *             longitude range specified in the labels.
   * @param minY Minimum y projection coordinate which covers the latitude
   *             longitude range specified in the labels.
   * @param maxY Maximum y projection coordinate which covers the latitude
   *             longitude range specified in the labels.
   * 
   * @return @b bool Indicates whether the method was successful.
   * @see XYRange()
   * @author Stephen Lambright 
   * @internal
   *   @history 2011-07-02 Jeannie Backer - Moved this code from
   *                           ObliqueCylindrical class to its own method here.
   */
  bool Projection::xyRangeOblique(double &minX, double &maxX, 
                                  double &minY, double &maxY) {
    if (minX == Null || maxX == Null || minY == Null || maxY == Null) {
      return false;
    }
    //For oblique, we'll have to walk all 4 sides to find out min/max x/y values
    if (!HasGroundRange()) return false; // Don't have min/max lat/lon, 
                                        //can't continue

    m_specialLatCases.clear();
    m_specialLonCases.clear();

    // First, search longitude for min X/Y
    double minFoundX1, minFoundX2;
    double minFoundY1, minFoundY2;

    // Search for minX between minlat and maxlat along minlon
    doSearch(MinimumLatitude(), MaximumLatitude(), 
             minFoundX1, MinimumLongitude(), true, true, true);
    // Search for minX between minlat and maxlat along maxlon
    doSearch(MinimumLatitude(), MaximumLatitude(), 
             minFoundX2, MaximumLongitude(), true, true, true);
    // Search for minY between minlat and maxlat along minlon
    doSearch(MinimumLatitude(), MaximumLatitude(), 
             minFoundY1, MinimumLongitude(), false, true, true);
    // Search for minY between minlat and maxlat along maxlon
    doSearch(MinimumLatitude(), MaximumLatitude(), 
             minFoundY2, MaximumLongitude(), false, true, true);

    // Second, search latitude for min X/Y
    double minFoundX3, minFoundX4;
    double minFoundY3, minFoundY4;

    // Search for minX between minlon and maxlon along minlat
    doSearch(MinimumLongitude(), MaximumLongitude(), 
             minFoundX3, MinimumLatitude(), true, false, true);
    // Search for minX between minlon and maxlon along maxlat
    doSearch(MinimumLongitude(), MaximumLongitude(), 
             minFoundX4, MaximumLatitude(), true, false, true);
    // Search for minY between minlon and maxlon along minlat
    doSearch(MinimumLongitude(), MaximumLongitude(), 
             minFoundY3, MinimumLatitude(), false, false, true);
    // Search for minY between minlon and maxlon along maxlat
    doSearch(MinimumLongitude(), MaximumLongitude(), 
             minFoundY4, MaximumLatitude(), false, false, true);

    // We've searched all possible minimums, go ahead and store the lowest
    double minFoundX5 = min(minFoundX1, minFoundX2);
    double minFoundX6 = min(minFoundX3, minFoundX4);
    m_minimumX = min(minFoundX5, minFoundX6);

    double minFoundY5 = min(minFoundY1, minFoundY2);
    double minFoundY6 = min(minFoundY3, minFoundY4);
    m_minimumY = min(minFoundY5, minFoundY6);

    // Search longitude for max X/Y
    double maxFoundX1, maxFoundX2;
    double maxFoundY1, maxFoundY2;

    // Search for maxX between minlat and maxlat along minlon
    doSearch(MinimumLatitude(), MaximumLatitude(), 
             maxFoundX1, MinimumLongitude(), true, true, false);
    // Search for maxX between minlat and maxlat along maxlon
    doSearch(MinimumLatitude(), MaximumLatitude(), 
             maxFoundX2, MaximumLongitude(), true, true, false);
    // Search for maxY between minlat and maxlat along minlon
    doSearch(MinimumLatitude(), MaximumLatitude(), 
             maxFoundY1, MinimumLongitude(), false, true, false);
    // Search for maxY between minlat and maxlat along maxlon
    doSearch(MinimumLatitude(), MaximumLatitude(), 
             maxFoundY2, MaximumLongitude(), false, true, false);

    // Search latitude for max X/Y
    double maxFoundX3, maxFoundX4;
    double maxFoundY3, maxFoundY4;

    // Search for maxX between minlon and maxlon along minlat
    doSearch(MinimumLongitude(), MaximumLongitude(), 
             maxFoundX3, MinimumLatitude(), true, false, false);
    // Search for maxX between minlon and maxlon along maxlat
    doSearch(MinimumLongitude(), MaximumLongitude(), 
             maxFoundX4, MaximumLatitude(), true, false, false);
    // Search for maxY between minlon and maxlon along minlat
    doSearch(MinimumLongitude(), MaximumLongitude(), 
             maxFoundY3, MinimumLatitude(), false, false, false);
    // Search for maxY between minlon and maxlon along maxlat
    doSearch(MinimumLongitude(), MaximumLongitude(), 
             maxFoundY4, MaximumLatitude(), false, false, false);

    // We've searched all possible maximums, go ahead and store the highest
    double maxFoundX5 = max(maxFoundX1, maxFoundX2);
    double maxFoundX6 = max(maxFoundX3, maxFoundX4);
    m_maximumX = max(maxFoundX5, maxFoundX6);

    double maxFoundY5 = max(maxFoundY1, maxFoundY2);
    double maxFoundY6 = max(maxFoundY3, maxFoundY4);
    m_maximumY = max(maxFoundY5, maxFoundY6);

    // Look along discontinuities for more extremes
    vector<double> specialLatCases = m_specialLatCases;
    for (unsigned int specialLatCase = 0; 
        specialLatCase < specialLatCases.size(); 
        specialLatCase ++) {
      double minX, maxX, minY, maxY;

      // Search for minX between minlon and maxlon along latitude discontinuities
      doSearch(MinimumLongitude(), MaximumLongitude(), 
               minX, specialLatCases[specialLatCase], true,  false, true);
      // Search for minY between minlon and maxlon along latitude discontinuities
      doSearch(MinimumLongitude(), MaximumLongitude(), 
               minY, specialLatCases[specialLatCase], false, false, true);
      // Search for maxX between minlon and maxlon along latitude discontinuities
      doSearch(MinimumLongitude(), MaximumLongitude(), 
               maxX, specialLatCases[specialLatCase], true,  false, false);
      // Search for maxX between minlon and maxlon along latitude discontinuities
      doSearch(MinimumLongitude(), MaximumLongitude(), 
               maxY, specialLatCases[specialLatCase], false, false, false);

      m_minimumX = min(minX, m_minimumX);
      m_maximumX = max(maxX, m_maximumX);
      m_minimumY = min(minY, m_minimumY);
      m_maximumY = max(maxY, m_maximumY);
    }

    vector<double> specialLonCases = m_specialLonCases;
    for (unsigned int specialLonCase = 0; 
        specialLonCase < specialLonCases.size(); 
        specialLonCase ++) {
      double minX, maxX, minY, maxY;

      // Search for minX between minlat and maxlat along longitude discontinuities
      doSearch(MinimumLatitude(), MaximumLatitude(), 
               minX, specialLonCases[specialLonCase], true,  true, true);
      // Search for minY between minlat and maxlat along longitude discontinuities
      doSearch(MinimumLatitude(), MaximumLatitude(), 
               minY, specialLonCases[specialLonCase], false, true, true);
      // Search for maxX between minlat and maxlat along longitude discontinuities
      doSearch(MinimumLatitude(), MaximumLatitude(), 
               maxX, specialLonCases[specialLonCase], true,  true, false);
      // Search for maxY between minlat and maxlat along longitude discontinuities
      doSearch(MinimumLatitude(), MaximumLatitude(), 
               maxY, specialLonCases[specialLonCase], false, true, false);

      m_minimumX = min(minX, m_minimumX);
      m_maximumX = max(maxX, m_maximumX);
      m_minimumY = min(minY, m_minimumY);
      m_maximumY = max(maxY, m_maximumY);
    }

    m_specialLatCases.clear();
    m_specialLonCases.clear();

    // Make sure everything is ordered
    if (m_minimumX >= m_maximumX) return false;
    if (m_minimumY >= m_maximumY) return false;

    // Return X/Y min/maxs
    minX = m_minimumX;
    maxX = m_maximumX;
    minY = m_minimumY;
    maxY = m_maximumY;

    return true;
  }

  /**
   * This method searches for extreme (min/max/discontinuity) coordinate values
   * along the constBorder line between minBorder and maxBorder (that is, 
   * across latitudes/longitudes). This method locates the extrema by utilizing
   * the findExtreme() method until the coordinate values converge. Then, 
   * extremeVal parameter is updated with this value before returning. 
   *  
   * Discontinuities are stored in m_specialLatCases and m_specialLonCases so 
   * they may be checked again later, which creates significantly more accuracy 
   * in some cases. 
   *  
   * @param minBorder Minimum latitude or longitude to search between.
   * @param maxBorder Maximum latitude or longitude to search between.
   * @param extremeVal The resulting global coordinate value (min or max 
   *            value for x or y, depending on findMin and searchX) on the
   *            constBorder.
   * @param constBorder The latitude or longitude that remains constant.  The 
   *            method will step along this border.
   * @param searchX Indicates whether the method is searching for a min or max 
   *            x-coordinate.  If false the method searches for min or max
   *            y-coordinate.
   * @param searchLongitude Indicates whether the method will search 
   *            along a longitude. If true, constBorder is longitude and all
   *            other borders are latitudes. If false, the method searches a
   *            latitude (constBorder is a lat, other borders lons).
   * @param findMin Indicates whether the method is looking for a minimum 
   *            coordinate value. If false, the method is looking for a maximum
   *            value.
   * @author Steven Lambright 
   * @internal
   *   @history 2011-07-02 Jeannie Backer - Moved this code from
   *                           ObliqueCylindrical class to its own method
   *                           here. Added condition to do-while loop for
   *                           more precision. Replaced hard-coded
   *                           TOLERANCE with 1/2 pixel resolution.
   *                           (Defaults to 0.5)
   */
  void Projection::doSearch(double minBorder, double maxBorder, 
                            double &extremeVal, const double constBorder, 
                            bool searchX, bool searchLongitude, bool findMin) {
    if (minBorder == Null || maxBorder == Null || constBorder == Null) {
      return;
    }
    const double TOLERANCE = m_pixelResolution/2;
    const int NUM_ATTEMPTS = (unsigned int)DBL_DIG; // It's unsafe to go past 
                                                    // this precision

    double minBorderX, minBorderY, maxBorderX, maxBorderY;
    int attempts = 0;

    do {
      findExtreme(minBorder, maxBorder, minBorderX, minBorderY, maxBorderX, 
                  maxBorderY, constBorder, searchX, searchLongitude, findMin);
      if (minBorderX == Null && maxBorderX == Null 
          && minBorderY == Null && maxBorderY == Null ) {
        attempts = NUM_ATTEMPTS;
        continue;
      }
      attempts ++;
    }
    while ((fabs(minBorderX - maxBorderX) > TOLERANCE 
           || fabs(minBorderY - maxBorderY) > TOLERANCE)
           && (attempts < NUM_ATTEMPTS)); 
    // check both x and y distance in case symmetry of map
    // For example, if minBorderX = maxBorderX but minBorderY = -maxBorderY,
    // these points may not be close enough.

    if (attempts >= NUM_ATTEMPTS) {
      // We zoomed in on a discontinuity because our range never shrank, this
      // will need to be rechecked later. 
      // *min and max border should be nearly identical, so it doesn't matter
      //  which is used here
      if (searchLongitude) {
        m_specialLatCases.push_back(minBorder);
      }
      else {
        m_specialLonCases.push_back(minBorder);
      }
    }

    // These values will always be accurate, even over a discontinuity
    if (findMin) {
      if (searchX) extremeVal = min(minBorderX, maxBorderX);
      else         extremeVal = min(minBorderY, maxBorderY);
    }
    else {
      if (searchX) extremeVal = max(minBorderX, maxBorderX);
      else         extremeVal = max(minBorderY, maxBorderY);
    }
    return;
  }

  /**
   * Searches for extreme (min/max/discontinuity) coordinate values across 
   * latitudes/longitudes. 
   *  
   * This method looks for these extrema along the constBorder between minBorder 
   * and maxBorder by stepping along constBorder (10 times) from the minBorder 
   * and maxBorder. Then, the range of this extreme value is recorded in 
   * minBorder and maxBorder and the coordinate values corresponding to these 
   * new borders are stored in minBorderX, minBorderY, maxBorderX and 
   * maxBorderY. 
   *  
   * This function should be used by calling it repeatedly until minBorderX and 
   * minBorderY do not equal maxBorderX and maxBorderY, respectively. 
   * Discontinuities will cause the minBorderX, minBorderY, maxBorderX and 
   * maxBorderY to never converge. If minBorderX never comes close to maxBorderX 
   * or minBorderY never comes close to maxBorderY, then between minBorder and 
   * maxBorder is the value of the most extreme value. In this case, either the 
   * smaller or larger of the x or y values found will be correct, depending on 
   * the values of findMin and searchX. 
   *  
   *  
   * 
   *
   * @param minBorder Minimum latitude or longitude to search between. This 
   *            value gets updated to a more precise range.
   * @param maxBorder Maximum latitude or longitude to search between. This 
   *            value gets updated to a more precise range.
   * @param minBorderX The x-value corresponding to the lower resultant 
   *            minBorder and the constBorder, which is more accurate when
   *            nearly equal to maxBorderX.
   * @param minBorderY The y-value corresponding to the lower resultant 
   *            minBorder and the constBorder, which is more accurate when
   *            nearly equal to maxBorderY.
   * @param maxBorderX The x-value corresponding to the higher resultant 
   *            maxBorder and the constBorder, which is more accurate when
   *            nearly equal to minBorderX.
   * @param maxBorderY The y-value corresponding to the higher resultant 
   *            maxBorder and the constBorder, which is more accurate when
   *            nearly equal to minBorderY.
   * @param constBorder The latitude or longitude that remains constant.  The 
   *            method will step along this border.
   * @param searchX Indicates whether the method is searching for a min or max 
   *            x-coordinate.  If false the method searches for min or max
   *            y-coordinate.
   * @param searchLongitude Indicates whether the method will search 
   *            along a longitude. If true, constBorder is longitude and all
   *            other borders are latitudes. If false, the method searches a
   *            latitude (constBorder is a lat, other borders lons).
   * @param findMin Indicates whether the method is looking for a minimum 
   *            coordinate value. If false, the method is looking for a maximum
   *            value.
   * @author Stephen Lambright 
   * @internal
   *   @history 2011-07-02 Jeannie Backer - Moved this code from
   *                           ObliqueCylindrical class to its own method here.
   *                           Replaced parameters minVal and maxVal with
   *                           minBorderX, minBorderY, maxBorderX, and
   *                           maxBorderY.
   */
  void Projection::findExtreme(double &minBorder,  double &maxBorder,
                               double &minBorderX, double &minBorderY,
                               double &maxBorderX, double &maxBorderY, 
                               const double constBorder, bool searchX, 
                               bool searchLongitude, bool findMin) {
    if (minBorder == Null || maxBorder == Null || constBorder == Null) {
      minBorderX = Null;
      minBorderY = minBorderX;
      minBorderX = minBorderX;
      minBorderY = minBorderX;
      return;
    }
    if (!searchLongitude && (fabs(fabs(constBorder) - 90.0) < DBL_EPSILON)) {
      // it is impossible to search "along" a pole
      setSearchGround(minBorder, constBorder, searchLongitude);
      minBorderX = XCoord();
      minBorderY = YCoord();
      maxBorderX = minBorderX;
      maxBorderY = minBorderY;
      return;
    }
    // Always do 10 steps
    const double STEP_SIZE = (maxBorder - minBorder) / 10.0;
    const double LOOP_END = maxBorder + (STEP_SIZE / 2.0); // This ensures we do 
                                                           // all of the steps
                                                           // properly
    double currBorderVal = minBorder;
    setSearchGround(minBorder, constBorder, searchLongitude);

    // this makes sure that the initial currBorderVal is valid before entering
    // the loop below
    if (!m_good){
      // minBorder = currBorderVal+STEP_SIZE < LOOP_END until setGround is good?
      // then, if still not good return?
      while (!m_good && currBorderVal <= LOOP_END) {
        currBorderVal+=STEP_SIZE;
        if (searchLongitude && (currBorderVal - 90.0 > DBL_EPSILON)) {
          currBorderVal = 90.0;
        }
        setSearchGround(currBorderVal, constBorder, searchLongitude);
      }
      if (!m_good) {
        minBorderX = Null;
        minBorderY = minBorderX;
        minBorderX = minBorderX;
        minBorderY = minBorderX;
        return;
      }
    }

    // save the values of three consecutive steps from the minBorder towards
    // the maxBorder along the constBorder. initialize these three border
    // values (the non-constant lat or lon)
    double border1 = currBorderVal;
    double border2 = currBorderVal;
    double border3 = currBorderVal;

    // save the coordinate (x or y) values that correspond to the first
    // two borders that are being saved.
    // initialize these two coordinate values (x or y)
    double value1 = (searchX) ? XCoord() : YCoord();
    double value2 = value1;

    // initialize the extreme coordinate value 
    // -- this is the largest coordinate value found so far
    double extremeVal2 = value2;

    // initialize the extreme border values
    // -- these are the borders on either side of the extreme coordinate value
    double extremeBorder1 = minBorder;
    double extremeBorder3 = minBorder;

    while (currBorderVal <= LOOP_END) {

      // this conditional was added to prevent trying to SetGround with an
      // invalid latitude greater than 90 degrees. There is no need check for
      // latitude less than -90 since we start at the minBorder (already
      // assumed to be valid) and step forward toward (and possibly past)
      // maxBorder
      if (searchLongitude && (currBorderVal - 90.0 > DBL_EPSILON)) {
        currBorderVal = 90.0;
      }

      // update the current border value along constBorder
      currBorderVal += STEP_SIZE;
      setSearchGround(currBorderVal, constBorder, searchLongitude);
      if (!m_good){ 
        continue;
      } 
                     
      // update the border and coordinate values 
      border3 = border2;
      border2 = border1;
      border1 = currBorderVal;                                             
      value2 = value1;
      value1 = (searchX) ? XCoord() : YCoord();

      if ((findMin && value2 < extremeVal2) 
          || (!findMin && value2 > extremeVal2)) {
        // Compare the coordinate value associated with the center border with 
        // the current extreme. If the updated coordinate value is more extreme
        // (smaller or larger, depending on findMin), then we update the
        // extremeVal and it's borders.
        extremeVal2 = value2;

        extremeBorder3 = border3;
        extremeBorder1 = border1;
      }
    }

    // update min/max border values to the values on either side of the most 
    // extreme coordinate found in this call to this method
    
    minBorder = extremeBorder3; // Border 3 is lagging and thus smaller

    // since the loop steps past the original maxBorder, we want to retain 
    // the original maxBorder value so we don't go outside of the original
    // min/max range given
    if (extremeBorder1 <= maxBorder ) {
      maxBorder = extremeBorder1; // Border 1 is leading and thus larger
    }

    // update minBorder coordinate values
    setSearchGround(minBorder, constBorder, searchLongitude);
    // if (!m_good){
    //   this should not happen since minBorder has already been verified in 
    //   the while loop above
    // }

    minBorderX = XCoord();
    minBorderY = YCoord();

    // update maxBorder coordinate values
    setSearchGround(maxBorder, constBorder, searchLongitude);
    // if (!m_good){
    //   this should not happen since maxBorder has already been verified in
    //   the while loop above
    // }

    maxBorderX = XCoord();
    maxBorderY = YCoord();
    return;
  }

  /**
   * This function sets the ground for the given border values.  It calls the 
   * SetGround(lat, lon) method with the appropriate lat/lon values, depending 
   * on whether variableIsLat is true. 
   *  
   * This method is used by doSearch and findExtreme in order to set the ground 
   * correctly each time. 
   *
   * @param variableBorder The latitude or longitude that is variable in the 
   *            search methods.
   * @param constBorder The latitude or longitude that is constant in the search 
   *            methods.
   * @param variableIsLat Indicates whether variableBorder is the latittude 
   *            value and constBorder is the longitude value. If false,
   *            variableBorder is the longitude value and constBorder is the
   *            latitude value.
   * @author Stephen Lambright 
   * @internal
   *   @history 2011-07-02 Jeannie Backer - Moved this code from
   *                           ObliqueCylindrical class to its own method here.
   *                           Added error.
   */
  void Projection::setSearchGround(const double variableBorder, 
                                   const double constBorder, 
                                   bool variableIsLat) {
    if (variableBorder == Null || constBorder == Null) {
      return;
    }
    double lat, lon;
    if (variableIsLat) {
      lat = variableBorder;
      lon = constBorder;
    }
    else {
      lat = constBorder;
      lon = variableBorder;
    }
    SetGround(lat, lon);
    return;
  }

  /**
   * This method sets the UpperLeftCornerX and UpperLeftCornerY keywords in the 
   * projection mapping group, in meters.
   *
   * @param x the upper left corner x value
   * @param y the upper left corner y value
   */
  void Projection::SetUpperLeftCorner(const Displacement &x, 
                                      const Displacement &y) {
    PvlKeyword xKeyword("UpperLeftCornerX", toString(x.meters()), "meters");
    PvlKeyword yKeyword("UpperLeftCornerY", toString(y.meters()), "meters");
    m_mappingGrp.AddKeyword(xKeyword,Pvl::Replace);
    m_mappingGrp.AddKeyword(yKeyword,Pvl::Replace);
  }

  /**
   * This function returns the keywords that this projection uses.
   *
   * @return PvlGroup The keywords that this projection uses
   */
  PvlGroup Projection::Mapping() {
    PvlGroup mapping("Mapping");

    if (m_mappingGrp.HasKeyword("TargetName")) {
      mapping += m_mappingGrp["TargetName"];
    }

    mapping += m_mappingGrp["ProjectionName"];
    mapping += m_mappingGrp["EquatorialRadius"];
    mapping += m_mappingGrp["PolarRadius"];
    mapping += m_mappingGrp["LatitudeType"];
    mapping += m_mappingGrp["LongitudeDirection"];
    mapping += m_mappingGrp["LongitudeDomain"];

    if (m_mappingGrp.HasKeyword("PixelResolution")) {
      mapping += m_mappingGrp["PixelResolution"];
    }
    if (m_mappingGrp.HasKeyword("Scale")) {
      mapping += m_mappingGrp["Scale"];
    }
    if (m_mappingGrp.HasKeyword("UpperLeftCornerX")) {
      mapping += m_mappingGrp["UpperLeftCornerX"];
    }
    if (m_mappingGrp.HasKeyword("UpperLeftCornerY")) {
      mapping += m_mappingGrp["UpperLeftCornerY"];
    }

    if (HasGroundRange()) {
      mapping += m_mappingGrp["MinimumLatitude"];
      mapping += m_mappingGrp["MaximumLatitude"];
      mapping += m_mappingGrp["MinimumLongitude"];
      mapping += m_mappingGrp["MaximumLongitude"];
    }

    if (m_mappingGrp.HasKeyword("Rotation")) {
      mapping += m_mappingGrp["Rotation"];
    }

    return mapping;
  }

  /**
   * This function returns the latitude keywords that this projection uses
   *
   * @return PvlGroup The latitude keywords that this projection uses
   */
  PvlGroup Projection::MappingLatitudes() {
    PvlGroup mapping("Mapping");

    if (HasGroundRange()) {
      mapping += m_mappingGrp["MinimumLatitude"];
      mapping += m_mappingGrp["MaximumLatitude"];
    }

    return mapping;
  }

  /**
   * This function returns the longitude keywords that this projection uses
   *
   * @return PvlGroup The longitude keywords that this projection uses
   */
  PvlGroup Projection::MappingLongitudes() {
    PvlGroup mapping("Mapping");

    if (HasGroundRange()) {
      mapping += m_mappingGrp["MinimumLongitude"];
      mapping += m_mappingGrp["MaximumLongitude"];
    }

    return mapping;
  }

  /**
   * A convience method to compute Snyder's @a q equation (3-12) for a given 
   * latitude, @f$\phi@f$ 
   *  
   * @f[ 
   * q = (1 - e^2) \left[ \frac{\sin(\phi)}{1 - e^2 
   * \sin^2(\phi)} 
   * - 
   * \frac{1}{2e} \ln\left(\frac{1 - e \sin(\phi)}{1 + e 
   *      \sin(\phi)}\right) \right]
   * @f] 
   * where @f$e@f$ is the eccentricity for the body. 
   * 
   * @param sinPhi The sine value for a latitude, phi. 
   *  
   * @throw IException::Unknown - "Snyder's q variable should only be 
   *            computed for ellipsoidal projections."
   *  
   * @return @b double Value for Snyder's q variable. 
   */
  double Projection::qCompute(const double sinPhi) const {
    if (m_eccentricity < DBL_EPSILON) {
      QString msg = "Snyder's q variable should only be computed for "
                    "ellipsoidal projections.";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
    double eSinPhi = m_eccentricity * sinPhi;
    return (1 - m_eccentricity * m_eccentricity)
           * (sinPhi / (1 - eSinPhi * eSinPhi) 
               - 1 / (2 * m_eccentricity) * log( (1 - eSinPhi) / (1 + eSinPhi) ));
           // Note: We know that q is well defined since 
           //   0 < e < 1 and -1 <= sin(phi) <= 1 
           //   implies that -1 < e*sin(phi) < 1
           //   Thus, there are no 0 denominators and the log domain is 
           //   satisfied, (1-e*sin(phi))/(1+e*sin(phi)) > 0
  }

  /**
   * A convience method to compute latitude angle phi2 given small t, from 
   * Syder's recursive equation (7-9)
   *  
   * @f[ 
   * \phi_{i+1} = \frac{\pi}{2} - 2\arctan\left(t 
   *      \left[\frac{1-e\sin(\phi_i)}{1+e\sin(\phi_i)}\right]^{e/2}\right)
   * @f]
   * where @f$e@f$ is the eccentricity for the body and @f$ \phi_0 = 
   * \frac{\pi}{2} - 2\arctan(t) @f$ . 
   *  
   * @param t small t
   *  
   * @throw IException::Unknown - "Failed to converge in Projection::phi2Compute()"
   * @return double The value for the latitude.
   */
  double Projection::phi2Compute(const double t) const {
    double localPhi = HALFPI - 2.0 * atan(t);
    double halfEcc = 0.5 * Eccentricity();
    double difference = DBL_MAX;
    int iteration = 0;
    // a failure in this loop means an exception will be thrown, which is
    //   an expensive operation. So letting let the loop iterate quite a bit
    //   is not a big deal. Also, the user will be unable to project at all
    //   if this fails so more reason to let this loop iterate: better to
    //   function slow than not at all.
    const int MAX_ITERATIONS = 45;

    while ((iteration < MAX_ITERATIONS) && (difference > 0.0000000001))  {
      double eccTimesSinphi = Eccentricity() * sin(localPhi);
      double newPhi = HALFPI -
                      2.0 * atan(t * pow((1.0 - eccTimesSinphi) /
                                         (1.0 + eccTimesSinphi), halfEcc));
      difference = fabs(newPhi - localPhi);
      localPhi = newPhi;
      iteration++;
    }

    if (iteration >= MAX_ITERATIONS) {
      QString msg = "Failed to converge in Projection::phi2Compute()";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    return localPhi;
  }

  /**
   * A convience method to compute Snyder's @a m equation (14-15) for a given 
   * latitude, @f$\phi@f$ 
   *  
   * @f[ 
   * m = \frac{\cos(\phi)}{\sqrt{1-e^2 \sin^2(\phi)}} 
   * @f] 
   * where @f$e@f$ is the eccentricity for the body. 
   *  
   * @param sinphi sine of phi
   * @param cosphi cosine of phi
   *
   * @return double Value for Snyder's m variable.
   */
  double Projection::mCompute(const double sinphi, const double cosphi) const {
    double eccTimesSinphi = Eccentricity() * sinphi;
    double denominator = sqrt(1.0 - eccTimesSinphi * eccTimesSinphi);
    return cosphi / denominator;
  }

  /**
   * A convience method to compute Snyder's @a t equation (15-9) for a given 
   * latitude, @f$\phi@f$ 
   *  
   * @f[ 
   * t = 
   * \frac{\tan\left(\frac{\pi}{4} - \frac{\phi}{2}\right)}
   *      {\left[\frac{1-e\sin(\phi)}
   *             {1+e\sin(\phi)}\right]^{e/2}}
   * @f] 
   * where @f$e@f$ is the eccentricity for the body. 
   *
   * @param phi  phi
   * @param sinphi sin of phi
   *
   * @return double The value for Snyder's t variable.
   */
  double Projection::tCompute(const double phi, const double sinphi) const {
    if ((HALFPI) - fabs(phi) < DBL_EPSILON) return 0.0;

    double eccTimesSinphi = Eccentricity() * sinphi;
    double denominator  = pow((1.0 - eccTimesSinphi) /
                              (1.0 + eccTimesSinphi),
                              0.5 * Eccentricity());
    return tan(0.5 * (HALFPI - phi)) / denominator;
  }

  /**
   * A convience method to compute 
   *  
   * @f[ 
   * e4 = 
   * \sqrt{(1+e )^{1+e}(1-e)^{1-e}}
   * @f] 
   * where @a e is the eccentricity of the body.
   *  
   * @return double The value for the e4 formula.
   */
  double Projection::e4Compute() const {
    double onePlusEcc = 1.0 + Eccentricity();
    double oneMinusEcc = 1.0 - Eccentricity();

    return sqrt(pow(onePlusEcc, onePlusEcc) *
                pow(oneMinusEcc, oneMinusEcc));
  }

} //end namespace isis


