/**
 * @file
 * $Revision: 1.4 $
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
#include <iostream>
#include "PolarStereographic.h"
#include "iException.h"
#include "Constants.h"

using namespace std;
namespace Isis {
 /**
  * Constructs a PolarStereographic object.
  *
  * @param label This argument must be a Pvl containing the proper mapping
  *              information as indicated in the Projection class. Additionally,
  *              the polar stereographic projection requires the center
  *              longitude and latitude to be defined in the keyword
  *              CenterLongitude and CenterLatitude respectively.
  *
  * @param allowDefaults If set to false the constructor expects that a keyword
  *                      of CenterLongitude and CenterLatitude will be in the
  *                      label. Otherwise it will attempt to compute them using
  *                      the middle of the longitude range specified in the
  *                      labels. The center latitude will be set to one of the
  *                      poles depending on the average of the latitude range.
  *                      Defaults to false.
  *
  * @throws Isis::iException::Projection - An error is thrown if the label does
  *                                        not contain the keyword
  *                                        CenterLongitude or latitude.
  */
  PolarStereographic::PolarStereographic(Isis::Pvl &label, bool allowDefaults) :
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
        if (lat > 0.0) {
          mapGroup += Isis::PvlKeyword("CenterLatitude",90.0);
        }
        else {
          mapGroup += Isis::PvlKeyword("CenterLatitude",-90.0);
        }
      }

      // Get the center longitude, convert to radians and adjust for longitude
      // direction
      p_centerLongitude = mapGroup["CenterLongitude"];
      p_centerLongitude *= Isis::PI / 180.0;
      if (p_longitudeDirection == PositiveWest) p_centerLongitude *= -1.0;

      // Get the center latitude, make sure it is ographic, and convert to
      // radians.
      p_centerLatitude = mapGroup["CenterLatitude"];
      if (p_centerLatitude == 0) {
        string msg = "Invalid value for keyword [CenterLatitude] in map file.";
        msg += "  CenterLatitude cannot equal 0.0";
        throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
      }
      if (this->IsPlanetocentric()) {
        p_centerLatitude = this->ToPlanetographic(p_centerLatitude);
      }
      p_centerLatitude *= Isis::PI / 180.0;

      // Compute some constants
      p_e4 = e4Compute ();
      p_signFactor = 1.0;
      if (p_centerLatitude < 0.0) p_signFactor = -1.0;

      if ((Isis::HALFPI - fabs(p_centerLatitude)) > DBL_EPSILON) {
        p_poleFlag = true;  // We are not at a pole
        double phi = p_signFactor * p_centerLatitude;
        double sinphi = sin(phi);
        double cosphi = cos(phi);
        p_m = mCompute (sinphi,cosphi);
        p_t = tCompute (phi,sinphi);
        if (fabs(p_t) < DBL_EPSILON) p_poleFlag = false;
      }
      else {
        p_poleFlag = false;  // Implies we are at a pole
      }
    }
    catch (Isis::iException &e) {
      string message = "Invalid label group [Mapping]";
      throw Isis::iException::Message(Isis::iException::Projection,message,_FILEINFO_);
    }
  }

  //! Destroys the PolarStereographic object
  PolarStereographic::~PolarStereographic() {
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
  bool PolarStereographic::SetGround(const double lat,const double lon) {
    // Fix up longitude
    p_longitude = lon;
    double lonRadians = lon * Isis::PI / 180.0;
    if (p_longitudeDirection == PositiveWest) lonRadians *= -1.0;

    // Now do latitude ... it must be planetographic
    p_latitude = lat;
    double latRadians = lat;
    if (IsPlanetocentric()) latRadians = ToPlanetographic(latRadians);
    latRadians = latRadians * Isis::PI / 180.0;

    // Compute easting and northing
    double lamda = p_signFactor * (lonRadians - p_centerLongitude);
    double phi = p_signFactor * latRadians;
    double sinphi = sin(phi);
    double t = tCompute (phi,sinphi);

    double dist;
    if (p_poleFlag) {
      dist = p_equatorialRadius * p_m * t / p_t;
    }
    else {
      dist = p_equatorialRadius * 2.0 * t / p_e4;
    }

    double x = p_signFactor * dist * sin(lamda);
    double y = -(p_signFactor * dist * cos(lamda));
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
  bool PolarStereographic::SetCoordinate(const double x, const double y) {
    // Save the coordinate
    SetXY(x,y);

    double east = p_signFactor * GetX();
    double north = p_signFactor * GetY();
    double dist = sqrt (east * east + north * north);

    double t;
    if(p_poleFlag) {
      t = dist * p_t / (p_m * p_equatorialRadius);
    }
    else {
      t = dist * p_e4 / (2.0 * p_equatorialRadius);
    }

    // Compute the latitude
    double phi = phi2Compute(t);
    p_latitude = p_signFactor * phi;

    if (fabs(p_latitude) > Isis::HALFPI) {
      string msg = "X,Y causes latitude to be outside [-90,90] in PolarStereographic Class";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }

    // Compute the longitude
    if (dist == 0.0) {
      p_longitude = p_signFactor * p_centerLongitude;
    }
    else {
      p_longitude = p_signFactor * atan2(east,-north) + p_centerLongitude;
    }

    // Cleanup the longitude
    p_longitude *= 180.0 / Isis::PI;
    if (p_longitudeDirection == PositiveWest) p_longitude *= -1.0;
    p_longitude = To360Domain (p_longitude);
    if (p_longitudeDomain == 180) p_longitude = To180Domain(p_longitude);

    // Cleanup the latitude
    p_latitude *= 180.0 / Isis::PI;
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
  bool PolarStereographic::XYRange(double &minX, double &maxX,
                               double &minY, double&maxY) {
    // Check the corners of the lat/lon range
    XYRangeCheck (p_minimumLatitude,p_minimumLongitude);
    XYRangeCheck (p_maximumLatitude,p_minimumLongitude);
    XYRangeCheck (p_minimumLatitude,p_maximumLongitude);
    XYRangeCheck (p_maximumLatitude,p_maximumLongitude);

    // Find the closest longitude >= to the minimum longitude that is offset from
    // the center longitude by a multiple of 90.
    double lon1 = p_centerLongitude * 180.0 / Isis::PI;
    if (p_longitudeDirection == PositiveWest) lon1 *= -1.0;
    while (lon1 > p_minimumLongitude) lon1 -= 90.0;
    while (lon1 < p_minimumLongitude) lon1 += 90.0;

    while (lon1 <= p_maximumLongitude) {
      XYRangeCheck(p_minimumLatitude,lon1);
      XYRangeCheck(p_maximumLatitude,lon1);
      lon1 += 90.0;
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
  PvlGroup PolarStereographic::Mapping() {
    PvlGroup mapping = Projection::Mapping();

    mapping += p_mappingGrp["CenterLatitude"];
    mapping += p_mappingGrp["CenterLongitude"];

    return mapping;
  }

  /**
   * This function returns the latitude keywords that this projection uses
   * 
   * @return PvlGroup The latitude keywords that this projection uses
   */
  PvlGroup PolarStereographic::MappingLatitudes() {
    PvlGroup mapping = Projection::MappingLatitudes();

    mapping += p_mappingGrp["CenterLatitude"];

    return mapping;
  }

  /**
   * This function returns the longitude keywords that this projection uses
   * 
   * @return PvlGroup The longitude keywords that this projection uses
   */
  PvlGroup PolarStereographic::MappingLongitudes() {
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
  bool PolarStereographic::operator== (const Isis::Projection &proj) {
    if (!Isis::Projection::operator==(proj)) return false;
  // dont do the below it is a recusive plunge
  //  if (Isis::Projection::operator!=(proj)) return false;
    PolarStereographic *pola = (PolarStereographic *) &proj;
    if (pola->p_centerLongitude != this->p_centerLongitude) return false;
    if (pola->p_centerLatitude != this->p_centerLatitude) return false;
    return true;
  }
}

extern "C" Isis::Projection *PolarStereographicPlugin (Isis::Pvl &lab,
                                                     bool allowDefaults) {
 return new Isis::PolarStereographic(lab,allowDefaults);
}
