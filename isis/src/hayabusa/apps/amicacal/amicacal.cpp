// $Id: amicacal.cpp 6045 2015-02-07 02:06:59Z moses@GS.DOI.NET $
#include "Isis.h"

#include <vector>
#include <algorithm>
#include <memory>
#include <cstdio>
#include <cmath>

#include <QFile>
#include <QString>
#include <QScopedPointer>
#include <QTemporaryFile>
#include <QVector>

#include "AlphaCube.h"
#include "AmicaCalUtils.h"
#include "Buffer.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "LineManager.h"
#include "Pixel.h"
#include "ProcessByLine.h"
#include "ProcessBySample.h"
#include "ProcessByBrick.h"
#include "ProcessByBoxcar.h"
#include "ProgramLauncher.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Spice.h"
#include "Statistics.h"
#include "TextFile.h"


using namespace Isis;
using namespace std;

// Calibration support routines
FileName DetermineFlatFieldFile(const QString &filter, const bool nullPolarPix);
void Calibrate(vector<Buffer *>& in, vector<Buffer *>& out);

void loadCalibrationVariables();
void psfCorrection(vector<Buffer *>& in, vector<Buffer *>& out);
void psfCorrectionBoxcar(Buffer &in, double &result);


// Temporary cube file pointer deleter
struct TemporaryCubeDeleter {
   static inline void cleanup(Cube *cube) {
     if ( cube ) {

       FileName filename( cube->fileName() );
       delete cube;
       remove( filename.expanded().toLatin1().data() );
     }
   }
};


//For subimage and binning mapping
static AlphaCube *alpha(0);


QString g_filter = "";
static QString g_target ="";
static int g_HayabusaNaifCode = -130;
static Pvl g_configFile;

//Bias calculation variables

static double g_b0(0);
static double g_b1(0);
static double g_b2(0);
static double g_bias(0);

static QString g_launchTimeStr;
static iTime g_launchTime;
static QString g_startTime;


//Dark Current variables
static double g_d0(0);
static double g_d1(0);
static double g_temp(0);
static double g_darkCurrent(0);



//Smear calculation variables
static double g_Tvct(0);       //!< Vertical charge-transfer period (in seconds).
static double g_texp(1);       //!< Exposure time.
static double g_timeRatio(1.0);

//Linearity calculation variables
static double g_Gamma(0);
static double g_L0(0);
static double g_L1(0);


// Calibration parameters
static int nsubImages(0);      //!< Number of sub images
static int binning(1);         //!< The number of samples/lines which are binned
static bool g_nullPolarizedPixels = true;   /**< Flag which tells us if the Polarized pixels are to
                                                 to be set to ISIS::Null */

static bool g_iofCorrection = true;  //!< Is I/F correction to be applied?


//  I/F variables
static double g_solarDist(1.0);  /**< Distance from the Sun to the target body
                                 (used to calculate g_iof) */
static double g_iof(1.0);        //!< I/F conversion value
static double g_iofScale(1.0);
static double g_solarFlux(1.0);  //!< The solar flux (used to calculate g_iof).
static double g_v_standard(3.42E-3);//!< Base conversion for all filters (Tbl. 9)


//Hot pixel vector container
static QVector<Pixel> hotPixelVector;  //!< A pixel vector that contains the Hot Pixel locations

//PSF variables
static int ns,nl,nb;     //!< Number of samples, lines, bands of the input cube
//static int g_size(23);   //!< The size of the Boxcar used for calculating the light diffusion model.

static const int g_N = 6;
static double g_alpha(0.0);
static double * g_psfFilter;
static double g_sigma[g_N];
static double g_A[g_N];


void IsisMain() {


  UserInterface& ui = Application::GetUserInterface();
  g_nullPolarizedPixels = ui.GetBoolean("NULLPOLARPIX");
  g_iofCorrection = ui.GetBoolean("IOF");

  const QString amicacal_program = "amicacal";
  const QString amicacal_version = "1.0";
  const QString amicacal_revision = "$Revision$";
  QString amicacal_runtime = Application::DateTime();

  ProcessBySample p;

  Cube *icube = p.SetInputCube("FROM");




  // Basic assurances...
  if (icube->bandCount() != 1) {
    throw IException(IException::User,
                     "AMICA images may only contain one band", _FILEINFO_);
  }

  PvlGroup& inst = icube->group("Instrument");
  PvlGroup &bandbin = icube->group("BandBin");
  PvlGroup &archive = icube->group("Archive");

  QString filter = bandbin["Name"];
  g_filter=filter;

  binning = inst["Binning"];
  int startLine = inst["FirstLine"];
  int startSample = inst["FirstSample"];
  int lastLine = inst["LastLine"];
  int lastSample = inst["LastSample"];

  //Set up binning and image subarea mapping

  AlphaCube myAlpha(1024,1024,icube->sampleCount(), icube->lineCount(),
                    startSample+1,startLine+1,lastSample+1,lastLine+1);

  alpha=&myAlpha;

  try {

  g_texp = inst["ExposureDuration"] ; 

  }
  catch(IException &e) {

    QString msg = "Unable to read [ExposureDuration] keyword in the Instrument group "
                  "from input file [" + icube->fileName() + "]";
    throw IException(e, IException::Io,msg, _FILEINFO_);

  }


  try {

  g_temp = inst["CcdTemperature"] ;

  }
  catch(IException &e) {

    QString msg = "Unable to read [CcdTemperature] keyword in the Instrument group "
                  "from input file [" + icube->fileName() + "]";
    throw IException(e, IException::Io,msg, _FILEINFO_);

  }



//tjw
  QString startTime = inst["SpacecraftClockStartCount"];





  g_startTime = startTime;
  binning = inst["Binning"];

  int startline = inst["FirstLine"];
  int startsample = inst["FirstSample"];
  int lastline = inst["LastLine"];
  int lastsample = inst["LastSample"];


  nsubImages = archive["SubImageCount"];  // If > 1, some correction is 
                                          // not needed/performed





  // I/F values
  QString target = inst["TargetName"];
  g_target = target;


  if ( sunDistanceAU(startTime,target,g_solarDist) ) {

    g_iof = pi_c()*(g_solarDist*g_solarDist)*(g_v_standard*g_iofScale);

  }



  //  Determine if we need to subsample the flat field should pixel binning
  //  occurred
  QScopedPointer<Cube, TemporaryCubeDeleter> flatcube;
  FileName flatfile= DetermineFlatFieldFile(g_filter,g_nullPolarizedPixels);

  QString reducedFlat(flatfile.expanded());


//Image is not cropped
if (startline ==0 && startsample == 0){


  if (binning > 1) {
    QString scale(toString(binning));
    FileName newflat = FileName::createTempFile("$TEMPORARY/" + flatfile.baseName() + "_reduced.cub");

    reducedFlat = newflat.expanded();
    QString parameters = "FROM=" + flatfile.expanded() +
       " TO="   + newflat.expanded() +
       " MODE=SCALE" +
       " LSCALE=" + scale +
       " SSCALE=" + scale;

    try {
      ProgramLauncher::RunIsisProgram("reduce", parameters);
    }
    catch (IException& ie) {
      remove(reducedFlat.toLatin1().data());
      throw ie;
    }
    QScopedPointer<Cube, TemporaryCubeDeleter> reduced(new Cube(reducedFlat, "r"));
    flatcube.swap( reduced );
  }

  // Set up processing for flat field as a second input file
  CubeAttributeInput att;
  p.SetInputCube(reducedFlat, att);

}

else {


   //Image is cropped so we have to deal with it
      FileName transFlat =
          FileName::createTempFile("$TEMPORARY/" + flatfile.baseName() + "_translated.cub");

      Cube *flatOriginal = new Cube(flatfile.expanded() );

      int transform[5] = {binning,startsample,startline,lastsample,lastline};

      //Translates and scales the flatfield image.  Scaling
      //might be necessary in the event that the raw image was also binned.

      translate(flatOriginal,transform,transFlat.expanded());

      QScopedPointer<Cube, TemporaryCubeDeleter> translated(new Cube(transFlat.expanded(), "r"));
      flatcube.swap(translated);

      CubeAttributeInput att;
      p.SetInputCube(transFlat.expanded(),att);

 }

  Cube *ocube;
  ocube = p.SetOutputCube("TO");
  QString fname = ocube->fileName();

  ns = icube->sampleCount();
  nl = icube->lineCount();
  nb = icube->bandCount();


  loadCalibrationVariables();

  g_timeRatio = g_Tvct/(g_texp+g_Tvct);


  g_darkCurrent = g_d0*exp(g_d1*g_temp);

  // Calibrate!
  try {
    p.Progress()->SetText("Calibrating Hayabusa Cube");
    p.StartProcess(Calibrate);
  }
  catch (IException &ie) {
    throw IException(ie, IException::Programmer, 
                     "Radiometric calibration failed!", _FILEINFO_);
  }


  p.EndProcess();
#if 0
  //PSF correction
  CubeAttributeInput attInput;
  CubeAttributeOutput attOutput;

  ProcessByBoxcar pDiffusionModel;
  QScopedPointer<Cube, TemporaryCubeDeleter> diffusionModel;

  QString kernel_sz = QString::number(g_size);


  //QTemporaryFile psfModel("$TEMPORARY/psfModel.cub");


  QFile psfModel("psfModel"+QString::number(g_size)+".cub");

  //QFile("psfModel.cub");
  //psfModel.setAutoRemove(false);


  pDiffusionModel.SetInputCube(fname,attInput);
  pDiffusionModel.SetOutputCube(psfModel.fileName(),attOutput,ns,nl,nb);

  pDiffusionModel.SetBoxcarSize(g_size,g_size);
  g_psfFilter = setPSFFilter(g_size, g_A,g_sigma, g_alpha,g_N,binning);

  try {

       pDiffusionModel.StartProcess(psfCorrectionBoxcar);  //Determine the diffusion model.

    }

    catch(IException &ie){

    throw IException(ie, IException::Programmer,
                     "Calculating the diffusion model failed!", _FILEINFO_);

    }


    pDiffusionModel.EndProcess();

   //Apply the PSF correction
    ProcessByLine pPSFCorrection;

    //The diffusion model
    pPSFCorrection.SetInputCube(psfModel.fileName(),attInput);

    //The original output cube.
    pPSFCorrection.SetInputCube(fname,attInput);
    pPSFCorrection.SetOutputCube("PSFCORRECTED");


    try {

      pPSFCorrection.StartProcess(psfCorrection);
    }

    catch(IException &ie){

      throw IException(ie, IException::Programmer,
                       "Applying the PSF correction failed!", _FILEINFO_);


    }    


   pPSFCorrection.EndProcess();

#endif
  // Log calibration activity
  PvlGroup calibrationLog("RadiometricCalibration");
  calibrationLog.addKeyword(PvlKeyword("SoftwareName", amicacal_program));
  calibrationLog.addKeyword(PvlKeyword("SoftwareVersion", amicacal_version));
  calibrationLog.addKeyword(PvlKeyword("ProcessDate", amicacal_runtime));
  calibrationLog.addKeyword(PvlKeyword("FlatFieldFile", flatfile.originalPath()
                                       + "/" + flatfile.name()));

  // Write Calibration group to file and log
  //finalCube->putGroup(calibrationLog);
  Application::Log(calibrationLog);
  //configFile.clear();



}


/**
 * @brief Determine name of flat field file to apply
 * 
 * @author 2016-03-30 Kris Becker
 * 
 * @param filter  Name of AMICA filter
 * 
 * @return FileName Path and name of flat file file
 */
FileName DetermineFlatFieldFile(const QString &filter, const bool nullPolarPix) {

  QString fileName = "$hayabusa/calibration/flatfield/";


  // FileName consists of binned/notbinned, camera, and filter

  if (nullPolarPix) {
      fileName += "flat_" + filter.toLower() + "np.cub";
  }
  else {

    fileName += "flat_" + filter.toLower() + ".cub";

  }
  FileName final(fileName);

  //tjw:  So was this part
  //final = final.highestVersion();
  return final;
}


/**
 * @brief This function moves the PSF kernel through each pixel of the input cube and approximates
 * the amount of light diffusion produced by that pixel.
 * @param in The radiometrically calibrated cube (minus PSF correction).
 * @param result  The light diffusion estimate at the central pixel of the boxcar of all the
 * surrounding pixels.
 */

void psfCorrectionBoxcar(Buffer &in, double &result) {

    result = 0;

    Statistics stats;

    for (int i = 0; i < in.size(); i++) {


      if(!IsSpecial(in[i])) {

        stats.AddData(in[i]*g_psfFilter[i]);
        //result += in[i]*g_psfFilter[i];
      }

    }

    result = stats.Sum();



}


/**
 * @brief Applies the PSF function
 * @param in[0]  The PSF light diffusion model (a cube)
 * @parm  in[1]  The radiometrically corrected cube (without PSF correction).
 * @param out    The radiometrically corrected cube after PSF correction has been applied.
 */

void psfCorrection(vector<Buffer *> &in, vector<Buffer *> &out) {


  Buffer& nopsf    = *in[1];
  Buffer& psfVals =  *in[0];
  Buffer& imageOut  = *out[0];


  for (int i = 0; i < nopsf.size(); i++) {
    if (!IsSpecial(psfVals[i])) {

      imageOut[i] = nopsf[i]-psfVals[i];
      //imageOut[i] = psfVals[i];

    }
    else {
      imageOut[i] = nopsf[i];
    }
  }

}


/**
 * @brief Loads the calibration variables into the program.
 */

void loadCalibrationVariables()  {


  FileName calibFile("$hayabusa/calibration/amica/amicaCalibration????.trn");
  calibFile = calibFile.highestVersion();

  //Pvl configFile;
  g_configFile.read(calibFile.expanded());


  //Load the groups

  PvlGroup &Bias = g_configFile.findGroup("Bias");
  PvlGroup &DarkCurrent = g_configFile.findGroup("DarkCurrent");
  PvlGroup &Smear = g_configFile.findGroup("SmearRemoval");
  PvlGroup &Linearity = g_configFile.findGroup("Linearity");
  PvlGroup &hotPixels = g_configFile.findGroup("HotPixels");
  PvlGroup &psfDiffuse = g_configFile.findGroup("PSFDiffuse");
  PvlGroup &psfFocused = g_configFile.findGroup("PSFFocused");
  PvlGroup &solar = g_configFile.findGroup("SOLARFLUX");
  PvlGroup &iof = g_configFile.findGroup("IOF");


  //Load the hot pixels into a vector

  for (int i = 0; i< hotPixels.keywords(); i++ ){

    int samp(hotPixels[i][0].toInt());
    int line (hotPixels[i][1].toInt());


    hotPixelVector.append( Pixel(alpha->BetaSample(samp),alpha->BetaLine(line),1,0));
  }


  //Load linearity variables
  g_Gamma = Linearity["Gamma"];
  g_Gamma = 1.0-g_Gamma;

  g_L0 = Linearity["L"][0].toDouble();
  g_L1 = Linearity["L"][1].toDouble();

  //Load Smear Removal Variables
  g_Tvct = Smear["Tvct"];



  //Load DarkCurrent variables
  g_d0 = DarkCurrent["D"][0].toDouble();
  g_d1 = DarkCurrent["D"][1].toDouble();


  //Load Bias variables
  g_b0 = Bias["B"][0].toDouble();
  g_b1 = Bias["B"][1].toDouble();
  g_b2 = Bias["B"][2].toDouble();


  g_launchTimeStr=QString(Bias["launchTime"]);

  //cout << g_launchTimeStr << endl;

  //static iTime g_launchTime("2003-05-09T04:29:25");
  g_launchTime =g_launchTimeStr;
  //iTime g_t0(g_startTime);
  //cout << "g_t0"  << g_t0.EtString();

  //Compute BIAS correction factor (it's a constant so do it once!)

  double obsStartTime;
  double tsecs;
  double tdays;

  loadNaifTiming();  //Ensure the proper kernels are loaded

  scs2e_c(g_HayabusaNaifCode,g_startTime.toLatin1().data(), &obsStartTime);  
  tsecs = obsStartTime - g_launchTime.Et();
  tdays = tsecs/86400;
  g_bias = g_b0+g_b1*tdays+g_b2*(tdays*tdays);

  //g_bias = 0;
  //cout << "g_bias = "  << g_bias << endl;


  //Load the PSF constants.  These come from
  //Ishiguro, 2014 ('Scattered light correction of Hayabusa/AMICA data and
  //quantitative spectral comparisons of Itokawa')

   g_alpha = psfFocused[g_filter.toLower()];

   for (int i =0; i < g_N; i++) {

     g_sigma[i] = psfDiffuse["sigma"][i].toDouble();
     g_A[i] = psfDiffuse[g_filter.toLower()][i].toDouble();

   }

  //Load the Solar Flux for the specific filter
  g_solarFlux=solar[g_filter.toLower()];

  //radiance = g_v_standard*g_iofScale
  //iof      = radiance * pi *dist_au^2

  g_v_standard = iof["iof_standard"];
  g_iofScale   = iof[g_filter];

  return;

}


/**
 * @brief Apply radiometric correction to each line of an AMICA image.
 * @author 2016-03-30 Kris Becker
 * @param in   Raw image and flat field
 * @param out  Radometrically corrected image
 */

void Calibrate(vector<Buffer *>& in, vector<Buffer *>& out) {

  Buffer& imageIn   = *in[0];
  Buffer& flatField = *in[1];
  Buffer& imageOut  = *out[0];

  int pixelsToNull = 12;

  int currentSample = imageIn.Sample();
  int alphaSample = alpha->AlphaSample(currentSample);

  if ( (alphaSample <= pixelsToNull)  || (alphaSample >= (1024 - pixelsToNull ))) {

    for (int i = 0; i < imageIn.size(); i++ ) {
      imageOut[i] = Isis::Null;
    }
    return;

  }



  //Compute smear component here as its a constant for the entire sample

  double t1 = g_timeRatio/imageIn.size();
  double b = binning;
  double c1(1.0);  //default if no binning

  if (binning > 1) {
    c1 = 1.0/(1.0 + t1*((b -1.0)/(2.0*b) ) );
  }

  double smear = 0;

  for (int j = 0; j < imageIn.size(); j++ ) {
    smear += t1*(imageIn[j] - g_bias);
  }



  //iterate over the line space

  for (int i = 0; i < imageIn.size(); i++) {

    imageOut[i] = imageIn[i];


    // Check for special pixel in input image and pass through
    if (Isis::IsSpecial(imageOut[i])) {
      imageOut[i] = imageIn[i];
      continue;
    }


    // 1) BIAS Removal - Only needed if not on-board corrected
    if ( nsubImages <= 1 ) {

      if ( (imageOut[i] - g_bias) < 0) {
        imageOut[i] = Isis::Null;
      }
      else {
      imageOut[i]= imageOut[i] - g_bias;

      }
    }

    // 2) LINEARITY Correction - always done

    if (imageOut[i] != Isis::Null) {
      imageOut[i] = pow(imageOut[i],g_Gamma) +g_L0*imageOut[i]*exp(g_L1*imageOut[i]);
    }


    // 3) DARK Current - Currently negligible and removed

#if 0

      imageOut[i] = imageOut[i]-g_darkCurrent;

#endif


    // 4) HOT Pixel Removal

      bool hot = false;

      for (int j=0; j < hotPixelVector.size(); j++) {

        if ((hotPixelVector[j].sample() == currentSample) && (hotPixelVector[j].line() == i)) {

          imageOut[i] = Isis::Null;
          hot = true;

        }
      }

      if (hot == true)
        continue;



    // 5) READOUT Smear Removal - Not needed if on-board corrected.  Binning is
    //    accounted for in computation of c1 before loop.



    if ( imageOut[i] != Isis::Null && nsubImages <= 1 ) {

      imageOut[i] = c1*(imageOut[i] - smear);


      }


    // 6) FLATFIELD correction
    //  Check for any special pixels in the flat field (unlikely)


      if (Isis::IsSpecial(flatField[i])) {
        imageOut[i] = Isis::Null;
        continue;
      }
    else {
       if (imageOut[i] != Isis::Null )
          imageOut[i] /= flatField[i];
    }


    // 7) I/F Conversion


    if (imageOut[i] != Isis::Null && g_iofCorrection == true) {
          imageOut[i] *= g_iof;
       }

  }

  return;
}

