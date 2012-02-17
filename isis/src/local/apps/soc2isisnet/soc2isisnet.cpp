#include "Isis.h"
#include "iException.h"

#include "Camera.h"
#include "ControlNet.h"
#include "ControlPoint.h"
#include "ControlMeasure.h"
#include "Portal.h"
#include "Pvl.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <vector>
#include <QString>
#include "Latitude.h"
#include "Longitude.h"
#include "Distance.h"
#include "iTime.h"
#include "iString.h"
#include "SurfacePoint.h"


using namespace std;
using namespace Isis;

#define DEFAULT_SIGMA  9999.0
#define MIN_SIGMA      1e-7 

enum epointParameters { type, numMeasures, origVal_X, origVal_Y, origVal_Z, origSigma_X, origSigma_Y, origSigma_Z, resVal_X, resVal_Y, resVal_Z,
                     adjVal_X, adjVal_Y, adjVal_Z, adjSigma_X, adjSigma_Y, adjSigma_Z};
#define numPointParams  17

enum epointStats { totalPoints, pointsIgnored, pointsTie, pointsZControl, pointsXYControl, pointsXYZControl, pointsCheck, pointsFree, pointsFixed, pointsConstrained, 
  pointsEditLocked, pointMsrEditLocked, totalMeasures, totalValidMeasures};
#define numPointStats  14

enum epointType { Tie, Z_Control, XY_Control, XYZ_Control, CheckPoint };
#define numPointType  5

enum units { Filler, Meters, DecimalDegrees, DegMinSec };

typedef struct {
  map< string, vector<double> > pointParamsMap;
  vector<int> pointStats;
  vector<string> pointID;
}pointInfoStruct;

void ProcessControlNet(ControlNet & cnet, Pvl & logPvl, pointInfoStruct & pointInfo);
void LogControlNet(pointInfoStruct & pointInfo, Pvl & logPvl);

double ConvertDegMinSecToDeg(char * str); 
void SkipLines(int lines,fstream & fileStrm); 
void SkipWords(int,fstream&); //!skips a number of string items in a file stream

string GetGpfFilename(const Filename & inFile);
string GetPrjFilename(const Filename & inFile);
string GetTarget(const Filename & inputAtf);
void   GetTargetRadius(const string & targetName, double & equatorialRad, double & polarRad);

bool ParseProjectAndSetMapping(Pvl & mapPvl, PvlGroup & logGrp, int & unitsXY, const string & targetName, 
                               const string & prjFilename);
void ProcessX(vector<double> & pointParams, fstream & repstream, int unitsXY);
void ProcessY(vector<double> & pointParams, fstream & repstream, int unitsXY);
void ProcessZ(vector<double> & pointParams, fstream & repstream);

void ParseReport(const string & atfFilename, const int numPoints, map< string, vector<double> > & pointParamMap, 
                 vector<int> & pointStats, vector<string> & pointID, int unitsXY);
void ParseGpf   (const string & gpfFilename, const string & atfFilename, const string & targetName, ControlNet&, 
                 pointInfoStruct & pointInfo, Pvl & logPvl, Projection*, int unitsXY); 
void ParseIpfs  (const string & atfFilename, ControlNet& cnet, Pvl & logPvl); 
void ReadIpf    (const string & inputIpf, ControlNet & cnet, Pvl & logPvl);    

void IsisMain(){
  ControlNet cnet;
  
  Pvl logPvl;
  PvlGroup logGrp("Results");
  
  UserInterface &ui = Application::GetUserInterface();

  // Get the input and output file names
  string atfFilename   = ui.GetAsString("FROM");
  string netoutputfile = ui.GetFilename("TO");

  Filename atfFile(atfFilename);
  
  // Get the control net descriptors
  cnet.SetNetworkId  (ui.GetString("NETWORKID"));
  cnet.SetUserName   (ui.GetString("USERNAME"));
  cnet.SetDescription(ui.GetString("DESCRIPTION"));

  string gpfFilename = GetGpfFilename(atfFile);
  string prjFilename = GetPrjFilename(atfFile);

  // Gets the Control Net Target
  string target = GetTarget(atfFile);
  cnet.SetTarget(target);
  
  Pvl mapPvl;
  int unitsXY = Meters;
  bool isProjected = ParseProjectAndSetMapping(mapPvl, logGrp, unitsXY, target, prjFilename);
  Projection* proj  = NULL;
  if (isProjected) {
    proj = ProjectionFactory::Create(mapPvl);
  }
  
  // Init Point Stats vector
  pointInfoStruct pointInfo;
  pointInfo.pointStats.reserve(numPointStats);
  for (int i=0; i<numPointStats; i++) {
    pointInfo.pointStats[i] = 0;
  }
  
  ParseGpf(gpfFilename, atfFilename, target, cnet, pointInfo, logPvl, proj, unitsXY);
  
  ParseIpfs(atfFilename, cnet, logPvl);

  ProcessControlNet(cnet, logPvl, pointInfo);
  
  cnet.Write(netoutputfile);
  
  // Logging
  LogControlNet(pointInfo, logPvl);
  
  if (ui.WasEntered("LOG")) {
    string log = ui.GetAsString("LOG");
    logPvl.Write(log);
  }
  
  Application::Log(logPvl.FindGroup("OrigPointStatistics"));
  Application::Log(logPvl.FindGroup("IsisPointStatistics"));
}

/**
 * Log the original socetnet and output isis Control Net Statistics
 * 
 * @author Sharmila Prasad (11/14/2011)
 *  
 * @param pointInfo - structure with the Control Net Info 
 * @param logPvl    - Log Pvl
 */
void LogControlNet(pointInfoStruct & pointInfo, Pvl & logPvl){
  vector<int> & pointStats = pointInfo.pointStats;
  
  PvlGroup statsGrp("OrigPointStatistics");
  
  statsGrp += PvlKeyword("Tie",                pointStats[pointsTie]);
  statsGrp += PvlKeyword("ZControl",           pointStats[pointsZControl]);
  statsGrp += PvlKeyword("XYControl",          pointStats[pointsXYControl]);
  statsGrp += PvlKeyword("XYZControl",         pointStats[pointsXYZControl]);
  statsGrp += PvlKeyword("Check",              pointStats[pointsCheck]);
  logPvl += statsGrp;
  
  statsGrp.Clear();
  statsGrp = PvlGroup("IsisPointStatistics");
  statsGrp += PvlKeyword("TotalPoints",        pointStats[totalPoints]);
  statsGrp += PvlKeyword("Ignored",            pointStats[pointsIgnored]);
  statsGrp += PvlKeyword("Free",               pointStats[pointsFree]);
  statsGrp += PvlKeyword("Fixed",              pointStats[pointsFixed]);
  statsGrp += PvlKeyword("Constrained",        pointStats[pointsConstrained]);
  statsGrp += PvlKeyword("EditLocked",         pointStats[pointsEditLocked]);
  statsGrp += PvlKeyword("MeasuresEditLocked", pointStats[pointMsrEditLocked]);
  statsGrp += PvlKeyword("TotalMeasures",      pointStats[totalMeasures]);
  statsGrp += PvlKeyword("TotalValidMeasures", pointStats[totalValidMeasures]);
  
  logPvl += statsGrp;
}

/**
 * Process the newly created Control Net. 
 *  1. Removes Control Points with Measures less than 2 
 *  2. Add Prefix to the Control Point ID if specified 
 *  3. EditLock Control Points/Measures as specified by the User 
 *  4. Gather the new Control Net Statistics 
 * 
 * @author Sharmila Prasad (11/15/2011)
 * 
 * @param cnet      - Generated Control Net
 * @param logPvl    - Log Pvl
 * @param pointInfo - Structure with the Control Net Info 
 */
void ProcessControlNet(ControlNet & cnet, Pvl & logPvl, pointInfoStruct & pointInfo){

  UserInterface & ui = Application::GetUserInterface();
  string editLock = ui.GetString("EDITLOCK");
  
  string prefix   = ui.GetString  ("POINT_ID_PREFIX");
  
  bool prefixFlag = false;
  if (prefix != "None" && prefix.length()) {
    prefixFlag = true;
  }
  
  vector<int> & pointStats = pointInfo.pointStats;
  
  PvlGroup pointGrp;
  PvlObject resultObj("Results");
  
  int pointsIgnored = 0;
  int numPoints  = cnet.GetNumPoints();
  pointStats[totalPoints] = numPoints;
    
  for(int i=0; i<numPoints; i++) {
    ControlPoint * cPoint = cnet[i];
    
    int numMeasures = cPoint->GetNumMeasures();
    pointStats[totalMeasures] += numMeasures;
    
    int validMeasures = cPoint->GetNumValidMeasures();
    pointStats[totalValidMeasures] += validMeasures;
    
    // Set the prefix if specified
    string pointId = "";
    if (prefixFlag) {
      pointId = prefix;
    }
    pointId += cPoint->GetId();
    cPoint->SetId(pointId);
    
    pointGrp.Clear();
    pointGrp = PvlGroup(pointId);
    
    // Set the first measure as Reference by default
    cPoint->SetRefMeasure(0);
    
    int pointType = cPoint->GetType();
    
    // Check for measures less than 2
    if(numMeasures < 2 || validMeasures < 2 ){
      cPoint->SetIgnored(true);
      pointStats[pointsIgnored]++;
      pointGrp += PvlKeyword("Ignored", "Valid Measures less than 2");
    }
    
    // Log Point type
    if (pointType == ControlPoint::Free) {
      pointStats[pointsFree]++;
    }
    else if (pointType == ControlPoint::Constrained) {
       pointStats[pointsConstrained]++;
    }
    else if(pointType == ControlPoint::Fixed) {
       pointStats[pointsFixed]++;
    }
    
    // Edit Lock only if the point is unignored
    if (!cPoint->IsIgnored()) {
      if (editLock == "POINTS") {
        cPoint->SetEditLock(true);
        pointStats[pointsEditLocked]++;
        pointGrp += PvlKeyword("PointEditLocked", "True");
      }
      else if (editLock == "CONSTRAINED_FIXED") {
        if (pointType == ControlPoint::Fixed || pointType == ControlPoint::Constrained) {
          cPoint->SetEditLock(true);
          pointStats[pointsEditLocked]++;
          pointGrp += PvlKeyword("PointEditLocked", "True");
        }
      }
      else if (editLock == "MEASURES") {
        pointGrp += PvlKeyword("MeasuresEditLocked", "True");
        for (int j=0; j<numMeasures; j++) {
          // Edit Lock only non Ignored Measures
          if (!cPoint->GetMeasure(j)->IsIgnored()) {
            cPoint->GetMeasure(j)->SetEditLock(true);
            pointStats[pointMsrEditLocked]++;
          }
        }
      }
    }
    resultObj += pointGrp;
  }
  logPvl += resultObj;
}

/**
 * Parse the ipf file from the atf file and then Read the ipf file
 * 
 * @param atfFilename - Input atfFile
 * @param cnet        - Output Control Net
 */
void ParseIpfs(const string & atfFilename, ControlNet& cnet, Pvl & logPvl){
  fstream atfStream(atfFilename.c_str());
  string str="";
  
  Filename atfF(atfFilename);
  
  // Get the number of images from ATF File
  while(str != "NUM_IMGS"){
    atfStream >> str;
  }
  int numImages = 0;
  atfStream >> numImages;

  for (int j=0; j<numImages; j++){
    while(str != "IMAGE_IPF"){
      atfStream >> str;
    }
    string ipfFilename, temp;
    atfStream >> temp;
    ipfFilename = atfF.Path() + "/" + temp;

    SkipWords(3, atfStream);
    
    atfStream >> str;
    
    if (str == "1"){
      //!Reads ControlMeasure data from each .ipf file included in solution.
      ReadIpf(ipfFilename, cnet, logPvl);
    }
  }
  atfStream.close();
}

/**
 * This function parses Control Measures data from the image Pvl and ipf file 
 * From the Pvl, info such as the serialNum, Flip info, image dimensions are 
 * retrieved. 
 *  
 * The Control Measure info is retrieved from the ipf file. Info such as the Control 
 * Point ID, Measure Validity, Measure 
 *  
 * Info is in the Format: 
 * ********************************************************************* 
 *  IMAGE POINT FILE
 *  54
 *  pt_id,val,fid_val,no_obs,l.,s.,sig_l,sig_s,res_l,res_s,fid_x,fid_y
 *  M130551343LE_01 1 0 0
 * -26049.443359  -2026.991211
 *  0.000000  0.000000
 *  0.212442  0.000863
 *  0.000000  0.000000
 * *********************************************************************
 * 
 * @param inputIpf - Input IPF file
 * @param cnet     - Isis Control Net
 * @param logPvl   - Log Pvl
 */
void ReadIpf(const string & inputIpf, ControlNet& cnet, Pvl & logPvl){
  fstream ipfStream;
  ipfStream.open(inputIpf.c_str());

  PvlGroup logGrp(inputIpf);
  
  //! pvlFile is the name of the translation.pvl file associated with the current ipf
  string pvlFile = inputIpf;
  pvlFile = pvlFile.erase(pvlFile.size()-4,pvlFile.size());
  pvlFile.append("_translation.pvl");
  
  // Read Translation Pvl 
  Pvl ipfObj(pvlFile);
  PvlGroup ipfGrp = ipfObj.FindGroup("ISIS_SS_TRANSLATION");
  
  string flip      = ipfGrp.FindKeyword("Flip");
  string flipOrder = ipfGrp.FindKeyword("FlipPadOrderToSS");
  string pside     = ipfGrp.FindKeyword("PadSide");
  string serialNum = ipfGrp.FindKeyword("ISIS_SerialNumber");
  
  int padPixels  = ipfGrp.FindKeyword("NumberPadPixels");
  int imgLines   = ipfGrp.FindKeyword("SS_Lines");
  int imgSamples = ipfGrp.FindKeyword("SS_Samples");
 
  SkipLines(1, ipfStream);
  
  // Number of points this image is part of
  int pointsCount=0;
  ipfStream >> pointsCount;
  
  SkipLines(2, ipfStream);
  
  for (int i=0; i<pointsCount; i++){
    // Get the control Point ID
    string pointId="";
    ipfStream >> pointId;
    
    bool valid = true;   // Measure Validity
    int fid_val, no_obs; // Not used
    ipfStream >> valid >> fid_val >> no_obs;
    
    // Get Line and Sample
    double line=0, sample=0;
    ipfStream >> line >> sample;
    
    SkipLines(2,ipfStream);
    
    // Get SampleRes, LineRes
    double sampleRes=0, lineRes=0;
    ipfStream >> lineRes >> sampleRes;
    
    SkipLines(2,ipfStream);
    
    ControlPoint* cPoint;
    try{
      cPoint = cnet.GetPoint(QString(pointId.c_str()));
    }
    catch(iException e){
      PvlKeyword pkey(pointId, "Point not found");
      logGrp += pkey;
      continue;
    }
    
    ControlMeasure* cMeasure = new ControlMeasure;

    cMeasure->SetCubeSerialNumber(serialNum);
    cMeasure->SetChooserName("soc2isisnet conversion");
    
    // Set current time
    iTime time;
    cMeasure->SetDateTime(time.CurrentGMT());
    
    // Set the location based on the image flip
    // In this logic block the ISIS cmeasure sample/line values are calculated.
    if (flip=="N") {
      if ( (pside=="Right") || (pside=="None") ) {
        cMeasure->SetCoordinate(imgSamples/2.0 + sample + 1, imgLines/2.0 + line + 1); 
      }
      else{
        cMeasure->SetCoordinate(imgSamples/2.0 + sample - padPixels + 1, imgLines/2.0 + line + 1);
      }
    }
    else {
      if( (pside=="Right" && flipOrder=="FlipThenPad") || (pside=="Left" && flipOrder=="PadThenFlip") ){
        cMeasure->SetCoordinate((imgSamples - 2*padPixels)/2.0 - sample + 1, imgLines/2.0 + line + 1);
      }
      else{
        cMeasure->SetCoordinate(imgSamples/2.0 - sample + 1, imgLines/2.0 + line + 1); 
      }
    }
   
    cMeasure->SetResidual(sampleRes,lineRes);
    cMeasure->SetType(ControlMeasure::RegisteredSubPixel);
    
    // Invalid Control Measure in ipf
    if(!valid || (sampleRes == 0.0 && lineRes == 0.0)){
      cMeasure->SetIgnored(true);
    }

    cPoint->Add(cMeasure);
  }
  
  ipfStream.close();
}

/**
 * Parse the gpf file for the Control Point info such as Point ID, Validity, PointType 
 * 
 * This also calls the Report file to get info from GROUND POINT PARAMETERS such as:
 * 
 * Image ID, Numberof Images(Measures), Point Type, Original value, Original Sigma, Residual value, Adjusted Value and Adjusted Sigma for
 * X, Y, Z Parameters 
 *  
 * File Format: 
 * ************************************************************************** 
 * GROUND POINT FILE
 * 175
 * point_id,stat,known,lat_Y_North,long_X_East,ht,sig(3),res(3)
 * M130551343_2_S 1 1
 * -0.04777108953044        2.91270594391845         871.79999999999995      
 * 0.000000 0.000000 10.000000
 * 16.527759 -0.687783 3.704044 
 *  ************************************************************************** 
 * Hence this builds the base Control Point info using the information 
 *  
 * @param gpfFilename  - gpf file 
 * @param atfFilename  - atf file 
 * @param targetName   - target name 
 * @param cnet         - Isis Output Control Net 
 * @param pointInfo    - Point Information structure 
 * @param logPvl       - Pvl for log 
 * @param proj         - Projection if created or NULL
 * @param unitsXY      - Coordinate system for this socetset 
 *  
 */
void ParseGpf(const string & gpfFilename, const string & atfFileName, const string & targetName, 
              ControlNet & cnet, pointInfoStruct & pointInfo, Pvl & logPvl, Projection* proj, 
              int unitsXY) { 
  UserInterface &ui = Application::GetUserInterface();
  
  string sigma           = ui.GetString("SIGMAS");
  string measurementType = ui.GetString("MEASUREMENTS"); //use_points
  
  PvlGroup checkPointsGrp("CheckPoints");
  
  vector<int> & pointStats = pointInfo.pointStats;
  vector<string> & pointID = pointInfo.pointID;
  map< string, vector<double> > & pointParamsMap = pointInfo.pointParamsMap;
  
  bool isProjected = false;
  if (proj != NULL) {
    isProjected = true;
  }
  
  // Handle Ellipsoid target such as Mars the radius differently
  // by using the Aroid file to calculate the radius
  bool isEllipsoid = ui.GetBoolean("ELLIPSOID");
  Cube *refEllip = NULL;
  Camera *refEllipCam = NULL;
  if (isEllipsoid) {
    refEllip = new Cube;
    refEllip->open(ui.GetAsString("REFERENCE_FILE"));
    refEllipCam = refEllip->getCamera();
  }
  
  double userSigmaLat = DEFAULT_SIGMA;
  double userSigmaLon = DEFAULT_SIGMA;
  double userSigmaRad = DEFAULT_SIGMA;
  
  bool overRide    = false;
  iString sigmaPointType = "ALL";
  if(sigma == "OVERRIDE"){
    sigmaPointType = ui.GetString("POINT_TYPE");
    userSigmaLat = ui.GetDouble("SLAT");
    userSigmaLon = ui.GetDouble("SLON");
    userSigmaRad = ui.GetDouble("SRAD");
    overRide = true;
  }
  
  sigmaPointType.UpCase();
  
  // Read the Ground Point File (gpf)
  fstream gpfFile;
  gpfFile.open(gpfFilename.c_str());
  SkipLines(1, gpfFile);
  
  // Get the total Control Points
  int numPoints = 0;
  gpfFile >> numPoints;

  //cerr << "\natf file=" << atfFileName << "\ngpf file=" << gpfFilename << " points=" << numPoints;
  SkipLines(2, gpfFile);

  ParseReport(atfFileName, numPoints, pointParamsMap, pointStats, pointID, unitsXY);
  
  double EquatorialRadius=0, PolarRadius=0;
  GetTargetRadius(targetName, EquatorialRadius, PolarRadius);
  Distance polar_rad(PolarRadius, Isis::Distance::Meters);
  Distance equatorial_rad(EquatorialRadius, Isis::Distance::Meters);
  
  //cerr << "\nTarget=" << targetName << " EquatorialRadius=" << EquatorialRadius << " PolarRadius=" << PolarRadius;
  //cerr << "\nOverride=" << overRide << " sigmaPointType=" << sigmaPointType;
  
  for (int i=0; i<numPoints; i++) {
    string pname;
    int stat; // indicates ignore or not
    int knownPointType;
    double sigmalat, sigmalon, sigmarad;
    double latitude=0, longitude=0, radius=0;
    
    // Read pointId, stat, knownPointType, lat, lon, height, sigm information from Gpf
    // Lat Lon are the Apriori values in radians
    gpfFile >> pname >> stat >> knownPointType >> latitude >> longitude >> radius >> sigmalat >> sigmalon >> sigmarad;
    
    // Skip Residuals info line and blank line
    SkipLines(2, gpfFile);
    
    // Discard Check Points pointParamsMap[][type] = CheckPoint;
    if(knownPointType == CheckPoint){
      checkPointsGrp += PvlKeyword(pname, "CheckPoint Deleted");
      //cerr << "Deleting Checkpoint " << pname ;
      continue;
    }

    // Get all original(apriori) values from Report File
    // Ignore the lat(y), lon(x), height from the gpf file
    latitude  = pointParamsMap[pname][origVal_Y];
    longitude = pointParamsMap[pname][origVal_X];
    radius    = pointParamsMap[pname][origVal_Z];
    
    //cerr << "\npoint name=" << pname << " type=" << knownPointType << " isProjected=" << isProjected << " latitude=" << latitude << " longitude=" << longitude << " radius=" << radius << "EquatorialRadius=" << EquatorialRadius <<"\n";

    // Ellipsoid Targets - Always projected (Simple Cylinderical)
    // Reference Ellipsoids hold the radius as the dn value
    if (isEllipsoid) {
      refEllipCam->SetUniversalGround(latitude, longitude);
      double sample = refEllipCam->Sample();
      double line   = refEllipCam->Line();
      Portal iPortal (1, 1, refEllip->getPixelType());
      iPortal.SetPosition(sample, line, 1);
      refEllip->read(iPortal);
      radius += iPortal[0];
    }
    // Spherical Targets
    else {
      // No projection 
      if (!isProjected){
        radius += EquatorialRadius;
      }
      else{
        proj->SetCoordinate(longitude, latitude); 
        latitude  = proj->UniversalLatitude();
        longitude = proj->UniversalLongitude();
        radius   += proj->LocalRadius();
      }
    }
    
    //cerr << " latitude=" << latitude << " longitude=" << longitude << " radius=" << radius << "\n";
    
    Distance  radDist(radius,    Isis::Distance::Meters);
    Latitude  latObj (latitude,  Isis::Angle::Degrees);
    Longitude lonObj (longitude, Isis::Angle::Degrees);

    ControlPoint * cPoint = new ControlPoint;
    iString(pname).Trim("\n");
    iString(pname).Trim(" ");
    
    cPoint->SetId(pname);

    // default Point Type
    cPoint->SetType(Isis::ControlPoint::Constrained);
    
    if (knownPointType == Tie && measurementType == "APRIORI") {
      cPoint->SetType(Isis::ControlPoint::Free);
    }
    
    // Ignore unused Point
    if(stat == 0){
      cPoint->SetIgnored(true);
    }
    
    if (abs(sigmalat) < MIN_SIGMA or knownPointType == Tie){
      sigmalat = DEFAULT_SIGMA;
    }
    if (abs(sigmalon) < MIN_SIGMA or knownPointType == Tie){
      sigmalon = DEFAULT_SIGMA;
    } 
    if (abs(sigmarad) < MIN_SIGMA or knownPointType == Tie){
      sigmarad = DEFAULT_SIGMA;
    } 
    
    if(measurementType == "APRIORI"){
      Distance sigmaLatDist(DEFAULT_SIGMA, Isis::Distance::Meters);
      Distance sigmaLonDist(DEFAULT_SIGMA, Isis::Distance::Meters);
      Distance sigmaRadDist(DEFAULT_SIGMA, Isis::Distance::Meters);
      
      if(overRide && (sigmaPointType == "ALL" || (sigmaPointType == "FREE" && knownPointType == Tie) ) ) {
        sigmaLatDist.setMeters(userSigmaLat);
        sigmaLonDist.setMeters(userSigmaLon);
        sigmaRadDist.setMeters(userSigmaRad);
      }
      else {
        // Tie Points - use default sigmas
        if (knownPointType != Tie) {
        
          if(knownPointType == Z_Control) { // default XY apriori Z
            sigmaRadDist.setMeters(sigmarad > DEFAULT_SIGMA ? DEFAULT_SIGMA : sigmarad);
          } 
          else if(knownPointType == XY_Control) { // apriori XY default Z
            sigmaLatDist.setMeters(sigmalat > DEFAULT_SIGMA ? DEFAULT_SIGMA : sigmalat);
            sigmaLonDist.setMeters(sigmalon > DEFAULT_SIGMA ? DEFAULT_SIGMA : sigmalon);
          }
          else if(knownPointType == XYZ_Control) { // apriori XYZ
            sigmaLatDist.setMeters(sigmalat > DEFAULT_SIGMA ? DEFAULT_SIGMA : sigmalat);
            sigmaLonDist.setMeters(sigmalon > DEFAULT_SIGMA ? DEFAULT_SIGMA : sigmalon);
            sigmaRadDist.setMeters(sigmarad > DEFAULT_SIGMA ? DEFAULT_SIGMA : sigmarad);
          }
        }
      }

      SurfacePoint sp(latObj, lonObj, radDist);
      sp.SetRadii(equatorial_rad, equatorial_rad, polar_rad);
      sp.SetSphericalSigmasDistance(sigmaLatDist, sigmaLonDist, sigmaRadDist);
      cPoint->SetAprioriSurfacePoint(sp);
    }
    else if(measurementType == "ADJUSTED") {
      double adjLat = pointParamsMap[pname][adjVal_Y];
      double adjLon = pointParamsMap[pname][adjVal_X];
      double adjRad = pointParamsMap[pname][adjVal_Z];

      //cerr << "\nPointName=" << pname << " overRide=" << overRide << " adjRad=" << adjRad;
      if (isEllipsoid) {
        refEllipCam->SetUniversalGround(adjLat, adjLon);
        double sample = refEllipCam->Sample();
        double line   = refEllipCam->Line();
        Portal iPortal (1, 1, refEllip->getPixelType());
        iPortal.SetPosition(sample, line, 1);
        refEllip->read(iPortal);
        adjRad += iPortal[0];
      }
      // Spherical Targets
      else {
        if(!isProjected){
          adjRad += EquatorialRadius;
        }
        else {
          proj->SetCoordinate(adjLon, adjLat);
          adjLat = proj->UniversalLatitude();
          adjLon = proj->UniversalLongitude();
          adjRad = proj->LocalRadius() + adjRad;
          //cerr << " localRadius=" <<  proj->LocalRadius();
        }
      }
      
      double adjSigmaLat = pointParamsMap[pname][adjSigma_Y];
      double adjSigmaLon = pointParamsMap[pname][adjSigma_X];
      double adjSigmaRad = pointParamsMap[pname][adjSigma_Z];

      Latitude  adjustedLat(adjLat, Isis::Angle::Degrees);
      Longitude adjustedLon(adjLon, Isis::Angle::Degrees);
      Distance  adjustedRad(adjRad, Isis::Distance::Meters);
      
      if(overRide && (sigmaPointType == "ALL" || (sigmaPointType == "FREE" && knownPointType == Tie) ) ) {
        adjSigmaLat = userSigmaLat;
        adjSigmaLon = userSigmaLon;
        adjSigmaRad = userSigmaRad;
      }

      Distance sigmaLatDist(adjSigmaLat, Isis::Distance::Meters);
      Distance sigmaLonDist(adjSigmaLon, Isis::Distance::Meters);
      Distance sigmaRadDist(adjSigmaRad, Isis::Distance::Meters);

      SurfacePoint sp(adjustedLat, adjustedLon, adjustedRad);
      sp.SetRadii(equatorial_rad, equatorial_rad, polar_rad);
      sp.SetSphericalSigmasDistance(sigmaLatDist, sigmaLonDist, sigmaRadDist);
      //cerr << "\nadjLat=" << adjLat << " adjLat=" << adjLat << " adjRad=" << adjRad;
      //cerr << "\nadjRad=" << adjRad << " adjSigmaRad=" << pointParamsMap[pname][adjSigma_Z] << " sigmaRadDist=" << sigmaRadDist.meters() << "\n";
      cPoint->SetAprioriSurfacePoint(sp);
    } // end Adjusted
    
    cnet.AddPoint(cPoint);
  }
  
  logPvl += checkPointsGrp;
  
  gpfFile.close();
}

/**
 * Read X Value(Original, Sigma, Adjusted, Adjusted Sigma, Residuals from 
 * the report file. The Original and Adjusted Values are based on the XY Units
 *  
 * XY Units: 
 * 1 - Meters 
 * 2 - Decimal Degrees 
 * 3 - Deg, Hr, Secs 
 *  
 * @author Sharmila Prasad (11/12/2011)
 * 
 * @param pointParams - structure to hold the X Values
 * @param repstream   - Report file stream
 * @param unitsXY     - Coordinate system
 */
void ProcessX(vector<double> & pointParams, fstream & repstream, int unitsXY){
  string temp;
  
  // Get Original Value X
  repstream >> temp;
  if (unitsXY == DegMinSec) {
    pointParams[origVal_X] = ConvertDegMinSecToDeg((char*)temp.c_str());
  }
  else { // in Decimal Degrees / Meters
    pointParams[origVal_X] = iString(temp).ToDouble();
  }
  
  // Get Original Sigma X
  repstream >> pointParams[origSigma_X];
  
  // Get Residual Value X
  repstream >> pointParams[resVal_X];
  
  // Get Adjusted Value X
  repstream >> temp;
  if (unitsXY == DegMinSec) {
    pointParams[adjVal_X] = ConvertDegMinSecToDeg((char*)temp.c_str());
  }
  else { // in Decimal Degrees / Meters
    pointParams[adjVal_X] = iString(temp).ToDouble();
  }
  
  // Get Adjusted Sigma X
  repstream >> pointParams[adjSigma_X];

  // Skip "y" on next line
  SkipWords(1, repstream);
}

/**
 * Read Y Value(Original, Sigma, Adjusted, Adjusted Sigma, Residuals from 
 * the report file. The Original and Adjusted Values are based on the XY Units 
 *  
 * XY Units: 
 * 1 - Meters 
 * 2 - Decimal Degrees 
 * 3 - Deg, Hr, Secs 
 *  
 * @author Sharmila Prasad (11/12/2011)
 * 
 * @param pointParams - structure to hold the Y Values
 * @param repstream   - Report file stream
 * @param unitsXY     - Coordinate system 
 */
void ProcessY(vector<double> & pointParams, fstream & repstream, int unitsXY) {
  string temp;
  
  // Get Original Value Y
  repstream >> temp;
  if (unitsXY == DegMinSec) {
    pointParams[origVal_Y] = ConvertDegMinSecToDeg((char*)temp.c_str());
  }
  else { // in Decimal Degrees / Meters
    pointParams[origVal_Y] = iString(temp).ToDouble();
  }

  // Get Original Sigma Y
  repstream >> pointParams[origSigma_Y];
    
  // Get Residual Value Y
  repstream >> pointParams[resVal_Y];
    
  // Get Adjusted Value Y
  repstream >> temp;
  if (unitsXY == DegMinSec) {
    pointParams[adjVal_Y] = ConvertDegMinSecToDeg((char*)temp.c_str());
  }
  else { // in Decimal Degrees / Meters
    pointParams[adjVal_Y] = iString(temp).ToDouble();
  }
    
  // Get Adjusted Sigma Y
  repstream >> pointParams[adjSigma_Y];
  
  // Skip "Z" on next line
  SkipWords(1,repstream);
}

/**
 * Read Z Value(Original, Sigma, Adjusted, Adjusted Sigma, Residuals from 
 * the report file.
 * 
 * @author Sharmila Prasad (11/12/2011)
 * 
 * @param pointParams - structure to hold the Z Values
 * @param repstream   - Report file stream
 */
void ProcessZ(vector<double> & pointParams, fstream & repstream) {
  
  // Get Original Value Z
  repstream >> pointParams[origVal_Z];

  // Get Original Sigma Z
  repstream >> pointParams[origSigma_Z];
    
  // Get Residual Value Z
  repstream >> pointParams[resVal_Z];
    
  // Get Adjusted Value Z
  repstream >> pointParams[adjVal_Z];
  
  // Get Adjusted Sigma Z
  repstream >> pointParams[adjSigma_Z];
}

/**
 * This function parses the report file for Point ID, Number of Measures, 
 * Original, Sigma, Adjusted, Adjusted Sigma, Residuals for X, Y, Z coordinates. 
 * 
 * @author Sharmila Prasad (11/16/2011)
 * 
 * @param atfFilename   - Atf file from which the report file is derived
 * @param numPoints     - Number of Control Points
 * @param pointParamMap - Structure to read Control Point Parameters
 * @param pointStats    - Structure to update Point Stats
 * @param pointID       - Structure to store Point Id's
 * @param unitsXY       - XY units 
 */
void ParseReport(const string & atfFilename, const int numPoints, map< string, vector<double> > & pointParamMap, vector<int> & pointStats, vector<string> & pointID, int unitsXY){

  // Extract the report file name from the ATF file 
  string repFilename = atfFilename;
  size_t found = atfFilename.find(".atf");
  repFilename.replace(found, 4, ".rep");
  
  fstream repstream;
  repstream.open(repFilename.c_str());
  
  // Checks whether the Report file exists
  if(!repstream.is_open()){
    string msg = "Report file not found.\n";
    throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_); 
  } 
  
  string searchStr;
  int i=0;
  while(repstream.good() && i < 2){
    if(searchStr == "ID,"){
      i++;
    }
    repstream >> searchStr;
  }

  // skip 23 words
  SkipWords(23,repstream);
  
  i=0;
  string pointId="";
  string temp1, temp2;
  iString temp="";
  vector <double> pointParams(numPointParams, 0);
  
  //! This loop parses the "GROUND POINT PARAMETERS" section of a .rep file
  do {
    // Get the Point ID(mentions Image ID in the report)
    repstream >> pointId;
    
    // Get the number of Images/Measures
    repstream >> temp;
    pointParams[numMeasures] = iString(temp).ToInteger();

    char ch = '\0';
    do {
      repstream.get(ch);
    } while (ch != '(');
    
    // Parse the Point Type
    temp ="";
    size_t found = string::npos;
    do {
      repstream >> temp1;
      
      if(temp1 == "RMS"){
        break;
      }
      
      found = temp1.find(')');
      if (found != string::npos) {
        temp1.replace(found, 1, "");
      }
      temp += temp1;
      temp += " ";
    } while (found == string::npos);
   
    // Get Point Type
    temp.ConvertWhiteSpace();
    string blank=" ";
    temp.Trim(blank);

    if (temp == "Tie") {
      pointStats[pointsTie]++;
      pointParams[type] = Tie;
    }
    else if (temp == "Z Cntrl") {
      pointStats[pointsZControl]++;
      pointParams[type] = Z_Control;
    }
    else if (temp == "XY Cntrl") {
      pointStats[pointsXYControl]++;
      pointParams[type] = XY_Control;
    }
    else if (temp == "XYZ Cntrl") {
      pointStats[pointsXYZControl]++;
      pointParams[type] = XYZ_Control;
    }
    else { // Check Point not used in  Isis
      pointStats[pointsCheck]++;
      pointParams[type] = CheckPoint;
    }

    ProcessX(pointParams, repstream, unitsXY);
    
    ProcessY(pointParams, repstream, unitsXY);
    
    ProcessZ(pointParams, repstream);
    
    pointParamMap[pointId] = pointParams;
    
    //cerr << "\npointId=" << pointId << " origVal_Z=" << pointParams[origVal_Z] << " origSigma_Z=" << pointParams[origSigma_Z] << " resVal_Z=" << pointParams[resVal_Z] 
    // << " adjVal_Z=" << pointParams[adjVal_Z] << " adjSigma_Z=" << pointParams[adjSigma_Z] << "\n";

    i++;

  } while (i < numPoints);
}

/**
 * Parse the project file for units, Coordinate System, Projection type. 
 * Depending on the Coordinate system, Projection Map pvl is created. 
 *  
 * Possible Projections PolarStereographic, Sinusoidal 
 *  
 * Coordinate systems: 1 - OGraphic (No Projection)  
 * 
 * @author sprasad (11/16/2011)
 * 
 * @param mapPvl      - Generated Projection map pvl
 * @param logGrp      - Pvlgroup for log info
 * @param unitsXY     - XY units
 * @param targetName  - Target Name
 * @param prjFilename - Project File name
 * 
 * @return bool - Whether there is a projection
 */
bool ParseProjectAndSetMapping(Pvl & mapPvl, PvlGroup & logGrp, int & unitsXY, const string & targetName, const string & prjFilename){

  fstream prjstream;
  prjstream.open(prjFilename.c_str());
  
  // No project file
  if (!prjstream.is_open()) {
    string msg = "WARNING: .prj file not found. Coordinates are assumed to be in Lat/Lon/Rad. "
                 "To use projected coordinates you must include the .prj file\n";
    PvlKeyword msgKey("Mapping", msg);
    logGrp += msgKey;
    return false;
  }
  
  string fileStr="";
  string coord_system="";
  string polar_aspect="";
  string grid_name="";
  string projType="";

  //Default values
  iString ProjectionName     = "PolarStereographic";
  iString CenterLongitude    = "0.0";
  iString CenterLatitude     = "90.0";
  iString LatitudeType       = "Planetographic";
  iString LongitudeDirection = "PositiveEast";
  iString LongitudeDomain    = "180";
  iString MinimumLatitude    = "-90.0";
  iString MaximumLatitude    = "90.0";
  iString MinimumLongitude   = "-180.0";
  iString MaximumLongitude   = "180.0";
  iString PixelResolution    = "100.0";
  
  while(prjstream.good()){
    prjstream >> fileStr;
    if (fileStr == "XY_UNITS") {
      prjstream >> unitsXY;
    }
    else if(fileStr == "COORD_SYS"){
      prjstream >> coord_system;
    }
    else if(fileStr == "PROJECTION_TYPE"){
      prjstream >> projType;
    }
    else if(fileStr == "POLAR_ASPECT"){
      prjstream >> polar_aspect;
    }
    else if(fileStr == "GRID_NAME"){
      prjstream >> grid_name;
    }
    else if (fileStr =="CENTER_LONGITUDE") { // for PolarStereographic Projection
      prjstream >> CenterLongitude;
    }
    else if (fileStr == "CENTRAL_MERIDIAN") { // for Sinusoidal Projection
      prjstream >> CenterLongitude; 
    }
  }
  prjstream.close();
  
  // OGraphic Coordiante System, no Projection
  if(coord_system == "1"){
    string msg = "OGraphic Coordinates, No Projection";
    PvlKeyword msgKey("Mapping", msg);
    logGrp += msgKey;
    return false;
  }
  
  if(projType == "POLAR_STEREOGRAPHIC_PROJECTION"){
    if(polar_aspect == "S"){
      CenterLatitude = "-90.0";
    }
    else{
      CenterLatitude = "90.0";
    }
  }
  else if (projType == "SINUSOIDAL_PROJECTION") {
    ProjectionName = "Sinusoidal";
  }
  
  double EquatorialRadius=0, PolarRadius=0;
  GetTargetRadius(targetName, EquatorialRadius, PolarRadius);
  
  // Create Projection Map 
  PvlGroup mapGrp("Mapping");
  mapGrp += PvlKeyword("TargetName",        targetName);
  mapGrp += PvlKeyword("EquatorialRaduis",  EquatorialRadius,"meters");
  mapGrp += PvlKeyword("PolarRadius",       PolarRadius,     "meters");
  mapGrp += PvlKeyword("LatitudeType",      LatitudeType);
  mapGrp += PvlKeyword("LongitudeDirection",LongitudeDirection);
  mapGrp += PvlKeyword("LongitudeDomain",   LongitudeDomain);
  mapGrp += PvlKeyword("ProjectionName",    ProjectionName);
  mapGrp += PvlKeyword("CenterLongitude",   CenterLongitude);
  mapGrp += PvlKeyword("CenterLatitude",    CenterLatitude);
  mapGrp += PvlKeyword("MinimumLatitude",   MinimumLatitude);
  mapGrp += PvlKeyword("MaximumLatitude",   MaximumLatitude);
  mapGrp += PvlKeyword("MinimumLongitude",  MinimumLongitude);
  mapGrp += PvlKeyword("MaximumLongitude",  MaximumLongitude);
  mapGrp += PvlKeyword("PixelResolution",   PixelResolution, "meters/pixel");

  logGrp = mapGrp;
  mapPvl += mapGrp;
  
  //cerr << mapPvl;
  
  return true;
}

/**
 * Get the target info from the one of the image translation PVL file. 
 * The name of the file is parsed from the "atf" file 
 * 
 * @param inputatf - Atf File
 */
string GetTarget(const Filename & inputAtf) {
  fstream atfFile;
  atfFile.open(inputAtf.absoluteFilePath().toStdString().c_str());

  string imgFile, temp;
  while (atfFile.good()){
    atfFile >> temp;
    if (temp.find("IMAGE_IPF") != string::npos) {
      break;
    }
  }
  atfFile >> temp;
  atfFile.close();

  temp.erase(temp.size()-4, temp.size());
  temp.append("_translation.pvl");
  
  imgFile = inputAtf.Path() + "/" + temp;

  string target="";
  try {
    Pvl imgPvl(imgFile);
    PvlGroup imgGrp = imgPvl.FindGroup("ISIS_SS_TRANSLATION");
    target = imgGrp.FindKeyword("Target")[0];
  } 
  catch (Isis::iException e) {
    string msg = "Target Not Found. Check if the Image Translation File \"";
    msg += temp;
    msg += "\" Exist";
    throw Isis::iException::Message(Isis::iException::User, msg, _FILEINFO_);
  }
  
  return target;
}

/**
 * Get the Equatorial and Polar radius of the Target
 * 
 * @author Sharmila Prasad (11/13/2011)
 * 
 * @param targetName    - Target Name
 * @param equatorialRad - Equalatorial Radius
 * @param polarRad      - Polar Radius
 */
void GetTargetRadius(const string & targetName, double & equatorialRad, double & polarRad){
  PvlGroup pvlgrp = Projection::TargetRadii(targetName);
  equatorialRad   = pvlgrp.FindKeyword("EquatorialRadius");
  polarRad        = pvlgrp.FindKeyword("PolarRadius");
}

/**
 * Get the gpf filename by parsing the atffile. 
 * 
 * @author Sharmila Prasad (11/9/2011)
 * 
 * @param inFile - atf file
 * 
 * @return string - gpf file
 */
string GetGpfFilename(const Filename & inFile){
  fstream atfFile(inFile.absoluteFilePath().toStdString().c_str());
  string ans;
  
  while (atfFile.good()){
    atfFile >> ans;
    if (ans.find("GP_FILE") != string::npos) {
      break;
    }
  }
  atfFile >> ans;
  atfFile.close();

  return (inFile.Path() + "/" + ans);
}

/**
 * Get the project file by parsing the atf file
 * 
 * @author Sharmila Prasad (11/9/2011)
 * 
 * @param inFile - atf file
 * 
 * @return string - project file name
 */
string GetPrjFilename(const Filename & inFile){
  fstream atfFile(inFile.absoluteFilePath().toStdString().c_str());
  string ans;

  while (atfFile.good()){
    atfFile >> ans;
    if (ans.find("PROJECT") != string::npos) {
      break;
    }
  }
  atfFile >> ans;

  ans.erase(0,8);
  atfFile.close();
  return (inFile.Path() + "/" + ans);
}

/**
 * Convert Str with Deg:Min:Secs format to decimal degrees
 * 
 * @author Sharmila Prasad (11/12/2011)
 * 
 * @param str in Deg:Min:Secs format
 * 
 * @return double - Decimal Degree
 */
double ConvertDegMinSecToDeg(char * str){
  double result=0.0, temp = 0.0;
  char *cptrStart=str, *cptrEnd=NULL;
  bool isNegative = false;

  // Get the Degrees
  temp = strtod(cptrStart, &cptrEnd);
  if(temp < 0) {
    isNegative = true;
    temp *= -1;
  }
  result = temp;
  
  //cerr << "\nConvertDegMinSecToDeg str=" << str << " deg=" << temp;

  // Get the Minutes
  cptrStart=cptrEnd;
  temp = strtod (cptrStart+1, &cptrEnd);
  //cerr << " mins=" << temp;
  temp /= 60.0;
  result += temp;
  
  // Get the Seconds
  temp = strtod (cptrEnd+1, NULL);
  //cerr << " secs=" << temp;
  temp /= 3600.0;
  result += temp;
  
  
  if(isNegative) {
    result *= -1;
  } 
  
  return result;
}

/**
 * Skips the specified number of lines from file stream
 * 
 * @param lines    - number of lines to skip
 * @param fileStrm - File stream
 */
void SkipLines(int lines,fstream & fileStrm){
  char buffer[256];
  
  for (int i=0; i<lines; i++){
    fileStrm.getline(buffer, 256);
  }
}

/**
 * Skip n number of words from a file stream
 * 
 * @param n    - number of words to skip
 * @param fstr - file stream
 */ 
void SkipWords(int n,fstream & fstr){
  string str;
  for(int i=0;i<n;i++){
    fstr >> str;
  }
}
