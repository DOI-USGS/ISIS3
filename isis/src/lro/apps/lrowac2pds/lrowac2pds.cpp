#include "Isis.h"
#include <cstdio>
#include <string>
#include <QRegExp>
#include "FileList.h"
#include "Brick.h"
#include "OriginalLabel.h"
#include "Brick.h"
#include "History.h"
#include "Stretch.h"
#include "iTime.h"
#include "ProcessExport.h"
#include "PvlTranslationManager.h"
#include "PvlFormatPds.h"
#include "md5.h"
#include "md5wrapper.h"
#include "OriginalLabel.h"
#include <sstream>

#define COLOR_SAMPLES 704
#define VIS_SAMPLES 704
#define UV_SAMPLES 128
#define BW_SAMPLES 1024
#define VIS_LINES 14
#define UV_LINES 4

using namespace std;
using namespace Isis;

vector<int> frameletLines;

void ResetGlobals ();
void mergeFramelets ();
string MD5Checksum ( string filename );
void OutputLabel ( std::ofstream &fout, Cube* cube, Pvl &pdsLab );
void CopyData ( std::ifstream &fin, std::ofstream &fout );

std::vector<int> padding;
int colorOffset = 0;
int inputCubeLines = 0;

// Output UV Files
Cube *uveven = NULL;
Cube *uvodd = NULL;

// Output VIS Files
Cube *viseven = NULL;
Cube *visodd = NULL;

Cube* out = NULL;

iString instrumentModeId = "";
iString productId = "";
iString g_productVersionId = "N/A";

string g_md5Checksum;

int numFramelets = 0;
int numSamples = 0;
int numLines = 0;
int numUVFilters = 0;
int numVisFilters = 0;

bool g_isIoF;

void IsisMain () {
    Pvl pdsLab;

    FileList list;
    UserInterface &ui = Application::GetUserInterface();
    list.read(ui.GetFileName("FROMLIST"));

    if (list.size() < 1) {
        string msg = "The list file [" + ui.GetFileName("FROMLIST") + "does not contain any data";
        throw IException(IException::User, msg, _FILEINFO_);
    }

    g_productVersionId = ui.GetString("VERSIONIDSTRING");

    for (int i = 0; i < list.size(); i++) {

        Pvl tempPvl;
        tempPvl.Read(list[i].toString());

        OriginalLabel origLab(list[i].toString());
        pdsLab = origLab.ReturnLabels();

        iString prodId = pdsLab["PRODUCT_ID"][0];
        if (productId == "")
            productId = prodId;

        if (productId != prodId) {
            string msg = "This program is intended for use on a single LROC WAC images only.";
            msg += "The ProductIds do not match.";
            throw IException(IException::User, msg, _FILEINFO_);
        }

        Isis::PvlGroup &inst = tempPvl.FindGroup("Instrument", Pvl::Traverse);
        iString instId = (string) inst["InstrumentId"];
        iString framelets = (string) inst["Framelets"];
        iString numFrames = (int) inst["NumFramelets"];

        if (instId != "WAC-VIS" && instId != "WAC-UV") {
            string msg = "This program is intended for use on LROC WAC images only. [";
            msg += list[i].toString() + "] does not appear to be a WAC image.";
            throw IException(IException::User, msg, _FILEINFO_);
        }

        iString instModeId = (string) inst["InstrumentModeId"];
        if (instrumentModeId == "")
            instrumentModeId = instModeId;
        if (numFramelets == 0)
            numFramelets = numFrames;
        g_isIoF = tempPvl.FindGroup("Radiometry", Pvl::Traverse).FindKeyword("RadiometricType")[0].Equal("IOF");

        if (instId == "WAC-VIS" && framelets == "Even") {
            viseven = new Cube();
            viseven->open(list[i].toString());
        }
        else if (instId == "WAC-VIS" && framelets == "Odd") {
            visodd = new Cube();
            visodd->open(list[i].toString());
        }
        if (instId == "WAC-UV" && framelets == "Even") {
            uveven = new Cube();
            uveven->open(list[i].toString());
        }
        else if (instId == "WAC-UV" && framelets == "Odd") {
            uvodd = new Cube();
            uvodd->open(list[i].toString());
        }
    }

    // Determine our band information based on
    // INSTRUMENT_MODE_ID - FILTER_NUMBER is
    // only going to be used for BW images
    if (instrumentModeId == "COLOR") {
        numUVFilters = 2;
        numVisFilters = 5;

        numSamples = COLOR_SAMPLES;
    }
    else if (instrumentModeId == "VIS") {
        numUVFilters = 0;
        numVisFilters = 5;

        numSamples = VIS_SAMPLES;
    }
    else if (instrumentModeId == "UV") {
        numUVFilters = 2;
        numVisFilters = 0;

        numSamples = UV_SAMPLES;
    }
    else if (instrumentModeId == "BW") {
        numUVFilters = 0;
        numVisFilters = 1;

        numSamples = BW_SAMPLES;
    }

    numLines = numFramelets * (UV_LINES * numUVFilters + VIS_LINES * numVisFilters);

    out = new Cube();
    out->setDimensions(numSamples, numLines, 1);
    out->setPixelType(Isis::Real);

    FileName mergedCube = FileName::createTempFile(
        "$TEMPORARY/" + FileName(ui.GetFileName("TO")).baseName() + ".cub");
    out->create(mergedCube.expanded().c_str());

    mergeFramelets();

    /*

     FileName outFile(ui.GetFileName("TO", "img"));
     string outFileName(outFile.expanded());
     ofstream oCube(outFileName.c_str());
     p.OutputLabel(oCube);
     p.StartProcess(oCube);
     oCube.close();
     p.EndProcess();

     */

    out->close();
    delete out;
    out = NULL;

    if (uveven) {
        uveven->close();
        delete uveven;
        uveven = NULL;
    }

    if (uvodd) {
        uvodd->close();
        delete uvodd;
        uvodd = NULL;
    }

    if (viseven) {
        viseven->close();
        delete viseven;
        viseven = NULL;
    }

    if (visodd) {
        visodd->close();
        delete visodd;
        visodd = NULL;
    }

    // Export data

    ProcessExport pe;

    // Setup the input cube
    Cube *inCube = pe.SetInputCube(mergedCube.expanded(), CubeAttributeInput());

    pe.SetOutputType(Isis::Real);
    pe.SetOutputEndian(Isis::Lsb);

    pe.SetOutputRange(Isis::VALID_MIN4, Isis::VALID_MAX4);

    pe.SetOutputNull(Isis::NULL4);
    pe.SetOutputLrs(Isis::LOW_REPR_SAT4);
    pe.SetOutputLis(Isis::LOW_INSTR_SAT4);
    pe.SetOutputHis(Isis::HIGH_INSTR_SAT4);
    pe.SetOutputHrs(Isis::HIGH_REPR_SAT4);

    FileName tempFile = FileName::createTempFile(
        "$TEMPORARY/" + FileName(ui.GetFileName("TO")).baseName() + ".temp");
    string tempFileName(tempFile.expanded());
    ofstream temporaryFile(tempFileName.c_str());

    pe.StartProcess(temporaryFile);
    temporaryFile.close();

    // Calculate MD5 Checksum
    g_md5Checksum = MD5Checksum(tempFileName);

    FileName outFile(ui.GetFileName("TO"));
    string outFileName(outFile.expanded());
    ifstream inFile(tempFileName.c_str());
    ofstream pdsFile(outFileName.c_str());

    // Output the label
    OutputLabel(pdsFile, inCube, pdsLab);

    // Then copy the image data
    CopyData(inFile, pdsFile);

    pdsFile.close();

    pe.EndProcess();

    remove((mergedCube.expanded()).c_str());
    remove(tempFileName.c_str());
    return;
}

void ResetGlobals () {
    colorOffset = 0;
    frameletLines.clear();

    uveven = NULL;
    uvodd = NULL;
    viseven = NULL;
    visodd = NULL;

    out = NULL;

    instrumentModeId = "";
    productId = "";
    g_md5Checksum = "";

    numFramelets = 0;
    numSamples = 0;
    numLines = 0;
    numUVFilters = 0;
    numVisFilters = 0;

    g_isIoF = false;
}

//! Merges each of the individual WAC framelets into the right place
void mergeFramelets () {
    Brick *uvevenManager = NULL, *uvoddManager = NULL, *visevenManager = NULL, *visoddManager = NULL;

    if (numUVFilters > 0) {
        uvevenManager = new Brick(*uveven, UV_SAMPLES, UV_LINES, numUVFilters);
        uvoddManager = new Brick(*uvodd, UV_SAMPLES, UV_LINES, numUVFilters);

        uvevenManager->begin();
        uvoddManager->begin();
    }
    if (numVisFilters > 0) {
        visevenManager = new Brick(*viseven, numSamples, VIS_LINES, numVisFilters);
        visoddManager = new Brick(*visodd, numSamples, VIS_LINES, numVisFilters);

        visevenManager->begin();
        visoddManager->begin();
    }

    Brick outManager(*out, numSamples, UV_LINES * numUVFilters + VIS_LINES * numVisFilters, 1);
    outManager.begin();

    // For each frame
    for (int f = 0; f < numFramelets; f++) {
        // write out the UV first
        if (numUVFilters > 0) {
            uveven->read(*uvevenManager);
            uvodd->read(*uvoddManager);

            int pad = (numSamples - UV_SAMPLES) / 2;
            for (int line = 0; line < numUVFilters * UV_LINES; line++) {
                int offset = numSamples * line;
                // left padding
                for (int i = 0; i < pad; i++)
                    outManager[offset + i] = Isis::Null;
                for (int i = 0; i < UV_SAMPLES; i++) {
                    int index = line * UV_SAMPLES + i;
                    if (f % 2 == 0)
                        outManager[i + offset + pad] = uvoddManager->at(index);
                    else
                        outManager[i + offset + pad] = uvevenManager->at(index);
                }
                // right padding
                for (int i = 0; i < pad; i++)
                    outManager[i + offset + pad + UV_SAMPLES] = Isis::Null;
            }
            uvevenManager->next();
            uvoddManager->next();
        }
        // then the vis
        if (numVisFilters > 0) {
            viseven->read(*visevenManager);
            visodd->read(*visoddManager);

            int offset = numUVFilters * UV_LINES * numSamples;
            for (int i = 0; i < numVisFilters * VIS_LINES * numSamples; i++) {
                if (f % 2 == 0)
                    outManager[i + offset] = visoddManager->at(i);
                else
                    outManager[i + offset] = visevenManager->at(i);
            }

            visevenManager->next();
            visoddManager->next();
        }

        out->write(outManager);
        outManager.next();
    }
}

string MD5Checksum ( string filename ) {
    md5wrapper md5;
    std::string checkSum = md5.getHashFromFile(filename.c_str());
    return checkSum;
}

void OutputLabel ( std::ofstream &fout, Cube* cube, Pvl &labelPvl ) {
    //Pvl to store the labels
    Pvl outLabel;
    PvlFormatPds *p_formatter = new PvlFormatPds("$lro/translations/pdsExportRootGen.typ");
    labelPvl.SetFormat(p_formatter);
    labelPvl.SetTerminator("END");
    //Set up the directory where the translations are
    PvlGroup dataDir(Preference::Preferences().FindGroup("DataDirectory"));
    iString transDir = (string) dataDir["Lro"] + "/translations/";

    stringstream stream;
    iString pdsLabel = "";

    //Translate the Original Pds Label
    FileName transFile(transDir + "lrowacPdsLabelExport.trn");
    PvlTranslationManager labelXlator(labelPvl, transFile.expanded());
    labelXlator.Auto(outLabel);

    // Copy any Translation changes over
    for (int i = 0; i < outLabel.Keywords(); i++) {
        bool hasUnit = false;
        string unit = "";
        if (labelPvl[outLabel[i].Name()].Unit() != "") {
            hasUnit = true;
            unit = labelPvl[outLabel[i].Name()].Unit();
        }
        bool hasComment = false;
        string comment = "";
        if (labelPvl[outLabel[i].Name()].Comments() > 0) {
            hasComment = true;
            comment = labelPvl[outLabel[i].Name()].Comment(0);
        }
        labelPvl[outLabel[i].Name()] = outLabel[i];

        if (hasUnit)
            labelPvl[outLabel[i].Name()].SetUnits(unit);
        if (hasComment)
            labelPvl[outLabel[i].Name()].AddComment(comment);
    }

    //Update the product ID
    labelPvl["PRODUCT_ID"][0].replace(11, 1, "C");

    // Update the product creation time
    labelPvl["PRODUCT_CREATION_TIME"].SetValue(iTime::CurrentGMT());

    labelPvl["PRODUCT_VERSION_ID"].SetValue(g_productVersionId);

    // Update the "IMAGE" Object
    PvlObject &imageObject = labelPvl.FindObject("IMAGE");
    imageObject.Clear();
    imageObject += PvlKeyword("LINES", cube->getLineCount());
    imageObject += PvlKeyword("LINE_SAMPLES", cube->getSampleCount());
    imageObject += PvlKeyword("SAMPLE_BITS", 32);
    imageObject += PvlKeyword("SAMPLE_TYPE", "PC_REAL");
    imageObject += PvlKeyword("VALID_MINIMUM", "16#FF7FFFFA#");
    imageObject += PvlKeyword("NULL", "16#FF7FFFFB#");
    imageObject += PvlKeyword("LOW_REPR_SATURATION", "16#FF7FFFFC#");
    imageObject += PvlKeyword("LOW_INSTR_SATURATION", "16#FF7FFFFD#");
    imageObject += PvlKeyword("HIGH_INSTR_SATURATION", "16#FF7FFFFE#");
    imageObject += PvlKeyword("HIGH_REPR_SATURATION", "16#FF7FFFFF#");
    if (g_isIoF == true)
        imageObject += PvlKeyword("UNIT", "\"I/F\"");
    else
    imageObject += PvlKeyword("UNIT", "W / (m**2 micrometer sr)");
    imageObject += PvlKeyword("MD5_CHECKSUM", (iString) g_md5Checksum);

    stream << labelPvl;

    int recordBytes = cube->getSampleCount();
    int labelRecords = (int) ((stream.str().length()) / recordBytes) + 1;

    labelPvl["RECORD_BYTES"] = recordBytes;
    labelPvl["FILE_RECORDS"] = (int) (cube->getLineCount() * 4 + labelRecords);
    labelPvl["LABEL_RECORDS"] = labelRecords;
    labelPvl["^IMAGE"] = (int) (labelRecords + 1);

    stream.str(std::string());

    stream << labelPvl;
    pdsLabel += stream.str();

    while (pdsLabel.length() < (unsigned int) (labelRecords * recordBytes)) {
        pdsLabel += '\n';
    }

    fout << pdsLabel;
    return;
}

void CopyData ( std::ifstream &fin, std::ofstream &fout ) {
    char line[704];
    while (!fin.eof()) {
        fin.read(line, 704);
        fout.write(line, fin.gcount());
    }
}

