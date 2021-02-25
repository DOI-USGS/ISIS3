#ifndef CubeTileHandler_h
#define CubeTileHandler_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "CubeIoHandler.h"

namespace Isis {

  /**
   * @brief IO Handler for Isis Cubes using the tile format.
   *
   * This class is used to open, create, read, and write data from Isis cube
   * files.
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2003-02-14 Jeff Anderson
   *
   * @internal
   *   @history 2007-09-14 Stuart Sides - Fixed bug where pixels
   *            from a buffer outside the ns/nl were being
   *            transfered to the right most and bottom most tiles
   *   @history 2011-06-16 Jai Rideout and Steven Lambright - Refactored to
   *                           work with the new parent.
   *   @history 2011-07-18 Jai Rideout and Steven Lambright - Added
   *                           unimplemented copy constructor and assignment
   *                           operator.
   */

  class CubeTileHandler : public CubeIoHandler {
    public:
      CubeTileHandler(QFile * dataFile, const QList<int> *virtualBandList,
          const Pvl &label, bool alreadyOnDisk);
      ~CubeTileHandler();

      void updateLabels(Pvl &label);

    protected:
      virtual void readRaw(RawCubeChunk &chunkToFill);
      virtual void writeRaw(const RawCubeChunk &chunkToWrite);

    private:
      /**
       * Disallow copying of this object.
       *
       * @param other The object to copy.
       */
      CubeTileHandler(const CubeTileHandler &other);

      /**
       * Disallow assignments of this object
       *
       * @param other The CubeTileHandler on the right-hand side of the
       *              assignment that we are copying into *this.
       * @return A reference to *this.
       */
      CubeTileHandler &operator=(const CubeTileHandler &other);

      int findGoodSize(int maxSize, int dimensionSize) const;
      BigInt getTileStartByte(const RawCubeChunk &chunk) const;
  };
}

#endif
