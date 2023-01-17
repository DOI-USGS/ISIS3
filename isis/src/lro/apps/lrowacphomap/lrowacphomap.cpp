#include <string>

#include "HapkeLRO.h"
#include "HapkeLROC.h"
#include "IException.h"
#include "PhotometricFunction.h"
#include "ProcessByBrick.h"
#include "PvlGroup.h"
#include "SpecialPixel.h"

#include "lrowacphomap.h"

using namespace std;
using namespace Isis;


namespace Isis {
  Pvl lrowacphomap(UserInterface &ui) {
    // Set up the input cube
    Cube *icube = new Cube();
    CubeAttributeInput inAtt = ui.GetInputAttribute("FROM");
    if (inAtt.bands().size() != 0) {
      icube->setVirtualBands(inAtt.bands());
    }
    icube->open(ui.GetCubeName("FROM"));

    Pvl log = lrowacphomap(icube, ui);

    delete icube;
    icube = NULL;

    return log;
  }

  Pvl lrowacphomap(Cube *icube, UserInterface &ui) {
    Pvl log;

    HapkeLRO *phoLro = NULL;
    HapkeLROC *phoLroc = NULL;
    QString algoName;

    bool normalized;
    bool useDem;
    bool photometryOnly;

    ProcessByBrick p;

    p.SetInputCube(icube);

    // Set up the output cube
    Cube *ocube = p.SetOutputCube(ui.GetCubeName("TO"), ui.GetOutputAttribute("TO"));

    bool useBackplane = false;

    if (ui.WasEntered("BACKPLANE")) {
      if (icube->bandCount() != 1) {
        string msg = "Invalid Image: The backplane option can only be used with a single image band at a time.";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      CubeAttributeInput backplaneCai = ui.GetInputAttribute("BACKPLANE");

      vector<QString> bands = backplaneCai.bands();

      if (bands.size() == 0) {
        bands.clear();
        bands.push_back("1");
        bands.push_back("2");
        bands.push_back("3");
        bands.push_back("4");
        bands.push_back("5");
      }
      else if (bands.size() != 5) {
        string msg = "Invalid Backplane: The backplane must be exactly 5 bands";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      CubeAttributeInput cai;
      cai.setAttributes("+" + bands[0]);
      p.SetInputCube(ui.GetCubeName("BACKPLANE"), cai);
      cai.setAttributes("+" + bands[1]);
      p.SetInputCube(ui.GetCubeName("BACKPLANE"), cai);
      cai.setAttributes("+" + bands[2]);
      p.SetInputCube(ui.GetCubeName("BACKPLANE"), cai);
      cai.setAttributes("+" + bands[3]);
      p.SetInputCube(ui.GetCubeName("BACKPLANE"), cai);
      cai.setAttributes("+" + bands[4]);
      p.SetInputCube(ui.GetCubeName("BACKPLANE"), cai);

      useBackplane = true;
    }

    // Get the name of the parameter files
    Pvl par(ui.GetFileName("PHOALGO"));
    Cube *parCube = new Cube();
    CubeAttributeInput parCubeAtt = ui.GetInputAttribute("PHOPARCUBE");
    if (parCubeAtt.bands().size() != 0) {
      parCube->setVirtualBands(parCubeAtt.bands());
    }
    parCube->open(ui.GetCubeName("PHOPARCUBE"));

    p.SetBrickSize(128, 128, icube->bandCount());

    algoName = PhotometricFunction::algorithmName(par).toUpper();

    photometryOnly = ui.GetBoolean("PHOTOMETRYONLY");
    normalized = ui.GetBoolean("NORMALIZED");

    if (algoName == "HAPKELRO") {
      phoLro = new HapkeLRO(par, *icube, !useBackplane, parCube);
      phoLro->setMinimumPhaseAngle(ui.GetDouble("MINPHASE"));
      phoLro->setMaximumPhaseAngle(ui.GetDouble("MAXPHASE"));
      phoLro->setMinimumEmissionAngle(ui.GetDouble("MINEMISSION"));
      phoLro->setMaximumEmissionAngle(ui.GetDouble("MAXEMISSION"));
      phoLro->setMinimumIncidenceAngle(ui.GetDouble("MININCIDENCE"));
      phoLro->setMaximumIncidenceAngle(ui.GetDouble("MAXINCIDENCE"));
      
      phoLro->setNormalized(normalized);
    }
    else if (algoName == "HAPKELROC") {
      phoLroc = new HapkeLROC(par, *icube, !useBackplane, parCube);
      phoLroc->setMinimumPhaseAngle(ui.GetDouble("MINPHASE"));
      phoLroc->setMaximumPhaseAngle(ui.GetDouble("MAXPHASE"));
      phoLroc->setMinimumEmissionAngle(ui.GetDouble("MINEMISSION"));
      phoLroc->setMaximumEmissionAngle(ui.GetDouble("MAXEMISSION"));
      phoLroc->setMinimumIncidenceAngle(ui.GetDouble("MININCIDENCE"));
      phoLroc->setMaximumIncidenceAngle(ui.GetDouble("MAXINCIDENCE"));

      phoLroc->setNormalized(normalized);
    }
    else {
      QString msg = " Algorithm Name [" + algoName + "] not recognized. ";
      msg += "Compatible Algorithms are:\n    HapkeLRO\n    HapkeLROC";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Determine how photometric angles should be calculated
    useDem = ui.GetBoolean("USEDEM");

    /**
    * @brief Apply photometric correction
    *
    * Short function dispatched for each brick to apply the photometrc
    * correction function.
    *
    * @author 2021-08-02 Cordell Michaud
    * 
    * @internal
    *   @history 2021-07-19 Cordell Michaud - Code adapted from lrowacphomap by Kris Becker
    *
    * @param in Buffer containing input data
    * @param out Buffer of photometrically corrected data
    */
    auto phoCal = [&phoLro, &phoLroc, algoName, useDem, photometryOnly](Buffer &in, Buffer &out) -> void {
      for (int i = 0; i < in.size(); i++) {
        //  Don't correct special pixels
        if (IsSpecial(in[i])) {
          out[i] = in[i];
        }
        else {
          // Get correction and test for validity
          double ph = Null;
          if (algoName == "HAPKELRO") {
            ph = phoLro->compute(in.Line(i), in.Sample(i), in.Band(i), useDem);
          }
          else if (algoName == "HAPKELROC") {
            ph = phoLroc->compute(in.Line(i), in.Sample(i), in.Band(i), useDem);
          }
          
          if (!photometryOnly) {
            out[i] = (IsSpecial(ph) ? Null : in[i] * ph);
          }
          else {
            out[i] = (IsSpecial(ph) ? Null : ph);
          }
        }
      }

      return;
    };

    /**
    * @brief Apply photometric correction with backplane
    *
    * Short function dispatched for each brick to apply the photometrc
    * correction function with a backplane.
    *
    * @author 2021-08-02 Cordell Michaud
    * 
    * @internal
    *   @history 2021-07-19 Cordell Michaud - Code adapted from lrowacphomap by Kris Becker
    *
    * @param in Buffer containing input data
    * @param out Buffer of photometrically corrected data
    */
    auto phoCalWithBackplane = [&phoLro, &phoLroc, algoName, photometryOnly](std::vector<Isis::Buffer *> &in, std::vector<Isis::Buffer *> &out) -> void {

      Buffer &image = *in[0];
      Buffer &phase = *in[1];
      Buffer &emission = *in[2];
      Buffer &incidence = *in[3];
      Buffer &lat = *in[4];
      Buffer &lon = *in[5];
      Buffer &calibrated = *out[0];

      for (int i = 0; i < image.size(); i++) {
        //  Don't correct special pixels
        if (IsSpecial(image[i])) {
          calibrated[i] = image[i];
        }
        else if (IsSpecial(phase[i]) || IsSpecial(emission[i]) || IsSpecial(incidence[i]) || IsSpecial(lat[i]) || IsSpecial(lon[i])) {
          calibrated[i] = Isis::Null;
        }
        else {
          // Get correction and test for validity
          double ph = Null;
          if (algoName == "HAPKELRO") {
            ph = phoLro->photometry(incidence[i], emission[i], phase[i], lat[i], lon[i], image.Band(i));
          }
          else if (algoName == "HAPKELROC") {
            ph = phoLroc->photometry(incidence[i], emission[i], phase[i], lat[i], lon[i], image.Band(i));
          }

          if (!photometryOnly) {
            calibrated[i] = (IsSpecial(ph) ? Null : image[i] * ph);
          }
          else {
            calibrated[i] = (IsSpecial(ph) ? Null : ph);
          }
        }
      }

      return;
    };


    // Start the processing
    if (useBackplane) {
      p.ProcessCubes(phoCalWithBackplane, false);
    }
    else {
      p.ProcessCube(phoCal, false);
    }

    PvlGroup photo("Photometry");
    if (algoName == "HAPKELRO") {
      phoLro->report(photo);
    }
    else if (algoName == "HAPKELROC") {
      phoLroc->report(photo);
    }

    ocube->putGroup(photo);
    log.addGroup(photo);

    delete phoLro;
    phoLro = NULL;
    delete phoLroc;
    phoLroc = NULL;

    return log;
  }
}