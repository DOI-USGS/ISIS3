#include "Isis.h"

#include "ProcessImportPds.h"
#include "UserInterface.h"
#include "FileName.h"
#include "TextFile.h"

using namespace std;
using namespace Isis;

void IsisMain() {
  ProcessImportPds p;
  Pvl pdsLabel;
  UserInterface &ui = Application::GetUserInterface();

  FileName inFile = ui.GetFileName("FROM");

  p.SetPdsFile(inFile.expanded(), "", pdsLabel);
  //65535 is set to NULL
  p.SetNull(65535, 65535);

  Cube *ocube = p.SetOutputCube("TO");

  Pvl outLabel;

  Pvl labelPvl(inFile.expanded());

  IString prodType;

  if(labelPvl.HasKeyword("PRODUCT_TYPE")) {
    prodType = (string)labelPvl.FindKeyword("PRODUCT_TYPE");
  }
  else {
    string msg = "Unsupported CRISM file type, supported types are: DDR, MRDR, and TRDR";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if(prodType.Equal("MAP_PROJECTED_MULTISPECTRAL_RDR")) {
    IString prodId;
    if(labelPvl.HasKeyword("PRODUCT_ID")) {
      prodId = (string)labelPvl.FindKeyword("PRODUCT_ID");
      prodId = prodId.substr(prodId.find("_") + 1, prodId.find("_"));
    }
    else {
      string msg = "Could not find label PRODUCT_ID, invalid MRDR";
      throw IException(IException::Unknown, msg, _FILEINFO_);
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
        IString tablePath = (string)labelPvl.FindKeyword("MRO:WAVELENGTH_FILE_NAME");
        tablePath = tablePath.DownCase();
        FileName tableFile(inFile.path() + "/" + tablePath);
        //Check if the wavelength file exists
        if(tableFile.fileExists()) {
          TextFile *fin = new TextFile(tableFile.expanded());
          // Open table file
          if(!fin->OpenChk()) {
            string msg = "Cannot open wavelength table [" + tableFile.expanded() + "]";
            throw IException(IException::Io, msg, _FILEINFO_);
          }

          //For each line in the wavelength table, add the width to
          //The band bin group
          IString st;
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
          ocube->putGroup(bandBin);
        }
        //Otherwise throw an error
        else {
          string msg = "Cannot fine wavelength table [" + tableFile.expanded() + "]";
          throw IException(IException::Io, msg, _FILEINFO_);
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
      for(int i = 0; i < bandNames.Size(); i++) {
        origBand += (i + 1);
        bandName += bandNames[i];
      }
      bandBin.AddKeyword(origBand);
      bandBin.AddKeyword(bandName);
      ocube->putGroup(bandBin);
    }
    //Translate the Mapping group
    p.TranslatePdsProjection(outLabel);
    ocube->putGroup(outLabel.FindGroup("Mapping", Pvl::Traverse));
  }
  else if(prodType.Equal("TARGETED_RDR")) {
  }
  else if(prodType.Equal("DDR")) {
    PvlGroup bandBin = PvlGroup("BandBin");
    PvlKeyword origBand = PvlKeyword("OriginalBand");
    PvlKeyword bandName = PvlKeyword("BandName");
    PvlKeyword bandNames = labelPvl.FindObject("FILE").FindObject("IMAGE").FindKeyword("BAND_NAME");
    for(int i = 0; i < bandNames.Size(); i++) {
      origBand += (i + 1);
      bandName += bandNames[i];
    }
    bandBin.AddKeyword(origBand);
    bandBin.AddKeyword(bandName);
    ocube->putGroup(bandBin);
  }
  else {
    string msg = "Unsupported CRISM file type, supported types are: DDR, MRDR, and TRDR";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Translate the Instrument group
  FileName transFile("$mro/translations/crismInstrument.trn");
  PvlTranslationManager instrumentXlater(labelPvl, transFile.expanded());
  instrumentXlater.Auto(outLabel);

  // Translate the Archive group
  transFile  = "$mro/translations/crismArchive.trn";
  PvlTranslationManager archiveXlater(labelPvl, transFile.expanded());
  archiveXlater.Auto(outLabel);

  ocube->putGroup(outLabel.FindGroup("Instrument", Pvl::Traverse));
  ocube->putGroup(outLabel.FindGroup("Archive", Pvl::Traverse));

  p.StartProcess();
  p.EndProcess();

  return;
}
