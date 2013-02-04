/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2010/03/27 06:56:17 $
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
#include "RadarSlantRangeMap.h"

#include "IString.h"
#include "iTime.h"
#include "PvlSequence.h"

namespace Isis {
  /** Radar ground to slant range map constructor
   *
   * Create a map from ground range distance to slant range
   * distance on a radar instrument
   *
   * @param parent        the parent camera that will use this distortion map
   *
   */
  RadarSlantRangeMap::RadarSlantRangeMap(Camera *parent, double groundRangeResolution) :
    CameraDistortionMap(parent, 1.0) {

    p_camera = parent;
    p_et = DBL_MAX;
    p_a[0] = p_a[1] = p_a[2] = p_a[3] = 0.0;

    // Need to come up with an initial guess when solving for ground
    // range given slantrange. We will compute the ground range at the
    // near and far edges of the image by evaluating the sample-to-
    // ground-range equation: r_gnd=(S-1)*groundRangeResolution
    // at the edges of the image. We also need to add some padding to
    // allow for solving for coordinates that are slightly outside of
    // the actual image area. Use S=-0.25*p_camera->Samples() and
    // S=1.25*p_camera->Samples().
    p_initialMinGroundRangeGuess = (-0.25 * p_camera->Samples() - 1.0)
                                   * groundRangeResolution;
    p_initialMaxGroundRangeGuess = (1.25 * p_camera->Samples() - 1.0)
                                   * groundRangeResolution;
    p_tolerance = 0.1; // Default tolerance is a tenth of a meter
    p_maxIterations = 30;
  }

  /** Set the ground range and compute a slant range
   *
   */
  bool RadarSlantRangeMap::SetFocalPlane(const double dx, const double dy) {
    p_focalPlaneX = dx;  // dx is a ground range distance in meters
    p_focalPlaneY = dy;  // dy is Doppler shift in htz and should always be 0

    if(p_et != p_camera->time().Et()) ComputeA();
    double slantRange = p_a[0] + p_a[1] * dx + p_a[2] * dx * dx + p_a[3] * dx * dx * dx; // meters

    p_camera->SetFocalLength(slantRange);
    p_undistortedFocalPlaneX = slantRange / p_rangeSigma;
    p_undistortedFocalPlaneY = 0;

    return true;
  }

  /** Set the slant range and compute a ground range
   *
   */
  bool RadarSlantRangeMap::SetUndistortedFocalPlane(const double ux,
      const double uy) {
    p_undistortedFocalPlaneX = ux * p_rangeSigma;    // ux converts to slant range in meters
    p_undistortedFocalPlaneY = uy * p_dopplerSigma;  // uy converts to Doppler shift in htz and should always be 0

    if(p_et != p_camera->time().Et()) ComputeA();

    // Evaluate the ground range at the 2 extremes of the image
    double slant = p_undistortedFocalPlaneX;
    double minGroundRangeGuess = slant - (p_a[0] + p_initialMinGroundRangeGuess *
                                          (p_a[1] + p_initialMinGroundRangeGuess * (p_a[2] +
                                              p_initialMinGroundRangeGuess * p_a[3])));
    double maxGroundRangeGuess = slant - (p_a[0] + p_initialMaxGroundRangeGuess *
                                          (p_a[1] + p_initialMaxGroundRangeGuess * (p_a[2] +
                                              p_initialMaxGroundRangeGuess * p_a[3])));

    // If the ground range guesses at the 2 extremes of the image are equal
    // or they have the same sign, then the ground range cannot be solved for.
    if((minGroundRangeGuess == maxGroundRangeGuess) ||
        (minGroundRangeGuess < 0.0 && maxGroundRangeGuess < 0.0) ||
        (minGroundRangeGuess > 0.0 && maxGroundRangeGuess > 0.0)) return false;

    // Use Wijngaarden/Dekker/Brent algorithm to find a root of the function:
    // g(groundRange) = slantRange - (p_a[0] + groundRange * (p_a[1] +
    //                  groundRange * (p_a[2] + groundRange * p_a[3])))
    // The algorithm used is a combination of the bisection method with the
    // secant method.
    int iter = 0;
    double eps = 3.E-8;
    double ax = p_initialMinGroundRangeGuess;
    double bx = p_initialMaxGroundRangeGuess;
    double fax = minGroundRangeGuess;
    double fbx = maxGroundRangeGuess;
    double fcx = fbx;
    double cx = 0.0;
    double d = 0.0;
    double e = 0.0;
    double tol1;
    double xm;
    double p, q, r, s, t;

    do {
      iter++;
      if(fbx * fcx > 0.0) {
        cx = ax;
        fcx = fax;
        d = bx - ax;
        e = d;
      }
      if(fabs(fcx) < fabs(fbx)) {
        ax = bx;
        bx = cx;
        cx = ax;
        fax = fbx;
        fbx = fcx;
        fcx = fax;
      }
      tol1 = 2.0 * eps * fabs(bx) + 0.5 * p_tolerance;
      xm = 0.5 * (cx - bx);
      if(fabs(xm) <= tol1 || fbx == 0.0) {
        p_focalPlaneX = bx;
        p_focalPlaneY = 0.0;
        return true;
      }
      if(fabs(e) >= tol1 && fabs(fax) > fabs(fbx)) {
        s = fbx / fax;
        if(ax == cx) {
          p = 2.0 * xm * s;
          q = 1.0 - s;
        }
        else {
          q = fax / fcx;
          r = fbx / fcx;
          p = s * (2.0 * xm * q * (q - r) - (bx - ax) * (r - 1.0));
          q = (q - 1.0) * (r - 1.0) * (s - 1.0);
        }
        if(p > 0.0) q = -q;
        p = fabs(p);
        t = 3.0 * xm * q - fabs(tol1 * q);
        if(t > fabs(e * q)) t = fabs(e * q);
        if(2.0 * p < t) {
          e = d;
          d = p / q;
        }
        else {
          d = xm;
          e = d;
        }
      }
      else {
        d = xm;
        e = d;
      }
      ax = bx;
      fax = fbx;
      if(fabs(d) > tol1) {
        bx = bx + d;
      }
      else {
        if(xm >= 0.0) {
          t = fabs(tol1);
        }
        else {
          t = -fabs(tol1);
        }
        bx = bx + t;
      }
      fbx = slant - (p_a[0] + bx * (p_a[1] + bx * (p_a[2] + bx * p_a[3])));
    }
    while(iter <= p_maxIterations);

    return false;
  }

  /** Load the ground range/slant range coefficients from the
   *  RangeCoefficientSet keyword
   *
   */
  void RadarSlantRangeMap::SetCoefficients(PvlKeyword &keyword) {
    PvlSequence seq;
    seq = keyword;
    for(int i = 0; i < seq.Size(); i++) {
      // TODO:  Test array size to be 4 if not throw error
      std::vector<QString> array = seq[i];
      double et;
      utc2et_c(array[0].toAscii().data(), &et);
      p_time.push_back(et);
      p_a0.push_back(toDouble(array[1]));
      p_a1.push_back(toDouble(array[2]));
      p_a2.push_back(toDouble(array[3]));
      p_a3.push_back(toDouble(array[4]));
      // TODO:  Test that times are ordered if not throw error
      // Make the mrf2isis program sort them if necessary
    }
  }

  /** Set new A-coefficients based on the current ephemeris time.
   *  The A-coefficients used will be those with the closest
   *  ephemeris time to the current ephemeris time.
   */
  void RadarSlantRangeMap::ComputeA() {
    double currentEt = p_camera->time().Et();

    std::vector<double>::iterator pos = lower_bound(p_time.begin(), p_time.end(), currentEt);

    int index = p_time.size() - 1;
    if (currentEt <= p_time[0]) {
      p_a[0] = p_a0[0];
      p_a[1] = p_a1[0];
      p_a[2] = p_a2[0];
      p_a[3] = p_a3[0];
    }
    else if (currentEt >= p_time.at(index)) {
      p_a[0] = p_a0[index];
      p_a[1] = p_a1[index];
      p_a[2] = p_a2[index];
      p_a[3] = p_a3[index];
    } else { 
      index = pos - p_time.begin();
      double weight = (currentEt - p_time.at(index-1)) / 
                      (p_time.at(index) - p_time.at(index-1));
      p_a[0] = p_a0[index-1] * (1.0 - weight) + p_a0[index] * weight;
      p_a[1] = p_a1[index-1] * (1.0 - weight) + p_a1[index] * weight;
      p_a[2] = p_a2[index-1] * (1.0 - weight) + p_a2[index] * weight;
      p_a[3] = p_a3[index-1] * (1.0 - weight) + p_a3[index] * weight;
    }
  }

  /** Set the weight factors for slant range and Doppler shift
   *
   */
  void RadarSlantRangeMap::SetWeightFactors(double range_sigma, double doppler_sigma) {
    p_rangeSigma = range_sigma; // meters scaling factor
    p_dopplerSigma = doppler_sigma; // htz scaling factor
  }
}
