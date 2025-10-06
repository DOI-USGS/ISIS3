/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "shadowcamcal.h"

using namespace std;

namespace Isis {

  void BiasPixelSubtraction(bool use_median, QString tmpCubeFileIn, QString tmpCubeFileOut, int lines) {
    puts("Subtracting per-channel bias pixel average.");
    if (use_median) 
      puts("Performing bias median average pixel subtraction");
    else
      puts("Performing bias mean average pixel subtraction");

    vector<vector<double>> bias_pile(SHC_CHANNELS);
    vector<double> bias_median(SHC_CHANNELS, -1.0);
    vector<double> bias_mean(SHC_CHANNELS);

    try{
      std::unique_ptr<LineManager> lineMgr;

      /**
        * @brief Lambda to subtract bias pixel averages
        *
        * This lambda function subtracts bias pixel averages, either mean or median from the
        * each sample, hence entire image. 
        *
        * @param in Reference to the input buffer.
        * 
      **/
      auto SubtractionBiasPixels = [&](Isis::Buffer &in, Isis::Buffer &out)->void { 
        for (int channel = 0; channel < SHC_CHANNELS; channel++) {
          for (int pixel = 0; pixel < SHC_AFE_WIDTH; pixel++) {
            int index = GetDataIndex(channel, SHC_AFE_WIDTH, pixel);
            if (!IsSpecialPixelSHC(in[index])) {
              if (use_median) {
                if (bias_median.at(channel) < 0)
                  cout << "WARNING: bias median for channel is negative " << endl;
                
                out[index] = in[index] - bias_median.at(channel);
              } 
              else {
                if (bias_mean[channel] < 0)
                  cout << "WARNING: bias mean for channel is negative." << endl;
                
                out[index] = in[index] - bias_mean[channel];
              }
            } 
            else 
              out[index] = in[index];
          }
        }
      };

      // Open input cube
      Cube *iCube = new Cube();
      iCube->open(tmpCubeFileIn, "r");
      lineMgr = std::make_unique<LineManager>(*iCube);

      // Collect non-special bias pixels in a pile
      for (int line = 1; line <= lines; line++) {
        lineMgr->SetLine(line, SHC_BANDS); 
        iCube->read(*lineMgr);
        for (int channel = 0; channel < SHC_CHANNELS; channel++) {
          for (int pixel = 2; pixel < 10; ++pixel) {
            int index = (channel * SHC_AFE_WIDTH) + pixel;
            if (!IsSpecialPixelSHC((*lineMgr)[index])) 
              bias_pile.at(channel).push_back((*lineMgr)[index]);
          }
        }
      }

      // Calculate bias median and mean
      for (int channel = 0; channel < SHC_CHANNELS; ++channel) {
        sort(bias_pile.at(channel).begin(), bias_pile.at(channel).end());

        // Calculate median
        int bias_count_per_channel = bias_pile.at(channel).size();
        if (bias_count_per_channel % 2 == 0) {
          bias_median.at(channel) = static_cast<double>(bias_pile.at(channel).at((bias_count_per_channel / 2 - 1)) + bias_pile.at(channel).at((bias_count_per_channel / 2))) / 2.0;
        }
        else
          bias_median.at(channel) = bias_pile.at(channel).at(bias_count_per_channel / 2);
        
        // Calculate mean
        double sum = accumulate(bias_pile.at(channel).begin(), bias_pile.at(channel).end(), 0.0);
        if (bias_count_per_channel == 0)
          throw IException(IException::Programmer, "ERROR (divideByZero): bias count per channel is zero (sum/bias_count_per_channel)" , _FILEINFO_);

        bias_mean.at(channel) = sum / bias_count_per_channel;
      }

      // Process the cube
      ProcessByLine p;
      CubeAttributeInput inputAtt = CubeAttributeInput();
      CubeAttributeOutput outputAtt = CubeAttributeOutput();
      p.SetInputCube(tmpCubeFileIn, inputAtt, 0);
      p.SetOutputCube(tmpCubeFileOut, outputAtt, SHC_AFE_WIDTH * SHC_CHANNELS, lines, SHC_BANDS);
      p.StartProcess(SubtractionBiasPixels);
      p.EndProcess();
      p.Finalize();
    }
    catch (const IException& e) {
      throw IException(e, IException::Programmer, "ISIS Exception: " + Isis::toString(e.what()) + ". Unable to apply bias pixel subtraction to image.", _FILEINFO_);
    }
    catch (const std::exception &e) {
      cerr << "Standard exception: " << e.what() << ". Unable to apply bias pixel subtraction to image." << endl;
      exit(1);
    }
    catch (...) {
      throw IException(IException::Programmer, "Unknown exception occured. Unable to apply bias pixel subtraction to image.", _FILEINFO_);
    }
  }
}
