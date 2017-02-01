#include "Isis.h"

#include <QString>
#include <QDomDocument>

#include "Cube.h"
#include "CubeAttribute.h"
#include "FileName.h"
#include "LineManager.h"
#include "OriginalXmlLabel.h"
#include "Preference.h"
#include "ProcessImport.h"
#include "UserInterface.h"
#include "XmlTranslationManager.h"
#include "iTime.h"

using namespace std;
using namespace Isis;

vector<Cube *> g_outputCubes;

int g_frameletLines; 
//vector <std::pair<int,int> > g_frameletLines;
//vector <int> g_frameletLines;
//int getCubeNumber(int line);
//void separateFrames(Buffer &in);
void translateCoreInfo(FileName &inputLabel, ProcessImport &importer);
void translateLabels(FileName &inputLabel, Cube *outputCube);

void IsisMain() {
// The commented-out code in this app is for processing multi-framelet images. The current test data
// is not multi-framelet, but there may be multi-framelet input data in the future. 

# if 0
//  g_outputCubes.clear();
//  g_frameletLines.clear();

  // Hard-coded defaults. Can also grab from XML header 
  // under PEHK_HEADER WindowN_(END|BEGIN)_ROW
  //
  // EVEN (cube 0): even lines
  //  ODD (cube 1): odd lines
  // 
  // As number of lines:
  //g_frameletLines.push_back(280); // PAN
  //g_frameletLines.push_back(256); // RED
  //g_frameletLines.push_back(256); // NIR
  //g_frameletLines.push_back(256); // BLU

  // As std::pair<begin, end>
// g_frameletLines.push_back(std::pair<int,int>(354,632)); // PAN
// g_frameletLines.push_back(std::pair<int,int>(712,966)); // RED
// g_frameletLines.push_back(std::pair<int,int>(1048,1302)); // NIR
// g_frameletLines.push_back(std::pair<int,int>(1409,1662)); // BLU

  //g_frameletLines = 50; // Fake framelet size for testing even/odd
# endif

  UserInterface &ui = Application::GetUserInterface();
  FileName xmlFileName = ui.GetFileName("FROM");

  ProcessImport importer;
  translateCoreInfo(xmlFileName, importer);

  importer.SetInputFile(xmlFileName.removeExtension().addExtension("dat").expanded());
  Cube *outputCube = importer.SetOutputCube("TO");

  translateLabels(xmlFileName, outputCube);
  
  FileName outputCubeFileName(ui.GetFileName("TO"));

  OriginalXmlLabel xmlLabel;
  xmlLabel.readFromXmlFile(xmlFileName);

#if 0
    // Set up even and odd cubes
    Cube *even = new Cube();
    Cube *odd = new Cube();
    even->setDimensions(importer.Samples(), importer.Lines(), importer.Bands());
    even->setPixelType(importer.PixelType());
    even->setByteOrder(importer.ByteOrder());

    odd->setDimensions(importer.Samples(), importer.Lines(), importer.Bands());
    odd->setPixelType(importer.PixelType());
    odd->setByteOrder(importer.ByteOrder());



    QString evenFile = outputCubeFileName.path() + "/" + outputCubeFileName.baseName() + ".even.cub";
    QString oddFile = outputCubeFileName.path() + "/" + outputCubeFileName.baseName() + ".odd.cub";

    even->create(evenFile);
    odd->create(oddFile);

    g_outputCubes.push_back(odd);
    g_outputCubes.push_back(even);

  importer.StartProcess(separateFrames);

  // Translate labels to every image and close output cubes before calling EndProcess

  std::vector<QString> framelets;
  framelets.push_back("Odd");
  framelets.push_back("Even");

  for(unsigned int i = 0; i < g_outputCubes.size(); i++) {
    translateLabels(xmlFileName, g_outputCubes[i]);

    // Add Framelet information to label
    g_outputCubes[i]->label()->findObject("IsisCube").findGroup("Instrument").addKeyword(
        PvlKeyword("Framelets", framelets[i]));

    // TODO: Add any original label information?

    importer.WriteHistory(*g_outputCubes[i]);
    delete g_outputCubes[i];
  }
  g_outputCubes.clear();
# endif

  importer.StartProcess();

  // Write out original label before closing the cube
  outputCube->write(xmlLabel);

  importer.EndProcess();

  // Remove now-unneeded "TO" output file if even/odd functionality is added back in
//  QFile::remove(outputCubeFileName.expanded());

  return;
}


#if 0
/**
 * Get output cube number for a given input line number
 *
 * @param line input line number (starting at 1)
 *
 * @return @b int Returns cube number that line belongs in 
 *  
 * @internal
 *   @history 2017-01-20 Kristin Berry - Original Version
 *  
 */
int getCubeNumber(int line) {
  return line/g_frameletLines % 2; 
}


/**
 * Separates the framelets into even and odd cubes. 
 * 
 * @author 2017-01-20 Kristin Berry
 * 
 * @param in Input Buffer
 */
void separateFrames(Buffer &in) {
  int outputCube = getCubeNumber(in.Line());

  LineManager mgr(*g_outputCubes[outputCube]); 
  mgr.SetLine(in.Line(), in.Band());

  for(int i = 0; i < mgr.size(); i++){
       mgr[i] = in[i];
  }

  g_outputCubes[outputCube]->write(mgr);

  // Null out every other cube
  for(int i = 0; i < (int)g_outputCubes.size(); i++) {
    if(i == outputCube) continue;

    LineManager mgr(*g_outputCubes[i]);
    mgr.SetLine(in.Line(), in.Band());

    for(int j = 0; j < mgr.size(); j++) {
      mgr[j] = Isis::Null;
    }
    g_outputCubes[i]->write(mgr);
  }
}
#endif 


/**
 * Translate core info from labels and set ProcessImport object with 
 * these values.
 *
 * @param inputLabel Reference to the xml label file name from the input image.
 * @param importer Reference to the ProcessImport object to which core info will
 *                 be set.
 *  
 * @internal
 *   @history 2017-01-20 Jeannie Backer - Original Version
 */
void translateCoreInfo(FileName &inputLabel, ProcessImport &importer) {
  // Get the directory where the Tgo translation tables are
  PvlGroup &dataDir = Preference::Preferences().findGroup("DataDirectory");
  QString missionDir = (QString) dataDir["Tgo"];
  FileName transFile(missionDir + "/translations/tgoCassis.trn");

  // Get the translation manager ready
  XmlTranslationManager labelXlater(inputLabel, transFile.expanded());

  QString str;
  // Set up the ProcessImport
  str = labelXlater.Translate("CoreSamples");
  int ns = toInt(str);
  str = labelXlater.Translate("CoreLines");
  int nl = toInt(str);
  str = labelXlater.Translate("CoreBands");
  int nb = toInt(str);
  importer.SetDimensions(nl, ns, nb); // nl/ns swapped: switched here for correct output.

  str = labelXlater.Translate("CoreType");    
  importer.SetPixelType(PixelTypeEnumeration(str));

  str = labelXlater.Translate("CoreByteOrder");    
  importer.SetByteOrder(ByteOrderEnumeration(str));

  importer.SetFileHeaderBytes(0);

  str = labelXlater.Translate("CoreBase");
  importer.SetBase(toDouble(str));
  str = labelXlater.Translate("CoreMultiplier");
  importer.SetMultiplier(toDouble(str));
}


/**
 * Translate instrument, bandbin, and archive info from xml label into ISIS3 
 * label and add kernels group. 
 *
 * @param inputLabel Reference to the xml label file name for the input image.
 * @param outputCube Pointer to output cube where ISIS3 labels will be added and 
 *                   updated.
 *  
 * @internal
 *   @history 2017-01-20 Jeannie Backer - Original Version
 *   @history 2017-01-23 Kristin Berry - Added support for bandBin group and archive group
 */
void translateLabels(FileName &inputLabel, Cube *outputCube) {
  // Get the directory where the Tgo translation tables are
  PvlGroup &dataDir = Preference::Preferences().findGroup("DataDirectory");
  QString missionDir = (QString) dataDir["Tgo"];
  FileName transFile(missionDir + "/translations/tgoCassisInstrument.trn");

  // Get the translation manager ready for translating the instrument label
  XmlTranslationManager labelXlater(inputLabel, transFile.expanded());

  // Pvl output label
  Pvl *outputLabel = outputCube->label();
  labelXlater.Auto(*(outputLabel));

  // Add needed keywords that are not in the translation table
  PvlGroup &inst = outputLabel->findGroup("Instrument", Pvl::Traverse);

  // Add units of measurement to keywords from translation table
  inst.findKeyword("ExposureDuration").setUnits("seconds");

  // Translate BandBin group
  FileName bandBinTransFile(missionDir + "/translations/tgoCassisBandBin.trn");
  XmlTranslationManager bandBinXlater(inputLabel, bandBinTransFile.expanded());

  // Pvl output label
  outputLabel = outputCube->label();
  bandBinXlater.Auto(*(outputLabel));

  PvlGroup &bandBin = outputLabel->findGroup("BandBin", Pvl::Traverse);
  bandBin.findKeyword("Center").setUnits("nm");
  bandBin.findKeyword("Width").setUnits("nm");

  // Create the Archive Group
  FileName archiveTransFile(missionDir + "/translations/tgoCassisArchive.trn");
  XmlTranslationManager archiveXlater(inputLabel, archiveTransFile.expanded());

  // Pvl output label
  outputLabel = outputCube->label();
  archiveXlater.Auto(*(outputLabel));

  // Create YearDoy keyword in Archive group
  iTime stime(outputLabel->findGroup("Instrument", Pvl::Traverse)["StartTime"][0]);

  PvlGroup &archive = outputLabel->findGroup("Archive", Pvl::Traverse);
                                                  
  PvlKeyword yeardoy("YearDoy", toString(stime.Year()*1000 + stime.DayOfYear()));
  archive.addKeyword(yeardoy);
  archive.findKeyword("PredictMaximumExposureTime").setUnits("ms");
  archive.findKeyword("CassisOffNadirAngle").setUnits("deg");
  archive.findKeyword("PredictedRepetitionFrequency").setUnits("ms");
  archive.findKeyword("GroundTrackVelocity").setUnits("km/s");
  archive.findKeyword("ForwardRotationAngle").setUnits("deg");
  archive.findKeyword("SpiceMisalignment").setUnits("deg");
  archive.findKeyword("FocalLength").setUnits("m");
  archive.findKeyword("ImageFrequency").setUnits("ms");
  archive.findKeyword("ExposureTimePEHK").setUnits("ms");

  // Setup the kernel group
  PvlGroup kern("Kernels");
  QString spacecraftNumber;
  QString instId  = (QString) inst.findKeyword("InstrumentId");
  QString spcName = (QString) inst.findKeyword("SpacecraftId");
  QString filter  = (QString) bandBin.findKeyword("FilterName");

  if(spcName.compare("TGO", Qt::CaseInsensitive) == 0
     && instId.compare("CaSSIS", Qt::CaseInsensitive) == 0) {

    int spacecraftCode = -143400;

    kern += PvlKeyword("NaifFrameCode", toString(spacecraftCode));

    if (filter.compare("PAN", Qt::CaseInsensitive) == 0) {
      spacecraftCode = -143421;
    }
    if (filter.compare("RED", Qt::CaseInsensitive) == 0) {
      spacecraftCode = -143422;
    }
    if (filter.compare("NIR", Qt::CaseInsensitive) == 0) {
      spacecraftCode = -143423;
    }
    if (filter.compare("BLU", Qt::CaseInsensitive) == 0) {
      spacecraftCode = -143424;
    }
    else {
      // ??? throw error or use default??? (for now, use default)
    }

    // Add Kernel to BandBin Group
    bandBin.addKeyword(PvlKeyword("NaifFrameCode", toString(spacecraftCode)));
  }
  else {
    QString msg = "Unrecognized Spacecraft name [" 
      + spcName
      + "] and instrument ID ["
      + instId
      + "]";
      throw IException(IException::User, msg, _FILEINFO_);
  }
  outputCube->putGroup(kern);
}

