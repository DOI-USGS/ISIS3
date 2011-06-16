#include "Isis.h"
#include "UserInterface.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"
#include "Pvl.h"
#include "ProcessExportMiniRFLroPds.h"
#include "PvlTranslationManager.h"
#include "OriginalLabel.h"
#include "SerialNumberList.h"
#include "Constants.h"
#include "Statistics.h"
#include "NaifStatus.h"
#include "Spice.h"
#include <fstream>
#include <iostream>
#include <cmath>

using namespace std;
using namespace Isis;

void FixLabel(Pvl &pcPdsLbl, bool &pbLevel2);
void GetSourceProductID(std::string psSrcListFile, std::string psSrcType, Pvl &pcPdsLabel);
void GetUserLabel(std::string psUserLbl, Pvl &pdsLabel, bool pbLevel2);

static unsigned int iCheckSum = 0;

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  Filename inFile = ui.GetFilename("FROM");

  // Set the processing object
  ProcessExportMiniRFLroPds cProcess;

  // Setup the input cube
  Cube *cInCube = cProcess.SetInputCube("FROM");
  Pvl *cInLabel =  cInCube->getLabel();

  // Get the output label file
  Filename outFile(ui.GetFilename("TO", "lbl"));
  string outFilename(outFile.Expanded());

  cProcess.SetDetached(true, outFilename);

  cProcess.SetExportType(ProcessExportPds::Fixed);

  //Set the resolution to  Kilometers
  cProcess.SetPdsResolution(ProcessExportPds::Kilometer);

  // 32bit
  cProcess.SetOutputType(Isis::Real);
  cProcess.SetOutputNull(Isis::NULL4);
  cProcess.SetOutputLrs(Isis::LOW_REPR_SAT4);
  cProcess.SetOutputLis(Isis::LOW_INSTR_SAT4);
  cProcess.SetOutputHrs(Isis::HIGH_REPR_SAT4);
  cProcess.SetOutputHis(Isis::HIGH_INSTR_SAT4);
  cProcess.SetOutputRange(-DBL_MAX, DBL_MAX);

  cProcess.SetOutputEndian(Isis::Msb);

  // Turn off Keywords
  cProcess.ForceScalingFactor(false);
  cProcess.ForceSampleBitMask(false);
  cProcess.ForceCoreNull(false);
  cProcess.ForceCoreLrs(false);
  cProcess.ForceCoreLis(false);
  cProcess.ForceCoreHrs(false);
  cProcess.ForceCoreHis(false);

  // Standard label Translation
  Pvl &pdsLabel = cProcess.StandardPdsLabel(ProcessExportPds::Image);

  // bLevel => Level 2 = True, Level 3 = False
  bool bLevel2 = cInCube->hasGroup("Instrument");

  // Translate the keywords from the original EDR PDS label that go in
  // this RDR PDS label for Level2 images only
  if(bLevel2) {
    OriginalLabel cOriginalBlob;
    cInCube->read(cOriginalBlob);
    Pvl cOrigLabel;
    PvlObject cOrigLabelObj = cOriginalBlob.ReturnLabels();
    cOrigLabelObj.SetName("OriginalLabelObject");
    cOrigLabel.AddObject(cOrigLabelObj);

    // Translates the ISIS labels along with the original EDR labels
    cOrigLabel.AddObject(*(cInCube->getLabel()));
    PvlTranslationManager cCubeLabel2(cOrigLabel, "$lro/translations/mrfExportOrigLabel.trn");
    cCubeLabel2.Auto(pdsLabel);


    if(cInLabel->FindObject("IsisCube").FindGroup("Instrument").HasKeyword("MissionName")) {
      PvlKeyword &cKeyMissionName = cInLabel->FindObject("IsisCube").FindGroup("Instrument").FindKeyword("MissionName");
      size_t sFound = cKeyMissionName[0].find("CHANDRAYAAN");
      if(sFound != string::npos) {
        cCubeLabel2 = PvlTranslationManager(cOrigLabel, "$lro/translations/mrfExportOrigLabelCH1.trn");
        cCubeLabel2.Auto(pdsLabel);
      }
      else {
        cCubeLabel2 = PvlTranslationManager(cOrigLabel, "$lro/translations/mrfExportOrigLabelLRO.trn");
        cCubeLabel2.Auto(pdsLabel);
      }
    }
  }
  else { //Level3 - add Band_Name keyword
    PvlGroup &cBandBinGrp = cInCube->getGroup("BandBin");
    PvlKeyword cKeyBandBin = PvlKeyword("BAND_NAME");
    PvlKeyword cKeyInBandBin;
    if(cBandBinGrp.HasKeyword("OriginalBand")) {
      cKeyInBandBin = cBandBinGrp.FindKeyword("OriginalBand");
    }
    else if(cBandBinGrp.HasKeyword("FilterName")) {
      cKeyInBandBin = cBandBinGrp.FindKeyword("FilterName");
    }
    for(int i = 0; i < cKeyInBandBin.Size(); i++) {
      cKeyBandBin += cKeyInBandBin[i];
    }
    PvlObject &cImageObject(pdsLabel.FindObject("IMAGE"));
    cImageObject += cKeyBandBin;
  }

  // Get the Sources Product ID if entered for Level2 only as per example
  if(ui.WasEntered("SRC") && bLevel2) {
    std::string sSrcFile = ui.GetFilename("SRC");
    std::string sSrcType = ui.GetString("TYPE");
    GetSourceProductID(sSrcFile, sSrcType, pdsLabel);
  }

  // Get the User defined Labels
  if(ui.WasEntered("USERLBL")) {
    std::string sUserLbl = ui.GetFilename("USERLBL");
    GetUserLabel(sUserLbl, pdsLabel, bLevel2);
  }

  // Calculate CheckSum
  Statistics *cStats =  cInCube->getStatistics();
  iCheckSum = (unsigned int)cStats->Sum();

  FixLabel(pdsLabel, bLevel2);

  // Add an output format template to the PDS PVL
  // Distinguish betweeen Level 2 and 3 images by calling the camera()
  // function as only non mosaic images(Level2) have a camera
  if(bLevel2) {
    pdsLabel.SetFormatTemplate("$lro/translations/mrfPdsLevel2.pft");
  }
  else {
    pdsLabel.SetFormatTemplate("$lro/translations/mrfPdsLevel3.pft");
  }

  size_t iFound = outFilename.find(".lbl");
  outFilename.replace(iFound, 4, ".img");
  ofstream oCube(outFilename.c_str());
  cProcess.OutputDetatchedLabel();
  //cProcess.OutputLabel(oCube);
  cProcess.StartProcess(oCube);
  oCube.close();
  cProcess.EndProcess();
}

/**
 * Reads the file with User input in PVL format and substitutes
 * non empty keyvalues with the existing values in the output
 * PVL.
 *
 * @author sprasad (11/10/2009)
 *
 * @param psUserLbl - User Label File
 * @param pcPdsLbl  - Output Pds PVL
 */
void GetUserLabel(std::string psUserLbl, Pvl &pcPdsLbl, bool pbLevel2) {
  Pvl cUsrPvl(psUserLbl);

  /*if (pbLevel2 &&
      (cUsrPvl.HasKeyword("SPACECRAFT_CLOCK_START_COUNT") ||
       cUsrPvl.HasKeyword("SPACECRAFT_CLOCK_STOP_COUNT")  ||
       cUsrPvl.HasKeyword("START_TIME") ||
       cUsrPvl.HasKeyword("STOP_TIME"))) {

       std::string msg = "Unsupported User defined keywords for Level2";
       throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
  }*/

  // Add all the keywords in the root
  for(int j = 0; j < cUsrPvl.Keywords(); j++) {
    if(pcPdsLbl.HasKeyword(cUsrPvl[j].Name())) {
      PvlKeyword &cKey = pcPdsLbl.FindKeyword(cUsrPvl[j].Name());
      cKey.Clear();
      cKey.SetValue(cUsrPvl[j][0]);
    }
    else
      pcPdsLbl.AddKeyword(cUsrPvl[j]);
  }

  // Add keywords in the objects
  for(int j = 0; j < cUsrPvl.Objects(); j++) {
    PvlObject cUsrObject = cUsrPvl.Object(j);
    if(pcPdsLbl.HasObject(cUsrObject.Name())) {
      PvlObject &cObject = pcPdsLbl.FindObject(cUsrObject.Name());
      for(int k = 0; k < cUsrObject.Keywords(); k++) {
        PvlKeyword cUsrKeyword = cUsrObject[k];
        if(cObject.HasKeyword(cUsrKeyword.Name())) {
          PvlKeyword &cKey = cObject.FindKeyword(cUsrKeyword.Name());
          cKey.Clear();
          cKey.SetValue(cUsrKeyword[0]);
        }
        else {
          cObject.AddKeyword(cUsrKeyword);
        }
      }
    }
  }
}

/**
 * Read the input file containing source id's for a mosaic and
 * write into the output PVL with  SOURCE_PRODUCT_ID keyname
 *
 * @author sprasad (11/10/2009)
 *
 * @param psSrcListFile - File containing source id's
 * @param pcPdsLbl      - Output PVL
 */
void GetSourceProductID(std::string psSrcListFile, std::string psSrcType, Pvl &pcPdsLbl) {
  PvlKeyword cKeySrcPrdId;

  if(pcPdsLbl.HasKeyword("SOURCE_PRODUCT_ID")) {
    pcPdsLbl.DeleteKeyword("SOURCE_PRODUCT_ID");
  }

  cKeySrcPrdId.SetName("SOURCE_PRODUCT_ID");

  // List File name
  if(psSrcType == "LIST") {
    SerialNumberList cSnl = psSrcListFile;
    for(int i = 0; i < cSnl.Size(); i++) {
      cKeySrcPrdId += cSnl.SerialNumber(i);
    }
  }
  // ID's - add directly to the PvlKeyword
  else {
    ifstream inIdFile;
    inIdFile.open(psSrcListFile.c_str(), std::ios::in);
    char buff[501];
    inIdFile.getline(buff, 500);
    while(!inIdFile.eof()) {
      if(buff[0] != '\n' && buff[0] != '\0') {
        cKeySrcPrdId += buff;
      }
      inIdFile.getline(buff, 500);
    }
    inIdFile.close();
  }
  pcPdsLbl += cKeySrcPrdId;
}

/**
 * Update, Add , Delete the labels in the output PVL to
 * match the required Mini RF PDS Label
 *
 * @author sprasad (11/10/2009)
 *
 * @param pcPdsLbl - Output PVL
 */
void FixLabel(Pvl &pcPdsLbl, bool &pbLevel2) {
  // Level 3
  if(!pbLevel2) {
    if(pcPdsLbl.HasKeyword("LINE_EXPOSURE_DURATION")) {
      pcPdsLbl.DeleteKeyword("LINE_EXPOSURE_DURATION");
    }
    if(pcPdsLbl.HasKeyword("ORBIT_NUMBER")) {
      pcPdsLbl.DeleteKeyword("ORBIT_NUMBER");
    }
    if(pcPdsLbl.HasKeyword("INCIDENCE_ANGLE")) {
      pcPdsLbl.DeleteKeyword("INCIDENCE_ANGLE");
    }
    if(pcPdsLbl.HasKeyword("INSTRUMENT_MODE_ID")) {
      pcPdsLbl.DeleteKeyword("INSTRUMENT_MODE_ID");
    }
    if(pcPdsLbl.HasKeyword("INSTRUMENT_MODE_DESC")) {
      pcPdsLbl.DeleteKeyword("INSTRUMENT_MODE_DESC");
    }
    if(pcPdsLbl.HasKeyword("LOOK_DIRECTION")) {
      pcPdsLbl.DeleteKeyword("LOOK_DIRECTION");
    }
  }

  // Additional keywords and update existing keywords
  if(pcPdsLbl.HasKeyword("LABEL_RECORDS")) {
    pcPdsLbl.DeleteKeyword("LABEL_RECORDS");
  }

  if(!pcPdsLbl.HasKeyword("PRODUCER_FULL_NAME")) {
    PvlKeyword cKeyPrdFullName = PvlKeyword("PRODUCER_FULL_NAME", "USGS AstroGeology Flagstaff");
    pcPdsLbl += cKeyPrdFullName;
  }

  if(!pcPdsLbl.HasKeyword("PRODUCER_INSTITUTION_NAME")) {
    PvlKeyword cKeyPrdInstName = PvlKeyword("PRODUCER_INSTITUTION_NAME", "USGS AstroGeology");
    pcPdsLbl += cKeyPrdInstName;
  }

  if(!pcPdsLbl.HasKeyword("MISSION_NAME")) {
    PvlKeyword cKeyMissionName = PvlKeyword("MISSION_NAME", "LUNAR RECONNAISSANCE ORBITER");
    pcPdsLbl += cKeyMissionName;
  }

  if(!pcPdsLbl.HasKeyword("PRODUCER_ID")) {
    PvlKeyword cKeyPrdID = PvlKeyword("PRODUCER_ID", "USGS");
    pcPdsLbl += cKeyPrdID;
  }

  time_t startTime = time(NULL);
  struct tm *tmbuf = gmtime(&startTime);
  char timestr[80];
  strftime(timestr, 80, "%Y-%m-%dT%H:%M:%S", tmbuf);
  PvlKeyword cKeyPrdCreationTime("PRODUCT_CREATION_TIME", timestr);
  pcPdsLbl += cKeyPrdCreationTime;

  PvlKeyword cKeySoftware("SOFTWARE_NAME", "ISIS3");
  pcPdsLbl += cKeySoftware;

  PvlKeyword cKeySoftwareVersion("SOFTWARE_VERSION_ID", Application::Version());
  pcPdsLbl += cKeySoftwareVersion;

  // Specific to IMAGE Object
  PvlObject &cImageObject(pcPdsLbl.FindObject("IMAGE"));
  if(cImageObject.HasKeyword("OFFSET")) {
    cImageObject.DeleteKeyword("OFFSET");
  }

  // Update SAMPLE_TYPE from "IEEE_" to "PC_"
  if(cImageObject.HasKeyword("SAMPLE_TYPE")) {
    PvlKeyword &cSampleType = cImageObject.FindKeyword("SAMPLE_TYPE");
    string sVal = cSampleType[0];
    int iFound = sVal.find('_');
    if(iFound >= 0) {
      sVal.replace(0, iFound, "PC");
      cSampleType.SetValue(sVal);
    }
  }

  if(pbLevel2) {
    char buff[62];
    sprintf(buff, "%d", iCheckSum);
    PvlKeyword cKeyChkSum("CHECKSUM", buff);
    cImageObject += cKeyChkSum;
  }

  // Projection object
  PvlObject &cProjectionObject(pcPdsLbl.FindObject("IMAGE_MAP_PROJECTION"));
  if(cProjectionObject.HasKeyword("PROJECTION_LATITUDE_TYPE")) {
    cProjectionObject.DeleteKeyword("PROJECTION_LATITUDE_TYPE");
  }

  if(!cProjectionObject.HasKeyword("COORDINATE_SYSTEM_TYPE")) {
    PvlKeyword cKeyCordSysType = PvlKeyword("COORDINATE_SYSTEM_TYPE", " ");
    cProjectionObject += cKeyCordSysType;
  }

  if(cProjectionObject.HasKeyword("MAP_PROJECTION_TYPE")) {
    PvlKeyword cKeyPrjType = cProjectionObject.FindKeyword("MAP_PROJECTION_TYPE");
    if(cKeyPrjType[0] == "OBLIQUE CYLINDRICAL") {
      PvlKeyword &cKeyCenLon = cProjectionObject.FindKeyword("CENTER_LONGITUDE");
      cKeyCenLon.SetValue("0.0 <DEG>");
      PvlKeyword &cKeyCenLat = cProjectionObject.FindKeyword("CENTER_LATITUDE");
      cKeyCenLat.SetValue("0.0 <DEG>");

      if(pbLevel2) {
        // Get the X,Y,Z values from Proj X Axis Vector
        PvlKeyword cKeyObXProj = cProjectionObject.FindKeyword("OBLIQUE_PROJ_X_AXIS_VECTOR");
        double x, y, z;
        x = iString(cKeyObXProj[0]).ToDouble();
        y = iString(cKeyObXProj[1]).ToDouble();
        z = iString(cKeyObXProj[2]).ToDouble();

        // Calculate Reference Latitude and Longitude
        // angle in degrees = angle in radians * 180 / Pi
        /*double dRad,dLon,dLat;
        reclat_c(xAxis,&dRad,&dLon,&dLat);
        dLat = dLat * 180.0 / Isis::PI;
        dLon = dLon * 180.0 / Isis::PI;
        */

        double dLon, dLat;
        dLon = atan(y / x) * (180 / Isis::PI);
        dLat = atan(z / (sqrt(pow(x, 2) + pow(y, 2)))) * (180 / Isis::PI);

        PvlKeyword cKeyRefLon("REFERENCE_LONGITUDE");
        iString iStr(dLon, 6);
        iStr += " <DEG>";
        cKeyRefLon.SetValue((std::string)iStr);
        cProjectionObject.AddKeyword(cKeyRefLon);

        PvlKeyword cKeyRefLat("REFERENCE_LATITUDE");
        iStr = iString(dLat, 6);
        iStr += " <DEG>";
        cKeyRefLat.SetValue(iStr);
        cProjectionObject.AddKeyword(cKeyRefLat);
      }
      else { // Level3 Projection object
        if(cProjectionObject.HasKeyword("OBLIQUE_PROJ_POLE_LATITUDE")) {
          cProjectionObject.DeleteKeyword("OBLIQUE_PROJ_POLE_LATITUDE");
        }
        if(cProjectionObject.HasKeyword("OBLIQUE_PROJ_POLE_LONGITUDE")) {
          cProjectionObject.DeleteKeyword("OBLIQUE_PROJ_POLE_LONGITUDE");
        }
        if(cProjectionObject.HasKeyword("OBLIQUE_PROJ_POLE_ROTATION")) {
          cProjectionObject.DeleteKeyword("OBLIQUE_PROJ_POLE_ROTATION");
        }
        if(cProjectionObject.HasKeyword("OBLIQUE_PROJ_X_AXIS_VECTOR")) {
          cProjectionObject.DeleteKeyword("OBLIQUE_PROJ_X_AXIS_VECTOR");
        }
        if(cProjectionObject.HasKeyword("OBLIQUE_PROJ_Y_AXIS_VECTOR")) {
          cProjectionObject.DeleteKeyword("OBLIQUE_PROJ_Y_AXIS_VECTOR");
        }
        if(cProjectionObject.HasKeyword("OBLIQUE_PROJ_Z_AXIS_VECTOR")) {
          cProjectionObject.DeleteKeyword("OBLIQUE_PROJ_Z_AXIS_VECTOR");
        }
      }
    }
  }

  PvlKeyword cKeyDataSetMapProj("^DATA_SET_MAP_PROJECTION", "DSMAP.CAT");
  cProjectionObject += cKeyDataSetMapProj;

  if(cProjectionObject.HasKeyword("LINE_PROJECTION_OFFSET")) {
    PvlKeyword &cLineProjOffset = cProjectionObject.FindKeyword("LINE_PROJECTION_OFFSET");
    string sVal = cLineProjOffset[0];
    int iFound = sVal.find('<');
    if(iFound >= 0) {
      string::iterator it;
      it = sVal.begin() + iFound;
      sVal.erase(it);
      cLineProjOffset.SetValue(sVal);
    }
  }

  if(cProjectionObject.HasKeyword("SAMPLE_PROJECTION_OFFSET")) {
    PvlKeyword &cSampleProjOffset = cProjectionObject.FindKeyword("SAMPLE_PROJECTION_OFFSET");
    string sVal = cSampleProjOffset[0];
    int iFound = sVal.find('<');
    if(iFound >= 0) {
      string::iterator it;
      it = sVal.begin() + iFound;
      sVal.erase(it);
      cSampleProjOffset.SetValue(sVal);
    }
  }

  if(cProjectionObject.HasKeyword("MAP_SCALE")) {
    PvlKeyword &cMapScale = cProjectionObject.FindKeyword("MAP_SCALE");
    string sVal = cMapScale[0];
    int iFound = sVal.find("EL");
    if(iFound >= 0) {
      sVal.erase(iFound, 2);
      cMapScale.SetValue(sVal);
    }
  }
}
