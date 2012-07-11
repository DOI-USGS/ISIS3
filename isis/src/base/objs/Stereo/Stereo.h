#ifndef Stereo_h
#define Stereo_h

/**
 * @file
 * $Revision: 1.1 $
 * $Date: 2009/09/09 23:42:41 $
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
