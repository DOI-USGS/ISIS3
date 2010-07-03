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

vector<double> g_iofResponsivity;
vector<double> g_radianceResponsivity;

bool g_dark = true, g_flatfield = true, g_radiometric = true, g_iof = true, g_specpix = true;

double g_exposure; // Exposure duration
double g_solarDistance = 1.01; // average distance in [AU]

vector<int> g_bands;

vector<double> darkCube, flatCube, specpixCube;

void IsisMain () {
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

    iString darkFile = ui.GetAsString("DARKFILE");
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
        if (darkFile.Equal("Default") || darkFile.length() == 0) {
            darkFile = "$lro/calibration/WAC_" + instModeId;
            if (instModeId == "BW")
                darkFile += "_" + filter + "_Mode" + mode;
            darkFile += "_Offset" + offset + "_Dark.????.cub";
        }
        CopyCubeIntoArray(darkFile, darkCube);
    }

    if (g_flatfield) {
        if (flatFile.Equal("Default") || flatFile.length() == 0) {
            flatFile = "$lro/calibration/WAC_" + instModeId;
            if (instModeId == "BW")
                flatFile += "_" + filter + "_Mode" + mode;
            flatFile += "_Flatfield.????.cub";
        }
        CopyCubeIntoArray(flatFile, flatCube);
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
        CopyCubeIntoArray(specpixFile, specpixCube);
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
    if (g_dark)
        calgrp += PvlKeyword("DarkFile", darkFile);
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

    darkCube.clear();
    flatCube.clear();
    specpixCube.clear();
}

// Calibrate each framelet
void Calibrate ( Buffer &inCube, Buffer &outCube ) {

    int frame = inCube.Line() / inCube.LineDimension();

    for (int i = 0; i < outCube.size(); i++)
        outCube[i] = inCube[i];

    if (g_dark) {
        for (int i = 0; i < outCube.size(); i++) {
            int offset = inCube.size() * (int) min(frame, (darkCube.size()-1) / inCube.size());

            if (IsSpecial(darkCube[offset + i]))
                outCube[i] = Isis::Null;
            else
                outCube[i] -= darkCube[offset + i];
        }
    }

    if (g_flatfield) {
        int offset = inCube.size() * (int) min(frame, (flatCube.size()-1) / inCube.size());

        for (int i = 0; i < outCube.size(); i++)
            if (flatCube[i] <= 0 || IsSpecial(flatCube[offset + i]))
                outCube[i] = Isis::Null;
            else
                outCube[i] /= flatCube[offset + i];
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
            int offset = inCube.size() * (int) min(frame, (specpixCube.size()-1) / inCube.size());

            if (IsSpecial(specpixCube[offset + i]))
                outCube[i] = specpixCube[offset + i];
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
