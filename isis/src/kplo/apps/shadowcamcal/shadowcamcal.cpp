#include <array>
#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <vector>

#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QTemporaryDir>

#include "Buffer.h"
#include "Cube.h"
#include "CubeAttribute.h"
#include "IException.h"
#include "LineManager.h"
#include "ProcessByLine.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "UserInterface.h"

#include "BiasPixelSubtraction.h"
#include "DarkSubtraction.h"
#include "GainCorrection.h"
#include "FlatFieldCorrection.h"
#include "RadianceCoefficients.h"
#include "ShadowCamConstants.h"
#include "ShadowCamUtilities.h"

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

    CubeAttributeOutput outputAtt = CubeAttributeOutput();

    const QTemporaryDir tempDir;
    const QString stepOutputDir = writeOutSteps ? QDir::currentPath() : tempDir.path();

    QString cubeFileOut = tempDir.path() + "/temp.load.shc_cal.cub";

    ProcessByLine loadProcess;
    loadProcess.SetInputCube(inCube, 0);
    loadProcess.SetOutputCube(cubeFileOut, outputAtt, ShadowCam::SHC_AFE_WIDTH * ShadowCam::SHC_CHANNELS, lines, bands);
    loadProcess.StartProcess(LoadOutputCube);
    loadProcess.EndProcess();
    loadProcess.Finalize();

    QString cubeFileIn = cubeFileOut;
    
    // bias average subtraction
    if (subtractBias) {
      const bool useMedian = ui.GetAsString("BIASAVGTYPE") == "MEDIAN";
      
      cubeFileOut = tempDir.path() + "/temp.remove_bias_pixels.shc_cal.cub";
      ShadowCam::SubtractBiasPixels(useMedian, cubeFileIn, cubeFileOut, lines);
      cubeFileIn = cubeFileOut;

      if (writeOutSteps){
        QString cubeStepOut = stepOutputDir + "/" + qBaseName + "-shc_cal-bias_subtract.cub";
        ShadowCam::WriteCube(cubeFileIn, cubeStepOut, ui, removeBias, lines);
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
        ShadowCam::WriteCube(cubeFileIn, cubeStepOut, ui, removeBias, lines);
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
        ShadowCam::WriteCube(cubeFileIn, cubeStepOut, ui, removeBias, lines);
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
        ShadowCam::WriteCube(cubeFileIn, cubeStepOut, ui, removeBias, lines);
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
        ShadowCam::WriteCube(cubeFileIn, cubeStepOut, ui, removeBias, lines);
      }
    }
    // final output cube name will get pulled automatically in ShadowCam::WriteCube
    QString noTempCube = "";
    ShadowCam::WriteCube(cubeFileIn, noTempCube, ui, removeBias, lines);
  }

  namespace ShadowCam {
    void WriteCube(const QString &cubeFileIn, const QString &cubeFileOut, UserInterface &ui, bool removeBias, int lines) {
      try {
        // Open input cube
        auto iCube = std::make_unique<Cube>(cubeFileIn);
        auto lineManager = std::make_unique<LineManager>(*iCube);

        int samples = SHC_AFE_WIDTH * SHC_CHANNELS;
        int channelWidth = SHC_AFE_WIDTH;

        if (removeBias) {
          samples = SHC_SCENE * SHC_CHANNELS;
          channelWidth = SHC_SCENE;
        }

        /**
          * @brief Lambda to remove bias pixels
          *
          * This lambda function removes bias pixels.
          *
          * @param out Reference to the output line buffer.
          **/
        auto WriteOutCube = [iCube=iCube.get(), lineManager=lineManager.get(), removeBias, samples, channelWidth](
            Isis::Buffer &out) -> void {
          lineManager->SetLine(out.Line(), 1);
          iCube->read(*lineManager);
          int outdex = 0;
          int index = 0;
          int column = 0;

          if (removeBias) {
            for (int channel = 0; channel < SHC_CHANNELS; channel++) {
              for (int pixel = 0; pixel < channelWidth; pixel++) {
                outdex = GetDataIndex(channel, SHC_SCENE, pixel);
                column = pixel + SHC_SCENE_OFFSET;
                index = GetDataIndex(channel, SHC_AFE_WIDTH, column);
                out[outdex] = (*lineManager)[index];
              }
            }
          }
          else {
            for (int index = 0; index < samples; index++) {
              out[index] = (*lineManager)[index];
            }
          }
        };

        // Setup the ProcessByLine object
        ProcessByLine wp;
        wp.PropagateHistory(false);
        CubeAttributeInput inputAtt = CubeAttributeInput();
        CubeAttributeOutput outputAtt = CubeAttributeOutput();
        wp.SetInputCube(cubeFileIn, inputAtt, 0);

        // Check if output file is provided
        if (cubeFileOut.isEmpty()) {
          if (removeBias) {
            std::cout << "Removing bias pixel columns from output cube" << std::endl;
          }
          wp.SetOutputCube(ui.GetCubeName("TO"), outputAtt, samples, lines, SHC_BANDS);
        }
        else {
          wp.SetOutputCube(cubeFileOut, outputAtt, samples, lines, SHC_BANDS);
        }

        wp.PropagateTables(false);
        wp.ClearInputCubes();
        wp.StartProcess(WriteOutCube);

        wp.EndProcess();
        wp.Finalize();
      }
      catch (const IException &e) {
        throw IException(e, IException::Programmer, "ISIS Exception: " + Isis::toString(e.what()) + ". Unable to write cube.", _FILEINFO_);
      }
      catch (const std::exception &e) {
        std::cerr << "Standard exception: " << e.what() << ". Unable to write cube." << std::endl;
        exit(1);
      }
      catch (...) {
        throw IException(IException::Programmer, "Unknown exception occurred. Unable to write cube.", _FILEINFO_);
      }
    }

    void SubtractBiasPixels(bool useMedian, const QString &tempCubeFileIn, const QString &tempCubeFileOut, int lines) {
      std::cout << "Subtracting per-channel bias pixel average." << std::endl;
      if (useMedian) {
        std::cout << "Performing bias median average pixel subtraction" << std::endl;
      }
      else {
        std::cout << "Performing bias mean average pixel subtraction" << std::endl;
      }

      std::array<std::vector<double>, SHC_CHANNELS> biasPile;
      std::vector<double> biasMedian(SHC_CHANNELS, -1.0);
      std::vector<double> biasMean(SHC_CHANNELS);

      try{
        /**
          * @brief Subtracts bias pixel averages
          *
          * Subtracts bias pixel averages, either mean or median, from the
          * each sample, hence entire image.
          *
          * @param in The input buffer.
          * @param out The output buffer.
        **/
        auto SubtractBufferBiasPixels = [useMedian, &biasMedian, &biasMean](Isis::Buffer &in, Isis::Buffer &out) -> void { 
          for (int channel = 0; channel < SHC_CHANNELS; channel++) {
            for (int pixel = 0; pixel < SHC_AFE_WIDTH; pixel++) {
              int index = GetDataIndex(channel, SHC_AFE_WIDTH, pixel);
              if (!IsSpecialPixelSHC(in[index])) {
                if (useMedian) {
                  if (biasMedian.at(channel) < 0) {
                    std::cout << "WARNING: bias median for channel is negative " << std::endl;
                  }
                  out[index] = in[index] - biasMedian.at(channel);
                }
                else {
                  if (biasMean.at(channel) < 0) {
                    std::cout << "WARNING: bias mean for channel is negative." << std::endl;
                  }
                  out[index] = in[index] - biasMean.at(channel);
                }
              } 
              else {
                out[index] = in[index];
              }
            }
          }
        };

        // Open input cube
        auto iCube = std::make_unique<Cube>(tempCubeFileIn);
        auto lineManager = std::make_unique<LineManager>(*iCube);

        // Collect non-special bias pixels in a pile
        for (int line = 1; line <= lines; line++) {
          lineManager->SetLine(line, SHC_BANDS);
          iCube->read(*lineManager);
          for (int channel = 0; channel < SHC_CHANNELS; channel++) {
            for (int pixel = 2; pixel < 10; ++pixel) {
              int index = (channel * SHC_AFE_WIDTH) + pixel;
              if (!IsSpecialPixelSHC((*lineManager)[index]))
                biasPile.at(channel).push_back((*lineManager)[index]);
            }
          }
        }

        // Calculate bias median and mean
        for (int channel = 0; channel < SHC_CHANNELS; ++channel) {
          std::sort(biasPile.at(channel).begin(), biasPile.at(channel).end());

          // Calculate median
          int biasCountPerChannel = biasPile.at(channel).size();
          if (biasCountPerChannel % 2 == 0) {
            biasMedian.at(channel) = static_cast<double>(biasPile.at(channel).at((biasCountPerChannel / 2 - 1)) + biasPile.at(channel).at((biasCountPerChannel / 2))) / 2.0;
          }
          else {
            biasMedian.at(channel) = biasPile.at(channel).at(biasCountPerChannel / 2);
          }

          // Calculate mean
          double sum = std::accumulate(biasPile.at(channel).begin(), biasPile.at(channel).end(), 0.0);
          if (biasCountPerChannel == 0) {
            throw IException(IException::Programmer, "ERROR (divideByZero): bias count per channel is zero (sum/biasCountPerChannel)" , _FILEINFO_);
          }
          biasMean.at(channel) = sum / biasCountPerChannel;
        }

        // Process the cube
        ProcessByLine p;
        CubeAttributeInput inputAtt = CubeAttributeInput();
        CubeAttributeOutput outputAtt = CubeAttributeOutput();
        p.SetInputCube(tempCubeFileIn, inputAtt, 0);
        p.SetOutputCube(tempCubeFileOut, outputAtt, SHC_AFE_WIDTH * SHC_CHANNELS, lines, SHC_BANDS);
        p.StartProcess(SubtractBufferBiasPixels);
        p.EndProcess();
        p.Finalize();
      }
      catch (const IException &e) {
        throw IException(e, IException::Programmer, "ISIS Exception: " + Isis::toString(e.what()) + ". Unable to apply bias pixel subtraction to image.", _FILEINFO_);
      }
      catch (const std::exception &e) {
        std::cerr << "Standard exception: " << e.what() << ". Unable to apply bias pixel subtraction to image." << std::endl;
        exit(1);
      }
      catch (...) {
        throw IException(IException::Programmer, "Unknown exception occured. Unable to apply bias pixel subtraction to image.", _FILEINFO_);
      }
    }
  }
}