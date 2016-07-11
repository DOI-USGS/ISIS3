/**
 * @file
 * $Revision: 1.5 $
 * $Date: 2010/06/07 22:42:38 $
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
#ifndef CameraPointInfo_h
#define CameraPointInfo_h

#include <QString>

namespace Isis {
  class Camera;
  class CubeManager;
  class Cube;
  class PvlGroup;


  /**
   * @brief CameraPointInfo provides quick access to the majority of information avaliable from a 
   *        camera on a point.
   *
   * CameraPointInfo provides the functionality which was a part of campt in class form. This 
   * functionality is access to the majoirty of information avaliable on any given point on an
   * image. The main difference is the use of a CubeManager within CameraPointInfo for effeciency 
   * when working with control nets and the opening of cubes several times.
   *
   * @author 2009-08-25 Mackenzie Boyd
   *
   * @internal
   *   @history 2009-09-13 Mackenzie Boyd - Added methods SetCenter(), SetSample() and SetLine() to
   *                           support campt functionality. Added CheckCube() private method to
   *                           check m_currentCube isn't NULL.
   *   @history 2010-03-25 Mackenzie Boyd - Modified longitude output to have Positive East and
   *                           West, 360 and 180 longitudes.
   *   @history 2010-05-25 Mackenzie Boyd - Many changes, primary changes had to do with how errors
   *                           are handled. Depending on the options sent in, errors can be handled
   *                           by putting an Error keyword into the PvlGroup instead of throwing an
   *                           exception. Other changes, addition of two booleans, both defaulting
   *                           to false, to the Set methods (excluding SetCube) so that allowoutside
   *                           option and allowerrors option could be taken in instead of using
   *                           setters. CheckConditions method was removed and placed within
   *                           GetPointInfo, GetPointInfo had 3 boolean parameters added,
   *                           passed - whether or not the SetImage or SetGround done above was
   *                           successful, allowoutside - if locations outside the cube are
   *                           acceptable, and allowerrors - what to do with errors.
   *   @history 2010-06-07 Mackenzie Boyd - Changed Error keyword so that it is always present when
   *                           allowErrors is true.
   *   @history 2010-09-13 Steven Lambright - Corrected units for SampleResolution and
   *                           LineResolution
   *   @history 2012-07-06 Debbie A. Cook - Updated Spice members to be more compliant with Isis
   *                           coding standards. References #972.
   *   @history 2013-03-27 Jeannie Backer - Added comment in code. References #1248.
   *   @history 2012-12-20 Debbie A. Cook - Changed to use TProjection.  References #775.
   *   @history 2013-03-16 Jeannie Backer - Added accessor methods camera() and cube(). Added
   *                           m_ prefix to member variables. Made GetPointInfo() virtual so it can
   *                           be redefined in child classes. References #775.
   *   @history 2014-04-17 Jeannie Backer - Added check for valid azimuth values.  If not, print
   *                           "NULL" to be consistent with caminfo's CamTools.cpp. Replaced local
   *                           variable names with more descriptive names. References #1659.
   *   @history 2015-10-01 Jeannie Backer - Made improvements to documentation and brought code
   *                           closer to ISIS coding standards. References #1438
   *   @history 2016-07-11 Curtis Rose - Added units to a few of the outputs. References #3979.
   */
  class CameraPointInfo {

    public:
      CameraPointInfo();
      virtual ~CameraPointInfo();

      void SetCube(const QString &cubeFileName);
      PvlGroup *SetImage(const double sample, const double line,
                         const bool outside = false, const bool error = false);
      PvlGroup *SetCenter(const bool outside = false, const bool error = false);
      PvlGroup *SetSample(const double sample, const bool outside = false,
                          const bool error = false);
      PvlGroup *SetLine(const double line, const bool outside = false,
                        const bool error = false);
      PvlGroup *SetGround(const double latitude, const double longitude,
                          const bool outside = false, const bool error = false);

    protected:
      Camera *camera();
      Cube *cube();

    private:
      bool CheckCube();
      virtual PvlGroup *GetPointInfo(bool passed, bool outside, bool errors);
      CubeManager *m_usedCubes;
      Cube *m_currentCube;
      Camera *m_camera;
  };
};

#endif
