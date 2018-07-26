/**
 * @file
 * $Revision: 1.7 $
 * $Date: 2010/03/27 06:36:41 $
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
#include "IrregularBodyCameraGroundMap.h"

#include <iostream>

#include <QDebug>

#include <SpiceUsr.h>

#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifStatus.h"
#include "SurfacePoint.h"
#include "Target.h"

using namespace std;

namespace Isis {

  /** 
   * Constructor
   *
   * @param parent Pointer to camera to be used for mapping with ground
   */
  IrregularBodyCameraGroundMap::IrregularBodyCameraGroundMap(Camera *parent,
                                                             const bool isIrregular) :
                                CameraGroundMap(parent), m_isBodyIrregular(isIrregular) {
  }


  /** 
   * Compute undistorted focal plane coordinate from ground position using current Spice 
   * from SetImage call
   *
   * This method will compute the undistorted focal plane coordinate for
   * a ground position, using the current Spice settings (time and kernels)
   * without resetting the current point values for lat/lon/radius/x/y.  The
   * class value for m_pB and m_lookJ are set by this method.
   *
   * @param point Surface point (ground position) 
   * @param cudx [out] Pointer to computed undistored x focal plane coordinate
   * @param cudy [out] Pointer to computed undistored y focal plane coordinate
   *
   * @return @b bool If conversion was successful
   */
  bool IrregularBodyCameraGroundMap::GetXY(const SurfacePoint &point, 
                                           double *cudx, double *cudy) {

    // Check to determine if the body is irregular. If not, go ahead and
    // use CameraGroundMap implementation
      if ( !m_isBodyIrregular ) {
          return ( CameraGroundMap::GetXY(point, cudx, cudy) );
      }
      

    // Do everything CameraGroundMap::GetXY does but the emission angle check.

    vector<double> pB(3);
    pB[0] = point.GetX().kilometers();
    pB[1] = point.GetY().kilometers();
    pB[2] = point.GetZ().kilometers();

    // Check for Sky images
    if (p_camera->target()->isSky()) {
      return false;
    }

    // Should a check be added to make sure SetImage has been called???

    // Get spacecraft vector in j2000 coordinates
    SpiceRotation *bodyRot = p_camera->bodyRotation();
    SpiceRotation *instRot = p_camera->instrumentRotation();
    vector<double> pJ = bodyRot->J2000Vector(pB);
    vector<double> sJ = p_camera->instrumentPosition()->Coordinate();

    // Calculate lookJ
    vector<double> lookJ(3);
    for (int ic = 0; ic < 3; ic++) {
      lookJ[ic] = pJ[ic] - sJ[ic];
    }

    // Save pB for target body partial derivative calculations NEW *** DAC 8-14-2015
    m_pB = pB;
    
    // Ok, this is where the emission angle check would be but we will assume 
    // it is always visible until a better solution can be identified. This 
    // method is primarily use in BundleAdjust and returning false will cause 
    // an abort. This must be fixed!
   
    
    // Get the look vector in the camera frame and the instrument rotation
    m_lookJ.resize(3);
    m_lookJ = lookJ;
    vector<double> lookC(3);
    lookC = instRot->ReferenceVector(m_lookJ);

    // Get focal length with direction for scaling coordinates
    double fl = p_camera->DistortionMap()->UndistortedFocalPlaneZ();

    *cudx = lookC[0] * fl / lookC[2];
    *cudy = lookC[1] * fl / lookC[2];
    return true;
  }

}
