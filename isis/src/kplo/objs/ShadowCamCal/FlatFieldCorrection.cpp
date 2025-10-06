/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "FlatFieldCorrection.h"

using namespace std;

namespace Isis {

  void FlatFieldCorrection(QString filename, PvlGroup instrument, QString cubeFileIn, QString cubeFileOut, int lines) {
    puts("Flat fielding.");

    try{
      // Default value for tdi direction "A" is zero for tdi factor
      int tdi_factor = 0;
      if (!instrument.hasKeyword("TDIDirection"))
        throw IException(IException::User, "Error: TDIDirection not found.", _FILEINFO_);
      
      if (QString::compare(instrument["TDIDirection"], "B", Qt::CaseInsensitive) == 0)
        tdi_factor = 1;
      
      vector<double> flat_coeff(SHC_CHANNELS * SHC_SCENE); 

      /**
        * @brief Lambda to read coefficients from the provided csv file and inserts values into the respective 
        * coefficient vectors
        *
        * @param filename Reference to incoming filename and directory address
        * @param flat_coeff Reference to coefficient vector
        *  
      **/
      auto ReadCoeffCSV = [&](QString filename, vector<double>& flat_coeff)->void {
        string csv_filename = GetVersionedFilename(filename);
        cout  << " Config file found: " << csv_filename << endl;;
      
        ifstream f_buffer(csv_filename.c_str());

        if (!f_buffer.is_open())
          throw IException(IException::User, "Failed to open CSV file: " + filename, _FILEINFO_);
      
        // Read scene pixels per line from the CSV file
        for (int column = 0; column < SHC_SCENE; column++) {
          // find a non-comment/header line
          string line;
          while(getline(f_buffer, line)) 
            if (line.rfind("#", 0) != 0) 
              break; 

          vector<double> coeff_tmp(SHC_CHANNELS * 2);
          stringstream ss(line);
          string token;

          getline(ss, token, ',');  // discard first token
          if(token.empty())
            throw IException(IException::User, "Expected ',' in " + filename + ". loading CSV failed.\n", _FILEINFO_);

          for(int i=0; i < SHC_CHANNELS * 2; i++){
            getline(ss, token, ',');
            if(token.empty())
              throw IException(IException::User, "Expected ',' in " + filename + ". loading CSV failed.\n", _FILEINFO_);

            coeff_tmp.at(i) = stod(token);  
          }

          // Assign values to flat_coeff of length 3072
          for (int channel = 0; channel < SHC_CHANNELS; ++channel) 
            flat_coeff.at(channel * SHC_SCENE + column) = coeff_tmp.at(channel + SHC_CHANNELS * tdi_factor);
          
        }
      };

      /**
        * @brief Lambda to apply the flatfield correction
        * to each pixel
        *  
        * This lambda function applies the flatfield correction to each pixel by dividing each pixel by the flatfield coefficient, which is 
        * calculated using the csv file provided by the user or by default. 
        *
        * @param in Reference to the input buffer.
        * 
      **/
      auto CorrectionFlatfield = [&](Isis::Buffer& in, Isis::Buffer& out)->void {

        // fill output buffer since flatfield correction is only applied to scene pixels
        for(int i = 0; i < in.size(); i++)
          out[i] = in[i];

        //flatfield is only applied to scene pixels
        for (int channel = 0; channel < SHC_CHANNELS; ++channel) {
          for(int column = 0; column < SHC_SCENE; column++) {

            uint16_t sample = column + SHC_SCENE_OFFSET;
            int index = GetDataIndex(channel, SHC_AFE_WIDTH, sample);
            int coeff_index = channel * SHC_SCENE + column;

            if (!IsSpecialPixelSHC(in[index])){
              if(flat_coeff.at(coeff_index) == 0){
                QString msg = "ERROR (divideByZero): Flatfield coefficient is zero for flat_coeff[" + 
                Isis::toString( static_cast<int>(coeff_index)) + "].";
                throw IException(IException::Programmer, msg, _FILEINFO_);
              }

              out[index] = in[index] / flat_coeff.at(coeff_index);
            }
          }
        }
      };

      // Read coefficients from CSV file
      ReadCoeffCSV(filename, flat_coeff);

      // Set up process to apply flatfield correction
      ProcessByLine p;
      CubeAttributeInput inputAtt = CubeAttributeInput();
      CubeAttributeOutput outputAtt = CubeAttributeOutput();
      p.SetInputCube(cubeFileIn, inputAtt, 0);
      p.SetOutputCube(cubeFileOut, outputAtt, SHC_AFE_WIDTH * SHC_CHANNELS, lines, SHC_BANDS);
      p.StartProcess(CorrectionFlatfield);
      p.EndProcess();
      p.Finalize();
    }
    catch (const IException& e) {
      throw IException(e, IException::Programmer, "ISIS Exception: " + Isis::toString(e.what()) + ". Unable to apply flatfield correction to image.", _FILEINFO_);
    }
    catch (const std::exception &e) {
      cerr << "Standard exception: " << e.what() << ". Unable to apply flatfield correction to image." << endl;
      exit(1);
    }
    catch (...) {
      throw IException(IException::Programmer, "Unknown exception occured. Unable to apply flatfield correction to image.", _FILEINFO_);
    }
  }
}
