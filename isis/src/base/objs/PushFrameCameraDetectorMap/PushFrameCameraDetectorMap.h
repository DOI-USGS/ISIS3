/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2009/10/21 18:37:02 $
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

#ifndef PushFrameCameraDetectorMap_h
#define PushFrameCameraDetectorMap_h

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
   *            forward direction and inside framelet check in the reverse
   *            direction
   */
  class PushFrameCameraDetectorMap : public CameraDetectorMap {
  public:
    /** Construct a detector map for push frame cameras
     *
     * @param parent    The parent camera model for the detector map
     * @param etStart   starting ephemeris time in seconds
     *                  at the first framelet (not including padded
     *                  framelets).
     * @param frameletRate  the time in seconds between framelets
     * @param frameletHeight Physical height of framelet in lines
     *                       (don't account for summing)
     *
     */
    PushFrameCameraDetectorMap(Camera *parent, const double etStart,
                               const double frameletRate, int frameletHeight) :
    CameraDetectorMap(parent){
      p_etStart = etStart;
      p_exposureDuration = 0.0;
      p_frameletRate = frameletRate;
      p_frameletHeight = frameletHeight;
      p_frameletOffset = 0;
      p_flippedFramelets = true;
      p_timeAscendingFramelets = true;
      p_nframelets = 0;
      p_bandStartDetector = 0;
    }

    //! Destructor
    virtual ~PushFrameCameraDetectorMap() {};

    /** Reset the starting ephemeris time
     *
     * Use this method to reset the starting time of the top edge of
     * the first line in the parent image.  That is the time, prior
     * to cropping, scaling, or padding.  Usually this will not need
     * to be done unless the time changes between bands.
     *
     * @param etStart starting ephemeris time in seconds
     *
     */
    void SetStartTime (const double etStart) { p_etStart = etStart;};


    /** Change the exposure duration in seconds
     *
     *  Use this method to change the exposure duration of each
     *  framelet which may be different than the framelet rate.
     *
     * @param exposureDuration
     */
    void SetExposureDuration(double exposureDuration) {
      p_exposureDuration = exposureDuration;
    }


    /** Reset the frame rate
     *
     * Use this method to reset the time between framelets.  Usually
     * this will not need to be done unless the rate changes between
     * bands.
     *
     * @param frameletRate the time in seconds between framelets
     *
     */
    void SetFrameletRate (const double frameletRate) { p_frameletRate = frameletRate;};

    //! Return the time in seconds between framelets
    double FrameletRate () const { return p_frameletRate;};

    /** Reset the frame offset
    *
    * Use this method to reset the frame offset.  Usually this will
    * not need to be done unless the offset changes between bands.
    *
    * @param frameletOffset Number of frames offset in cube
    *
    */
    void SetFrameletOffset (int frameletOffset) { p_frameletOffset = frameletOffset;};

    //! Return the frame offset
    int FrameletOffset () const { return p_frameletOffset;};

    virtual bool SetParent(const double sample, const double line);

    virtual bool SetDetector(const double sample, const double line);

    void SetFramelet(int framelet);

    /**
     * This method returns the current framelet. This framelet is 
     * calculated when SetParent is called.
     * 
     * @return int The current framelet
     */
    int Framelet() { return p_framelet; }


    /** Change the starting line in the detector based on band
     *
     *  Use this method to change which line is read out of the
     *  CCD for any given band.  That is, as the virtual SetBand
     *  method for the specfic camera is invoked this method should
     *  be called.
     *
     * @param firstLine 0-based offset to the first line (first line of 
     *                  detector = 0)
     */
    void SetBandFirstDetectorLine(int firstLine) { p_bandStartDetector = firstLine;}

    //! Return the starting line in the detector for the current band
    int GetBandFirstDetectorLine() { return p_bandStartDetector;}

    /** Changes the direction of the framelets
     *
     *  Use this method to change which direction the framelets are ordered.
     *  In some cases, the top framelet from the raw instrument data has been
     *  moved to the bottom of the image and this compensates for that.
     *  
     *  @param frameletsFlipped False if framelets flipped
     *  @param nframelets Number of framelets in each band, ignored
     *                    if frameletsFlipped is set to false
     */
    void SetFlippedFramelets(bool frameletsFlipped, int nframelets) {
      p_timeAscendingFramelets = !frameletsFlipped;
      p_nframelets = nframelets;
    }

    /** Mirrors the each framelet in the file
     *  
     *  Use this method to change which direction the framelets are geometrically
     *  placed. If the first line in the framelet has been changed to the last line
     *  in the framelet, then this should be true (DEFAULT).
     * 
     * @param frameletsFlipped True if geometric flip in the framelet
     */
    void SetGeometricallyFlippedFramelets(bool frameletsFlipped) {
      p_flippedFramelets = frameletsFlipped;
    }

    /**
     * This returns the starting ET of this band
     * 
     * 
     * @return double Starting time (often band-dependant)
     */
    double StartEphemerisTime () const { return p_etStart; };

    /**
     * Return the total number of framelets including padding
     *
     * @return int
     */
    int TotalFramelets() const
      { return (int) (p_camera->ParentLines() / (p_frameletHeight / LineScaleFactor())); };


    /**
     * This returns the calculated framelet sample 
     *  
     * @return double Current framelet sample
     */
    double frameletSample() const { return p_frameletSample; };

    /**
     * This returns the calculated framelet line 
     *  
     * @return double Current framelet line
     */    
    double frameletLine() const { return p_frameletLine; };

    /**
     * This returns how many lines are considered a single framelet
     * 
     * @return int Number of lines in a framelet
     */
    int frameletHeight() const { return p_frameletHeight; };

    bool timeAscendingFramelets() { return p_timeAscendingFramelets; }

  private:
    double p_etStart;      //!<Starting time at the top of the 1st parent line
    double p_exposureDuration; //!<Exposure duration in secs
    double p_frameletRate;    //!<iTime between framelets in parent cube
    int    p_frameletHeight; //!<Height of a framelet in detector lines
    int    p_bandStartDetector; //!<The first detector line of current band
    int    p_frameletOffset; //!<The numner of framelets padding the top of the band
    int    p_framelet;  //!<The current framelet
    int    p_nframelets; //!<If flipped framelets, the number of framelets in this band

    double p_frameletSample; //!<The sample in the current framelet
    double p_frameletLine; //!<The line in the current framelet

    bool   p_flippedFramelets;  //!< Is geometry in a framelet flipped
    bool   p_timeAscendingFramelets;  //!< Are framelets reversed from top-to-bottom in file

  };
};
#endif
