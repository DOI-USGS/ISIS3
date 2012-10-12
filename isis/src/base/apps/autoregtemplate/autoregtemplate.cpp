#include "Isis.h"
#include "PvlObject.h"
#include "PvlGroup.h"
#include "UserInterface.h"
#include "IException.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  // Get user interface
  UserInterface &ui = Application::GetUserInterface();

  Pvl algos("$ISISROOT/lib/AutoReg.plugin");
  Pvl p;

  // Begin creating the mapping group
  PvlObject autoreg("AutoRegistration");

  // Make sure the entered algorithm name is valid
  string algoName = ui.GetString("ALGORITHM");
  if(!algos.HasGroup(algoName)) {
    // Give the user a list of possible algorithms
    string msg = "Invalid value for [ALGORITHM] entered [" + algoName + "].  "
      + "Must be one of [";

    for(int i = 0; i < algos.Groups(); i++) {
      if(i != 0 &&
         algos.Group(i).Name() == algos.Group(0).Name()) break;

      if(i != 0) msg += ", ";
      msg += algos.Group(i).Name();
    }
    msg += "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Make algorithm group
  PvlGroup algorithm("Algorithm");
  algorithm += PvlKeyword("Name", algoName);

  // Set the tolerance
  double tol = ui.GetDouble("TOLERANCE");
  algorithm += PvlKeyword("Tolerance", IString(tol));

  // Set the reduction factor if the user entered it
  if(ui.WasEntered("REDUCTIONFACTOR")) {
    int reduction = ui.GetInteger("REDUCTIONFACTOR");
    algorithm += PvlKeyword("ReductionFactor", IString(reduction));

    if(reduction < 1) {
      string msg = "Invalid value for [REDUCTIONFACTOR] entered ["
        + IString(reduction) + "].  Must be greater than or equal to 1 (Default = 1)";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }

  bool subPixelAccuracy = ui.GetBoolean("SUBPIXELACCURACY");
  algorithm += PvlKeyword("SubpixelAccuracy", (subPixelAccuracy) ? "True" : "False");

  // Set the chip interpolator type
  if(ui.GetBoolean("CHIPINTERPOLATOR")) {
    IString itype = ui.GetString("INTERPOLATORTYPE");
    if(itype == "NEARESTNEIGHBORTYPE") {
      algorithm += PvlKeyword("ChipInterpolator", "NearestNeighborType");
    }
    else if(itype == "BILINEARTYPE") {
      algorithm += PvlKeyword("ChipInterpolator", "BiLinearType");
    }
    else {
      algorithm += PvlKeyword("ChipInterpolator", "CubicConvolutionType");
    }
  }
  else {
    if(ui.WasEntered("INTERPOLATORTYPE")) {
      string msg = "CHIPINTERPOLATOR parameter must be set to TRUE to enter INTERPOLATORTYPE parameter";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }

  // Add algorithm group to the autoreg object
  autoreg.AddGroup(algorithm);

  // Get pattern and search chip size values for error testing
  int psamp = ui.GetInteger("PSAMP");
  int pline = ui.GetInteger("PLINE");
  int ssamp = ui.GetInteger("SSAMP");
  int sline = ui.GetInteger("SLINE");

  // Make sure the pattern chip is not just one pixel
  if(psamp + pline < 3) {
    string msg = "The Pattern Chip must be larger than one pixel for the ";
    msg += "autoregistration to work properly";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  // Make sure the pattern chip is smaller than the search chip
  if(ssamp < psamp || sline < pline) {
    string msg = "The Pattern Chip must be smaller than the Search Chip";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  // Make sure the pattern chip spans at least a 3x3 window in the search chip
  if(psamp + 2 > ssamp || pline + 2 > sline) {
    string msg = "The Pattern Chip must span at least a 3x3 window in the ";
    msg += "Search Chip";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Set up the pattern chip group
  PvlGroup patternChip("PatternChip");
  patternChip += PvlKeyword("Samples", IString(psamp));
  patternChip += PvlKeyword("Lines", IString(pline));
  if(ui.WasEntered("PMIN")) {
    patternChip += PvlKeyword("ValidMinimum", IString(ui.GetInteger("PMIN")));
  }
  if(ui.WasEntered("PMAX")) {
    patternChip += PvlKeyword("ValidMaximum", IString(ui.GetInteger("PMAX")));
  }
  if(ui.WasEntered("MINIMUMZSCORE")) {
    double minimum = ui.GetDouble("MINIMUMZSCORE");
    patternChip += PvlKeyword("MinimumZScore", IString(minimum));

    if(minimum <= 0.0) {
      string msg = "Invalid value for [MINIMUMZSCORE] entered ["
        + IString(minimum) + "].  Must be greater than 0.0 (Default = 1.0)";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }
  if(ui.WasEntered("PVALIDPERCENT")) {
    double percent = ui.GetDouble("PVALIDPERCENT");
    if((percent <= 0.0) || (percent > 100.0)) {
      string msg = "Invalid value for [PVALIDPERCENT] entered ["
        + IString(percent) + "].  Must be greater than 0.0 and less than or equal to 100.0 (Default = 50.0)";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    patternChip += PvlKeyword("ValidPercent", IString(percent));
  }

  // Set up the search chip group
  PvlGroup searchChip("SearchChip");
  searchChip += PvlKeyword("Samples", IString(ssamp));
  searchChip += PvlKeyword("Lines", IString(sline));
  if(ui.WasEntered("SMIN")) {
    searchChip += PvlKeyword("ValidMinimum", IString(ui.GetInteger("SMIN")));
  }
  if(ui.WasEntered("SMAX")) {
    searchChip += PvlKeyword("ValidMaximum", IString(ui.GetInteger("SMAX")));
  }
  if(ui.WasEntered("SSUBCHIPVALIDPERCENT")) {
    double percent = ui.GetDouble("SSUBCHIPVALIDPERCENT");
    if((percent <= 0.0) || (percent > 100.0)) {
      string msg = "Invalid value for [SSUBCHIPVALIDPERCENT] entered ["
        + IString(percent) + "].  Must be greater than 0.0 and less than or equal to 100.0 (Default = 50.0)";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    searchChip += PvlKeyword("SubchipValidPercent", IString(percent));
  }

  // Add groups to the autoreg object
  autoreg.AddGroup(patternChip);
  autoreg.AddGroup(searchChip);

  // Set up the surface model testing group
  if(subPixelAccuracy) {
    PvlGroup surfaceModel("SurfaceModel");

    double distanceTol = ui.GetDouble("DISTANCETOLERANCE");
    surfaceModel += PvlKeyword("DistanceTolerance", IString(distanceTol));

    if(distanceTol <= 0.0) {
      string msg = "Invalid value for [DISTANCETOLERANCE] entered ["
        + IString(distanceTol) + "].  Must be greater than 0.0 (Default = 1.5)";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    int winSize = ui.GetInteger("WINDOWSIZE");
    surfaceModel += PvlKeyword("WindowSize", IString(winSize));

    // Make sure the window size is at least 3
    if(winSize < 3) {
      string msg = "Invalid value for [WINDOWSIZE] entered ["
        + IString(winSize) + "].  Must be greater than or equal to 3 (Default = 5)";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Make sure the window size is odd
    if(winSize % 2 == 0) {
      string msg = "Invalid value for [WINDOWSIZE] entered ["
        + IString(winSize) + "].  Must be an odd number (Default = 5)";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if(ui.GetBoolean("ECCENTRICITYTESTING")) {
      double eccRatio = ui.GetDouble("ECCENTRICITYRATIO");
      surfaceModel += PvlKeyword("EccentricityRatio", IString(eccRatio));

      if(eccRatio < 1) {
        string msg = "Invalid value for [ECCENTRICITYRATIO] entered ["
          + IString(eccRatio) + "].  Must be 1.0 or larger (Default = 2.0)";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    if(ui.GetBoolean("RESIDUALTESTING")) {
      double residualTol = ui.GetDouble("RESIDUALTOLERANCE");
      surfaceModel += PvlKeyword("ResidualTolerance", IString(residualTol));

      if(residualTol < 0) {
        string msg = "Invalid value for [RESIDUALTOLERANCE] entered ["
          + IString(residualTol) + "].  Must be 0.0 or larger (Default = 0.1)";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    autoreg.AddGroup(surfaceModel);
  }

  // Add autoreg group to Pvl
  p.AddObject(autoreg);

  // Write the autoreg group pvl to the output file
  string output = ui.GetFileName("TO");
  p.Write(output);

  Isis::Application::GuiLog(p);
}
