#include "Isis.h"
#include "PvlObject.h"
#include "PvlGroup.h"
#include "UserInterface.h"
#include "iException.h"

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
    string msg = "Invalid Algorithm Name [" + algoName + "] " +
                 "Possible Algorithms are: ";
    for(int i = 0; i < algos.Groups(); i++) {
      msg += algos.Group(i).Name();
      if(i < algos.Groups() - 1) msg += ", ";
    }
    throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
  }

  // Make algorithm group
  PvlGroup algorithm("Algorithm");
  algorithm += PvlKeyword("Name", algoName);

  // Set the tolerance
  double tol = ui.GetDouble("TOLERANCE");
  algorithm += PvlKeyword("Tolerance", iString(tol));

  // Set the reduction factor if the user entered it
  if(ui.WasEntered("REDUCTIONFACTOR")) {
    int reduction = ui.GetInteger("REDUCTIONFACTOR");
    algorithm += PvlKeyword("ReductionFactor", iString(reduction));

    if(reduction < 1) {
      string msg = "REDUCTIONFACTOR must be greater than or equal to 1 (Default = 1)";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }

  bool subPixelAccuracy = ui.GetBoolean("SUBPIXELACCURACY");
  algorithm += PvlKeyword("SubpixelAccuracy", (subPixelAccuracy) ? "True" : "False");

  // Set the chip interpolator type
  if(ui.GetBoolean("CHIPINTERPOLATOR")) {
    iString itype = ui.GetString("INTERPOLATORTYPE");
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
      string msg = "[CHIPINTERPOLATOR] parameter must be set to TRUE to enter [INTERPOLATORTYPE] parameter";
      throw iException::Message(iException::User, msg, _FILEINFO_);
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
    throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
  }
  // Make sure the pattern chip is smaller than the search chip
  if(ssamp < psamp || sline < pline) {
    string msg = "The Pattern Chip must be smaller than the Search Chip";
    throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
  }
  // Make sure the pattern chip spans at least a 3x3 window in the search chip
  if(psamp + 2 > ssamp || pline + 2 > sline) {
    string msg = "The Pattern Chip must span at least a 3x3 window in the ";
    msg += "Search Chip";
    throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
  }

  // Set up the pattern chip group
  PvlGroup patternChip("PatternChip");
  patternChip += PvlKeyword("Samples", iString(psamp));
  patternChip += PvlKeyword("Lines", iString(pline));
  if(ui.WasEntered("PMIN")) {
    patternChip += PvlKeyword("ValidMinimum", iString(ui.GetInteger("PMIN")));
  }
  if(ui.WasEntered("PMAX")) {
    patternChip += PvlKeyword("ValidMaximum", iString(ui.GetInteger("PMAX")));
  }
  if(ui.WasEntered("MINIMUMZSCORE")) {
    double minimum = ui.GetDouble("MINIMUMZSCORE");
    patternChip += PvlKeyword("MinimumZScore", iString(minimum));

    if(minimum <= 0.0) {
      string msg = "MINIMUMZSCORE must be greater than 0.0 (Default = 1.0)";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }

  // Set up the search chip group
  PvlGroup searchChip("SearchChip");
  searchChip += PvlKeyword("Samples", iString(ssamp));
  searchChip += PvlKeyword("Lines", iString(sline));
  if(ui.WasEntered("SMIN")) {
    searchChip += PvlKeyword("ValidMinimum", iString(ui.GetInteger("SMIN")));
  }
  if(ui.WasEntered("SMAX")) {
    searchChip += PvlKeyword("ValidMaximum", iString(ui.GetInteger("SMAX")));
  }
  if(ui.WasEntered("VALIDPERCENT")) {
    double percent = ui.GetDouble("VALIDPERCENT");
    searchChip += PvlKeyword("ValidPercent", iString(percent));

    if((percent <= 0.0) || (percent > 100.0)) {
      string msg = "VALIDPERCENT must be between 0.0 and 100.0 (Default = 50.0)";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }
  }

  // Add groups to the autoreg object
  autoreg.AddGroup(patternChip);
  autoreg.AddGroup(searchChip);

  // Set up the surface model testing group
  if(subPixelAccuracy) {
    PvlGroup surfaceModel("SurfaceModel");

    double distanceTol = ui.GetDouble("DISTANCETOLERANCE");
    surfaceModel += PvlKeyword("DistanceTolerance", iString(distanceTol));

    if(distanceTol <= 0.0) {
      string msg = "DISTANCETOLERANCE must be greater than 0.0 (Default = 1.5)";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    int winSize = ui.GetInteger("WINDOWSIZE");
    surfaceModel += PvlKeyword("WindowSize", iString(winSize));

    // Make sure the window size is at least 3
    if(winSize < 3) {
      string msg = "WINDOWSIZE must be greater than or equal to 3 (Default = 5)";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }

    // Make sure the window size is odd
    if(winSize % 2 == 0) {
      string msg = "WINDOWSIZE must be an odd number (Default = 5)";
      throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
    }

    if(ui.GetBoolean("ECCENTRICITYTESTING")) {
      double eccRatio = ui.GetDouble("ECCENTRICITYRATIO");
      surfaceModel += PvlKeyword("EccentricityRatio", iString(eccRatio));

      if(eccRatio < 1) {
        string msg = "ECCENTRICITYRATIO must be 1.0 or larger (Default = 2.0)";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
    if(ui.GetBoolean("RESIDUALTESTING")) {
      double residualTol = ui.GetDouble("RESIDUALTOLERANCE");
      surfaceModel += PvlKeyword("ResidualTolerance", iString(residualTol));

      if(residualTol < 0) {
        string msg = "RESIDUALTOLERANCE must be 0.0 or larger (Default = 0.1)";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }
    autoreg.AddGroup(surfaceModel);
  }

  // Add autoreg group to Pvl
  p.AddObject(autoreg);

  // Write the autoreg group pvl to the output file
  string output = ui.GetFilename("TO");
  p.Write(output);

  Isis::Application::GuiLog(p);
}
