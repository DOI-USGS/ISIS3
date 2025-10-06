/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "shadowcamcal.h"

using namespace std;

namespace Isis {
  void shadowcamcal(UserInterface &ui, Pvl *log ) {
  /*
  This application can not be run on any image that has been
  geometrically transformed (i.e. scaled, rotated, sheared, or
  reflected) or cropped and must be raw 8 bit EDR.
  */
    QString from = ui.GetAsString("FROM");
    CubeAttributeInput inAtt(from);
    from = ui.GetCubeName("FROM");
    QString to = ui.GetCubeName("TO");
    QFileInfo bName(to);
    QString qBaseName = bName.completeBaseName();
    cout << qBaseName << endl;
  
    // grab the Instrument pvl group from cube's labels
    Pvl *inLbl = new Pvl(from);
    PvlGroup instrument = inLbl->findObject("IsisCube").findGroup("Instrument");
    PvlGroup dims = inLbl->findObject("IsisCube").findObject("Core").findGroup("Dimensions");

    if(!instrument.hasKeyword("InstrumentId") && (!instrument.hasKeyword("InstrumentID"))){
      QString msg = "Keyword InstrumentID or InstrumentId was not found in labels.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    if (QString::compare(instrument["InstrumentId"], "ShadowCam", Qt::CaseInsensitive) != 0){
      QString msg = "Error: InstrumentId not equal to ShadowCam.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    
    /**
    * 
    * @brief Lambda to loads output cube to allow processInPlace
    *
    * This lambda function loads output cube to allow processInPlace
    *
    * @param in Reference to the input line buffer.
    * @param out Reference to the output line buffer.    * 
    **/
    auto LoadOutputCube = [&](Isis::Buffer &in, Isis::Buffer &out)->void { 
      for(int i = 0; i < in.size(); i++){
        out[i] = in[i];
      }
    };

    delete inLbl;
   
    static int lines = 0, bands = 0;
    lines = dims["Lines"];
    bands = dims["Bands"];
    
    bool remvBias = ui.GetBoolean("BiasPixelRemoval");
    bool subtrktBias = ui.GetBoolean("BiasAvgSubtraction");
    bool gainCrrkt = ui.GetBoolean("GainCorrection");
    bool darkSubtrkt = ui.GetBoolean("DarkSubtraction");
    bool flatCrrkt = ui.GetBoolean("FlatfieldCorrection");
    bool radCrrkt = ui.GetBoolean("RadianceCorrection");
    bool writeOutSteps = ui.GetBoolean("WriteOutSteps");

    Progress progress;
    progress.SetMaximumSteps(lines);
    CubeAttributeInput inputAtt = CubeAttributeInput();
    inputAtt = ui.GetInputAttribute("FROM");
    CubeAttributeOutput outputAtt = CubeAttributeOutput();

    LineManager *lineMgr = NULL;
    Cube *iCube = new Cube();
    QTemporaryDir tempDir;
    QString tempDirektory = NULL;
    QString notTempDirektory = NULL;
    QDir dir;

    if(writeOutSteps){
      notTempDirektory = dir.currentPath();
      tempDirektory = tempDir.path();
    }
    else{
      tempDirektory = tempDir.path();
      notTempDirektory = tempDirektory;
    }

    QString cubeFileIn;
    QString cubeFileOut;

    ProcessByLine loadProcess;
    loadProcess.SetInputCube(ui.GetCubeName("FROM"), inputAtt, 0);
    cubeFileOut = tempDirektory + "/temp.load.shc_cal.cub";
    loadProcess.SetOutputCube(cubeFileOut, outputAtt, SHC_AFE_WIDTH * SHC_CHANNELS, lines, bands);
    loadProcess.StartProcess(LoadOutputCube);
    loadProcess.EndProcess();
    loadProcess.Finalize();
    cubeFileIn = cubeFileOut;
    
    // bias average subtraction
    if(subtrktBias){
      bool use_median = false;
      
      if(ui.GetAsString("BIASAVGTYPE") == "MEDIAN"){
        use_median = true;  
      }
      
      cubeFileOut = tempDirektory + "/temp.rmvBiasPxl.shc_cal.cub";
      BiasPixelSubtraction(use_median, cubeFileIn, cubeFileOut, lines);
      iCube->clearIoCache();
      cubeFileIn = cubeFileOut;

      if(writeOutSteps){
        QString cubeStepOut = notTempDirektory + "/" + qBaseName + "-shc_cal-bias_subtract.cub";
        WriteCube(cubeFileIn, &cubeStepOut, ui, remvBias, lines);
      }
    }

    // gain correction
    if(gainCrrkt){
      QString gain_factors_csv = ui.GetAsString("GAINFACTORS");
      cubeFileOut = tempDirektory + "/temp.gainCrrkt.shc_cal.cub";
      GainCorrection(cubeFileIn, cubeFileOut, gain_factors_csv, instrument, lines);
      iCube->clearIoCache();
      cubeFileIn = cubeFileOut;

      if(writeOutSteps){
        QString cubeStepOut = notTempDirektory + "/" + qBaseName + "-shc_cal-gain_correct.cub";
        WriteCube(cubeFileIn, &cubeStepOut, ui, remvBias, lines);
      }
    }
    
    // dark subtraction
    if(darkSubtrkt){
      QString slope_coeffs_csv = ui.GetAsString("SLOPECOEFF");
      QString intrcpt_coeffs_csv = ui.GetAsString("INTRCPTCOEFF");
      cubeFileOut = tempDirektory + "/temp.darkSbtrkt.shc_cal.cub";
      
      DarkSubtraction(slope_coeffs_csv, intrcpt_coeffs_csv, instrument, cubeFileIn, cubeFileOut, lines);
      cubeFileIn = cubeFileOut;

      if(writeOutSteps){
        QString cubeStepOut = notTempDirektory + "/" + qBaseName + "-shc_cal-dark_subtract.cub";
        WriteCube(cubeFileIn, &cubeStepOut, ui, remvBias, lines);
      }
    }
    
    // flatfile
    if(flatCrrkt){
      QString flatfield_coeffs_csv = ui.GetAsString("FLATCOEFF");
      cubeFileOut = tempDirektory + "/temp.flatCrrkt.shc_cal.cub";
      FlatFieldCorrection(flatfield_coeffs_csv, instrument, cubeFileIn, cubeFileOut, lines);
      cubeFileIn = cubeFileOut;

      if(writeOutSteps){
        QString cubeStepOut = notTempDirektory + "/" + qBaseName + "-shc_cal-flat_field.cub";
        WriteCube(cubeFileIn, &cubeStepOut, ui, remvBias, lines);
      }
    }
    
    // radiance
    if(radCrrkt){
      QString radiance_coeffs_csv = ui.GetAsString("RADCOEFF");
      cubeFileOut = tempDirektory + "/temp.radiance.shc_cal.cub";
      RadianceCoefficients(radiance_coeffs_csv, instrument, cubeFileIn, cubeFileOut, lines);
      cubeFileIn = cubeFileOut;

      if(writeOutSteps){
        QString cubeStepOut = notTempDirektory + "/" + qBaseName + "-shc_cal-radiance_correct.cub";
        WriteCube(cubeFileIn, &cubeStepOut, ui, remvBias, lines);
      }
    }
    // final output cube name will get pulled automatically in WriteCube
    QString* noTempCube = nullptr;  
    WriteCube(cubeFileIn, noTempCube, ui, remvBias, lines);

    if(lineMgr) {
      puts("deleting lineMgr");
      delete lineMgr;
    }
    puts("\nFinished calibrating this image.\n");
  }
}  