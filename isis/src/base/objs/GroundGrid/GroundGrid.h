#ifndef GroundGrid_h
#define GroundGrid_h
/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2010/06/22 23:57:57 $
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

namespace Isis {
  class Angle;
  class Latitude;
  class Longitude;
  class PvlGroup;
  class Progress;
  class UniversalGroundMap;

  /**
   * @brief Calculates a lat/lon grid over an area
   *
   * This class, given a universal ground map, area width and height,
   *   base lat/lon, lat/lon increments, and optionalls a progress and
   *   resolutions will calculate where grid lines should lie.
   *
   * @author 2010-01-06 Steven Lambright
   *
   * @internal
   *   @history 2010-05-06 Steven Lambright - Added Split Lat/Lon
   *                           Functionality
   *   @history 2010-06-22 Steven Lambright - Improved handling of resolutions
   *   @history 2011-01-25 Steven Lambright - Now uses native units to the
   *                           projection, Lat/Lon classes, and several bug
   *                           fixes when it comes to out of range values or
   *                           non-standard projection types.
   *   @history 2011-01-26 Steven Lambright - Fixed a bug where the grid was not
   *                           consistent on the edges and added SetGroundLimits
   *                           and WalkBoundary for the new grid options Bound
   *                           lat/lon range.
   *   @history 2011-02-25 Steven Lambright - Min/Max Lat/Lons do not have
   *                           to be known in the constructor any more
   *   @history 2011-12-08 Steven Lambright - Fixed a bug causing the longitude
   *                           range to be incorrect. Fixes #607.
   *   @history 2014-06-06 Kristin Berry - Fixed a bug where lat/lon were swapped
   *                           in the code. Fixes #2081.
   *          
   */
  class GroundGrid {
    public:
      GroundGrid(UniversalGroundMap *gmap, bool splitLatLon,
                 unsigned int width, unsigned int height);

      virtual ~GroundGrid();

      void CreateGrid(Latitude baseLat, Longitude baseLon,
                      Angle latInc,  Angle lonInc,
                      Progress *progress = 0);

      void CreateGrid(Latitude baseLat, Longitude baseLon,
                      Angle latInc,  Angle lonInc,
                      Progress *progress,
                      Angle latRes, Angle lonRes);

      void WalkBoundary();

      void SetGroundLimits(Latitude minLat, Longitude minLon, Latitude maxLat,
                           Longitude maxLon);

      bool PixelOnGrid(int x, int y);
      bool PixelOnGrid(int x, int y, bool latGrid);

      /**
       * Returns a mapping group representation of the projection or camera.
       * This is useful for matching units with lat/lons.
       *
       * @returns Returns a mapping group representation of the projection or camera
       */
      PvlGroup *GetMappingGroup() { return p_mapping; }

    protected:
      virtual bool GetXY(Latitude lat, Longitude lon,
                         unsigned int &x, unsigned int &y);

      //! Returns the ground map for children
      UniversalGroundMap *GroundMap() {
        return p_groundMap;
      }

    private:
      void SetGridBit(unsigned int x, unsigned int y, bool latGrid);
      bool GetGridBit(unsigned int x, unsigned int y, bool latGrid);
      void DrawLineOnGrid(unsigned int x1, unsigned int y1,
                          unsigned int x2, unsigned int y2,
                          bool isLatLine);


      char *p_grid; //!< This stores the bits of each pixel in the grid
      char *p_latLinesGrid; //!< This stores the bits of each pixel in the grid
      char *p_lonLinesGrid; //!< This stores the bits of each pixel in the grid
      unsigned int p_width; //!< This is the width of the grid
      unsigned int p_height; //!< This is the height of the grid
      unsigned long p_gridSize; //!< This is width*height
      UniversalGroundMap *p_groundMap; //!< This calculates single grid pts

      Latitude  *p_minLat; //!< Lowest latitude in image
      Longitude *p_minLon; //!< Lowest longitude in image
      Latitude  *p_maxLat; //!< Highest latitude in image
      Longitude *p_maxLon; //!< Highest longitude in image

      //! The mapping group representation of the projection or camera
      PvlGroup *p_mapping;

      double p_defaultResolution; //!< Default step size in degrees/pixel

      bool p_reinitialize; //!< True if we need to reset p_grid in CreateGrid
  };

}


#endif
