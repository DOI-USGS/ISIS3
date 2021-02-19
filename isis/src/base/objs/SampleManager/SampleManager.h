#ifndef SampleManager_h
#define SampleManager_h

/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "BufferManager.h"
#include "Cube.h"

namespace Isis {
  /**
   * @brief Buffer manager, for moving through a cube in samples
   *
   * This class is used as a manager for moving through a cube one
   * sample buffer at a time. A sample buffer is defined as a one
   * dimensional sub-area of a cube. That is, the number of
   * lines by 1 sample by 1 band (1,nl,1). The manager moves this
   * (1,nl,1) shape through the cube sequentially accessing all
   * the sample buffer in the first band before proceeding to the
   * second band.
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2007-12-06 Christopher Austin
   *
   * @internal
   *  @history 2007-12-06 Christopher Austin Original Version
   *  @history 2008-06-18 Steven Lambright Fixed documentation
   *
   */
  class SampleManager : public Isis::BufferManager {

    public:
      // Constructors and Destructors
      SampleManager(const Isis::Cube &cube, const bool reverse = false);

      //! Destroys the SampleManager object
      ~SampleManager() {};

      bool SetSample(const int sample, const int band = 1);
  };
};

#endif

