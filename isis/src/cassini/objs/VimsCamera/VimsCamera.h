#ifndef VimsCamera_h
#define VimsCamera_h
/**
 * @file
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

#include "Camera.h"

#include <QList>
#include <QPointF>
#include <QString>

namespace Isis {
  class Pvl;
  /**
   * @brief Cassini Vims camera model
   *
   *   This is the camera model for the Cassini Vims instrument
   *
   * @ingroup SpiceInstrumentsAndCameras
   * @ingroup Cassini-Huygens
   *
   * @see Camera
   *
   * @author 2006-03-16 Tracie Sucharski
   *
   * @internal
   *
   *   @history 2006-03-16 Tracie Sucharski Original version
   *   @history 2009-04-06 Steven Lambright Fixed problem that caused double
   *                          deletion of sky map / ground map.
   *   @history 2009-08-03 Debbie A. Cook - Added new tolerance argument to
   *                          CreateCache call to be compatible with update to
   *                          Spice class
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                          inherit directly from Camera
   *   @history 2011-01-14 Travis Addair - Added new CK/SPK accessor methods,
   *                          pure virtual in Camera, implemented in mission
   *                          specific cameras.
   *   @history 2011-02-09 Steven Lambright - Major changes to camera classes.
   *   @history 2010-08-05 Jeannie Walldren - Updated documentation. Removed
   *                          Cassini namespace wrap inside Isis namespace wrap.
   *                          Added NAIF error check to constructor.
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new
   *                          methods. Updated documentation. Removed Cassini
   *                          namespace wrap inside Isis namespace wrap. Added
   *                          Isis Disclaimer to files. Added NAIF error check
   *                          to constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                          coding standards. References #972.
   *   @history 2012-03-04 Tracie Sucharski - Added new method, PixelIfovOffsets, which will return
   *                           the ifov offsets,in x and y, from the center of the pixel in mm.  The
   *                           pixel ifov size for vims is dependent on the sampling mode.
   *   @history 2013-09-10 Tracie Sucharski, The Vims 15 Mhz clock actually oscillates closer to
   *                           14.7456 Mhz, so the IR exposure duration, interline delay and
   *                           interframe delay in the VIMS header need to be scaled by 1.01725.
   *                           This change will affect where VIMS pixels map in projected space.
   *                           Fixes #1759.
   *   @history 2013-12-04 Tracie Sucharski - Fixed bug when checking for unsupported Nyquist
   *                           cubes.  The SamplingMode keyword is actually "UNDER" not "NYQUIST".
   *                           Print appropriate error indicating that these cubes are not supported.
   *   @history 2015-08-11 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   */
  class VimsCamera : public Camera {
    public:
      // constructors
      VimsCamera(Cube &cube);

      //! Destroys the VimsCamera object.
      ~VimsCamera() {};

      /**
       * The Vims camera is the only point camera we have.
       *  
       * @return CameraType Camera::Point
       */
      virtual CameraType GetCameraType() const {
        return Point;
      }

//       void SetBand (const int physicalBand);
//       bool IsBandIndependent () { return false; };

      /**
       * CK frame ID -  - Instrument Code from spacit run on CK
       *  
       * @return @b int The appropriate instrument code for the "Camera-matrix" 
       *         Kernel Frame ID
       */
      virtual int CkFrameId() const { return (-82000); }

      /** 
       * CK Reference ID - J2000
       * 
       * @return @b int The appropriate instrument code for the "Camera-matrix"
       *         Kernel Reference ID
       */
      virtual int CkReferenceId() const { return (1); }

      /** 
       *  SPK Center ID - 6 (Saturn)
       *  
       * @return @b int The appropriate instrument code for the Spacecraft 
       *         Kernel Center ID
       */
      virtual int SpkCenterId() const { return 6; }

      /** 
       *  SPK Reference ID - J2000
       *  
       * @return @b int The appropriate instrument code for the Spacecraft 
       *         Kernel Reference ID
       */
      virtual int SpkReferenceId() const { return (1); }

      virtual QList<QPointF> PixelIfovOffsets();
      
      virtual QString instrumentNameLong() const;
      virtual QString instrumentNameShort() const;
      virtual QString spacecraftNameLong() const;
      virtual QString spacecraftNameShort() const;

    private:
      double m_pixelPitchX;
      double m_pixelPitchY;
      
      QString m_instrumentNameLong; //!< Full instrument name
      QString m_instrumentNameShort; //!< Shortened instrument name
      QString m_spacecraftNameLong; //!< Full spacecraft name
      QString m_spacecraftNameShort; //!< Shortened spacecraft name
  };
};
#endif
