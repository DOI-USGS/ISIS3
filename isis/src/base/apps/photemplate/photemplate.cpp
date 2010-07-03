
#include "Isis.h"
#include "PvlGroup.h"
#include "UserInterface.h"
#include "iString.h"

using namespace std;
using namespace Isis;


//functions in the code
void addPhoModel(Pvl &pvl);
void addAtmosModel(Pvl &pvl);
void addNormalModel(Pvl &pvl);

void IsisMain() {
  //The PVL to be written out
  Pvl p;

  //Add the different models to the PVL
  addPhoModel(p);
  addAtmosModel(p);
  addNormalModel(p);

  // Get the output file name from the GUI and write the pvl 
  // to the file. If no extension is given, '.pvl' will be used.
  UserInterface &ui = Application::GetUserInterface(); 
  Filename out = ui.GetFilename("PVL");
  string output = ui.GetFilename("PVL");
  if (out.Extension() == "") {
    output += ".pvl"; 
  }

  p.Write(output);
}

//Function to add photometric model to the PVL
void addPhoModel(Pvl &pvl) {
  //Create an object for the photometric model
  PvlObject phoModel("PhotometricModel");
  //Create an algorithm group
  PvlGroup phoAlgo("Algorithm");

  UserInterface &ui = Application::GetUserInterface();

  //Get the photometric model and any parameters specific to that
  //model and write it to the algorithm group
 
  //Hapke Henyey Greenstein Photometric Model
  if (ui.GetString("PHOTOMETRIC") == "HAPKEHEN") {
    phoAlgo.AddKeyword(PvlKeyword("Name", "Hapkehen"));

    double theta = ui.GetDouble("THETA");
    phoAlgo.AddKeyword(PvlKeyword("Theta", theta));

    double wh = ui.GetDouble("WH");
    phoAlgo.AddKeyword(PvlKeyword("Wh", wh));

    double hg1 = ui.GetDouble("HG1");
    phoAlgo.AddKeyword(PvlKeyword("Hg1", hg1));

    double hg2 = ui.GetDouble("HG2");
    phoAlgo.AddKeyword(PvlKeyword("Hg2", hg2));

    double hh = ui.GetDouble("HH");
    phoAlgo.AddKeyword(PvlKeyword("Hh", hh));

    double b0 = ui.GetDouble("B0");
    phoAlgo.AddKeyword(PvlKeyword("B0", b0));
  }
  //Lunar Lambert Photometric Model
  else if(ui.GetString("PHOTOMETRIC") == "LUNARLAMBERT") {
    phoAlgo.AddKeyword(PvlKeyword("Name", "LunarLambert"));

    double l = ui.GetDouble("L");
    phoAlgo.AddKeyword(PvlKeyword("L", l));
  }
  //Minnaert Photometric Model
  else if(ui.GetString("PHOTOMETRIC") == "MINNAERT") {
    phoAlgo.AddKeyword(PvlKeyword("Name", "Minnaert"));

    double k = ui.GetDouble("K");
    phoAlgo.AddKeyword(PvlKeyword("K", k));
  }
  //Lambert Photometric Model
  else if(ui.GetString("PHOTOMETRIC") == "LAMBERT") {
    phoAlgo.AddKeyword(PvlKeyword("Name", "Lambert"));
  }
  //Lommel Seeliger Photometric Model
  else if(ui.GetString("PHOTOMETRIC") == "LOMMELSEELIGER") {
    phoAlgo.AddKeyword(PvlKeyword("Name", "LommelSeeliger"));
  }
  //Lunar Lambert McEwen Photometric Model
  else if(ui.GetString("PHOTOMETRIC") == "LUNARLAMBERTMCEWEN") {
    phoAlgo.AddKeyword(PvlKeyword("Name", "LunarLambertMcEwen"));
  }

  //Add the algorithm group to the photometric model object and add it to the PVL
  phoModel.AddGroup(phoAlgo);
  pvl.AddObject(phoModel);
}

//Function to add atmospheric model to the PVL
void addAtmosModel(Pvl &pvl) {
  UserInterface &ui = Application::GetUserInterface();

  //If the normalization model is one with an atmospheric model
  //then create an atmospheric model and add it to the PVL.
  if((ui.GetString("NORMALIZATION") == "ATMALBEDO" | 
     ui.GetString("NORMALIZATION") == "ATMSHADE" ||
     ui.GetString("NORMALIZATION") == "ATMTOPO")) {

    //Create an object for the atmospheric model
    PvlObject atmosModel("AtmosphericModel");
    //Create an algorithm group
    PvlGroup atmosAlgo("Algorithm");

    //Get the atmospheric model and any parameters specific to that
    //model and write it to the algorithm group

    //Anisotropic 1 Atmospheric Model
    if (ui.GetString("ATMOSPHERIC") == "ANISOTROPIC1") {
      atmosAlgo.AddKeyword(PvlKeyword("Name", "Anisotropic1"));

      bool nulneg = ui.GetBoolean("NULNEG");

      //if NULNEG is checked add it to the group, otherwise the 
      //default is to leave it out
      if(nulneg) {
        atmosAlgo.AddKeyword(PvlKeyword("Nulneg", "YES"));
      }

      double tau = ui.GetDouble("TAU");
      atmosAlgo.AddKeyword(PvlKeyword("Tau", tau));

      double tauref = ui.GetDouble("TAUREF");
      atmosAlgo.AddKeyword(PvlKeyword("Tauref", tauref));

      double wha = ui.GetDouble("WHA");
      atmosAlgo.AddKeyword(PvlKeyword("Wha", wha));

      //if WHAREF was entered add it to the group, otherwise the
      //default is to leave it out
      if(ui.WasEntered("WHAREF")) {
        double wharef = ui.GetDouble("WHAREF");
        atmosAlgo.AddKeyword(PvlKeyword("Wharef", wharef));
      }

      double bha = ui.GetDouble("BHA");
      atmosAlgo.AddKeyword(PvlKeyword("Bha", bha));

      //if BHAREF was entered add it to the group, otherwise the
      //default is to leave it out
      if(ui.WasEntered("BHAREF")) {
        double bharef = ui.GetDouble("BHAREF");
        atmosAlgo.AddKeyword(PvlKeyword("Bharef", bharef));
      }

      double hnorm = ui.GetDouble("HNORM");
      atmosAlgo.AddKeyword(PvlKeyword("Hnorm", hnorm));
    }
    //Anisotropic 2 Atmospheric Model
    else if(ui.GetString("ATMOSPHERIC") == "ANISOTROPIC2") {
      atmosAlgo.AddKeyword(PvlKeyword("Name", "Anisotropic2"));

      bool nulneg = ui.GetBoolean("NULNEG");

      //if NULNEG is checked add it to the group, otherwise the 
      //default is to leave it out
      if(nulneg) {
        atmosAlgo.AddKeyword(PvlKeyword("Nulneg", "Yes"));
      }

      double tau = ui.GetDouble("TAU");
      atmosAlgo.AddKeyword(PvlKeyword("Tau", tau));

      double tauref = ui.GetDouble("TAUREF");
      atmosAlgo.AddKeyword(PvlKeyword("Tauref", tauref));

      double wha = ui.GetDouble("WHA");
      atmosAlgo.AddKeyword(PvlKeyword("Wha", wha));

      //if WHAREF was entered add it to the group, otherwise the
      //default is to leave it out
      if(ui.WasEntered("WHAREF")) {
        double wharef = ui.GetDouble("WHAREF");
        atmosAlgo.AddKeyword(PvlKeyword("Wharef", wharef));
      }

      double bha = ui.GetDouble("BHA");
      atmosAlgo.AddKeyword(PvlKeyword("Bha", bha));

      //if BHAREF was entered add it to the group, otherwise the
      //default is to leave it out
      if(ui.WasEntered("BHAREF")) {
        double bharef = ui.GetDouble("BHAREF");
        atmosAlgo.AddKeyword(PvlKeyword("Bharef", bharef));
      }

      double hnorm = ui.GetDouble("HNORM");
      atmosAlgo.AddKeyword(PvlKeyword("Hnorm", hnorm));
    }
    //Hapke 1 Atmospheric Model
    else if(ui.GetString("ATMOSPHERIC") == "HAPKEATM1") {
      atmosAlgo.AddKeyword(PvlKeyword("Name", "HapkeAtm1"));

      bool nulneg = ui.GetBoolean("NULNEG");

      //if NULNEG is checked add it to the group, otherwise the 
      //default is to leave it out
      if(nulneg) {
        atmosAlgo.AddKeyword(PvlKeyword("Nulneg", "Yes"));
      }

      double tau = ui.GetDouble("TAU");
      atmosAlgo.AddKeyword(PvlKeyword("Tau", tau));

      double tauref = ui.GetDouble("TAUREF");
      atmosAlgo.AddKeyword(PvlKeyword("Tauref", tauref));

      double wha = ui.GetDouble("WHA");
      atmosAlgo.AddKeyword(PvlKeyword("Wha", wha));

      //if WHAREF was entered add it to the group, otherwise the
      //default is to leave it out
      if(ui.WasEntered("WHAREF")) {
        double wharef = ui.GetDouble("WHAREF");
        atmosAlgo.AddKeyword(PvlKeyword("Wharef", wharef));
      }

      double hga = ui.GetDouble("HGA");
      atmosAlgo.AddKeyword(PvlKeyword("Hga", hga));

      //if HGAREF was entered add it to the group, otherwise the
      //default is to leave it out
      if(ui.WasEntered("HGAREF")) {
        double hgaref = ui.GetDouble("HGAREF");
        atmosAlgo.AddKeyword(PvlKeyword("Hgaref", hgaref));
      }

      double hnorm = ui.GetDouble("HNORM");
      atmosAlgo.AddKeyword(PvlKeyword("Hnorm", hnorm));
    }
    //Hapke 2 Atmospheric Model
    else if(ui.GetString("ATMOSPHERIC") == "HAPKEATM2") {
      atmosAlgo.AddKeyword(PvlKeyword("Name", "HapkeAtm2"));

      bool nulneg = ui.GetBoolean("NULNEG");

      //if NULNEG is checked add it to the group, otherwise the 
      //default is to leave it out
      if(nulneg) {
        atmosAlgo.AddKeyword(PvlKeyword("Nulneg", "Yes"));
      }

      double tau = ui.GetDouble("TAU");
      atmosAlgo.AddKeyword(PvlKeyword("Tau", tau));

      double tauref = ui.GetDouble("TAUREF");
      atmosAlgo.AddKeyword(PvlKeyword("Tauref", tauref));

      double wha = ui.GetDouble("WHA");
      atmosAlgo.AddKeyword(PvlKeyword("Wha", wha));

      //if WHAREF was entered add it to the group, otherwise the
      //default is to leave it out
      if(ui.WasEntered("WHAREF")) {
        double wharef = ui.GetDouble("WHAREF");
        atmosAlgo.AddKeyword(PvlKeyword("Wharef", wharef));
      }

      double hga = ui.GetDouble("HGA");
      atmosAlgo.AddKeyword(PvlKeyword("Hga", hga));

      //if HGAREF was entered add it to the group, otherwise the
      //default is to leave it out
      if(ui.WasEntered("HGAREF")) {
        double hgaref = ui.GetDouble("HGAREF");
        atmosAlgo.AddKeyword(PvlKeyword("Hgaref", hgaref));
      }

      double hnorm = ui.GetDouble("HNORM");
      atmosAlgo.AddKeyword(PvlKeyword("Hnorm", hnorm));
    }
    //Isotropic 1 Atmospheric Model
    else if(ui.GetString("ATMOSPHERIC") == "ISOTROPIC1") {
      atmosAlgo.AddKeyword(PvlKeyword("Name", "Isotropic1"));

      bool nulneg = ui.GetBoolean("NULNEG");

      //if NULNEG is checked add it to the group, otherwise the 
      //default is to leave it out
      if(nulneg) {
        atmosAlgo.AddKeyword(PvlKeyword("Nulneg", "Yes"));
      }

      double tau = ui.GetDouble("TAU");
      atmosAlgo.AddKeyword(PvlKeyword("Tau", tau));

      double tauref = ui.GetDouble("TAUREF");
      atmosAlgo.AddKeyword(PvlKeyword("Tauref", tauref));

      double wha = ui.GetDouble("WHA");
      atmosAlgo.AddKeyword(PvlKeyword("Wha", wha));

      //if WHAREF was entered add it to the group, otherwise the
      //default is to leave it out
      if(ui.WasEntered("WHAREF")) {
        double wharef = ui.GetDouble("WHAREF");
        atmosAlgo.AddKeyword(PvlKeyword("Wharef", wharef));
      }

      double hnorm = ui.GetDouble("HNORM");
      atmosAlgo.AddKeyword(PvlKeyword("Hnorm", hnorm));
    }
    //Isotropic 2 Atmospheric Model
    else if(ui.GetString("ATMOSPHERIC") == "ISOTROPIC2") {
      atmosAlgo.AddKeyword(PvlKeyword("Name", "Isotropic2"));

      bool nulneg = ui.GetBoolean("NULNEG");

      //if NULNEG is checked add it to the group, otherwise the 
      //default is to leave it out
      if(nulneg) {
        atmosAlgo.AddKeyword(PvlKeyword("Nulneg", "Yes"));
      }

      double tau = ui.GetDouble("TAU");
      atmosAlgo.AddKeyword(PvlKeyword("Tau", tau));

      double tauref = ui.GetDouble("TAUREF");
      atmosAlgo.AddKeyword(PvlKeyword("Tauref", tauref));

      double wha = ui.GetDouble("WHA");
      atmosAlgo.AddKeyword(PvlKeyword("Wha", wha));

      //if WHAREF was entered add it to the group, otherwise the
      //default is to leave it out
      if(ui.WasEntered("WHAREF")) {
        double wharef = ui.GetDouble("WHAREF");
        atmosAlgo.AddKeyword(PvlKeyword("Wharef", wharef));
      }

      double hnorm = ui.GetDouble("HNORM");
      atmosAlgo.AddKeyword(PvlKeyword("Hnorm", hnorm));
    }

    //Add the algorithm group to the atmospheric model object and add it to the PVL
    atmosModel.AddGroup(atmosAlgo);
    pvl.AddObject(atmosModel);
  }
}

//Function to add normalization model to the PVL
void addNormalModel(Pvl &pvl) {
  //Create an object for the normalization model
  PvlObject normalModel("NormalizationModel");
  //Create an algorithm group
  PvlGroup normalAlgo("Algorithm");

  UserInterface &ui = Application::GetUserInterface();

  //Get the normalization model and any parameters specific to that
  //model and write it to the algorithm group

  //Albedo Normalization Model
  if (ui.GetString("NORMALIZATION") == "ALBEDO") {
    normalAlgo.AddKeyword(PvlKeyword("Name", "Albedo"));

    double incref = ui.GetDouble("INCREF");
    normalAlgo.AddKeyword(PvlKeyword("Incref", incref));

    double incmat = ui.GetDouble("INCMAT");
    normalAlgo.AddKeyword(PvlKeyword("Incmat", incmat));

    double thresh = ui.GetDouble("THRESH");
    normalAlgo.AddKeyword(PvlKeyword("Thresh", thresh));

    double albedo = ui.GetDouble("ALBEDO");
    normalAlgo.AddKeyword(PvlKeyword("Albedo", albedo));
  }
  //Mixed Normalization Model
  else if(ui.GetString("NORMALIZATION") == "MIXED") {
    normalAlgo.AddKeyword(PvlKeyword("Name", "Mixed"));

    double incref = ui.GetDouble("INCREF");
    normalAlgo.AddKeyword(PvlKeyword("Incref", incref));

    double incmat = ui.GetDouble("INCMAT");
    normalAlgo.AddKeyword(PvlKeyword("Incmat", incmat));

    double thresh = ui.GetDouble("THRESH");
    normalAlgo.AddKeyword(PvlKeyword("Thresh", thresh));

    double albedo = ui.GetDouble("ALBEDO");
    normalAlgo.AddKeyword(PvlKeyword("Albedo", albedo));
  }
  //Moon Albedo Normalization Model
  else if(ui.GetString("NORMALIZATION") == "MOONALBEDO") {
    normalAlgo.AddKeyword(PvlKeyword("Name", "MoonAlbedo"));

    double d = ui.GetDouble("D");
    normalAlgo.AddKeyword(PvlKeyword("D", d));

    double e = ui.GetDouble("E");
    normalAlgo.AddKeyword(PvlKeyword("E", e));

    double f = ui.GetDouble("F");
    normalAlgo.AddKeyword(PvlKeyword("F", f));

    double g2 = ui.GetDouble("G2");
    normalAlgo.AddKeyword(PvlKeyword("G2", g2));

    double h = ui.GetDouble("H");
    normalAlgo.AddKeyword(PvlKeyword("H", h));

    double xmul = ui.GetDouble("XMUL");
    normalAlgo.AddKeyword(PvlKeyword("Xmul", xmul));

    double wl = ui.GetDouble("WL");
    normalAlgo.AddKeyword(PvlKeyword("Wl", wl));

    double bsh1 = ui.GetDouble("BSH1");
    normalAlgo.AddKeyword(PvlKeyword("Bsh1", bsh1));

    double xb1 = ui.GetDouble("XB1");
    normalAlgo.AddKeyword(PvlKeyword("Xb1", xb1));

    double xb2 = ui.GetDouble("XB2");
    normalAlgo.AddKeyword(PvlKeyword("Xb2", xb2));
  }
  //Shade Normalization Model
  else if(ui.GetString("NORMALIZATION") == "SHADE") {
    normalAlgo.AddKeyword(PvlKeyword("Name", "Shade"));

    double incref = ui.GetDouble("INCREF");
    normalAlgo.AddKeyword(PvlKeyword("Incref", incref));

    double albedo = ui.GetDouble("ALBEDO");
    normalAlgo.AddKeyword(PvlKeyword("Albedo", albedo));
  }
  //Topographic Normalization Model
  else if(ui.GetString("NORMALIZATION") == "TOPO") {
    normalAlgo.AddKeyword(PvlKeyword("Name", "Topo"));

    double incref = ui.GetDouble("INCREF");
    normalAlgo.AddKeyword(PvlKeyword("Incref", incref));

    double thresh = ui.GetDouble("THRESH");
    normalAlgo.AddKeyword(PvlKeyword("Thresh", thresh));

    double albedo = ui.GetDouble("ALBEDO");
    normalAlgo.AddKeyword(PvlKeyword("Albedo", albedo));
  }
  //Albedo Atmospheric Normalization Model
  else if(ui.GetString("NORMALIZATION") == "ATMALBEDO") {
    normalAlgo.AddKeyword(PvlKeyword("Name", "AlbedoAtm"));

    double incref = ui.GetDouble("INCREF");
    normalAlgo.AddKeyword(PvlKeyword("Incref", incref));
  }
  else if(ui.GetString("NORMALIZATION") == "ATMSHADE") {
    normalAlgo.AddKeyword(PvlKeyword("Name", "ShadeAtm"));

    double incref = ui.GetDouble("INCREF");
    normalAlgo.AddKeyword(PvlKeyword("Incref", incref));

    double albedo = ui.GetDouble("ALBEDO");
    normalAlgo.AddKeyword(PvlKeyword("Albedo", albedo));
  }
  //Topographic Atmospheric Normalization Model
  else if(ui.GetString("NORMALIZATION") == "ATMTOPO") {
    normalAlgo.AddKeyword(PvlKeyword("Name", "TopoAtm"));

    double incref = ui.GetDouble("INCREF");
    normalAlgo.AddKeyword(PvlKeyword("Incref", incref));

    double albedo = ui.GetDouble("ALBEDO");
    normalAlgo.AddKeyword(PvlKeyword("Albedo", albedo));
  }
  //No Normalization
  else if(ui.GetString("NORMALIZATION") == "NONORMALIZATION") {
    normalAlgo.AddKeyword(PvlKeyword("Name", "NoNormalization"));
  }

  //Add the algorithm group to the normalization model object and add it to the PVL
  normalModel.AddGroup(normalAlgo);
  pvl.AddObject(normalModel);
}



