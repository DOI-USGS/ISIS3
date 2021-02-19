#ifndef PushFrameCameraDetectorMap_h
#define PushFrameCameraDetectorMap_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CameraDetectorMap.h"

namespace Isis {
  /** Convert between parent image coordinates and detector coordinates
   *
   * This class is used to convert between parent dector coordinates
   * (sample/line) and detector coordinates for a push frame camera.
   *
   * @ingroup Camera
   *
   * @see Camera
   *
   * @author 2007-10-15 Steven Lambright
   *
   * @internal
   *   @history 2008-06-18 Steven Lambright Added documentation
   *   @history 2008-10-23 Steven Lambright Added optimizations, fixed misc. bugs
   *   @history 2009-03-07 Debbie A. Cook Removed reference to obsolute CameraDetectorMap methods
   *   @history 2009-06-02 Steven Lambright Fixed framelet detection in the
   *                           forward direction and inside framelet check in the reverse
   *                           direction
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                           coding standards. References #972.
   *   @history 2014-04-17 Jeannie Backer - Renamed SetFlippedFramelets and
   *                           SetGeometricallyFlippedFramelets to SetFrameletOrderReversed and
   *                           SetFrameletsGeometricallyFlipped, respectively.  Moved
   *                           method implementations to cpp file.
   *                           References #1659.
   *   @history 2016-10-18 Kristin Berry - Added additional SetParent method with time offset.
   *                           Added detltaT variable to SetFramelet method. Updated unitTest.
   *                           References #4476.
   *   @history 2016-10-19 Jesse Mapel - Added exposureDuration method for accessing a pixel's
   *                           exposure duration. Updated documentation and unit test.
   *                           References #4476.
   */
  class PushFrameCameraDetectorMap : public CameraDetectorMap {
    public:
      PushFrameCameraDetectorMap(Camera *parent, 
                                 const double etStart,
                                 const double frameletRate, 
                                 int frameletHeight);

      virtual ~PushFrameCameraDetectorMap();

      virtual bool SetParent(const double sample, 
                             const double line);
      virtual bool SetParent(const double sample, 
                             const double line, 
                             const double deltaT);

      virtual bool SetDetector(const double sample, 
                               const double line);

      double FrameletRate() const;
      void SetFrameletRate(const double frameletRate);

      int FrameletOffset() const;
      void SetFrameletOffset(int frameletOffset);

      void SetFramelet(int framelet, const double deltaT=0);
      int Framelet();
      
      void SetBandFirstDetectorLine(int firstLine);
      int GetBandFirstDetectorLine();

      void SetStartTime(const double etStart);
      double StartEphemerisTime() const;

      void SetExposureDuration(double exposureDuration);
      virtual double exposureDuration(const double sample,
                                      const double line,
                                      const int band) const;

      void SetFrameletOrderReversed(bool frameletOrderReversed, int nframelets);
      void SetFrameletsGeometricallyFlipped(bool frameletsFlipped);

      int TotalFramelets() const;
      double frameletSample() const;
      double frameletLine() const;
      int frameletHeight() const;
      bool timeAscendingFramelets();

    private:
      double p_etStart;           //!< Starting time at the top of the first parent line.
      double p_exposureDuration;  //!< Exposure duration in secs.
      double p_frameletRate;      //!< iTime between framelets in parent cube.
      int    p_frameletHeight;    //!< Height of a framelet in detector lines.
      int    p_bandStartDetector; //!< The first detector line of current band.
      int    p_frameletOffset;    //!< The numner of framelets padding the top of the band.
      int    p_framelet;          //!< The current framelet.
      int    p_nframelets;        //!< If flipped framelets, the number of framelets in this band.

      double p_frameletSample; //!< The sample in the current framelet.
      double p_frameletLine;   //!< The line in the current framelet.

      bool   p_flippedFramelets;       //!< Indicates whether the geometry in a framelet is flipped.
      bool   p_timeAscendingFramelets; //!< Are framelets reversed from top-to-bottom in file.

  };
};
#endif
