/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2009/03/07 17:57:27 $
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

#ifndef LineScanCameraDetectorMap_h
#define LineScanCameraDetectorMap_h

#include "CameraDetectorMap.h"

namespace Isis {
  /** Convert between parent image coordinates and detector coordinates
   *
   * This class is used to convert between parent dector coordinates
   * (sample/line) and detector coordinates for a line scan camera.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2005-02-09 Jeff Anderson
   *
   * @internal
   *   @history 2009-03-07 Debbie A. Cook Removed reference to obsolute CameraDetectorMap methods
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                           coding standards. References #972.
   *   @history 2013-02-11 E. Howington-Kraus - Added accessor method  StartTime().
   *                           This is tested by application socetlinescankeywords since no
   *                           unitTest exists. References #1490.
   *   @history 2016-10-18 Kristin Berry - Added additional SetParent method with time offset
   *                           deltaT. Updated unitTest. References #4476.
   *   @history 2016-10-19 Jesse Mapel - Added exposureDuration method for accessing
   *                           a pixel's exposure duration. Updated unitTest. References #4476.
   *   @history 2016-10-27 Jeannie Backer - Moved method implementation to cpp file.
   *                           References #4476.
   */
  class LineScanCameraDetectorMap : public CameraDetectorMap {
    public:

      LineScanCameraDetectorMap(Camera *parent, 
                                const double etStart,
                                const double lineRate);
      virtual ~LineScanCameraDetectorMap();

      void SetStartTime(const double etStart);
      void SetLineRate(const double lineRate);
      double LineRate() const;

      virtual double exposureDuration(const double sample,
                                      const double line,
                                      const int band) const;

      virtual bool SetParent(const double sample, 
                             const double line);
      virtual bool SetParent(const double sample, 
                             const double line, 
                             const double deltaT);

      virtual bool SetDetector(const double sample, 
                               const double line);
      double StartTime() const;

    private:
      double p_etStart;     //!< Starting time at the top of the first parent line.
      double p_lineRate;    //!< Time, in seconds, between lines in parent cube.
  };
};
#endif
