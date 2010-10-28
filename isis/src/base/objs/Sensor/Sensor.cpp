/**
 * @file
 * $Revision: 1.17 $
 * $Date: 2010/05/22 00:08:59 $
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

#include "Sensor.h"
#include "CubeManager.h"
#include "iString.h"
#include "iException.h"
#include "iException.h"
#include "Constants.h"
#include "SpecialPixel.h"
#include "NaifStatus.h"
#include <iomanip>

#define MAX(x,y) (((x) > (y)) ? (x) : (y))

using namespace std;
namespace Isis {

  /**
   * Constructs a Sensor object and loads SPICE kernels using information from
   * the label object. The constructor expects an Instrument and Kernels group
   * to be in the labels (see Spice documentation).
   *
   * @param lab Label containing Instrument and Kernels groups.
   */
  Sensor::Sensor(Isis::Pvl &lab) : Isis::Spice(lab) {
    // Assume no shape model
    p_hasElevationModel = false;
    p_demProj = NULL;
    p_demCube = NULL;
    p_interp = NULL;
    p_portal = NULL;
    string demCube = "";

    // Do we have one
    Isis::PvlGroup &kernels = lab.FindGroup("Kernels", Isis::Pvl::Traverse);
    if(kernels.HasKeyword("ElevationModel") &&
        !kernels["ElevationModel"].IsNull() &&
        !IsSky()) {
      demCube = (string) kernels["ElevationModel"];
    }
    else if(kernels.HasKeyword("ShapeModel") &&
            !kernels["ShapeModel"].IsNull() &&
            !IsSky()) {
      demCube = (string) kernels["ShapeModel"];
    }

    if(demCube != "") {
      p_hasElevationModel = true;
      p_demCube = CubeManager::Open(demCube);
      p_demProj = Isis::ProjectionFactory::CreateFromCube(*(p_demCube->Label()));

      p_interp = new Isis::Interpolator(Interpolator::BiLinearType);
      p_portal = new Isis::Portal(p_interp->Samples(), p_interp->Lines(),
                                  p_demCube->PixelType(),
                                  p_interp->HotSample(), p_interp->HotLine());

      // Read in the min/max radius of the DEM file and the Scale of the DEM
      // file in pixels/degree
      Isis::Pvl demlab = *(p_demCube->Label());
      if (p_demProj->IsEquatorialCylindrical()) {
        if (!p_demCube->HasTable("ShapeModelStatistics")) {
          std::string msg = "The input cube references a ShapeModel that has not been ";
          msg += "updated for the new ray tracing algorithm. All DEM files must now be ";
          msg += "padded at the poles and contain a ShapeModelStatistics table defining ";
          msg += "their minimum and maximum radii values. The demprep program should be ";
          msg += "used to prepare the DEM before you can run this program. There is more ";
          msg += "information available in the documentation of the demprep program.";
          throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
        }
        Table table("ShapeModelStatistics");
        p_demCube->Read(table);
        p_minRadius = table[0]["MinimumRadius"];
        p_maxRadius = table[0]["MaximumRadius"];
        Isis::PvlGroup &mapgrp = demlab.FindGroup("Mapping",Isis::Pvl::Traverse);
        p_demScale = (double) mapgrp["Scale"];
      }
    }

    // No intersection with the target yet
    p_hasIntersection = false;
  }

  //! Destroys the Sensor
  Sensor::~Sensor() {
    if(p_demProj) {
      delete p_demProj;
      p_demProj = NULL;
    }

    // we do not have ownership of p_demCube
    p_demCube = NULL;

    if(p_interp) {
      delete p_interp;
      p_interp = NULL;
    }

    if(p_portal) {
      delete p_portal;
      p_portal = NULL;
    }
  }

  /**
   * This allows you to ignore the elevation model
   *
   * @param ignore True for no elevation model
   */
  void Sensor::IgnoreElevationModel(bool ignore) {
    // if we have an elevation model and are not ignoring it,
    //   set p_hasElevationModel to true
    if(p_demProj && !ignore) {
      p_hasElevationModel = true;
    }
    else {
      p_hasElevationModel = false;
    }
  }

  /**
  * By setting the ephemeris time you essential set the position of the
  * spacecraft and body as indicated in the class Spice. However, after this
  * is invoked there will be no intersection point until SetLookDirection or
  * SetUniversalGround is invoked.
  *
  * @param time Ephemeris time (read NAIF documentation for a detailed
  *             description)
  */
  void Sensor::SetEphemerisTime(const double time) {
    Isis::Spice::SetEphemerisTime(time);
    p_hasIntersection = false;
  }

  /**
   * Sets the look direction of the spacecraft. This routine will then attempt to
   * intersect the look direction with the target. If successful you can utilize
   * the methods which return the lat/lon, phase, incidence, etc. This routine
   * returns false if the look direction does not intersect the target.
   *
   * @param v[] A look vector in camera coordinates. For example, (0,0,1) is
   *            usually the look direction out of the boresight of a camera.
   *
   * history  2009-09-23  Tracie Sucharski - Convert negative longitudes
   *                         returned my reclat.
   * history  2010-09-15  Janet Barrett - Modified this method to use a new
   *                         algorithm for finding the intersection of a ray with
   *                         the DEM. This was required to take care of problems that
   *                         were encountered at the poles of global DEM files. The
   *                         algorithm that is being used was taken from "Intersection
   *                         between spacecraft viewing vectors and digital elevation
   *                         models" by N.A. Teanby. This algorithm only works on 
   *                         Equatorial Cylindrical projections. Other projections still
   *                         use the previous algorithm.
   * history  2010-10-26  Janet Barrett - The tolerance value of 1E-5 was too small and
   *                         was causing a divide by zero error when the current and last
   *                         intersections were virtually the same. The tolerance was
   *                         changed to 3E-8 * a (where a is the equatorial radius of the
   *                         planet we are dealing with).
   * @return bool
   */
  bool Sensor::SetLookDirection(const double v[3]) {
    // The look vector must be in the camera coordinate system
    vector<double> lookC(3);
    lookC[0] = v[0];
    lookC[1] = v[1];
    lookC[2] = v[2];

    // Convert it to body-fixed
    vector<double> lookJ = InstrumentRotation()->J2000Vector(lookC);
    vector<double> lookB = BodyRotation()->ReferenceVector(lookJ);
    p_lookB[0] = lookB[0];
    p_lookB[1] = lookB[1];
    p_lookB[2] = lookB[2];
    p_newLookB = true;

    // Don't try to intersect the sky
    if(IsSky()) {
      p_hasIntersection = false;
      return p_hasIntersection;
    }

    // Prep for surfpt by obtaining the radii
    SpiceDouble a, b, c;
    a = p_radii[0];
    b = p_radii[1];
    c = p_radii[2];

    // See if it intersects the planet
    SpiceBoolean found;
    std::vector<double> sB = BodyRotation()->ReferenceVector(InstrumentPosition()->Coordinate());
    surfpt_c((SpiceDouble *) &sB[0], p_lookB, a, b, c, p_pB, &found);
    if(!found) {
      p_hasIntersection = false;
      return p_hasIntersection;
    }

    // If we have a dem kernel then we should iterate until
    // the point converges
    double plen=0.0;
    SpiceDouble plat, plon, pradius;
    if(p_hasElevationModel) {

      double cmin = cos((90.0 - 1.0 / (2.0*p_demScale)) * Isis::PI/180.0);

      // Separate iteration algorithms are used for different projections -
      // use this iteration for equatorial cylindrical type projections 
      if (p_demProj->IsEquatorialCylindrical()) {
        // Set hasIntersection flag to true so Resolution can be calculated
        p_hasIntersection = true;
        int maxit = 10;
        int it = 0;
        bool done = false;

        // Normalize the look vector
        SpiceDouble ulookB[3];
        ulookB[0] = lookB[0];
        ulookB[1] = lookB[1];
        ulookB[2] = lookB[2];
        vhat_c(ulookB,ulookB);

        // Calculate the limb viewing angle to see if the line of sight is
        // pointing away from the planet
        SpiceDouble observer[3];
        observer[0] = sB[0];
        observer[1] = sB[1];
        observer[2] = sB[2];
        SpiceDouble negobserver[3];
        vminus_c(observer,negobserver);
        double psi0 = vsep_c(negobserver,ulookB);
        double cospsi0 = cos(psi0);

        // If psi0 is greater than 90 degrees, then reject data as looking
        // away from the planet and no proper tangent point exists in the
        // direction that the spacecraft is looking
        if (psi0 > Isis::PI/2.0) {
          p_hasIntersection = false;
          return p_hasIntersection;
        }

        // Calculate the vector to the tangent point
        SpiceDouble tvec[3];
        double observerdist = vnorm_c(observer);
        tvec[0] = observer[0] + observerdist*cospsi0*ulookB[0];
        tvec[1] = observer[1] + observerdist*cospsi0*ulookB[1];
        tvec[2] = observer[2] + observerdist*cospsi0*ulookB[2];
        double tlen = vnorm_c(tvec);

        // Calculate distance along look vector to first and last test point
        double d0 = observerdist*cospsi0 - sqrt(p_maxRadius*p_maxRadius - tlen*tlen);
        double dm = observerdist*cospsi0 + sqrt(p_maxRadius*p_maxRadius - tlen*tlen);

        // Set the properties at the first test observation point
        double d = d0;
        SpiceDouble g1[3];
        g1[0] = observer[0] + d0*ulookB[0];
        g1[1] = observer[1] + d0*ulookB[1];
        g1[2] = observer[2] + d0*ulookB[2];
        double g1len = vnorm_c(g1);
        SpiceDouble g1lat, g1lon, g1radius;
        reclat_c(g1,&g1radius,&g1lon,&g1lat);
        g1lat *= 180.0 / Isis::PI;
        g1lon *= 180.0 / Isis::PI;
        if (g1lon < 0.0) g1lon += 360.0;
        SpiceDouble negg1[3];
        vminus_c(g1,negg1);
        double psi1 = vsep_c(negg1,ulookB);

        // Set dalpha to be half the grid spacing for nyquist sampling
        //double dalpha = (Isis::PI/180.0)/(2.0*p_demScale);
        double dalpha = (Isis::PI/180.0) * MAX(cos(g1lat*(Isis::PI/180.0)),cmin) / (2.0*p_demScale);
        double r1 = DemRadius(g1lat,g1lon);
        if (Isis::IsSpecial(r1)) {
          p_hasIntersection = false;
          return p_hasIntersection;
        }

        // Set default to limb observation until we determine that it is a nadir observation
        int ptype = 0;

        // Set the tolerance to a fraction of the equatorial radius, a
        double tolerance = 3E-8 * a;

        // Main iteration loop
        // Loop from g1 to gm stepping by angles of dalpha until intersection is found
        while(!done) {
          if (d > dm) {
            p_hasIntersection = false;
            return p_hasIntersection;
          }
          it = 0;

          // Calculate the angle between the look vector and the planet radius at the current
          // test point
          double psi2 = psi1 + dalpha;

          // Calculate the step size
          double dd = g1len * sin(dalpha) / sin(Isis::PI-psi2);

          // Calculate the vector to the current test point from the planet center
          d = d + dd;
          SpiceDouble g2[3];
          g2[0] = observer[0] + d * ulookB[0];
          g2[1] = observer[1] + d * ulookB[1];
          g2[2] = observer[2] + d * ulookB[2];
          double g2len = vnorm_c(g2);

          // Determine lat,lon,radius at this point
          SpiceDouble g2lat, g2lon, g2radius;
          reclat_c(g2,&g2radius,&g2lon,&g2lat);
          g2lat *= 180.0 / Isis::PI;
          g2lon *= 180.0 / Isis::PI;
          if (g2lon < 0.0) g2lon += 360.0;
          double r2 = DemRadius(g2lat,g2lon);
          if (Isis::IsSpecial(r2)) {
            p_hasIntersection = false;
            return p_hasIntersection;
          }

          // Test for intersection
          if (r2 > g2len) {
            // An intersection has occurred. Interpolate between g1 and g2 to get the
            // lat,lon of the intersect point.

            // If g1 and g2 straddle a hill, then we may need to iterate a few times
            // until we are on the linear segment.

            // First, set flag for the nadir observation
            ptype = 1;
            it = it + 1;

            while (it < maxit && !done) {
              // Calculate the fractional distance "v" to move along the look vector
              // to the intersection point. Check to see if there was a convergence
              // of the solution and the tolerance was too small to detect it.
              if ((g2len*r1/r2 - g1len) == 0.0) {
                std::string msg = "The tolerance is too small to detect the convergence of ";
                msg += "a solution. The tolerance must be re-evaluated to fix this problem. ";
                msg += "Please provide the program you are running along with the inputs to ";
                msg += "ISIS support so that this problem can be addressed.";
                throw Isis::iException::Message(Isis::iException::Programmer, msg, _FILEINFO_);
              }
              double v = (r1-g1len) / (g2len*r1/r2 - g1len);
              p_pB[0] = g1[0] + v * dd * ulookB[0];
              p_pB[1] = g1[1] + v * dd * ulookB[1];
              p_pB[2] = g1[2] + v * dd * ulookB[2];
              plen = vnorm_c(p_pB);
              reclat_c(p_pB,&pradius,&plon,&plat);
              plat *= 180.0 / Isis::PI;
              plon *= 180.0 / Isis::PI;
              if (plon < 0.0) plon += 360.0;
              if (plon > 360.0) plon -= 360.0;
              pradius = DemRadius(plat,plon);
              if (Isis::IsSpecial(pradius)) {
                p_hasIntersection = false;
                return p_hasIntersection;
              } 
              double palt = plen - pradius;

              // The altitude relative to surface is +ve at the observation point,
              // so reset g1=p and r1=pradius
              if (palt > tolerance) {
                it = it + 1;
                g1[0] = p_pB[0];
                g1[1] = p_pB[1];
                g1[2] = p_pB[2];
                g1len = plen;
                r1 = pradius;
                dd = dd * (1.0 - v);

              // The altitude relative to surface -ve at the observation point,
              // so reset g2=p and r2=pradius
              } else if (palt < -tolerance) {
                it = it + 1;
                g2[0] = p_pB[0];
                g2[1] = p_pB[1];
                g2[2] = p_pB[2];
                g2len = plen;
                r2 = pradius;
                dd = dd * v;

              // We are within the tolerance, so the solution has converged
              } else {
                p_hasIntersection = true;
                plen = pradius;
                palt = 0.0;
                done = true;
              }
            }
            g1[0] = g2[0];
            g1[1] = g2[1];
            g1[2] = g2[2];
            g1len = g2len; 
            r1 = r2;
            psi1 = psi2;
            dalpha = (Isis::PI/180.0) * MAX(cos(g2lat*(Isis::PI/180.0)),cmin) / (2.0*p_demScale);
          }
        }

        p_latitude = plat;
        p_longitude = plon;
        p_radius = plen;

        surfpt_c((SpiceDouble *)&sB[0], p_lookB, p_radius, p_radius, p_radius,
                  p_pB, &found);
        if(!found) {
          p_hasIntersection = false;
          return p_hasIntersection;
        }
      } else {
        // Set hasIntersection flag to true so Resolution can be calculated
        p_hasIntersection = true;
        int maxit = 100;
        int it = 1;
        bool done = false;
        SpiceDouble pB[3];
        while(!done) {
          if(it > maxit) {
            p_hasIntersection = false;
            return p_hasIntersection;
          }
          // Set the tolerance for 1/100 of a pixel in meters
          double tolerance = Resolution() / 100.0;
          double lat, lon, radius;
          reclat_c(p_pB, &radius, &lon, &lat);
          lat *= 180.0 / Isis::PI;
          lon *= 180.0 / Isis::PI;
          if(lon < 0.0) lon += 360.0;

          if(it == 1) {
            p_radius = DemRadius(lat, lon);
          }
          else {
            double demRadius = DemRadius(lat, lon);

            if(!Isis::IsSpecial(demRadius)) {
              p_radius = (p_radius + demRadius) / 2.0;
            }
          }

          if(Isis::IsSpecial(p_radius)) {
            p_hasIntersection = false;
            return p_hasIntersection;
          }

          pB[0] = p_pB[0];
          pB[1] = p_pB[1];
          pB[2] = p_pB[2];
          surfpt_c((SpiceDouble *)&sB[0], p_lookB, p_radius, p_radius, p_radius,
                   p_pB, &found);
          if(!found) {
            p_hasIntersection = false;
            return p_hasIntersection;
          }

          double dist = sqrt((pB[0] - p_pB[0]) * (pB[0] - p_pB[0]) +
                             (pB[1] - p_pB[1]) * (pB[1] - p_pB[1]) +
                             (pB[2] - p_pB[2]) * (pB[2] - p_pB[2])) * 1000.;
          if(dist < tolerance) {
            // Now recompute tolerance at updated surface point and recheck
            double tolerance = Resolution() / 100.0;
            if(dist < tolerance) done = true;
          }

          it++;
        }
      }
    }

    // Convert x/y/z to lat/lon and radius
    p_hasIntersection = true;
    reclat_c(p_pB, &p_radius, &p_longitude, &p_latitude);
    p_latitude *= 180.0 / Isis::PI;
    p_longitude *= 180.0 / Isis::PI;
    while(p_longitude < 0.0) p_longitude += 360.0;
    while(p_longitude > 360.0) p_longitude -= 360.0;
    return p_hasIntersection;
  }

  /**
   * Returns the x,y,z of the surface intersection in BodyFixed km.
   *
   * @param p[] The coordinate of the surface intersection
   */
  void Sensor::Coordinate(double p[3]) const {
    p[0] = p_pB[0];
    p[1] = p_pB[1];
    p[2] = p_pB[2];
  }

  /**
   * Returns the phase angle in degrees. This does not use the surface model.
   *
   * @return double
   */
  double Sensor::PhaseAngle() const {
    SpiceDouble psB[3], upsB[3], dist;
    std::vector<double> sB = BodyRotation()->ReferenceVector(InstrumentPosition()->Coordinate());
    vsub_c((SpiceDouble *) &sB[0], p_pB, psB);
    unorm_c(psB, upsB, &dist);

    SpiceDouble puB[3], upuB[3];
    vsub_c(p_uB, p_pB, puB);
    unorm_c(puB, upuB, &dist);

    double angle = vdot_c(upsB, upuB);
    if(angle > 1.0) return 0.0;
    if(angle < -1.0) return 180.0;
    return acos(angle) * 180.0 / Isis::PI;
  }

  /**
   * Returns the emission angle in degrees. This does not use the surface model.
   *
   * @return double
   */
  double Sensor::EmissionAngle() const {
    SpiceDouble psB[3], upsB[3], upB[3], dist;
    std::vector<double> sB = BodyRotation()->ReferenceVector(InstrumentPosition()->Coordinate());
    vsub_c((SpiceDouble *) &sB[0], p_pB, psB);
    unorm_c(psB, upsB, &dist);
    unorm_c(p_pB, upB, &dist);

    double angle = vdot_c(upB, upsB);
    if(angle > 1.0) return 0.0;
    if(angle < -1.0) return 180.0;
    return acos(angle) * 180.0 / Isis::PI;
  }

  /**
   * Returns the incidence angle in degrees. This does not use the surface model.
   *
   * @return double
   */
  double Sensor::IncidenceAngle() const {
    SpiceDouble puB[3], upuB[3], upB[3], dist;
    vsub_c(p_uB, p_pB, puB);
    unorm_c(puB, upuB, &dist);
    unorm_c(p_pB, upB, &dist);

    double angle = vdot_c(upB, upuB);
    if(angle > 1.0) return 0.0;
    if(angle < -1.0) return 180.0;
    return acos(angle) * 180.0 / Isis::PI;
  }

  /**
   * This is the opposite routine for SetLookDirection. Instead of computing a
   * point on the target, a point is set and the look direction is computed.
   * Other methods such as lat/lon, phase, incidence, etc. can be used if this
   * method returns a true.
   *
   * @param latitude Planetocentric latitude
   *
   * @param longitude Positive east longitude
   *
   * @param backCheck If true this method will check the lat/lon point to see if
   *                  it falls on the backside of the target (or beyond the
   *                  horizon). If false this test will not occur.
   *                  Defaults to true
   *
   * @return bool
   */
  bool Sensor::SetUniversalGround(const double latitude,
                                  const double longitude, bool backCheck) {
    // Can't intersect the sky
    if(IsSky()) {
      p_hasIntersection = false;
      return p_hasIntersection;
    }

    // Load the latitude/longitude
    p_latitude = latitude;
    p_longitude = longitude;

    double lon = p_longitude * Isis::PI / 180.0;
    double lat = p_latitude * Isis::PI / 180.0;

    if(p_hasElevationModel) {
      p_radius = DemRadius(p_latitude, p_longitude);
      if(Isis::IsSpecial(p_radius)) {
        p_hasIntersection = false;
        return p_hasIntersection;
      }
    }
    else {
      // Otherwise compute the local radius on the ellipsoid
      double a = p_radii[0];
      double b = p_radii[1];
      double c = p_radii[2];
      double xyradius = a * b / sqrt(pow(b * cos(lon), 2) + pow(a * sin(lon), 2));
      p_radius = xyradius * c / sqrt(pow(c * cos(lat), 2) + pow(xyradius * sin(lat), 2));
    }

    return SetGroundLocal(backCheck);
  }

  /**
   * This overloaded method has the opposite function as SetLookDirection. Instead
   * of computing a point on the target, a point is set and the look direction is
   * computed.  Other methods such as lat/lon, phase, incidence, etc. can be used if
   * this method returns a true.
   *
   * @param latitude Planetocentric latitude in degrees
   *
   * @param longitude Positive east longitude in degrees
   *
   * @param radius Radius in meters
   *
   * @param backCheck If true this method will check the lat/lon point to see if
   *                  it falls on the backside of the target (or beyond the
   *                  horizon). If false this test will not occur.
   *                  Defaults to true
   *
   * @return bool
   */
  bool Sensor::SetUniversalGround(const double latitude,
                                  const double longitude,
                                  const double radius, bool backCheck) {
    // Can't intersect the sky
    if(IsSky()) {
      p_hasIntersection = false;
      return p_hasIntersection;
    }

    // Load the latitude/longitude/radius
    p_latitude = latitude;
    p_longitude = longitude;
    p_radius = radius / 1000.;

    return SetGroundLocal(backCheck);
  }

  /**
  * This method handles the common functions for the overloaded SetUniversalGround methods.
  * Instead of computing a point on the target, a point is set (lat,lon,radius) and the look
  * direction is computed.
  *
  * @param backCheck If true this method will check the lat/lon point to see if
  *                  it falls on the backside of the target (or beyond the
  *                  horizon). If false this test will not occur.
  *                  Defaults to true
  *
  * @return bool
  *
  */
  bool Sensor::SetGroundLocal(bool backCheck) {
    // With the 3 spherical value compute the x/y/z coordinate
    latrec_c(p_radius, (p_longitude * Isis::PI / 180.0), (p_latitude * Isis::PI / 180.0), p_pB);

    // Make sure the point isn't on the backside of the body
    std::vector<double> sB = BodyRotation()->ReferenceVector(InstrumentPosition()->Coordinate());
    p_lookB[0] = p_pB[0] - sB[0];
    p_lookB[1] = p_pB[1] - sB[1];
    p_lookB[2] = p_pB[2] - sB[2];
    p_newLookB = true;

    // Get emission angle for testing to see if surface point is viewable
    double emission = EmissionAngle();

    // See if the point is on the backside of the target
    if(backCheck) {
      if(fabs(emission) > 90.) {
        p_hasIntersection = false;
        return p_hasIntersection;
      }
    }

    // return with success
    p_hasIntersection = true;
    return p_hasIntersection;

  }



  /**
  * Returns the look direction in the camera coordinate system
  *
  * @param v[] The look vector
  */
  void Sensor::LookDirection(double v[3]) const {
    vector<double> lookB(3);
    lookB[0] = p_lookB[0];
    lookB[1] = p_lookB[1];
    lookB[2] = p_lookB[2];
    vector<double> lookJ = BodyRotation()->J2000Vector(lookB);
    vector<double> lookC = InstrumentRotation()->ReferenceVector(lookJ);
    v[0] = lookC[0];
    v[1] = lookC[1];
    v[2] = lookC[2];
  }

  /**
   * Returns the right ascension angle (sky longitude)
   */
  double Sensor::RightAscension() {
    if(p_newLookB) computeRaDec();
    return p_ra;
  }

  /**
   * Returns the declination angle (sky latitude)
   */
  double Sensor::Declination() {
    if(p_newLookB) computeRaDec();
    return p_dec;
  }

  /**
   * Protected method which computes the ra/dec of the current look direction
   */
  void Sensor::computeRaDec() {
    p_newLookB = false;
    vector<double> lookB(3);
    lookB[0] = p_lookB[0];
    lookB[1] = p_lookB[1];
    lookB[2] = p_lookB[2];
    vector<double> lookJ = BodyRotation()->J2000Vector(lookB);;

    SpiceDouble range;
    recrad_c((SpiceDouble *)&lookJ[0], &range, &p_ra, &p_dec);
    p_ra *= 180.0 / Isis::PI;
    p_dec *= 180.0 / Isis::PI;
  }

  /**
   * Given the ra/dec compute the look direction
   *
   * @param ra    right ascension in degrees (sky longitude)
   * @param dec   declination in degrees (sky latitude)
   *
   * @returns success or failure
   */
  bool Sensor::SetRightAscensionDeclination(const double ra, const double dec) {
    vector<double> lookJ(3);
    radrec_c(1.0, ra * Isis::PI / 180.0, dec * Isis::PI / 180.0, (SpiceDouble *)&lookJ[0]);

    vector<double> lookC = InstrumentRotation()->ReferenceVector(lookJ);
    return SetLookDirection((double *)&lookC[0]);
  }


  //! Return the distance between the spacecraft and surface point in km
  double Sensor::SlantDistance() const {
    SpiceDouble psB[3], upsB[3];
    SpiceDouble dist;

    std::vector<double> sB = BodyRotation()->ReferenceVector(InstrumentPosition()->Coordinate());
    vsub_c(p_pB, (SpiceDouble *) &sB[0], psB);
    unorm_c(psB, upsB, &dist);
    return dist;
  }

  //! Return the local solar time in hours
  double Sensor::LocalSolarTime() {
    double slat, slon;
    SubSolarPoint(slat, slon);

    double lst = UniversalLongitude() - slon + 180.0;
    lst = lst / 15.0;  // 15 degress per hour
    if(lst < 0.0) lst += 24.0;
    if(lst > 24.0) lst -= 24.0;
    return lst;
  }

  //! Returns the distance between the sun and surface point in AU
  double Sensor::SolarDistance() const {
    // Get the sun coord
    double sB[3];
    Isis::Spice::SunPosition(sB);

    // Calc the change
    double xChange = sB[0] - p_pB[0];
    double yChange = sB[1] - p_pB[1];
    double zChange = sB[2] - p_pB[2];

    // Calc the distance and convert to AU
    double dist = sqrt(pow(xChange, 2) + pow(yChange, 2) + pow(zChange, 2));
    dist /= 149597870.691;
    return dist;
  }

  /**
   * Returns the distance from the spacecraft to the subspacecraft point in km.
   * It uses the ellipsoid, not the shape model
   */
  double Sensor::SpacecraftAltitude() {
    // Get the spacecraft coord
    double spB[3];
    Isis::Spice::InstrumentPosition(spB);

    // Get subspacecraft point
    double lat, lon;
    SubSpacecraftPoint(lat, lon);
    double rlat = lat * Isis::PI / 180.0;
    double rlon = lon * Isis::PI / 180.0;

    // Compute radius
    double rad;
    if(p_hasElevationModel) {
      rad = DemRadius(lat, lon);
    }
    else {
      double a = p_radii[0];
      double b = p_radii[1];
      double c = p_radii[2];
      double xyradius = a * b / sqrt(pow(b * cos(rlon), 2) + pow(a * sin(rlon), 2));
      rad = xyradius * c / sqrt(pow(c * cos(rlat), 2) + pow(xyradius * sin(rlat), 2));
    }

    // Now with the 3 spherical value compute the x/y/z coordinate
    double ssB[3];
    latrec_c(rad, rlon, rlat, ssB);

    // Calc the change
    double xChange = spB[0] - ssB[0];
    double yChange = spB[1] - ssB[1];
    double zChange = spB[2] - ssB[2];

    // Calc the distance
    double dist = sqrt(pow(xChange, 2) + pow(yChange, 2) + pow(zChange, 2));
    return dist;
  }

  /** Grab the radius from the dem if we have one
   */
  double Sensor::DemRadius(double lat, double lon) {
    if(!p_hasElevationModel) return Isis::Null;

    p_demProj->SetUniversalGround(lat, lon);
    if(!p_demProj->IsGood()) {
      return Isis::Null;
    }

    p_portal->SetPosition(p_demProj->WorldX(), p_demProj->WorldY(), 1);
    p_demCube->Read(*p_portal);

    double radius = p_interp->Interpolate(p_demProj->WorldX(), p_demProj->WorldY(),
                                          p_portal->DoubleBuffer());
    if(Isis::IsSpecial(radius)) {
      return Isis::Null;
    }

    return radius / 1000.0;
  }

}
