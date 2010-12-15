/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2010/03/27 06:54:43 $
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
#include "RadarGroundMap.h"

namespace Isis {
  RadarGroundMap::RadarGroundMap(Camera *parent, Radar::LookDirection ldir,
                                 double waveLength) :
    CameraGroundMap(parent) {
    p_camera = parent;
    p_lookDirection = ldir;
    p_waveLength = waveLength;

    // Angular tolerance based on radii and
    // slant range (Focal length)
    double radii[3];
    p_camera->Radii(radii);
//    p_tolerance = p_camera->FocalLength() / radii[2];
//    p_tolerance *= 180.0 / Isis::PI;
    p_tolerance = .00000001;

    // Compute a default time tolerance to a 1/20 of a pixel
    double et1 = p_camera->Spice::CacheStartTime();
    double et2 = p_camera->Spice::CacheEndTime();
    p_timeTolerance = (et2 - et1) / p_camera->Lines() / 20.0;
  }

  /** Compute ground position from slant range
   *
   * @param ux Slant range distance in meters scaled to focal plane
   * @param uy Doppler shift (always 0.0)
   * @param uz Not used
   *
   * @return conversion was successful
   */
  bool RadarGroundMap::SetFocalPlane(const double ux, const double uy,
                                     double uz) {

    SpiceRotation *bodyFrame = p_camera->BodyRotation();
    SpicePosition *spaceCraft = p_camera->InstrumentPosition();

    // Get spacecraft position and velocity to create a state vector
    std::vector<double> Ssc(6);
    // Load the state into Ssc
    vequ_c((SpiceDouble *) & (spaceCraft->Coordinate()[0]), &Ssc[0]);
    vequ_c((SpiceDouble *) & (spaceCraft->Velocity()[0]), &Ssc[3]);

    // Rotate state vector to body-fixed
    std::vector<double> bfSsc(6);
    bfSsc = bodyFrame->ReferenceVector(Ssc);

    // Extract body-fixed position and velocity
    std::vector<double> Vsc(3);
    std::vector<double> Xsc(3);
    vequ_c(&bfSsc[0], (SpiceDouble *) & (Xsc[0]));
    vequ_c(&bfSsc[3], (SpiceDouble *) & (Vsc[0]));

    // Compute intrack, crosstrack, and radial coordinate
    SpiceDouble i[3];
    vhat_c(&Vsc[0], i);

    SpiceDouble c[3];
    SpiceDouble dp;
    dp = vdot_c(&Xsc[0], i);
    SpiceDouble p[3], q[3];
    vscl_c(dp, i, p);
    vsub_c(&Xsc[0], p, q);
    vhat_c(q, c);

    SpiceDouble r[3];
    vcrss_c(i, c, r);

    // What is the initial guess for R
    double radii[3];
    p_camera->Radii(radii); // km
    SpiceDouble R = radii[0];
    SpiceDouble lastR = DBL_MAX;
    SpiceDouble rlat;
    SpiceDouble rlon;

    SpiceDouble lat = DBL_MAX;
    SpiceDouble lon = DBL_MAX;

    double slantRangeSqr = (ux * p_rangeSigma) / 1000.;   // convert to meters, then km
    slantRangeSqr = slantRangeSqr * slantRangeSqr;
    SpiceDouble X[3];

    int iter = 0;
    do {
      double normXsc = vnorm_c(&Xsc[0]);
      double alpha = (R * R - slantRangeSqr - normXsc * normXsc) /
                     (2.0 * vdot_c(&Xsc[0], c));

      double arg = slantRangeSqr - alpha * alpha;
      if(arg < 0.0) return false;

      double beta = sqrt(arg);
      if(p_lookDirection == Radar::Left) beta *= -1.0;

      SpiceDouble alphac[3], betar[3];
      vscl_c(alpha, c, alphac);
      vscl_c(beta, r, betar);

      vadd_c(alphac, betar, alphac);
      vadd_c(&Xsc[0], alphac, X);

      // Convert X to lat,lon
      lastR = R;
      reclat_c(X, &R, &lon, &lat);

      rlat = lat * 180.0 / Isis::PI;
      rlon = lon * 180.0 / Isis::PI;
      R = GetRadius(rlat, rlon);  // km
      iter++;
    }
    while(fabs(R - lastR) > p_tolerance && iter < 30);

    if(fabs(R - lastR) > p_tolerance) return false;

    lat = lat * 180.0 / Isis::PI;
    lon = lon * 180.0 / Isis::PI;
    while(lon < 0.0) lon += 360.0;

    // Compute body fixed look direction
    std::vector<double> lookB;
    lookB.resize(3);
    lookB[0] = X[0] - Xsc[0];
    lookB[1] = X[1] - Xsc[1];
    lookB[2] = X[2] - Xsc[2];

    std::vector<double> lookJ = bodyFrame->J2000Vector(lookB);
    SpiceRotation *cameraFrame = p_camera->InstrumentRotation();
    std::vector<double> lookC = cameraFrame->ReferenceVector(lookJ);

    SpiceDouble unitLookC[3];
    vhat_c(&lookC[0], unitLookC);
    p_camera->SetLookDirection(unitLookC);

    return p_camera->Sensor::SetUniversalGround(lat, lon);
  }

  /** Compute undistorted focal plane coordinate from ground position
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   *
   * @return conversion was successful
   *
   * @internal
   *   @history 2010-12-10 Debbie A. Cook,  Corrected radius units
   *                        in SetGround call to be meters
   */
  bool RadarGroundMap::SetGround(const double lat, const double lon) {
    return SetGround(lat, lon, GetRadius(lat, lon)*1000.);
  }

  /** Compute undistorted focal plane coordinate from ground position that includes a local radius
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   * @param radius local radius in meters
   *
   * @return conversion was successful
   *
   * @internal
   *   @history 2010-12-10 Debbie A. Cook,  Corrected radius units
   *                        in latrec call to be km and set
   *                        set p_focalPlaneY to the scaled
   *                        doppler frequency
   */
  bool RadarGroundMap::SetGround(const double lat, const double lon, const double radius) {
    // Get the ground point in rectangular coordinates (X)
    SpiceDouble X[3];
    SpiceDouble rlat = lat * Isis::PI / 180.0;
    SpiceDouble rlon = lon * Isis::PI / 180.0;
    latrec_c(radius/1000., rlon, rlat, X);

    // Compute lower bound for Doppler shift
    double et1 = p_camera->Spice::CacheStartTime();
    p_camera->Sensor::SetEphemerisTime(et1);
    double xv1 = ComputeXv(X);

    // Compute upper bound for Doppler shift
    double et2 = p_camera->Spice::CacheEndTime();
    p_camera->Sensor::SetEphemerisTime(et2);
    double xv2 = ComputeXv(X);

    // Make sure we bound root (xv = 0.0)
    if((xv1 < 0.0) && (xv2 < 0.0)) return false;
    if((xv1 > 0.0) && (xv2 > 0.0)) return false;

    // Order the bounds
    double fl, fh, xl, xh;
    if(xv1 < xv2) {
      fl = xv1;
      fh = xv2;
      xl = et1;
      xh = et2;
    }
    else {
      fl = xv2;
      fh = xv1;
      xl = et2;
      xh = et1;
    }

    // Iterate a max of 30 times
    for(int j = 0; j < 30; j++) {
      // Use the secant method to guess the next et
      double etGuess = xl + (xh - xl) * fl / (fl - fh);

      // Compute the guessed Doppler shift.  Hopefully
      // this guess converges to zero at some point
      p_camera->Sensor::SetEphemerisTime(etGuess);
      double fGuess = ComputeXv(X);

      // Update the bounds
      double delTime;
      if(fGuess < 0.0) {
        delTime = xl - etGuess;
        xl = etGuess;
        fl = fGuess;
      }
      else {
        delTime = xh - etGuess;
        xh = etGuess;
        fh = fGuess;
      }

      // See if we are done
      if((fabs(delTime) <= p_timeTolerance) || (fGuess == 0.0)) {
        SpiceRotation *bodyFrame = p_camera->BodyRotation();
        SpicePosition *spaceCraft = p_camera->InstrumentPosition();

        // Get body fixed spacecraft velocity and position
        std::vector<double> Ssc(6);

        // Load the state into Ssc and rotate to body-fixed
        vequ_c((SpiceDouble *) & (spaceCraft->Coordinate()[0]), &Ssc[0]);
        vequ_c((SpiceDouble *) & (spaceCraft->Velocity()[0]), &Ssc[3]);
        std::vector<double> bfSsc(6);
        bfSsc = bodyFrame->ReferenceVector(Ssc);

        // Extract the body-fixed position and velocity from the state
        std::vector<double> Vsc(3);
        std::vector<double> Xsc(3);
        vequ_c(&bfSsc[0], (SpiceDouble *) & (Xsc[0]));
        vequ_c(&bfSsc[3], (SpiceDouble *) & (Vsc[0]));

        // Determine if focal plane coordinate falls on the correct side of the
        // spacecraft. Radar has both left and right look directions. Make sure
        // the coordinate is on the same side as the look direction. This is done
        // by (X - S) . (V x S) where X=ground point vector, S=spacecraft position
        // vector, and V=velocity vector. If the dot product is greater than 0, then
        // the point is on the right side. If the dot product is less than 0, then
        // the point is on the left side. If the dot product is 0, then the point is
        // directly under the spacecraft (neither left or right) and is invalid.
        SpiceDouble vout1[3];
        SpiceDouble vout2[3];
        SpiceDouble dp;
        vsub_c(X, &Xsc[0], vout1);
        vcrss_c(&Vsc[0], &Xsc[0], vout2);
        dp = vdot_c(vout1, vout2);
        if(dp > 0.0 && p_lookDirection == Radar::Left) return false;
        if(dp < 0.0 && p_lookDirection == Radar::Right) return false;
        if(dp == 0.0) return false;

        // Compute body fixed look direction
        std::vector<double> lookB;
        lookB.resize(3);
        lookB[0] = X[0] - Xsc[0];
        lookB[1] = X[1] - Xsc[1];
        lookB[2] = X[2] - Xsc[2];

        std::vector<double> lookJ = bodyFrame->J2000Vector(lookB);
        SpiceRotation *cameraFrame = p_camera->InstrumentRotation();
        std::vector<double> lookC = cameraFrame->ReferenceVector(lookJ);

        SpiceDouble unitLookC[3];
        vhat_c(&lookC[0], unitLookC);
        p_camera->SetLookDirection(unitLookC);

        p_camera->SetFocalLength(p_slantRange * 1000.0); // p_slantRange is km so focal length is in m
        p_focalPlaneX = p_slantRange * 1000.0 / p_rangeSigma; // km to meters and scaled to focal plane
        //        p_focalPlaneY = 0.0;
        p_focalPlaneY = p_dopplerFreq / p_dopplerSigma;   // htx to focal plane coord  OR should this be etGuess?
        return true;
      }
    }

    return false;
  }



  /** Compute undistorted focal plane coordinate from ground position using current Spice from SetImage call
   *
   * This method will compute the undistorted focal plane coordinate for
   * a ground position, using the current Spice settings (time and kernels)
   * without resetting the current point values for lat/lon/radius/x/y and
   * related radar parameter p_slantRange.
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   * @param radius local radius in m
   *
   * @return conversion was successful
   */
  bool RadarGroundMap::GetXY(SurfacePoint point, double *cudx, double *cudy) {

    // Get the ground point in rectangular body-fixed coordinates (X)
    double X[3];
    X[0] = point.GetX();
    X[1] = point.GetY();
    X[2] = point.GetX();

    // Compute body-fixed look vector
    SpiceRotation *bodyFrame = p_camera->BodyRotation();
    SpicePosition *spaceCraft = p_camera->InstrumentPosition();

    std::vector<double> sJ(6);   // Spacecraft state vector (position and velocity) in J2000 frame
    // Load the state into sJ
    vequ_c((SpiceDouble *) & (spaceCraft->Coordinate()[0]), &sJ[0]);
    vequ_c((SpiceDouble *) & (spaceCraft->Velocity()[0]), &sJ[3]);

    // Rotate the state to body-fixed
    p_sB.resize(6);
    p_sB = bodyFrame->ReferenceVector(sJ);

    // Extract the body-fixed position and velocity
    SpiceDouble VsB[3];
    SpiceDouble PsB[3];
    vequ_c(&p_sB[0], PsB);
    vequ_c(&p_sB[3], VsB);

    p_lookB.resize(3);
    vsub_c(X, PsB, &p_lookB[0]);

    p_groundSlantRange = vnorm_c(&p_lookB[0]);  // km
    p_groundDopplerFreq = 2. / p_waveLength / p_groundSlantRange * vdot_c(&p_lookB[0], &VsB[0]);
    *cudx = p_groundSlantRange * 1000.0 / p_rangeSigma;  // to meters, then to focal plane coord
    *cudy = p_groundDopplerFreq / p_dopplerSigma;   // htx to focal plane coord

    return true;

  }




  /** Compute the slant range and the doppler frequency for current body position
   *
   * @param X body-fixed coordinate of surface point
   *
   * @return doppler frequency
   *
   * @internal
   *   @history 2010-12-10 Debbie A. Cook,  Corrected radius units
   *                        in SetGround call to be meters and set
   *                        p_dopplerFreq
   */
  double RadarGroundMap::ComputeXv(SpiceDouble X[3]) {
    // Get the spacecraft position (Xsc) and velocity (Vsc) in body fixed
    // coordinates
    SpiceRotation *bodyFrame = p_camera->BodyRotation();
    SpicePosition *spaceCraft = p_camera->InstrumentPosition();

    // Load the state into Ssc
    std::vector<double> Ssc(6);
    vequ_c((SpiceDouble *) & (spaceCraft->Coordinate()[0]), &Ssc[0]);
    vequ_c((SpiceDouble *) & (spaceCraft->Velocity()[0]), &Ssc[3]);

    // Rotate the state to body-fixed
    std::vector<double> bfSsc(6);
    bfSsc = bodyFrame->ReferenceVector(Ssc);

    // Extract the body-fixed position and velocity
    std::vector<double> Vsc(3);
    std::vector<double> Xsc(3);
    vequ_c(&bfSsc[0], &Xsc[0]);
    vequ_c(&bfSsc[3], &Vsc[0]);

    // Compute the slant range
    SpiceDouble lookB[3];
    vsub_c(&Xsc[0], X, lookB);
    p_slantRange = vnorm_c(lookB);   // units are km

    // Compute and return xv = -2 * (point - observer) dot (point velocity - observer velocity) / (slantRange*wavelength)
    // In body-fixed coordinates, the point velocity = 0. so the equation becomes
    //    double xv = 2.0 * vdot_c(lookB,&Vsc[0]) / (vnorm_c(lookB) * WaveLength() );
    double xv = -2.0 * vdot_c(lookB, &Vsc[0]) / (vnorm_c(lookB) * WaveLength()); // - is applied to lookB above
    p_dopplerFreq = xv;
    return xv;
  }


  /** Compute the radius based on the shape model for the cube
   *
   * This method will compute the local radius at the specified lat/lon
   * using the shape model specified in the kernels group of the cube.
   *
   * @param lat planetocentric latitude in degrees
   * @param lon planetocentric longitude in degrees
   * @param radius local radius in km
   *
   * @return radius local radius in km
   */
  double RadarGroundMap::GetRadius(double lat, double lon) {
    if(p_camera->HasElevationModel()) {
      return p_camera->DemRadius(lat, lon);
    }

    double radii[3];
    p_camera->Radii(radii);
    double a = radii[0];
    double b = radii[1];
    double c = radii[2];
    double xyradius = a * b / sqrt(pow(b * cos(lon), 2) + pow(a * sin(lon), 2));
    return xyradius * c / sqrt(pow(c * cos(lat), 2) + pow(xyradius * sin(lat), 2));
  }




  /** Compute derivative w/r to position of focal plane coordinate from ground position using current Spice from SetImage call
   *
   * This method will compute the derivative of the undistorted focal plane coordinate for
   * a ground position with respect to a spacecraft position coordinate, using the current
   * Spice settings (time and kernels) without resetting the current point values for lat/lon/radius/x/y.
   *
   * @param varType enumerated partial type (definitions in SpicePosition)
   * @param coefIndex coefficient index of fit polynomial
   * @param *dx pointer to partial derivative of undistorted focal plane x
   * @param *dy pointer to partial derivative of undistorted focal plane y
   *
   * @return conversion was successful
   */
  // d_slantRange = (lookB dot d_lookB) / slantRange
  // d_dopplerFrequency = -dopplerFrequency/slantRange*d_slantRange -
  //                      2./wavelength/slantRange*(d_lookB dot vlookB) -
  //                      2./wavelength/slantRange*(lookB dot d_vlookB)

  // Add the partial for the x coordinate of the position (differentiating
  // point(x,y,z) - spacecraftPosition(x,y,z) in body-fixed and the velocity
  // Load the derivative of the state into d_lookJ
  bool RadarGroundMap::GetdXYdPosition(const SpicePosition::PartialType varType, int coefIndex,
                                       double *dx, double *dy) {
    SpicePosition *instPos = p_camera->InstrumentPosition();
    SpiceRotation *bodyRot = p_camera->BodyRotation();

    std::vector <double> d_lookJ(6);
    vequ_c(&(instPos->CoordinatePartial(varType, coefIndex))[0], &d_lookJ[0]);
    vequ_c(&(instPos->VelocityPartial(varType, coefIndex))[0], &d_lookJ[3]);

    std::vector<double> d_lookB =  bodyRot->ReferenceVector(d_lookJ);

    double d_slantRange = (-1.) * vdot_c(&p_lookB[0], &d_lookB[0]) / p_groundSlantRange;
    double d_dopplerFreq = (-1.) * p_groundDopplerFreq * d_slantRange / p_groundSlantRange -
                           2. / p_waveLength / p_groundSlantRange * vdot_c(&d_lookB[0], &p_sB[3]) +
                           2. / p_waveLength / p_groundSlantRange * vdot_c(&p_lookB[0], &d_lookB[3]);

    *dx = d_slantRange * 1000.0 / p_rangeSigma;// km to meters, then to focal plane coord
    *dy = d_dopplerFreq / p_dopplerSigma;   // htz scaled to focal plane

    return true;
  }

  /** Compute derivative of focal plane coordinate w/r to ground point from ground position using current Spice from SetImage call
   *
   * This method will compute the derivative of the undistorted focal plane coordinate for
   * a ground position with respect to lat, lon, or radius, using the current Spice settings (time and kernels)
   * without resetting the current point values for lat/lon/radius/x/y.
   *
   * @param varType enumerated partial type (definitions in SpicePosition)
   * @param coefIndex coefficient index of fit polynomial
   * @param *dx pointer to partial derivative of undistorted focal plane x
   * @param *dy pointer to partial derivative of undistorted focal plane y
   *
   * @return conversion was successful
   */
  bool RadarGroundMap::GetdXYdPoint(double lat, double lon, double radius, PartialType wrt,
                                    double *dx, double *dy) {

    //  TODO  add a check to make sure p_lookB has been set

    // Get the partial derivative of the surface point
    std::vector<double> d_lookB = PointPartial(lat, lon, radius, wrt);

    double d_slantRange = vdot_c(&p_lookB[0], &d_lookB[0]) / p_groundSlantRange;  // km
    // After switching to J2000, the last term will not be 0. as it is in body-fixed
//    double d_dopplerFreq = p_groundDopplerFreq*d_slantRange/p_groundSlantRange // Ken
//      + 2./p_waveLength/p_groundSlantRange*vdot_c(&d_lookB[0], &p_sB[3]);
    double d_dopplerFreq = (-1.) * p_groundDopplerFreq * d_slantRange / p_groundSlantRange
                           + 2. / p_waveLength / p_groundSlantRange * vdot_c(&d_lookB[0], &p_sB[3]);
//        + 2./p_wavelength/slantRange*vdot_c(&p_lookB[0], 0);

    *dx = d_slantRange * 1000.0 / p_rangeSigma;
    *dy = d_dopplerFreq / p_dopplerSigma;

    return true;
  }

}
