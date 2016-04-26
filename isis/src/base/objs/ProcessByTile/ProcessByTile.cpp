/**
 * @file
 * $Revision: 1.1.1.1 $
 * $Date: 2006/10/31 23:18:09 $
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

#include "ProcessByTile.h"
#include "TileManager.h"

using namespace std;
namespace Isis {

  /**
   * Sets the tile size
   *
   * @param ns Number of samples
   *
   * @param nl Number of lines
   */
  void ProcessByTile::SetTileSize(const int ns, const int nl) {
    p_tileSamples = ns;
    p_tileLines = nl;
    p_tileSizeSet = true;
  }

  /**
   * Starts the systematic processing of the input cube by moving an arbitrary
   * shaped tile through the cube. This method requires that exactly one input
   * cube and one output cube be loaded using the SetInputCube and SetOutputCube
   * methods.
   *
   * @deprecated Please use ProcessCube()
   * @param funct (Buffer &in, Buffer &out) Receive an nxm tile in
   *              the input buffer and output the an nxm tile. If n=1 and m=lines
   *              this will process by columns. Likewise if n=samples and m=1
   *              this will process by lines.
   *
   * @throws iException::Programmer
   */
  void ProcessByTile::StartProcess(void
                                   funct(Buffer &in, Buffer &out)) {

    //SetBrickSizesForProcessCube();
      VerifyCubes(InputOutput);
      SetBricks(InputOutput);
      ProcessByBrick::StartProcess(funct);
  }


  /**
   * Starts the systematic processing of the input cube by moving an arbitrary
   * shaped tile through the cube. This method requires that exactly one input
   * cube be loaded. No output cubes are produced.
   *
   * @deprecated Please use ProcessCubeInPlace()
   * @param funct (Buffer &in) Receive an nxm tile in the input buffer. If
   *                                n=1 and m=lines this will process by columns.
   *                                Likewise if n=samples and m=1 this will
   *                                process by lines.
   *
   * @throws iException::Programmer
   */
  void ProcessByTile::StartProcess(void funct(Buffer &in)) {

      VerifyCubes(InPlace);
      SetBricks(InPlace);
    //SetBrickSizesForProcessCubeInPlace();
    ProcessByBrick::StartProcess(funct);
  }


  /**
   * Starts the systematic processing of the input cube by moving an arbitrary
   * shaped tile through the cube. This method allows multiple input and output
   * cubes.
   *
   * @deprecated Please use ProcessCubes()
   * @param funct (vector<Buffer *> &in, vector<Buffer *> &out) Receive
   *                                an nxm tile in the input buffer. If
   *                                n=1 and m=lines this will process by columns.
   *                                Likewise if n=samples and m=1 this will
   *                                process by lines.
   *
   * @throws iException::Programmer
   */
  void ProcessByTile::StartProcess(void funct(std::vector<Buffer *> &in,
                                   std::vector<Buffer *> &out)) {
    //SetBrickSizesForProcessCubes();
      VerifyCubes(InputOutputList);
      SetBricks(InputOutputList);
    ProcessByBrick::StartProcess(funct);
  }


  /**
   * End the processing sequence and cleans up by closing cubes, freeing memory,
   * etc.
   *
   * @deprecated Please use Finalize()
   */
  void ProcessByTile::EndProcess() {
    p_tileSizeSet = false;
    ProcessByBrick::EndProcess();
  }


  /**
   * Cleans up by closing cubes and freeing memory
   */
  void ProcessByTile::Finalize() {
    p_tileSizeSet = false;
    ProcessByBrick::Finalize();
  }


  void ProcessByTile::SetBricks(IOCubes cn){

      switch(cn) {

        case InPlace:

          //  Make sure the tile size has been set
          if(!p_tileSizeSet) {
            string m = "Use the SetTileSize method to set the tile size";
            throw IException(IException::Programmer, m, _FILEINFO_);
          }

          ProcessByBrick::SetBrickSize(p_tileSamples, p_tileLines, 1);

          break;


        case InputOutput:
          if(!p_tileSizeSet) {
            string m = "Use the SetTileSize method to set the tile size";
            throw IException(IException::Programmer, m, _FILEINFO_);
          }

          ProcessByBrick::SetBrickSize(p_tileSamples, p_tileLines, 1);

          break;

        case InputOutputList:

          if(!p_tileSizeSet) {
            string m = "Use the SetTileSize method to set the tile size";
            throw IException(IException::Programmer, m, _FILEINFO_);
          }

           ProcessByBrick::SetBrickSize(p_tileSamples, p_tileLines, 1);

          break;


      }


  }



} // end namespace isis

