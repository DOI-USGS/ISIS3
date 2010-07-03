/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2008/05/09 18:49:25 $
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

#include <cmath>
#include <cfloat>
#include "TransverseMercator.h"
#include "iException.h"
#include "Constants.h"
#include "Projection.h"

using namespace std;
namespace Isis {
 /**
  * Constructs a TransverseMercator object
  *
  * @param label This argument must be a Label containing the proper mapping
  *              information as indicated in the Projection class. Additionally,
  *              the transversemercator projection requires the center longitude
  *              to be defined in the keyword CenterLongitude, and the scale
  *              factor to be defined in the keyword ScaleFactor.
  *
  * @param allowDefaults If set to false the constructor expects that a keyword
  *                      of CenterLongitude and ScaleFactor will be in the label.
  *                      Otherwise it will attempt to compute the center
  *                      longitude using the middle of the longitude range
  *                      specified in the label, and the scale factor will
  *                      default to 1.0. Defaults to false.
  *
  * @throws Isis::iException::Io
  */
  TransverseMercator::TransverseMercator(Isis::Pvl &label, bool allowDefaults) :
    Isis::Projection::Projection (label) {
    try {
      // Try to read the mapping group
      Isis::PvlGroup &mapGroup = label.FindGroup ("Mapping",Isis::Pvl::Traverse);

      // Compute and write the default center longitude if allowed and
      // necessary
      if ((allowDefaults) && (!mapGroup.HasKeyword("CenterLongitude"))) {
        double lon = (p_minimumLongitude + p_maximumLongitude) / 2.0;
        mapGroup += Isis::PvlKeyword("CenterLongitude",lon);
      }

      // Compute and write the default center latitude if allowed and
      // necessary
      if ((allowDefaults) && (!mapGroup.HasKeyword("CenterLatitude"))) {
        double lat = (p_minimumLatitude + p_maximumLatitude) / 2.0;
        mapGroup += Isis::PvlKeyword("CenterLatitude",lat);
      }

      // Get the center longitude  & latitude
      p_centerLongitude = mapGroup["CenterLongitude"];
      p_centerLatitude = mapGroup["CenterLatitude"];

      // make sure the center latitude value is valid
      if (fabs(p_centerLatitude) >= 90.0) {
        string msg = "Invalid Center Latitude Value. Must be between -90 and 90";
        throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
      }

      // make sure the center longitude value is valid
      if (fabs(p_centerLongitude) > 360.0) {
        string msg = "Invalid Center Longitude Value. Must be between -360 and 360";
        throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
      }

      // convert latitude to planetographic if it is planetocentric
      if (this->IsPlanetocentric()) {
        p_centerLatitude = this->ToPlanetographic(p_centerLatitude);
      }

      // convert to radians and adjust for longitude direction
      if (p_longitudeDirection == PositiveWest) p_centerLongitude *= -1.0;
      p_centerLatitude *= Isis::PI / 180.0;
      p_centerLongitude *= Isis::PI / 180.0;

      // Compute other necessary variables
      p_eccsq = Eccentricity()*Eccentricity();
      p_esp = p_eccsq;
      p_e0 = 1.0 - 0.25 * p_eccsq * (1.0 + p_eccsq / 16.0 * (3.0 + 1.25 *p_eccsq));
      p_e1 = 0.375 * p_eccsq * (1.0 + 0.25 * p_eccsq * ( 1.0 + 0.468750 * p_eccsq));
      p_e2 = 0.058593750 * p_eccsq * p_eccsq * (1.0 + 0.750 * p_eccsq);
      p_e3 = p_eccsq * p_eccsq * p_eccsq * (35.0 / 3072.0);
      p_ml0 = p_equatorialRadius * (p_e0 * p_centerLatitude - p_e1 * sin(2.0 * p_centerLatitude) +
                p_e2 * sin(4.0 * p_centerLatitude) - p_e3 * sin(6.0 * p_centerLatitude));

      // Set flag for sphere or ellipsiod
      p_sph = true; // Sphere
      if (Eccentricity() >= .00001) {
        p_sph = false; // Ellipsoid
        p_esp = p_eccsq / (1.0 - p_eccsq);
      }

      // Get the scale factor
      if ((allowDefaults) && (!mapGroup.HasKeyword("ScaleFactor"))) {
        mapGroup += Isis::PvlKeyword("ScaleFactor",1.0);
      }
      p_scalefactor = mapGroup["ScaleFactor"];
    }
    catch (Isis::iException &e) {
      string message = "Invalid label group [Mapping]";
      throw Isis::iException::Message(Isis::iException::Io,message,_FILEINFO_);
    }
  }

  //! Destroys the TransverseMercator object
  TransverseMercator::~TransverseMercator() {
  }

 /**
  * This method is used to set the latitude/longitude (assumed to be of the
  * correct LatitudeType, LongitudeDirection, and LongitudeDomain. The Set
  * forces an attempted calculation of the projection X/Y values. This may or
  * may not be successful and a status is returned as such.
  *
  * @param lat Latitude value to project
  *
  * @param lon Longitude value to project
  *
  * @return bool
  */
  bool TransverseMercator::SetGround(const double lat,const double lon) {
    // Get longitude & fix direction
    p_longitude = lon;
    if (p_longitudeDirection == PositiveWest) p_longitude *= -1.0;

    double cLonDeg = p_centerLongitude * 180.0 / Isis::PI;
    double deltaLon = p_longitude - cLonDeg;
    while (deltaLon < -360.0) deltaLon += 360.0;
    while (deltaLon > 360.0) deltaLon -= 360.0;
    double deltaLonRads = deltaLon * Isis::PI / 180.0;

    // Now convert latitude to radians & clean up ... it must be planetographic
    p_latitude = lat;
    double latRadians = p_latitude * Isis::PI / 180.0;
    if (IsPlanetocentric()) {
      latRadians = ToPlanetographic(p_latitude) * Isis::PI / 180.0;
    }

    double ml = p_equatorialRadius * (p_e0 * latRadians - p_e1 * sin(2.0 * latRadians)
                 + p_e2 * sin(4.0 * latRadians) - p_e3 * sin(6.0 * latRadians));

    // Declare variables
    const double epsilon = 1.0e-10;

    // Sphere Conversion
    double x,y;
    if (p_sph) {
       double cosphi = cos(latRadians);
       double b = cosphi * sin(deltaLonRads);

       // Point projects to infinity
       if (fabs(fabs(b) - 1.0) <= epsilon) {
         p_good = false;
         return p_good;
       }
       x = 0.5 * p_equatorialRadius * p_scalefactor * log((1.0 + b) / (1.0 - b));

       // If arcosine argument is too close to 1, con=0.0 because arcosine(1)=0
       double con = cosphi * cos(deltaLonRads) / sqrt(1.0 - b * b);
       if (fabs(con) > 1.0) {
         con = 0.0;
       }
       else {
         con = acos(con);
       }
       if(p_latitude < 0.0) con = -con;
       y = p_equatorialRadius * p_scalefactor * (con - p_centerLatitude);
    }

    // Ellipsoid Conversion
    else {
      if(fabs(Isis::HALFPI - fabs(latRadians)) < epsilon) {
        x = 0.0;
        y = p_scalefactor * (ml - p_ml0);
      }
      else {
        double sinphi = sin(latRadians);
        double cosphi = cos(latRadians);
        double al = cosphi * deltaLonRads;
        double als = al * al;
        double c = p_esp * cosphi * cosphi;
        double tq = tan(latRadians);
        double t = tq * tq;
        double n = p_equatorialRadius / sqrt (1.0 - p_eccsq * sinphi * sinphi);
        x = p_scalefactor * n * al * (1.0 + als / 6.0 * (1.0 - t + c + als
                  / 20.0 * (5.0 - 18.0 * t + t * t + 72.0 * c - 58.0 * p_esp)));
        y = p_scalefactor *(ml - p_ml0 + n * tq * (als * (0.5 + als / 24.0 *
                  (5.0 - t + 9.0 * c + 4.0 * c * c + als / 30.0 *
                  (61.0 - 58.0 * t + t * t + 600.0 * c - 330.0 * p_esp)))));
      }
    }

    SetComputedXY(x,y);
    p_good = true;
    return p_good;
  }

 /**
  * This method is used to set the projection x/y. The Set forces an attempted
  * calculation of the corresponding latitude/longitude position. This may or
  * may not be successful and a status is returned as such.
  *
  * @param x X coordinate of the projection in units that are the same as the
  *          radii in the label
  *
  * @param y Y coordinate of the projection in units that are the same as the
  *          radii in the label
  *
  * @return bool
  */
  bool TransverseMercator::SetCoordinate(const double x, const double y) {
    // Save the coordinate
    SetXY(x,y);

    // Declare & Initialize variables
    double f,g,h,temp,con,phi,dphi,sinphi,cosphi,tanphi;
    double c,cs,t,ts,n,rp,d,ds;
    const double epsilon = 1.0e-10;

    // Sphere Conversion
    if (p_sph) {
      f = exp(GetX() / (p_equatorialRadius * p_scalefactor));
      g = 0.5 * (f - 1.0 / f);
      temp = p_centerLatitude + GetY() / (p_equatorialRadius * p_scalefactor);
      h = cos(temp);
      con = sqrt((1.0 - h * h) / (1.0 + g * g));
      if (con > 1.0) con = 1.0;
      if (con < -1.0) con = -1.0;
      p_latitude = asin(con);
      if (temp < 0.0 ) p_latitude = -p_latitude;
      p_longitude = p_centerLongitude;
      if (g != 0.0 || h != 0.0) {
        p_longitude = atan2(g,h) + p_centerLongitude;
      }
    }

    // Ellipsoid Conversion
    else if (!p_sph) {
      con = (p_ml0 + GetY() / p_scalefactor) / p_equatorialRadius;
      phi = con;
      for (int i = 1; i < 7; i++) {
        dphi = ((con + p_e1 * sin(2.0 * phi) - p_e2 * sin(4.0 * phi)
                 + p_e3 * sin(6.0 * phi)) / p_e0) - phi;
        phi += dphi;
        if (fabs(dphi) <= epsilon) break;
      }

      // Didn't converge
      if (fabs(dphi) > epsilon) {
        p_good = false;
        return p_good;
      }
      if (fabs(phi) >= Isis::HALFPI) {
        if (GetY() >= 0.0) p_latitude = fabs(Isis::HALFPI);
        if (GetY() < 0.0) p_latitude = - fabs(Isis::HALFPI);
        p_longitude = p_centerLongitude;
      }
      else {
        sinphi = sin(phi);
        cosphi = cos(phi);
        tanphi = tan(phi);
        c = p_esp * cosphi * cosphi;
        cs = c * c;
        t = tanphi * tanphi;
        ts = t * t;
        con = 1.0 - p_eccsq * sinphi * sinphi;
        n = p_equatorialRadius / sqrt(con);
        rp = n * (1.0 - p_eccsq) / con;
        d = GetX() / (n * p_scalefactor);
        ds = d * d;
        p_latitude = phi - (n * tanphi * ds / rp) * (0.5 - ds /
                      24.0 * (5.0 + 3.0 * t + 10.0 * c - 4.0 * cs - 9.0 *
                      p_esp - ds / 30.0 * (61.0 + 90.0 * t + 298.0 * c +
                      45.0 * ts - 252.0 * p_esp - 3.0 * cs)));


        // Latitude cannot be greater than + or - halfpi radians (or 90 degrees)
        if (fabs(p_latitude) > Isis::HALFPI) {
          p_good = false;
          return p_good;
        }
        p_longitude = p_centerLongitude + (d * (1.0 - ds / 6.0 *
                      (1.0 + 2.0 * t + c - ds / 20.0 * (5.0 - 2.0 * c +
                      28.0 * t - 3.0 * cs + 8.0 * p_esp + 24.0 * ts))) / cosphi);
      }
    }

    // Convert to Degrees
    p_latitude *= 180.0 / Isis::PI;
    p_longitude *= 180.0 / Isis::PI;

    // Cleanup the longitude
    if (p_longitudeDirection == PositiveWest) p_longitude *= -1.0;
  // These need to be done for circular type projections
    p_longitude = To360Domain (p_longitude);
    if (p_longitudeDomain == 180) p_longitude = To180Domain(p_longitude);

    // Cleanup the latitude
    if (IsPlanetocentric()) p_latitude = ToPlanetocentric(p_latitude);

    p_good = true;
    return p_good;
  }

 /**
  * This method is used to determine the x/y range which completely covers the
  * area of interest specified by the lat/lon range. The latitude/longitude
  * range may be obtained from the labels. The purpose of this method is to
  * return the x/y range so it can be used to compute how large a map may need
  * to be. For example, how big a piece of paper is needed or how large of an
  * image needs to be created. The method may fail as indicated by its return
  * value.
  *
  * @param minX Minimum x projection coordinate which covers the latitude
  *             longitude range specified in the labels.
  *
  * @param maxX Maximum x projection coordinate which covers the latitude
  *             longitude range specified in the labels.
  *
  * @param minY Minimum y projection coordinate which covers the latitude
  *             longitude range specified in the labels.
  *
  * @param maxY Maximum y projection coordinate which covers the latitude
  *             longitude range specified in the labels.
  *
  * @return bool
  */
  bool TransverseMercator::XYRange(double &minX, double &maxX, double &minY, double&maxY){
    // Check the corners of the lat/lon range
    XYRangeCheck (p_minimumLatitude,p_minimumLongitude);
    XYRangeCheck (p_maximumLatitude,p_minimumLongitude);
    XYRangeCheck (p_minimumLatitude,p_maximumLongitude);
    XYRangeCheck (p_maximumLatitude,p_maximumLongitude);

    // convert center latitude to degrees & test
    double clat = p_centerLatitude * 180.0 / Isis::PI;

    if (clat > p_minimumLatitude &&
        clat < p_maximumLatitude) {
      XYRangeCheck (clat, p_minimumLongitude);
      XYRangeCheck (clat, p_maximumLongitude);
    }

    // convert center longitude to degrees & test
    double clon = p_centerLongitude * 180.0 / Isis::PI;
    if (clon > p_minimumLongitude &&
        clon < p_maximumLongitude) {
      XYRangeCheck (p_minimumLatitude, clon);
      XYRangeCheck (p_maximumLatitude, clon);
    }

    // Make sure everything is ordered
    if (p_minimumX >= p_maximumX) return false;
    if (p_minimumY >= p_maximumY) return false;

    // Return X/Y min/maxs
    minX = p_minimumX;
    maxX = p_maximumX;
    minY = p_minimumY;
    maxY = p_maximumY;
    return true;
  }


  /**
   * This function returns the keywords that this projection uses.
   * 
   * @return PvlGroup The keywords that this projection uses
   */
  PvlGroup TransverseMercator::Mapping() {
    PvlGroup mapping = Projection::Mapping();

    mapping += p_mappingGrp["CenterLatitude"];
    mapping += p_mappingGrp["CenterLongitude"];
    mapping += p_mappingGrp["ScaleFactor"];

    return mapping;
  }

  /**
   * This function returns the latitude keywords that this projection uses
   * 
   * @return PvlGroup The latitude keywords that this projection uses
   */
  PvlGroup TransverseMercator::MappingLatitudes() {
    PvlGroup mapping = Projection::MappingLatitudes();

    mapping += p_mappingGrp["CenterLatitude"];

    return mapping;
  }

  /**
   * This function returns the longitude keywords that this projection uses
   * 
   * @return PvlGroup The longitude keywords that this projection uses
   */
  PvlGroup TransverseMercator::MappingLongitudes() {
    PvlGroup mapping = Projection::MappingLongitudes();

    mapping += p_mappingGrp["CenterLongitude"];

    return mapping;
  }

 /**
  * Compares two Projection objects to see if they are equal
  *
  * @param proj Projection object to do comparison on
  *
  * @return bool Returns true if the Projection objects are equal, and false if
  *              they are not
  */
  bool TransverseMercator::operator== (const Isis::Projection &proj) {
    if (!Isis::Projection::operator==(proj)) return false;
  // dont do the below it is a recusive plunge
  //  if (Isis::Projection::operator!=(proj)) return false;
    TransverseMercator *trans = (TransverseMercator *) &proj;
    if ((trans->p_centerLongitude != this->p_centerLongitude) ||
        (trans->p_centerLatitude != this->p_centerLatitude)) return false;
    return true;
  }
} // end namespace isis

extern "C" Isis::Projection *TransverseMercatorPlugin (Isis::Pvl &lab,
                                             bool allowDefaults) {
 return new Isis::TransverseMercator(lab,allowDefaults);
}


