/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2007/02/13 23:03:54 $
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
#ifndef MvicCameraFocalPlaneMap_h
#define MvicCameraFocalPlaneMap_h

#include "CameraFocalPlaneMap.h"

namespace Isis {
  class Camera;
  /** Convert between distorted focal plane and detector coordinates for New Horizons MVIC
   *
   * This class is used to convert between distorted focal plane  coordinates (x/y) in millimeters 
   * and detector coordinates in pixels for the New Horizons/MVIC instrument.  The boresight for 
   * MVIC is in the -X direction, so an extra 90 degree rotations is needed to rotate from the 
   * Y/Z plane into the X/Y plane.  The class expects to find a set of coefficients in the
   * naif instrument (or instrument addendum) kernel that describe the
   * transform from detector to focal plane and vice versa.  The transform
   * from detector to focal plane is:
   *
   * @code
   * x = transx[0] + sample * transx[1] + line * transx[2];
   * y = transy[0] + sample * transy[1] + line * transy[2];
   *
   * where, transx and transy are the coefficients from the naif kernel.
   * The should be in the form of:
   *
   * INSxxxxxx_TRANSX = ( a, b, c)
   * INSxxxxxx_TRANSY = ( d, e, f)
   *
   * where, xxxxxx is the NAIF instrument id code.
   * @endcode
   *
   * Likewise, the inverse transform is:
   *
   * @code
   * samp = itranss[0] + x * itranss[1] + y * itranss[2];
   * line = itransl[0] + x * itransl[1] + y * itransl[2];
   *
   * where, itranss and itranss are the coefficients from the naif kernel.
   * The should be in the form of:
   *
   * INSxxxxxx_ITRANSS = ( a, b, c)
   * INSxxxxxx_ITRANSL = ( d, e, f)
   *
   * where, xxxxxx is the NAIF instrument id code.
   * @endcode
   *
   * @ingroup Camera
   *
   * @author 2005-02-05 Jeff Anderson
   *
   * @see Camera
   *
   * @internal
   *   @history 2007-02-13 Debbie A. Cook - Added methods SignMostSigX() and
   *                           SignMostSigY()
   *   @history 2011-05-25 Janet Barrett and Steven Lambright - Spice::GetDouble
   *                           is no longer static. 
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more
   *                           compliant with Isis coding standards.
   *                           References #972.
   *   @history 2012-11-21 Jeannie Backer - Added accesssor methods:
   *                           TransX(), TransY(), TransS(), and TransL().
   *                           These are tested by application hideal2pds
   *                           since no unitTest exists. Fixed indentation
   *                           of history entries, order of includes,
   *                           moved method implementations to cpp, and
   *                           fixed control statement padding to be more
   *                           compliant with Isis standards. Added
   *                           documentation to member
   *                           variables.References #678.
   *  
   */
  class MvicCameraFocalPlaneMap : public CameraFocalPlaneMap {
    public:
      MvicCameraFocalPlaneMap(Camera *parent, const int naifIkCode);
      MvicCameraFocalPlaneMap(const int naifIkCode);
      virtual ~MvicCameraFocalPlaneMap();

      virtual bool SetDetector(const double sample, const double line);
      virtual bool SetFocalPlane(const double dx, const double dy);

  };
};
#endif
