#include "Isis.h"
#include "ProcessExport.h"
#include "ProcessByLine.h"
#include "Pvl.h"
#include "PvlTranslationManager.h"
#include "PvlFormatPds.h"
#include "OriginalLabel.h"
#include "iTime.h"
#include "md5.h"
#include "md5wrapper.h"
#include <sstream>

#define SCALING_FACTOR 32767

using namespace std;
using namespace Isis;

void ResetGlobals ();
void ProcessImage ( Buffer &in, Buffer &out );
string MD5Checksum ( string filename );
void OutputLabel ( std::ofstream &fout, Cube* cube );
void CopyData ( std::ifstream &fin, std::ofstream &fout );

string g_md5Checksum;

bool g_isIof;

iString g_productVersionId = "N/A";

void IsisMain () {
    ResetGlobals();
    UserInterface &ui = Application::GetUserInterface();

    g_productVersionId = ui.GetString("VERSIONIDSTRING");

    // Set the processing object

    ProcessByLine p;
    Cube *inCube = p.SetInputCube("FROM");

    g_isIof = inCube->getLabel()->FindGroup("Radiometry", Pvl::Traverse).FindKeyword("RadiometricType")[0].Equal("IOF");

    Filename scaledCube = ui.GetFilename("FROM");
    scaledCube.Temporary(Filename(ui.GetFilename("FROM")).Basename(), "cub");
    p.SetOutputCube(
        scaledCube.Expanded(), CubeAttributeOutput(),
        inCube->getSampleCount(), inCube->getLineCount(),
        inCube->getBandCount());

    // Scale image and calculate max and min values
    p.StartProcess(ProcessImage);
    p.EndProcess();

    ProcessExport pe;

    // Setup the input cube
    inCube = pe.SetInputCube(scaledCube.Expanded(), CubeAttributeInput());

    if (g_isIof) {
        pe.SetOutputType(Isis::SignedWord);
        pe.SetOutputEndian(Isis::Lsb);

        pe.SetOutputRange(Isis::VALID_MIN2, Isis::VALID_MAX2);

        pe.SetOutputNull(Isis::NULL2);
        pe.SetOutputLrs(Isis::LOW_REPR_SAT2);
        pe.SetOutputLis(Isis::LOW_INSTR_SAT2);
        pe.SetOutputHis(Isis::HIGH_INSTR_SAT2);
        pe.SetOutputHrs(Isis::HIGH_REPR_SAT2);
    }
    else {
        pe.SetOutputType(Isis::Real);
        pe.SetOutputEndian(Isis::Lsb);

        pe.SetOutputRange(Isis::VALID_MIN4, Isis::VALID_MAX4);

        pe.SetOutputNull(Isis::NULL4);
        pe.SetOutputLrs(Isis::LOW_REPR_SAT4);
        pe.SetOutputLis(Isis::LOW_INSTR_SAT4);
        pe.SetOutputHis(Isis::HIGH_INSTR_SAT4);
        pe.SetOutputHrs(Isis::HIGH_REPR_SAT4);
    }

    Filename tempFile(ui.GetFilename("TO"));
    tempFile.Temporary(Filename(ui.GetFilename("TO")).Basename(), "temp");
    string tempFilename(tempFile.Expanded());
    ofstream temporaryFile(tempFilename.c_str());

    pe.StartProcess(temporaryFile);
    temporaryFile.close();

    // Calculate MD5 Checksum
    g_md5Checksum = MD5Checksum(tempFilename);

    Filename outFile(ui.GetFilename("TO"));
    string outFilename(outFile.Expanded());
    ifstream inFile(tempFilename.c_str());
    ofstream pdsFile(outFilename.c_str());

    // Output the label
    OutputLabel(pdsFile, inCube);

    // Then copy the image data
    CopyData(inFile, pdsFile);

    pdsFile.close();

    pe.EndProcess();

    remove((scaledCube.Expanded()).c_str());
    remove(tempFilename.c_str());
    return;
}

void ResetGlobals () {
    g_md5Checksum = "";
    g_isIof = false;
}

void ProcessImage ( Buffer &in, Buffer &out ) {
    for (int i = 0; i < in.size(); i++) {
        if (IsSpecial(in[i]))
            out[i] = in[i];
        else if (g_isIof)
            out[i] = SCALING_FACTOR * in[i];
        else
            out[i] = in[i];
    }
}

string MD5Checksum ( string filename ) {
    md5wrapper md5;
    std::string checkSum = md5.getHashFromFile(filename.c_str());
    return checkSum;
}

void OutputLabel ( std::ofstream &fout, Cube* cube ) {
    OriginalLabel origLab(cube->getFilename());
    Pvl labelPvl = origLab.ReturnLabels();

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
    Filename transFile(transDir + "lronacPdsLabelExport.trn");
    PvlTranslationManager labelXlator(labelPvl, transFile.Expanded());
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
    if (g_isIof) {
        imageObject += PvlKeyword("SAMPLE_BITS", 16);
        imageObject += PvlKeyword("SAMPLE_TYPE", "LSB_INTEGER");
        imageObject += PvlKeyword("SCALING_FACTOR", 1.0 / SCALING_FACTOR);
        imageObject += PvlKeyword("VALID_MINIMUM", Isis::VALID_MIN2);
        imageObject += PvlKeyword("NULL", Isis::NULL2);
        imageObject += PvlKeyword("LOW_REPR_SATURATION", Isis::LOW_REPR_SAT2);
        imageObject += PvlKeyword("LOW_INSTR_SATURATION", Isis::LOW_INSTR_SAT2);
        imageObject += PvlKeyword("HIGH_INSTR_SATURATION", Isis::HIGH_INSTR_SAT2);
        imageObject += PvlKeyword("HIGH_REPR_SATURATION", Isis::HIGH_REPR_SAT2);
        imageObject += PvlKeyword("UNIT", "Scaled I/F");
    }
    else {
        imageObject += PvlKeyword("SAMPLE_BITS", 32);
        imageObject += PvlKeyword("SAMPLE_TYPE", "PC_REAL");
        imageObject += PvlKeyword("VALID_MINIMUM", "16#FF7FFFFA#");
        imageObject += PvlKeyword("NULL", "16#FF7FFFFB#");
        imageObject += PvlKeyword("LOW_REPR_SATURATION", "16#FF7FFFFC#");
        imageObject += PvlKeyword("LOW_INSTR_SATURATION", "16#FF7FFFFD#");
        imageObject += PvlKeyword("HIGH_INSTR_SATURATION", "16#FF7FFFFE#");
        imageObject += PvlKeyword("HIGH_REPR_SATURATION", "16#FF7FFFFF#");
        imageObject += PvlKeyword("UNIT", "W / (m**2 micrometer sr)");
    }
    imageObject += PvlKeyword("MD5_CHECKSUM", (iString) g_md5Checksum);

    stream << labelPvl;

    int recordBytes = cube->getSampleCount();
    int labelRecords = (int) ((stream.str().length()) / recordBytes) + 1;

    labelPvl["RECORD_BYTES"] = recordBytes;
    if (g_isIof)
        labelPvl["FILE_RECORDS"] = (int) (cube->getLineCount() * 2 + labelRecords);
    else
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
    char line[2532];
    while (!fin.eof()) {
        fin.read(line, 2532);
        fout.write(line, fin.gcount());
    }
}

