#define GUIHELPERS

#include "Isis.h"
#include <string>
#include "Camera.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "Photometry.h"
#include "Pvl.h"
#include "Cube.h"

#include "PvlGroup.h"
#include "iException.h"

#include "Angle.h"

void PrintPvl();
void LoadPvl();

map <string, void *> GuiHelpers() {
  map <string, void *> helper;
  helper ["PrintPvl"] = (void *) PrintPvl;
  helper ["LoadPvl"] = (void *) LoadPvl;
  return helper;
}

using namespace std;
using namespace Isis;

// Global variables
Camera *cam;
Cube *icube;
Photometry *pho;
double maxema;
double maxinc;
string angleSource;
double centerPhase;
double centerIncidence;
double centerEmission;

void photomet(Buffer &in, Buffer &out);

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
            if (phtGrp->HasKeyword("PHTNAME") || phtGrp->HasKeyword("NAME")) {
              if (phtGrp->HasKeyword("PHTNAME")) phtVal = (string)phtGrp->FindKeyword("PHTNAME");
              else phtVal = (string)phtGrp->FindKeyword("NAME");
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
          if (phtGrp->HasKeyword("PHTNAME")) phtVal = (string)phtGrp->FindKeyword("PHTNAME");
          else phtVal = (string)phtGrp->FindKeyword("NAME");
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

  iString nrmName = ui.GetAsString("NORMNAME");
  nrmName = nrmName.UpCase();
  iString nrmVal;
  if (nrmName == "NONE") {
    if (inPvl.HasObject("NormalizationModel")) {
      PvlObject nrmObj = inPvl.FindObject("NormalizationModel");
      if (nrmObj.HasGroup("Algorithm")) {
        PvlObject::PvlGroupIterator nrmGrp = nrmObj.BeginGroup();
        nrmName = (string)nrmGrp->FindKeyword("NORMNAME");
        nrmName = nrmName.UpCase();
      }
    }
  }
  if (nrmName != "NONE") {
    if (inPvl.HasObject("NormalizationModel")) {
      PvlObject nrmObj = inPvl.FindObject("NormalizationModel");
      if (nrmObj.HasGroup("Algorithm")) {
        PvlObject::PvlGroupIterator nrmGrp = nrmObj.BeginGroup();
        bool wasFound = false;
        nrmVal = (string)nrmGrp->FindKeyword("NORMNAME");
        nrmVal = nrmVal.UpCase();
        if (nrmName == nrmVal) {
          wasFound = true;
        }
        if (ui.WasEntered("NORMNAME") && !wasFound) {
          while (nrmGrp != nrmObj.EndGroup()) {
            if (nrmGrp->HasKeyword("NORMNAME") || nrmGrp->HasKeyword("NAME")) {
              if (nrmGrp->HasKeyword("NORMNAME")) nrmVal = (string)nrmGrp->FindKeyword("NORMNAME");
              else nrmVal = (string)nrmGrp->FindKeyword("NAME");
              nrmVal = nrmVal.UpCase();
              if (nrmName == nrmVal) {
                wasFound = true;
                break;
              }
            }
            nrmGrp++;
          }
        }
        if (wasFound) {
          if (nrmName != "ALBEDOATM" && nrmName != "SHADEATM" && nrmName != "TOPOATM") {
            ui.Clear("ATMNAME");
          }  
          ui.Clear("NORMNAME");
          ui.Clear("INCREF");
          ui.Clear("INCMAT");
          ui.Clear("THRESH");
          ui.Clear("ALBEDO");
          ui.Clear("D");
          ui.Clear("E");
          ui.Clear("F");
          ui.Clear("G2");
          ui.Clear("XMUL");
          ui.Clear("WL");
          ui.Clear("H");
          ui.Clear("BSH1");
          ui.Clear("XB1");
          ui.Clear("XB2");
          if (nrmGrp->HasKeyword("NORMNAME")) nrmVal = (string)nrmGrp->FindKeyword("NORMNAME");
          else nrmVal = (string)nrmGrp->FindKeyword("NAME");
          nrmVal = nrmVal.UpCase();
          if (nrmVal != "MOONALBEDO") {
            if (nrmVal == "ALBEDO" || nrmVal == "MIXED") {
              if (nrmGrp->HasKeyword("INCREF")) {
                double incref = nrmGrp->FindKeyword("INCREF");
                ui.PutDouble("INCREF", incref);
              } 
              if (nrmGrp->HasKeyword("INCMAT")) {
                double incmat = nrmGrp->FindKeyword("INCMAT");
                ui.PutDouble("INCMAT", incmat);
              }
              if (nrmGrp->HasKeyword("THRESH")) {
                double thresh = nrmGrp->FindKeyword("THRESH");
                ui.PutDouble("THRESH", thresh);
              }
              if (nrmGrp->HasKeyword("ALBEDO")) {
                double albedo = nrmGrp->FindKeyword("ALBEDO");
                ui.PutDouble("ALBEDO", albedo);
              }  
            } else if (nrmVal == "SHADE") {
              if (nrmGrp->HasKeyword("INCREF")) {
                double incref = nrmGrp->FindKeyword("INCREF");
                ui.PutDouble("INCREF", incref);
              }
              if (nrmGrp->HasKeyword("ALBEDO")) {
                double albedo = nrmGrp->FindKeyword("ALBEDO");
                ui.PutDouble("ALBEDO", albedo);
              }
            } else if (nrmVal == "TOPO") {
              if (nrmGrp->HasKeyword("INCREF")) {
                double incref = nrmGrp->FindKeyword("INCREF");
                ui.PutDouble("INCREF", incref);
              }
              if (nrmGrp->HasKeyword("ALBEDO")) {
                double albedo = nrmGrp->FindKeyword("ALBEDO");
                ui.PutDouble("ALBEDO", albedo);
              }
              if (nrmGrp->HasKeyword("THRESH")) {
                double thresh = nrmGrp->FindKeyword("THRESH");
                ui.PutDouble("THRESH", thresh);
              }
            } else if (nrmVal == "ALBEDOATM") {
              if (nrmGrp->HasKeyword("INCREF")) {
                double incref = nrmGrp->FindKeyword("INCREF");
                ui.PutDouble("INCREF", incref);
              }
            } else if (nrmVal == "SHADEATM") {
              if (nrmGrp->HasKeyword("INCREF")) {
                double incref = nrmGrp->FindKeyword("INCREF");
                ui.PutDouble("INCREF", incref);
              }
              if (nrmGrp->HasKeyword("ALBEDO")) {
                double albedo = nrmGrp->FindKeyword("ALBEDO");
                ui.PutDouble("ALBEDO", albedo);
              }
            } else if (nrmVal == "TOPOATM") {
              if (nrmGrp->HasKeyword("INCREF")) {
                double incref = nrmGrp->FindKeyword("INCREF");
                ui.PutDouble("INCREF", incref);
              } 
              if (nrmGrp->HasKeyword("ALBEDO")) {
                double albedo = nrmGrp->FindKeyword("ALBEDO");
                ui.PutDouble("ALBEDO", albedo);
              }
            } else {
              string message = "Unsupported normalization model [" + nrmVal + "].";
              throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
            }
          } else {
            if (nrmGrp->HasKeyword("D")) {
              double d = nrmGrp->FindKeyword("D");
              ui.PutDouble("D", d);
            }
            if (nrmGrp->HasKeyword("E")) {
              double e = nrmGrp->FindKeyword("E");
              ui.PutDouble("E", e);
            }
            if (nrmGrp->HasKeyword("F")) {
              double f = nrmGrp->FindKeyword("F");
              ui.PutDouble("F", f);
            }
            if (nrmGrp->HasKeyword("G2")) {
              double g2 = nrmGrp->FindKeyword("G2");
              ui.PutDouble("G2", g2);
            }
            if (nrmGrp->HasKeyword("XMUL")) {
              double xmul = nrmGrp->FindKeyword("XMUL");
              ui.PutDouble("XMUL", xmul);
            }
            if (nrmGrp->HasKeyword("WL")) {
              double wl = nrmGrp->FindKeyword("WL");
              ui.PutDouble("WL", wl);
            }
            if (nrmGrp->HasKeyword("H")) {
              double h = nrmGrp->FindKeyword("H");
              ui.PutDouble("H", h);
            }
            if (nrmGrp->HasKeyword("BSH1")) {
              double bsh1 = nrmGrp->FindKeyword("BSH1");
              ui.PutDouble("BSH1", bsh1);
            }
            if (nrmGrp->HasKeyword("XB1")) {
              double xb1 = nrmGrp->FindKeyword("XB1");
              ui.PutDouble("XB1", xb1);
            }
            if (nrmGrp->HasKeyword("XB2")) {
              double xb2 = nrmGrp->FindKeyword("XB2");
              ui.PutDouble("XB2", xb2);
            }
          }
          ui.PutAsString("NORMNAME", nrmVal);
        }
      }
    }
  }

  if (nrmName != "ALBEDOATM" && nrmName != "SHADEATM" && nrmName != "TOPOATM") {
    return;
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
  if (atmName != "NONE") {
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
            if (atmGrp->HasKeyword("ATMNAME") || atmGrp->HasKeyword("NAME")) {
              if (atmGrp->HasKeyword("ATMNAME")) atmVal = (string)atmGrp->FindKeyword("ATMNAME");
              else atmVal = (string)atmGrp->FindKeyword("NAME");
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
        if (atmGrp->HasKeyword("ATMNAME")) atmVal = (string)atmGrp->FindKeyword("ATMNAME");
        else atmVal = (string)atmGrp->FindKeyword("NAME");
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
          if (atmGrp->HasKeyword("NULNEG")) {
            string nulneg = (string)atmGrp->FindKeyword("NULNEG");
            if (nulneg.compare("YES")) {
              ui.PutBoolean("NULNEG", true);
            } else {
              ui.PutBoolean("NULNEG", false);
            }
          }
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

}

void IsisMain() {
  // We will be processing by line
  ProcessByLine p;

  // Set up the user interface
  UserInterface &ui = Application::GetUserInterface();

  Pvl toNormPvl;
  iString normName;
  // Check to make sure that a normalization model was specified
  if (ui.GetAsString("NORMNAME") == "NONE") {
    if (!ui.WasEntered("FROMPVL")) {
      string message = "A Normalization model must be specified before running this program.";
      throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
    } else {
      Pvl fromNormPvl; 
      PvlObject fromNormObj;
      PvlGroup fromNormGrp;
      string input = ui.GetFilename("FROMPVL");
      fromNormPvl.Read(input);
      if (!fromNormPvl.HasObject("NormalizationModel")) {
        string message = "A Normalization model must be specified before running this program.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
      fromNormObj = fromNormPvl.FindObject("NormalizationModel");
      if (!fromNormObj.HasGroup("Algorithm")) {
        string message = "A Normalization model must be specified before running this program.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      } else {
        fromNormGrp = fromNormObj.FindGroup("Algorithm");
        if (fromNormGrp.HasKeyword("NORMNAME")) normName = (string)fromNormGrp.FindKeyword("NORMNAME");
        else if (fromNormGrp.HasKeyword("NAME")) normName = (string)fromNormGrp.FindKeyword("NAME");
        else normName = "NONE";
        normName = normName.UpCase();
        if (normName != "ALBEDO" && normName != "ALBEDOATM" && normName != "MIXED" &&
            normName != "MOONALBEDO" && normName != "SHADE" && normName != "SHADEATM" &&
            normName != "TOPO" && normName != "TOPOATM") {
          string message = "A Normalization model must be specified before running this program.";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
      toNormPvl.AddObject(fromNormObj);
    }
  } else {
    iString normName = ui.GetAsString("NORMNAME");
    normName = normName.UpCase();
    vector<string> inclusion;
    inclusion.clear();
    inclusion.push_back("NORMNAME");
    if (normName == "ALBEDO" || normName == "MIXED") {
      inclusion.push_back("INCREF");
      inclusion.push_back("INCMAT");
      inclusion.push_back("THRESH");
      inclusion.push_back("ALBEDO");
    } else if (normName == "MOONALBEDO") {
      inclusion.push_back("D");
      inclusion.push_back("E");
      inclusion.push_back("F");
      inclusion.push_back("G2");
      inclusion.push_back("XMUL");
      inclusion.push_back("WL");
      inclusion.push_back("H");
      inclusion.push_back("BSH1");
      inclusion.push_back("XB1");
      inclusion.push_back("XB2");
    } else if (normName == "SHADE") {
      inclusion.push_back("INCREF");
      inclusion.push_back("ALBEDO");
    } else if (normName == "TOPO") {
      inclusion.push_back("INCREF");
      inclusion.push_back("THRESH");
      inclusion.push_back("ALBEDO");
    } else if (normName == "ALBEDOATM") {
      inclusion.push_back("ATMNAME");
      inclusion.push_back("INCREF");
    } else if (normName == "SHADEATM") {
      inclusion.push_back("ATMNAME");
      inclusion.push_back("INCREF");
      inclusion.push_back("ALBEDO");
    } else if (normName == "TOPOATM") {
      inclusion.push_back("ATMNAME");
      inclusion.push_back("INCREF");
      inclusion.push_back("ALBEDO");
    }
    ui.CreatePVL(toNormPvl, "Normalization Model", "NormalizationModel", "Algorithm", inclusion);
  }
 
  Pvl toAtmPvl;
  iString atmName;
  // Check to make sure that an atmospheric model was specified (if the
  // normalization model requires it)
  if (normName == "ALBEDOATM" || normName == "SHADEATM" || normName == "TOPOATM") {
    if (ui.GetAsString("ATMNAME") == "NONE") {
      if (!ui.WasEntered("FROMPVL")) {
        string message = "An Atmospheric model must be specified when doing normalization with atmosphere.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      } else {
        Pvl fromAtmPvl; 
        PvlObject fromAtmObj;
        PvlGroup fromAtmGrp;
        string input = ui.GetFilename("FROMPVL");
        fromAtmPvl.Read(input);
        if (!fromAtmPvl.HasObject("AtmosphericModel")) {
          string message = "An Atmospherice model must be specified when doing normalization with atmosphere.";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
        fromAtmObj = fromAtmPvl.FindObject("AtmosphericModel");
        if (!fromAtmObj.HasGroup("Algorithm")) {
          string message = "An Atmospheric model must be specified when doing normalization with atmosphere.";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        } else {
          fromAtmGrp = fromAtmObj.FindGroup("Algorithm");
          if (fromAtmGrp.HasKeyword("ATMNAME")) atmName = (string)fromAtmGrp.FindKeyword("ATMNAME");
          else if (fromAtmGrp.HasKeyword("NAME")) atmName = (string)fromAtmGrp.FindKeyword("NAME");
          else atmName = "NONE";
          atmName = atmName.UpCase();
          if (atmName != "ANISOTROPIC1" && atmName != "ANISOTROPIC2" && atmName != "HAPKEATM1" &&
              atmName != "HAPKEATM2" && atmName != "ISOTROPIC1" && atmName != "ISOTROPIC2") {
            string message = "An Atmospheric model must be specified when doing normalization with atmosphere.";
            throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
          }
        }
        toAtmPvl.AddObject(fromAtmObj);
      }
    } else {
      iString atmName = ui.GetAsString("ATMNAME");
      atmName = atmName.UpCase();
      vector<string> inclusion;
      inclusion.clear();
      inclusion.push_back("ATMNAME");
      if (atmName == "ANISOTROPIC1" || atmName == "ANISOTROPIC2" ||
          atmName == "HAPKEATM1" || atmName == "HAPKEATM2" ||
          atmName == "ISOTROPIC1" || atmName == "ISOTROPIC2") {
        inclusion.push_back("HNORM");
        inclusion.push_back("TAU");
        inclusion.push_back("TAUREF");
        inclusion.push_back("WHA");
        inclusion.push_back("NULNEG");
      }
      if (atmName == "ANISOTROPIC1" || atmName == "ANISOTROPIC2") {
        inclusion.push_back("BHA");
      }
      if (atmName == "HAPKEATM1" || atmName == "HAPKEATM2") {
        inclusion.push_back("HGA");
      }
      ui.CreatePVL(toAtmPvl, "Atmospheric Model", "AtmosphericModel", "Algorithm", inclusion);
    }
  }

  Pvl toPhtPvl;
  iString phtName;
  // Check to make sure that a photometric model was specified 
  if (ui.GetAsString("PHTNAME") == "NONE") {
    if (!ui.WasEntered("FROMPVL")) {
      string message = "A Photometric model must be specified before running this program.";
      throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
    } else {
      Pvl fromPhtPvl; 
      PvlObject fromPhtObj;
      PvlGroup fromPhtGrp;
      string input = ui.GetFilename("FROMPVL");
      fromPhtPvl.Read(input);
      if (!fromPhtPvl.HasObject("PhotometricModel")) {
        string message = "A Photometric model must be specified before running this program.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
      fromPhtObj = fromPhtPvl.FindObject("PhotometricModel");
      if (!fromPhtObj.HasGroup("Algorithm")) {
        string message = "A Photometric model must be specified before running this program.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      } else {
        fromPhtGrp = fromPhtObj.FindGroup("Algorithm");
        if (fromPhtGrp.HasKeyword("PHTNAME")) phtName = (string)fromPhtGrp.FindKeyword("PHTNAME");
        else if (fromPhtGrp.HasKeyword("NAME")) phtName = (string)fromPhtGrp.FindKeyword("NAME");
        else phtName = "NONE";
        phtName = phtName.UpCase();
        if (phtName != "HAPKEHEN" && phtName != "HAPKELEG" && phtName != "LUNARLAMBERTEMPIRICAL" &&
            phtName != "MINNAERTEMPIRICAL" && phtName != "LUNARLAMBERT" && phtName != "MINNAERT" &&
            phtName != "LAMBERT" && phtName != "LOMMELSEELIGER" && phtName != "LUNARLAMBERTMCEWEN") {
          string message = "A Photometric model must be specified before running this program.";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
      toPhtPvl.AddObject(fromPhtObj);
    }
  } else {
    iString phtName = ui.GetAsString("PHTNAME");
    phtName = phtName.UpCase();
    vector<string> inclusion;
    inclusion.clear();
    inclusion.push_back("PHTNAME");
    if (phtName == "HAPKEHEN" || phtName == "HAPKELEG") {
      inclusion.push_back("THETA");
      inclusion.push_back("WH");
      inclusion.push_back("HH");
      inclusion.push_back("B0");
      if (phtName == "HAPKEHEN") {
        inclusion.push_back("HG1");
        inclusion.push_back("HG2");
      } else {
        inclusion.push_back("BH");
        inclusion.push_back("CH");
      }
    } else if (phtName == "LUNARLAMBERTEMPIRICAL" || phtName == "MINNAERTEMPIRICAL") {
      inclusion.push_back("PHASELIST");
      inclusion.push_back("PHASECURVELIST");
      if (phtName == "LUNARLAMBERTEMPIRICAL") {
        inclusion.push_back("LLIST");
      } else {
        inclusion.push_back("KLIST");
      }
    } else if (phtName == "LUNARLAMBERT") {
      inclusion.push_back("L");
    } else if (phtName == "MINNAERT") {
      inclusion.push_back("K");
    }
    ui.CreatePVL(toPhtPvl, "Photometric Model", "PhotometricModel", "Algorithm", inclusion);
  }

  PvlObject normObj = toNormPvl.FindObject("NormalizationModel");
  PvlObject phtObj = toPhtPvl.FindObject("PhotometricModel");
  PvlObject atmObj;
  if (normName == "ALBEDOATM" || normName == "SHADEATM" || normName == "TOPOATM") {
    atmObj = toAtmPvl.FindObject("AtmosphericModel");
  }

  Pvl par;
  par.AddObject(normObj);
  par.AddObject(phtObj);
  if (normName == "ALBEDOATM" || normName == "SHADEATM" || normName == "TOPOATM") {
    par.AddObject(atmObj);
  }
cout << par << endl;

  // Set value for maximum emission/incidence angles chosen by user
  maxema = ui.GetDouble("MAXEMISSION");
  maxinc = ui.GetDouble("MAXINCIDENCE");
  
  // determine how photometric angles should be calculated
  angleSource = ui.GetString("ANGLESOURCE");
 
  // Get camera information if needed
  if (angleSource == "ELLIPSOID" || angleSource == "DEM" || 
      angleSource == "CENTER_FROM_IMAGE") {
    // Set up the input cube 
    icube = p.SetInputCube("FROM");
    cam = icube->getCamera();
  } 
  else {
    p.SetInputCube("FROM");
  }

  // Create the output cube
  p.SetOutputCube("TO");

  Pvl inLabel;
  inLabel.Read(ui.GetFilename("FROM"));

  // If the source of photometric angles is the center of the image,
  // then get the angles at the center of the image.
  if (angleSource == "CENTER_FROM_IMAGE") {
    cam->SetImage(cam->Samples()/2, cam->Lines()/2);
    centerPhase = cam->PhaseAngle();
    centerIncidence = cam->IncidenceAngle();
    centerEmission = cam->EmissionAngle();
  }
  else if (angleSource == "CENTER_FROM_LABEL") {
    centerPhase = inLabel.FindKeyword("PhaseAngle", Pvl::Traverse);
    centerIncidence = inLabel.FindKeyword("IncidenceAngle", Pvl::Traverse);
    centerEmission = inLabel.FindKeyword("EmissionAngle", Pvl::Traverse);
  }
  else if (angleSource == "CENTER_FROM_USER") {
    centerPhase = ui.GetDouble("PHASE_ANGLE");
    centerIncidence = ui.GetDouble("INCIDENCE_ANGLE");
    centerEmission = ui.GetDouble("EMISSION_ANGLE");
  }
 
  // Get the BandBin Center from the image
  PvlGroup pvlg = inLabel.FindGroup("BandBin", Pvl::Traverse);
  double wl;
  if(pvlg.HasKeyword("Center")) {
    PvlKeyword &wavelength = pvlg.FindKeyword("Center");
    wl = wavelength[0];
  }
  else {
    wl = 1.0;
  }

  // Create the photometry object and set the wavelength
  PvlGroup &algo = par.FindObject("NormalizationModel").FindGroup("Algorithm", Pvl::Traverse);
  if(!algo.HasKeyword("Wl")) {
    algo.AddKeyword(Isis::PvlKeyword("Wl", wl));
  }
  pho = new Photometry(par);
  pho->SetPhotomWl(wl);

  // Start the processing
  p.StartProcess(photomet);
  p.EndProcess();
}

/**
 * Perform photometric correction
 *
 * @param in Buffer containing input DN values
 * @param out Buffer containing output DN values
 * @author Janet Barrett
 * @internal
 *   @history 2009-01-08 Jeannie Walldren - Modified to set off
 *            target pixels to null.  Added check for new maxinc
 *            and maxema parameters.
 */
void photomet(Buffer &in, Buffer &out) {

  double dempha=0., deminc=0., demema=0., mult=0., base=0.;
  double ellipsoidpha=0., ellipsoidinc=0., ellipsoidema=0.;
  
  for (int i = 0; i < in.size(); i++) {
  
    // if special pixel, copy to output
    if(!IsValidPixel(in[i])) {
      out[i] = in[i];
    }
    
    // if off the target, set to null
    else if((angleSource == "ELLIPSOID" || angleSource == "DEM" ||
            angleSource == "CENTER_FROM_IMAGE") &&
            (!cam->SetImage(in.Sample(i), in.Line(i)))) {
      out[i] = NULL8;
    }
    
    // otherwise, compute angle values
    else {
      
      bool success = true;
      if (angleSource == "CENTER_FROM_IMAGE" || 
          angleSource == "CENTER_FROM_LABEL" ||
          angleSource == "CENTER_FROM_USER") {
        ellipsoidpha = centerPhase;
        ellipsoidinc = centerIncidence;
        ellipsoidema = centerEmission;
        dempha = centerPhase;
        deminc = centerIncidence;
        demema = centerEmission;
      } else {
        // calculate photometric angles
        ellipsoidpha = cam->PhaseAngle();
        ellipsoidinc = cam->IncidenceAngle();
        ellipsoidema = cam->EmissionAngle();
        if (angleSource == "DEM") {
          Angle phase, incidence, emission;
          cam->LocalPhotometricAngles(phase, incidence, emission, success);
          if (success) {
            dempha = phase.GetDegrees();
            deminc = incidence.GetDegrees();
            demema = emission.GetDegrees();
          }
        } else if (angleSource == "ELLIPSOID") {
          dempha = ellipsoidpha;
          deminc = ellipsoidinc;
          demema = ellipsoidema;
        }
      }

      // if invalid angles, set to null
      if(!success) {
        out[i] = NULL8;
      }
      else if(deminc >= 90.0 || demema >= 90.0) {
        out[i] = NULL8;
      }
      // if angles greater than max allowed by user, set to null
      else if(deminc > maxinc || demema > maxema) {
        out[i] = NULL8;
      }
      // otherwise, do photometric correction
      else {
        pho->Compute(ellipsoidpha, ellipsoidinc, ellipsoidema, deminc, demema, in[i], out[i], mult, base);
      }
    }
  }
}
