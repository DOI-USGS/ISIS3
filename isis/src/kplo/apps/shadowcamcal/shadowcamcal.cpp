#include <array>
#include <algorithm>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
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

#include "ShadowCamConstants.h"
#include "ShadowCamUtilities.h"

#include "shadowcamcal.h"

namespace Isis {
  void shadowcamcal(UserInterface &ui) {
    FileName inCubeName = FileName(ui.GetCubeName("FROM"));
    if (ShadowCam::IsCalibrated(inCubeName)) {
      const QString msg = "File [" + inCubeName.name() + "] is calibrated, no need to run shadowcamcal";
      throw IException(IException::User, msg, _FILEINFO_);
    }

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
     * @param in The input line buffer.
     * @param out The output line buffer.
     */
    std::function<void(Buffer &in, Buffer &out)> LoadOutputCube = [](Isis::Buffer &in, Isis::Buffer &out) -> void {
      for (int i = 0; i < in.size(); i++) {
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

      if (writeOutSteps) {
        QString cubeStepOut = stepOutputDir + "/" + qBaseName + "-shc_cal-bias_subtract.cub";
        ShadowCam::WriteCube(cubeFileIn, cubeStepOut, ui, removeBias, lines);
      }
    }

    // gain correction
    if (correctGain) {
      const QString gainFactorsCsv = ui.GetAsString("GAINFACTORS");
      cubeFileOut = tempDir.path() + "/temp.gaincorrection.shc_cal.cub";
      ShadowCam::CorrectGain(cubeFileIn, cubeFileOut, gainFactorsCsv, instrumentGroup, lines);
      cubeFileIn = cubeFileOut;

      if (writeOutSteps) {
        QString cubeStepOut = stepOutputDir + "/" + qBaseName + "-shc_cal-gain_correct.cub";
        ShadowCam::WriteCube(cubeFileIn, cubeStepOut, ui, removeBias, lines);
      }
    }
    
    // dark subtraction
    if (subtractDark) {
      const QString slopeCoeffsCsv = ui.GetAsString("SLOPECOEFF");
      const QString interceptCoeffsCsv = ui.GetAsString("INTERCEPTCOEFF");
      cubeFileOut = tempDir.path() + "/temp.subtractDark.shc_cal.cub";
      
      ShadowCam::SubtractDark(slopeCoeffsCsv, interceptCoeffsCsv, instrumentGroup, cubeFileIn, cubeFileOut, lines);
      cubeFileIn = cubeFileOut;

      if (writeOutSteps) {
        QString cubeStepOut = stepOutputDir + "/" + qBaseName + "-shc_cal-dark_subtract.cub";
        ShadowCam::WriteCube(cubeFileIn, cubeStepOut, ui, removeBias, lines);
      }
    }
    
    // flatfile
    if (correctFlatfield) {
      const QString flatfieldCoeffsCsv = ui.GetAsString("FLATCOEFF");
      cubeFileOut = tempDir.path() + "/temp.correctFlatfield.shc_cal.cub";
      ShadowCam::CorrectFlatfield(flatfieldCoeffsCsv, instrumentGroup, cubeFileIn, cubeFileOut, lines);
      cubeFileIn = cubeFileOut;

      if (writeOutSteps) {
        QString cubeStepOut = stepOutputDir + "/" + qBaseName + "-shc_cal-flat_field.cub";
        ShadowCam::WriteCube(cubeFileIn, cubeStepOut, ui, removeBias, lines);
      }
    }
    
    // radiance
    if (correctRadiance) {
      const QString radianceCoeffsCsv = ui.GetAsString("RADCOEFF");
      cubeFileOut = tempDir.path() + "/temp.radiance.shc_cal.cub";
      ShadowCam::CorrectRadiance(radianceCoeffsCsv, instrumentGroup, cubeFileIn, cubeFileOut, lines);
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
         * @brief Removes bias pixels
         *
         * @param out The output line buffer.
         */
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
        QString msg = QString("ISIS Exception: %1. Unable to write cube.").arg(QString(e.what()));
        throw IException(e, IException::Programmer, msg, _FILEINFO_);
      }
      catch (const std::exception &e) {
        QString msg = QString("Standard exception: %1. Unable to write cube.").arg(QString(e.what()));
        throw IException(IException::Programmer, msg, _FILEINFO_);
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
         * Subtracts bias pixel averages, either mean or median, from
         * each sample, hence entire image.
         *
         * @param in The input buffer.
         * @param out The output buffer.
         */
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
        QString msg = QString("ISIS Exception: %1. Unable to apply bias pixel subtraction to image.").arg(QString(e.what()));
        throw IException(e, IException::Programmer, msg, _FILEINFO_);
      }
      catch (const std::exception &e) {
        QString msg = QString("Standard exception: %1. Unable to apply bias pixel subtraction to image.").arg(QString(e.what()));
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
      catch (...) {
        throw IException(IException::Programmer, "Unknown exception occurred. Unable to apply bias pixel subtraction to image.", _FILEINFO_);
      }
    }

    void SubtractDark(const QString &slopeFilename, const QString &interceptFilename,
        const PvlGroup &instrumentGroup, const QString &cubeFileIn, const QString &cubeFileOut, int lines) {
      try {
        const double fpaATemp = (GetFromLabels(instrumentGroup, "TemperatureFPAA")).toDouble();
        const double lineRateMs = (GetFromLabels(instrumentGroup, "LineRate")).toDouble();

        if (!instrumentGroup.hasKeyword("TDIDirection")) {
          throw IException(IException::User, "Error: TDIDirection not found.", _FILEINFO_);
        }

        const int tdiFactor = GetTdiFactor(instrumentGroup);

        constexpr int slopeInterceptCount = SHC_SCENE * SHC_AFE_WIDTH;

        std::vector<double> slopeA(slopeInterceptCount);
        std::vector<double> slopeB(slopeInterceptCount);
        std::vector<double> slopeRmse(slopeInterceptCount);
        std::vector<double> interceptA(slopeInterceptCount);
        std::vector<double> interceptB(slopeInterceptCount);
        std::vector<double> interceptRmse(slopeInterceptCount);

        /**
         * @brief Reads slope and intercept coefficients from a CSV into coefficient vectors
         *
         * @param filename Input CSV filename
         * @param coeffA
         * @param coeffB
         * @param coeffRmse
         */
        auto ReadCoeffCsv = [tdiFactor](const QString &filename, std::vector<double> &coeffA,
            std::vector<double> &coeffB, std::vector<double> &coeffRmse) -> void {
          const std::string csvFilename = GetVersionedFilename(filename);

          std::ifstream csvFileBuffer(csvFilename.c_str());

          if (!csvFileBuffer.is_open()) {
            throw IException(IException::User, "Failed to open CSV file: " + filename, _FILEINFO_);
          }

          for (int channel = 0; channel < SHC_CHANNELS; channel++) {
            for (int column = 0; column < SHC_SCENE; column++) {
              std::string line;

              while (std::getline(csvFileBuffer, line)) {
                if ((line.rfind("#", 0) != 0) && (ToLower(line).rfind("tdi", 0) != 0)) {
                  break;
                }
              }

              std::vector<double> coeffTemp(SHC_CHANNELS);

              std::stringstream ss(line);
              std::string token = "";
              std::getline(ss, token, ',');
              if (token == "") {
                throw IException(IException::User, "Expected ',' in file. Loading CSV failed.", _FILEINFO_);
              }

              for (int i = 0; i < SHC_CHANNELS; i++) {
                try {
                  coeffTemp.at(i) = std::stod(token);
                }
                catch (const std::exception &e) {
                  throw IException(IException::User, ("Error reading CSV file: " + std::string(e.what())).c_str(), _FILEINFO_);
                }
                if (i != 5) {
                  token.clear();
                  std::getline(ss, token, ',');
                  if (token == "") {
                    throw IException(IException::User, "Expected ',' in file. Loading CSV failed", _FILEINFO_);
                  }
                }
              }

              std::uint16_t index = channel * SHC_SCENE + column;

              if (coeffA.size() < index){
                coeffA.resize(index);
                coeffB.resize(index);
                coeffRmse.resize(index);
              }

              coeffA.at(index) = coeffTemp.at(0 + 3 * tdiFactor);
              coeffB.at(index) = coeffTemp.at(1 + 3 * tdiFactor);
              coeffRmse.at(index) = coeffTemp.at(2 + 3 * tdiFactor);
            }
          }
        };

        /**
         * @brief Subtracts dark current from each pixel
         *
         * Subtracts the dark current named darkLevel, which is calculated using the two csv files provided by the user
         * or by default. This value is then subracted by each scene pixel value and assigned to that pixel. 
         *
         * @param in The input buffer.
         * @param out The output buffer.
         */
        auto SubtractBufferDark = [&slopeA, &slopeB, fpaATemp, lineRateMs, &interceptA, &interceptB](
            Isis::Buffer &in, Isis::Buffer &out) -> void {
          // fill buffer
          for (int i = 0; i < in.size(); i++) {
            out[i] = in[i];
          }

          for (int channel = 0; channel < SHC_CHANNELS; channel++) {
            for (int column = 0; column < SHC_SCENE; column++) {
              std::uint16_t sample = column + SHC_SCENE_OFFSET;
              std::uint16_t ci = channel * SHC_SCENE + column;
              int index = GetDataIndex(channel, SHC_AFE_WIDTH, sample);

              if (!IsSpecialPixelSHC(in[index])) {
                // Calculate dark level based on temperature, line rate, and coefficients
                double darkLevel = slopeA.at(ci) * exp(slopeB.at(ci) * fpaATemp) * lineRateMs + interceptA.at(ci) * exp(interceptB.at(ci) * fpaATemp);
                out[index] = in[index] - darkLevel;
              }
            }
          }
        };

        ReadCoeffCsv(slopeFilename, slopeA, slopeB, slopeRmse);
        ReadCoeffCsv(interceptFilename, interceptA, interceptB, interceptRmse);

        ProcessByLine p;
        CubeAttributeInput inputAtt = CubeAttributeInput();
        CubeAttributeOutput outputAtt = CubeAttributeOutput();
        p.SetInputCube(cubeFileIn, inputAtt, 0);
        p.SetOutputCube(cubeFileOut, outputAtt, SHC_AFE_WIDTH * SHC_CHANNELS, lines, SHC_BANDS);
        p.StartProcess(SubtractBufferDark);
        p.EndProcess();
        p.Finalize();
      }
      catch (const IException &e) {
        QString msg = QString("ISIS Exception: %1. Unable to apply dark correction to image.").arg(QString(e.what()));
        throw IException(e, IException::Programmer, msg, _FILEINFO_);
      }
      catch (const std::exception &e) {
        QString msg = QString("Standard exception: %1. Unable to apply dark correction to image.").arg(QString(e.what()));
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
      catch (...) {
        throw IException(IException::Programmer, "Unknown exception occurred. Unable to apply dark correction to image.", _FILEINFO_);
      }
    }

    void CorrectFlatfield(const QString &flatfieldCoeffFilename, const PvlGroup &instrumentGroup, const QString &cubeFileIn,
        const QString &cubeFileOut, int lines) {
      try {
        if (!instrumentGroup.hasKeyword("TDIDirection")) {
          throw IException(IException::User, "Error: TDIDirection not found.", _FILEINFO_);
        }

        const int tdiFactor = GetTdiFactor(instrumentGroup);
        
        std::vector<double> flatCoeff(SHC_CHANNELS * SHC_SCENE); 

        /**
         * @brief Reads coefficients from a CSV file and inserts values into the respective coefficient vectors.
         *
         * @param filename Input CSV filename
         * @param flatCoeff Flatfield coefficient vector
         */
        auto ReadCoeffCsv = [tdiFactor](QString filename, std::vector<double> &flatCoeff)->void {
          std::string csvFilename = GetVersionedFilename(filename);
        
          std::ifstream csvBuffer(csvFilename.c_str());

          if (!csvBuffer.is_open()) {
            throw IException(IException::User, "Failed to open CSV file: " + filename, _FILEINFO_);
          }

          // Read scene pixels per line from the CSV file
          for (int column = 0; column < SHC_SCENE; column++) {
            // find a non-comment/header line
            std::string line;
            while (std::getline(csvBuffer, line)) {
              if (line.rfind("#", 0) != 0) {
                break;
              }
            }

            std::vector<double> coeffTmp(SHC_CHANNELS * 2);
            std::stringstream ss(line);
            std::string token;

            std::getline(ss, token, ',');  // discard first token
            if (token.empty()) {
              throw IException(IException::User, "Expected ',' in " + filename + ". loading CSV failed.\n", _FILEINFO_);
            }

            for (int i = 0; i < SHC_CHANNELS * 2; i++) {
              std::getline(ss, token, ',');
              if (token.empty()) {
                throw IException(IException::User, "Expected ',' in " + filename + ". loading CSV failed.\n", _FILEINFO_);
              }

              coeffTmp.at(i) = std::stod(token);  
            }

            // Assign values to flatCoeff of length 3072
            for (int channel = 0; channel < SHC_CHANNELS; channel++) {
              flatCoeff.at(channel * SHC_SCENE + column) = coeffTmp.at(channel + SHC_CHANNELS * tdiFactor);
            }
          }
        };

        /**
         * @brief Applies the flatfield correction to each pixel in the buffer.
         *
         * Applies the flatfield correction to each pixel by dividing each pixel by the flatfield coefficient, which is
         * calculated using the csv file provided by the user or by default.
         *
         * @param in The input buffer.
         * @param out The output buffer.
         */
        auto CorrectBufferFlatfield = [&flatCoeff](Isis::Buffer &in, Isis::Buffer &out) -> void {
          // fill output buffer since flatfield correction is only applied to scene pixels
          for (int i = 0; i < in.size(); i++) {
            out[i] = in[i];
          }

          // flatfield is only applied to scene pixels
          for (int channel = 0; channel < SHC_CHANNELS; channel++) {
            for (int column = 0; column < SHC_SCENE; column++) {

              std::uint16_t sample = column + SHC_SCENE_OFFSET;
              int index = GetDataIndex(channel, SHC_AFE_WIDTH, sample);
              int coeffIndex = channel * SHC_SCENE + column;

              if (!IsSpecialPixelSHC(in[index])) {
                if (flatCoeff.at(coeffIndex) == 0) {
                  QString msg = QString("ERROR (divideByZero): Flatfield coefficient is zero for flatCoeff [%1].").arg(QString::number(coeffIndex));
                  throw IException(IException::Programmer, msg, _FILEINFO_);
                }

                out[index] = in[index] / flatCoeff.at(coeffIndex);
              }
            }
          }
        };

        // Read coefficients from CSV file
        ReadCoeffCsv(flatfieldCoeffFilename, flatCoeff);

        // Set up process to apply flatfield correction
        ProcessByLine p;
        CubeAttributeInput inputAtt = CubeAttributeInput();
        CubeAttributeOutput outputAtt = CubeAttributeOutput();
        p.SetInputCube(cubeFileIn, inputAtt, 0);
        p.SetOutputCube(cubeFileOut, outputAtt, SHC_AFE_WIDTH * SHC_CHANNELS, lines, SHC_BANDS);
        p.StartProcess(CorrectBufferFlatfield);
        p.EndProcess();
        p.Finalize();
      }
      catch (const IException &e) {
        QString msg = QString("ISIS Exception: %1. Unable to apply flatfield correction to image.").arg(QString(e.what()));
        throw IException(e, IException::Programmer, msg, _FILEINFO_);
      }
      catch (const std::exception &e) {
        QString msg = QString("Standard exception: %1. Unable to apply flatfield correction to image.").arg(QString(e.what()));
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
      catch (...) {
        throw IException(IException::Programmer, "Unknown exception occurred. Unable to apply flatfield correction to image.", _FILEINFO_);
      }
    }

    void CorrectGain(const QString &cubeFileIn, const QString &cubeFileOut, const QString &gainCoeffFilename,
        const PvlGroup &instrumentGroup, int lines) {
      // get gain channels from labels
      std::array<double, TERMS> gainChannels;

      try {
        for (int i = 0; i < TERMS; i++) {
          QString gainChannelKey = "GainCh" + toString(i);
          if (instrumentGroup.hasKeyword(gainChannelKey)) {
            gainChannels.at(i) = instrumentGroup[gainChannelKey];
          }
          else {
            throw IException(IException::User, "Gain Channels not found in Instrument label group.", _FILEINFO_);
          }
        }

        if (!instrumentGroup.hasKeyword("TDIDirection")) {
          throw IException(IException::User, "Error: TDIDirection not found.", _FILEINFO_);
        }

        const int tdiFactor = GetTdiFactor(instrumentGroup);

        /**
         * @brief Reads gain coefficients from CSV files
         *
         * Reads the gain coefficient CSV file line by line and adds to gainFactors which will be used when applying
         * the gain correction.
         *
         * @param filename The path to the CSV file.
         * @param gainCoeffs The vector to store the gain coefficients for each channel.
         *
         */
        auto ReadCoeffCsv = [tdiFactor, &gainChannels](
            const QString &filename, std::vector<double> &gainCoeffs) -> void {
          std::string line;
          std::string csvFilename = GetVersionedFilename(filename);

          std::ifstream csvBuffer(csvFilename.c_str());

          if(!csvBuffer.is_open()) {
            throw IException(IException::User, "Unable to open csv file!", _FILEINFO_);
          }

          while (std::getline(csvBuffer, line)) {
            if ((line.rfind("#", 0) == 0) || (line.rfind("gain_code", 0) == 0)) {
              continue;
            }

            std::vector<double> gainTmp(SHC_CHANNELS * 2);

            std::stringstream ss(line);
            std::string token;

            std::getline(ss, token, ',');
            if (token.empty()) {
              throw IException(IException::User, "Gain Code not found in CSV. Loading CSV failed.", _FILEINFO_);
            }

            double gainCode = stod(token);

            for (int i = 0; i < SHC_CHANNELS * 2; i++){
              std::getline(ss, token, ',');
              if (token.empty()){
                throw IException(IException::User, "Expected a ',' in " + line + ". Loading CSV failed.", _FILEINFO_);
              }
              gainTmp.at(i) = std::stod(token);
            }

            for (int channel = 0; channel < SHC_CHANNELS; channel++) {
              if (gainChannels.at(channel) == gainCode) {
                gainCoeffs.at(channel) = gainTmp.at(channel + SHC_CHANNELS * tdiFactor);
              }
            }
          }
        };

        // Read coefficients from CSV file
        std::vector<double> gainFactors(SHC_CHANNELS);
        ReadCoeffCsv(gainCoeffFilename, gainFactors);

        /**
         * @brief Applies the gain correction to each pixel
         *
         * Divides each pixel by the the channel's gain factor which is calculated using the values found in the CSV
         * file provided by the user or by default.
         *
         * @param in The input buffer.
         * @param out The output buffer.
         */
        auto CorrectBufferGain = [&gainFactors](Isis::Buffer &in, Isis::Buffer &out) -> void {
          for (int channel = 0; channel < SHC_CHANNELS; channel++) {
            for (int pixel = 0; pixel < SHC_AFE_WIDTH; pixel++) {
              int index = GetDataIndex(channel, SHC_AFE_WIDTH, pixel);
              double gainFactor = gainFactors.at(channel);

              if (!IsSpecialPixelSHC(in[index])) {
                if (gainFactor == 0) {
                  throw IException(
                    IException::User,
                    QString("WARNING: Gain for channel %1: %2. Can't apply correction (divideByZero).").arg(
                      QString::number(channel), QString::number(gainFactor)),
                    _FILEINFO_);
                }
                out[index] = in[index] / gainFactor;
              }
              else {
                out[index] = in[index];
              }
            }
          }
        };

        ProcessByLine p;
        CubeAttributeInput inputAtt = CubeAttributeInput();
        CubeAttributeOutput outputAtt = CubeAttributeOutput();
        p.SetInputCube(cubeFileIn, inputAtt, 0);
        p.SetOutputCube(cubeFileOut, outputAtt, SHC_AFE_WIDTH * SHC_CHANNELS, lines, SHC_BANDS);

        // Apply correction line by line
        p.StartProcess(CorrectBufferGain);
        p.EndProcess();
        p.Finalize();
      }
      catch (const IException &e) {
        QString msg = QString("ISIS Exception: %1. Unable to apply gain correction to image.").arg(QString(e.what()));
        throw IException(e, IException::Programmer, msg, _FILEINFO_);
      }
      catch (const std::exception &e) {
        QString msg = QString("Standard exception: %1. Unable to apply gain correction to image.").arg(
          QString(e.what()));
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
      catch (...) {
        throw IException(IException::Programmer,
          "Unknown exception occurred. Unable to apply gain correction to image.", _FILEINFO_);
      }
    }

    void CorrectRadiance(const QString &radianceCoeffFilename, const PvlGroup &instrumentGroup,
        const QString &cubeFileIn, const QString &cubeFileOut, int lines) {
      try {
        if (!instrumentGroup.hasKeyword("TDIDirection")) {
          throw IException(IException::User, "Error: TDIDirection not found.", _FILEINFO_);
        }

        const int tdiFactor = GetTdiFactor(instrumentGroup);

        const double lineRateMs = (GetFromLabels(instrumentGroup, "LineRate")).toDouble();

        std::array<double, SHC_CHANNELS> radianceCoeff;

        /**
          * @brief Lambda to read coefficients from the provided CSV file and inserts values into
          * the respective coefficient vectors
          *
          * @param radianceCoeffFilename Reference to incoming filename and directory address
          * @param coeff    Reference to coefficient vector
          */
        auto ReadCoeffCsv = [tdiFactor](const QString &filename, std::array<double, SHC_CHANNELS> &coeff) -> void {
          std::string line;

          std::string csvFilename = GetVersionedFilename(filename);

          std::ifstream csvBuffer(csvFilename.c_str());

          // Check if the file is empty
          if (!csvBuffer.is_open()) {
            throw IException(IException::User, "Failed to open CSV file: " + filename, _FILEINFO_);
          }

          while (std::getline(csvBuffer, line)) {
            if ((line.rfind("#", 0) == 0)) {
              continue;
            }

            std::vector<double> coeff_tmp(SHC_CHANNELS * 2);
            std::stringstream ss(line);
            std::string token;

            std::getline(ss, token, ',');  // discard first token
            if (token.empty()) {
              throw IException(IException::User, "Expected ',' in " + filename + ". loading CSV failed.\n", _FILEINFO_);
            }
            for (int i = 0; i < SHC_CHANNELS * 2; i++) {
              std::getline(ss, token, ',');
              if (token.empty()) {
                throw IException(IException::User,
                  "Expected ',' in " + filename + ". loading CSV failed.\n", _FILEINFO_);
              }
              coeff_tmp.at(i) = std::stod(token);
            }

            for (int channel = 0; channel < SHC_CHANNELS; channel++) {
              coeff.at(channel) = coeff_tmp.at(channel + SHC_CHANNELS * tdiFactor);
            }
          }
        };

        /**
         * @brief Applies radiance correction to each pixel in the buffer.
         *
         * Applies the radiance correction by dividing each pixel by the product of the line rate
         * and the channel's radiance coefficient from the CSV file provided by the user or by
         * default.
         *
         * @param in The input buffer.
         * @param out The output buffer.
         */
        auto ApplyRadianceCoefficients = [lineRateMs, &radianceCoeff](Isis::Buffer &in, Isis::Buffer &out) -> void {
          for (int channel = 0; channel < SHC_CHANNELS; channel++) {
            for (int pixel = 0; pixel < SHC_AFE_WIDTH; pixel++) {
              int index = GetDataIndex(channel, SHC_AFE_WIDTH, pixel);
              if (!IsSpecialPixelSHC(in[index])) {
                double lineRateScaledRadiance = lineRateMs * radianceCoeff.at(channel);
                if (lineRateScaledRadiance == 0) {
                  throw IException(IException::Programmer, "ERROR (divideByZero): linerate * radiance coefficient is zero ", _FILEINFO_);
                }
                else {
                  out[index] = in[index] / lineRateScaledRadiance;
                }
              }
              else {
                out[index] = in[index];
              }
            }
          }
        };

        // Read coefficients from CSV file
        ReadCoeffCsv(radianceCoeffFilename, radianceCoeff);

        // Set up the process to apply radiance correction
        ProcessByLine p;
        CubeAttributeInput inputAtt = CubeAttributeInput();
        CubeAttributeOutput outputAtt = CubeAttributeOutput();
        p.SetInputCube(cubeFileIn, inputAtt, 0);
        p.SetOutputCube(cubeFileOut, outputAtt, SHC_AFE_WIDTH * SHC_CHANNELS, lines, SHC_BANDS);
        p.StartProcess(ApplyRadianceCoefficients);
        p.EndProcess();
        p.Finalize();
      }
      catch (const IException &e) {
        QString msg = QString("ISIS Exception: %1. Unable to apply radiance coefficients to image.").arg(QString(e.what()));
        throw IException(e, IException::Programmer, msg, _FILEINFO_);
      }
      catch (const std::exception &e) {
        QString msg = QString("Standard exception: %1. Unable to apply radiance coefficients to image.").arg(QString(e.what()));
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
      catch (...) {
        throw IException(IException::Programmer,
          "Unknown exception occurred. Unable to apply radiance coefficients to image.", _FILEINFO_);
      }
    }
  }
}