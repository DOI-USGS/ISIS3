#include <memory>

#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QTemporaryDir>

#include "Buffer.h"
#include "Cube.h"
#include "CubeAttribute.h"
#include "IException.h"
#include "ProcessByLine.h"
#include "Progress.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "UserInterface.h"

#include "BiasPixelSubtraction.h"
#include "DarkSubtraction.h"
#include "GainCorrection.h"
#include "FlatFieldCorrection.h"
#include "RadianceCoefficients.h"
#include "ShadowCamConstants.h"
#include "WriteCube.h"

#include "shadowcamcal.h"

namespace Isis {
  void shadowcamcal(UserInterface &ui) {
    QString inCubeName = ui.GetCubeName("FROM");
    std::unique_ptr<Cube> inCube = std::make_unique<Cube>(inCubeName);
    shadowcamcal(inCube.get(), ui);
  }

  void shadowcamcal(Cube *inCube, UserInterface &ui) {
    /*
    This application can not be run on any image that has been
    geometrically transformed (i.e. scaled, rotated, sheared, or
    reflected) or cropped and must be raw 8 bit EDR.
    */
    const QString qBaseName = QFileInfo(ui.GetCubeName("TO")).completeBaseName();
  
    // grab the Instrument pvl group from cube's labels
    const Pvl *inLabel = inCube->label();
    const PvlGroup &instrumentGroup = inLabel->findObject("IsisCube").findGroup("Instrument");
    const PvlGroup &dimsGroup = inLabel->findObject("IsisCube").findObject("Core").findGroup("Dimensions");

    if (!instrumentGroup.hasKeyword("InstrumentId") && (!instrumentGroup.hasKeyword("InstrumentID"))) {
      QString msg = "Keyword InstrumentID or InstrumentId was not found in labels.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if (QString::compare(instrumentGroup["InstrumentId"], "ShadowCam", Qt::CaseInsensitive) != 0) {
      QString msg = "Error: InstrumentId not equal to ShadowCam.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    
    /**
    * @brief Loads output cube to allow processInPlace
    *
    * @param in Reference to the input line buffer.
    * @param out Reference to the output line buffer.
    **/
    auto LoadOutputCube = [](Isis::Buffer &in, Isis::Buffer &out) -> void {
      for(int i = 0; i < in.size(); i++){
        out[i] = in[i];
      }
    };

    const int lines = static_cast<int>(dimsGroup["Lines"]);
    const int bands = static_cast<int>(dimsGroup["Bands"]);
    
    const bool removeBias = ui.GetBoolean("BiasPixelRemoval");
    const bool subtractBias = ui.GetBoolean("BiasAvgSubtraction");
    const bool correctGain = ui.GetBoolean("GainCorrection");
    const bool subtractDark = ui.GetBoolean("DarkSubtraction");
    const bool correctFlatfield = ui.GetBoolean("FlatfieldCorrection");
    const bool correctRadiance = ui.GetBoolean("RadianceCorrection");
    const bool writeOutSteps = ui.GetBoolean("WriteOutSteps");

    Progress progress;
    progress.SetMaximumSteps(lines);
    CubeAttributeOutput outputAtt = CubeAttributeOutput();

    const QTemporaryDir tempDir;
    const QString stepOutputDir = writeOutSteps ? QDir::currentPath() : tempDir.path();

    QString cubeFileOut = tempDir.path() + "/temp.load.shc_cal.cub";

    ProcessByLine loadProcess;
    loadProcess.SetInputCube(inCube, 0);
    loadProcess.SetOutputCube(cubeFileOut, outputAtt, SHC_AFE_WIDTH * SHC_CHANNELS, lines, bands);
    loadProcess.StartProcess(LoadOutputCube);
    loadProcess.EndProcess();
    loadProcess.Finalize();

    QString cubeFileIn = cubeFileOut;
    
    // bias average subtraction
    if (subtractBias) {
      const bool useMedian = ui.GetAsString("BIASAVGTYPE") == "MEDIAN";
      
      cubeFileOut = tempDir.path() + "/temp.remove_bias_pixels.shc_cal.cub";
      BiasPixelSubtraction(useMedian, cubeFileIn, cubeFileOut, lines);
      cubeFileIn = cubeFileOut;

      if (writeOutSteps){
        QString cubeStepOut = stepOutputDir + "/" + qBaseName + "-shc_cal-bias_subtract.cub";
        WriteCube(cubeFileIn, &cubeStepOut, ui, removeBias, lines);
      }
    }

    // gain correction
    if(correctGain){
      QString gain_factors_csv = ui.GetAsString("GAINFACTORS");
      cubeFileOut = tempDir.path() + "/temp.gaincorrection.shc_cal.cub";
      GainCorrection(cubeFileIn, cubeFileOut, gain_factors_csv, instrumentGroup, lines);
      cubeFileIn = cubeFileOut;

      if (writeOutSteps) {
        QString cubeStepOut = stepOutputDir + "/" + qBaseName + "-shc_cal-gain_correct.cub";
        WriteCube(cubeFileIn, &cubeStepOut, ui, removeBias, lines);
      }
    }
    
    // dark subtraction
    if (subtractDark) {
      const QString slope_coeffs_csv = ui.GetAsString("SLOPECOEFF");
      const QString intrcpt_coeffs_csv = ui.GetAsString("INTRCPTCOEFF");
      cubeFileOut = tempDir.path() + "/temp.subtractDark.shc_cal.cub";
      
      DarkSubtraction(slope_coeffs_csv, intrcpt_coeffs_csv, instrumentGroup, cubeFileIn, cubeFileOut, lines);
      cubeFileIn = cubeFileOut;

      if (writeOutSteps) {
        QString cubeStepOut = stepOutputDir + "/" + qBaseName + "-shc_cal-dark_subtract.cub";
        WriteCube(cubeFileIn, &cubeStepOut, ui, removeBias, lines);
      }
    }
    
    // flatfile
    if (correctFlatfield) {
      const QString flatfield_coeffs_csv = ui.GetAsString("FLATCOEFF");
      cubeFileOut = tempDir.path() + "/temp.correctFlatfield.shc_cal.cub";
      FlatFieldCorrection(flatfield_coeffs_csv, instrumentGroup, cubeFileIn, cubeFileOut, lines);
      cubeFileIn = cubeFileOut;

      if (writeOutSteps) {
        QString cubeStepOut = stepOutputDir + "/" + qBaseName + "-shc_cal-flat_field.cub";
        WriteCube(cubeFileIn, &cubeStepOut, ui, removeBias, lines);
      }
    }
    
    // radiance
    if (correctRadiance) {
      const QString radiance_coeffs_csv = ui.GetAsString("RADCOEFF");
      cubeFileOut = tempDir.path() + "/temp.radiance.shc_cal.cub";
      RadianceCoefficients(radiance_coeffs_csv, instrumentGroup, cubeFileIn, cubeFileOut, lines);
      cubeFileIn = cubeFileOut;

      if (writeOutSteps) {
        QString cubeStepOut = stepOutputDir + "/" + qBaseName + "-shc_cal-radiance_correct.cub";
        WriteCube(cubeFileIn, &cubeStepOut, ui, removeBias, lines);
      }
    }
    // final output cube name will get pulled automatically in WriteCube
    QString* noTempCube = nullptr;  
    WriteCube(cubeFileIn, noTempCube, ui, removeBias, lines);
  }
}