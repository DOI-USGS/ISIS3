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
  QString algoName = ui.GetString("ALGORITHM");
  if(!algos.hasGroup(algoName.toStdString())) {
    // Give the user a list of possible algorithms
    std::string msg = "Invalid value for [ALGORITHM] entered [" + algoName.toStdString() + "].  "
        + "Must be one of [";

    for(int i = 0; i < algos.groups(); i++) {
      if(i != 0 &&
         algos.group(i).name() == algos.group(0).name()) break;

      if(i != 0) msg += ", ";
      msg += algos.group(i).name();
    }
    msg += "]";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Make algorithm group
  PvlGroup algorithm("Algorithm");
  algorithm += PvlKeyword("Name", algoName.toStdString());

  // Set the tolerance
  double tol = ui.GetDouble("TOLERANCE");
  algorithm += PvlKeyword("Tolerance", std::to_string(tol));

  // Set the reduction factor if the user entered it
  if(ui.WasEntered("REDUCTIONFACTOR")) {
    int reduction = ui.GetInteger("REDUCTIONFACTOR");
    algorithm += PvlKeyword("ReductionFactor", std::to_string(reduction));

    if(reduction < 1) {
      std::string msg = "Invalid value for [REDUCTIONFACTOR] entered ["
        + std::to_string(reduction) + "].  Must be greater than or equal to 1 (Default = 1)";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }

  bool subPixelAccuracy = ui.GetBoolean("SUBPIXELACCURACY");
  algorithm += PvlKeyword("SubpixelAccuracy", (subPixelAccuracy) ? "True" : "False");

  // Set the chip interpolator type
  QMap<QString, QString> paramToInterpMap;
  paramToInterpMap["NEARESTNEIGHBORTYPE"] = "NearestNeighborType";
  paramToInterpMap["BILINEARTYPE"] = "BiLinearType";
  paramToInterpMap["CUBICCONVOLUTIONTYPE"] = "CubicConvolutionType";
  
  algorithm += PvlKeyword("ChipInterpolator", paramToInterpMap[ui.GetString("INTERP")].toStdString());

  // Add algorithm group to the autoreg object
  autoreg.addGroup(algorithm);

  // Get pattern and search chip size values for error testing
  int psamp = ui.GetInteger("PSAMP");
  int pline = ui.GetInteger("PLINE");
  int ssamp = ui.GetInteger("SSAMP");
  int sline = ui.GetInteger("SLINE");

  // Make sure the pattern chip is not just one pixel
  if(psamp + pline < 3) {
    QString msg = "The Pattern Chip must be larger than one pixel for the ";
    msg += "autoregistration to work properly";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  // Make sure the pattern chip is smaller than the search chip
  if(ssamp < psamp || sline < pline) {
    QString msg = "The Pattern Chip must be smaller than the Search Chip";
    throw IException(IException::User, msg, _FILEINFO_);
  }
  // Make sure the pattern chip spans at least a 3x3 window in the search chip
  if(psamp + 2 > ssamp || pline + 2 > sline) {
    QString msg = "The Pattern Chip must span at least a 3x3 window in the ";
    msg += "Search Chip";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Set up the pattern chip group
  PvlGroup patternChip("PatternChip");
  patternChip += PvlKeyword("Samples", std::to_string(psamp));
  patternChip += PvlKeyword("Lines", std::to_string(pline));
  if(ui.WasEntered("PMIN")) {
    patternChip += PvlKeyword("ValidMinimum", std::to_string(ui.GetInteger("PMIN")));
  }
  if(ui.WasEntered("PMAX")) {
    patternChip += PvlKeyword("ValidMaximum", std::to_string(ui.GetInteger("PMAX")));
  }
  if(ui.WasEntered("MINIMUMZSCORE")) {
    double minimum = ui.GetDouble("MINIMUMZSCORE");
    patternChip += PvlKeyword("MinimumZScore", std::to_string(minimum));

    if(minimum <= 0.0) {
      std::string msg = "Invalid value for [MINIMUMZSCORE] entered ["
        + std::to_string(minimum) + "].  Must be greater than 0.0 (Default = 1.0)";
      throw IException(IException::User, msg, _FILEINFO_);
    }
  }
  if(ui.WasEntered("PVALIDPERCENT")) {
    double percent = ui.GetDouble("PVALIDPERCENT");
    if((percent <= 0.0) || (percent > 100.0)) {
      std::string msg = "Invalid value for [PVALIDPERCENT] entered ["
        + std::to_string(percent) + "].  Must be greater than 0.0 and less than or equal to 100.0 (Default = 50.0)";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    patternChip += PvlKeyword("ValidPercent", std::to_string(percent));
  }

  // Set up the search chip group
  PvlGroup searchChip("SearchChip");
  searchChip += PvlKeyword("Samples", std::to_string(ssamp));
  searchChip += PvlKeyword("Lines", std::to_string(sline));
  if(ui.WasEntered("SMIN")) {
    searchChip += PvlKeyword("ValidMinimum", std::to_string(ui.GetInteger("SMIN")));
  }
  if(ui.WasEntered("SMAX")) {
    searchChip += PvlKeyword("ValidMaximum", std::to_string(ui.GetInteger("SMAX")));
  }
  if(ui.WasEntered("SSUBCHIPVALIDPERCENT")) {
    double percent = ui.GetDouble("SSUBCHIPVALIDPERCENT");
    if((percent <= 0.0) || (percent > 100.0)) {
      std::string msg = "Invalid value for [SSUBCHIPVALIDPERCENT] entered ["
        + std::to_string(percent) + "].  Must be greater than 0.0 and less than or equal to 100.0 (Default = 50.0)";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    searchChip += PvlKeyword("SubchipValidPercent", std::to_string(percent));
  }

  // Add groups to the autoreg object
  autoreg.addGroup(patternChip);
  autoreg.addGroup(searchChip);

  // Set up the surface model testing group
  if(subPixelAccuracy) {
    PvlGroup surfaceModel("SurfaceModel");

    double distanceTol = ui.GetDouble("DISTANCETOLERANCE");
    surfaceModel += PvlKeyword("DistanceTolerance", std::to_string(distanceTol));

    if(distanceTol <= 0.0) {
      std::string msg = "Invalid value for [DISTANCETOLERANCE] entered ["
        + std::to_string(distanceTol) + "].  Must be greater than 0.0 (Default = 1.5)";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    int winSize = ui.GetInteger("WINDOWSIZE");
    surfaceModel += PvlKeyword("WindowSize", std::to_string(winSize));

    // Make sure the window size is odd
    if(winSize % 2 == 0) {
      std::string msg = "Invalid value for [WINDOWSIZE] entered ["
        + std::to_string(winSize) + "].  Must be an odd number (Default = 5)";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    autoreg.addGroup(surfaceModel);
  }

  // Add autoreg group to Pvl
  p.addObject(autoreg);

  // Write the autoreg group pvl to the output file
  QString output = ui.GetFileName("TOPVL");
  p.write(output.toStdString());

  Isis::Application::GuiLog(p);
}
