/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $                                                             
 * $Date: 2009/09/09 23:42:41 $                                                 
                
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
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "Camera.h"
#include "Projection.h"
#include "SpecialPixel.h"
#include "Stereo.h"

#include "naif/SpiceUsr.h"
#include "naif/SpiceZfc.h"
#include "naif/SpiceZmc.h"

namespace Isis {


  bool Stereo::Elevation(Camera &cam1, Camera &cam2, double &radius,
                         double &latitude, double &longitude, 
                         double &sepang, double &error) {

    // Gut check on input Camera points
    radius = latitude = longitude = Isis::Null;
    if ( !cam1.HasSurfaceIntersection() ) return (false);
    if ( !cam2.HasSurfaceIntersection() ) return (false);

    // Get spacecraft position from target
    double TC1[3], TC2[3];
    TargetToSpacecraft(cam1, TC1);
    TargetToSpacecraft(cam2, TC2);


    // Get surface vectors from center of body to surface
    double TP1[3], TP2[3];
    TargetToSurface(cam1, TP1);
    TargetToSurface(cam2, TP2);

    // Stereo angle
    sepang = vsep_c(TC1, TC2) * dpr_c();

    SpiceDouble CP1[3], CP2[3];
    vsub_c(TC1, TP1, CP1);
    vsub_c(TC2, TP2, CP2);

    sepang = vsep_c(CP1, CP2) * dpr_c();

    double DR1, DR2;
    DR1 = vnorm_c(CP1);
    DR2 = vnorm_c(CP2);

    vscl_c(1.0/DR1, CP1, CP1);
    vscl_c(1.0/DR2, CP2, CP2);

    // Do stereo intersections
    double aa = CP2[0];
    double bb = CP2[1];
    double cc = CP2[2];
    double xx = CP1[0];
    double yy = CP1[1];
    double zz = CP1[2];

    // Vector between both spacecraft
    double dd = TC2[0] - TC1[0];
    double ee = TC2[1] - TC1[1];
    double ff = TC2[2] - TC1[2];

    //  Do the stereo intersection 
    double bzcy = bb*zz - cc*yy;
    double cebf = cc*ee - bb*ff;
    double cxaz = cc*xx - aa*zz;
    double afcd = aa*ff - cc*dd;
    double aybx = aa*yy - bb*xx;
    double bdae = bb*dd - aa*ee;

    // Get fraction `T' along left vector to "intersection point"
    double T=-(bzcy*cebf+cxaz*afcd+aybx*bdae)/
              (bzcy*bzcy+cxaz*cxaz+aybx*aybx);
    double lx=TC1[0] + T * CP1[0];
    double ly=TC1[1] + T * CP1[1];
    double lz=TC1[2] + T * CP1[2];

    //  Find the Perp. vector between both lines (at shortest sep.)
    double x = TC2[0] - lx;
    double y = TC2[1] - ly;
    double z = TC2[2] - lz;

    // Find the separation distance - useful later
    double rx = y * CP2[2] - CP2[1] * z;
    double ry = CP2[0] * z - x * CP2[2];
    double rz = x * CP2[1] - CP2[0] * y;
    double dr = std::sqrt(rx*rx+ry*ry+rz*rz);

    // Find position of intersection on lower line
    rx = CP1[1] * CP2[2] - CP2[1] * CP1[2];
    ry = CP2[0] * CP1[2] - CP1[0] * CP2[2];
    rz = CP1[0] * CP2[1] - CP2[0] * CP1[1];
    double raa = std::sqrt(rx*rx+ry*ry+rz*rz);

    // Normalize our new Perpendicular vector
    rx = rx/raa;
    ry = ry/raa;
    rz = rz/raa;

    // Get the other intersection position
    rx = lx - rx*dr;
    ry = ly - ry*dr;
    rz = lz - rz*dr;

    // Compute the mid point of the intersection on the planets
    // surface
    double mx = (lx+rx)/2.0;
    double my = (ly+ry)/2.0;
    double mz = (lz+rz)/2.0;
    Rectangular(mx, my, mz, latitude, longitude, radius);
    radius *= 1000.0 ;  // convert to meters
    error = dr * 1000.0;
    return (true);
  }


  void Stereo::Spherical(const double latitude, const double longitude, 
                         const double radius, 
                         double &x, double &y, double &z) { 
    SpiceDouble rec[3];
    latrec_c(radius/1000.0, longitude*rpd_c(), latitude*rpd_c(), &rec[0]);
    x = rec[0];
    y = rec[1];
    z = rec[2];
    return;
  }

   void Stereo::Rectangular(const double x, const double y, const double z, 
                            double &latitude, double &longitude,
                            double &radius) { 
     SpiceDouble rec[3];
     rec[0] = x;
     rec[1] = y;
     rec[2] = z;
     reclat_c(&rec[0], &radius, &longitude, &latitude);
     longitude *= dpr_c();
     latitude *= dpr_c();
     longitude = Projection::To360Domain(longitude);
     return;
     return;
   }

   std::vector<double> Stereo::Array2StdVec(const double d[3]) {
     std::vector<double> v;
     for ( int  i = 0 ; i < 3 ; i++ ) {
       v.push_back(d[i]);
     }
     return (v);
   }

   double *Stereo::StdVec2Array(const std::vector<double> &v, double *d) {
     if ( d == NULL ) {
       d = new double(v.size());
     }

     for ( unsigned int i = 0 ; i < v.size() ; i++ ) {
       d[i] = v[i];
     }
     return (d);
   }

   void Stereo::TargetToSpacecraft(Camera &camera, double TP[3]) {
     camera.InstrumentPosition(TP);
     return;
   }

   void Stereo::TargetToSurface(Camera &camera, double TC[3]) {
     camera.Coordinate(TC);
     return;
   }

}
