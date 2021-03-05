#ifndef ShapeModelFactory_h
#define ShapeModelFactory_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ShapeModel.h"

namespace Isis {
  class Pvl;

  /**
   * This class is used to create ShapeModel objects.  It determines the type
   * of shape model in the input Pvl object and creates the appropriate type of
   * shape model.
   *
   * @author 2010-07-29 Debbie A. Cook
   *
   * @internal
   *   @history 2012-10-31 - Ken Edmundson - Implemented plane shape
   *   @history 2014-01-14 - Jeannie Backer - Improved error message. Fixes #1957.
   *   @history 2015-03-08 - Jeannie Backer - Added implementation for NAIF DSK
   *                             models. References #2035.
   *   @history 2017-05-19 Christopher Combs - Modified unitTest.cpp: added ReportError method to
   *                           truncate paths before data directory. Allows test to pass when not
   *                           using the default data area. Fixes #4738.
   *   @history 2017-06-08 Makayla Shepherd - Added a cube pointer deletion to fix a memory leak.
   *                           Fixes #4890.
   *   @history 2017-03-23 Kris Becker - Added support for Embree and Bullet models.
   *   @history 2017-08-04 Kristin Berry - Removed checks for a 'CubeSupported' IsisPreferences Pvl
   *                           Keyword. ISIS Cube DEMs are not supported by Embree and Bullet
   *                           at this time.
   */
  class ShapeModelFactory {
    public:
    static ShapeModel *create(Target *target, Pvl &pvl);

    private:
      ShapeModelFactory();
      ~ShapeModelFactory();

      // Supported shape models
      enum {
        Ellipsoid,
        Isis3EquatorialCylindrical,
        Isis3Dem,
        Plane,
        Stack,
        NaifDSK,
        Bullet,
        Embree};
  };
}

#endif
