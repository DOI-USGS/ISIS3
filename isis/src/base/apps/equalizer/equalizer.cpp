#include "Isis.h"

#include <cmath>

#include <jama/jama_svd.h>
#include <QString>
#include <tnt/tnt_array2d.h>

#include "CubeAttribute.h"
#include "Equalization.h"
#include "FileList.h"
#include "FileName.h"
#include "IException.h"
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
  QString solveMethod = ui.GetString("SOLVEMETHOD");

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

  // BOTH, RETRYBOTH, CALCULATE or RECALCULATE need to calculate statistics
  if (processOpt != "APPLY") {

    try {
      if (processOpt == "RETRYBOTH" || processOpt == "RECALCULATE") {
        equalizer.recalculateStatistics(ui.GetFileName("INSTATS"));
      }
      else {
        // Test to ensure sampling percent in bound
        double sampPercent = ui.GetDouble("PERCENT");

        int mincnt = ui.GetInteger("MINCOUNT");
        bool wtopt = ui.GetBoolean("WEIGHT");

        LeastSquares::SolveMethod methodType = LeastSquares::QRD;
        if (solveMethod == "SVD") {
          methodType = LeastSquares::SVD;
        }

        equalizer.calculateStatistics(sampPercent, mincnt, wtopt, methodType);
      }
    }
    // Need to make sure we write statistics and log results if we encounter:
    //  "There are input images that do not overlap..."
    //  "Unable to calculate the equalization statistics..."
    catch (IException &e) {
      if (ui.WasEntered("OUTSTATS")) {
        PvlGroup results = equalizer.getResults();
        Application::Log(results);
        equalizer.write(ui.GetFileName("OUTSTATS"));
      }
      // Now that outstats have been created and the results have been logged,
      // re-throw the exception to halt equalizer and inform the user.
      throw e;
    }

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
  // i.e. BOTH or RETRYBOTH
  if (processOpt != "CALCULATE" && processOpt != "RECALCULATE") {
    equalizer.applyCorrection(ui.WasEntered("TOLIST") ?
        ui.GetFileName("TOLIST") : "");
  }
}
