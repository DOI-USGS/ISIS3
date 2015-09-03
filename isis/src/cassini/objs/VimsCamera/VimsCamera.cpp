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

#include "VimsCamera.h"
#include "VimsGroundMap.h"
#include "VimsSkyMap.h"

#include <fstream>
#include <iostream>
#include <iomanip>

#include <QDebug>
#include <QList>
#include <QPointF>
#include <QString>

#include "CameraDetectorMap.h"
#include "CameraDistortionMap.h"
#include "CameraFocalPlaneMap.h"
#include "Constants.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "iTime.h"
#include "NaifStatus.h"
#include "SpecialPixel.h"

using namespace std;

namespace Isis {
  /**
   * Constructor for the Cassini Vims Camera Model
   *
   * @param [in] lab   (Pvl &)  Label used to create camera model
   *
   * @internal 
   *   @history 2007-12-12  Tracie Sucharski,  After creating spice cache with
   *                           padding, reset et by calling SetImage(1,1) so that
   *                           et is initialized properly at beginning of image
   *                           without padding.
   * @internal
   *   @history 2011-05-03 Jeannie Walldren - Added NAIF error check.
   *
   */
  VimsCamera::VimsCamera(Cube &cube) : Camera(cube) {
    m_instrumentNameLong = "Visible and Infrared Mapping Spectrometer";
    m_instrumentNameShort = "VIMS";
    m_spacecraftNameLong = "Cassini Huygens";
    m_spacecraftNameShort = "Cassini";
    
    NaifStatus::CheckErrors();

    Pvl &lab = *cube.label();
    PvlGroup &inst = lab.findGroup("Instrument", Pvl::Traverse);
    QString channel = (QString) inst ["Channel"];

    //  Vims pixel pitch is not always square, but Isis does not have the ability to store
    //  more than a single value for pixel pitch.  Member variables for pixelPitch x and y
    //  were created for proper calculation of ifov.
    if (channel == "VIS") {
      //LoadFrameMounting ("CASSINI_SC_COORD","CASSINI_VIMS_V");

      SetFocalLength(143.0);
      if (QString((QString)inst["SamplingMode"]).toUpper() == "NORMAL") {
        SetPixelPitch(3 * .024);
        // Should this .506?  According to 2002 paper ground calibration shows .506 +/- .003 mrad
        m_pixelPitchX = 0.024 * 3;
        m_pixelPitchY = 0.024 * 3;
      }
      else if (QString((QString)inst["SamplingMode"]).toUpper() == "HI-RES") {
        SetPixelPitch(.024);
        m_pixelPitchX = 0.024;
        m_pixelPitchY = 0.024;
      }
      else if (QString((QString)inst["SamplingMode"]).toUpper() == "UNDER") {
        QString msg = "Isis cannot process images with a SamplingMode = \"UNDER\" (or NYQUIST)";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
      else {
        QString msg = "Unknown SamplingMode [" + (QString) inst["SamplingMode"] + "]";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }
    else if (channel == "IR") {
      //LoadFrameMounting ("CASSINI_SC_COORD","CASSINI_VIMS_IR");

      SetFocalLength(426.0);
      SetPixelPitch(.2);
      if (QString((QString)inst["SamplingMode"]).toUpper() == "NORMAL") {
        m_pixelPitchX = 0.2;
        m_pixelPitchY = 0.2;
      }
      else if (QString((QString)inst["SamplingMode"]).toUpper() == "HI-RES") {
        m_pixelPitchX = 0.103;
        m_pixelPitchY = 0.2;
      }
      else if (QString((QString)inst["SamplingMode"]).toUpper() == "UNDER") {
        QString msg = "Isis cannot process images with a SamplingMode = \"UNDER\" (or NYQUIST)";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
      else {
        QString msg = "Unknown SamplingMode [" + (QString) inst["SamplingMode"] + "]";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }

    // Get the start time in et
    QString stime = inst ["NativeStartTime"][0];
    QString intTime = stime.split(".").first();
    stime = stime.split(".").last();

    double etStart = getClockTime(intTime).Et();

    //  Add 2 seconds to either side of time range because the time are for IR
    // channel, the VIS may actually start integrating before NATIVE_START_TIME.
    //  This insures the cache is large enough.
    etStart += toDouble(stime) / 15959.0 - 2.;

    // Get the end time in et
    QString etime = (QString) inst ["NativeStopTime"];
    intTime = etime.split(".").first();
    etime = etime.split(".").last();

    double etStop = getClockTime(intTime).Et();

    //  Add 2 seconds to either side of time range because the time are for IR
    // channel, the VIS may actually start integrating before NATIVE_START_TIME.
    //  This insures the cache is large enough.
    etStop += toDouble(stime) / 15959.0 + 2.;

    //  Setup detector map
    new CameraDetectorMap(this);

    // Setup focal plane map
    new CameraFocalPlaneMap(this, naifIkCode());

    // Setup distortion map
    new CameraDistortionMap(this);

    // Setup the ground and sky map
    new VimsGroundMap(this, lab);
    new VimsSkyMap(this, lab);

    ((VimsGroundMap *)GroundMap())->Init(lab);
    ((VimsSkyMap *)SkyMap())->Init(lab);

    double tol = PixelResolution();

    if (tol < 0.) {
      // Alternative calculation of .01*ground resolution of a pixel
      tol = PixelPitch() * SpacecraftAltitude() / FocalLength() / 1000. / 100.;
    }

    if (channel == "VIS") createCache(etStart, etStop, 64 * 64, tol);
    if (channel == "IR") createCache(etStart, etStop, 64 * 64, tol);

    //  Call SetImage so that the et is reset to beginning of image w/o
    //   padding.
    IgnoreProjection(true);
    SetImage(1, 1);
    IgnoreProjection(false);
    NaifStatus::CheckErrors();
    return;
  }


  /**
   * Returns the pixel ifov offsets from center of pixel.  For vims this will be a rectangle or 
   * square, depending on the sampling mode.  The first vertex is the top left. 
   *  
   * @internal 
   *   @history 2013-08-09 Tracie Sucharski - Add more vertices along each edge.  This might need
   *                          to be a user parameter evenually?  Might be dependent on resolution.
   */
   QList<QPointF> VimsCamera::PixelIfovOffsets() {

     QList<QPointF> offsets;

     //  Create 100 pts on each edge of pixel
     int npts = 100;

     //  Top edge of pixel
     for (double x = -m_pixelPitchX / 2.0; x <= m_pixelPitchX / 2.0; x += m_pixelPitchX / (npts-1)) {
       offsets.append(QPointF(x, -m_pixelPitchY / 2.0));
     }
     //  Right edge of pixel
     for (double y = -m_pixelPitchY / 2.0; y <= m_pixelPitchY / 2.0; y += m_pixelPitchY / (npts-1)) {
       offsets.append(QPointF(m_pixelPitchX / 2.0, y));
     }
     //  Bottom edge of pixel
     for (double x = m_pixelPitchX / 2.0; x >= -m_pixelPitchX / 2.0; x -= m_pixelPitchX / (npts-1)) {
       offsets.append(QPointF(x, m_pixelPitchY / 2.0));
     }
     //  Left edge of pixel
     for (double y = m_pixelPitchY / 2.0; y >= -m_pixelPitchY / 2.0; y -= m_pixelPitchY / (npts-1)) {
       offsets.append(QPointF(-m_pixelPitchX / 2.0, y));
     }

     return offsets;
   }
   
   
   /**
   * This method returns the full instrument name.
   *
   * @return QString
   */
  QString VimsCamera::instrumentNameLong() const {
    return m_instrumentNameLong;
  }
  
  
  /**
   * This method returns the shortened instrument name.
   *
   * @return QString
   */
  QString VimsCamera::instrumentNameShort() const {
    return m_instrumentNameShort;
  }
  
  
  /**
   * This method returns the full spacecraft name.
   * 
   * @return QString
   */
  QString VimsCamera::spacecraftNameLong() const {
    return m_spacecraftNameLong;
  }
  
  
  /**
   * This method returns the shortened spacecraft name.
   *
   * @return QString
   */
  QString VimsCamera::spacecraftNameShort() const {
    return m_spacecraftNameShort;
  }
}

// Plugin
/** 
 * This is the function that is called in order to instantiate a VimsCamera object. 
 *
 * @param lab Cube labels
 *
 * @return Isis::Camera* VimsCamera
 * @internal 
 *   @history 2011-05-03 Jeannie Walldren - Added documentation.  Removed
 *            Cassini namespace.
 */
extern "C" Isis::Camera *VimsCameraPlugin(Isis::Cube &cube) {
  return new Isis::VimsCamera(cube);
}

