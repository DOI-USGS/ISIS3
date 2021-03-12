#ifndef GroundGrid_h
#define GroundGrid_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

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
   *   @history 2010-05-06 Steven Lambright - Added Split Lat/Lon functionality
   *   @history 2010-06-22 Steven Lambright - Improved handling of resolutions
   *   @history 2011-01-25 Steven Lambright - Now uses native units to the projection,
   *                           Lat/Lon classes, and several bug fixes when it comes to
   *                           out of range values or non-standard projection types.
   *   @history 2011-01-26 Steven Lambright - Fixed a bug where the grid was not consistent
   *                           on the edges and added SetGroundLimits and WalkBoundary
   *                           for the new grid options Bound lat/lon range.
   *   @history 2011-02-25 Steven Lambright - Min/Max Lat/Lons do not have to be known
   *                           in the constructor any more
   *   @history 2011-12-08 Steven Lambright - Fixed a bug causing the longitude range
   *                           to be incorrect. Fixes #607.
   *   @history 2014-06-06 Kristin Berry - Fixed a bug where lat/lon were swapped in the code.
   *                           Fixes #2081.
   *   @history 2016-09-29 Jeannie Backer - Changed Latitude objects that were created with
   *                           Latitude's mapping group constructor to have enum value
   *                           Latitude::AllowPastPole. This was done for p_minLat in the
   *                           GroundGrid constructor and for startLat in the CreateGrid()
   *                           method. The reason for this is that these objects are copied
   *                           to initialize another Latitude variable (lat) within for-loops
   *                           that go past the poles. Thus lat must be allowed to go past
   *                           the poles. This bug was uncovered when Latitude's virtual setAngle()
   *                           method was fixed to match the signature of the parent method.
   *                           Moved implementation of GroundMap() and GetMappingGroup() to the
   *                           cpp file per ISIS coding standards.
   *   @history 2017-04-10 Jesse Mapel - Modified to not throw an exception when calculating
   *                           longitude lines close to 90 or -90 latitude. Fixes #4766.
   *   @history 2017-06-28 Jesse Mapel - Added a flag to extend the grid past the longitude
   *                           domain boundary. Fixes #2185.
   */
  class GroundGrid {
    public:
      GroundGrid(UniversalGroundMap *gmap, 
                 bool splitLatLon,
                 bool extendGrid,
                 unsigned int width, 
                 unsigned int height);

      virtual ~GroundGrid();

      void CreateGrid(Latitude baseLat, 
                      Longitude baseLon,
                      Angle latInc,  
                      Angle lonInc,
                      Progress *progress = 0);

      void CreateGrid(Latitude baseLat, 
                      Longitude baseLon,
                      Angle latInc,  
                      Angle lonInc,
                      Progress *progress,
                      Angle latRes, 
                      Angle lonRes);

      void WalkBoundary();

      void SetGroundLimits(Latitude minLat, 
                           Longitude minLon, 
                           Latitude maxLat,
                           Longitude maxLon);

      bool PixelOnGrid(int x, int y);
      bool PixelOnGrid(int x, int y, bool latGrid);

      Latitude minLatitude() const;
      Longitude minLongitude() const;
      Latitude maxLatitude() const;
      Longitude maxLongitude() const;

      PvlGroup *GetMappingGroup();

    protected:
      virtual bool GetXY(Latitude lat, Longitude lon,
                         unsigned int &x, unsigned int &y);

      UniversalGroundMap *GroundMap();

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
      bool m_extendGrid; //!< If the grid should extend past the longitude domain boundary.
  };

}


#endif
