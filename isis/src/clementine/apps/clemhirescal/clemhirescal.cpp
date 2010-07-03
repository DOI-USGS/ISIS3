#include "Isis.h"

#include "Cube.h"
#include "CubeAttribute.h"
#include "iException.h"
#include "iString.h"
#include "ProcessByLine.h"
#include "Pvl.h"
#include "SpecialPixel.h"

#include <vector>

using namespace std; 
using namespace Isis;

void clemhirescal(vector <Buffer *> &in, vector <Buffer *> &out);

double offset;
double abscoef;

void IsisMain() {
  // Open the input cube
  ProcessByLine p;
  Cube *input = p.SetInputCube("FROM");
  
  // Check for filter type of A-D
  Pvl *label = input->Label();
  iString wave = (string)label->FindGroup("BandBin", Pvl::Traverse)["FilterName"];
  if ((wave != "A") && (wave != "B") && (wave != "C") && (wave != "D")) {
    string message = "Invalid FilterName [" + wave + "], can only handle A-D filters";
    throw iException::Message(Isis::iException::None, message, _FILEINFO_);
  }
  // Determine and load calibration flat field file
  wave.DownCase();
  iString flatFile("$Clementine1/calibration/hires/lh" +
                    wave + "_flat.cub");
  CubeAttributeInput cubeAtt;
  p.SetInputCube(flatFile, cubeAtt);

   // Check the offset mode for validity
  int index = label->FindGroup("Instrument", Pvl::Traverse)["OffsetModeID"];
  if (index < 0 || index > 5) {
    string message = "Invalid OffsetModeID, can only handle offests 0-5";
    throw iException::Message(Isis::iException::None, message, _FILEINFO_);
  }

  // Set the offset (b0) value based on mode
  double dataOffset[] = {-49.172, -41.0799, -32.8988, -24.718, -16.98, -8.0};
  offset = dataOffset[index];

  // Computer the K value to convert to I/F.  The K value per MCP and wavelength
  // were obtained from JGR publication Vol 108, A radiometric calibration for the 
  // Clementine HIRES camera: Robinson, Malart, White, page 17
  UserInterface &ui = Application::GetUserInterface();
  if (ui.GetString("KFROM").compare("COMPUTED") == 0) {
    wave.UpCase();
    int MCP = label->FindGroup("Instrument", Pvl::Traverse)["MCPGainModeID"];
    // Two possible MCP gains for filter A
    if (wave == "A") {
      if (MCP == 156) {
        abscoef = 0.00105;
      }
      else if (MCP == 159) {
        abscoef = 0.00089;
      }
      else {
        string message = "Image is not one of supported MCP Gain Mode IDs, enter your own K value";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    // Three possiblities for filter D
    else if (wave == "D") {
      if (MCP == 151) {
        abscoef = 0.001655;
      }
      else if (MCP == 154) {
        abscoef = 0.001375;
      }
      else if (MCP == 158) {
        abscoef = 0.00097;
      }
      else {
        string message = "Image is not one of supported MCP Gain Mode IDs, enter your own K value";
        throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
      }
    }
    // Other filters not supported for preset K value
    else {
      string message = "Image is of filter [" + wave + "], not supported type A or D, enter your own K value";
      throw Isis::iException::Message(Isis::iException::User, message, _FILEINFO_);
    }
  }

  // Otherwise get K value from user
  else {
    abscoef = ui.GetDouble("KVALUE");
  }

  // K, Offset, and flat file are defined.  Create output cube and process
  // each line
  p.SetOutputCube("TO");
  p.StartProcess(clemhirescal);
  p.EndProcess();
}


void clemhirescal (vector <Buffer *> &in, vector <Buffer *> &out) {
  Buffer &num = *in[0];   // Input line
  Buffer &den = *in[1];   // Denominator line
  Buffer &rat = *out[0];  // Output line

  for (int i = 0 ; i < num.size() ; i ++) { 
    // If it is special, write it out and continue
    if (IsSpecial(num[i])) {                
      rat[i] = num[i];                      
    }                                       
    else {
      //Check denominator for both unusable conditions
      if (IsSpecial(den[i]) || den[i] == 0.0) {
        rat[i] = Isis::Null;
      } 
      else {
        //Do ratio and multiply by k constant.
        rat[i] = ((num[i] + offset) / den[i]) * abscoef; 
      }     
    }
  }
}
