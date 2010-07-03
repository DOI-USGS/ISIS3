/**
 * @file
 * $Revision: 1.3 $
 * $Date: 2009/03/07 18:02:33 $
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

#ifndef VariableLineScanCameraDetectorMap_h
#define VariableLineScanCameraDetectorMap_h

#include "LineScanCameraDetectorMap.h"

namespace Isis {
  class LineRateChange;

  /** Convert between parent image coordinates and detector coordinates
   *
   * This class is used to convert between parent dector coordinates
   * (sample/line) and detector coordinates for a line scan camera.
   *
   * @author 2008-08-08 Steven Lambright
   * 
   * @ingroup Camera
   * @see Camera
   *
   * @internal
   *   @history 2008-08-08 Steven Lambright Original version
   *   @history 2009-03-07 Debbie A. Cook Removed reference to obsolute CameraDetectorMap methods
   *
   */
  class VariableLineScanCameraDetectorMap : public LineScanCameraDetectorMap {
    public:
      /** 
       * Constructs a VariableLineScanCameraDetectorMap.
       * 
       * @param parent The camera
       * @param p_lineRates This should be a vector with an entry for every 
       *          scan rate change in it. The pair consists of the line number and
       *          ET of the changed time; the first entry should be line 1 and the last 
       *          entry should be one line past the end of the image. See
       *          HrscCamera for an example.
       */
      VariableLineScanCameraDetectorMap(Camera *parent, std::vector< LineRateChange > &lineRates);

      //! Destructor
      virtual ~VariableLineScanCameraDetectorMap() {};

      virtual bool SetParent(const double sample, const double line);

      virtual bool SetDetector(const double sample, const double line);

    private:
      std::vector< LineRateChange > &p_lineRates;
  };

  class LineRateChange {
    public:
      LineRateChange(int line, double stime, double rate) {
        p_line = line;
        p_stime = stime;
        p_rate = rate;
      };

      int GetStartLine() { return p_line; }
      double GetStartEt() { return p_stime; }
      double GetLineScanRate() { return p_rate; }

    private:
      int p_line;
      double p_stime;
      double p_rate;
  };
};
#endif
