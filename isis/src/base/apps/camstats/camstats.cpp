#include "Isis.h"
#include "Statistics.h"
#include "Camera.h"
#include "UserInterface.h"
#include "Cube.h"
#include "Progress.h"
#include "Process.h"

#include "iString.h"

using namespace std;
using namespace Isis;

//function to build stats data
void buildStats (Camera *cam, int &sample, int &line);
void writeFlat (ofstream &os, Statistics &s);

/**  Produces NULL values for special pixels
 * 
 * @param keyname  Name of keyword to generate
 * @param value    Value to write to keyword
 * @param unit     Optional units for keywords
 * 
 * @return PvlKeyword Newly created keyword
 */
static inline PvlKeyword ValidateKey(const std::string keyname, const double 
 &value, const std::string &unit = "") { 
  if (IsSpecial(value)) {
    return (PvlKeyword(keyname, "NULL"));
  }
  else {
    return (PvlKeyword(keyname,value,unit));
  }
}


//global stats opjects that will be used by function 
Statistics latStat;
Statistics lonStat;
Statistics resStat;
Statistics sampleResStat;
Statistics lineResStat;
Statistics aspectRatioStat;
Statistics phaseStat;
Statistics emissionStat;
Statistics incidenceStat;
Statistics localSolarTimeStat;
Statistics localRaduisStat;
Statistics northAzimuthStat;

void IsisMain(){

  Process p;

  // Reset all the stats objects because they are global
  latStat.Reset();
  lonStat.Reset();
  resStat.Reset();
  sampleResStat.Reset();
  lineResStat.Reset();
  aspectRatioStat.Reset();
  phaseStat.Reset();
  emissionStat.Reset();
  incidenceStat.Reset();
  localSolarTimeStat.Reset();
  localRaduisStat.Reset();
  northAzimuthStat.Reset();

  UserInterface &ui = Application::GetUserInterface();

  Cube *icube = p.SetInputCube("FROM");
  Camera *cam = icube->Camera();

//  Cube cube;
//  cube.Open(ui.GetFilename("FROM"));
//  Camera *cam = cube.Camera();

  int eband = cam->Bands();
  // if the camera is band independent that only run one band
  if (cam->IsBandIndependent()) eband = 1;
  int linc = ui.GetInteger("LINC");
  int sinc = ui.GetInteger("SINC");

  int pTotal = eband * ((cam->Lines()-2) / linc + 2) ;
  Progress progress;
  progress.SetMaximumSteps(pTotal);
  progress.CheckStatus();

  for (int band=1; band<=eband; band++) {
    cam->SetBand(band);
    for (int line=1; line<(int)cam->Lines(); line=line+linc) {
      for (int sample=1; sample< cam->Samples(); sample=sample+sinc) {
        buildStats(cam, sample, line);
      }
      //set the sample value to the last sample and run buildstats
      int sample = cam->Samples();
      buildStats(cam, sample, line);
      progress.CheckStatus();
    }
    //set the line value to the last line and run on all samples(sample + sinc)
    int line = cam->Lines();
    for (int sample=1; sample< cam->Samples(); sample=sample+sinc) {
      buildStats(cam, sample, line);
    }
    //set last sample and run with last line
    int sample = cam->Samples();
    buildStats(cam, sample, line);
    progress.CheckStatus();
  }

  //Set up the Pvl groups and get min, max, avg, and sd for each statstics object  
  PvlGroup pUser("User Parameters");
  pUser += PvlKeyword("Filename",ui.GetFilename("FROM"));
  pUser += PvlKeyword("Linc",ui.GetInteger("LINC"));
  pUser += PvlKeyword("Sinc",ui.GetInteger("SINC"));

  PvlGroup pLat("Latitude");
  pLat += ValidateKey("LatitudeMinimum",latStat.Minimum());
  pLat += ValidateKey("LatitudeMaximum",latStat.Maximum());
  pLat += ValidateKey("LatitudeAverage",latStat.Average());
  pLat += ValidateKey("LatitudeStandardDeviation",latStat.StandardDeviation());

  PvlGroup pLon("Longitude");
  pLon += ValidateKey("LongitudeMinimum",lonStat.Minimum());
  pLon += ValidateKey("LongitudeMaximum",lonStat.Maximum());
  pLon += ValidateKey("LongitudeAverage",lonStat.Average());
  pLon += ValidateKey("LongitudeStandardDeviation",lonStat.StandardDeviation());

  PvlGroup pSampleRes("SampleResolution");
  pSampleRes += ValidateKey("SampleResolutionMinimum",sampleResStat.Minimum(),
                           "meters/pixel");
  pSampleRes += ValidateKey("SampleResolutionMaximum",sampleResStat.Maximum(),
                           "meters/pixel");
  pSampleRes += ValidateKey("SampleResolutionAverage",sampleResStat.Average(),
                           "meters/pixel");
  pSampleRes += ValidateKey("SampleResolutionStandardDeviation",
                           sampleResStat.StandardDeviation(),"meters/pixel");

  PvlGroup pLineRes("LineResolution");
  pLineRes += ValidateKey("LineResolutionMinimum",lineResStat.Minimum(),
                         "meters/pixel");
  pLineRes += ValidateKey("LineResolutionMaximum",lineResStat.Maximum(),
                         "meters/pixel");
  pLineRes += ValidateKey("LineResolutionAverage",lineResStat.Average(),
                         "meters/pixel");
  pLineRes += ValidateKey("LineResolutionStandardDeviation",
                         lineResStat.StandardDeviation(),"meters/pixel");

  PvlGroup pResolution("Resolution");
  pResolution += ValidateKey("ResolutionMinimum",resStat.Minimum(),
                            "meters/pixel");
  pResolution += ValidateKey("ResolutionMaximum",resStat.Maximum(),
                            "meters/pixel");
  pResolution += ValidateKey("ResolutionAverage",resStat.Average(),
                            "meters/pixel");
  pResolution += ValidateKey("ResolutionStandardDeviation",
                            resStat.StandardDeviation(),"meters/pixel");

  PvlGroup pAspectRatio("AspectRatio");
  pAspectRatio += ValidateKey("AspectRatioMinimum",aspectRatioStat.Minimum());
  pAspectRatio += ValidateKey("AspectRatioMaximun",aspectRatioStat.Maximum());
  pAspectRatio += ValidateKey("AspectRatioAverage",aspectRatioStat.Average());
  pAspectRatio += ValidateKey("AspectRatioStandardDeviation",
                             aspectRatioStat.StandardDeviation());

  PvlGroup pPhase("PhaseAngle");
  pPhase += ValidateKey("PhaseMinimum",phaseStat.Minimum());
  pPhase += ValidateKey("PhaseMaximum",phaseStat.Maximum());
  pPhase += ValidateKey("PhaseAverage",phaseStat.Average());
  pPhase += ValidateKey("PhaseStandardDeviation",phaseStat.StandardDeviation());

  PvlGroup pEmission("EmissionAngle");
  pEmission += ValidateKey("EmissionMinimum",emissionStat.Minimum());
  pEmission += ValidateKey("EmissionMaximum",emissionStat.Maximum());
  pEmission += ValidateKey("EmissionAverage",emissionStat.Average());
  pEmission += ValidateKey("EmissionStandardDeviation",
                          emissionStat.StandardDeviation());

  PvlGroup pIncidence("IncidenceAngle");
  pIncidence += ValidateKey("IncidenceMinimum",incidenceStat.Minimum());
  pIncidence += ValidateKey("IncidenceMaximum",incidenceStat.Maximum());
  pIncidence += ValidateKey("IncidenceAverage",incidenceStat.Average());
  pIncidence += ValidateKey("IncidenceStandardDeviation",
                           incidenceStat.StandardDeviation());

  PvlGroup pTime("LocalSolarTime");
  pTime += ValidateKey("LocalSolarTimeMinimum",localSolarTimeStat.Minimum(),
                      "hours");
  pTime += ValidateKey("LocalSolarTimeMaximum",localSolarTimeStat.Maximum(),
                      "hours");
  pTime += ValidateKey("LocalSolarTimeAverage",localSolarTimeStat.Average(),
                      "hours");
  pTime += ValidateKey("LocalSolarTimeStandardDeviation",
                      localSolarTimeStat.StandardDeviation(),"hours");

  PvlGroup pLocalRadius("LocalRadius");
  pLocalRadius += ValidateKey("LocalRadiusMinimum",localRaduisStat.Minimum());
  pLocalRadius += ValidateKey("LocalRadiusMaximum",localRaduisStat.Maximum());
  pLocalRadius += ValidateKey("LocalRadiusAverage",localRaduisStat.Average());
  pLocalRadius += ValidateKey("LocalRadiusStandardDeviation",
                             localRaduisStat.StandardDeviation());

  PvlGroup pNorthAzimuth("NorthAzimuth");
  pNorthAzimuth += ValidateKey("NorthAzimuthMinimum",northAzimuthStat.Minimum());
  pNorthAzimuth += ValidateKey("NorthAzimuthMaximum",northAzimuthStat.Maximum());
  pNorthAzimuth += ValidateKey("NorthAzimuthAverage",northAzimuthStat.Average());
  pNorthAzimuth += ValidateKey("NorthAzimuthStandardDeviation",
                              northAzimuthStat.StandardDeviation());

  // Send the Output to the log area
  Application::Log(pUser);
  Application::Log(pLat);
  Application::Log(pLon);
  Application::Log(pSampleRes);
  Application::Log(pLineRes);
  Application::Log(pResolution);
  Application::Log(pAspectRatio);
  Application::Log(pPhase);
  Application::Log(pEmission);
  Application::Log(pIncidence);
  Application::Log(pTime);
  Application::Log(pLocalRadius);
  Application::Log(pNorthAzimuth);

  if (ui.WasEntered("TO")) {
    string from = ui.GetFilename("FROM");
    string outfile = Filename(ui.GetFilename("TO")).Expanded();
    bool exists = Filename(outfile).Exists();
    bool append = ui.GetBoolean("APPEND");

    //If the user chooses a fromat of PVL then write to the output file ("TO")
    if (ui.GetString("FORMAT") == "PVL") {
      Pvl temp;
      temp.SetTerminator("");
      temp.AddGroup(pUser);
      temp.AddGroup(pLat);
      temp.AddGroup(pLon);
      temp.AddGroup(pSampleRes);
      temp.AddGroup(pLineRes);
      temp.AddGroup(pResolution);
      temp.AddGroup(pAspectRatio);
      temp.AddGroup(pPhase);
      temp.AddGroup(pEmission);
      temp.AddGroup(pIncidence);
      temp.AddGroup(pTime);
      temp.AddGroup(pLocalRadius);
      temp.AddGroup(pNorthAzimuth);

      if (append) {
        temp.Append(outfile);
      }
      else {
        temp.Write(outfile);
      }
    }

    //Create a flatfile of the data with columhn headings 
    // the flatfile is comma delimited and can be imported in to spreadsheets
    else {
      ofstream os;
      bool writeHeader = true;
      if (append) {
        os.open(outfile.c_str(),ios::app);
        if (exists) {
          writeHeader = false;
        }
      }
      else {
        os.open(outfile.c_str(),ios::out);
      }

      // if new file or append and no file exists then write header
      if(writeHeader){
      os << "Filename,"<<
        "LatitudeMinimum,"<<
        "LatitudeMaximum,"<<
        "LatitudeAverage,"<<
        "LatitudeStandardDeviation,"<<
        "LongitudeMinimum,"<<
        "LongitudeMaximum,"<<
        "LongitudeAverage,"<<
        "LongitudeStandardDeviation,"<<
        "SampleResolutionMinimum,"<<
        "SampleResolutionMaximum,"<<
        "SampleResolutionAverage,"<<
        "SampleResolutionStandardDeviation,"<<
        "LineResolutionMinimum,"<<
        "LineResolutionMaximum,"<<
        "LineResolutionAverage,"<<
        "LineResolutionStandardDeviation,"<<
        "ResolutionMinimum,"<<
        "ResolutionMaximum,"<<
        "ResolutionAverage,"<<
        "ResolutionStandardDeviation,"<<
        "AspectRatioMinimum,"<<
        "AspectRatioMaximum,"<<
        "AspectRatioAverage,"<<
        "AspectRatioStandardDeviation,"<<
        "PhaseMinimum,"<<
        "PhaseMaximum,"<<
        "PhaseAverage,"<<
        "PhaseStandardDeviation,"<<
        "EmissionMinimum,"<<
        "EmissionMaximum,"<<
        "EmissionAverage,"<<
        "EmissionStandardDeviation,"<<
        "IncidenceMinimum,"<<
        "IncidenceMaximum,"<<
        "IncidenceAverage,"<<
        "IncidenceStandardDeviation,"<<
        "LocalSolarTimeMinimum,"<<
        "LocalSolarTimeMaximum,"<<
        "LocalSolarTimeAverage,"<<
        "LocalSolarTimeStandardDeviation,"<<
        "LocalRadiusMaximum,"<<
        "LocalRadiusMaximum,"<<
        "LocalRadiusAverage,"<<
        "LocalRadiusStandardDeviation,"<<
        "NorthAzimuthMinimum,"<<
        "NorthAzimuthMaximum,"<<
        "NorthAzimuthAverage,"<<
        "NorthAzimuthStandardDeviation,"<<endl;
      }
      os << Filename(from).Expanded() <<",";
        //call the function to write out the values for each group
        writeFlat(os, latStat);
        writeFlat(os, lonStat);
        writeFlat(os, sampleResStat);
        writeFlat(os, lineResStat);
        writeFlat(os, resStat);
        writeFlat(os, aspectRatioStat);
        writeFlat(os, phaseStat);
        writeFlat(os, emissionStat);
        writeFlat(os, incidenceStat);
        writeFlat(os, localSolarTimeStat);
        writeFlat(os, localRaduisStat);
        writeFlat(os, northAzimuthStat);
        os << endl;
    }
  }

  if( ui.GetBoolean("ATTACH") ) {

    string cam_name = "CameraStatistics";

    //Creates new CameraStatistics Table
    TableField fname( "Name", Isis::TableField::Text, 20 );
    TableField fmin( "Minimum", Isis::TableField::Double );
    TableField fmax( "Maximum", Isis::TableField::Double );
    TableField favg( "Average", Isis::TableField::Double );
    TableField fstd( "StandardDeviation", Isis::TableField::Double );

    TableRecord record;
    record += fname;
    record += fmin;
    record += fmax;
    record += favg;
    record += fstd;

    Table table( cam_name, record );

    vector<PvlGroup> grps;
    grps.push_back( pLat );
    grps.push_back( pLon );
    grps.push_back( pSampleRes );
    grps.push_back( pLineRes );
    grps.push_back( pResolution );
    grps.push_back( pAspectRatio );
    grps.push_back( pPhase );
    grps.push_back( pEmission );
    grps.push_back( pIncidence );
    grps.push_back( pTime );
    grps.push_back( pLocalRadius );
    grps.push_back( pNorthAzimuth );

    for( vector<PvlGroup>::iterator g = grps.begin(); g != grps.end(); g++ ) {
      int i = 0;
      record[i++] = g->Name();
      record[i++] = (double) (*g)[0][0];
      record[i++] = (double) (*g)[1][0];
      record[i++] = (double) (*g)[2][0];
      record[i++] = (double) (*g)[3][0];
      table += record;
    }

    icube->ReOpen( "rw" );
    icube->Write( table );
    p.WriteHistory(*icube);
    icube->Close();

  }

}

//function to add stats data to the stats object.
//also tests if the line and samp are valid
void buildStats (Camera *cam, int &sample, int &line){
  cam->SetImage(sample, line);
  if (cam->HasSurfaceIntersection()) {
    latStat.AddData(cam->UniversalLatitude());
    lonStat.AddData(cam->UniversalLongitude());
    resStat.AddData(cam->PixelResolution());
    sampleResStat.AddData(cam->SampleResolution());
    lineResStat.AddData(cam->LineResolution());
    phaseStat.AddData(cam->PhaseAngle());
    emissionStat.AddData(cam->EmissionAngle());
    incidenceStat.AddData(cam->IncidenceAngle());
    localSolarTimeStat.AddData(cam->LocalSolarTime());
    localRaduisStat.AddData(cam->LocalRadius());
    northAzimuthStat.AddData(cam->NorthAzimuth());

    double Aratio = cam->LineResolution() / cam->SampleResolution();
    aspectRatioStat.AddData(Aratio);    
  }
}

static inline string ValidateValue(const double &value) {
  if ( IsSpecial(value) ) {
    return (string("NULL"));
  }
  else {
    return ((string) iString(value));
  }
}

//function to write the stats values to flat file
void writeFlat (ofstream &os, Statistics &s){
  os << ValidateValue(s.Minimum())<<","<<
        ValidateValue(s.Maximum())<<","<<
        ValidateValue(s.Average())<<","<<
        ValidateValue(s.StandardDeviation())<<",";
}
