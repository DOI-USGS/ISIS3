/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2009/12/17 21:13:17 $
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch 
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website, 
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */                                                                 

#include <iostream>
#include <string>
#include <sstream>

#include "Preference.h"

#include "ProcessImportVicar.h"
#include "iException.h"
#include "iException.h"
#include "iException.h"
#include "LineManager.h"
#include "Pvl.h"
#include "PixelType.h"
#include "SpecialPixel.h"
#include "iString.h"
#include "UserInterface.h"

using namespace std;
namespace Isis {
 /** 
  * Opens a vicar file which can then be immediately imported by invoking the 
  * inherited StartProcess method. 
  * 
  * @param vicarFile Name of the vicar file to open
  * 
  * @param vicarLab A PVL object which will contain the vicar labels.
  * 
  * @throws Isis::iException::Message
  */
  void ProcessImportVicar::SetVicarFile (const std::string &vicarFile, Isis::Pvl &vicarLab) {

    //  Open vicar file
    ifstream vicFile (vicarFile.c_str (), ios::in);
  
    if (!vicFile) {
      string msg = "Cannot open vicar file [" + vicarFile + "]";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
  
    // Read the LBLSIZE from Vicar file
    try {
      //  Convert the LBLSIZE to an integer
      // theoretically this value is always size 4....
      char *lblSizeValue = new char [1024];
      vicFile.seekg (string("LBLSIZE=").size());

      for(int pos = 0; pos < 1024-1; pos++) {
        if(!vicFile.good())
          break;

        if(vicFile.peek() == ' ') 
          break;

        lblSizeValue[pos] = vicFile.get();
        lblSizeValue[pos+1] = '\0';

        // we're totally lost at this point
        if(pos == 1023) {
          string msg = "Cannot read vicar file [" + vicarFile + "]";
          throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
        }
      }

      int lblSize = iString(lblSizeValue).ToInteger();

      delete [] lblSizeValue;
      lblSizeValue = NULL;

      char *buf = new char[lblSize+1];
    
      //  Read all of VICAR labels
      vicFile.seekg (0);
      vicFile.read (buf, lblSize);
      buf[lblSize] = '\0';
      vicFile.close ();
  
      // Transform the vicar labels into valid pvl labels
      iString vicLabels = buf;

      bool inQuote = false;
      for(unsigned int pos = 0; pos < vicLabels.size(); pos++) {
        if(vicLabels[pos] == '\'' ||
           vicLabels[pos] == '"') {
          inQuote = !inQuote;
        }

        if(!inQuote && vicLabels[pos] == ' ') {
          vicLabels[pos] = '\n';
        }
      }

      // Fill temp Pvl label for Isis::ProcessImport startprocess
      stringstream lbl;
      lbl << vicLabels << " End" << endl;
      Isis::Pvl vLab;
      lbl >> vLab;
      vicarLab = vLab;
    
      //  Set the fileHeaderBytes
      SetFileHeaderBytes(vLab["LBLSIZE"]);
  
      //  Set the dataHeaderBytes
      SetDataHeaderBytes((int) vLab["NLB"] * (int)vLab["RECSIZE"]);
    
      //  Are there binary prefix bytes on each image line?
      SetDataPrefixBytes(vLab["NBB"]);
      SetDataSuffixBytes(0);
      
      SetDimensions(vLab["NS"],vLab["NL"],vLab["NB"]);
  
      string pixType = vLab["FORMAT"];
      Isis::PixelType pixelType = Isis::None;
      if (pixType == "BYTE") pixelType = Isis::UnsignedByte;
      if (pixType == "HALF") pixelType = Isis::SignedWord;
      if (pixType == "REAL") pixelType = Isis::Real;
      if (pixelType == Isis::None) {
        string msg = "Unsupported pixel type [FORMAT=" + pixType + "]";
        throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);      
      }
      SetPixelType(pixelType);
    
      string order = vLab["INTFMT"];
      if (order == "LOW") {
        SetByteOrder(Isis::Lsb);
      }
      else {
        SetByteOrder(Isis::Msb);
      }
    
      string organization = vLab["ORG"];
      if (organization == "BSQ") {
        SetOrganization(ProcessImport::BSQ);
      }
      else if (organization == "BIL") {
        SetOrganization(ProcessImport::BIL);
      }
      else if (organization == "BIP") {
        SetOrganization(ProcessImport::BIP);
      }
      else {
        string msg = "Unsupported file organization [ORG=" + organization + "]";
        throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
      }
  
      // See if there is end-of-dataset labels
      // If so read them and merge
      if (vLab.HasKeyword("EOL")) {
        if ((int) vLab["EOL"] == 1) {
          int startByte = (int) vLab["LBLSIZE"] + 
                          (int) vLab["NLB"] * (int) vLab["RECSIZE"] +
                          (int) vLab["NL"] * (int) vLab["NB"] *
                          (int) vLab["RECSIZE"]; 
          ifstream vicFile (vicarFile.c_str (), ios::in);
          vicFile.seekg (startByte,ios::beg);
          Isis::Pvl endLab;
          vicFile >> endLab;
          vicFile.close();
  
          for (int k=0; k<endLab.Keywords(); k++) {
            vicarLab += endLab[k];
          }
        }
      }
    }
    catch (Isis::iException &e) {
      string msg = "Input file [" + vicarFile + "] does not appear to be a vicar file";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
  
    SetInputFile(vicarFile);
  }
} // end namespace isis
