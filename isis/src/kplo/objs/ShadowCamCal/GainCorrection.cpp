/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "GainCorrection.h"

using namespace std;

namespace Isis {
  void GainCorrection(QString cubeFileIn, QString cubeFileOut, QString filename, PvlGroup instrument, int lines ){
    puts("Performing gain correction");

    // get gain channels from labels
    static double gain_channel[TERMS];

    try{
      for(int i = 0; i < TERMS; i++){
        QString gch = "GainCh" + toString(i);
        if(instrument.hasKeyword(gch))
          gain_channel[i] = instrument[gch];
        else
          throw IException(IException::User, "Gain Channels not found in Instrument label group.", _FILEINFO_);
      }
      
      // Default value for tdi direction "A" is zero for tdi factor
      int tdi_factor = 0;
      if (!instrument.hasKeyword("TDIDirection"))
        throw IException(IException::User, "Error: TDIDirection not found.", _FILEINFO_);
      
      if (QString::compare(instrument["TDIDirection"], "B", Qt::CaseInsensitive) == 0)
        tdi_factor = 1;

      static vector<double>gain_factor_vktr(SHC_CHANNELS); 

      /**
        * @brief Lambda to get coefficients from csv files
        *  
        * This lambda function reads the coefficent csv file line by line and adds to gain_factor_vktr which 
        * will be used when applying the gain correction. 
        *
        * @param in Reference to the input buffer.
        * 
      **/
      auto ReadCoeffCSV = [&](QString filename, vector<double>& flat_coeff) {
        string line;
        string csv_filename = GetVersionedFilename(filename);
        cout  << " Config file found: " << csv_filename << endl;;

        ifstream f_buffer(csv_filename.c_str());
        
        if(!f_buffer.is_open())
          throw IException(IException::User, "Unable to open csv file!", _FILEINFO_);
        
        while(getline(f_buffer, line)){
          if((line.rfind("#", 0) == 0) || (line.rfind("gain_code", 0) == 0)) 
            continue;

          vector<double> gain_tmp(SHC_CHANNELS * 2);

          stringstream ss(line);
          string token;

          getline(ss, token, ',');
          if (token.empty())
            throw IException(IException::User, "Gain Code not found in CSV. Loading CSV failed.", _FILEINFO_);
          
          double gain_code = stod(token);
          
          for(int i = 0; i < SHC_CHANNELS * 2; i++){
            getline(ss, token, ',');
            
            if (token.empty()){
              throw IException(IException::User, "Expected a ',' in " + line + ". Loading CSV failed.", _FILEINFO_);
            }
            
            gain_tmp.at(i) = stod(token);
          }      

          for (int channel = 0; channel < SHC_CHANNELS; channel++) {
            if (gain_channel[channel] == gain_code) {
              flat_coeff.at(channel) = gain_tmp.at(channel + SHC_CHANNELS * tdi_factor);

              QString msg = "Gain for channel " + Isis::toString( static_cast<int>(channel)) + ": " + Isis::toString(static_cast<double>(gain_factor_vktr.at(channel))) + "\n";
              cout << msg << endl;
              
              if (flat_coeff.at(channel) == 0) {
                cout << "WARNING: " << msg << endl;
              }
            }
          }
        }
      };

      /**
        * @brief Lambda to apply the gain correction to each pixel
        *  
        * This lambda function divides each pixel by the the channel's gain factor which is calculated 
        * using the values found in the csv file provided by the user or by default. 
        *
        * @param in Reference to the input buffer.
        * 
      **/
      auto CorrectionGain = [&](Isis::Buffer &in, Isis::Buffer &out)->void {
        for (int channel=0; channel < SHC_CHANNELS; channel++){
          for (int pixel=0; pixel < SHC_AFE_WIDTH; pixel++){
            int index = GetDataIndex(channel, SHC_AFE_WIDTH, pixel);
            if(index >= in.size())
              cout << "index is equal to or greater than index.size()" << endl;
            double gain_factor = gain_factor_vktr.at(channel);

            if (!IsSpecialPixelSHC(in[index])){
              if(gain_factor == 0)
                throw IException(IException::User, "WARNING: Gain for channel " + Isis::toString( static_cast<int>(channel)) + ":" + Isis::toString(static_cast<double>(gain_factor)) + ". Can't apply correction (divideByZero).", _FILEINFO_);

              out[index] = in[index] / gain_factor;
            }
            else{
              out[index] = in[index];
            }
          }
        }
      };

      // Read coefficients from CSV file
      ReadCoeffCSV(filename, gain_factor_vktr);
      
      ProcessByLine p;
      CubeAttributeInput inputAtt = CubeAttributeInput();
      CubeAttributeOutput outputAtt = CubeAttributeOutput();
      p.SetInputCube(cubeFileIn, inputAtt, 0);
      p.SetOutputCube(cubeFileOut, outputAtt, SHC_AFE_WIDTH * SHC_CHANNELS, lines, SHC_BANDS);
      
      // Apply correction line by line
      p.StartProcess(CorrectionGain);
      p.EndProcess();
      p.Finalize();
    }
    catch (const IException& e) {
      throw IException(e, IException::Programmer, "ISIS Exception: " + Isis::toString(e.what()) + ". Unable to apply gain correction to image.", _FILEINFO_);
    }
    catch (const std::exception &e) {
      cerr << "Standard exception: " << e.what() << ". Unable to apply gain correction to image." << endl;
      exit(1);
    }
    catch (...) {
      throw IException(IException::Programmer, "Unknown exception occured. Unable to apply gain correction to image.", _FILEINFO_);
    }
  }
}

