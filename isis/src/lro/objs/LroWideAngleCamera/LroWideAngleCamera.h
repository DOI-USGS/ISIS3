#ifndef LroWideAngleCamera_h
#define LroWideAngleCamera_h
/** 
 * @file 
 *  
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software
 *   and related material nor shall the fact of distribution constitute any such
 *   warranty, and no responsibility is assumed by the USGS in connection
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "PushFrameCamera.h"

#include <QString>
#include <QVector>

namespace Isis {
  /**
   * @brief LRO Wide Angle Camera Model
   *
   * This is the camera model for the Lunar Reconnaissance Orbiter wide angle 
   * camera. Much work has been put into this model by the ASU LROC team. 
   *  
   * The current best model (2013-02-19) has the following items changing per 
   * band: 
   *     - FOCAL_LENGTH
   *     - BORESIGHT_SAMPLE
   *     - BORESIGHT_LINE
   *     - OD_K
   *     - TRANSX
   *     - TRANSY
   *     - ITRANSS
   *     - ITRANSL 
   *  
   *  
   * These values are incorporated in the SPICE kernels (FK, IK and IAK). 
   *  
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup LunarReconnaissanceOrbiter
   *
   * @author 2009-07-08 Jeff Anderson
   *
   * @internal
   *   @history 2009-07-15 Steven Lambright - Added support for COLOROFFSET
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer inherit directly from
   *                           Camera
   *   @history 2009-09-09 Steven Lambright - Updated wavelengths
   *   @history 2009-11-06 Steven Lambright - FilterName keyword is now Center
   *   @history 2010-03-15 Steven Lambright - Tiling hint now set to a safe
   *                           value regardless of output projection resolution. Also
   *                           incorporated ASU's changes for new modes.
   *   @history 2010-05-12 Kris Becker - Added checks for number of bands to match number of values
   *                           in BandBin/Center keyword and insure a valid band is selected in
   *                           SetBand() method;  Rewrote the camera distortion model that also
   *                           requires negative coefficients in IK kernel.
   *   @history 2010-08-21 Kris Becker - Reworked the camera model to
   *                           utilize the contents of the IK, which is new. The LRO/LROC IK
   *                           lro_lroc_v14.ti and higher contain the appropriate parameters to
   *                           coincide with the code changes made here. IMPORTANT:  This
   *                           results in Version = 2 of the LroWideAngleCamera as depicted in
   *                           the Camera.plugin for both WAC-UV and WAC-VIS.
   *   @history 2010-10-04 Kris Becker - Modified the frame kernel code to use
   *                           the instrument code instead of the WAC ID.  This change was
   *                           brought about with the release of frames kernel
   *                           lro_frames_2010214_v01.tf (actually used version 2010277 that
   *                           contains updated angles for VIS and UV).
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new
   *                           methods. Updated documentation. Removed Lro namespace wrap
   *                           inside Isis namespace wrap. Added Isis Disclaimer to files.
   *                           Added NAIF error check to constructor.
   *   @history 2012-07-06 Debbie A. Cook - Updated Spice members to be more compliant with
   *                           Isis coding standards. References #972.
   *   @history 2014-04-17 Jeannie Backer - Updated due to method name change in
   *                           PushFrameCameraDetectorMap. Moved method implementations to cpp file.
   *                           References #1659
   *   @history 2013-03-05 Kris Becker - added band dependent parameters as
   *                           determined by the ASU LROC team.
   *   @history 2015-08-25 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   *   @history 2015-10-16 Ian Humphrey - Removed declarations of spacecraft and instrument 
   *                           members and methods and removed implementation of these methods
   *                           since Camera now handles this. References #2335.
   */
  class LroWideAngleCamera : public PushFrameCamera {
    public:
      // constructor
      LroWideAngleCamera(Cube &cube);
      ~LroWideAngleCamera();

      // Sets the band to the band number given
      void SetBand(const int band);
      bool IsBandIndependent();

      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkReferenceId() const;

    private:
      typedef QVector<int>    IntParameterList;
      typedef QVector<double> DblParameterList;
      double p_etStart;              //!< Ephemeris Start iTime
      double p_bandTimeOffset;       //!< Offset iTime for Band
      double p_exposureDur;          //!< Exposure Duration value from labels
      double p_interframeDelay;      //!< Interframe Delay value from labels
      int p_nframelets;                 //!< Number of framelets in whole image
      IntParameterList p_detectorStartLines;
      IntParameterList p_frameletOffsets;
      DblParameterList p_focalLength;
      DblParameterList p_boreSightSample;
      DblParameterList p_boreSightLine;

      int PoolKeySize(const QString &key) const;
      IntParameterList GetVector(const QString &key);
  };
}  // namespace Isis
#endif

