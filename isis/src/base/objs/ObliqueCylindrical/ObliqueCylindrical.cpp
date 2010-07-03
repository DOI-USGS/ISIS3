/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2010/02/08 19:02:07 $
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
#include "ObliqueCylindrical.h"
#include "iException.h"
#include "Constants.h"
#include "naif/SpiceUsr.h"

using namespace std;
namespace Isis {     
  /** 
   * Constructs an ObliqueCylindrical object.
   * 
   * @param label This argument must be a Label containing the proper mapping 
   *              information as indicated in the Projection class.  
   *              Additionally, the Oblique cylindrical projection
   *              requires the Pole latitude, longitude and
   *              rotation.
   * 
   * @param allowDefaults This does nothing currently
   * 
   * @throws Isis::iException::Io
   */
  ObliqueCylindrical::ObliqueCylindrical(Isis::Pvl &label, bool allowDefaults) :
    Isis::Projection::Projection (label) {
    try {
      // Try to read the mapping group
      Isis::PvlGroup &mapGroup = label.FindGroup ("Mapping", Isis::Pvl::Traverse);

      p_poleLatitude = mapGroup["PoleLatitude"];

      // All latitudes must be planetographic
      if (this->IsPlanetocentric()) {
        p_poleLatitude = this->ToPlanetographic(p_poleLatitude);
      }

      if(p_poleLatitude < -90 || p_poleLatitude > 90) {
          throw iException::Message(iException::Pvl,
                                   "Pole latitude must be between -90 and 90.",
                                   _FILEINFO_);
      }

      p_poleLongitude = mapGroup["PoleLongitude"];

      if(p_poleLongitude < -360 || p_poleLongitude > 360) {
          throw iException::Message(iException::Pvl,
                                   "Pole longitude must be between -360 and 360.",
                                   _FILEINFO_);
      }

      p_poleRotation = mapGroup["PoleRotation"];

      if(p_poleRotation < -360 || p_poleRotation > 360) {
          throw iException::Message(iException::Pvl,
                                   "Pole rotation must be between -360 and 360.",
                                   _FILEINFO_);
      }

      bool calculateVectors = false;

      // Check vectors for the right array size
      if(!mapGroup.HasKeyword("XAxisVector") || mapGroup["XAxisVector"].Size() != 3) {
          calculateVectors = true;
      }

      if(!mapGroup.HasKeyword("YAxisVector") || mapGroup["YAxisVector"].Size() != 3) {
          calculateVectors = true;
      }

      if(!mapGroup.HasKeyword("ZAxisVector") || mapGroup["ZAxisVector"].Size() != 3) {
          calculateVectors = true;
      }

      if(!calculateVectors) {
        // Read in vectors
        p_xAxisVector.push_back(mapGroup["XAxisVector"][0]);
        p_xAxisVector.push_back(mapGroup["XAxisVector"][1]);
        p_xAxisVector.push_back(mapGroup["XAxisVector"][2]);
  
        p_yAxisVector.push_back(mapGroup["YAxisVector"][0]);
        p_yAxisVector.push_back(mapGroup["YAxisVector"][1]);
        p_yAxisVector.push_back(mapGroup["YAxisVector"][2]);
  
        p_zAxisVector.push_back(mapGroup["ZAxisVector"][0]);
        p_zAxisVector.push_back(mapGroup["ZAxisVector"][1]);
        p_zAxisVector.push_back(mapGroup["ZAxisVector"][2]);
      } else {
        // Calculate the vectors and store them in the labels
        // The vectors are useful for processing later on, but are
        // not actually used here
        double rotationAngle = p_poleRotation * (Isis::PI / 180.0);
        double latitudeAngle = (90.0 - p_poleLatitude) * (Isis::PI / 180.0);
        double longitudeAngle = (360.0 - p_poleLongitude) * (Isis::PI / 180.0);
        double pvec[3][3];

        eul2m_c(rotationAngle,latitudeAngle,longitudeAngle,3,2,3,pvec);

        // Reset the vector keywords
        if(mapGroup.HasKeyword("XAxisVector")) {
          mapGroup.DeleteKeyword("XAxisVector");
        }
        if(mapGroup.HasKeyword("YAxisVector")) {
          mapGroup.DeleteKeyword("YAxisVector");
        }
        if(mapGroup.HasKeyword("ZAxisVector")) {
          mapGroup.DeleteKeyword("ZAxisVector");
        }

        mapGroup += Isis::PvlKeyword("XAxisVector"); 
        mapGroup += Isis::PvlKeyword("YAxisVector");
        mapGroup += Isis::PvlKeyword("ZAxisVector");

        // Store calculations both in vector and PVL
        for(int i = 0; i < 3; i++) {
          p_xAxisVector.push_back(pvec[0][i]); //X[i]
          p_yAxisVector.push_back(pvec[1][i]); //Y[i]
          p_zAxisVector.push_back(pvec[2][i]); //Z[i]

          mapGroup["XAxisVector"] += pvec[0][i]; //X[i]
          mapGroup["YAxisVector"] += pvec[1][i]; //Y[i]
          mapGroup["ZAxisVector"] += pvec[2][i]; //Z[i]
        }
      }

      init();
    }
    catch (Isis::iException &e) {
      string message = "Invalid label group [Mapping]";
      throw Isis::iException::Message(Isis::iException::Io,message,_FILEINFO_);
    }
  }
  
  //! Destroys the ObliqueCylindrical object
  ObliqueCylindrical::~ObliqueCylindrical() {
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
  bool ObliqueCylindrical::SetGround(const double lat,const double lon) {
    double normalLat, normalLon;    // normal lat/lon
    double obliqueLat, obliqueLon;    // oblique lat/lon copy

    // Store lat,lon
    p_latitude = lat;
    p_longitude = lon;

    // Use oblat,oblon as radians version of lat,lon now for calculations
    normalLat = p_latitude * Isis::PI / 180.0;
    normalLon = p_longitude * Isis::PI / 180.0;
    if (p_longitudeDirection == PositiveWest) normalLon *= -1.0;


    /*******************************************************************************
    * Calculate the oblique lat/lon from the normal lat/lon
    *******************************************************************************/
    double poleLatitude = (p_poleLatitude * Isis::PI / 180.0);
    double poleLongitude = (p_poleLongitude * Isis::PI / 180.0);
    double poleRotation = (p_poleRotation * Isis::PI / 180.0);

    obliqueLat = asin (sin(poleLatitude) * sin(normalLat) +
                  cos(poleLatitude) * cos(normalLat) * cos(normalLon - poleLongitude));
  
    obliqueLon = atan2 (cos(normalLat) * sin(normalLon - poleLongitude), 
                   sin(poleLatitude) * cos(normalLat) * cos (normalLon - poleLongitude) - 
                       cos(poleLatitude) * sin(normalLat)) - poleRotation;

    while (obliqueLon < - Isis::PI) {
      obliqueLon += (2.0 * Isis::PI);
    }
  
    while (obliqueLon >= Isis::PI) {
      obliqueLon -= (2.0 * Isis::PI);
    }

    // Compute the coordinate
    double x = p_equatorialRadius * obliqueLon; 
    double y = p_equatorialRadius * obliqueLat;
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
  bool ObliqueCylindrical::SetCoordinate(const double x, const double y) {
    // Save the coordinate
    SetXY(x,y);

    /*******************************************************************************
    * Calculate the oblique latitude and check to see if it is outside or equal 
    * to [-90,90]. If it is, return with an error.
    *******************************************************************************/
    p_latitude = GetY() / p_equatorialRadius;

    if (abs(abs(p_latitude) - Isis::HALFPI) < DBL_EPSILON) {
      p_good = false;
      return p_good;
    }

    /*******************************************************************************
    * The check to see if p_equatorialRadius == 0 is done in the init function
    *******************************************************************************/
    p_longitude = GetX() / p_equatorialRadius;

    /*******************************************************************************
    * Convert the oblique lat/lon to normal lat/lon
    *******************************************************************************/
    double obliqueLat, obliqueLon;
    double poleLatitude = (p_poleLatitude * Isis::PI / 180.0);
    double poleLongitude = (p_poleLongitude * Isis::PI / 180.0);
    double poleRotation = (p_poleRotation * Isis::PI / 180.0);

    obliqueLat = asin (sin(poleLatitude) * sin(p_latitude) - 
                cos(poleLatitude) * cos(p_latitude) * cos(p_longitude + poleRotation));

    obliqueLon = atan2(cos(p_latitude) * sin(p_longitude + poleRotation),
                        sin(poleLatitude) * cos(p_latitude) * cos(p_longitude + poleRotation) + 
                          cos(poleLatitude) * sin(p_latitude)) + poleLongitude;

    /*******************************************************************************
    * Convert the latitude/longitude to degrees and apply target longitude direction
    * correction to the longitude
    *******************************************************************************/
    p_latitude = obliqueLat * 180.0 / Isis::PI;
    p_longitude = obliqueLon * 180.0 / Isis::PI;
  
    // Cleanup the longitude
    if (p_longitudeDirection == PositiveWest) p_longitude *= -1.0;

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
  * This function works for most cases, especially on smaller areas. However,
  * larger areas are likely to fail due to numerous discontinuities and a lack
  * of a mathematical algorithm to solve the range. This method works by searching
  * the boundaries, using DoSearch, and then searching lines tangent to discontinuities.
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
  bool ObliqueCylindrical::XYRange(double &minX, double &maxX, 
                                      double &minY, double&maxY) {
    // For oblique cylindrical, we'll have to walk all 4 sides to find out min/max x/y values.
    if(!HasGroundRange()) return false; // Don't have min/max lat/longs, can't continue

    p_specialLatCases.clear();
    p_specialLonCases.clear();

    // First, search longitude for min X/Y
    double minFoundX1, minFoundX2;
    double minFoundY1, minFoundY2;

    doSearch(MinimumLatitude(), MaximumLatitude(), minFoundX1, MinimumLongitude(), true, true, true);
    doSearch(MinimumLatitude(), MaximumLatitude(), minFoundX2, MaximumLongitude(), true, true, true);
    doSearch(MinimumLatitude(), MaximumLatitude(), minFoundY1, MinimumLongitude(), false, true, true);
    doSearch(MinimumLatitude(), MaximumLatitude(), minFoundY2, MaximumLongitude(), false, true, true);

    // Second, search latitude for min X/Y
    double minFoundX3, minFoundX4;
    double minFoundY3, minFoundY4;

    doSearch(MinimumLongitude(), MaximumLongitude(), minFoundX3, MinimumLatitude(), true, false, true);
    doSearch(MinimumLongitude(), MaximumLongitude(), minFoundX4, MaximumLatitude(), true, false, true);
    doSearch(MinimumLongitude(), MaximumLongitude(), minFoundY3, MinimumLatitude(), false, false, true);
    doSearch(MinimumLongitude(), MaximumLongitude(), minFoundY4, MaximumLatitude(), false, false, true);

    // We've searched all possible minimums, go ahead and store the lowest
    double minFoundX5 = min(minFoundX1, minFoundX2);
    double minFoundX6 = min(minFoundX3, minFoundX4);
    p_minimumX = min(minFoundX5, minFoundX6);

    double minFoundY5 = min(minFoundY1, minFoundY2);
    double minFoundY6 = min(minFoundY3, minFoundY4);
    p_minimumY = min(minFoundY5, minFoundY6);

    // Search longitude for max X/Y
    double maxFoundX1, maxFoundX2;
    double maxFoundY1, maxFoundY2;

    doSearch(MinimumLatitude(), MaximumLatitude(), maxFoundX1, MinimumLongitude(), true, true, false);
    doSearch(MinimumLatitude(), MaximumLatitude(), maxFoundX2, MaximumLongitude(), true, true, false);
    doSearch(MinimumLatitude(), MaximumLatitude(), maxFoundY1, MinimumLongitude(), false, true, false);
    doSearch(MinimumLatitude(), MaximumLatitude(), maxFoundY2, MaximumLongitude(), false, true, false);

    // Search latitude for max X/Y
    double maxFoundX3, maxFoundX4;
    double maxFoundY3, maxFoundY4;

    doSearch(MinimumLongitude(), MaximumLongitude(), maxFoundX3, MinimumLatitude(), true, false, false);
    doSearch(MinimumLongitude(), MaximumLongitude(), maxFoundX4, MaximumLatitude(), true, false, false);
    doSearch(MinimumLongitude(), MaximumLongitude(), maxFoundY3, MinimumLatitude(), false, false, false);
    doSearch(MinimumLongitude(), MaximumLongitude(), maxFoundY4, MaximumLatitude(), false, false, false);

    // We've searched all possible maximums, go ahead and store the highest
    double maxFoundX5 = max(maxFoundX1, maxFoundX2);
    double maxFoundX6 = max(maxFoundX3, maxFoundX4);
    p_maximumX = max(maxFoundX5, maxFoundX6);

    double maxFoundY5 = max(maxFoundY1, maxFoundY2);
    double maxFoundY6 = max(maxFoundY3, maxFoundY4);
    p_maximumY = max(maxFoundY5, maxFoundY6);

    // Look along discontinuities for more extremes
    std::vector<double> specialLatCases = p_specialLatCases;
    for(unsigned int specialLatCase = 0; specialLatCase < specialLatCases.size(); specialLatCase ++) {
      double minX, maxX, minY, maxY;

      doSearch(MinimumLongitude(), MaximumLongitude(), minX, specialLatCases[specialLatCase], true,  false, true);
      doSearch(MinimumLongitude(), MaximumLongitude(), minY, specialLatCases[specialLatCase], false, false, true);
      doSearch(MinimumLongitude(), MaximumLongitude(), maxX, specialLatCases[specialLatCase], true,  false, false);
      doSearch(MinimumLongitude(), MaximumLongitude(), maxY, specialLatCases[specialLatCase], false, false, false);

      p_minimumX = min(minX, p_minimumX);
      p_maximumX = max(maxX, p_maximumX);
      p_minimumY = min(minY, p_minimumY);
      p_maximumY = max(maxY, p_maximumY);
    }

    std::vector<double> specialLonCases = p_specialLonCases;
    for(unsigned int specialLonCase = 0; specialLonCase < specialLonCases.size(); specialLonCase ++) {
      double minX, maxX, minY, maxY;

      doSearch(MinimumLatitude(), MaximumLatitude(), minX, specialLonCases[specialLonCase], true,  true, true);
      doSearch(MinimumLatitude(), MaximumLatitude(), minY, specialLonCases[specialLonCase], false, true, true);
      doSearch(MinimumLatitude(), MaximumLatitude(), maxX, specialLonCases[specialLonCase], true,  true, false);
      doSearch(MinimumLatitude(), MaximumLatitude(), maxY, specialLonCases[specialLonCase], false, true, false);

      p_minimumX = min(minX, p_minimumX);
      p_maximumX = max(maxX, p_maximumX);
      p_minimumY = min(minY, p_minimumY);
      p_maximumY = max(maxY, p_maximumY);
    }

    p_specialLatCases.clear();
    p_specialLonCases.clear();

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
  PvlGroup ObliqueCylindrical::Mapping() {
    PvlGroup mapping = Projection::Mapping();

    mapping += p_mappingGrp["PoleLatitude"];
    mapping += p_mappingGrp["PoleLongitude"];
    mapping += p_mappingGrp["PoleRotation"];

    return mapping;
  }

  /**
   * This function returns the latitude keywords that this projection uses
   * 
   * @return PvlGroup The latitude keywords that this projection uses
   */
  PvlGroup ObliqueCylindrical::MappingLatitudes() {
    PvlGroup mapping = Projection::MappingLatitudes();

    return mapping;
  }

  /**
   * This function returns the longitude keywords that this projection uses
   * 
   * @return PvlGroup The longitude keywords that this projection uses
   */
  PvlGroup ObliqueCylindrical::MappingLongitudes() {
    PvlGroup mapping = Projection::MappingLongitudes();

    return mapping;
  }

  /**
   * Searches for extreme (min/max/discontinuity) values across lat/lons. Discontinuities are 
   * stored in p_specialLatCases and p_specialLonCases so they may be checked again later, which
   * creates significantly more accuracy in some cases. This method utilizes findExtreme to locate
   * extreme values.
   * 
   * @param minBorder Minimum lat or lon to start searching from
   * @param maxBorder Maximum lat or lon to start searching from
   * @param extremeVal The resulting global min/max on this line
   * @param constBorder The lat or lon that stays constant
   * @param searchLongitude True if you're searching along a longitude (constBorder is a lon, other borders lats).
   *                        False if you're searching a latitude (constBorder is a lat, other borders lons).
   * @param findMin True if looking for a minimum, false if looking for a maximum.
   */
  void ObliqueCylindrical::doSearch(double minBorder, double maxBorder, double &extremeVal,
                                    const double constBorder, bool searchX, bool searchLongitude, bool findMin) {
    const double TOLERANCE = 0.5;
    const int NUM_ATTEMPTS = (unsigned int)DBL_DIG; // It's unsafe to go past this precision

    double minFoundVal, maxFoundVal;
    int attempts = 0;

    do {
      findExtreme(minBorder, maxBorder, minFoundVal, maxFoundVal, constBorder, searchX, searchLongitude, findMin);
      attempts ++;
    } while((abs(minFoundVal - maxFoundVal) > TOLERANCE) && (attempts < NUM_ATTEMPTS));

    if(attempts >= NUM_ATTEMPTS) {
      // We zoomed in on a discontinuity because our range never shrank, this will need to be rechecked later.
      // *min and max border should be nearly identicle, so it doesnt matter which is used here
      if(searchLongitude) {
        p_specialLatCases.push_back(minBorder);
      }
      else {
        p_specialLonCases.push_back(minBorder);
      }
    }
    
    // These values will always be accurate, even over a discontinuity
    if(findMin) {
      extremeVal = min(maxFoundVal, minFoundVal);
    }
    else {
      extremeVal = max(maxFoundVal, minFoundVal);
    }
  }

 /**
  * Compares two Projection objects to see if they are equal
  * 
  * @param proj Projection object to do comparison on
  * 
  * @return bool Returns true if the Projection objects are equal, and false if 
  *              they are not
  */
  bool ObliqueCylindrical::operator== (const Isis::Projection &proj) {
    if (!Isis::Projection::operator==(proj)) return false;

    ObliqueCylindrical *obProjection = (ObliqueCylindrical *) &proj;    

    if(obProjection->GetPoleLatitude()  != GetPoleLatitude())  return false;
    if(obProjection->GetPoleLongitude() != GetPoleLongitude()) return false;
    if(obProjection->GetPoleRotation()  != GetPoleRotation())  return false;

    return true;
  }

  void ObliqueCylindrical::init() {
    /*******************************************************************************
    * Apply target correction for longitude direction
    *******************************************************************************/
    if (p_longitudeDirection == PositiveWest) p_longitude *= -1.0;
    if (p_longitudeDirection == PositiveWest) p_poleLongitude *= -1.0;
  
    /*******************************************************************************
    * Check that p_equatorialRadius isn't zero because we'll divide by it later
    *******************************************************************************/
    if(abs(p_equatorialRadius) <= DBL_EPSILON) {
            throw iException::Message(iException::Pvl,
                                     "The input center latitude is too close to a pole which will result in a division by zero.",
                                     _FILEINFO_);
    }
  }

  /**
   * Searches for extreme (min/max/discontinuity) values across lat/lons. Discontinuities will cause
   * the minVal and maxVal to never converge. This works by stepping (10 times) between the minBorder
   * and maxBorder to find an extreme. Then, the range of this extreme is recorder in minBorder and
   * maxBorder and the values on these new borders are stored in minVal or maxVal (could be X or Y depending
   * on searchX). This function should be used by calling it repeatidly until minVal ~= maxVal. If minVal never
   * comes close to maxVal, then between minBorder and maxBorder is the value of the most extreme value and either
   * minVal or maxVal will be correct (whichever is smaller or bigger depending on findMin).
   * 
   * @param minBorder Minimum lat or lon to start searching from, which gets updated to a more precise range
   * @param maxBorder Maximum lat or lon to start searching from, which gets updated to a more precise range
   * @param minVal The lower resultant, which is more accurate when nearly equal to maxVal
   * @param maxVal The higher resultant, which is more accurate when nearly equal to minVal
   * @param constBorder The lat or lon that stays constant
   * @param searchX True if searching X Coordinates, False if searching Y Coordinates
   * @param searchLongitude True if you're searching along a longitude (constBorder is a lon, other borders lats).
   *                        False if you're searching a latitude (constBorder is a lat, other borders lons).
   * @param findMin True if looking for a minimum, false if looking for a maximum.
   */
  void ObliqueCylindrical::findExtreme(double &minBorder, double &maxBorder,
                                    double &minVal, double &maxVal, const double constBorder,
                                    bool searchX, bool searchLongitude, bool findMin) {
    // Always do 10 steps
    const double STEP_SIZE = (maxBorder - minBorder) / 10.0;
    const double LOOP_END = maxBorder + (STEP_SIZE / 2.0); // This ensures we do all of the steps properly
    double currBorderVal = minBorder;

    SetSearchGround(minBorder, constBorder, searchLongitude);
    double value1 = (searchX)? XCoord() : YCoord();
    double value2 = value1;
    double value3 = value1;

    double border1 = minBorder;
    double border2 = minBorder;
    double border3 = minBorder;

    double extremeVal1 = value1;
    double extremeVal2 = value1;
    double extremeVal3 = value1;

    double extremeBorder1 = minBorder;
    double extremeBorder2 = minBorder;
    double extremeBorder3 = minBorder;

    while(currBorderVal <= LOOP_END) {
      SetSearchGround(currBorderVal, constBorder, searchLongitude);

      value3 = value2;
      value2 = value1;
      value1 = (searchX)? XCoord() : YCoord();
      border3 = border2;
      border2 = border1;
      border1 = currBorderVal;
  
      if((findMin && value2 < extremeVal2) || (!findMin && value2 > extremeVal2)) {
        extremeVal1 = value1;
        extremeVal2 = value2;
        extremeVal3 = value3;

        extremeBorder3 = border3;
        extremeBorder2 = border2;
        extremeBorder1 = border1;
      }

      currBorderVal += STEP_SIZE;
    }

    minBorder = extremeBorder3; // Border 3 is lagging and thus smaller
    maxBorder = extremeBorder1; // Border 1 is leading and thus larger

    SetSearchGround(minBorder, constBorder, searchLongitude);
    minVal = (searchX)? XCoord() : YCoord();
    SetSearchGround(maxBorder, constBorder, searchLongitude);
    maxVal = (searchX)? XCoord() : YCoord();
  }

  /**
   * This function sets the ground based on two variables and a boolean. This is meant
   * for use by DoSearch and findExtreme in order to set the ground correctly each time.
   * 
   * @param variableBorder The lat/lon that is variable in the search methods
   * @param constBorder The lat/lon that is constant in the search methods
   * @param variableIsLat True if variableBorder is a lat, False if variableBorder is a lon
   */
  void ObliqueCylindrical::SetSearchGround(const double variableBorder, const double constBorder, bool variableIsLat) {
    if(variableIsLat) {
      SetGround(variableBorder, constBorder);
    }
    else {
      SetGround(constBorder, variableBorder);
    }
  }
} // end namespace isis

extern "C" Isis::Projection *ObliqueCylindricalPlugin (Isis::Pvl &lab,
                                                    bool allowDefaults) {
  return new Isis::ObliqueCylindrical(lab,allowDefaults);
}
