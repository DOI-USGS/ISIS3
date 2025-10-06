/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "DarkSubtraction.h"

using namespace std;

namespace Isis {
  void DarkSubtraction(QString slope_filename, QString intrcpt_filename, PvlGroup instrument, QString cubeFileIn, QString cubeFileOut, int lines){
    puts("Dark subtraction.");

    try{
      const double fpa_a_temp = (GetFromLabels(instrument, "TemperatureFPAA")).toDouble();
      const double line_rate_ms = (GetFromLabels(instrument, "LineRate")).toDouble();
      // Default value for tdi direction "A" is zero for tdi factor
      int tdi_factor = 0;
      if (!instrument.hasKeyword("TDIDirection"))
        throw IException(IException::User, "Error: TDIDirection not found.", _FILEINFO_);
      
      if (QString::compare(instrument["TDIDirection"], "B", Qt::CaseInsensitive) == 0)
        tdi_factor = 1;
      
      int vktr_size = SHC_SCENE * SHC_AFE_WIDTH;
      
      vector<double> slope_a(vktr_size);
      vector<double> slope_b(vktr_size);
      vector<double> slope_rmse(vktr_size);
      vector<double> intrcpt_a(vktr_size);
      vector<double> intrcpt_b(vktr_size);
      vector<double> intrcpt_rmse(vktr_size);

      /**
        * @brief Lambda to read slope and intercept coefficients
        * from the provided csv file and inserts values into
        * the respective coefficent vectors
        *
        * @param filename Reference to incoming filename and directory address
        * @param coeff_a
        * @param coeff_b
        * @param coeff_rmsa
        * 
        *  
      **/
      auto ReadCoeffCSV = [&](QString filename, vector<double>& coeff_a, vector<double>& coeff_b, vector<double>& coeff_rmse)->void { 
        string csv_filename = GetVersionedFilename(filename);
        cout  << " Config file found: " << csv_filename << endl;;

        ifstream f_buffer(csv_filename.c_str());

        if (!f_buffer.is_open()) {
          throw IException(IException::User, "Failed to open CSV file: " + filename, _FILEINFO_);
        }
  
        for (int channel = 0; channel < SHC_CHANNELS; channel++){
          for (int column = 0; column < SHC_SCENE; column++) {
            string line;

            while(getline(f_buffer, line))
              if ((line.rfind("#", 0) != 0) && (ToLower(line).rfind("tdi", 0) != 0)) 
                break;
                
            vector<double> coeff_tmp(SHC_CHANNELS);

            stringstream ss(line);
            string token = "";
            getline(ss, token, ',');
            if(token == ""){
              throw IException(IException::User, "Expected ',' in file. Loading CSV failed.", _FILEINFO_);
            }
            
            for (int i=0; i < 6; i++){
              try{
                coeff_tmp.at(i) = stod(token);
              }
              catch (const std::exception& e) {
                cout << "token: " << token << endl;
                throw IException(IException::User, ("Error reading CSV file: " + string(e.what())).c_str(), _FILEINFO_);
              }
              if(i != 5){
                token.clear();
                getline(ss, token, ',');
                if(token == ""){
                  throw IException(IException::User, "Expected ',' in file. Loading CSV failed", _FILEINFO_);
                }
              }
            }

            uint16_t index = channel * SHC_SCENE + column;

            if(coeff_a.size() < index){
              coeff_a.resize(index);
              coeff_b.resize(index);
              coeff_rmse.resize(index);
              cout << "resized coeff vectors to " << index << endl;
            }

            coeff_a.at(index) = coeff_tmp.at(0 + 3 * tdi_factor);
            coeff_b.at(index) = coeff_tmp.at(1 + 3 * tdi_factor);
            coeff_rmse.at(index) = coeff_tmp.at(2 + 3 * tdi_factor);
          }
        }
      };

      /**
        * @brief Lambda to subtract dark current from
        * each pixel
        *  
        * This lambda function subtracts the dark current
        * named dark_level, which is calculated using 
        * the two csv files provided by the user or by
        * default. This value is then subracted by each 
        * scene pixel value and assigned to that pixel. 
        *
        * @param in Reference to the input buffer.
        * 
      **/
      auto SubtractionDark = [&](Isis::Buffer &in, Isis::Buffer &out)->void {
        // fill buffer
        for(int i = 0; i < in.size(); i++){
          out[i] = in[i];
        }

        for (int channel=0; channel < SHC_CHANNELS; channel++){
          for (int column=0; column < SHC_SCENE; column++) {
            uint16_t sample = column + SHC_SCENE_OFFSET;
            uint16_t ci = channel * SHC_SCENE + column;
            int index = GetDataIndex(channel, SHC_AFE_WIDTH, sample);

            if (!IsSpecialPixelSHC(in[index])) {
              // Calculate dark level based on temperature, line rate, and coefficients
              double dark_level = slope_a.at(ci) * exp(slope_b.at(ci) * fpa_a_temp) * line_rate_ms + intrcpt_a.at(ci) * exp(intrcpt_b.at(ci) * fpa_a_temp);
              out[index] = in[index] - dark_level;
            }
          }
        }
      };

      ReadCoeffCSV(slope_filename, slope_a, slope_b, slope_rmse);
      ReadCoeffCSV(intrcpt_filename, intrcpt_a, intrcpt_b, intrcpt_rmse);

      ProcessByLine p;
      CubeAttributeInput inputAtt = CubeAttributeInput();
      CubeAttributeOutput outputAtt = CubeAttributeOutput();
      p.SetInputCube(cubeFileIn, inputAtt, 0);
      p.SetOutputCube(cubeFileOut, outputAtt, SHC_AFE_WIDTH * SHC_CHANNELS, lines, SHC_BANDS);
      p.StartProcess(SubtractionDark);
      p.EndProcess();
      p.Finalize();
    }
    catch (const IException& e) {
      throw IException(e, IException::Programmer, "ISIS Exception: " + Isis::toString(e.what()) + ". Unable to apply dark correction to image.", _FILEINFO_);
    }
    catch (const std::exception &e) {
      cerr << "Standard exception: " << e.what() << ". Unable to apply dark correction to image." << endl;
      exit(1);
    }
    catch (...) {
      throw IException(IException::Programmer, "Unknown exception occured. Unable to apply dark correction to image.", _FILEINFO_);
    }
  }
}