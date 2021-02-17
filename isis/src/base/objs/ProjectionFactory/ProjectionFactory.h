#ifndef ProjectionFactory_h
#define ProjectionFactory_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Plugin.h"
#include "WorldMapper.h"

namespace Isis {
  class Camera;
  class Cube;
  class Projection;
  class RingPlaneProjection;

  /**
   * @brief Initialize a map projection
   *
   * This factory class is used to obtain a Projection object given a PVL which
   * contains a valid Mapping group. The Mapping group can come from an
   * image/cube or a hand-created PVL file. The projection is loaded based on
   * information using the ProjectionName contained in the Mapping group. It is
   * plugin oriented. That is, this class looks in
   * $ISISROOT/lib/Projection.plugin to convert the ProjectionName into a
   * pointer to the appropriate projection class (e.g., Sinusoidal,
   * SimpleCylindrical). This allows programmers who develop new projection to
   * create a plugin without the need for recompiling all the Isis cartographic
   * programs (cam2map, map2map, mappos, etc).
   *
   * @ingroup MapProjection
   *
   * @author 2005-06-22 Elizabeth Ribelin
   *
   *
   * @internal
   *   @history 2005-06-22 Elizabeth Ribelin - Merged ProjectionManager and
   *                         CubeProjection into a single class
   *   @history 2006-01-27 Jacob Danton - Renamed PixelMapper to PFPixelMapper
   *   @history 2006-05-19 Elizabeth Miller - Depricated ProjectionManager and
   *                         CubeProjection.  Renamed ProjectionManager.plugin
   *                         to Projection.plugin
   *   @history 2006-09-07 Elizabeth Miller - Added the bool sizeMatch
   *                         parameter to CreateForCube()
   *   @history 2007-03-13 Jeff Anderson - Added new method CreateForCube using
   *                         a camera
   *   @history 2007-06-29 Steven Lambright - Removed TrueScaleLatitude keyword
   *                         from CreateForCube methods, added units to Scale
   *                         and PixelResolution keywords
   *   @history 2008-06-18 Steven Koechle - Fixed Documentation Error
   *   @history 2009-06-18 Jeff Anderson - Modified the CreateForCube method to
   *                         make sure extra pixels were not included in the 
   *                         image size due to machine precision roundoff
   *                         problems.
   *   @history 2011-02-10 Jai Rideout - UpperLeftCornerX and UpperLeftCornerY
   *                         are now set in projection mapping group via the new
   *                         SetUpperLeftCorner method in Projection.
   *   @history 2011-05-23 Jannet Barrett and Steven Lambright -
   *                         Added m_projPlugin to reduce cost of instantiating
   *                         Projections.
   *   @history 2013-03-13 Debbie A. Cook - Modified to differentiate between
   *                         Project subclasses:  TProjection and RingPlaneProjection.
   *   @history 2013-05-09 Jeannie Backer - Modified CreateForCube and RingsCreateForCube() methods
   *                           to ensure that the distances from the MinimumX to the MaximumX and
   *                           from the MinimumY to the MaximumY are at least one pixel resolution.
   *                           This ensures that the created cube will have at least one sample and
   *                           at least one line. References #775.
   *  @history 2014-01-16 Kimberly Oyama - Updated the error messages for unsupported projections
   *                          to include the source of the error. Fixes #988.
   *  @history 2016-09-02 Tyler Wilson - Fixed a bug in the CreateForCube function because it was
   *  producing an unequal number of lines/samples for maps from projections that are symmetric about 
   *  the prime meridian or the equator.  Fixes #2245.
   */
  class ProjectionFactory {
    public:
      static Isis::Projection *Create(Isis::Pvl &label, bool allowDefaults = false);
      static Isis::Projection *RingsCreate(Isis::Pvl &label, bool allowDefaults = false);
      static Isis::Projection *CreateFromCube(Isis::Cube &cube);
      static Isis::Projection *RingsCreateFromCube(Isis::Cube &cube);
      static Isis::Projection *CreateFromCube(Isis::Pvl &label); // Load Method in cm
      static Isis::Projection *RingsCreateFromCube(Isis::Pvl &label); // Load Method in cm
      static Isis::Projection *CreateForCube(Isis::Pvl &label, int &ns, int &nl,
                                             bool sizeMatch = true); // Create method in cm
      static Isis::Projection *RingsCreateForCube(Isis::Pvl &label,
                                                   int &samples, int &lines,
                                                   bool sizeMatch);
      static Isis::Projection *CreateForCube(Isis::Pvl &label,
                                             int &samples, int &lines,
                                             Camera &cam);
      static Isis::Projection *RingsCreateForCube(Isis::Pvl &label,
                                                   int &samples, int &lines,
                                                   Camera &cam);


    private:
      /**
       * Constructor (Its private, so you cannot use it.)  Use the Create method
       * instead
       */
      ProjectionFactory() {};

      //! Destroys the ProjectionFactory object
      ~ProjectionFactory() {};
      
      static Plugin m_projPlugin;
  };


  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class PFPixelMapper : public Isis::WorldMapper {
    public:
      /**
       * Constructs a PFFixelMapper object with the given pixel resolution and
       * location.
       * @param pixelResolution The pixel resolution
       * @param upperLeftX The x value for the upper left corner
       * @param upperLeftY The y value for the upper left corner
       */
      PFPixelMapper(double pixelResolution, double upperLeftX, double upperLeftY) {
        p_pixelResolution = pixelResolution;
        p_upperLeftX = upperLeftX;
        p_upperLeftY = upperLeftY;
      };

      /**
       * Returns the world x position for the given x projection value
       * @param projX The x projection value to get the world x position for
       *
       * @return double The world x position
       */
      double WorldX(const double projX) const {
        return (projX - p_upperLeftX) / p_pixelResolution + 0.5;
      };

      /**
       * Returns the world y position for the given y projection value
       * @param projY The y projection value to get the world y position for
       *
       * @return double The world y position
       */
      double WorldY(const double projY) const {
        return (p_upperLeftY - projY) / p_pixelResolution + 0.5;
      };

      /**
       * Returns the x projection of the given sample
       * @param sample The sample to get the x projection value for
       *
       * @return double The x projection value
       */
      double ProjectionX(const double sample) const {
        return (sample - 0.5) * p_pixelResolution + p_upperLeftX;
      };

      /**
       * Returns the y projection of the given line
       * @param line The line to get the y projection value for
       *
       * @return double The y projection value
       */
      double ProjectionY(const double line) const {
        return p_upperLeftY - (line - 0.5) * p_pixelResolution;
      };

      //! Returns the pixel resolution
      double Resolution() const {
        return p_pixelResolution;
      }

    private:
      double p_pixelResolution;
      double p_upperLeftX;
      double p_upperLeftY;
  };
  /// @endcond
};

#endif

