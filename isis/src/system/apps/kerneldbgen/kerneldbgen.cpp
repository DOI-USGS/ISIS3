#include "Isis.h"
#include "SpiceDbGen.h"
#include "iTime.h"

using namespace Isis;
void IsisMain() {

  UserInterface &ui = Application::GetUserInterface();
  PvlGroup dependency("Dependencies");

  //create the database writer based on the kernel type
  SpiceDbGen sdg(ui.GetString("TYPE"));

  //Load the SCLK. If it exists, add its location to the dependency group
  //If there is none, set a flag so that no file is searched for
  bool needSclk = false;
  FileName sclkFile("");
  if(ui.WasEntered("SCLK")) {
    QString sclkString = ui.GetAsString("SCLK");
    if(sclkString.length() != 0) {
      sclkString.remove("\\");
      sclkFile = FileName(sclkString);
      if (sclkFile.isVersioned()) {
        sclkFile = sclkFile.highestVersion();
        sclkString = sclkFile.originalPath() + "/" + sclkFile.name();
      }
      dependency += PvlKeyword("SpacecraftClockKernel", sclkString);
      needSclk = true;
    }
  }

  QString lskString = ui.GetAsString("LSK");
  lskString.remove("\\");
  FileName lskFile(lskString);
  if (lskFile.isVersioned()) {
    lskFile = lskFile.highestVersion();
    lskString = lskFile.originalPath() + "/" + lskFile.name();
  }
  dependency += PvlKeyword("LeapsecondKernel", lskString);
  //furnish dependencies with an SCLK
  if(needSclk) {
    sdg.FurnishDependencies(sclkFile.expanded(), lskFile.expanded());
  }
  //Furnish dependencies without an SCLK
  else {
    sdg.FurnishDependencies("", lskFile.expanded());
  }

  //Determine the type of kernel that the user wants a database for. This will
  //eventually become the name of the object in the output PVL
  QString kernelType;
  if(ui.GetString("TYPE") == "CK") {
    kernelType = "SpacecraftPointing";
  }
  else if(ui.GetString("TYPE") == "SPK") {
    kernelType = "SpacecraftPosition";
  }
  PvlObject selections(kernelType);

  selections += PvlKeyword("RunTime", iTime::CurrentLocalTime());
  selections.AddGroup(dependency);

  /* Removed because Nadir is not done using this*/
  //if (ui.GetString("NADIRFILTER") != "none" &&
  //    ui.GetString("NADIRDIR") != "none"){
  //  QString location = "";
  //  location = ui.GetString("NADIRDIR");
  //  location.Trim("\\");
  //  std::vector<QString> filter;
  //  ui.GetString("NADIRFILTER", filter);
  //  PvlObject result = sdg.Direct("Nadir",location, filter);
  //  PvlObject::PvlGroupIterator grp = result.BeginGroup();
  //  while(grp != result.EndGroup()){ selections.AddGroup(*grp);grp++;}
  //}
  if(ui.GetString("PREDICTFILTER") != "none" &&
      ui.GetString("PREDICTDIR") != "none") {
    QString location = "";
    location = ui.GetString("PREDICTDIR");
    location.remove("\\");
    std::vector<QString> filter;
    ui.GetString("PREDICTFILTER", filter);
    PvlObject result = sdg.Direct("Predicted", location, filter);
    PvlObject::PvlGroupIterator grp = result.BeginGroup();
    while(grp != result.EndGroup()) {
      selections.AddGroup(*grp);
      grp++;
    }
  }

  if(ui.GetString("RECONDIR") != "none" &&
      ui.GetString("RECONFILTER") != "none") {
    QString location = "";
    location = ui.GetString("RECONDIR");
    location.remove("\\");
    std::vector<QString> filter;
    ui.GetString("RECONFILTER", filter);
    PvlObject result = sdg.Direct("Reconstructed", location, filter);
    PvlObject::PvlGroupIterator grp = result.BeginGroup();
    while(grp != result.EndGroup()) {
      selections.AddGroup(*grp);
      grp++;
    }
  }

  if(ui.GetString("SMITHEDDIR") != "none" &&
      ui.GetString("SMITHEDFILTER") != "none") {
    QString location = "";
    location = ui.GetString("SMITHEDDIR");
    location.remove("\\");
    std::vector<QString> filter;
    ui.GetString("SMITHEDFILTER", filter);
    PvlObject result = sdg.Direct("Smithed", location, filter);
    PvlObject::PvlGroupIterator grp = result.BeginGroup();
    while(grp != result.EndGroup()) {
      selections.AddGroup(*grp);
      grp++;
    }
  }

  //if (filter == ""){
  if(!ui.WasEntered("PREDICTFILTER") && !ui.WasEntered("RECONFILTER") &&
      !ui.WasEntered("SMITHEDFILTER")) {
    QString message =
      "You must enter a filter AND directory for at least one type of kernel";
    throw IException(IException::User, message, _FILEINFO_);
  }

  //specify a name for the output file
  FileName to("./kernels.????.db");
  if(ui.WasEntered("TO")) {
    to = ui.GetFileName("TO");
  }
  //create a new output version if the user specified any version sequence
  if (to.isVersioned()) {
    to = to.newVersion();
  }

  Pvl writer;
  writer.AddObject(selections);
  writer.Write(to.expanded());
}
