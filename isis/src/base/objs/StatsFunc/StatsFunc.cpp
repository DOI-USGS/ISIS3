#include "StatsFunc.h"

#include <string>
#include <iostream>

#include "Application.h"
#include "CubeAttribute.h"
#include "Cube.h"
#include "FileName.h"
#include "Histogram.h"
#include "Process.h"
#include "Pvl.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

namespace Isis {
  void stats(UserInterface &ui) {

    Cube *inputCube = new Cube();
    CubeAttributeInput inAtt(ui.GetAsString("FROM"));
    inputCube->setVirtualBands(inAtt.bands());
    inputCube->open(ui.GetFileName("FROM"));

    double validMin = Isis::ValidMinimum;
    double validMax = Isis::ValidMaximum;

    if ( ui.WasEntered("VALIDMIN") ) {
      validMin = ui.GetDouble("VALIDMIN");
    }

    if ( ui.WasEntered("VALIDMAX") ) {
      validMax = ui.GetDouble("VALIDMAX");
    }

    Pvl statsPvl = stats(inputCube, validMin, validMax);

    for (int resultIndex = 0; resultIndex < statsPvl.groups(); resultIndex++) {
      if (statsPvl.group(resultIndex).name() == "Results") {
        Application::Log(statsPvl.group(resultIndex));
      }
    }

    delete inputCube;
    inputCube = NULL;

    if ( ui.WasEntered("TO") ) {
      QString outFile = FileName(ui.GetFileName("TO")).expanded();
      bool append = ui.GetBoolean("APPEND");
      //write the results in the requested format.
      if ( ui.GetString("FORMAT") == "PVL" ) {
        if (append) {
          statsPvl.append(outFile);
        }
        else {
          statsPvl.write(outFile);
        }
      }
      else {
        bool exists = FileName(outFile).fileExists();
        bool writeHeader = false;
        ofstream *os = new ofstream;
        if (append) {
          os->open(outFile.toLatin1().data(), ios::app);
          if (!exists) {
            writeHeader = true;
          }
        }
        else {
          os->open(outFile.toLatin1().data(), ios::out);
          writeHeader = true;
        }
        writeStatsStream(statsPvl, writeHeader, os);
        delete os;
        os = NULL;
      }
    }
  }


  Pvl stats(Cube *cube, double validMin, double validMax) {

    Process process;

    // Get the histogram
    process.SetInputCube(cube);

    // Set a global Pvl for storing results
    Pvl statsPvl;

    // Get the number of bands to process
    int bandCount = cube->bandCount();

    for (int i = 1; i <= bandCount; i++) {
      Histogram *stats = cube->histogram(i, validMin, validMax);

      // Construct a label with the results
      PvlGroup results("Results");
      results += PvlKeyword("From", cube->fileName());
      results += PvlKeyword("Band", toString(cube->physicalBand(i)));
      if ( stats->ValidPixels() != 0 ) {
        results += PvlKeyword("Average", toString(stats->Average()));
        results += PvlKeyword("StandardDeviation", toString(stats->StandardDeviation()));
        results += PvlKeyword("Variance", toString(stats->Variance()));
        // These statistics only worked on a histogram
        results += PvlKeyword("Median", toString(stats->Median()));
        results += PvlKeyword("Mode", toString(stats->Mode()));
        results += PvlKeyword("Skew", toString(stats->Skew()));
        results += PvlKeyword("Minimum", toString(stats->Minimum()));
        results += PvlKeyword("Maximum", toString(stats->Maximum()));
        results += PvlKeyword("Sum", toString(stats->Sum()));
      }
      results += PvlKeyword("TotalPixels", toString(stats->TotalPixels()));
      results += PvlKeyword("ValidPixels", toString(stats->ValidPixels()));
      results += PvlKeyword("OverValidMaximumPixels", toString(stats->OverRangePixels()));
      results += PvlKeyword("UnderValidMinimumPixels", toString(stats->UnderRangePixels()));
      results += PvlKeyword("NullPixels", toString(stats->NullPixels()));
      results += PvlKeyword("LisPixels", toString(stats->LisPixels()));
      results += PvlKeyword("LrsPixels", toString(stats->LrsPixels()));
      results += PvlKeyword("HisPixels", toString(stats->HisPixels()));
      results += PvlKeyword("HrsPixels", toString(stats->HrsPixels()));

      statsPvl.addGroup(results);

      delete stats;
      stats = NULL;
    }

    return statsPvl;
  }


  void writeStatsStream(Pvl statsPvl, bool writeHeader, ostream *stream) {
    if (writeHeader) {
      for (int i = 0; i < statsPvl.group(0).keywords(); i++) {
        *stream << statsPvl.group(0)[i].name();
        if ( i < statsPvl.group(0).keywords() - 1 ) {
          *stream << ",";
        }
      }
      *stream << endl;
    }

    for (int i = 0; i < statsPvl.groups(); i++) {
      for (int j = 0; j < statsPvl.group(i).keywords(); j++) {
        *stream << (QString) statsPvl.group(i)[j];
        if ( j < statsPvl.group(i).keywords() - 1 ) {
          *stream << ",";
        }
      }
      *stream << endl;
    }
  }

}
