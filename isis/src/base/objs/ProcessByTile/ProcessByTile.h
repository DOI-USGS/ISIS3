#ifndef ProcessByTile_h
#define ProcessByTile_h
/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "ProcessByBrick.h"
#include "Buffer.h"

namespace Isis {
  /**
   * @brief Process cubes by tile
   *
   * This is the processing class used to move a tile through cube data. This
   * class allows only one input cube and one output cube or one input cube. If
   * the tile size does not evenly divide into the image the tile will be padded
   * with Null pixels as it falls off the right and/or bottom edge of the image.
   * The tile shape is only spatial-oriented with one band of data.
   *
   * @ingroup HighLevelCubeIO
   *
   * @author 2004-05-14 Jeff Anderson
   *
   * @internal
   *   @history 2005-02-08 Elizabeth Ribelin - Modified file to support Doxygen
   *                           documentation
   *   @history 2005-11-28 Jacob Danton - Modified file to allow processing
   *                           with multiple input and output cubes.
   *   @history 2006-04-03 Jacob Danton - Rewrote the code to extend
   *                           ProcessByBrick class
   *   @history 2011-08-19 Jeannie Backer - Modified unitTest to use
   *                           $temporary variable instead of /tmp directory.
   *  
   *  
   *  @todo 2005-02-08 Jeff Anderson - add coded example, and implementation
   *                       example to class documentation
   */
  class ProcessByTile : public ProcessByBrick {

    private:
      bool p_tileSizeSet;  //!< Indicates whether the tile size has been set
      int p_tileSamples;   //!< Number of samples in the tile
      int p_tileLines;     //!< Number of lines in the tile

    public:

      //! Constructs a ProcessByTile object
      ProcessByTile() {
        p_tileSizeSet = false;
      };

      //! Destroys the ProcessByTile object
      ~ProcessByTile() {};

      void SetTileSize(const int ns, const int nl);

      void StartProcess(void funct(Buffer &in));
      void StartProcess(void funct(Buffer &in, Buffer &out));

      void StartProcess(void funct(std::vector<Buffer *> &in,
                                   std::vector<Buffer *> &out));
      void EndProcess();
      void Finalize();

      /**
       * @see ProcessByBrick::ProcessCubeInPlace()
       * @param funct
       * @param threaded
       */
      template <typename Functor>
      void ProcessCubeInPlace(const Functor & funct, bool threaded = true) {
          VerifyCubes(InPlace);
          SetBricks(InPlace);
        //SetBrickSizesForProcessCubeInPlace();
        ProcessByBrick::ProcessCubeInPlace(funct, threaded);
      }

      /**
       * @see ProcessByBrick::ProcessCube()
       * @param funct
       * @param threaded
       */
      template <typename Functor>
      void ProcessCube(const Functor & funct, bool threaded = true) {

        VerifyCubes(InputOutput)  ;
        SetBricks(InputOutput);

        //SetBrickSizesForProcessCube();
        ProcessByBrick::ProcessCube(funct, threaded);
      }

      /**
       * @see ProcessByBrick::ProcessCubes()
       * @param funct
       * @param threaded
       */
      template <typename Functor>
      void ProcessCubes(const Functor & funct, bool threaded = true) {

          VerifyCubes(InputOutputList);
          SetBricks(InputOutputList);
        //SetBrickSizesForProcessCubes();
        ProcessByBrick::ProcessCubes(funct, threaded);
      }

    private:
      void SetBricks(IOCubes cn);
      void SetBrickSizesForProcessCubeInPlace();
      void SetBrickSizesForProcessCube();
      void SetBrickSizesForProcessCubes();
  };
};

#endif
