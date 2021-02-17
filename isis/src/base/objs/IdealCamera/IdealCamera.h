#ifndef IdealCamera_h
#define IdealCamera_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"

#include <QString>

namespace Isis {
  /**
   * @brief Ideal Camera Model
   *
   * This class is the implementation of a generic camera model
   * with no optical distortion.
   *
   * @ingroup SpiceInstrumentsAndCameras
   *
   * @author  2006-10-17 Jeff Anderson and Debbie Cook
   *
   * @internal
   *   @history 2007-10-18 Debbie A. Cook  -Corrected coding error with TransS0
   *                           and TransL0 and added ldir and sdir.
   *   @history 2008-06-18 Stuart Sides - Fixed doc error
   *   @history 2008-08-08 Steven Lambright - Made the unit test work with a
   *                           Sensor change. Also, now using the new
   *                           LoadCache(...) method instead of CreateCache(...).
   *   @history 2009-08-28 Steven Lambright - Changed inheritance to no longer
   *                           inherit directly from Camera
   *   @history 2010-09-16 Jeannie Walldren - Modified test cube to run properly
   *                           with ShapeModel changes to Sensor class and
   *                           updated unitTest known lat/lon values.
   *   @history 2011-05-03 Jeannie Walldren - Updated unitTest to test for new
   *                           methods. Updated documentation. Added Isis
   *                           Disclaimer to files. Added NAIF error check to
   *                           constructor.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                           coding standards. References #972.
   *   @history 2012-09-10 Steven Lambright - Added support for reading ideal information from
   *                           the NaifKeywords object (via readValue()). This camera now
   *                           works in 3 ways: detached spice (ideal keywords, e.g. TransX0, must
   *                           be in the instrument group), legacy attached spice (ideal keywords
   *                           are in the instrument group, but spice is attached so kernels will
   *                           not be furnished), and attached spice (ideal keywords are already in
   *                           the NaifKeywords object). The first method (detached spice) works by
   *                           pushing the ideal keywords into the naif kernel pool. The second
   *                           method (legacy attached spice) works by pushing the keywords on the
   *                           fly into the NaifKeywords object (kernels won't need to be
   *                           furnished). The third method works automatically. References #1094.
   *  @history 2015-08-25 Ian Humphrey and Makayla Shepherd - Added new data members and methods
   *                           to get spacecraft and instrument names. Extended unit test to test
   *                           these methods.
   */
  class IdealCamera : public Camera {
    public:
      // Constructs a HiriseCamera object
      IdealCamera(Cube &cube);

      // Destroys the HiriseCamera object
      ~IdealCamera();

      /**
       * Returns the type of camera that was created.
       *
       * @return CameraType
       */
      virtual CameraType GetCameraType() const {
        if(p_framing) return Framing;
        return LineScan;
      }

      virtual int CkFrameId() const;
      virtual int CkReferenceId() const;
      virtual int SpkTargetId() const;
      virtual int SpkCenterId() const;
      virtual int SpkReferenceId() const;
      virtual QString instrumentNameLong() const;
      virtual QString instrumentNameShort() const;
      virtual QString spacecraftNameLong() const;
      virtual QString spacecraftNameShort() const;

    private:
      bool p_framing; //!< true if framing camera
      QString m_instrumentNameLong; //!< Full instrument name
      QString m_instrumentNameShort; //!< Shortened instrument name
      QString m_spacecraftNameLong; //!< Full spacecraft name
      QString m_spacecraftNameShort; //!< Shortened spacecraft name
  };
};

#endif
