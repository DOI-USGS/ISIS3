#ifndef Stereo_h
#define Stereo_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Camera.h"

namespace Isis {

  /**
   * @brief Provide stereo information/data for a point or relationship
   *
   * @author  2009-09-11 Kris Becker
   *
   * @internal 
   *   @history 2012-02-28 Tracie Sucharski - Moved from the app, smtk,
   *                          directory, to the base/objs directory path.
   *                          Updated to new coding standards, added unitTest.
   *   @history 2012-07-06 Debbie A. Cook, Updated Spice members to be more compliant with Isis 
   *                          coding standards. References #972.
   *   @history 2012-12-20 Debbie A. Cook - Changed to use TProjection instead of Projection.  
   *                          References #775.
   */
  class Stereo  {
    public:

      /**
       * @brief Construct a Stereo object
       */
      Stereo () { }

      /** Destructor for Stereo */
      virtual ~Stereo() {}

      static bool elevation(Camera &cam1, Camera &cam2, double &radius,
                            double &latitude, double &longitude,
                            double &sepang, double &error);

      static void spherical(const double latitude, const double longitude,
                            const double radius, double &x, double &y,
                            double &z);

      static void rectangular(const double x, const double y, const double z,
                              double &latitude, double &longitude,
                              double &radius);

  private:
    static std::vector<double> array2StdVec(const double d[3]);
    static double *stdVec2Array(const std::vector<double> &v, double *d = 0);
    static void targetToSpacecraft(Camera &camera, double TP[3]);
    static void targetToSurface(Camera &camera, double TC[3]);

  };
};

#endif
