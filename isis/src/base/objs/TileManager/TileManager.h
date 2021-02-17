#ifndef TileManager_h
#define TileManager_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "BufferManager.h"
#include "Cube.h"

namespace Isis {
  /**
   * @brief Buffer manager, for moving through a cube in tiles
   *
   * This class is used as a manager for moving through a cube one tile at a time.
   * A tile is defined as a two dimensional (n samples by m lines) sub area of a
   * cube. The band direction is always one deep. The sequence of tiles starts
   * with the tile containing sample one, line one and band one. It then moves
   * across the cube in the sample direction then to the next tile in the line
   * direction and finally to the next tile in the band direction.
   *
   * If you would like to see TileManager being used in implementation,
   * see the ProcessByTile class
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2002-10-10 Stuart Sides
   *
   * @internal
   *  @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                                     isis.astrogeology...
   *  @history 2005-02-22 Elizabeth Ribelin - Modified file to support Doxygen
   *                                          documentation
   *
   *  @todo 2005-05-23 Jeff Anderson - There could be problems with 2GB files if
   *  the tile size is very small, 1x1, 2x1, 1x2.  Should we worry about this?
   */
  class TileManager : public Isis::BufferManager {

    private:
      int p_numSampTiles; //!<Stores the number of tiles in the sample direction
      int p_numLineTiles; //!<Stores the number of tiles in the line direction

    public:
      //  Constructors and Destructors
      TileManager(const Isis::Cube &cube,
                  const int &bufNumSamples = 128, const int &bufNumLines = 128);

      //! Destroys the TileManager object
      ~TileManager() {};

      bool SetTile(const int Tile, const int band = 1);

      /**
       * Returns the number of Tiles in the cube.
       *
       * @return int
       */
      inline int Tiles() {
        return MaxMaps();
      };
  };
};

#endif


