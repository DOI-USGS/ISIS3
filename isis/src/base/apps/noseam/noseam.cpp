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
    std::list<FileName> tempFiles = {};

    // Get user parameters
    FileList cubes;
    cubes.read(cubeListFileName);

    QString match = ui.GetAsString("MATCHBANDBIN");

    // Sets up the pathName to be used for most application calls
    FileName inFile = cubes[0];

    FileName mosaicFile = FileName::createTempFile("$TEMPORARY/OriginalMosaic.cub");
    tempFiles.push_back(mosaicFile);

    /**
     * Creates a mosaic from the original images.  It is placed here
     * so that the failure MATCHBANDBIN causes does not leave
     * highpasses cubes lying around!
    */
    QString parameters = "FROMLIST=" + cubeListFileName.original() + 
                        " MOSAIC=" + mosaicFile.expanded() +
                        " MATCHBANDBIN=" + match;
    ProgramLauncher::RunIsisProgram("automos", parameters);

    // Creates the highpass cubes from the cubes FileList
    FileList highPassList;
    for(int i = 0; i < cubes.size(); i++) {
      inFile = cubes[i];
      FileName highpassCube = FileName::createTempFile("$TEMPORARY/" + inFile.baseName() + "_highpass.cub");
      tempFiles.push_back(highpassCube);
      parameters = "FROM=" + inFile.expanded() +
                   " TO=" + highpassCube.expanded() +
                   " SAMPLES=" + toString(samples) + 
                   " LINES=" + toString(lines);
      ProgramLauncher::RunIsisProgram("highpass", parameters);
      // Reads the just created highpass cube into a list file for automos
      highPassList.push_back(highpassCube);
    }
    FileName highpassListFile = FileName::createTempFile("$TEMPORARY/HighPassList.lis");
    highPassList.write(highpassListFile);
    tempFiles.push_back(highpassListFile);

    // Makes a mosaic out of the highpass cube filelist
    FileName highpassFile = FileName::createTempFile("$TEMPORARY/HighpassMosaic.cub");
    tempFiles.push_back(highpassFile);

    parameters = "FROMLIST=" + highpassListFile.expanded() +
                 " MOSAIC=" + highpassFile.expanded() +
                 " MATCHBANDBIN=" + match;
    ProgramLauncher::RunIsisProgram("automos", parameters);

    FileName lowpassFile = FileName::createTempFile("$TEMPORARY/HighpassMosaic.cub");
    tempFiles.push_back(lowpassFile);

    // Does a lowpass on the original mosaic
    parameters = "FROM=" + mosaicFile.expanded() +
                 " TO=" + lowpassFile.expanded() +
                 " SAMPLES=" + toString(samples) + " LINES=" + toString(lines);
    ProgramLauncher::RunIsisProgram("lowpass", parameters);

    // Finally combines the highpass and lowpass mosaics
    parameters = "FROM=" + highpassFile.expanded() +
                 " FROM2=" + lowpassFile.expanded() +
                 " TO=" + ui.GetCubeName("TO") +
                 " OPERATOR= add";
    ProgramLauncher::RunIsisProgram("algebra", parameters);

    // Will remove all of the temp files by default
    if(ui.GetBoolean("REMOVETEMP")) {
      QString topLevelMsg = "Failed to remove noseam temp file(s)";
      IException e(IException::Unknown, topLevelMsg, _FILEINFO_);
      QString msg = "Failed to remove temp file [";
      for (FileName file : tempFiles) {
        remove(file.expanded().toLatin1().data());
        if (file.fileExists()) {
          QString fileMsg = msg + file.name() + "]";
          e.append(IException(IException::Unknown, fileMsg, _FILEINFO_));
        }
      }
      if (e.length() > 0) {
        throw e;
      }
    }
  }
}
