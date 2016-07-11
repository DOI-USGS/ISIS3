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
#include <QVector>

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
       remove( filename.expanded().toAscii().data() );
     }
   }
};

Cube *mCube = NULL;

QString g_filter = "";
static QString g_target ="";
static int g_HayabusaNaifCode = -130;
static Pvl g_configFile;

//Bias calculation variables

static double g_b0(0);
static double g_b1(0);
static double g_b2(0);

static QString g_launchTimeStr;
static iTime g_launchTime;
static QString g_startTime;


//Dark Current variables

static double g_d0(0);
static double g_d1(0);
static double g_temp(0);
static double g_darkCurrent(0);



//Smear calculation variables

static double g_Tvct(0);       // vertical charge-transfer period (in seconds)
static double g_texp(1);       // exposure time;
static double g_timeRatio(1.0);

//Linearity calculation variables

static double g_Gamma(0);


static double g_L0(0);
static double g_L1(0);


// Calibration parameters
static int nsubImages(0);      // Number of sub images
static int binning(1);
static bool g_nullPolarizedPixels = true;
static bool g_applyPSFCorrection = true;
static bool g_iofCorrection = true;

//  I/F variables
static double g_solarDist(1.0);
static double g_iof(1.0);   //  I/F value for observation
double g_iofScale(1.0);
static double g_solarFlux(1.0);


//Hot pixel vector container
static QVector<Pixel> hotPixelVector;

//PSF variables

static int ns,nl,nb;
static int g_size(43);
static double g_weight(0.0006);
static const int g_N = 6;
static double g_alpha(0.0);
static double * g_psfFilter;
static double g_sigma[g_N];
static double g_A[g_N];


void IsisMain() {


  UserInterface& ui = Application::GetUserInterface();
  g_nullPolarizedPixels = ui.GetBoolean("NULLPOLARPIX");
  //g_applyPSFCorrection = ui.GetBoolean("PSF");
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

  loadCalibrationVariables();

  try {

  g_texp = inst["ExposureDuration"] ; 
  cout << "g_texp = " << g_texp << endl;

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

  g_timeRatio = g_Tvct/(g_texp+g_Tvct);
  cout <<"g_timeRatio = " << g_timeRatio << endl;
  g_darkCurrent = g_d0*exp(g_d1*g_temp);
  cout << "Dark current = "<< g_darkCurrent << endl;


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

    g_iof = pi_c()*(g_solarDist*g_solarDist)/g_solarFlux;

  }

  cout << "g_iof = "  << g_iof << endl;

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
      remove(reducedFlat.toAscii().data());
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

      cout << "transflat = " << transFlat.expanded() << endl;
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




    CubeAttributeOutput att10;
    Cube *ocube;
    //FileName psfStart = FileName::createTempFile("$TEMPORARY/psfStart.cub");



/*
    if (!g_applyPSFCorrection) {
      ocube = p.SetOutputCube("TO");
      //QScopedPointer<Cube,TemporaryCubeDeleter> psfCorrectionInputCube(ocube);
    }
    else {

      ocube = p.SetOutputCube("TO");
      //QScopedPointer<Cube,TemporaryCubeDeleter> ocube (new Cube(psfStart.expanded(),"r") );
      ocube = p.SetOutputCube(psfStart.expanded(),att10);

    }

*/

 ocube = p.SetOutputCube("TO");

  QString fname = ocube->fileName();

  //ns = ocube->sampleCount();
  //nl = ocube->lineCount();
  //nb = ocube->bandCount();


  ns = icube->sampleCount();
  nl = icube->lineCount();
  nb = icube->bandCount();



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


CubeAttributeInput att1;


  //if (g_applyPSFCorrection) {

    //ocube is input for this process

    ProcessByBoxcar p1;

    QString kernel_sz = QString::number(g_size);


    p1.SetInputCube(fname,att1);
    CubeAttributeOutput att2;
    QString psfVals = "psf"+kernel_sz+".cub";
    //Cube *diffusionCube = new Cube(psfVals,"rw");
    //QScopedPointer<Cube,TemporaryCubeDeleter> dCube (diffusionCube );
    Cube *diffusionCube = p1.SetOutputCube(psfVals,att2,ns,nl);

    //p1.SetOutputCube(psfVals,att2,ns,nl,nb);

    p1.SetBoxcarSize(g_size,g_size);


    g_psfFilter = setPSFFilter(g_size, g_A,g_sigma, g_alpha,g_N,binning);

    try{
    p1.StartProcess(psfCorrectionBoxcar);        //Determine the diffusion model.
    }

    catch(IException &ie){

      cout << ie.toString() << endl;

    }

    p1.EndProcess();






   //Apply the PSF correction
    ProcessByLine p2;
    CubeAttributeInput att12;
    CubeAttributeOutput att13;

    //The diffusion model
    p2.SetInputCube(psfVals,att12);
    //The original output cube.



    p2.SetInputCube(fname,att1);

    p2.SetOutputCube("PSFCORRECTED");



    try{
    p2.StartProcess(psfCorrection);
    }

    catch(IException &ie){

      cout << ie.toString() << endl;

    }    


    p2.EndProcess();

    try {
      QFile tempFile(psfVals);
      tempFile.remove();
    }
    catch(IException &ie){

      cout << ie.toString() << endl;

    }




  //}




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
  //tjw:  Wrong directory
  //QString filename = "$hayabusa/calibration/amica/flatfield/";

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

void psfCorrectionBoxcar(Buffer &in, double &result) {



    result = 0;

    for (int i =0; i < in.size(); i++) {

      if(!IsSpecial(in[i])) {

        result+=in[i]*g_psfFilter[i];
      }


    }


    //if(result != Isis::Null) {
      result *= g_weight;

    //}



}



void psfCorrection(vector<Buffer *> &in, vector<Buffer *> &out) {


  Buffer& nopsf    = *in[1];
  Buffer& psfVals =  *in[0];
  Buffer& imageOut  = *out[0];

  for (int i = 0; i < nopsf.size(); i++) {
    if (!IsSpecial(psfVals[i])) {
      imageOut[i] = nopsf[i]-psfVals[i];

    }
    else {
      imageOut[i] = nopsf[i];
    }
  }

}


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


  //Load the hot pixels into a vector


  for (int i = 0; i< hotPixels.keywords(); i++ ){

    QString samp(hotPixels[i][0]);
    QString line(hotPixels[i][1]);

    Pixel *hotpix = new Pixel(samp.toInt(),line.toInt(),1,0);
    hotPixelVector.append(*hotpix);
  }


  //Load linearity variables

  g_Gamma = Linearity["Gamma"];
  g_Gamma = 1-g_Gamma;

  g_L0 = Linearity["L"][0].toDouble();
  g_L1 = Linearity["L"][1].toDouble();

  cout << "Gamma = " << setprecision(20)  << g_Gamma << endl;
  cout << "<L0,L1> = "<< "<" << g_L0 << "," << g_L1 <<">" << endl;
  //Load Smear Removal Variables

  g_Tvct = Smear["Tvct"];

  cout <<"g_Tvct = " << g_Tvct << endl;

  //Load DarkCurrent variables


  g_d0 = DarkCurrent["D"][0].toDouble();
  g_d1 = DarkCurrent["D"][1].toDouble();

  cout << "<D0,D1> = "<< "<" << g_d0 << "," << g_d1 <<">" << endl;


  //Load Bias variables

  g_b0 = Bias["B"][0].toDouble();
  g_b1 = Bias["B"][1].toDouble();
  g_b2 = Bias["B"][2].toDouble();

  cout << "<B0,B1,B2> = "<< "<" << g_b0 << "," << g_b1 << "," << g_b2 << ">" << endl;

  g_launchTimeStr=QString(Bias["launchTime"]);

  cout <<"Launch Time = " << g_launchTimeStr << endl;
  //static iTime g_launchTime("2003-05-09T04:29:25");
  g_launchTime =g_launchTimeStr;



  //Load the PSF constants.  These come from
  //Ishiguro, 2014 ('Scattered light correction of Hayabusa/AMICA data and
  //quantitative spectral comparisons of Itokawa')

   g_alpha = psfFocused[g_filter.toLower()];

   cout <<"g_alpha = " << g_alpha << endl;

   for (int i =0; i < g_N; i++) {

     g_sigma[i] = psfDiffuse["sigma"][i].toDouble();
     g_A[i] = psfDiffuse[g_filter.toLower()][i].toDouble();

   }


  //Load the Solar Flux for the specific filter


  g_solarFlux=solar[g_filter.toLower()];

  cout <<"g_solarFlux = " <<g_solarFlux << endl;




}


/**
 * @brief Apply radiometric correction to each line of an AMICA image
 *  
 *  
 * @author 2016-03-30 Kris Becker
 * 
 * @param in   Raw image and flat field
 * @param out  Radometrically corrected image
 */
void Calibrate(vector<Buffer *>& in, vector<Buffer *>& out) {


  Buffer& imageIn   = *in[0];
  Buffer& flatField = *in[1];
  Buffer& imageOut  = *out[0];
  double obsStartTime;
  double tsecs;
  double tdays;
  double bias=0;
  double smear = 0;
  double t1;
  double c1;
  int pixelsToNull = 12/binning;

  int currentSample = imageIn.Sample();



  if(currentSample <= pixelsToNull) {
    for (int i = 0; i < imageIn.size(); i++ ) {

      imageOut[i]=Isis::Null;

    }
    return;
  }

 else if ( currentSample >= ( imageIn.size() -pixelsToNull ) ) {
      for (int i = 0; i < imageIn.size(); i++ ) {

        imageOut[i]=Isis::Null;

      }

    return;
  }

  //iterate over the line space


  for (int i = 0; i < imageIn.size(); i++) {



    // Check for special pixel in input image and pass through
    if (Isis::IsSpecial(imageIn[i])) {
      imageOut[i] = imageIn[i];
      continue;
    }

    // 1) BIAS Removal - Only needed if not on-board corrected
    if ( nsubImages <= 1 ) {

      scs2e_c(g_HayabusaNaifCode,g_startTime.toAscii().data(), &obsStartTime);
      tsecs = obsStartTime - g_launchTime.Et();
      tdays = tsecs/86400;
      bias = g_b0+g_b1*tdays+g_b2*(tdays*tdays);
      imageOut[i]= imageIn[i] - bias;



    }

    // 2) LINEARITY Correction - always done

      imageOut[i] = pow(imageIn[i],g_Gamma) +g_L0*imageIn[i]*exp(g_L1*imageIn[i]);

    // 3) DARK Current - Currently negligible and removed


      //imageOut[i] = imageIn[i]-g_darkCurrent;


    // 4) HOT Pixel Removal

      if (binning < 2) {


          for(int j =0;j < hotPixelVector.size(); j++) {


              if (hotPixelVector[j].sample() == currentSample && hotPixelVector[j].line() == i )
              {
                  imageOut[i] = Isis::Null;

              }
          }

      }

    // 5) READOUT Smear Removal - Not needed if on-board corrected
    if ( nsubImages <= 1 ) {


      if(binning >=2) {

        smear =0;

        t1 = g_timeRatio/imageIn.size() ;


        int b = binning;

        c1 = 1.0/ (1.0+t1*((b-1.0)/(2.0*b)) ) ;



        for (int j = 0; j < imageIn.size(); j++ ) {
           smear +=   t1*imageIn[j];
        }


        imageOut[i]=c1*(imageIn[i]-smear);

      }

      // If we are not binning, then it is OK to apply Formula 7 of Ishiguro(2007)
      // to calculate smear

      else {

        smear=0;

        t1= g_timeRatio*(1.0/imageIn.size());
        //cout << "t1 = " << t1 << endl;
        // sum over the line space for each sample
        for (int j = 0; j< imageIn.size(); j++) {

          smear+= t1*(imageIn[j]);
        }

        //cout << smear << endl;

        imageOut[i] = (imageIn[i] - smear);


      }

    }



    // 6) FLATFIELD correction
    //  Check for any special pixels in the flat field (unlikely)

    if (Isis::IsSpecial(flatField[i])) {
      imageOut[i] = Isis::Null;
      //continue;
    }
    else {

       imageOut[i] /= flatField[i];
    }



    // 7) I/F Conversion


    if (g_iofCorrection) {

          imageOut[i] *= g_iof;

       }


    //imageOut[i]= imageIn[i]-g_darkCurrent;






  }

  return;
}

