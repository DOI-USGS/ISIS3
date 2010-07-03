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
   *   @history 2010-05-06 Steven Lambright Added Split Lat/Lon
   *            Functionality
   *   @history 2010-06-22 Steven Lambright Improved handling of resolutions 
   */
  class GroundGrid {
    public:
      GroundGrid(UniversalGroundMap *gmap, bool splitLatLon,
                 unsigned int width, unsigned int height);

      virtual ~GroundGrid();

      void CreateGrid(double baseLat, double baseLon,
                      double latInc,  double lonInc,
                      Progress *progress = 0,
                      double latRes = 0.0, double lonRes = 0.0);

      bool PixelOnGrid(int x, int y);
      bool PixelOnGrid(int x, int y, bool latGrid);

    protected:
      virtual bool GetXY(double lat, double lon,
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

      double p_minLat; //!< Lowest latitude in image
      double p_minLon; //!< Lowest longitude in image
      double p_maxLat; //!< Highest latitude in image
      double p_maxLon; //!< Highest longitude in image

      double p_defaultResolution; //!< Default step size in degrees/pixel

      bool p_reinitialize; //!< True if we need to reset p_grid in CreateGrid
  };

}


#endif
