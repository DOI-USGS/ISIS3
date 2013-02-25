#include "Isis.h"

#include <cmath>

#include <jama/jama_svd.h>
#include <tnt/tnt_array2d.h>

#include "CubeAttribute.h"
#include "Equalization.h"
#include "FileList.h"
#include "FileName.h"
#include "IString.h"
#include "LeastSquares.h"
#include "LineManager.h"
#include "MultivariateStatistics.h"
#include "OverlapNormalization.h"
#include "OverlapStatistics.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"

using namespace Isis;
using namespace std;


void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();

  // Make sure the user enters an "OUTSTATS" file if the CALCULATE option
  // is selected
  QString processOpt = ui.GetString("PROCESS");

  // Determine whether to calculate gains or offsets
  QString adjust = ui.GetString("ADJUST");
  OverlapNormalization::SolutionType sType = OverlapNormalization::Both;
  if (adjust == "CONTRAST") {
    sType = OverlapNormalization::Gains;
  }
  else if (adjust == "BRIGHTNESS") {
    sType = OverlapNormalization::Offsets;
  }
  else if (adjust == "GAIN") {
    sType = OverlapNormalization::GainsWithoutNormalization;
  }

  Equalization equalizer(sType, ui.GetFileName("FROMLIST"));

  // Read hold list if one was entered
  if (ui.WasEntered("HOLD")) {
    equalizer.addHolds(ui.GetFileName("HOLD"));
  }

  if (processOpt != "APPLY") {
    // Test to ensure sampling percent in bound
    double sampPercent = ui.GetDouble("PERCENT");

    int mincnt = ui.GetInteger("MINCOUNT");
    bool wtopt = ui.GetBoolean("WEIGHT");

    LeastSquares::SolveMethod methodType = LeastSquares::QRD;
    if (ui.GetString("SOLVEMETHOD") == "SVD") {
      methodType = LeastSquares::SVD;
    }
    else if (ui.GetString("SOLVEMETHOD") == "SPARSE") {
      methodType = LeastSquares::SPARSE;
    }
    equalizer.calculateStatistics(sampPercent, mincnt, wtopt, methodType);

    // Write the results to the log
    PvlGroup results = equalizer.getResults();
    Application::Log(results);

    // Setup the output text file if the user requested one
    if (ui.WasEntered("OUTSTATS")) {
      equalizer.write(ui.GetFileName("OUTSTATS"));
    }
  }
  else {
    equalizer.importStatistics(ui.GetFileName("INSTATS"));
  }

  // Apply the correction to the images if the user wants this done
  if (processOpt != "CALCULATE") {
    equalizer.applyCorrection(ui.WasEntered("TOLIST") ?
        ui.GetFileName("TOLIST") : "");
  }
}

