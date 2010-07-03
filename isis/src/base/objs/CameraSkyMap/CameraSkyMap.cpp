/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2008/07/15 15:07:19 $                                                                 
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

#include "CameraSkyMap.h"
#include "NaifStatus.h"

namespace Isis {
  /** Constructor a map between focal plane x/y and right acension/declination
   * 
   * @param parent  parent camera which will use this map
   *
   */
  CameraSkyMap::CameraSkyMap(Camera *parent) {
    p_camera = parent;
    p_camera->SetSkyMap(this);
  }

  /** Compute ra/dec from focal plane coordinate
   * 
   * This method will compute the right ascension and declination given an
   * undistorted focal plane coordinate.  Note that the ra/dec values
   * can be obtained from the parent camera class passed into the constructor.
   * 
   * @param ux distorted focal plane x in millimeters
   * @param uy distorted focal plane y in millimeters
   * @param uz distorted focal plane z in millimeters
   * 
   * @return conversion was successful
   */
  bool CameraSkyMap::SetFocalPlane(const double ux, const double uy, 
                                   double uz) {
    NaifStatus::CheckErrors();

    SpiceDouble lookC[3];
    lookC[0] = ux;
    lookC[1] = uy;
    lookC[2] = uz;

    SpiceDouble unitLookC[3];
    vhat_c(lookC,unitLookC);
    p_camera->SetLookDirection(unitLookC);

    NaifStatus::CheckErrors();

    return true;
  }

  /**
   * Compute undistorted focal plane coordinate from ra/dec
   * 
   * @param ra The right ascension angle
   * @param dec The declination
   * 
   * @return conversion was successful
   * @todo what happens if we are looking behind the focal plane?????
   * @todo what happens if we are looking parallel to the focal plane??
   * @todo can lookC[2] == zero imply parallel
   * @todo can this all be solved by restricting the physical size of 
   * the focal plane?
   */
  bool CameraSkyMap::SetSky(const double ra, const double dec) {
    p_camera->Sensor::SetRightAscensionDeclination(ra,dec);
    double lookC[3];
    p_camera->Sensor::LookDirection(lookC);
    double scale = p_camera->FocalLength() / lookC[2];
    p_focalPlaneX = lookC[0] * scale;
    p_focalPlaneY = lookC[1] * scale;
    return true;
  }
}
