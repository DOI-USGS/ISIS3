/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include "VimsGroundMap.h"

#include <iostream>
#include <iomanip>

#include <QVector>

#include "Camera.h"
#include "Constants.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"
#include "iTime.h"
#include "Latitude.h"
#include "LeastSquares.h"
#include "Longitude.h"
#include "PolynomialBivariate.h"
#include "Preference.h"
#include "SpecialPixel.h"

using namespace std;

namespace Isis {
  /**
   * Constructs the VimsGroundMap object
   *  
   * @param parent A pointer to the parent Camera object
   * @param lab Pvl labels for the image 
   */ 
  VimsGroundMap::VimsGroundMap(Camera *parent, Pvl &lab) :
    CameraGroundMap(parent) {
    // Init(lab);
    p_minLat = NULL;
    p_maxLat = NULL;
    p_minLon = NULL;
    p_maxLon = NULL;
    p_latMap = NULL;
    p_lonMap = NULL;

    p_minLat = new Latitude();
    p_maxLat = new Latitude();
    p_minLon = new Longitude();
    p_maxLon = new Longitude();

    p_latMap = new QVector< QVector<Latitude> >(64, QVector<Latitude>(64));
    p_lonMap = new QVector< QVector<Longitude> >(64, QVector<Longitude>(64));

    if (parent->ParentSamples() > 64 || parent->ParentLines() > 64) {
      iString msg = "The Vims ground map does not understand cubes that "
                    "initially have more than 64 lines or 64 samples.";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }

  /**
   * Destroys the VimsGroundMap object
   */
  VimsGroundMap::~VimsGroundMap() {
    if(p_minLat) {
      delete p_minLat;
      p_minLat = NULL;
    }

    if(p_maxLat) {
      delete p_maxLat;
      p_minLat = NULL;
    }

    if(p_minLon) {
      delete p_minLon;
      p_minLon = NULL;
    }

    if(p_maxLon) {
      delete p_maxLon;
      p_maxLon = NULL;
    }

    if(p_latMap) {
      delete p_latMap;
      p_latMap = NULL;
    }

    if(p_lonMap) {
      delete p_lonMap;
      p_lonMap = NULL;
    }
  }

  /**
   * Initialize vims camera model
   *
   * @param [in] lab   (Pvl &)      Cube Pvl label 
   *  
   * @throw iException::Io - "Cannot process NYQUIST(undersampled) mode " 
   *  
   * @internal 
   *   @history  2007-04-16 Tracie Sucharski - Look for unit vectors in
   *                              the proper directory.
   *   @history  2007-04-18 Tracie Sucharski, The inaccuracy of the 15 Mhz clock
   *                    (exposure , interline_delay) is already taken care of
   *                     in the labels values, so remove the adjustment from the
   *                     code (exp * 1.01725).
   *                     *Reference:  email from John Ivens 11/27/2006.
   *   @history 2007-04-19 Tracie Sucharski,  Swap bytes on unit vectors. They
   *                     are currently LSB binary files.
   *                     ???Todo:???  Convert unit vectors to text files.
   *   @history 2008-02-05 Tracie Sucharski, Replaced unitVector files with
   *                           Rick McCloskey's code to calculate look
   *                           direction.
   *   @history 2009-08-06 Tracie Sucharski, Bug in unit vector change made
   *                           on 2008-02-05, had the incorrect boresight for
   *                           VIS Hires.
   */
  void VimsGroundMap::Init(Pvl &lab) {

    PvlGroup inst = lab.FindGroup("Instrument", Pvl::Traverse);

    //  Vis or IR
    p_channel = (string) inst ["Channel"];
    // Get the start time in et
    iString stime = (string) inst ["NativeStartTime"];
    string intTime = stime.Token(".");

    p_etStart = p_camera->getClockTime(intTime).Et();
    p_etStart += stime.ToDouble() / 15959.0;
    //----------------------------------------------------------------------
    //  Because of inaccuracy with the 15 Mhz clock, the IR exposure and
    //  interline delay need to be adjusted.
    //----------------------------------------------------------------------
    p_irExp = (double) inst ["ExposureDuration"] / 1000.;
    p_visExp = (double) inst ["ExposureDuration"][1] / 1000.;
    p_interlineDelay =
      (double) inst ["InterlineDelayDuration"] / 1000.;

    // Get summation mode
    string sampMode = iString((string)inst ["SamplingMode"]).UpCase();

    //  Get sample/line offsets
    int sampOffset = inst ["XOffset"];
    int lineOffset = inst ["ZOffset"];

    //  Get swath width/length which will be image size unless occultation image
    p_swathWidth = inst ["SwathWidth"];
    p_swathLength = inst ["SwathLength"];

    if(p_channel == "VIS") {
      if(sampMode == "NORMAL") {
        p_xPixSize = p_yPixSize = 0.00051;
        p_xBore = p_yBore = 31;
        p_camSampOffset = sampOffset - 1;
        p_camLineOffset = lineOffset - 1;

      }
      else {
        p_xPixSize = p_yPixSize = 0.00051 / 3.0;
        p_xBore = p_yBore = 94;
        //New as of 2009-08-04 per Dyer Lytle's email
        // Old Values:p_camSampOffset = 3 * (sampOffset - 1) + p_swathWidth;
        //            p_camLineOffset = 3 * (lineOffset - 1) + p_swathLength;
        p_camSampOffset = (3 * (sampOffset + p_swathWidth / 2)) - p_swathWidth / 2;
        p_camLineOffset = (3 * (lineOffset + p_swathLength / 2)) - p_swathLength / 2;
      }
    }
    else if(p_channel == "IR") {
      if(sampMode == "NORMAL") {
        p_xPixSize = p_yPixSize = 0.000495;
        p_xBore = p_yBore = 31;
        p_camSampOffset = sampOffset - 1;
        p_camLineOffset = lineOffset - 1;
      }
      if(sampMode == "HI-RES") {
        p_xPixSize = 0.000495 / 2.0;
        p_yPixSize = 0.000495;
        p_xBore = 62.5;
        p_yBore = 31;
        p_camSampOffset = 2 * ((sampOffset - 1) + ((p_swathWidth - 1) / 4));
        p_camLineOffset = lineOffset - 1;
      }
      if(sampMode == "NYQUIST") {
        string msg = "Cannot process NYQUIST(undersampled) mode ";
        throw iException::Message(iException::Io, msg, _FILEINFO_);
      }
    }

    //---------------------------------------------------------------------
    //  Loop for each pixel in cube, get pointing information and calculate
    //  control point (line,sample,lat,lon) for later use in latlon_to_linesamp.
    //---------------------------------------------------------------------
    p_camera->IgnoreProjection(true);
    for(int line = 0; line < p_camera->ParentLines(); line++) {

      //  VIS exposure is for a single line.  According to SIS,
      //  NATIVE_START_TIME is for the first pixel of the IR exposure.
      //  "The offset from IR start to VIS start is calculated by
      //   IrExposMsec - VisExposMsec)/2".
      //  This needs to be moved to forward routine

      if(p_channel == "VIS") {
        double et = ((double)p_etStart + (((p_irExp * p_swathWidth) - p_visExp) / 2.)) +
                    ((line + 0.5) * p_visExp);
        p_camera->SetTime(et);
      }

      for(int samp = 0; samp < p_camera->ParentSamples(); samp++) {
        if(p_channel == "IR") {
          double et = (double)p_etStart +
                      (line * p_camera->ParentSamples() * p_irExp) +
                      (line * p_interlineDelay) + ((samp + 0.5) * p_irExp);
          p_camera->SetTime(et);
        }

        if(p_camera->SetImage((double) samp + 1, (double)line + 1)) {
          const Latitude &latitude = p_camera->GetLatitude();
          const Longitude &longitude = p_camera->GetLongitude();
          // could do
          // double lat = this->UniversalLatitude ()
          if(!p_minLat->Valid() || latitude < *p_minLat)
            *p_minLat = latitude;
          if(!p_maxLat->Valid() || latitude > *p_maxLat)
            *p_maxLat = latitude;
          if(!p_minLon->Valid() || longitude < *p_minLon)
            *p_minLon = longitude;
          if(!p_maxLon->Valid() || longitude > *p_maxLon)
            *p_maxLon = longitude;
          (*p_latMap)[line][samp] = latitude;
          (*p_lonMap)[line][samp] = longitude;
        }
      }
    }
    p_camera->IgnoreProjection(false);
  }


  /** Compute ground position from focal plane coordinate
   *
   * This method will compute the ground position given an
   * undistorted focal plane coordinate.  Note that the latitude/longitude
   * value can be obtained from the camera class passed into the constructor.
   *
   * @param ux Distorted focal plane x in millimeters
   * @param uy Distorted focal plane y in millimeters
   * @param uz Distorted focal plane z in millimeters
   *
   * @return @b bool Indicates whether the conversion was successful
   *
   * @internal 
   * @history 2008-01-02 Tracie Sucharski - Check incoming pixel for validity
   *                                against edge of pixel not center.
   * @history 2008-02-05 Tracie Sucharski, Replaced unitVector files with
   *                         Rick McCloskey's code to calculate look
   *                         direction
   * @history 2008-02-14 Tracie Sucharski, Change imgSamp/imgLine to double
   *                         so that fractional pixels are used in calculations.
   */
  bool VimsGroundMap::SetFocalPlane(const double ux, const double uy,
                                    const double uz) {

    p_ux = ux;
    p_uy = uy;
    p_uz = uz;

    double imgSamp = ux;
    double imgLine = uy;

    if((imgLine < 0.5) || (imgLine > p_camera->ParentLines() + 0.5) ||
        (imgSamp < 0.5) || (imgSamp > p_camera->ParentSamples() + 0.5)) {
      return false;
    }
    imgLine--;
    imgSamp--;

    // does interline_delay & exposure-duration account for summing modes?
    // if not, won't use p_parentLine/p_parentSample
    double et = 0.;
    if(p_channel == "VIS") {
      et = (p_etStart + ((p_irExp * p_swathWidth) - p_visExp) / 2.) +
           ((imgLine + 0.5) * p_visExp);
    }
    else if(p_channel == "IR") {
      et = (double)p_etStart +
           (imgLine * p_camera->ParentSamples() * p_irExp) +
           (imgLine * p_interlineDelay) + ((imgSamp + 0.5) * p_irExp);
    }
    p_camera->SetTime(et);

    //  get Look Direction
    SpiceDouble lookC[3];
    LookDirection(lookC);

    SpiceDouble unitLookC[3];
    vhat_c(lookC, unitLookC);
    return p_camera->SetLookDirection(unitLookC);
  }

  /** Compute undistorted focal plane coordinate from ground position
   *
   * @param lat Planetocentric latitude in degrees
   * @param lon Planetocentric longitude in degrees
   *
   * @return @b bool Indicates whether the conversion was successful
   *
   * @internal 
   * @history 2007-04-18  Tracie Sucharski - Added check for reasonable
   *                             match when attempting to find closest
   *                             lat/lon in map arrays.
   * @history 2007-09-14  Tracie Sucharski - Added check for longitude
   *                             outside min/max bounds.  Don't know why
   *                             this wasn't put in before (lat check was
   *                             in), was it oversight, or did I take it out
   *                             for some reason???
   * @history 2007-12-14  Tracie Sucharski - Remove resolution test, too
   *                            image dependent and the resolution for vims is
   *                            incorrect due to the instrument having
   *                            rectangular pixels.
   * @history 2008-01-02  Tracie Sucharski -  Check validity of resulting
   *                            sample and line against edge of starting
   *                            ending pixels (0.5/Parent+0.5) instead of
   *                            center of pixels.
   *
   */
  bool VimsGroundMap::SetGround(const Latitude &lat, const Longitude &lon) {

    if(lat < *p_minLat || lat > *p_maxLat) return false;
    if(lon < *p_minLon || lon > *p_maxLon) return false;

    //  Find closest points  ??? what tolerance ???
    double minDist = 9999.;
    int minSamp = -1;
    int minLine = -1;

    for(int line = 0; line < p_camera->ParentLines(); line++) {

      for(int samp = 0; samp < p_camera->ParentSamples(); samp++) {
        const Latitude &mapLat = (*p_latMap)[line][samp];
        if(!mapLat.Valid()) continue;
        Longitude mapLon = (*p_lonMap)[line][samp];
        if(!mapLon.Valid()) continue;

        //  If on boundary convert lons.  If trying to find 360, convert
        //  lons on other side of meridian to values greater than 360.  If
        //  trying to find 1.0, convert lons on other side to negative numbers.
        WrapWorldToBeClose(lon, mapLon);

        Angle deltaLat = lat - mapLat;
        Angle deltaLon = lon - mapLon;
        double dist = (deltaLat.GetRadians() * deltaLat.GetRadians()) +
                      (deltaLon.GetRadians() * deltaLon.GetRadians());
        if(dist < minDist) {
          minDist = dist;
          minSamp = samp;
          minLine = line;
        }
      }
    }

    //-----------------------------------------------------------------
    //  If dist is less than some ??? tolerance ??? this is the
    //  closest point.  Use this point and surrounding 8 pts as
    //  control pts.
    //----------------------------------------------------------------
    if(minDist >= 9999.) return false;

    //-----------------------------------------------------------------
    //  Test for reasonable point using resolution.
    //-----------------------------------------------------------------
#if 0
      p_camera->IgnoreProjection(true);
      p_camera->SetImage(minSamp + 1, minLine + 1);
      p_camera->IgnoreProjection(false);

      double radii[3];
      p_camera->Radii(radii);
      double degPerSamp =
        1.0 / ((rpd_c() * radii[0] * 1000.) / p_camera->SampleResolution());
      double degPerLine =
        1.0 / ((rpd_c() * radii[0] * 1000.) / p_camera->LineResolution());

      // Convert lon if needed
      double mapLon = p_lonMap[minLine][minSamp];
      if(abs(mapLon - lon) > 180) {
        if((lon - mapLon) > 0) {
          mapLon = 360. + mapLon;
        }
        else if((lon - mapLon) < 0) {
          mapLon = mapLon - 360.;
        }
      }
      if(abs(p_latMap[minLine][minSamp] - lat) > (degPerLine * 5.) ||
          abs(mapLon - lon) > (degPerSamp * 5.))
        return false;
#endif
    //-------------------------------------------------------------
    //  Set-up for LU decomposition (least2 fit).
    //  Assume we will have 9 control points, this may not be true
    //  and will need to be adjusted before the final solution.
    //-------------------------------------------------------------
    PolynomialBivariate sampBasis(1);
    PolynomialBivariate lineBasis(1);
    LeastSquares sampLsq(sampBasis);
    LeastSquares lineLsq(lineBasis);
    vector<double> known(2);

    for(int line = minLine - 1; line < minLine + 2; line++) {
      if(line < 0 || line > p_camera->ParentLines() - 1) continue;
      for(int samp = minSamp - 1; samp < minSamp + 2; samp++) {
        //  Check for edges
        if(samp < 0 || samp > p_camera->ParentSamples() - 1) continue;

        Latitude mapLat = (*p_latMap)[line][samp];
        Longitude mapLon = (*p_lonMap)[line][samp];
        if((!mapLat.Valid()) || (!mapLon.Valid())) continue;

        //  If on boundary convert lons.  If trying to find 360, convert
        //  lons on other side of meridian to values greater than 360.  If
        //  trying to find 1.0, convert lons on other side to negative numbers.
        WrapWorldToBeClose(lon, mapLon);

        known[0] = mapLat.GetDegrees();
        known[1] = mapLon.GetDegrees();
        sampLsq.AddKnown(known, samp + 1);
        lineLsq.AddKnown(known, line + 1);
      }
    }
    if(sampLsq.Knowns() < 3) return false;

    sampLsq.Solve();
    lineLsq.Solve();

    //  Solve for new sample position
    known[0] = lat.GetDegrees();
    known[1] = lon.GetDegrees();
    double inSamp = sampLsq.Evaluate(known);
    double inLine = lineLsq.Evaluate(known);

// OLD CODE:  Why < 0 and not < 1???
//      if (inSamp < 0 || inSamp > p_camera->ParentSamples() + 0.5 ||
//          inLine < 0 || inLine > p_camera->ParentLines() + 0.5) {
    if(inSamp < 0.5 || inSamp > p_camera->ParentSamples() + 0.5 ||
        inLine < 0.5 || inLine > p_camera->ParentLines() + 0.5) {
      return false;
    }

    p_camera->IgnoreProjection(true);
    p_camera->SetImage(inSamp, inLine);
    p_camera->IgnoreProjection(false);
    if(!p_camera->HasSurfaceIntersection()) return false;

    p_focalPlaneX = inSamp;
    p_focalPlaneY = inLine;

    return true;
  }


  /**
   * If on boundary convert longitude values.  If trying to find 360, convert
   * longitude values on other side of meridian to values greater than 360.  If
   * trying to find 1.0, convert longitude values on other side to negative 
   * numbers.
   *
   * This modifies lon2 and leaves lon1 alone. 
   * @param lon1 Longitude value 1
   * @param lon2 Longitude value to be modified.
   */
  void VimsGroundMap::WrapWorldToBeClose(const Longitude &lon1,
                                         Longitude &lon2) {
    if(abs((lon1 - lon2).GetDegrees()) > 180) {
      if(lon1 > lon2) {
        lon2 += Angle(360, Angle::Degrees);
      }
      else {
        lon2 -= Angle(360, Angle::Degrees);
      }
    }
  }


  /** 
   *  Compute undistorted focal plane coordinate from ground position.
   *
   * @param surfacePoint Ground surface point
   *
   * @return @b bool Indicates whether the conversion was successful.
   *
   * @internal 
   */
  bool VimsGroundMap::SetGround(const SurfacePoint &surfacePoint) {
    return SetGround(surfacePoint.GetLatitude(), surfacePoint.GetLongitude());
  }


  /**
   * Determines the look direction in the camera coordinate system
   *
   * @param  [out]  v   Look direction vector in camera coordinates
   *
   * This method will compute the look direction vector in the camera coordinate
   * system.  This code was converted from Rick McCloskey's point_tbl c
   * code.
   *
   * @internal 
   *   @history 2008-01-031  Tracie Sucharski - Converted Rick's code rather than
   *                                 using the unitVector files from Rick.
   */
  void VimsGroundMap::LookDirection(double v[3]) {

    double x = p_ux - 1. + p_camSampOffset;
    double y = p_uy - 1. + p_camLineOffset;

    //  Compute pointing angles based on pixel size separation
    double theta = Isis::HALFPI - (y - p_yBore) * p_yPixSize;
    double phi = (-1. * Isis::HALFPI) + (x - p_xBore) * p_xPixSize;

    v[0] = sin(theta) * cos(phi);
    v[1] = cos(theta);
    v[2] = sin(theta) * sin(phi) / -1.;
  }
}
