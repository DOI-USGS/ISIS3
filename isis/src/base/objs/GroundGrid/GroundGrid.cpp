#include "GroundGrid.h"

#include <cmath>

#include "ProjectionFactory.h"
#include "UniversalGroundMap.h"
#include "Camera.h"
#include "Projection.h"
#include "Progress.h"

using namespace std;

namespace Isis {
  /**
   * This method initializes the class by allocating the grid,
   * calculating the lat/lon range, and getting a default grid
   * resolution.
   *
   * @param gmap A universal ground map to use for calculating the grid
   * @param splitLatLon Make two grids: one for latitude lines and
   *                    one for longitude lines
   * @param width The width of the grid; often cube samples
   * @param height The height of the grid; often cube samples
   */
  GroundGrid::GroundGrid(UniversalGroundMap *gmap, bool splitLatLon,
                         unsigned int width, unsigned int height) {
    p_width = width;
    p_height = height;

    // Initialize our grid pointer to null
    p_grid = 0;
    p_latLinesGrid = 0;
    p_lonLinesGrid = 0;

    // Now let's figure out how big the grid needs to be, then allocate and
    //   initialize
    p_gridSize = (unsigned long)(ceil(width * height / 8.0) + 0.5);

    if(!splitLatLon) {
      p_grid = new char[p_gridSize];
    }
    else {
      p_latLinesGrid = new char[p_gridSize];
      p_lonLinesGrid = new char[p_gridSize];
    }

    for(unsigned long i = 0; i < p_gridSize; i++) {
      if(p_grid) p_grid[i] = 0;
      if(p_latLinesGrid) p_latLinesGrid[i] = 0;
      if(p_lonLinesGrid) p_lonLinesGrid[i] = 0;
    }

    // The first call of CreateGrid doesn't have to reinitialize
    p_reinitialize = false;

    p_groundMap = gmap;

    // We need a lat/lon range for gridding, use the mapping group
    //  (in case of camera, use BasicMapping)
    p_minLat = 0.0;
    p_minLon = 0.0;
    p_maxLat = 0.0;
    p_maxLon = 0.0;

    PvlGroup mapping;

    if(p_groundMap->Camera()) {
      Pvl tmp;
      p_groundMap->Camera()->BasicMapping(tmp);
      mapping = tmp.FindGroup("Mapping");
    }
    else {
      mapping = p_groundMap->Projection()->Mapping();
    }

    p_minLat = mapping["MinimumLatitude"];
    p_maxLat = mapping["MaximumLatitude"];
    p_minLon = mapping["MinimumLongitude"];
    p_maxLon = mapping["MaximumLongitude"];

    double radius1 = mapping["EquatorialRadius"];
    double radius2 = mapping["PolarRadius"];

    double largerRadius = max(radius1, radius2);

    // p_defaultResolution is in degrees/pixel

    if(p_groundMap->HasCamera()) {
      p_defaultResolution =
        (p_groundMap->Camera()->HighestImageResolution() / largerRadius) * 10;
    }
    else {
      p_defaultResolution = (p_groundMap->Resolution() / largerRadius) * 10;
    }

    if(p_defaultResolution < 0) {
      p_defaultResolution = 10.0 / largerRadius;
    }
  }

  /**
   * Delete the object
   */
  GroundGrid::~GroundGrid() {
    if(p_grid) {
      delete [] p_grid;
      p_grid = NULL;
    }

    if(p_latLinesGrid) {
      delete [] p_latLinesGrid;
      p_latLinesGrid = NULL;
    }

    if(p_lonLinesGrid) {
      delete [] p_lonLinesGrid;
      p_lonLinesGrid = NULL;
    }
  }


  /**
   * This method draws the grid internally. It is not valid to call PixelOnGrid
   * until this method has been called.
   *
   * @param baseLat Latitude to hit in the grid
   * @param baseLon Longitude to hit in the grid
   * @param latInc  Distance between latitude lines
   * @param lonInc  Distance between longitude lines
   * @param progress If passed in, this progress will be used
   * @param latRes  Resolution of latitude lines (in degrees/pixel)
   * @param lonRes  Resolution of longitude lines (in degrees/pixel)
   */
  void GroundGrid::CreateGrid(double baseLat, double baseLon,
                              double latInc,  double lonInc,
                              Progress *progress,
                              double latRes, double lonRes) {
    if(p_groundMap == NULL ||
        (p_grid == NULL && p_latLinesGrid == NULL && p_lonLinesGrid == NULL)) {
      iString msg = "GroundGrid::CreateGrid missing ground map or grid array";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if(p_reinitialize) {
      for(unsigned long i = 0; i < p_gridSize; i++) {
        if(p_grid) p_grid[i] = 0;
        if(p_latLinesGrid) p_latLinesGrid[i] = 0;
        if(p_lonLinesGrid) p_lonLinesGrid[i] = 0;
      }
    }

    // subsequent calls to this method must always reinitialize the grid
    p_reinitialize = true;

    // Find starting points for lat/lon
    double startLat = baseLat - floor((baseLat - p_minLat) / latInc) * latInc;
    double startLon = baseLon - floor((baseLat - p_minLon) / lonInc) * lonInc;

    if(latRes == 0.0) {
      latRes = p_defaultResolution;
    }

    if(lonRes == 0.0) {
      lonRes = p_defaultResolution;
    }

    double endLat = (long)((p_maxLat - startLat) / latInc) * latInc + startLat;
    double endLon = (long)((p_maxLon - startLon) / lonInc) * lonInc + startLon;

    if(progress) {
      double numSteps = (endLat - startLat) / latInc + 1;
      numSteps += (endLon - startLon) / lonInc + 1;

      if(numSteps <= 0) {
        iString msg = "No gridlines would intersect the image";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }

      progress->SetMaximumSteps((long)(numSteps + 0.5));
      progress->CheckStatus();
    }

    for(double lat = startLat; lat <= endLat; lat += latInc) {
      unsigned int previousX = 0;
      unsigned int previousY = 0;
      bool havePrevious = false;

      for(double lon = startLon; lon <= p_maxLon; lon += latRes) {
        unsigned int x = 0;
        unsigned int y = 0;
        bool valid = GetXY(lat, lon, x, y);

        if(valid && havePrevious) {
          if(previousX == x && previousY == y) {
            continue;
          }

          DrawLineOnGrid(previousX, previousY, x, y, true);
        }

        havePrevious = valid;
        previousX = x;
        previousY = y;
      }

      if(progress) {
        progress->CheckStatus();
      }
    }

    for(double lon = startLon; lon <= endLon; lon += lonInc) {
      unsigned int previousX = 0;
      unsigned int previousY = 0;
      bool havePrevious = false;

      for(double lat = startLat; lat <= p_maxLat; lat += lonRes) {
        unsigned int x = 0;
        unsigned int y = 0;

        bool valid = GetXY(lat, lon, x, y);

        if(valid && havePrevious) {
          if(previousX == x && previousY == y) {
            continue;
          }

          DrawLineOnGrid(previousX, previousY, x, y, false);
        }

        havePrevious = valid;
        previousX = x;
        previousY = y;
      }

      if(progress) {
        progress->CheckStatus();
      }
    }
  }


  /**
   * Returns true if the grid is on this point. Using this method
   * is recommended if lat/lon grids are separate.
   *
   * @param x X-Coordinate of grid (0-based)
   * @param y Y-Coordinate of grid (0-based)
   * @param latGrid True for latitude lines, false for longitude
   *                lines
   *
   * @return bool Pixel lies on grid
   */
  bool GroundGrid::PixelOnGrid(int x, int y, bool latGrid) {
    if(x < 0) return false;
    if(y < 0) return false;

    if(x >= (int)p_width) return false;
    if(y >= (int)p_height) return false;

    return GetGridBit(x, y, latGrid);
  }


  /**
   * Returns true if the grid is on this point.
   *
   * @param x X-Coordinate of grid (0-based)
   * @param y Y-Coordinate of grid (0-based)
   *
   * @return bool Pixel lies on grid
   */
  bool GroundGrid::PixelOnGrid(int x, int y) {
    if(x < 0) return false;
    if(y < 0) return false;

    if(x >= (int)p_width) return false;
    if(y >= (int)p_height) return false;

    return GetGridBit(x, y, true); // third argument shouldnt matter
  }


  /**
   * This method converts a lat/lon to an X/Y. This implementation converts to
   * sample/line.
   *
   * @param lat Latitude of Lat/Lon pair to convert to S/L
   * @param lon Longitude of Lat/Lon pair to convert to S/L
   * @param x   Output sample (0-based)
   * @param y   Output line (0-based)
   *
   * @return bool Successful
   */
  bool GroundGrid::GetXY(double lat, double lon,
                         unsigned int &x, unsigned int &y) {
    if(!GroundMap()) return false;

    if(!GroundMap()->SetUniversalGround(lat, lon)) return false;
    x = (int)(p_groundMap->Sample() - 0.5);
    y = (int)(p_groundMap->Line() - 0.5);

    return true;
  }


  /**
   * This flags a bit as on the grid lines.
   *
   * @param x X-Coordinate (0-based)
   * @param y Y-Coordinate (0-based)
   * @param latGrid True if this is derived from a latitude line
   */
  void GroundGrid::SetGridBit(unsigned int x, unsigned int y, bool latGrid) {
    unsigned long bitPosition = (y * p_width) + x;
    unsigned long byteContainer = bitPosition / 8;
    unsigned int bitOffset = bitPosition % 8;

    if(byteContainer < 0 || byteContainer > p_gridSize) return;

    if(p_grid) {
      char &importantByte = p_grid[byteContainer];
      importantByte |= (1 << bitOffset);
    }
    else if(latGrid && p_latLinesGrid) {
      char &importantByte = p_latLinesGrid[byteContainer];
      importantByte |= (1 << bitOffset);
    }
    else if(!latGrid && p_lonLinesGrid) {
      char &importantByte = p_lonLinesGrid[byteContainer];
      importantByte |= (1 << bitOffset);
    }
    else {
      iString msg = "GroundGrid::SetGridBit no grids available";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }
  }


  /**
   * Returns true if the specified coordinate is on the grid lines.
   *
   * @param x X-Coordinate (0-based)
   * @param y Y-Coordinate (0-based)
   *
   * @return bool Value at grid coordinate
   */
  bool GroundGrid::GetGridBit(unsigned int x, unsigned int y, bool latGrid) {
    unsigned long bitPosition = (y * p_width) + x;
    unsigned long byteContainer = bitPosition / 8;
    unsigned int bitOffset = bitPosition % 8;

    if(byteContainer < 0 || byteContainer > p_gridSize) return false;

    bool result = false;

    if(p_grid) {
      char &importantByte = p_grid[byteContainer];
      result = (importantByte >> bitOffset) & 1;
    }
    else if(latGrid && p_latLinesGrid) {
      char &importantByte = p_latLinesGrid[byteContainer];
      result = (importantByte >> bitOffset) & 1;
    }
    else if(!latGrid && p_lonLinesGrid) {
      char &importantByte = p_lonLinesGrid[byteContainer];
      result = (importantByte >> bitOffset) & 1;
    }
    else {
      iString msg = "GroundGrid::GetGridBit no grids available";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    return result;
  }


  /**
   * This sets the bits on the grid along the specified line.
   *
   * @param x1 Start X
   * @param y1 Start Y
   * @param x2 End X
   * @param y2 End Y
   * @param isLatLine
   */
  void GroundGrid::DrawLineOnGrid(unsigned int x1, unsigned int y1,
                                  unsigned int x2, unsigned int y2,
                                  bool isLatLine) {
    int dx = x2 - x1;
    int dy = y2 - y1;

    SetGridBit(x1, y1, isLatLine);

    if(dx != 0) {
      double m = (double)dy / (double)dx;
      double b = y1 - m * x1;

      dx = (x2 > x1) ? 1 : -1;
      while(x1 != x2) {
        x1 += dx;
        y1 = (int)(m * x1 + b + 0.5);
        SetGridBit(x1, y1, isLatLine);
      }
    }
  }
};
