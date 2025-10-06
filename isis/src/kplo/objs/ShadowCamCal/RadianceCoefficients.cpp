#include "RadianceCoefficients.h"

using namespace std;

namespace Isis {

  void RadianceCoefficients(QString filename, PvlGroup instrument, QString cubeFileIn, QString cubeFileOut, int lines){
    puts("Radiance");

    try{    
      // Default value for tdi direction "A" is zero for tdi factor
      int tdi_factor = 0;
      if (!instrument.hasKeyword("TDIDirection"))
        throw IException(IException::User, "Error: TDIDirection not found.", _FILEINFO_);
      
      if (QString::compare(instrument["TDIDirection"], "B", Qt::CaseInsensitive) == 0)
        tdi_factor = 1;
      
      static double line_rate_ms = (GetFromLabels(instrument, "LineRate")).toDouble();

      vector<double> radiance_coeff(SHC_CHANNELS);

      /**
        * @brief Lambda to read coefficients from the provided CSV file and inserts values into
        * the respective coefficient vectors
        *
        * @param filename Reference to incoming filename and directory address
        * @param coeff    Reference to coefficient vector
        */  
      auto ReadCoeffCSV = [&](QString filename, vector<double>& coeff)->void {
        string line;

        string csv_filename = GetVersionedFilename(filename);
        cout  << " Config file found: " << csv_filename << endl;;

        ifstream f_buffer(csv_filename.c_str());

        // Check if the file is empty
        if (!f_buffer.is_open())
          throw IException(IException::User, "Failed to open CSV file: " + filename, _FILEINFO_);

        while(getline(f_buffer, line)) {
          if ((line.rfind("#", 0) == 0))
            continue;
          
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

          for (int channel = 0; channel < SHC_CHANNELS; channel++){
            coeff.at(channel) = coeff_tmp.at(channel + SHC_CHANNELS * tdi_factor);
            cout << "  Radiance for channel " << channel << ": " << coeff.at(channel) << endl;
          }
        }
      };

      /**
        * @brief Lambda to apply radiance correction to each pixel
        *  
        * This lambda function applies the radiance correction by dividing each pixel by the product
        * of the line rate and the channel's radiance coefficient from the CSV file provided by the 
        * user or by default. 
        *
        * @param in Reference to the input buffer.
        */
      auto ApplyRadianceCoefficients = [&](Isis::Buffer &in, Isis::Buffer &out)->void {
        for (int channel = 0; channel < SHC_CHANNELS; channel++) {
          for (int pixel = 0; pixel < SHC_AFE_WIDTH; pixel++) {
            int index = GetDataIndex(channel, SHC_AFE_WIDTH, pixel);
            if (!IsSpecialPixelSHC(in[index])){
              double lr_radxcoeff = line_rate_ms * radiance_coeff.at(channel);
              if (lr_radxcoeff == 0)
                throw IException(IException::Programmer, "ERROR (divideByZero): linerate * radiance coefficient is zero ", _FILEINFO_);
              else
                out[index] = in[index] / lr_radxcoeff;
            }
            else 
              out[index] = in[index];
          }
        }
      };

      // Read coefficients from CSV file
      ReadCoeffCSV(filename, radiance_coeff);

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
    catch (const IException& e) {
      throw IException(e, IException::Programmer, "ISIS Exception: " + Isis::toString(e.what()) + ". Unable to apply radiance coefficients to image.", _FILEINFO_);
    }
    catch (const std::exception &e) {
      cerr << "Standard exception: " << e.what() << ". Unable to apply radiance coefficients to image." << endl;
      exit(1);
    }
    catch (...) {
      throw IException(IException::Programmer, "Unknown exception occured. Unable to apply radiance coefficients to image.", _FILEINFO_);
    }
  }
}
