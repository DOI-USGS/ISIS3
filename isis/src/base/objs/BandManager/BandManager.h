#ifndef BandManager_h
#define BandManager_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "BufferManager.h"
#include "Cube.h"

namespace Isis {
  /**
   * @brief Buffer manager, for moving through a cube in bands
   *
   * This class is used as a manager for moving through a cube one
   * band buffer at a time. A band buffer is defined as a one
   * dimensional sub-area of a cube. That is, the number of
   * bands by 1 sample by 1 line (1,1,nb). The manager moves this
   * (1,1,nb) shape through the cube sequentially accessing all
   * the band buffers in the first sample before proceeding to the
   * second line.
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2007-12-06 Christopher Austin
   *
   * @internal
   *  @history 2008-06-18 Christopher Austin - Fixed documentation errors
   *
   */
  class BandManager : public Isis::BufferManager {

    public:
      // Constructors and Destructors
      BandManager(const Isis::Cube &cube, const bool reverse = false);

      //! Destroys the SampleManager object
      ~BandManager() {};

      bool SetBand(const int sample, const int line = 1);
  };
};

#endif

