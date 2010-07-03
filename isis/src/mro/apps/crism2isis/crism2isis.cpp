#include "Isis.h"

#include "ProcessImportPds.h"
#include "UserInterface.h"
#include "Filename.h"
#include "TextFile.h"

using namespace std;
using namespace Isis;

void IsisMain() 
{
  ProcessImportPds p;
  Pvl pdsLabel;
  UserInterface &ui = Application::GetUserInterface();

  Filename inFile = ui.GetFilename("FROM");

  p.SetPdsFile (inFile.Expanded(), "", pdsLabel);
  //65535 is set to NULL
  p.SetNull(65535, 65535);

  Cube *ocube = p.SetOutputCube("TO");

  Pvl outLabel;

  Pvl labelPvl (inFile.Expanded());

  iString prodType;

  if(labelPvl.HasKeyword("PRODUCT_TYPE")) {
    prodType = (string)labelPvl.FindKeyword("PRODUCT_TYPE");
  }
  else {
    string msg = "Unsupported CRISM file type, supported types are: DDR, MRDR, and TRDR";
    throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
  }

  if(prodType.Equal("MAP_PROJECTED_MULTISPECTRAL_RDR")) {
    iString prodId;
    if(labelPvl.HasKeyword("PRODUCT_ID")) {
      prodId = (string)labelPvl.FindKeyword("PRODUCT_ID");
      prodId = prodId.substr(prodId.find("_")+1, prodId.find("_"));
    }
    else {
      string msg = "Could not find label PRODUCT_ID, invalid MRDR";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }
  
    //If the product type is AL (Lambert albedo) or IF (I/F)
    //Read the wavelength table and put the corresponding 
    //widths in the band bin group
    if(prodId.Equal("MRRAL") || prodId.Equal("MRRIF")) {
      //If the wavelength file is specified in the label
      if(labelPvl.HasKeyword("MRO:WAVELENGTH_FILE_NAME")) {
        PvlGroup bandBin = PvlGroup("BandBin");
        PvlKeyword origBand = PvlKeyword("OriginalBand");
        PvlKeyword widths = PvlKeyword("Width");
        iString tablePath = (string)labelPvl.FindKeyword("MRO:WAVELENGTH_FILE_NAME");
        tablePath = tablePath.DownCase();
        Filename tableFile = inFile.Path() + "/" + tablePath;
        //Check if the wavelength file exists
        if(tableFile.Exists()) {
          TextFile *fin = new TextFile(tableFile.Expanded());
          // Open table file
          if (!fin->OpenChk()) {
            string msg = "Cannot open wavelength table [" + tableFile.Expanded() + "]";
            throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
          }
  
          //For each line in the wavelength table, add the width to
          //The band bin group
          iString st;
          int band = 1;
          while(fin->GetLine(st)) {
            st.Token(",");
            st.Token(",");
            origBand += band;
            widths += st.Token(",").ConvertWhiteSpace().Trim(" ").ToDouble();
            band++;
          }
          delete fin;
  
          bandBin.AddKeyword(origBand);
          bandBin.AddKeyword(widths);
          ocube->PutGroup(bandBin);
        }
        //Otherwise throw an error
        else {
          string msg = "Cannot fine wavelength table [" + tableFile.Expanded() + "]";
          throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
        }
      }
    } 
    //If the product type is DE (Derived products for I/F) or DL 
    //(Derived products for Lambert albedo) write the band names 
    //to the band bin group
    else if(prodId.Equal("MRRDE") || prodId.Equal("MRRDL")) {
      PvlGroup bandBin = PvlGroup("BandBin");
      PvlKeyword origBand = PvlKeyword("OriginalBand");
      PvlKeyword bandName = PvlKeyword("BandName");
      PvlKeyword bandNames = labelPvl.FindObject("IMAGE").FindKeyword("BAND_NAME");
      for (int i = 0; i < bandNames.Size(); i++) {
        origBand += (i+1);
        bandName += bandNames[i];
      }
      bandBin.AddKeyword(origBand);
      bandBin.AddKeyword(bandName);
      ocube->PutGroup(bandBin);
    }  
    //Translate the Mapping group
    p.TranslatePdsProjection(outLabel);
    ocube->PutGroup(outLabel.FindGroup("Mapping", Pvl::Traverse));
  } 
  else if(prodType.Equal("TARGETED_RDR")) {
  }
  else if(prodType.Equal("DDR")) {
    PvlGroup bandBin = PvlGroup("BandBin");
    PvlKeyword origBand = PvlKeyword("OriginalBand");
    PvlKeyword bandName = PvlKeyword("BandName");
    PvlKeyword bandNames = labelPvl.FindObject("FILE").FindObject("IMAGE").FindKeyword("BAND_NAME");
    for (int i = 0; i < bandNames.Size(); i++) {
      origBand += (i+1);
      bandName += bandNames[i];
    }
    bandBin.AddKeyword(origBand);
    bandBin.AddKeyword(bandName);
    ocube->PutGroup(bandBin);
  }
  else {
    string msg = "Unsupported CRISM file type, supported types are: DDR, MRDR, and TRDR";
    throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
  }

  // Translate the Instrument group
  Filename transFile ("$mro/translations/crismInstrument.trn");
  PvlTranslationManager instrumentXlater (labelPvl, transFile.Expanded());
  instrumentXlater.Auto (outLabel);
  
  // Translate the Archive group
  transFile  = "$mro/translations/crismArchive.trn";
  PvlTranslationManager archiveXlater (labelPvl, transFile.Expanded());  
  archiveXlater.Auto (outLabel);

  ocube->PutGroup(outLabel.FindGroup("Instrument", Pvl::Traverse));
  ocube->PutGroup(outLabel.FindGroup("Archive", Pvl::Traverse));

  p.StartProcess();
  p.EndProcess();

  return;
}
