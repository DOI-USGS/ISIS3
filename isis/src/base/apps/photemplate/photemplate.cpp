#define GUIHELPERS

#include "Isis.h"
#include "PvlGroup.h"
#include "UserInterface.h"
#include "iString.h"

using namespace std;
using namespace Isis;

void PrintPvl();
void LoadPvl();

map <string, void *> GuiHelpers() {
  map <string, void *> helper;
  helper ["PrintPvl"] = (void *) PrintPvl;
  helper ["LoadPvl"] = (void *) LoadPvl;
  return helper;
}

//functions in the code
void addPhoModel(Pvl &pvl, Pvl &outPvl);
void addAtmosModel(Pvl &pvl, Pvl &outPvl);

// Helper function to print the input pvl file to session log
void PrintPvl() {
  UserInterface &ui = Application::GetUserInterface();

  // Write file out to log
  string inFile(ui.GetFilename("FROMPVL"));
  Pvl inPvl;
  inPvl.Read(ui.GetFilename("FROMPVL"));
  string Ostring = "***** Output of [" + inFile + "] *****";
  Application::GuiLog(Ostring);
  Application::GuiLog(inPvl);
}

// Helper function to load the input pvl file into the GUI
void LoadPvl() {
  UserInterface &ui = Application::GetUserInterface();
  string inFile(ui.GetFilename("FROMPVL"));
  Pvl inPvl;
  inPvl.Read(inFile);
  iString phtName = ui.GetAsString("PHTNAME");
  phtName = phtName.UpCase();
  if (phtName == "NONE") {
    if (inPvl.HasObject("PhotometricModel")) {
      PvlObject phtObj = inPvl.FindObject("PhotometricModel");
      if (phtObj.HasGroup("Algorithm")) {
        PvlObject::PvlGroupIterator phtGrp = phtObj.BeginGroup();
        phtName = (string)phtGrp->FindKeyword("PHTNAME");
        phtName = phtName.UpCase();
      }
    }
  }
  if (phtName != "NONE") {
    if (inPvl.HasObject("PhotometricModel")) {
      PvlObject phtObj = inPvl.FindObject("PhotometricModel");
      iString phtVal;
      if (phtObj.HasGroup("Algorithm")) {
        PvlObject::PvlGroupIterator phtGrp = phtObj.BeginGroup();
        bool wasFound = false;
        phtVal = (string)phtGrp->FindKeyword("PHTNAME");
        phtVal = phtVal.UpCase();
        if (phtName == phtVal) {
          wasFound = true;
        }
        if (ui.WasEntered("PHTNAME") && !wasFound) {
          phtName = ui.GetAsString("PHTNAME");
          phtName = phtName.UpCase();
          while (phtGrp != phtObj.EndGroup()) {
            if (phtGrp->HasKeyword("PHTNAME")) {
              phtVal = (string)phtGrp->FindKeyword("PHTNAME");
              phtVal = phtVal.UpCase();
              if (phtName == phtVal) {
                wasFound = true;
                break;
              }
            }
            phtGrp++;
          }
        }
        if (wasFound) {
          ui.Clear("PHTNAME");
          ui.Clear("THETA");
          ui.Clear("WH");
          ui.Clear("HG1");
          ui.Clear("HG2");
          ui.Clear("HH");
          ui.Clear("B0");
          ui.Clear("BH");
          ui.Clear("CH");
          ui.Clear("L");
          ui.Clear("K");
          ui.Clear("PHASELIST");
          ui.Clear("KLIST");
          ui.Clear("LLIST");
          ui.Clear("PHASECURVELIST");
          phtVal = (string)phtGrp->FindKeyword("PHTNAME");
          phtVal = phtVal.UpCase();
          if (phtVal == "HAPKEHEN" || phtVal == "HAPKELEG") {
            if (phtGrp->HasKeyword("THETA")) {
              double theta = phtGrp->FindKeyword("THETA");
              ui.PutDouble("THETA", theta);
            }
            if (phtGrp->HasKeyword("WH")) {
              double wh = phtGrp->FindKeyword("WH");
              ui.PutDouble("WH", wh);
            }
            if (phtGrp->HasKeyword("HH")) {
              double hh = phtGrp->FindKeyword("HH");
              ui.PutDouble("HH", hh);
            } 
            if (phtGrp->HasKeyword("B0")) {
              double b0 = phtGrp->FindKeyword("B0");
              ui.PutDouble("B0", b0);
            }
            if (phtVal == "HAPKEHEN") {
              if (phtGrp->HasKeyword("HG1")) {
                double hg1 = phtGrp->FindKeyword("HG1");
                ui.PutDouble("HG1", hg1);
              }
              if (phtGrp->HasKeyword("HG2")) {
                double hg2 = phtGrp->FindKeyword("HG2");
                ui.PutDouble("HG2", hg2);
              }
            }
            if (phtVal == "HAPKELEG") {
              if (phtGrp->HasKeyword("BH")) {
                double bh = phtGrp->FindKeyword("BH");
                ui.PutDouble("BH", bh);
              }
              if (phtGrp->HasKeyword("CH")) {
                double ch = phtGrp->FindKeyword("CH");
                ui.PutDouble("CH", ch);
              }
            }
          } else if (phtVal == "LUNARLAMBERTEMPIRICAL" || phtVal == "MINNAERTEMPIRICAL") {
            if (phtGrp->HasKeyword("PHASELIST")) {
              string phaselist = (string)phtGrp->FindKeyword("PHASELIST");
              ui.PutAsString("PHASELIST", phaselist);
            } 
            if (phtGrp->HasKeyword("PHASECURVELIST")) {
              string phasecurvelist = (string)phtGrp->FindKeyword("PHASECURVELIST");
              ui.PutAsString("PHASECURVELIST", phasecurvelist);
            }
            if (phtVal == "LUNARLAMBERTEMPIRICAL") {
              if (phtGrp->HasKeyword("LLIST")) {
                string llist = (string)phtGrp->FindKeyword("LLIST");
                ui.PutAsString("LLIST", llist);
              }
            }
            if (phtVal == "MINNAERTEMPIRICAL") {
              if (phtGrp->HasKeyword("KLIST")) {
                string klist = (string)phtGrp->FindKeyword("KLIST");
                ui.PutAsString("KLIST", klist);
              }
            }
          } else if (phtVal == "LUNARLAMBERT") {
            if (phtGrp->HasKeyword("L")) {
              double l = phtGrp->FindKeyword("L");
              ui.PutDouble("L", l);
            } 
          } else if (phtVal == "MINNAERT") {
            if (phtGrp->HasKeyword("K")) {
              double k = phtGrp->FindKeyword("K");
              ui.PutDouble("K", k);
            }
          } else if (phtVal != "LAMBERT" && phtVal != "LOMMELSEELIGER" &&
                     phtVal != "LUNARLAMBERTMCEWEN") {
            string message = "Unsupported photometric model [" + phtVal + "].";
            throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
          }
          ui.PutAsString("PHTNAME", phtVal);
        }
      }
    }
  }
  iString atmName = ui.GetAsString("ATMNAME");
  atmName = atmName.UpCase();
  if (atmName == "NONE") {
    if (inPvl.HasObject("AtmosphericModel")) {
      PvlObject atmObj = inPvl.FindObject("AtmosphericModel");
      if (atmObj.HasGroup("Algorithm")) {
        PvlObject::PvlGroupIterator atmGrp = atmObj.BeginGroup();
        atmName = (string)atmGrp->FindKeyword("ATMNAME");
        atmName = atmName.UpCase();
      }
    }
  }
  if (atmName == "NONE") {
    return;
  }
  if (inPvl.HasObject("AtmosphericModel")) {
    PvlObject atmObj = inPvl.FindObject("AtmosphericModel");
    iString atmVal;
    if (atmObj.HasGroup("Algorithm")) {
      PvlObject::PvlGroupIterator atmGrp = atmObj.BeginGroup();
      bool wasFound = false;
      atmVal = (string)atmGrp->FindKeyword("ATMNAME");
      atmVal = atmVal.UpCase();
      if (atmName == atmVal) {
        wasFound = true;
      }
      if (ui.WasEntered("ATMNAME") && !wasFound) {
        while (atmGrp != atmObj.EndGroup()) {
          if (atmGrp->HasKeyword("ATMNAME")) {
            atmVal = (string)atmGrp->FindKeyword("ATMNAME");
            atmVal = atmVal.UpCase();
            if (atmName == atmVal) {
              wasFound = true;
              break;
            }
          }
          atmGrp++;
        }
      }
      if (!wasFound) {
        return;
      }
      ui.Clear("ATMNAME");
      ui.Clear("HNORM");
      ui.Clear("BHA");
      ui.Clear("TAU");
      ui.Clear("TAUREF");
      ui.Clear("WHA");
      ui.Clear("HGA");
      atmVal = (string)atmGrp->FindKeyword("ATMNAME");
      atmVal = atmVal.UpCase();
      if (atmVal == "ANISOTROPIC1" || atmVal == "ANISOTROPIC2" ||
          atmVal == "HAPKEATM1" || atmVal == "HAPKEATM2" ||
          atmVal == "ISOTROPIC1" || atmVal == "ISOTROPIC2") {
        if (atmGrp->HasKeyword("HNORM")) {
          double hnorm = atmGrp->FindKeyword("HNORM");
          ui.PutDouble("HNORM", hnorm);
        }
        if (atmGrp->HasKeyword("TAU")) {
          double tau = atmGrp->FindKeyword("TAU");
          ui.PutDouble("TAU", tau);
        }
        if (atmGrp->HasKeyword("TAUREF")) {
          double tauref = atmGrp->FindKeyword("TAUREF");
          ui.PutDouble("TAUREF", tauref);
        }
        if (atmGrp->HasKeyword("WHA")) {
          double wha = atmGrp->FindKeyword("WHA");
          ui.PutDouble("WHA", wha);
        }
/*        if (atmGrp->HasKeyword("NULNEG")) {
          string nulneg = (string)atmGrp->FindKeyword("NULNEG");
          if (nulneg.compare("YES")) {
            ui.PutBoolean("NULNEG", true);
          } else {
            ui.PutBoolean("NULNEG", false);
          }
        }*/
      }
      if (atmVal == "ANISOTROPIC1" || atmVal == "ANISOTROPIC2") {
        if (atmGrp->HasKeyword("BHA")) {
          double bha = atmGrp->FindKeyword("BHA");
          ui.PutDouble("BHA", bha);
        }
      } 
      if (atmVal == "HAPKEATM1" || atmVal == "HAPKEATM2") {
        if (atmGrp->HasKeyword("HGA")) {
          double hga = atmGrp->FindKeyword("HGA");
          ui.PutDouble("HGA", hga);
        }
      }

      if (atmVal != "ANISOTROPIC1" && atmVal != "ANISOTROPIC2" &&
          atmVal != "HAPKEATM1" && atmVal != "HAPKEATM2" &&
          atmVal != "ISOTROPIC1" && atmVal != "ISOTROPIC2") {
        string message = "Unsupported atmospheric model [" + atmVal + "].";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
      ui.PutAsString("ATMNAME", atmVal);
    }
  }
}

void IsisMain() {
  // Get the output file name from the GUI and write the pvl
  // to the file. If no extension is given, '.pvl' will be used.
  UserInterface &ui = Application::GetUserInterface();
  Filename out = ui.GetFilename("TOPVL");
  string output = ui.GetFilename("TOPVL");
  if(out.Extension() == "") {
    output += ".pvl";
  }

  //The PVL to be written out
  Pvl p;
  Pvl op;

  if (ui.WasEntered("FROMPVL")) {
    string input = ui.GetFilename("FROMPVL");

    p.Read(input);
  }

  //Add the different models to the PVL
  if (ui.GetAsString("PHTNAME") != "NONE") {
    addPhoModel(p, op);
  }

  if (ui.GetAsString("ATMNAME") != "NONE") {
    addAtmosModel(p, op);
  }

  op.Write(output);
}

//Function to add photometric model to the PVL
void addPhoModel(Pvl &pvl, Pvl &outPvl) {
  UserInterface &ui = Application::GetUserInterface();

  bool wasFound = false;
  //Create an object for the photometric model
  PvlObject phoModel("PhotometricModel");
  //Create an algorithm group
  PvlGroup phoAlgo("Algorithm");
  PvlObject::PvlGroupIterator phtGrp;
  PvlObject phtObj;
  if (pvl.HasObject("PhotometricModel")) {
    phtObj = pvl.FindObject("PhotometricModel");
    if (phtObj.HasGroup("Algorithm")) {
      phtGrp = phtObj.BeginGroup();
      if (ui.WasEntered("PHTNAME")) {
        iString phtName = ui.GetAsString("PHTNAME");
        phtName = phtName.UpCase();
        int index = 0;
        while (phtGrp != phtObj.EndGroup()) {
          if (phtGrp->HasKeyword("PHTNAME")) {
            iString phtVal = (string)phtGrp->FindKeyword("PHTNAME");
            phtVal = phtVal.UpCase();
            if (phtName == phtVal) {
              phtObj.DeleteGroup(index);
              wasFound = true;
            }
          }
          phtGrp++;
          index++;
        }
      }
    }
  } 

  //Get the photometric model and any parameters specific to that
  //model and write it to the algorithm group

  //Hapke Henyey Greenstein Photometric Model
  if(ui.GetString("PHTNAME") == "HAPKEHEN") {
    double theta = ui.GetDouble("THETA");
    double wh = ui.GetDouble("WH");
    double hg1 = ui.GetDouble("HG1");
    double hg2 = ui.GetDouble("HG2");
    double hh = ui.GetDouble("HH");
    double b0 = ui.GetDouble("B0");

    if (!wasFound) {
      phoAlgo.AddKeyword(PvlKeyword("PhtName", "Hapkehen"));
      phoAlgo.AddKeyword(PvlKeyword("Theta", theta));
      phoAlgo.AddKeyword(PvlKeyword("Wh", wh));
      phoAlgo.AddKeyword(PvlKeyword("Hg1", hg1));
      phoAlgo.AddKeyword(PvlKeyword("Hg2", hg2));
      phoAlgo.AddKeyword(PvlKeyword("Hh", hh));
      phoAlgo.AddKeyword(PvlKeyword("B0", b0));
    } else {
      PvlGroup pg("Algorithm");
      pg.AddKeyword(PvlKeyword("PhtName", "HapkeHen"));
      pg.AddKeyword(PvlKeyword("Theta", theta));
      pg.AddKeyword(PvlKeyword("Wh", wh));
      pg.AddKeyword(PvlKeyword("Hg1", hg1));
      pg.AddKeyword(PvlKeyword("Hg2", hg2));
      pg.AddKeyword(PvlKeyword("Hh", hh));
      pg.AddKeyword(PvlKeyword("B0", b0));
      phtObj.AddGroup(pg);
    }
  }
  //Hapke Legendre Photometric Model
  else if(ui.GetString("PHTNAME") == "HAPKELEG") {
    double theta = ui.GetDouble("THETA");
    double wh = ui.GetDouble("WH");
    double bh = ui.GetDouble("BH");
    double ch = ui.GetDouble("CH");
    double hh = ui.GetDouble("HH");
    double b0 = ui.GetDouble("B0");

    if (!wasFound) {
      phoAlgo.AddKeyword(PvlKeyword("PhtName", "HapkeLeg"));
      phoAlgo.AddKeyword(PvlKeyword("Theta", theta));
      phoAlgo.AddKeyword(PvlKeyword("Wh", wh));
      phoAlgo.AddKeyword(PvlKeyword("Bh", bh));
      phoAlgo.AddKeyword(PvlKeyword("Ch", ch));
      phoAlgo.AddKeyword(PvlKeyword("Hh", hh));
      phoAlgo.AddKeyword(PvlKeyword("B0", b0));
    } else {
      PvlGroup pg("Algorithm");
      pg.AddKeyword(PvlKeyword("PhtName", "HapkeLeg"));
      pg.AddKeyword(PvlKeyword("Theta", theta));
      pg.AddKeyword(PvlKeyword("Wh", wh));
      pg.AddKeyword(PvlKeyword("Bh", bh));
      pg.AddKeyword(PvlKeyword("Ch", ch));
      pg.AddKeyword(PvlKeyword("Hh", hh));
      pg.AddKeyword(PvlKeyword("B0", b0));
      phtObj.AddGroup(pg);
    }
  }
  //Lunar Lambert Photometric Model
  else if(ui.GetString("PHTNAME") == "LUNARLAMBERT") {
    double l = ui.GetDouble("L");

    if (!wasFound) {
      phoAlgo.AddKeyword(PvlKeyword("PhtName", "LunarLambert"));
      phoAlgo.AddKeyword(PvlKeyword("L", l));
    } else {
      PvlGroup pg("Algorithm");
      pg.AddKeyword(PvlKeyword("PhtName", "LunarLambert"));
      pg.AddKeyword(PvlKeyword("L", l));
      phtObj.AddGroup(pg);
    }
  }
  //Lunar Lambert Empirical Photometric Model
  else if(ui.GetString("PHTNAME") == "LUNARLAMBERTEMPIRICAL") {
    string phaselist = ui.GetString("PHASELIST");
    string llist = ui.GetString("LLIST");
    string phasecurvelist = ui.GetString("PHASECURVELIST");

    if (!wasFound) {
      phoAlgo.AddKeyword(PvlKeyword("PhtName", "LunarLambertEmpirical"));
      phoAlgo.AddKeyword(PvlKeyword("PhaseList", phaselist));
      phoAlgo.AddKeyword(PvlKeyword("LList", llist));
      phoAlgo.AddKeyword(PvlKeyword("PhaseCurveList", phasecurvelist));
    } else {
      PvlGroup pg("Algorithm");
      pg.AddKeyword(PvlKeyword("PhtName", "LunarLambertEmpirical"));
      pg.AddKeyword(PvlKeyword("PhaseList", phaselist));
      pg.AddKeyword(PvlKeyword("LList", llist));
      pg.AddKeyword(PvlKeyword("PhaseCurveList", phasecurvelist));
      phtObj.AddGroup(pg);
    }
  }
  //Minnaert Photometric Model
  else if(ui.GetString("PHTNAME") == "MINNAERT") {
    double k = ui.GetDouble("K");

    if (!wasFound) {
      phoAlgo.AddKeyword(PvlKeyword("PhtName", "Minnaert"));
      phoAlgo.AddKeyword(PvlKeyword("K", k));
    } else {
      PvlGroup pg("Algorithm");
      pg.AddKeyword(PvlKeyword("PhtName", "Minnaert"));
      pg.AddKeyword(PvlKeyword("K", k));
      phtObj.AddGroup(pg);
    }
  }
  //Minnaert Empirical Photometric Model
  else if(ui.GetString("PHTNAME") == "MINNAERTEMPIRICAL") {
    string phaselist = ui.GetString("PHASELIST");
    string klist = ui.GetString("KLIST");
    string phasecurvelist = ui.GetString("PHASECURVELIST");

    if (!wasFound) {
      phoAlgo.AddKeyword(PvlKeyword("PhtName", "MinnaertEmpirical"));
      phoAlgo.AddKeyword(PvlKeyword("PhaseList", phaselist));
      phoAlgo.AddKeyword(PvlKeyword("KList", klist));
      phoAlgo.AddKeyword(PvlKeyword("PhaseCurveList", phasecurvelist));
    } else {
      PvlGroup pg("Algorithm");
      pg.AddKeyword(PvlKeyword("PhtName", "MinnaertEmpirical"));
      pg.AddKeyword(PvlKeyword("PhaseList", phaselist));
      pg.AddKeyword(PvlKeyword("KList", klist));
      pg.AddKeyword(PvlKeyword("PhaseCurveList", phasecurvelist));
      phtObj.AddGroup(pg);
    }
  }
  //Lambert Photometric Model
  else if(ui.GetString("PHTNAME") == "LAMBERT") {
    if (!wasFound) {
      phoAlgo.AddKeyword(PvlKeyword("PhtName", "Lambert"));
    } else {
      PvlGroup pg("Algorithm");
      pg.AddKeyword(PvlKeyword("PhtName", "Lambert"));
      phtObj.AddGroup(pg);
    }
  }
  //Lommel Seeliger Photometric Model
  else if(ui.GetString("PHTNAME") == "LOMMELSEELIGER") {
    if (!wasFound) {
      phoAlgo.AddKeyword(PvlKeyword("PhtName", "LommelSeeliger"));
    } else {
      PvlGroup pg("Algorithm");
      pg.AddKeyword(PvlKeyword("PhtName", "LommelSeeliger"));
      phtObj.AddGroup(pg);
    }
  }
  //Lunar Lambert McEwen Photometric Model
  else if(ui.GetString("PHTNAME") == "LUNARLAMBERTMCEWEN") {
    if (!wasFound) {
      phoAlgo.AddKeyword(PvlKeyword("PhtName", "LunarLambertMcEwen"));
    } else {
      PvlGroup pg("Algorithm");
      pg.AddKeyword(PvlKeyword("PhtName", "LunarLambertMcEwen"));
      phtObj.AddGroup(pg);
    }
  }

  //Add the algorithm group to the photometric model object and add it to the PVL
  if (!wasFound) {
    phoModel.AddGroup(phoAlgo);
    outPvl.AddObject(phoModel);
  } else {
    outPvl.AddObject(phtObj);
  }
}

//Function to add atmospheric model to the PVL
void addAtmosModel(Pvl &pvl, Pvl &outPvl) {
  UserInterface &ui = Application::GetUserInterface();

  bool wasFound = false;
  //Create an object for the atmospheric model
  PvlObject atmosModel("AtmosphericModel");
  //Create an algorithm group
  PvlGroup atmosAlgo("Algorithm");
  PvlObject::PvlGroupIterator atmGrp;
  PvlObject atmObj;
  if (pvl.HasObject("AtmosphericModel")) {
    atmObj = pvl.FindObject("AtmosphericModel");
    if (atmObj.HasGroup("Algorithm")) {
      atmGrp = atmObj.BeginGroup();
      if (ui.WasEntered("ATMNAME")) {
        iString atmName = ui.GetAsString("ATMNAME");
        atmName = atmName.UpCase();
        int index = 0;
        while (atmGrp != atmObj.EndGroup()) {
          if (atmGrp->HasKeyword("ATMNAME")) {
            iString atmVal = (string)atmGrp->FindKeyword("ATMNAME");
            atmVal = atmVal.UpCase();
            if (atmName == atmVal) {
              atmObj.DeleteGroup(index);
              wasFound = true;
            }
          }
          atmGrp++;
          index++;
        }
      }
    }
  } 

  //Get the atmospheric model and any parameters specific to that
  //model and write it to the algorithm group

  //Anisotropic 1 Atmospheric Model
  if(ui.GetString("ATMNAME") == "ANISOTROPIC1") {
    double tau = ui.GetDouble("TAU");
    double tauref = ui.GetDouble("TAUREF");
    double wha = ui.GetDouble("WHA");
    double bha = ui.GetDouble("BHA");
    double hnorm = ui.GetDouble("HNORM");

    if (!wasFound) {
      atmosAlgo.AddKeyword(PvlKeyword("AtmName", "Anisotropic1"));
      atmosAlgo.AddKeyword(PvlKeyword("Tau", tau));
      atmosAlgo.AddKeyword(PvlKeyword("Tauref", tauref));
      atmosAlgo.AddKeyword(PvlKeyword("Wha", wha));
      atmosAlgo.AddKeyword(PvlKeyword("Bha", bha));
      atmosAlgo.AddKeyword(PvlKeyword("Hnorm", hnorm));
    } else {
      PvlGroup pg("Algorithm");
      pg.AddKeyword(PvlKeyword("AtmName", "Anisotropic1"));
      pg.AddKeyword(PvlKeyword("Tau", tau));
      pg.AddKeyword(PvlKeyword("Tauref", tauref));
      pg.AddKeyword(PvlKeyword("Wha", wha));
      pg.AddKeyword(PvlKeyword("Bha", bha));
      pg.AddKeyword(PvlKeyword("Hnorm", hnorm));
      atmObj.AddGroup(pg);
    }
  }
  //Anisotropic 2 Atmospheric Model
  else if(ui.GetString("ATMNAME") == "ANISOTROPIC2") {
    double tau = ui.GetDouble("TAU");
    double tauref = ui.GetDouble("TAUREF");
    double wha = ui.GetDouble("WHA");
    double bha = ui.GetDouble("BHA");
    double hnorm = ui.GetDouble("HNORM");

    if (!wasFound) {
      atmosAlgo.AddKeyword(PvlKeyword("AtmName", "Anisotropic2"));
      atmosAlgo.AddKeyword(PvlKeyword("Tau", tau));
      atmosAlgo.AddKeyword(PvlKeyword("Tauref", tauref));
      atmosAlgo.AddKeyword(PvlKeyword("Wha", wha));
      atmosAlgo.AddKeyword(PvlKeyword("Bha", bha));
      atmosAlgo.AddKeyword(PvlKeyword("Hnorm", hnorm));
    } else {
      PvlGroup pg("Algorithm");
      pg.AddKeyword(PvlKeyword("AtmName", "Anisotropic2"));
      pg.AddKeyword(PvlKeyword("Tau", tau));
      pg.AddKeyword(PvlKeyword("Tauref", tauref));
      pg.AddKeyword(PvlKeyword("Wha", wha));
      pg.AddKeyword(PvlKeyword("Bha", bha));
      pg.AddKeyword(PvlKeyword("Hnorm", hnorm));
      atmObj.AddGroup(pg);
    }

  }
  //Hapke 1 Atmospheric Model
  else if(ui.GetString("ATMNAME") == "HAPKEATM1") {
    double tau = ui.GetDouble("TAU");
    double tauref = ui.GetDouble("TAUREF");
    double wha = ui.GetDouble("WHA");
    double hga = ui.GetDouble("HGA");
    double hnorm = ui.GetDouble("HNORM");

    if (!wasFound) {
      atmosAlgo.AddKeyword(PvlKeyword("AtmName", "HapkeAtm1"));
      atmosAlgo.AddKeyword(PvlKeyword("Tau", tau));
      atmosAlgo.AddKeyword(PvlKeyword("Tauref", tauref));
      atmosAlgo.AddKeyword(PvlKeyword("Wha", wha));
      atmosAlgo.AddKeyword(PvlKeyword("Hga", hga));
      atmosAlgo.AddKeyword(PvlKeyword("Hnorm", hnorm));
    } else {
      PvlGroup pg("Algorithm");
      pg.AddKeyword(PvlKeyword("AtmName", "HapkeAtm1"));
      pg.AddKeyword(PvlKeyword("Tau", tau));
      pg.AddKeyword(PvlKeyword("Tauref", tauref));
      pg.AddKeyword(PvlKeyword("Wha", wha));
      pg.AddKeyword(PvlKeyword("Hga", hga));
      pg.AddKeyword(PvlKeyword("Hnorm", hnorm));
      atmObj.AddGroup(pg);
    }
  }
  //Hapke 2 Atmospheric Model
  else if(ui.GetString("ATMNAME") == "HAPKEATM2") {
    double tau = ui.GetDouble("TAU");
    double tauref = ui.GetDouble("TAUREF");
    double wha = ui.GetDouble("WHA");
    double hga = ui.GetDouble("HGA");
    double hnorm = ui.GetDouble("HNORM");

    if (!wasFound) {
      atmosAlgo.AddKeyword(PvlKeyword("AtmName", "HapkeAtm2"));
      atmosAlgo.AddKeyword(PvlKeyword("Tau", tau));
      atmosAlgo.AddKeyword(PvlKeyword("Tauref", tauref));
      atmosAlgo.AddKeyword(PvlKeyword("Wha", wha));
      atmosAlgo.AddKeyword(PvlKeyword("Hga", hga));
      atmosAlgo.AddKeyword(PvlKeyword("Hnorm", hnorm));
    } else {
      PvlGroup pg("Algorithm");
      pg.AddKeyword(PvlKeyword("AtmName", "HapkeAtm2"));
      pg.AddKeyword(PvlKeyword("Tau", tau));
      pg.AddKeyword(PvlKeyword("Tauref", tauref));
      pg.AddKeyword(PvlKeyword("Wha", wha));
      pg.AddKeyword(PvlKeyword("Hga", hga));
      pg.AddKeyword(PvlKeyword("Hnorm", hnorm));
      atmObj.AddGroup(pg);
    }
  }
  //Isotropic 1 Atmospheric Model
  else if(ui.GetString("ATMNAME") == "ISOTROPIC1") {
    double tau = ui.GetDouble("TAU");
    double tauref = ui.GetDouble("TAUREF");
    double wha = ui.GetDouble("WHA");
    double hnorm = ui.GetDouble("HNORM");

    if (!wasFound) {
      atmosAlgo.AddKeyword(PvlKeyword("AtmName", "Isotropic1"));
      atmosAlgo.AddKeyword(PvlKeyword("Tau", tau));
      atmosAlgo.AddKeyword(PvlKeyword("Tauref", tauref));
      atmosAlgo.AddKeyword(PvlKeyword("Wha", wha));
      atmosAlgo.AddKeyword(PvlKeyword("Hnorm", hnorm));
    } else {
      PvlGroup pg("Algorithm");
      pg.AddKeyword(PvlKeyword("AtmName", "Isotropic1"));
      pg.AddKeyword(PvlKeyword("Tau", tau));
      pg.AddKeyword(PvlKeyword("Tauref", tauref));
      pg.AddKeyword(PvlKeyword("Wha", wha));
      pg.AddKeyword(PvlKeyword("Hnorm", hnorm));
      atmObj.AddGroup(pg);
    }
  }
  //Isotropic 2 Atmospheric Model
  else if(ui.GetString("ATMNAME") == "ISOTROPIC2") {
    double tau = ui.GetDouble("TAU");
    double tauref = ui.GetDouble("TAUREF");
    double wha = ui.GetDouble("WHA");
    double hnorm = ui.GetDouble("HNORM");

    if (!wasFound) {
      atmosAlgo.AddKeyword(PvlKeyword("AtmName", "Isotropic2"));
      atmosAlgo.AddKeyword(PvlKeyword("Tau", tau));
      atmosAlgo.AddKeyword(PvlKeyword("Tauref", tauref));
      atmosAlgo.AddKeyword(PvlKeyword("Wha", wha));
      atmosAlgo.AddKeyword(PvlKeyword("Hnorm", hnorm));
    } else {
      PvlGroup pg("Algorithm");
      pg.AddKeyword(PvlKeyword("AtmName", "Isotropic2"));
      pg.AddKeyword(PvlKeyword("Tau", tau));
      pg.AddKeyword(PvlKeyword("Tauref", tauref));
      pg.AddKeyword(PvlKeyword("Wha", wha));
      pg.AddKeyword(PvlKeyword("Hnorm", hnorm));
      atmObj.AddGroup(pg);
    }
  }

  //Add the algorithm group to the atmospheric model object and add it to the PVL
  if (!wasFound) {
    atmosModel.AddGroup(atmosAlgo);
    outPvl.AddObject(atmosModel);
  } else {
    outPvl.AddObject(atmObj);
  }
}
