/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "WriteCube.h"

using namespace std;

namespace Isis {
  void WriteCube(QString& cubeFileIn, QString* cubeFileOut, UserInterface &ui, bool remvBias, int lines) {
    try {
      LineManager *lineMgr = nullptr; // Initialize pointer to nullptr
      Cube *iCube = new Cube();
    
      // Open input cube
      iCube->open(cubeFileIn, "r");
      lineMgr = new LineManager(*iCube);  // Initialize LineManager
      
      int samples = SHC_AFE_WIDTH * SHC_CHANNELS;
      int channelWidth = SHC_AFE_WIDTH;

      if (remvBias) {
        samples = SHC_SCENE * SHC_CHANNELS;
        channelWidth = SHC_SCENE;
      }

      /**
        * @brief Lambda to remove bias pixels
        *
        * This lambda function removes bias pixels.
        *
        * @param out Reference to the output line buffer.    
        **/
      auto WriteOutCube = [&](Isis::Buffer &out) -> void {
        lineMgr->SetLine(out.Line(), 1);
        iCube->read(*lineMgr);
        int outdex = 0;
        int index = 0;
        int column = 0;

        if (remvBias) {
          for (int channel = 0; channel < SHC_CHANNELS; channel++) {
            for (int pixel = 0; pixel < channelWidth; pixel++) {
              outdex = GetDataIndex(channel, SHC_SCENE, pixel);
              column = pixel + SHC_SCENE_OFFSET;
              index = GetDataIndex(channel, SHC_AFE_WIDTH, column);
              out[outdex] = (*lineMgr)[index];
            }
          }
        } 
        else {
          for (int index = 0; index < samples; index++) {
            out[index] = (*lineMgr)[index];
          }
        }
      };

      // Setup the ProcessByLine object
      ProcessByLine wp;
      wp.PropagateHistory(false);
      CubeAttributeInput inputAtt = CubeAttributeInput();
      CubeAttributeOutput outputAtt = CubeAttributeOutput();
      wp.SetInputCube(cubeFileIn, inputAtt, 0);

      // Check if output file is provided
      if (cubeFileOut == nullptr || cubeFileOut->isEmpty()) {
        if (remvBias) {
          puts("Removing bias pixel columns from output cube");
        }
        wp.SetOutputCube(ui.GetCubeName("TO"), outputAtt, samples, lines, SHC_BANDS);
      } 
      else {
        wp.SetOutputCube(*cubeFileOut, outputAtt, samples, lines, SHC_BANDS);
      }

      wp.PropagateTables(false);
      wp.ClearInputCubes();
      wp.StartProcess(WriteOutCube);

      // Ensure cleanup of lineMgr pointer
      if (lineMgr) {
        delete lineMgr;
      }

      wp.EndProcess();
      wp.Finalize();
      
    }
    catch (const IException& e) {
      throw IException(e, IException::Programmer, "ISIS Exception: " + Isis::toString(e.what()) + ". Unable to write cube.", _FILEINFO_);
    }
    catch (const std::exception &e) {
      cerr << "Standard exception: " << e.what() << ". Unable to write cube." << endl;
      exit(1);
    }
    catch (...) {
      throw IException(IException::Programmer, "Unknown exception occured. Unable to write cube.", _FILEINFO_);
    }
  }
}
