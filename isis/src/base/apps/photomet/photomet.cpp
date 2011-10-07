#define GUIHELPERS

#include "Isis.h"
#include <sstream>
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
bool usedem;
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
  std::stringstream os;
  UserInterface &ui = Application::GetUserInterface();
  string inFile(ui.GetFilename("FROMPVL"));
  Pvl inPvl;
  inPvl.Read(inFile);
  iString phtName = ui.GetAsString("PHTNAME");
  phtName = phtName.UpCase();
  iString atmName = ui.GetAsString("ATMNAME");
  atmName = atmName.UpCase();
  iString nrmName = ui.GetAsString("NORMNAME");
  nrmName = nrmName.UpCase();
  if (phtName == "NONE" && atmName == "NONE" && nrmName == "NONE") {
    string message = "A photometric or atmospheric or normalization model must be specified before it can be loaded from the PVL";
    throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
  }
  if (phtName != "NONE") {
    if (inPvl.HasObject("PhotometricModel")) {
      PvlObject phtObj = inPvl.FindObject("PhotometricModel");
      iString phtVal;
      if (phtObj.HasGroup("Algorithm")) {
        PvlObject::PvlGroupIterator phtGrp = phtObj.BeginGroup();
        bool wasFound = false;
        if (phtGrp->HasKeyword("PHTNAME")) {
          phtVal = (string)phtGrp->FindKeyword("PHTNAME");
        } else if (phtGrp->HasKeyword("NAME")) {
          phtVal = (string)phtGrp->FindKeyword("NAME");
        } else {
          phtVal = "NONE";
        }
        phtVal = phtVal.UpCase();
        if (phtName == phtVal) {
          wasFound = true;
        }
        if (ui.WasEntered("PHTNAME") && !wasFound) {
          phtName = ui.GetAsString("PHTNAME");
          phtName = phtName.UpCase();
          while (phtGrp != phtObj.EndGroup()) {
            if (phtGrp->HasKeyword("PHTNAME") || phtGrp->HasKeyword("NAME")) {
              if (phtGrp->HasKeyword("PHTNAME")) {
                phtVal = (string)phtGrp->FindKeyword("PHTNAME");
              } else if (phtGrp->HasKeyword("NAME")) {
                phtVal = (string)phtGrp->FindKeyword("NAME");
              } else {
                phtVal = "NONE";
              }
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
          if (phtGrp->HasKeyword("PHTNAME")) {
            phtVal = (string)phtGrp->FindKeyword("PHTNAME");
          } else if (phtGrp->HasKeyword("NAME")) {
            phtVal = (string)phtGrp->FindKeyword("NAME");
          } else {
            phtVal = "NONE";
          }
          phtVal = phtVal.UpCase();
          if (phtVal == "HAPKEHEN" || phtVal == "HAPKELEG") {
            if (phtGrp->HasKeyword("THETA")) {
              double theta = phtGrp->FindKeyword("THETA");
              os.str("");
              os << theta;
              ui.PutAsString("THETA", os.str());
            }
            if (phtGrp->HasKeyword("WH")) {
              double wh = phtGrp->FindKeyword("WH");
              os.str("");
              os << wh;
              ui.PutAsString("WH", os.str());
            }
            if (phtGrp->HasKeyword("HH")) {
              double hh = phtGrp->FindKeyword("HH");
              os.str("");
              os << hh;
              ui.PutAsString("HH", os.str());
            }
            if (phtGrp->HasKeyword("B0")) {
              double b0 = phtGrp->FindKeyword("B0");
              os.str("");
              os << b0;
              ui.PutAsString("B0", os.str());
            }
            if (phtVal == "HAPKEHEN") {
              if (phtGrp->HasKeyword("HG1")) {
                double hg1 = phtGrp->FindKeyword("HG1");
                os.str("");
                os << hg1;
                ui.PutAsString("HG1", os.str());
              }
              if (phtGrp->HasKeyword("HG2")) {
                double hg2 = phtGrp->FindKeyword("HG2");
                os.str("");
                os << hg2;
                ui.PutAsString("HG2", os.str());
              }
            }
            if (phtVal == "HAPKELEG") {
              if (phtGrp->HasKeyword("BH")) {
                double bh = phtGrp->FindKeyword("BH");
                os.str("");
                os << bh;
                ui.PutAsString("BH", os.str());
              }
              if (phtGrp->HasKeyword("CH")) {
                double ch = phtGrp->FindKeyword("CH");
                os.str("");
                os << ch;
                ui.PutAsString("CH", os.str());
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
              os.str("");
              os << l;
              ui.PutAsString("L", os.str());
            } 
          } else if (phtVal == "MINNAERT") {
            if (phtGrp->HasKeyword("K")) {
              double k = phtGrp->FindKeyword("K");
              os.str("");
              os << k;
              ui.PutAsString("K", os.str());
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

  iString nrmVal;
//  if (nrmName == "NONE") {
//    string message = "A normalization model must be specified before it can be loaded from the PVL";
//    throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
//  }
  if (nrmName != "NONE") {
    if (inPvl.HasObject("NormalizationModel")) {
      PvlObject nrmObj = inPvl.FindObject("NormalizationModel");
      if (nrmObj.HasGroup("Algorithm")) {
        PvlObject::PvlGroupIterator nrmGrp = nrmObj.BeginGroup();
        bool wasFound = false;
        if (nrmGrp->HasKeyword("NORMNAME")) {
          nrmVal = (string)nrmGrp->FindKeyword("NORMNAME");
        } else if (nrmGrp->HasKeyword("NAME")) {
          nrmVal = (string)nrmGrp->FindKeyword("NAME");
        } else {
          nrmVal = "NONE";
        }
        nrmVal = nrmVal.UpCase();
        if (nrmName == nrmVal) {
          wasFound = true;
        }
        if (ui.WasEntered("NORMNAME") && !wasFound) {
          while (nrmGrp != nrmObj.EndGroup()) {
            if (nrmGrp->HasKeyword("NORMNAME") || nrmGrp->HasKeyword("NAME")) {
              if (nrmGrp->HasKeyword("NORMNAME")) {
                nrmVal = (string)nrmGrp->FindKeyword("NORMNAME");
              } else if (nrmGrp->HasKeyword("NAME")) {
                nrmVal = (string)nrmGrp->FindKeyword("NAME");
              } else {
                nrmVal = "NONE";
              }
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
          if (nrmGrp->HasKeyword("NORMNAME")) {
            nrmVal = (string)nrmGrp->FindKeyword("NORMNAME");
          } else if (nrmGrp->HasKeyword("NAME")) {
            nrmVal = (string)nrmGrp->FindKeyword("NAME");
          } else {
            nrmVal = "NONE";
          }
          nrmVal = nrmVal.UpCase();
          if (nrmVal != "MOONALBEDO") {
            if (nrmVal == "ALBEDO" || nrmVal == "MIXED") {
              if (nrmGrp->HasKeyword("INCREF")) {
                double incref = nrmGrp->FindKeyword("INCREF");
                os.str("");
                os << incref;
                ui.PutAsString("INCREF", os.str());
              } 
              if (nrmGrp->HasKeyword("INCMAT")) {
                double incmat = nrmGrp->FindKeyword("INCMAT");
                os.str("");
                os << incmat;
                ui.PutAsString("INCMAT", os.str());
              }
              if (nrmGrp->HasKeyword("THRESH")) {
                double thresh = nrmGrp->FindKeyword("THRESH");
                os.str("");
                os << thresh;
                ui.PutAsString("THRESH", os.str());
              }
              if (nrmGrp->HasKeyword("ALBEDO")) {
                double albedo = nrmGrp->FindKeyword("ALBEDO");
                os.str("");
                os << albedo;
                ui.PutAsString("ALBEDO", os.str());
              }  
            } else if (nrmVal == "SHADE") {
              if (nrmGrp->HasKeyword("INCREF")) {
                double incref = nrmGrp->FindKeyword("INCREF");
                os.str("");
                os << incref;
                ui.PutAsString("INCREF", os.str());
              }
              if (nrmGrp->HasKeyword("ALBEDO")) {
                double albedo = nrmGrp->FindKeyword("ALBEDO");
                os.str("");
                os << albedo;
                ui.PutAsString("ALBEDO", os.str());
              }
            } else if (nrmVal == "TOPO") {
              if (nrmGrp->HasKeyword("INCREF")) {
                double incref = nrmGrp->FindKeyword("INCREF");
                os.str("");
                os << incref;
                ui.PutAsString("INCREF", os.str());
              }
              if (nrmGrp->HasKeyword("ALBEDO")) {
                double albedo = nrmGrp->FindKeyword("ALBEDO");
                os.str("");
                os << albedo;
                ui.PutAsString("ALBEDO", os.str());
              }
              if (nrmGrp->HasKeyword("THRESH")) {
                double thresh = nrmGrp->FindKeyword("THRESH");
                os.str("");
                os << thresh;
                ui.PutAsString("THRESH", os.str());
              }
            } else if (nrmVal == "ALBEDOATM") {
              if (nrmGrp->HasKeyword("INCREF")) {
                double incref = nrmGrp->FindKeyword("INCREF");
                os.str("");
                os << incref;
                ui.PutAsString("INCREF", os.str());
              }
            } else if (nrmVal == "SHADEATM") {
              if (nrmGrp->HasKeyword("INCREF")) {
                double incref = nrmGrp->FindKeyword("INCREF");
                os.str("");
                os << incref;
                ui.PutAsString("INCREF", os.str());
              }
              if (nrmGrp->HasKeyword("ALBEDO")) {
                double albedo = nrmGrp->FindKeyword("ALBEDO");
                os.str("");
                os << albedo;
                ui.PutAsString("ALBEDO", os.str());
              }
            } else if (nrmVal == "TOPOATM") {
              if (nrmGrp->HasKeyword("INCREF")) {
                double incref = nrmGrp->FindKeyword("INCREF");
                os.str("");
                os << incref;
                ui.PutAsString("INCREF", os.str());
              } 
              if (nrmGrp->HasKeyword("ALBEDO")) {
                double albedo = nrmGrp->FindKeyword("ALBEDO");
                os.str("");
                os << albedo;
                ui.PutAsString("ALBEDO", os.str());
              }
            } else {
              string message = "Unsupported normalization model [" + nrmVal + "].";
              throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
            }
          } else {
            if (nrmGrp->HasKeyword("D")) {
              double d = nrmGrp->FindKeyword("D");
              os.str("");
              os << d;
              ui.PutAsString("D", os.str());
            }
            if (nrmGrp->HasKeyword("E")) {
              double e = nrmGrp->FindKeyword("E");
              os.str("");
              os << e;
              ui.PutAsString("E", os.str());
            }
            if (nrmGrp->HasKeyword("F")) {
              double f = nrmGrp->FindKeyword("F");
              os.str("");
              os << f;
              ui.PutAsString("F", os.str());
            }
            if (nrmGrp->HasKeyword("G2")) {
              double g2 = nrmGrp->FindKeyword("G2");
              os.str("");
              os << g2;
              ui.PutAsString("G2", os.str());
            }
            if (nrmGrp->HasKeyword("XMUL")) {
              double xmul = nrmGrp->FindKeyword("XMUL");
              os.str("");
              os << xmul;
              ui.PutAsString("XMUL", os.str());
            }
            if (nrmGrp->HasKeyword("WL")) {
              double wl = nrmGrp->FindKeyword("WL");
              os.str("");
              os << wl;
              ui.PutAsString("WL", os.str());
            }
            if (nrmGrp->HasKeyword("H")) {
              double h = nrmGrp->FindKeyword("H");
              os.str("");
              os << h;
              ui.PutAsString("H", os.str());
            }
            if (nrmGrp->HasKeyword("BSH1")) {
              double bsh1 = nrmGrp->FindKeyword("BSH1");
              os.str("");
              os << bsh1;
              ui.PutAsString("BSH1", os.str());
            }
            if (nrmGrp->HasKeyword("XB1")) {
              double xb1 = nrmGrp->FindKeyword("XB1");
              os.str("");
              os << xb1;
              ui.PutAsString("XB1", os.str());
            }
            if (nrmGrp->HasKeyword("XB2")) {
              double xb2 = nrmGrp->FindKeyword("XB2");
              os.str("");
              os << xb2;
              ui.PutAsString("XB2", os.str());
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
//  if (atmName == "NONE") {
//    string message = "An atmospheric model must be specified before it can be loaded from the PVL";
//    throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
//  }
  if (atmName != "NONE") {
    if (inPvl.HasObject("AtmosphericModel")) {
      PvlObject atmObj = inPvl.FindObject("AtmosphericModel");
      iString atmVal;
      if (atmObj.HasGroup("Algorithm")) {
        PvlObject::PvlGroupIterator atmGrp = atmObj.BeginGroup();
        bool wasFound = false;
        if (atmGrp->HasKeyword("ATMNAME")) {
          atmVal = (string)atmGrp->FindKeyword("ATMNAME");
        } else if (atmGrp->HasKeyword("NAME")) {
          atmVal = (string)atmGrp->FindKeyword("NAME");
        } else {
          atmVal = "NONE";
        }
        atmVal = atmVal.UpCase();
        if (atmName == atmVal) {
          wasFound = true;
        }
        if (ui.WasEntered("ATMNAME") && !wasFound) {
          while (atmGrp != atmObj.EndGroup()) {
            if (atmGrp->HasKeyword("ATMNAME") || atmGrp->HasKeyword("NAME")) {
              if (atmGrp->HasKeyword("ATMNAME")) {
                atmVal = (string)atmGrp->FindKeyword("ATMNAME");
              } else if (atmGrp->HasKeyword("NAME")) {
                atmVal = (string)atmGrp->FindKeyword("NAME");
              } else {
                atmVal = "NONE";
              }
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
        if (atmGrp->HasKeyword("ATMNAME")) {
          atmVal = (string)atmGrp->FindKeyword("ATMNAME");
        } else if (atmGrp->HasKeyword("NAME")) {
          atmVal = (string)atmGrp->FindKeyword("NAME");
        } else {
          atmVal = "NONE";
        }
        atmVal = atmVal.UpCase();
        if (atmVal == "ANISOTROPIC1" || atmVal == "ANISOTROPIC2" ||
            atmVal == "HAPKEATM1" || atmVal == "HAPKEATM2" ||
            atmVal == "ISOTROPIC1" || atmVal == "ISOTROPIC2") {
          if (atmGrp->HasKeyword("HNORM")) {
            double hnorm = atmGrp->FindKeyword("HNORM");
            os.str("");
            os << hnorm;
            ui.PutAsString("HNORM", os.str());
          }
          if (atmGrp->HasKeyword("TAU")) {
            double tau = atmGrp->FindKeyword("TAU");
            os.str("");
            os << tau;
            ui.PutAsString("TAU", os.str());
          }
          if (atmGrp->HasKeyword("TAUREF")) {
            double tauref = atmGrp->FindKeyword("TAUREF");
            os.str("");
            os << tauref;
            ui.PutAsString("TAUREF", os.str());
          }
          if (atmGrp->HasKeyword("WHA")) {
            double wha = atmGrp->FindKeyword("WHA");
            os.str("");
            os << wha;
            ui.PutAsString("WHA", os.str());
          }
          if (atmGrp->HasKeyword("NULNEG")) {
            string nulneg = (string)atmGrp->FindKeyword("NULNEG");
            if (nulneg.compare("YES")) {
              ui.PutString("NULNEG", "YES");
            } else if (nulneg.compare("NO")) {
              ui.PutString("NULNEG", "NO");
            } else {
              string message = "The NULNEG value is invalid - must be set to YES or NO";
              throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
            }
          }
        }
        if (atmVal == "ANISOTROPIC1" || atmVal == "ANISOTROPIC2") {
          if (atmGrp->HasKeyword("BHA")) {
            double bha = atmGrp->FindKeyword("BHA");
            os.str("");
            os << bha;
            ui.PutAsString("BHA", os.str());
          }
        }
        if (atmVal == "HAPKEATM1" || atmVal == "HAPKEATM2") {
          if (atmGrp->HasKeyword("HGA")) {
            double hga = atmGrp->FindKeyword("HGA");
            os.str("");
            os << hga;
            ui.PutAsString("HGA", os.str());
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
  iString normName = ui.GetAsString("NORMNAME");
  normName = normName.UpCase();
  // Check to make sure that a normalization model was specified
  if (normName == "NONE") {
    string message = "A Normalization model must be specified before running this program.";
    throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
  }
  bool wasFound = false;
  if (ui.WasEntered("FROMPVL")) {
    iString normVal;
    Pvl fromNormPvl; 
    PvlObject fromNormObj;
    PvlGroup fromNormGrp;
    string input = ui.GetFilename("FROMPVL");
    fromNormPvl.Read(input);
    if (fromNormPvl.HasObject("NormalizationModel")) {
      fromNormObj = fromNormPvl.FindObject("NormalizationModel");
      if (fromNormObj.HasGroup("Algorithm")) {
        PvlObject::PvlGroupIterator fromNormGrp = fromNormObj.BeginGroup();
        if (fromNormGrp->HasKeyword("NORMNAME")) {
          normVal = (string)fromNormGrp->FindKeyword("NORMNAME");
        } else if (fromNormGrp->HasKeyword("NAME")) {
          normVal = (string)fromNormGrp->FindKeyword("NAME");
        } else {
          normVal = "NONE";
        }
        normVal = normVal.UpCase();
        if (normName == normVal) {
          wasFound = true;
        }
        if (!wasFound) {
          while (fromNormGrp != fromNormObj.EndGroup()) {
            if (fromNormGrp->HasKeyword("NORMNAME") || fromNormGrp->HasKeyword("NAME")) {
              if (fromNormGrp->HasKeyword("NORMNAME")) {
                normVal = (string)fromNormGrp->FindKeyword("NORMNAME");
              } else if (fromNormGrp->HasKeyword("NAME")) {
                normVal = (string)fromNormGrp->FindKeyword("NAME");
              } else {
                normVal = "NONE";
              }
              normVal = normVal.UpCase();
              if (normName == normVal) {
                wasFound = true;
                break;
              }
            }
            fromNormGrp++;
          }
        }
      }
    }
    if (wasFound) {
      toNormPvl.AddObject(fromNormObj);
    } else {
      toNormPvl.AddObject(PvlObject("NormalizationModel"));
      toNormPvl.FindObject("NormalizationModel").AddGroup(PvlGroup("Algorithm"));
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("NORMNAME",normName),Pvl::Replace);
    }
  } else {
    toNormPvl.AddObject(PvlObject("NormalizationModel"));
    toNormPvl.FindObject("NormalizationModel").AddGroup(PvlGroup("Algorithm"));
    toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
              AddKeyword(PvlKeyword("NORMNAME",normName),Pvl::Replace);
  }

  if (normName == "ALBEDO" || normName == "MIXED") {
    if (ui.WasEntered("INCREF")) {
      iString keyval = ui.GetString("INCREF");
      double incref = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("INCREF",incref),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("INCREF")) {
        string message = "The " + normName + " Normalization model requires a value for the INCREF parameter.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("INCMAT")) {
      iString keyval = ui.GetString("INCMAT");
      double incmat = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("INCMAT",incmat),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("INCMAT")) {
        string message = "The " + normName + " Normalization model requires a value for the INCMAT parameter.";
        message += "The normal range for INCMAT is: 0 <= INCMAT < 90";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("THRESH")) {
      iString keyval = ui.GetString("THRESH");
      double thresh = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("THRESH",thresh),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("THRESH")) {
        string message = "The " + normName + " Normalization model requires a value for the THRESH parameter.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("ALBEDO")) {
      iString keyval = ui.GetString("ALBEDO");
      double albedo = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("ALBEDO",albedo),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("ALBEDO")) {
        string message = "The " + normName + " Normalization model requires a value for the ALBEDO parameter.";
        message += "The ALBEDO parameter has no limited range";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
  } else if (normName == "MOONALBEDO") {
    if (ui.WasEntered("D")) {
      iString keyval = ui.GetString("D");
      double d = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("D",d),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("D")) {
        string message = "The " + normName + " Normalization model requires a value for the D parameter.";
        message += "The D parameter has no limited range";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("E")) {
      iString keyval = ui.GetString("E");
      double e = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("E",e),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("E")) {
        string message = "The " + normName + " Normalization model requires a value for the E parameter.";
        message += "The E parameter has no limited range";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("F")) {
      iString keyval = ui.GetString("F");
      double f = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("F",f),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("F")) {
        string message = "The " + normName + " Normalization model requires a value for the F parameter.";
        message += "The F parameter has no limited range";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("G2")) {
      iString keyval = ui.GetString("G2");
      double g2 = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("G2",g2),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("G2")) {
        string message = "The " + normName + " Normalization model requires a value for the G2 parameter.";
        message += "The G2 parameter has no limited range";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("XMUL")) {
      iString keyval = ui.GetString("XMUL");
      double xmul = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("XMUL",xmul),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("XMUL")) {
        string message = "The " + normName + " Normalization model requires a value for the XMUL parameter.";
        message += "The XMUL parameter has no range limit";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("WL")) {
      iString keyval = ui.GetString("WL");
      double wl = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("WL",wl),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("WL")) {
        string message = "The " + normName + " Normalization model requires a value for the WL parameter.";
        message += "The WL parameter has no range limit";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("H")) {
      iString keyval = ui.GetString("H");
      double h = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("H",h),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("H")) {
        string message = "The " + normName + " Normalization model requires a value for the H parameter.";
        message += "The H parameter has no limited range";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("BSH1")) {
      iString keyval = ui.GetString("BSH1");
      double bsh1 = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("BSH1",bsh1),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("BSH1")) {
        string message = "The " + normName + " Normalization model requires a value for the BSH1 parameter.";
        message += "The normal range for BSH1 is: 0 <= BSH1";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("XB1")) {
      iString keyval = ui.GetString("XB1");
      double xb1 = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("XB1",xb1),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("XB1")) {
        string message = "The " + normName + " Normalization model requires a value for the XB1 parameter.";
        message += "The XB1 parameter has no range limit";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("XB2")) {
      iString keyval = ui.GetString("XB2");
      double xb2 = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("XB2",xb2),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("XB2")) {
        string message = "The " + normName + " Normalization model requires a value for the XB2 parameter.";
        message += "The XB2 parameter has no range limit";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
  } else if (normName == "SHADE") {
    if (ui.WasEntered("INCREF")) {
      iString keyval = ui.GetString("INCREF");
      double incref = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("INCREF",incref),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("INCREF")) {
        string message = "The " + normName + " Normalization model requires a value for the INCREF parameter.";
        message += "The normal range for INCREF is: 0 <= INCREF < 90";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("ALBEDO")) {
      iString keyval = ui.GetString("ALBEDO");
      double albedo = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("ALBEDO",albedo),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("ALBEDO")) {
        string message = "The " + normName + " Normalization model requires a value for the ALBEDO parameter.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
  } else if (normName == "TOPO") {
    if (ui.WasEntered("INCREF")) {
      iString keyval = ui.GetString("INCREF");
      double incref = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("INCREF",incref),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("INCREF")) {
        string message = "The " + normName + " Normalization model requires a value for the INCREF parameter.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("THRESH")) {
      iString keyval = ui.GetString("THRESH");
      double thresh = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("THRESH",thresh),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("THRESH")) {
        string message = "The " + normName + " Normalization model requires a value for the THRESH parameter.";
        message += "The THRESH parameter has no range limit";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("ALBEDO")) {
      iString keyval = ui.GetString("ALBEDO");
      double albedo = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("ALBEDO",albedo),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("ALBEDO")) {
        string message = "The " + normName + " Normalization model requires a value for the ALBEDO parameter.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
  } else if (normName == "ALBEDOATM") {
    if (ui.WasEntered("INCREF")) {
      iString keyval = ui.GetString("INCREF");
      double incref = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("INCREF",incref),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("INCREF")) {
        string message = "The " + normName + " Normalization model requires a value for the INCREF parameter.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
  } else if (normName == "SHADEATM") {
    if (ui.WasEntered("INCREF")) {
      iString keyval = ui.GetString("INCREF");
      double incref = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("INCREF",incref),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("INCREF")) {
        string message = "The " + normName + " Normalization model requires a value for the INCREF parameter.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("ALBEDO")) {
      iString keyval = ui.GetString("ALBEDO");
      double albedo = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("ALBEDO",albedo),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("ALBEDO")) {
        string message = "The " + normName + " Normalization model requires a value for the ALBEDO parameter.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
  } else if (normName == "TOPOATM") {
    if (ui.WasEntered("INCREF")) {
      iString keyval = ui.GetString("INCREF");
      double incref = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("INCREF",incref),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("INCREF")) {
        string message = "The " + normName + " Normalization model requires a value for the INCREF parameter.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("ALBEDO")) {
      iString keyval = ui.GetString("ALBEDO");
      double albedo = keyval.ToDouble();
      toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("ALBEDO",albedo),Pvl::Replace);
    } else {
      if (!toNormPvl.FindObject("NormalizationModel").FindGroup("Algorithm").
                     HasKeyword("ALBEDO")) {
        string message = "The " + normName + " Normalization model requires a value for the ALBEDO parameter.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
  }
 
  Pvl toAtmPvl;
  iString atmName = ui.GetAsString("ATMNAME");
  atmName = atmName.UpCase();
  // Check to make sure that an atmospheric model was specified (if the
  // normalization model requires it)
  if (normName == "ALBEDOATM" || normName == "SHADEATM" || normName == "TOPOATM") {
    if (atmName == "NONE") {
      string message = "An Atmospheric model must be specified when doing normalization with atmosphere.";
      throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
    }
    wasFound = false;
    if (ui.WasEntered("FROMPVL")) {
      iString atmVal;
      Pvl fromAtmPvl; 
      PvlObject fromAtmObj;
      PvlGroup fromAtmGrp;
      string input = ui.GetFilename("FROMPVL");
      fromAtmPvl.Read(input);
      if (fromAtmPvl.HasObject("AtmosphericModel")) {
        fromAtmObj = fromAtmPvl.FindObject("AtmosphericModel");
        if (fromAtmObj.HasGroup("Algorithm")) {
          PvlObject::PvlGroupIterator fromAtmGrp = fromAtmObj.BeginGroup();
          if (fromAtmGrp->HasKeyword("ATMNAME")) {
            atmVal = (string)fromAtmGrp->FindKeyword("ATMNAME");
          } else if (fromAtmGrp->HasKeyword("NAME")) {
            atmVal = (string)fromAtmGrp->FindKeyword("NAME");
          } else {
            atmVal = "NONE";
          }
          atmVal = atmVal.UpCase();
          if (atmName == atmVal) {
            wasFound = true;
          }
          if (!wasFound) {
            while (fromAtmGrp != fromAtmObj.EndGroup()) {
              if (fromAtmGrp->HasKeyword("ATMNAME") || fromAtmGrp->HasKeyword("NAME")) {
                if (fromAtmGrp->HasKeyword("ATMNAME")) {
                  atmVal = (string)fromAtmGrp->FindKeyword("ATMNAME");
                } else if (fromAtmGrp->HasKeyword("NAME")) {
                  atmVal = (string)fromAtmGrp->FindKeyword("NAME");
                } else {
                  atmVal = "NONE";
                }
                atmVal = atmVal.UpCase();
                if (atmName == atmVal) {
                  wasFound = true;
                  break;
                }
              }
              fromAtmGrp++;
            }
          }
        }
      }
      if (wasFound) {
        toAtmPvl.AddObject(fromAtmObj);
      } else {
        toAtmPvl.AddObject(PvlObject("AtmosphericModel"));
        toAtmPvl.FindObject("AtmosphericModel").AddGroup(PvlGroup("Algorithm"));
        toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                 AddKeyword(PvlKeyword("ATMNAME",atmName),Pvl::Replace);
      }
    } else {
      toAtmPvl.AddObject(PvlObject("AtmosphericModel"));
      toAtmPvl.FindObject("AtmosphericModel").AddGroup(PvlGroup("Algorithm"));
      toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("ATMNAME",atmName),Pvl::Replace);
    }


    if (atmName == "ANISOTROPIC1" || atmName == "ANISOTROPIC2" ||
        atmName == "HAPKEATM1" || atmName == "HAPKEATM2" ||
        atmName == "ISOTROPIC1" || atmName == "ISOTROPIC2") {
      if (ui.WasEntered("HNORM")) {
        iString keyval = ui.GetString("HNORM");
        double hnorm = keyval.ToDouble();
        toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                 AddKeyword(PvlKeyword("HNORM",hnorm),Pvl::Replace);
      } else {
        if (!toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                      HasKeyword("HNORM")) {
          string message = "The " + atmName + " Atmospheric model requires a value for the HNORM parameter.";
          message += "The normal range for HNORM is: 0 <= HNORM";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
      if (ui.WasEntered("TAU")) {
        iString keyval = ui.GetString("TAU");
        double tau = keyval.ToDouble();
        toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("TAU",tau),Pvl::Replace);
      } else {
        if (!toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                      HasKeyword("TAU")) {
          string message = "The " + atmName + " Atmospheric model requires a value for the TAU parameter.";
          message += "The normal range for TAU is: 0 <= TAU";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
      if (ui.WasEntered("TAUREF")) {
        iString keyval = ui.GetString("TAUREF");
        double tauref = keyval.ToDouble();
        toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("TAUREF",tauref),Pvl::Replace);
      } else {
        if (!toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                      HasKeyword("TAUREF")) {
          string message = "The " + atmName + " Atmospheric model requires a value for the TAUREF parameter.";
          message += "The normal range for TAUREF is: 0 <= TAUREF";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
      if (ui.WasEntered("WHA")) {
        iString keyval = ui.GetString("WHA");
        double wha = keyval.ToDouble();
        toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("WHA",wha),Pvl::Replace);
      } else {
        if (!toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                      HasKeyword("WHA")) {
          string message = "The " + atmName + " Atmospheric model requires a value for the WHA parameter.";
          message += "The normal range for WHA is: 0 < WHA < 1";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
      if (ui.GetString("NULNEG") == "YES") {
        toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                 AddKeyword(PvlKeyword("NULNEG","YES"),Pvl::Replace);
      } else if (ui.GetString("NULNEG") == "NO") {
        toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                 AddKeyword(PvlKeyword("NULNEG","NO"),Pvl::Replace);
      } else {
        if (!toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                      HasKeyword("NULNEG")) {
          string message = "The " + atmName + " Atmospheric model requires a value for the NULNEG parameter.";
          message += "The valid values for NULNEG are: YES, NO";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
    }

    if (atmName == "ANISOTROPIC1" || atmName == "ANISOTROPIC2") {
      if (ui.WasEntered("BHA")) {
        iString keyval = ui.GetString("BHA");
        double bha = keyval.ToDouble();
        toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("BHA",bha),Pvl::Replace);
      } else {
        if (!toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                      HasKeyword("BHA")) {
          string message = "The " + atmName + " Atmospheric model requires a value for the BHA parameter.";
          message += "The normal range for BHA is: -1 <= BHA <= 1";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
    }
    if (atmName == "HAPKEATM1" || atmName == "HAPKEATM2") {
      if (ui.WasEntered("HGA")) {
        iString keyval = ui.GetString("HGA");
        double hga = keyval.ToDouble();
        toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                AddKeyword(PvlKeyword("HGA",hga),Pvl::Replace);
      } else {
        if (!toAtmPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                      HasKeyword("HGA")) {
          string message = "The " + atmName + " Atmospheric model requires a value for the HGA parameter.";
          message += "The normal range for HGA is: -1 < HGA < 1";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
    }
  }


  Pvl toPhtPvl;
  iString phtName = ui.GetAsString("PHTNAME");
  phtName = phtName.UpCase();
  // Check to make sure that a photometric model was specified 
  if (phtName == "NONE") {
    string message = "A Photometric model must be specified before running this program.";
    throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
  }
  wasFound = false;
  if (ui.WasEntered("FROMPVL")) {
    iString phtVal;
    Pvl fromPhtPvl; 
    PvlObject fromPhtObj;
    PvlGroup fromPhtGrp;
    string input = ui.GetFilename("FROMPVL");
    fromPhtPvl.Read(input);
    if (fromPhtPvl.HasObject("PhotometricModel")) {
      fromPhtObj = fromPhtPvl.FindObject("PhotometricModel");
      if (fromPhtObj.HasGroup("Algorithm")) {
        PvlObject::PvlGroupIterator fromPhtGrp = fromPhtObj.BeginGroup();
        if (fromPhtGrp->HasKeyword("PHTNAME")) {
          phtVal = (string)fromPhtGrp->FindKeyword("PHTNAME");
        } else if (fromPhtGrp->HasKeyword("NAME")) {
          phtVal = (string)fromPhtGrp->FindKeyword("NAME");
        } else {
          phtVal = "NONE";
        }
        phtVal = phtVal.UpCase();
        if (phtName == phtVal) {
          wasFound = true;
        }
        if (!wasFound) {
          while (fromPhtGrp != fromPhtObj.EndGroup()) {
            if (fromPhtGrp->HasKeyword("PHTNAME") || fromPhtGrp->HasKeyword("NAME")) {
              if (fromPhtGrp->HasKeyword("PHTNAME")) {
                phtVal = (string)fromPhtGrp->FindKeyword("PHTNAME");
              } else if (fromPhtGrp->HasKeyword("NAME")) {
                phtVal = (string)fromPhtGrp->FindKeyword("NAME");
              } else {
                phtVal = "NONE";
              }
              phtVal = phtVal.UpCase();
              if (phtName == phtVal) {
                wasFound = true;
                break;
              }
            }
            fromPhtGrp++;
          }
        }
      }
    }
    if (wasFound) {
      toPhtPvl.AddObject(fromPhtObj);
    } else {
      toPhtPvl.AddObject(PvlObject("PhotometricModel"));
      toPhtPvl.FindObject("PhotometricModel").AddGroup(PvlGroup("Algorithm"));
      toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("PHTNAME",phtName),Pvl::Replace);
    }
  } else {
    toPhtPvl.AddObject(PvlObject("PhotometricModel"));
    toPhtPvl.FindObject("PhotometricModel").AddGroup(PvlGroup("Algorithm"));
    toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("PHTNAME",phtName),Pvl::Replace);
  }

  if (phtName == "HAPKEHEN" || phtName == "HAPKELEG") {
    if (ui.WasEntered("THETA")) {
      iString keyval = ui.GetString("THETA");
      double theta = keyval.ToDouble();
      toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("THETA",theta),Pvl::Replace);
    } else {
      if (!toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                    HasKeyword("THETA")) {
        string message = "The " + phtName + " Photometric model requires a value for the THETA parameter.";
        message += "The normal range for THETA is: 0 <= THETA <= 90";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("WH")) {
      iString keyval = ui.GetString("WH");
      double wh = keyval.ToDouble();
      toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("WH",wh),Pvl::Replace);
    } else {
      if (!toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                    HasKeyword("WH")) {
        string message = "The " + phtName + " Photometric model requires a value for the WH parameter.";
        message += "The normal range for WH is: 0 < WH <= 1";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("HH")) {
      iString keyval = ui.GetString("HH");
      double hh = keyval.ToDouble();
      toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("HH",hh),Pvl::Replace);
    } else {
      if (!toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                    HasKeyword("HH")) {
        string message = "The " + phtName + " Photometric model requires a value for the HH parameter.";
        message += "The normal range for HH is: 0 <= HH";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("B0")) {
      iString keyval = ui.GetString("B0");
      double b0 = keyval.ToDouble();
      toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("B0",b0),Pvl::Replace);
    } else {
      if (!toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                    HasKeyword("B0")) {
        string message = "The " + phtName + " Photometric model requires a value for the B0 parameter.";
        message += "The normal range for B0 is: 0 <= B0";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (phtName == "HAPKEHEN") {
      if (ui.WasEntered("HG1")) {
        iString keyval = ui.GetString("HG1");
        double hg1 = keyval.ToDouble();
        toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                 AddKeyword(PvlKeyword("HG1",hg1),Pvl::Replace);
      } else {
        if (!toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                      HasKeyword("HG1")) {
          string message = "The " + phtName + " Photometric model requires a value for the HG1 parameter.";
          message += "The normal range for HG1 is: -1 < HG1 < 1";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
      if (ui.WasEntered("HG2")) {
        iString keyval = ui.GetString("HG2");
        double hg2 = keyval.ToDouble();
        toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                 AddKeyword(PvlKeyword("HG2",hg2),Pvl::Replace);
      } else {
        if (!toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                      HasKeyword("HG2")) {
          string message = "The " + phtName + " Photometric model requires a value for the HG2 parameter.";
          message += "The normal range for HG2 is: 0 <= HG2 <= 1";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
    } else {
      if (ui.WasEntered("BH")) {
        iString keyval = ui.GetString("BH");
        double bh = keyval.ToDouble();
        toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                 AddKeyword(PvlKeyword("BH",bh),Pvl::Replace);
      } else {
        if (!toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                      HasKeyword("BH")) {
          string message = "The " + phtName + " Photometric model requires a value for the BH parameter.";
          message += "The normal range for BH is: -1 <= BH <= 1";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
      if (ui.WasEntered("CH")) {
        iString keyval = ui.GetString("CH");
        double ch = keyval.ToDouble();
        toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                 AddKeyword(PvlKeyword("CH",ch),Pvl::Replace);
      } else {
        if (!toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                      HasKeyword("CH")) {
          string message = "The " + phtName + " Photometric model requires a value for the CH parameter.";
          message += "The normal range for CH is: -1 <= CH <= 1";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
    }
  } else if (phtName == "LUNARLAMBERTEMPIRICAL" || phtName == "MINNAERTEMPIRICAL") {
    if (ui.WasEntered("PHASELIST")) {
      iString keyval = ui.GetString("PHASELIST");
      toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("PHASELIST",keyval),Pvl::Replace);
    } else {
      if (!toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                    HasKeyword("PHASELIST")) {
        string message = "The " + phtName + " Photometric model requires a value for the PHASELIST parameter.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("PHASECURVELIST")) {
      iString keyval = ui.GetString("PHASECURVELIST");
      toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("PHASECURVELIST",keyval),Pvl::Replace);
    } else {
      if (!toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                    HasKeyword("PHASECURVELIST")) {
        string message = "The " + phtName + " Photometric model requires a value for the PHASECURVELIST parameter.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (phtName == "LUNARLAMBERTEMPIRICAL") {
      if (ui.WasEntered("LLIST")) {
        iString keyval = ui.GetString("LLIST");
        toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                 AddKeyword(PvlKeyword("LLIST",keyval),Pvl::Replace);
      } else {
        if (!toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                      HasKeyword("LLIST")) {
          string message = "The " + phtName + " Photometric model requires a value for the LLIST parameter.";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
    } else {
      if (ui.WasEntered("KLIST")) {
        iString keyval = ui.GetString("KLIST");
        toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                 AddKeyword(PvlKeyword("KLIST",keyval),Pvl::Replace);
      } else {
        if (!toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                      HasKeyword("KLIST")) {
          string message = "The " + phtName + " Photometric model requires a value for the KLIST parameter.";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
    }
  } else if (phtName == "LUNARLAMBERT") {
    if (ui.WasEntered("L")) {
      iString keyval = ui.GetString("L");
      double l = keyval.ToDouble();
      toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("L",l),Pvl::Replace);
    } else {
      if (!toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                    HasKeyword("L")) {
        string message = "The " + phtName + " Photometric model requires a value for the L parameter.";
        message += "The L parameter has no limited range";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
  } else if (phtName == "MINNAERT") {
    if (ui.WasEntered("K")) {
      iString keyval = ui.GetString("K");
      double k = keyval.ToDouble();
      toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("K",k),Pvl::Replace);
    } else {
      if (!toPhtPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                    HasKeyword("K")) {
        string message = "The " + phtName + " Photometric model requires a value for the K parameter.";
        message += "The normal range for K is: 0 <= K";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
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

  // Set value for maximum emission/incidence angles chosen by user
  maxema = ui.GetDouble("MAXEMISSION");
  maxinc = ui.GetDouble("MAXINCIDENCE");
  usedem = ui.GetBoolean("USEDEM");
  
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
      else if(usedem && (deminc > maxinc || demema > maxema)) {
        out[i] = NULL8;
      }
      else if(!usedem && (ellipsoidinc > maxinc || ellipsoidema > maxema)) {
        out[i] = NULL8;
      }
      // otherwise, do photometric correction
      else {
        pho->Compute(ellipsoidpha, ellipsoidinc, ellipsoidema, deminc, demema, in[i], out[i], mult, base);
      }
    }
  }
}
