#include "Isis.h"
#include "SpecialPixel.h"
#include "CubeAttribute.h"
#include "Cube.h"
#include "Camera.h"
#include "Constants.h"
#include "Statistics.h"
#include "ProcessByBrick.h"
#include "Brick.h"
#include "iTime.h"
#include <vector>

using namespace Isis;
using namespace std;

#define POLAR_MODE_SAMPLES 1024
#define NO_POLAR_MODE_SAMPLES 704
#define BW_BANDS 1
#define VIS_LINES 14
#define COLOR_BANDS 5
#define UV_SAMPLES 128
#define UV_LINES 4
#define UV_BANDS 2
#define KM_PER_AU 149597871

void ResetGlobals ();
void Calibrate ( Buffer &in, Buffer &out );
void CopyCubeIntoArray ( string &fileString, vector<double> &data );
double min ( double a, double b );

void GetDark(string fileString, double temp, vector<double> & data1, vector<double> & data2, double & temp1, double & temp2, string & file1, string & file2);

vector<double> g_iofResponsivity;
vector<double> g_radianceResponsivity;

bool g_dark = true, g_flatfield = true, g_radiometric = true, g_iof = true, g_specpix = true;

double g_exposure; // Exposure duration
double g_solarDistance = 1.01; // average distance in [AU]
double g_startTemperature, g_endTemperature;
double g_temp1, g_temp2;

int g_numFrames;

vector<int> g_bands;

vector<double> g_darkCube1, g_darkCube2, g_flatCube, g_specpixCube;

void IsisMain () {
    ResetGlobals();
    UserInterface &ui = Application::GetUserInterface();

    ProcessByBrick p;
    Cube *icube = p.SetInputCube("FROM");

    // Make sure it is a WAC cube
    Isis::PvlGroup &inst = icube->Label()->FindGroup("Instrument", Pvl::Traverse);
    iString instId = (string) inst["InstrumentId"];
    instId.UpCase();
    if (instId != "WAC-VIS" && instId != "WAC-UV") {
        string msg = "This program is intended for use on LROC WAC images only. [";
        msg += icube->Filename() + "] does not appear to be a WAC image.";
        throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    // And check if it has already run through calibration
    if (icube->Label()->FindObject("IsisCube").HasGroup("Radiometry")) {
        string msg = "This image has already been calibrated";
        throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    if (icube->Label()->FindObject("IsisCube").HasGroup("AlphaCube")) {
        string msg = "This application can not be run on any image that has been geometrically transformed (i.e. scaled, rotated, sheared, or reflected) or cropped.";
        throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    g_dark = ui.GetBoolean("DARK");
    g_flatfield = ui.GetBoolean("FLATFIELD");
    g_radiometric = ui.GetBoolean("RADIOMETRIC");
    g_iof = (ui.GetString("RADIOMETRICTYPE") == "IOF");
    g_specpix = ui.GetBoolean("SPECIALPIXELS");

    // Determine the dark/flat files to use
    iString offset = (string) inst["BackgroundOffset"];
    iString mode = (string) inst["Mode"];
    iString instModeId = (string) inst["InstrumentModeId"];
    instModeId.UpCase();

    if (instModeId == "COLOR" && (string) inst["InstrumentId"] == "WAC-UV")
        instModeId = "UV";
    else if (instModeId == "VIS")
        instModeId = "COLOR";

    g_startTemperature = (double) inst["BeginTemperatureFpa"];
    g_endTemperature = (double) inst["EndTemperatureFpa"];

    g_numFrames = (int) inst["NumFramelets"];

    vector<string> darkFiles;
    ui.GetAsString("DARKFILE", darkFiles);
    iString flatFile = ui.GetAsString("FLATFIELDFILE");
    iString radFile = ui.GetAsString("RADIOMETRICFILE");
    iString specpixFile = ui.GetAsString("SPECIALPIXELSFILE");

    // Figure out which bands are input
    for (int i = 1; i <= icube->Bands(); i++) {
        g_bands.push_back(icube->PhysicalBand(i));
    }

    Isis::PvlGroup &bandBin = icube->Label()->FindGroup("BandBin", Pvl::Traverse);
    iString filter = (string) bandBin["Center"][0];

    if (g_dark) {
        if (darkFiles.size() == 0 || darkFiles[0] =="Default" || darkFiles[0].length() == 0) {
            darkFiles.resize(2);
            double temp = (double) inst["MiddleTemperatureFpa"];
            string darkFile = "$lro/calibration/wac_darks/WAC_" + instModeId;
            if (instModeId == "BW")
                darkFile += "_" + filter + "_Mode" + mode;
            darkFile += "_Offset" + offset + "_*C_Dark.????.cub";
            GetDark (darkFile, temp, g_darkCube1, g_darkCube2, g_temp1, g_temp2, darkFiles[0], darkFiles[1]);
        }
        else if (darkFiles.size() == 1) {
            CopyCubeIntoArray(darkFiles[0], g_darkCube1);
            g_temp1 = 0.0;
            CopyCubeIntoArray(darkFiles[0], g_darkCube2);
            g_temp2 = g_temp1;
        }
        else {
            CopyCubeIntoArray(darkFiles[0], g_darkCube1);
            int index = darkFiles[0].find_last_of("_");
            g_temp1 = iString(darkFiles[0].substr( darkFiles[0].find_last_of("_", index-1), index)).ToDouble();
            CopyCubeIntoArray(darkFiles[1], g_darkCube2);
            index = darkFiles[1].find_last_of("_");
            g_temp2 = iString(darkFiles[1].substr( darkFiles[1].find_last_of("_", index-1), index)).ToDouble();
        }
    }

    if (g_flatfield) {
        if (flatFile.Equal("Default") || flatFile.length() == 0) {
            flatFile = "$lro/calibration/WAC_" + instModeId;
            if (instModeId == "BW")
                flatFile += "_" + filter + "_Mode" + mode;
            flatFile += "_Flatfield.????.cub";
        }
        CopyCubeIntoArray(flatFile, g_flatCube);
    }

    PvlKeyword responsivity;

    if (g_radiometric) {
        if (radFile.Equal("Default") || radFile.length() == 0)
            radFile = "$lro/calibration/WAC_RadiometricResponsivity.????.pvl";

        Filename radFilename(radFile);
        if ((radFilename.Expanded()).find("?") != string::npos)
            radFilename.HighestVersion();
        if (!radFilename.Exists()) {
            string msg = radFile + " does not exist.";
            throw iException::Message(iException::User, msg, _FILEINFO_);
        }

        Pvl radPvl(radFilename.Expanded());

        if (g_iof) {
            responsivity = radPvl["IOF_" + instModeId];
            for (int i = 0; i < responsivity.Size(); i++)
                g_iofResponsivity.push_back(responsivity[i]);

            try {
                iTime startTime((string) inst["StartTime"]);
                double etStart = startTime.Et();
                // Get the distance between the Moon and the Sun at the given time in
                // Astronomical Units (AU)
                string bspKernel1 = p.MissionData("lro", "/kernels/tspk/moon_pa_de421_1900-2050.bpc", false);
                string bspKernel2 = p.MissionData("lro", "/kernels/tspk/de421.bsp", false);
                furnsh_c(bspKernel1.c_str());
                furnsh_c(bspKernel2.c_str());
                string pckKernel1 = p.MissionData("base", "/kernels/pck/pck?????.tpc", true);
                string pckKernel2 = p.MissionData("lro", "/kernels/pck/moon_080317.tf", false);
                string pckKernel3 = p.MissionData("lro", "/kernels/pck/moon_assoc_me.tf", false);
                furnsh_c(pckKernel1.c_str());
                furnsh_c(pckKernel2.c_str());
                furnsh_c(pckKernel3.c_str());
                double sunpos[6], lt;
                spkezr_c("sun", etStart, "MOON_ME", "LT+S", "MOON", sunpos, &lt);
                g_solarDistance = vnorm_c(sunpos) / KM_PER_AU;
                unload_c(bspKernel1.c_str());
                unload_c(bspKernel2.c_str());
                unload_c(pckKernel1.c_str());
                unload_c(pckKernel2.c_str());
                unload_c(pckKernel3.c_str());
            }
            catch (iException &e) {
                string msg = "Can not find necessary SPICE kernels for converting to IOF";
                throw iException::Message(iException::User, msg, _FILEINFO_);
            }
        }
        else {
            responsivity = radPvl["Radiance_" + instModeId];
            for (int i = 0; i < responsivity.Size(); i++)
                g_radianceResponsivity.push_back(responsivity[i]);
        }
    }

    if (g_specpix) {
        if (specpixFile.Equal("Default") || specpixFile.length() == 0) {
            specpixFile = "$lro/calibration/WAC_" + instModeId;
            if (instModeId == "BW")
                specpixFile += "_" + filter + "_Mode" + mode;
            specpixFile += "_SpecialPixels.????.cub";
        }
        CopyCubeIntoArray(specpixFile, g_specpixCube);
    }

    if (instModeId == "BW") {
        if (mode == "1" || mode == "0")
            p.SetBrickSize(NO_POLAR_MODE_SAMPLES, VIS_LINES, (int)min(BW_BANDS, g_bands.size()));
        else
            p.SetBrickSize(POLAR_MODE_SAMPLES, VIS_LINES, (int)min(BW_BANDS, g_bands.size()));
    }
    else if (instModeId == "COLOR") {
        p.SetBrickSize(NO_POLAR_MODE_SAMPLES, VIS_LINES, (int)min(COLOR_BANDS, g_bands.size()));
    }
    else if (instModeId == "UV") {
        p.SetBrickSize(UV_SAMPLES, UV_LINES, (int)min(UV_BANDS, g_bands.size()));
    }

    g_exposure = inst["ExposureDuration"];

    Cube *ocube = p.SetOutputCube("TO");
    p.StartProcess(Calibrate);

    // Add an output group with the appropriate information
    PvlGroup calgrp("Radiometry");
    if (g_dark) {
        PvlKeyword darks("DarkFiles");
        darks.AddValue(darkFiles[0]);
        if (darkFiles.size() > 1)
            darks.AddValue(darkFiles[1]);
        calgrp += darks;
    }
    if (g_flatfield)
        calgrp += PvlKeyword("FlatFile", flatFile);
    if (g_radiometric) {
        if (g_iof)
            calgrp += PvlKeyword("RadiometricType", "IOF");
        else
            calgrp += PvlKeyword("RadiometricType", "AbsoluteRadiance");

        responsivity.SetName("ResponsivityValues");
        calgrp += responsivity;
        calgrp += PvlKeyword("SolarDistance", g_solarDistance);
    }
    ocube->PutGroup(calgrp);

    p.EndProcess();
}

void ResetGlobals () {
    g_iofResponsivity.clear();
    g_radianceResponsivity.clear();

    g_dark = true;
    g_flatfield = true;
    g_radiometric = true;
    g_iof = true;
    g_specpix = true;

    g_bands.clear();

    g_exposure = 1.0; // Exposure duration
    g_solarDistance = 1.01; // average distance in [AU]

    g_temp1 = 0.0;
    g_temp2 = 0.0;

    g_numFrames = 0;

    g_darkCube1.clear();
    g_darkCube2.clear();
    g_flatCube.clear();
    g_specpixCube.clear();
}

// Calibrate each framelet
void Calibrate ( Buffer &inCube, Buffer &outCube ) {

    int frame = inCube.Line() / inCube.LineDimension();

    for (int i = 0; i < outCube.size(); i++)
        outCube[i] = inCube[i];

    if (g_dark) {
        for (int i = 0; i < outCube.size(); i++) {
            int offset = inCube.size() * (int) min(frame, (g_darkCube1.size()-1) / inCube.size());

            // Calculate the temperature for the current frame
            double temp = (g_endTemperature - g_startTemperature)/g_numFrames * frame + g_startTemperature;

            // Interpolate between the two darks with the current temperature
            if (IsSpecial(g_darkCube1[offset + i]) || IsSpecial(g_darkCube2[offset + i]))
                outCube[i] = Isis::Null;
            else {
              if (g_temp1 != g_temp2)
                outCube[i] -= (g_darkCube1[offset + i] - g_darkCube2[offset + i])/(g_temp1-g_temp2) * (temp - g_temp2) + g_darkCube2[offset + i];
              else
                outCube[i] -= g_darkCube1[offset + i];
}
        }
    }

    if (g_flatfield) {
        int offset = inCube.size() * (int) min(frame, (g_flatCube.size()-1) / inCube.size());

        for (int i = 0; i < outCube.size(); i++)
            if (g_flatCube[i] <= 0 || IsSpecial(g_flatCube[offset + i]))
                outCube[i] = Isis::Null;
            else
                outCube[i] /= g_flatCube[offset + i];
    }

    if (g_radiometric) {
        for (int i = 0; i < outCube.size(); i++) {
            if (IsSpecial(outCube[i]))
                outCube[i] = Isis::Null;
            else {
                outCube[i] /= g_exposure;
                if (g_iof)
                    outCube[i] *= pow(g_solarDistance, 2) / g_iofResponsivity[outCube.Band() - 1];
                else
                    outCube[i] /= g_radianceResponsivity[outCube.Band() - 1];
            }
        }
    }

    if (g_specpix) {
        for (int i = 0; i < outCube.size(); i++) {
            int offset = inCube.size() * (int) min(frame, (g_specpixCube.size()-1) / inCube.size());

            if (IsSpecial(g_specpixCube[offset + i]))
                outCube[i] = g_specpixCube[offset + i];
        }
    }

}

void CopyCubeIntoArray ( string &fileString, vector<double> & data ) {
    Cube cube;
    Filename filename(fileString);
    if ((filename.Expanded()).find("?") != string::npos)
        filename.HighestVersion();
    if (!filename.Exists()) {
        string msg = fileString + " does not exist.";
        throw iException::Message(iException::User, msg, _FILEINFO_);
    }
    cube.Open(filename.Expanded());
    Brick brick(cube.Samples(), cube.Lines(), cube.Bands(), cube.PixelType());
    brick.SetBasePosition(1, 1, 1);
    cube.Read(brick);

    data.clear();

    for (unsigned int b = 0; b < g_bands.size(); b++) {
        for (int l = 1; l <= brick.LineDimension(); l++)
            for (int s = 1; s <= brick.SampleDimension(); s++) {
                data.push_back(brick.at(brick.Index(s, l, g_bands[b])));
            }
    }

    fileString = filename.Expanded();
}

double min ( double a, double b ) {
    if (a < b)
        return a;
    return b;
}

void GetDark(string fileString, double temp, vector<double> & data1, vector<double> & data2, double & temp1, double & temp2, string & file1, string & file2) {
// Find the beginning and end of the "?"s in the versioned filename
   Filename filename(fileString);
   string absolutePath = filename.Expanded();
   string basename = Filename(filename.Basename()).Basename(); // We do it twice to remove the ".????.cub"

    unsigned int index = basename.find_first_of("*");
    unsigned int charsAfterVersion = basename.length() - index - 1;

   vector<double> temperatures;

   QDir dir( (QString)(iString)(absolutePath.substr(0, absolutePath.find_last_of("/"))) );

    // Loop through all files in the dir and see if they match our name
    for (unsigned int indx=0; indx < dir.count(); indx++) {

      string file = Filename( Filename(dir[indx].toStdString()).Basename()).Basename(); // We do it twice to remove ".????.cub"
      bool leftSide = file.substr(0, index) == basename.substr(0, index);
      bool rightSide = ((int)file.length()-(int)charsAfterVersion) >= 0;
      if (rightSide) {
        rightSide = rightSide &&
        (file.substr(file.length()-charsAfterVersion) == basename.substr(index+1));
      }

      if (leftSide && rightSide) {

        Isis::iString version = file.substr(index, file.length()-charsAfterVersion-index);

        if ((version.length() > 0) &&
            (version.find_first_not_of("0123456789.-") == string::npos)) {
          // Make sure it isn't already included, otherwise add it
          bool add = true;
          for (unsigned int j=0; j<temperatures.size(); j++)
              if (temperatures[j] == version.ToDouble())
                  add = false;
          if (add)
              temperatures.push_back( version.ToDouble());
        }
      }
    }

  // Now that we have all the available temperatures, we need to find the nearest two temperatures and interpolate (or extrapolate) between them
  if (temperatures.size() == 0) {
    string msg = "No Dark files exist for these image options [" + basename + "]";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }
  else if ( temperatures.size() > 2) {
    if (abs(temp - temperatures[0]) < abs(temp - temperatures[1])){
        temp1 = temperatures[0];
        temp2 = temperatures[1];
    }
    else {
        temp1 = temperatures[1];
        temp2 = temperatures[0];
    }
    for (unsigned int i=2; i<temperatures.size(); i++){
        if (abs(temp - temperatures[i]) < abs(temp - temp1)) {
            temp2 = temp1;
            temp1 = temperatures[i];
        }
        else if (abs(temp - temperatures[i]) < abs(temp - temp2)) {
            temp2 = temperatures[i];
        }
    }
  }
  else {
    temp1=temperatures[0];
    temp2=temp1;
  }

    index = fileString.find_first_of("*");

    file1 = fileString.substr(0, index) + iString((int)temp1) + fileString.substr(index+1);
    CopyCubeIntoArray ( file1, data1 );
    file2 = fileString.substr(0, index) + iString((int)temp2) + fileString.substr(index+1);
    CopyCubeIntoArray ( file2, data2 );
 
}
