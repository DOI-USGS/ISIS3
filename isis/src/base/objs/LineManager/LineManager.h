#ifndef LineManager_h
#define LineManager_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "BufferManager.h"
#include "Cube.h"

namespace Isis {
  /**
   * @brief Buffer manager, for moving through a cube in lines
   *
   * This class is used as a manager for moving through a cube one line at a time.
   * A line is defined as a one dimensional sub-area of a cube. That is, the
   * number of samples by 1 line by 1 band (ns,1,1). The manager moves this
   * (ns,1,1) shape through the cube sequentially accessing all the lines in the
   * first band before proceeding to the second band.
   *
   * If you would like to see LineManager being used in implementation,
   * see the ProcessByLine class.
   *
   * @ingroup LowLevelCubeIO
   *
   * @author 2003-02-13 Jeff Anderson
   *
   * @internal
   *   @history 2003-05-16 Stuart Sides - Modified schema from astrogeology...
   *                           isis.astrogeology...
   *   @history 2005-02-28 Elizabeth Ribelin - Modified file to support Doxygen
   *                           documentation
   *   @history 2007-12-06 Chris Austin - Added option to constructor to change
   *                           the order of the progression through the cube
   *   @history 2012-11-21 Jeannie Backer - Improved error messages. References
   *                           #1058.
   */
  class LineManager : public Isis::BufferManager {

    public:
      // Constructors and Destructors
      LineManager(const Isis::Cube &cube, const bool reverse = false);

      //! Destroys the LineManager object
      ~LineManager() {};

      bool SetLine(const int line, const int band = 1);
  };
};

#endif

