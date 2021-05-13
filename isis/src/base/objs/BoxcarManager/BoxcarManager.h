#ifndef BoxcarManager_h
#define BoxcarManager_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "BufferManager.h"
#include "Cube.h"

namespace Isis {
  /**
   * @brief Buffer manager, for moving through a cube by boxcar.
   *
   * This class is used as a manager for moving through a cube one boxcar at
   * a time. A boxcar is defined as two-dimensional (n samples by m lines)
   * sub area of a cube. The band direction is always one deep.
   *
   * The sequence of boxcars starts with the boxcar containing sample one, line
   * one and band one. It then moves across the cube in the sample direction one
   * pixel at a time, then the line direction and finally to the next boxcar in
   * the band direction.
   *
   * The pixel being processed will be indexed into the boxcar buffer as follows:
   * index = (int((boxLines-1)/2) * boxSamples) + int((boxSamples-1)/2),
   * for example, if the boxcar is 5x5, the pixel being processed would be at
   * sample 3, line 3 of the boxcar.  If the boxcar is a 4x4, the pixel being
   * processed would be at sample 2, line 2.
   *
   * If you would like to see BoxcarManager being used in implementation,
   * see the ProcessByBoxcar class.
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2003-01-02 Tracie Sucharski
   *
   * @internal
   *   @history 2003-04-01 Tracie Sucharski - added documentation explaining the
   *                                          offsets (which pixel is being
   *                                          processed within the boxcar) and
   *                                          added unitTest.
   *   @history 2003-05-16 Stuart Sides - modified schema from
   *                                      astrogeology...isis.astrogeology
   */
  class BoxcarManager : public Isis::BufferManager {

    public:
      //  Constructors and Destructors
      BoxcarManager(const Isis::Cube &cube,
                    const int &boxSamples, const int &boxLines);

      //! Destroys the BoxcarManager object
      ~BoxcarManager() {};

  };
};

#endif


