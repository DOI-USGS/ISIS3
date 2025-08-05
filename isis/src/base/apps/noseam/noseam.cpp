/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "noseam.h"

#include <iostream>
#include <fstream>

//#include "automos.h"
#include "FileList.h"
#include "FileName.h"
#include "Cube.h"
#include "Preference.h"
#include "ProgramLauncher.h"

using namespace std;

namespace Isis {

  /**
   * Noseam creates a mosaic from a list of input cubes using an
   * algorithm that minimizes seams.
   *
   * @param ui UserInterface object containing parameters
   */
  void noseam(UserInterface &ui) {

    // Get Filename with list of cubes to mosaic
    FileName cubeListFileName(ui.GetFileName("FROMLIST"));

    return noseam(cubeListFileName, ui);
  }


  /**
   * Noseam creates a mosaic from a list of input cubes using an
   * algorithm that minimizes seams.
   *
   * @param cubeListFileName Filename with list of cubes to mosaic
   * @param ui UserInterface object containing parameters
   */
  void noseam(FileName &cubeListFileName, UserInterface &ui) {

    // Boxcar samples and lines must be odd and 1 or greater
    int samples;
    if (ui.WasEntered("SAMPLES")) {
      samples = ui.GetInteger("SAMPLES");
      
      if (samples < 1 || samples % 2 == 0) {
        string msg = "Value for [SAMPLES] must be odd and greater or equal to 1.";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    else {
      string msg = "Parameter [SAMPLES] must be entered.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    int lines;
    if (ui.WasEntered("LINES")) {
      lines = ui.GetInteger("LINES");

      if (lines < 1 || lines % 2 == 0) {
        string msg = "Value for [LINES] must be odd and greater or equal to 1.";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    else {
      string msg = "Parameter [LINES] must be entered.";
      throw IException(IException::User, msg, _FILEINFO_);      
    }
    // Get user parameters
    FileList cubes;
    cubes.read(cubeListFileName);

    QString match = ui.GetAsString("MATCHBANDBIN");

    QString pathName = FileName("$TEMPORARY/").expanded();

    /**
     * Creates a mosaic from the original images.
    */
    QString parameters = "FROMLIST=" + cubeListFileName.original() + 
                        " MOSAIC=" + pathName + "OriginalMosaic.cub" +
                        " MATCHBANDBIN=" + match;
    ProgramLauncher::RunIsisProgram("automos", parameters);

    // Does a highpass on the original mosaic
    parameters = "FROM=" + pathName + "OriginalMosaic.cub" +
                  " TO=" + pathName + "HighpassMosaic.cub"
                  + " SAMPLES=" + toString(samples) + " LINES=" + toString(lines);
    ProgramLauncher::RunIsisProgram("highpass", parameters);

    // Does a lowpass on the original mosaic
    parameters = "FROM=" + pathName + "OriginalMosaic.cub"
                 + " TO=" + pathName + "LowpassMosaic.cub"
                 + " SAMPLES=" + toString(samples) + " LINES=" + toString(lines);
    ProgramLauncher::RunIsisProgram("lowpass", parameters);

    // Finally combines the highpass and lowpass mosaics
    parameters = "FROM=" + pathName + "HighpassMosaic.cub" +
                 " FROM2=" + pathName + "LowpassMosaic.cub" +
                 " TO=" + ui.GetCubeName("TO") +
                 " OPERATOR= add";
    ProgramLauncher::RunIsisProgram("algebra", parameters);

    // Will remove all of the temp files by default
    if(ui.GetBoolean("REMOVETEMP")) {
      QString file;
      file = pathName + "HighpassMosaic.cub";
      remove(file.toLatin1().data());
      file = pathName + "LowpassMosaic.cub";
      remove(file.toLatin1().data());
      file = pathName + "OriginalMosaic.cub";
      remove(file.toLatin1().data());
    }
  }
}
