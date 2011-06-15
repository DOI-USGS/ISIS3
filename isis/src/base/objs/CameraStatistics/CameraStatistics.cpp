#include "IsisDebug.h"
#include "CameraStatistics.h"

#include "Camera.h"
#include "Cube.h"
#include "Distance.h"
#include "Progress.h"
#include "Statistics.h"

namespace Isis {
  CameraStatistics::CameraStatistics(std::string filename, int sinc, int linc) {
    Cube cube;
    cube.Open(filename);
    Camera *cam = cube.Camera();
    init(cam, sinc, linc, filename);
  }


  CameraStatistics::CameraStatistics(Camera *cam, int sinc, int linc) {
    init(cam, sinc, linc, "");
  }


  CameraStatistics::CameraStatistics(Camera *cam, int sinc, int linc,
      std::string filename) {
    init(cam, sinc, linc, filename);
  }


  void CameraStatistics::init(Camera *cam, int sinc, int linc,
      std::string filename) {

    m_filename = filename;
    m_sinc = sinc;
    m_linc = linc;

    m_latStat = new Statistics();
    m_lonStat = new Statistics();
    m_resStat = new Statistics();
    m_sampleResStat = new Statistics();
    m_lineResStat = new Statistics();
    m_aspectRatioStat = new Statistics();
    m_phaseStat = new Statistics();
    m_emissionStat = new Statistics();
    m_incidenceStat = new Statistics();
    m_localSolarTimeStat = new Statistics();
    m_localRaduisStat = new Statistics();
    m_northAzimuthStat = new Statistics();

    int eband = cam->Bands();

    // If the camera is band independent then only run one band
    if(cam->IsBandIndependent()) eband = 1;

    int pTotal = eband * ((cam->Lines() - 2) / linc + 2);
    Progress progress;
    progress.SetMaximumSteps(pTotal);
    progress.CheckStatus();

    for(int band = 1; band <= eband; band++) {
      cam->SetBand(band);
      for(int line = 1; line < (int)cam->Lines(); line = line + linc) {
        for(int sample = 1; sample < cam->Samples(); sample = sample + sinc) {
          addStats(cam, sample, line);
        }
        //set the sample value to the last sample and run buildstats
        int sample = cam->Samples();
        addStats(cam, sample, line);
        progress.CheckStatus();
      }
      //set the line value to the last line and run on all samples(sample + sinc)
      int line = cam->Lines();
      for(int sample = 1; sample < cam->Samples(); sample = sample + sinc) {
        addStats(cam, sample, line);
      }
      //set last sample and run with last line
      int sample = cam->Samples();
      addStats(cam, sample, line);
      progress.CheckStatus();
    }
  }


  CameraStatistics::~CameraStatistics() {
    if (m_latStat != NULL) {
      delete m_latStat;
      m_latStat = NULL;
    }
    if (m_lonStat != NULL) {
      delete m_lonStat;
      m_lonStat = NULL;
    }
    if (m_resStat != NULL) {
      delete m_resStat;
      m_resStat = NULL;
    }
    if (m_sampleResStat != NULL) {
      delete m_sampleResStat;
      m_sampleResStat = NULL;
    }
    if (m_lineResStat != NULL) {
      delete m_lineResStat;
      m_lineResStat = NULL;
    }
    if (m_aspectRatioStat != NULL) {
      delete m_aspectRatioStat;
      m_aspectRatioStat = NULL;
    }
    if (m_phaseStat != NULL) {
      delete m_phaseStat;
      m_phaseStat = NULL;
    }
    if (m_emissionStat != NULL) {
      delete m_emissionStat;
      m_emissionStat = NULL;
    }
    if (m_incidenceStat != NULL) {
      delete m_incidenceStat;
      m_incidenceStat = NULL;
    }
    if (m_localSolarTimeStat != NULL) {
      delete m_localSolarTimeStat;
      m_localSolarTimeStat = NULL;
    }
    if (m_localRaduisStat != NULL) {
      delete m_localRaduisStat;
      m_localRaduisStat = NULL;
    }
    if (m_northAzimuthStat != NULL) {
      delete m_northAzimuthStat;
      m_northAzimuthStat = NULL;
    }
  }


  //function to add stats data to the stats object.
  //also tests if the line and samp are valid
  void CameraStatistics::addStats(Camera *cam, int &sample, int &line) {
    cam->SetImage(sample, line);
    if(cam->HasSurfaceIntersection()) {
      m_latStat->AddData(cam->UniversalLatitude());
      m_lonStat->AddData(cam->UniversalLongitude());
      m_resStat->AddData(cam->PixelResolution());
      m_sampleResStat->AddData(cam->SampleResolution());
      m_lineResStat->AddData(cam->LineResolution());
      m_phaseStat->AddData(cam->PhaseAngle());
      m_emissionStat->AddData(cam->EmissionAngle());
      m_incidenceStat->AddData(cam->IncidenceAngle());
      m_localSolarTimeStat->AddData(cam->LocalSolarTime());
      m_localRaduisStat->AddData(cam->LocalRadius().GetMeters());
      m_northAzimuthStat->AddData(cam->NorthAzimuth());

      double aspectRatio = cam->LineResolution() / cam->SampleResolution();
      m_aspectRatioStat->AddData(aspectRatio);
    }
  }


  /**  Produces NULL values for special pixels
   *
   * @param keyname  Name of keyword to generate
   * @param value    Value to write to keyword
   * @param unit     Optional units for keywords
   *
   * @return PvlKeyword Newly created keyword
   */
  PvlKeyword CameraStatistics::constructKeyword(std::string keyname,
      double value, std::string unit="") const {

    if(IsSpecial(value)) {
      return (PvlKeyword(keyname, "NULL"));
    }
    else {
      return (PvlKeyword(keyname, value, unit));
    }
  }


  Pvl CameraStatistics::toPvl() const {
    // Set up the Pvl groups and get min, max, avg, and sd for each statstics
    // object
    PvlGroup pUser("User Parameters");
    if (m_filename != "") pUser += PvlKeyword("Filename", m_filename);
    pUser += PvlKeyword("Linc", m_linc);
    pUser += PvlKeyword("Sinc", m_sinc);

    PvlGroup pLat("Latitude");
    pLat += constructKeyword("LatitudeMinimum", m_latStat->Minimum());
    pLat += constructKeyword("LatitudeMaximum", m_latStat->Maximum());
    pLat += constructKeyword("LatitudeAverage", m_latStat->Average());
    pLat += constructKeyword("LatitudeStandardDeviation", m_latStat->StandardDeviation());

    PvlGroup pLon("Longitude");
    pLon += constructKeyword("LongitudeMinimum", m_lonStat->Minimum());
    pLon += constructKeyword("LongitudeMaximum", m_lonStat->Maximum());
    pLon += constructKeyword("LongitudeAverage", m_lonStat->Average());
    pLon += constructKeyword("LongitudeStandardDeviation", m_lonStat->StandardDeviation());

    PvlGroup pSampleRes("SampleResolution");
    pSampleRes += constructKeyword("SampleResolutionMinimum", m_sampleResStat->Minimum(),
        "meters/pixel");
    pSampleRes += constructKeyword("SampleResolutionMaximum", m_sampleResStat->Maximum(),
        "meters/pixel");
    pSampleRes += constructKeyword("SampleResolutionAverage", m_sampleResStat->Average(),
        "meters/pixel");
    pSampleRes += constructKeyword("SampleResolutionStandardDeviation",
        m_sampleResStat->StandardDeviation(), "meters/pixel");

    PvlGroup pLineRes("LineResolution");
    pLineRes += constructKeyword("LineResolutionMinimum", m_lineResStat->Minimum(),
        "meters/pixel");
    pLineRes += constructKeyword("LineResolutionMaximum", m_lineResStat->Maximum(),
        "meters/pixel");
    pLineRes += constructKeyword("LineResolutionAverage", m_lineResStat->Average(),
        "meters/pixel");
    pLineRes += constructKeyword("LineResolutionStandardDeviation",
        m_lineResStat->StandardDeviation(), "meters/pixel");

    PvlGroup pResolution("Resolution");
    pResolution += constructKeyword("ResolutionMinimum", m_resStat->Minimum(),
        "meters/pixel");
    pResolution += constructKeyword("ResolutionMaximum", m_resStat->Maximum(),
        "meters/pixel");
    pResolution += constructKeyword("ResolutionAverage", m_resStat->Average(),
        "meters/pixel");
    pResolution += constructKeyword("ResolutionStandardDeviation",
        m_resStat->StandardDeviation(), "meters/pixel");

    PvlGroup pAspectRatio("AspectRatio");
    pAspectRatio += constructKeyword("AspectRatioMinimum", m_aspectRatioStat->Minimum());
    pAspectRatio += constructKeyword("AspectRatioMaximun", m_aspectRatioStat->Maximum());
    pAspectRatio += constructKeyword("AspectRatioAverage", m_aspectRatioStat->Average());
    pAspectRatio += constructKeyword("AspectRatioStandardDeviation",
        m_aspectRatioStat->StandardDeviation());

    PvlGroup pPhase("PhaseAngle");
    pPhase += constructKeyword("PhaseMinimum", m_phaseStat->Minimum());
    pPhase += constructKeyword("PhaseMaximum", m_phaseStat->Maximum());
    pPhase += constructKeyword("PhaseAverage", m_phaseStat->Average());
    pPhase += constructKeyword("PhaseStandardDeviation", m_phaseStat->StandardDeviation());

    PvlGroup pEmission("EmissionAngle");
    pEmission += constructKeyword("EmissionMinimum", m_emissionStat->Minimum());
    pEmission += constructKeyword("EmissionMaximum", m_emissionStat->Maximum());
    pEmission += constructKeyword("EmissionAverage", m_emissionStat->Average());
    pEmission += constructKeyword("EmissionStandardDeviation",
        m_emissionStat->StandardDeviation());

    PvlGroup pIncidence("IncidenceAngle");
    pIncidence += constructKeyword("IncidenceMinimum", m_incidenceStat->Minimum());
    pIncidence += constructKeyword("IncidenceMaximum", m_incidenceStat->Maximum());
    pIncidence += constructKeyword("IncidenceAverage", m_incidenceStat->Average());
    pIncidence += constructKeyword("IncidenceStandardDeviation",
        m_incidenceStat->StandardDeviation());

    PvlGroup pTime("LocalSolarTime");
    pTime += constructKeyword("LocalSolarTimeMinimum", m_localSolarTimeStat->Minimum(),
        "hours");
    pTime += constructKeyword("LocalSolarTimeMaximum", m_localSolarTimeStat->Maximum(),
        "hours");
    pTime += constructKeyword("LocalSolarTimeAverage", m_localSolarTimeStat->Average(),
        "hours");
    pTime += constructKeyword("LocalSolarTimeStandardDeviation",
        m_localSolarTimeStat->StandardDeviation(), "hours");

    PvlGroup pLocalRadius("LocalRadius");
    pLocalRadius += constructKeyword("LocalRadiusMinimum", m_localRaduisStat->Minimum());
    pLocalRadius += constructKeyword("LocalRadiusMaximum", m_localRaduisStat->Maximum());
    pLocalRadius += constructKeyword("LocalRadiusAverage", m_localRaduisStat->Average());
    pLocalRadius += constructKeyword("LocalRadiusStandardDeviation",
        m_localRaduisStat->StandardDeviation());

    PvlGroup pNorthAzimuth("NorthAzimuth");
    pNorthAzimuth += constructKeyword("NorthAzimuthMinimum", m_northAzimuthStat->Minimum());
    pNorthAzimuth += constructKeyword("NorthAzimuthMaximum", m_northAzimuthStat->Maximum());
    pNorthAzimuth += constructKeyword("NorthAzimuthAverage", m_northAzimuthStat->Average());
    pNorthAzimuth += constructKeyword("NorthAzimuthStandardDeviation",
        m_northAzimuthStat->StandardDeviation());

    Pvl returnPvl;
    returnPvl.SetTerminator("");
    returnPvl.AddGroup(pUser);
    returnPvl.AddGroup(pLat);
    returnPvl.AddGroup(pLon);
    returnPvl.AddGroup(pSampleRes);
    returnPvl.AddGroup(pLineRes);
    returnPvl.AddGroup(pResolution);
    returnPvl.AddGroup(pAspectRatio);
    returnPvl.AddGroup(pPhase);
    returnPvl.AddGroup(pEmission);
    returnPvl.AddGroup(pIncidence);
    returnPvl.AddGroup(pTime);
    returnPvl.AddGroup(pLocalRadius);
    returnPvl.AddGroup(pNorthAzimuth);
    return returnPvl;
  }


} // End namespace Isis

