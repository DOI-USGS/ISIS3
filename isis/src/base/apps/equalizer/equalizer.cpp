/** This work is free and unencumbered software released into the public domain.
  * In jurisdictions that recognize copyright laws, the author or authors of
  * this software dedicate any and all copyright interest in the software to
  * the public domain.
  *
  * SPDX-License-Identifier: CC0-1.0
  */

#include "equalizer.h"

#include <jama/jama_svd.h>
#include <tnt/tnt_array2d.h>

#include "Application.h"
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

namespace Isis {

void equalizer(UserInterface &ui) {

  QString processOpt  = ui.GetString("PROCESS");
  QString solveMethod = ui.GetString("SOLVEMETHOD");

  // Determine whether to calculate gains or offsets
  QString adjust = ui.GetString("ADJUST");

  OverlapNormalization::SolutionType sType =
      OverlapNormalization::Both;

  if (adjust == "CONTRAST") {
    sType = OverlapNormalization::Gains;
  }
  else if (adjust == "BRIGHTNESS") {
    sType = OverlapNormalization::Offsets;
  }
  else if (adjust == "GAIN") {
    sType = OverlapNormalization::GainsWithoutNormalization;
  }

  Equalization equalizerObj(sType, ui.GetFileName("FROMLIST"));

  // Read hold list if entered
  if (ui.WasEntered("HOLD")) {
    equalizerObj.addHolds(ui.GetFileName("HOLD"));
  }

  // CALCULATE / BOTH / RETRYBOTH / RECALCULATE require stats
  if (processOpt != "APPLY") {

    try {
      if (processOpt == "RETRYBOTH" ||
          processOpt == "RECALCULATE") {

        equalizerObj.recalculateStatistics(
            ui.GetFileName("INSTATS"));
      }
      else {
        double sampPercent = ui.GetDouble("PERCENT");
        int mincnt         = ui.GetInteger("MINCOUNT");
        bool wtopt         = ui.GetBoolean("WEIGHT");

        LeastSquares::SolveMethod methodType =
            LeastSquares::QRD;

        if (solveMethod == "SVD") {
          methodType = LeastSquares::SVD;
        }
        else if (solveMethod == "SPARSE") {
          methodType = LeastSquares::SPARSE;
        }

        equalizerObj.calculateStatistics(
            sampPercent, mincnt, wtopt, methodType);
      }
    }
    catch (IException &e) {

      // If CALCULATE fails but OUTSTATS was requested,
      // write results before rethrowing.
      if (ui.WasEntered("OUTSTATS")) {
        PvlGroup results = equalizerObj.getResults();
        Application::Log(results);
        equalizerObj.write(ui.GetFileName("OUTSTATS"));
      }

      throw;
    }

    // Log results
    PvlGroup results = equalizerObj.getResults();
    Application::Log(results);

    // Write OUTSTATS if requested
    if (ui.WasEntered("OUTSTATS")) {
      equalizerObj.write(ui.GetFileName("OUTSTATS"));
    }
  }
  else {
    // APPLY imports statistics only
    equalizerObj.importStatistics(
        ui.GetFileName("INSTATS"));
  }

  // Apply correction unless CALCULATE or RECALCULATE only
  if (processOpt != "CALCULATE" &&
      processOpt != "RECALCULATE") {

    equalizerObj.applyCorrection(
        ui.WasEntered("TOLIST") ?
        ui.GetFileName("TOLIST") : "");
  }
}

} // namespace Isis