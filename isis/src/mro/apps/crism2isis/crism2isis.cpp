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

  QString prodType;

  if(labelPvl.HasKeyword("PRODUCT_TYPE")) {
    prodType = (QString)labelPvl.FindKeyword("PRODUCT_TYPE");
  }
  else {
    QString msg = "Unsupported CRISM file type, supported types are: DDR, MRDR, and TRDR";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if(prodType.toUpper() == "MAP_PROJECTED_MULTISPECTRAL_RDR") {
    QString prodId;
    if(labelPvl.HasKeyword("PRODUCT_ID")) {
      prodId = (QString)labelPvl.FindKeyword("PRODUCT_ID");
      prodId = prodId.mid(prodId.indexOf("_") + 1, prodId.indexOf("_"));
    }
    else {
      QString msg = "Could not find label PRODUCT_ID, invalid MRDR";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    //If the product type is AL (Lambert albedo) or IF (I/F)
    //Read the wavelength table and put the corresponding
    //widths in the band bin group
    if(prodId.toUpper() == "MRRAL" || prodId.toUpper() == "MRRIF") {
      //If the wavelength file is specified in the label
      if(labelPvl.HasKeyword("MRO:WAVELENGTH_FILE_NAME")) {
        PvlGroup bandBin = PvlGroup("BandBin");
        PvlKeyword origBand = PvlKeyword("OriginalBand");
        PvlKeyword widths = PvlKeyword("Width");
        QString tablePath = (QString)labelPvl.FindKeyword("MRO:WAVELENGTH_FILE_NAME");
        tablePath = tablePath.toLower();
        FileName tableFile(inFile.path() + "/" + tablePath);
        //Check if the wavelength file exists
        if(tableFile.fileExists()) {
          TextFile *fin = new TextFile(tableFile.expanded());
          // Open table file
          if(!fin->OpenChk()) {
            QString msg = "Cannot open wavelength table [" + tableFile.expanded() + "]";
            throw IException(IException::Io, msg, _FILEINFO_);
          }

          //For each line in the wavelength table, add the width to
          //The band bin group
          QString st;
          int band = 1;
          while(fin->GetLine(st)) {
            st = st.simplified().trimmed();
            QStringList cols = st.split(",");

            origBand += toString(band);
            widths += cols[2];
            band++;
          }
          delete fin;

          bandBin.AddKeyword(origBand);
          bandBin.AddKeyword(widths);
          ocube->putGroup(bandBin);
        }
        //Otherwise throw an error
        else {
          QString msg = "Cannot fine wavelength table [" + tableFile.expanded() + "]";
          throw IException(IException::Io, msg, _FILEINFO_);
        }
      }
    }
    //If the product type is DE (Derived products for I/F) or DL
    //(Derived products for Lambert albedo) write the band names
    //to the band bin group
    else if(prodId.toUpper() == "MRRDE" || prodId.toUpper() == "MRRDL") {
      PvlGroup bandBin = PvlGroup("BandBin");
      PvlKeyword origBand = PvlKeyword("OriginalBand");
      PvlKeyword bandName = PvlKeyword("BandName");
      PvlKeyword bandNames = labelPvl.FindObject("IMAGE").FindKeyword("BAND_NAME");
      for(int i = 0; i < bandNames.Size(); i++) {
        origBand += toString(i + 1);
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
  else if(prodType.toUpper() == "TARGETED_RDR") {
  }
  else if(prodType.toUpper() == "DDR") {
    PvlGroup bandBin = PvlGroup("BandBin");
    PvlKeyword origBand = PvlKeyword("OriginalBand");
    PvlKeyword bandName = PvlKeyword("BandName");
    PvlKeyword bandNames = labelPvl.FindObject("FILE").FindObject("IMAGE").FindKeyword("BAND_NAME");
    for(int i = 0; i < bandNames.Size(); i++) {
      origBand += toString(i + 1);
      bandName += bandNames[i];
    }
    bandBin.AddKeyword(origBand);
    bandBin.AddKeyword(bandName);
    ocube->putGroup(bandBin);
  }
  else {
    QString msg = "Unsupported CRISM file type, supported types are: DDR, MRDR, and TRDR";
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
