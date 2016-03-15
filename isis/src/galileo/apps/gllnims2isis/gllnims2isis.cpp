#include "Isis.h"

//ISIS libraries

#include "BoxcarCachingAlgorithm.h"
#include "Brick.h"
#include "CubeAttribute.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "OriginalLabel.h"
#include "Message.h"
#include "ProcessImportPds.h"
#include "ProcessByLine.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlTranslationManager.h"

#include "Table.h"
#include "UserInterface.h"


//STD libraries

#include <cstdio>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>


//QT libraries
#include <QString>
#include <QBuffer>
#include <QRegExp>
#include <QByteArray>
#include <QBuffer>
#include <QTextStream>
#include <QFileInfo>
#include <QFile>


using namespace std;
using namespace Isis;
enum CubeType {CORE,SUFFIX};


void importQubs(QString coreParamName,QString suffixParamName);
void ProcessBands(Pvl &pdsLab, Cube *nimsCube, ProcessImportPds &importPds);
void translateNIMSLabels(Pvl &pdsLab, Cube *ocube, FileName inFile, CubeType ctype);
QByteArray pvlFix(QString fileName);


Cube *g_oCube;
Brick *g_oBuff;
Cube *g_coreCube;
Brick *g_coreBuff;
Cube *g_suffixCube;
Brick *g_suffixBuff;
PvlGroup g_results("Results");
Table *g_utcTable;

int g_coreBands;
int g_suffixBands;
int g_totalBands;
int g_coreItemBytes;
int g_suffixItemBytes;
Isis::ByteOrder g_byteOrder;
Isis::PixelType g_corePixelType,g_suffixPixelType;




void IsisMain() {

 if(Isis::IsLsb())
     g_byteOrder = Isis::Lsb;

 else
     g_byteOrder = Isis::Msb;


  g_coreBands = 0;
  g_suffixBands = 0;
  g_totalBands = 0;

  g_coreItemBytes = 4;
  g_suffixItemBytes = 4;

  g_corePixelType = Isis::None;
  g_suffixPixelType = Isis::None;

  importQubs("CORE","SUFFIX");


  return;
}



/**
 * @brief   Main function called by IsisMain which takes an input NIMS cube and separates
 *          it into an ISIS3 cube that contains core mission data, and a suffix ISIS3 cube
 *          which contains backplane data gathered from the spectrometers and other instruments.
 * @param   coreParamName The XML doc parameter where the user-specified core cube filename is
 *          entered via the user interface.
 * @param   suffixParamName The XML doc parameter where the user-specificed suffix cube filename
 *          is entered via the user interface.
 * @return  None
 *
 */


void importQubs(QString coreParamName, QString suffixParamName) {

  ProcessImportPds::PdsFileType fileType = ProcessImportPds::Qube;

  // We should be processing a PDS file
  UserInterface &ui = Application::GetUserInterface();
  if (!ui.WasEntered(coreParamName) || !ui.WasEntered(suffixParamName)) {
    return;
  }

  g_oCube = NULL;
  g_oBuff = NULL;

  ProcessImportPds importPds;

  importPds.Progress()->SetText((QString)"Writing " + coreParamName + " file");


  FileName inFile = ui.GetFileName("FROM");
  QFileInfo fi(inFile.expanded());

  //cout << "Processing:  " << inFile.expanded() << endl;

  //Fix the broken XML tags in the pvl file
  QByteArray pvlData= pvlFix(inFile.expanded());
  QTextStream pvlTextStream(&pvlData);
  istringstream pvlStream(pvlTextStream.readAll().toStdString());
  Pvl *pdsLabel = new Pvl();

  try{
  pvlStream >> *pdsLabel;
  }
  catch(IException &e) {
    QString msg = "Input file [" + inFile.expanded() +
                 "] is not a valid PVL file.";

    throw IException(e, IException::User, msg, _FILEINFO_);
  }


  // Create holder for original label
  OriginalLabel origLabel(*pdsLabel);
  //pdsLabel->write(fi.baseName()+".pvl");


  //QFileInfo inputFileInfo(inFile.expanded());
  //pdsLabel->write(inputFileInfo.baseName()+".pvl");

  const PvlObject &qube = pdsLabel->findObject("Qube");


  QString dataSetId(qube["DATA_SET_ID"]);


  //Verify that we have a NIMS cube

  QRegExp galileoRx("GO-[A-Z]-NIMS*");
  galileoRx.setPatternSyntax(QRegExp::Wildcard);

  try {
    if (!galileoRx.exactMatch(dataSetId) )
    {
      QString msg = "Invalid DATA_SET_ID [" + dataSetId + "]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }


  }
  catch(IException &e) {
    QString msg = "Unable to read [DATA_SET_ID] from input file [" +
                 inFile.expanded() + "]";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  //Determine the dimensions and pixel type of the core/suffix bands

  QString g_coreItemBytesStr(qube["CORE_ITEM_BYTES"][0]);
  QString g_suffixItemBytesStr(qube["SUFFIX_BYTES"][0]);

  QString backPlanesStr(qube["SUFFIX_ITEMS"][2]);
  QString corePlanesStr(qube["CORE_ITEMS"][2]);

  g_coreItemBytes = g_coreItemBytesStr.toInt();
  g_suffixItemBytes = g_suffixItemBytesStr.toInt();
  g_suffixBands = backPlanesStr.toInt();
  g_coreBands = corePlanesStr.toInt();

  g_totalBands = g_suffixBands+g_coreBands;

  PvlKeyword coreKey = qube.findKeyword("CORE_ITEM_TYPE");


  // Set PixelType for core and suffix bands

  if (coreKey[0] == "VAX_REAL")
      g_corePixelType = Isis::Real;
  else if (coreKey[0] == "VAX_INTEGER")
      g_corePixelType = Isis::SignedWord;


  PvlKeyword suffixKey;

  if (qube.hasKeyword("BAND_SUFFIX_ITEM_TYPE") ){
          suffixKey = qube.findKeyword("BAND_SUFFIX_ITEM_TYPE");
          if (suffixKey[0] == "VAX_REAL")
             g_suffixPixelType = Isis::Real;
          else if (suffixKey[0] == "VAX_INTEGER")
             g_suffixPixelType = Isis::SignedWord;
  }


  // Convert the pds file to a cube
  try {
      importPds.SetPdsFile(*pdsLabel,inFile.expanded(),fileType);
  }
  catch(IException &e) {
    QString msg = "Input file [" + inFile.expanded() +
                 "] does not appear to be a Galileo NIMS detached PDS label";
    throw IException(e, IException::User, msg, _FILEINFO_);
  }

  importPds.SetDimensions(importPds.Samples(),importPds.Lines(),g_coreBands);
  importPds.SetPixelType(g_corePixelType);

  Isis::CubeAttributeOutput coreatt;
  coreatt = CubeAttributeOutput("+REAL");

  g_coreCube = importPds.SetOutputCube("CORE");

  g_coreCube->addCachingAlgorithm(new BoxcarCachingAlgorithm());


  importPds.StartProcess();
  translateNIMSLabels(*pdsLabel,g_coreCube,inFile,CORE);
  importPds.EndProcess();

  importPds.Progress()->SetText((QString)"Writing " + suffixParamName + " file");
  importPds.SetDimensions(importPds.Samples(),importPds.Lines(),g_suffixBands);
  importPds.SetPixelType(g_suffixPixelType);

  if(g_suffixPixelType==Isis::Real)
      importPds.SetVAXConvert(true);


  importPds.ClearOutputCubes();
  g_suffixCube = importPds.SetOutputCube("SUFFIX");



  importPds.SetSuffixOffset(importPds.Samples(),importPds.Lines(),g_coreBands,g_coreItemBytes);
  g_suffixCube->addCachingAlgorithm(new BoxcarCachingAlgorithm());


  importPds.StartProcess(); 
  translateNIMSLabels(*pdsLabel,g_suffixCube,inFile,SUFFIX);
  importPds.EndProcess();

}


/**
 * @brief   Fixes the broken XML tags in the Pvl file.  The Pvl file is loaded into memory,
 *          and after the fixes are appled, the corrected XML code is returned as
 *          a QByteArray.
 * @param   fileName The full path to the NIMS input cube
 * @return  A QByteArray containing XML code which has been fixed.  This is fed to
 *          a QTextStream which is then fed into a Pvl object.
 *
 */


QByteArray pvlFix(QString fileName){


  QByteArray null;
  QFile pvlFile;

  pvlFile.setFileName(fileName);

  if ( !pvlFile.open(QFile::ReadOnly|QIODevice::Text))
    return null;


  //Read the Pvl file into a byte array
  QByteArray fileData = pvlFile.readAll();
  QByteArray pvlData;

  QString pvlEnd("QUBE\nEND");
  int ix = fileData.lastIndexOf(pvlEnd);

  pvlData = fileData.left(ix+pvlEnd.size());

  //Is this one of the messed up files?
  if (pvlData.contains(QByteArray("*/\"") ) ){

  pvlData.replace("*/\""," */");

  }

  if (pvlData.contains(QByteArray("//") ) ){

  pvlData.replace("//","  ");

  }

  return pvlData;


}



/**
 * @brief Processes the Pvl label for tube/mosaic NIMS cubes
 *
 * @param   pdsLab The Pvl label of the NIMS cube
 * @param   ocube A pointer to the output cube.
 * @param   importPds The ProcessImportPds object which is needed to set the
 *          base/multiplier of the suffix bands.
 * @return  None
 *
 */


void translateNIMSLabels(Pvl &pdsLab, Cube *ocube,FileName inFile,CubeType ctype){


  Pvl archiveLabel;
  Pvl instrumentLabel;
  Pvl bandBinLabel;


  Pvl pdsLabel(pdsLab);
  //QFileInfo fi(inFile.expanded());

  PvlObject qube(pdsLab.findObject("Qube"));



  // Directory containing translation tables


  PvlGroup dataDir(Preference::Preferences().findGroup("DataDirectory"));
  QString transDir = (QString) dataDir["galileo"] + "/translations/";


  QString instrument="galileoNIMSInstrument.trn";
  QString archive = "galileoNIMSArchive.trn";
  QString coreBandBin = "galileoNIMSCoreBandBin.trn";
  QString suffixBandBin = "galileoNIMSSuffixBandBin.trn";


  FileName coreBandBinFile(transDir+coreBandBin);
  FileName suffixBandBinFile(transDir+suffixBandBin);

  FileName instrumentFile(transDir+instrument);
  FileName archiveFile(transDir+archive);

  PvlTranslationManager archiveXlator(pdsLabel, archiveFile.expanded());
  PvlTranslationManager instrumentXlator(pdsLabel, instrumentFile.expanded());
  PvlTranslationManager coreBandBinXlator(pdsLabel,coreBandBinFile.expanded());
  PvlTranslationManager suffixBandBinXlator(pdsLabel,suffixBandBinFile.expanded());



  archiveXlator.Auto(archiveLabel);

  if (ctype==CORE) {

    coreBandBinXlator.Auto(bandBinLabel);

  }
  else {

    suffixBandBinXlator.Auto(bandBinLabel);

  }

  instrumentXlator.Auto(instrumentLabel);


  ocube->putGroup(archiveLabel.findGroup("Archive",Pvl::Traverse));
  ocube->putGroup(instrumentLabel.findGroup("Instrument",Pvl::Traverse));
  ocube->putGroup(bandBinLabel.findGroup("BandBin",Pvl::Traverse));



}



/**
 * @brief Processes the Pvl bands for tube/mosaic NIMS cubes
 *
 * @param   pdsLab The Pvl label of the NIMS cube
 * @param   nimsCube A pointer to the output cube containing backplane data
 * @param   importPds The ProcessImportPds object which is needed to set the
 *          base/multiplier of the suffix bands.
 * @return  None
 *
 */


void ProcessBands(Pvl &pdsLab, Cube *nimsCube, ProcessImportPds &importPds) {

//Create the BandBin Group


  PvlObject qube(pdsLab.findObject("Qube"));
  PvlGroup bandBin("BandBin");



  PvlKeyword suffixNames("BandSuffixName");
  PvlKeyword suffixUnits("BandSuffixUnit");
  PvlKeyword suffixCenters("Center");
  PvlKeyword suffixDetectors("Detector");
  PvlKeyword suffixGratingPositions("GratingPosition");
  PvlKeyword suffixOriginalBands("OriginalBand");
  PvlKeyword suffixSolarFluxes("SolarFlux");
  PvlKeyword suffixSensitivities("Sensitivity");


  QString baseStr,multStr;
  vector<double> multi(g_suffixBands);
  vector<double> base(g_suffixBands);


  for(int i = 0; i < g_suffixBands; i++) {
    suffixNames+= (QString)qube["BAND_SUFFIX_NAME"][i];

    if(qube.hasKeyword("BAND_SUFFIX_UNIT"))
        suffixUnits+= (QString)qube["BAND_SUFFIX_UNIT"][i];

    if (qube.hasKeyword("BAND_BIN_CENTER") )
        suffixCenters += (QString)qube["BAND_BIN_CENTER"][i];

    if (qube.hasKeyword("BAND_BIN_ORIGINAL_BAND") )
        suffixOriginalBands += (QString)qube["BAND_BIN_ORIGINAL_BAND"][i];

    if (qube.hasKeyword("BAND_BIN_GRATING_POSITION") )
        suffixGratingPositions += (QString)qube["BAND_BIN_GRATING_POSITION"][i];


    if (qube.hasKeyword("BAND_BIN_DETECTOR") )
        suffixDetectors += (QString)qube["BAND_BIN_DETECTOR"][i];

    if (qube.hasKeyword("BAND_BIN_SOLAR_FLUX") )
        suffixSolarFluxes += (QString)qube["BAND_BIN_SOLAR_FLUX"][i];

    if (qube.hasKeyword("BAND_BIN_SENSITIVITY") )
        suffixSensitivities += (QString)qube["BAND_BIN_SENSITIVITY"][i];


  }



  bandBin += suffixNames;
  bandBin += suffixUnits;
  bandBin += suffixCenters;
  bandBin += suffixDetectors;
  bandBin += suffixGratingPositions;
  bandBin += suffixOriginalBands;
  bandBin += suffixSensitivities;
  bandBin += suffixSolarFluxes;

   if(qube.hasKeyword("BAND_SUFFIX_NOTE"))
     bandBin += (QString)qube["BAND_SUFFIX_NOTE"];

   if(qube.hasKeyword("STD_DEV_SELECTED_BAND_NUMBER"))
     bandBin += (QString)qube["STD_DEV_SELECTED_BAND_NUMBER"];


  for(int i = 0; i < g_suffixBands; i++) {

    multStr = (QString)qube["BAND_SUFFIX_MULTIPLIER"];
    baseStr = (QString)qube["BAND_SUFFIX_BASE"];
    multi[i]=multStr.toDouble();
    base[i] = baseStr.toDouble();

    }

  importPds.SetMultiplier(multi);
  importPds.SetBase(base);
  nimsCube->putGroup(bandBin);

}

