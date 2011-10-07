#define GUIHELPERS

#include "Isis.h"
#include <sstream>
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
  std::stringstream os;
  UserInterface &ui = Application::GetUserInterface();
  string inFile(ui.GetFilename("FROMPVL"));
  Pvl inPvl;
  inPvl.Read(inFile);
  iString phtName = ui.GetAsString("PHTNAME");
  phtName = phtName.UpCase();
  iString atmName = ui.GetAsString("ATMNAME");
  atmName = atmName.UpCase();
  if (phtName == "NONE" && atmName == "NONE") {
    string message = "A photometric model or an atmospheric model must be specified before it can be loaded from the PVL";
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
          string message = "The FROMPVL file must have a PHTNAME or NAME keyword specifying the photometric model type";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
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
                string message = "The FROMPVL file must have a PHTNAME or NAME keyword specifying the photometric model type";
                throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
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
            string message = "The FROMPVL file must have a PHTNAME or NAME keyword specifying the photometric model type";
            throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
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
          } else if (phtVal == "MINNAERT") {
            if (phtGrp->HasKeyword("K")) {
              double k = phtGrp->FindKeyword("K");
              os.str("");
              os << k;
              ui.PutAsString("K", os.str());
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
  if (atmName == "NONE") {
    return;
  }
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
        string message = "The FROMPVL file must have a ATMNAME or NAME keyword specifying the atmospheric model type";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
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
              string message = "The FROMPVL file must have a ATMNAME or NAME keyword specifying the atmospheric model type";
              throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
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
        string message = "The FROMPVL file must have a ATMNAME or NAME keyword specifying the atmospheric model type";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
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

  //Check to make sure that a model was specified
  if (ui.GetAsString("PHTNAME") == "NONE" && ui.GetAsString("ATMNAME") == "NONE") {
    string message = "A photometric model or an atmospheric model must be specified before running this program";
    throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
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
  iString phtName = ui.GetAsString("PHTNAME");
  phtName = phtName.UpCase();
  iString phtVal;
  PvlObject pvlObj;
  PvlGroup pvlGrp;
  if (pvl.HasObject("PhotometricModel")) {
    pvlObj = pvl.FindObject("PhotometricModel");
    if (pvlObj.HasGroup("Algorithm")) {
      PvlObject::PvlGroupIterator pvlGrp = pvlObj.BeginGroup();
      if (pvlGrp->HasKeyword("PHTNAME")) {
        phtVal = (string)pvlGrp->FindKeyword("PHTNAME");
      } else if (pvlGrp->HasKeyword("NAME")) {
        phtVal = (string)pvlGrp->FindKeyword("NAME");
      } else {
        string message = "The FROMPVL file must have a PHTNAME or NAME keyword specifying the photometric model type";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
      phtVal = phtVal.UpCase();
      if (phtName == phtVal) {
        wasFound = true;
      }
      if (!wasFound) {
        while (pvlGrp != pvlObj.EndGroup()) {
          if (pvlGrp->HasKeyword("PHTNAME") || pvlGrp->HasKeyword("NAME")) {
            if (pvlGrp->HasKeyword("PHTNAME")) {
              phtVal = (string)pvlGrp->FindKeyword("PHTNAME");
            } else if (pvlGrp->HasKeyword("NAME")) {
              phtVal = (string)pvlGrp->FindKeyword("NAME");
            } else {
              string message = "The FROMPVL file must have a PHTNAME or NAME keyword specifying the photometric model type";
              throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
            }
            phtVal = phtVal.UpCase();
            if (phtName == phtVal) {
              wasFound = true;
              break;
            }
          }
          pvlGrp++;
        }
      }
    }
    if (wasFound) {
      outPvl.AddObject(pvlObj);
    } else {
      outPvl.AddObject(PvlObject("PhotometricModel"));
      outPvl.FindObject("PhotometricModel").AddGroup(PvlGroup("Algorithm"));
      outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("PHTNAME",phtName),Pvl::Replace);
    }
  } else {
    outPvl.AddObject(PvlObject("PhotometricModel"));
    outPvl.FindObject("PhotometricModel").AddGroup(PvlGroup("Algorithm"));
    outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
           AddKeyword(PvlKeyword("PHTNAME",phtName),Pvl::Replace);
  }

  //Get the photometric model and any parameters specific to that
  //model and write it to the algorithm group

  //Hapke Photometric Models
  if(phtName == "HAPKEHEN" || phtName == "HAPKELEG") {
    if (ui.WasEntered("THETA")) {
      iString keyval = ui.GetString("THETA");
      double theta = keyval.ToDouble();
      outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("THETA",theta),Pvl::Replace);
    } else {
      if (!outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                  HasKeyword("THETA")) {
        string message = "The " + phtName + " Photometric model requires a value for the THETA parameter.";
        message += "The normal range for THETA is: 0 <= THETA <= 90";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("WH")) {
      iString keyval = ui.GetString("WH");
      double wh = keyval.ToDouble();
      outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("WH",wh),Pvl::Replace);
    } else {
      if (!outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                  HasKeyword("WH")) {
        string message = "The " + phtName + " Photometric model requires a value for the WH parameter.";
        message += "The normal range for WH is: 0 < WH <= 1";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("HH")) {
      iString keyval = ui.GetString("HH");
      double hh = keyval.ToDouble();
      outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("HH",hh),Pvl::Replace);
    } else {
      if (!outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                  HasKeyword("HH")) {
        string message = "The " + phtName + " Photometric model requires a value for the HH parameter.";
        message += "The normal range for HH is: 0 <= HH";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("B0")) {
      iString keyval = ui.GetString("B0");
      double b0 = keyval.ToDouble();
      outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("B0",b0),Pvl::Replace);
    } else {
      if (!outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
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
        outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("HG1",hg1),Pvl::Replace);
      } else {
        if (!outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                    HasKeyword("HG1")) {
          string message = "The " + phtName + " Photometric model requires a value for the HG1 parameter.";
          message += "The normal range for HG1 is: -1 < HG1 < 1";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
      if (ui.WasEntered("HG2")) {
        iString keyval = ui.GetString("HG2");
        double hg2 = keyval.ToDouble();
        outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("HG2",hg2),Pvl::Replace);
      } else {
        if (!outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
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
        outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("BH",bh),Pvl::Replace);
      } else {
        if (!outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                    HasKeyword("BH")) {
          string message = "The " + phtName + " Photometric model requires a value for the BH parameter.";
          message += "The normal range for BH is: -1 <= BH <= 1";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
      if (ui.WasEntered("CH")) {
        iString keyval = ui.GetString("CH");
        double ch = keyval.ToDouble();
        outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("CH",ch),Pvl::Replace);
      } else {
        if (!outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                    HasKeyword("CH")) {
          string message = "The " + phtName + " Photometric model requires a value for the CH parameter.";
          message += "The normal range for CH is: -1 <= CH <= 1";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
    }

  }
  //Lunar Lambert Empirical and Minnaert Empirical Photometric Models
  else if (phtName == "LUNARLAMBERTEMPIRICAL" || phtName == "MINNAERTEMPIRICAL") {
    if (ui.WasEntered("PHASELIST")) {
      iString keyval = ui.GetString("PHASELIST");
      outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("PHASELIST",keyval),Pvl::Replace);
    } else {
      if (!outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                    HasKeyword("PHASELIST")) {
        string message = "The " + phtName + " Photometric model requires a value for the PHASELIST parameter.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("PHASECURVELIST")) {
      iString keyval = ui.GetString("PHASECURVELIST");
      outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("PHASECURVELIST",keyval),Pvl::Replace);
    } else {
      if (!outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                  HasKeyword("PHASECURVELIST")) {
        string message = "The " + phtName + " Photometric model requires a value for the PHASECURVELIST parameter.";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (phtName == "LUNARLAMBERTEMPIRICAL") {
      if (ui.WasEntered("LLIST")) {
        iString keyval = ui.GetString("LLIST");
        outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("LLIST",keyval),Pvl::Replace);
      } else {
        if (!outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                    HasKeyword("LLIST")) {
          string message = "The " + phtName + " Photometric model requires a value for the LLIST parameter.";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
    } else {
      if (ui.WasEntered("KLIST")) {
        iString keyval = ui.GetString("KLIST");
        outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
               AddKeyword(PvlKeyword("KLIST",keyval),Pvl::Replace);
      } else {
        if (!outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                    HasKeyword("KLIST")) {
          string message = "The " + phtName + " Photometric model requires a value for the KLIST parameter.";
          throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
        }
      }
    }
  }
  //Lunar Lambert Photometric Model
  else if (phtName == "LUNARLAMBERT") {
    if (ui.WasEntered("L")) {
      iString keyval = ui.GetString("L");
      double l = keyval.ToDouble();
      outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("L",l),Pvl::Replace);
    } else {
      if (!outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                  HasKeyword("L")) {
        string message = "The " + phtName + " Photometric model requires a value for the L parameter.";
        message += "The L parameter has no limited range";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
  }
  //Minnaert Photometric Model
  else if (phtName == "MINNAERT") {
    if (ui.WasEntered("K")) {
      iString keyval = ui.GetString("K");
      double k = keyval.ToDouble();
      outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("K",k),Pvl::Replace);
    } else {
      if (!outPvl.FindObject("PhotometricModel").FindGroup("Algorithm").
                  HasKeyword("K")) {
        string message = "The " + phtName + " Photometric model requires a value for the K parameter.";
        message += "The normal range for K is: 0 <= K";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
  }
}

//Function to add atmospheric model to the PVL
void addAtmosModel(Pvl &pvl, Pvl &outPvl) {
  UserInterface &ui = Application::GetUserInterface();

  bool wasFound = false;
  iString atmName = ui.GetAsString("ATMNAME");
  atmName = atmName.UpCase();
  iString atmVal;
  PvlObject pvlObj;
  PvlGroup pvlGrp;
  if (pvl.HasObject("AtmosphericModel")) {
    pvlObj = pvl.FindObject("AtmosphericModel");
    if (pvlObj.HasGroup("Algorithm")) {
      PvlObject::PvlGroupIterator pvlGrp = pvlObj.BeginGroup();
      if (pvlGrp->HasKeyword("ATMNAME")) {
        atmVal = (string)pvlGrp->FindKeyword("ATMNAME");
      } else if (pvlGrp->HasKeyword("NAME")) {
        atmVal = (string)pvlGrp->FindKeyword("NAME");
      } else { 
        string message = "The FROMPVL file must have a ATMNAME or NAME keyword specifying the atmospheric model type";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
      atmVal = atmVal.UpCase();
      if (atmName == atmVal) {
        wasFound = true;
      }
      if (!wasFound) {
        while (pvlGrp != pvlObj.EndGroup()) {
          if (pvlGrp->HasKeyword("ATMNAME") || pvlGrp->HasKeyword("NAME")) {
            if (pvlGrp->HasKeyword("ATMNAME")) {
              atmVal = (string)pvlGrp->FindKeyword("ATMNAME");
            } else if (pvlGrp->HasKeyword("NAME")) {
              atmVal = (string)pvlGrp->FindKeyword("NAME");
            } else { 
              string message = "The FROMPVL file must have a ATMNAME or NAME keyword specifying the atmospheric model type";
              throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
            }
            atmVal = atmVal.UpCase();
            if (atmName == atmVal) {
              wasFound = true;
              break;
            }
          }
          pvlGrp++;
        }
      }
    }
    if (wasFound) {
      outPvl.AddObject(pvlObj);
    } else {
      outPvl.AddObject(PvlObject("AtmosphericModel"));
      outPvl.FindObject("AtmosphericModel").AddGroup(PvlGroup("Algorithm"));
      outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("ATMNAME",atmName),Pvl::Replace);
    }
  } else {
    outPvl.AddObject(PvlObject("AtmosphericModel"));
    outPvl.FindObject("AtmosphericModel").AddGroup(PvlGroup("Algorithm"));
    outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
           AddKeyword(PvlKeyword("ATMNAME",atmName),Pvl::Replace);
  }

  //Get the atmospheric model and any parameters specific to that
  //model and write it to the algorithm group

  if (atmName == "ANISOTROPIC1" || atmName == "ANISOTROPIC2" ||
      atmName == "HAPKEATM1" || atmName == "HAPKEATM2" ||
      atmName == "ISOTROPIC1" || atmName == "ISOTROPIC2") {
    if (ui.WasEntered("HNORM")) {
      iString keyval = ui.GetString("HNORM");
      double hnorm = keyval.ToDouble();
      outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("HNORM",hnorm),Pvl::Replace);
    } else {
      if (!outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                  HasKeyword("HNORM")) {
        string message = "The " + atmName + " Atmospheric model requires a value for the HNORM parameter.";
        message += "The normal range for HNORM is: 0 <= HNORM";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("TAU")) {
      iString keyval = ui.GetString("TAU");
      double tau = keyval.ToDouble();
      outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("TAU",tau),Pvl::Replace);
    } else {
      if (!outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                  HasKeyword("TAU")) {
        string message = "The " + atmName + " Atmospheric model requires a value for the TAU parameter.";
        message += "The normal range for TAU is: 0 <= TAU";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("TAUREF")) {
      iString keyval = ui.GetString("TAUREF");
      double tauref = keyval.ToDouble();
      outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("TAUREF",tauref),Pvl::Replace);
    } else {
      if (!outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                  HasKeyword("TAUREF")) {
        string message = "The " + atmName + " Atmospheric model requires a value for the TAUREF parameter.";
        message += "The normal range for TAUREF is: 0 <= TAUREF";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.WasEntered("WHA")) {
      iString keyval = ui.GetString("WHA");
      double wha = keyval.ToDouble();
      outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("WHA",wha),Pvl::Replace);
    } else {
      if (!outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                  HasKeyword("WHA")) {
        string message = "The " + atmName + " Atmospheric model requires a value for the WHA parameter.";
        message += "The normal range for WHA is: 0 < WHA < 1";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    if (ui.GetString("NULNEG") == "YES") {
      outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("NULNEG","YES"),Pvl::Replace);
    } else if (ui.GetString("NULNEG") == "NO") {
      outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("NULNEG","NO"),Pvl::Replace);
    } else {
      if (!outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
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
      outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("BHA",bha),Pvl::Replace);
    } else {
      if (!outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
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
      outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
             AddKeyword(PvlKeyword("HGA",hga),Pvl::Replace);
    } else {
      if (!outPvl.FindObject("AtmosphericModel").FindGroup("Algorithm").
                  HasKeyword("HGA")) {
        string message = "The " + atmName + " Atmospheric model requires a value for the HGA parameter.";
        message += "The normal range for HGA is: -1 < HGA < 1";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
  }
}
