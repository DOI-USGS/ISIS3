#include "Isis.h"
#include "ProcessImportPds.h"
#include "UserInterface.h"
#include "Filename.h"
#include "iException.h"
#include "iString.h"
#include "Pvl.h"
#include "OriginalLabel.h"
#include "History.h"
#include "LineManager.h"
#include <vector>

#define MAX_INPUT_VALUE 4095

using namespace std;
using namespace Isis;

void ResetGlobals();
void Import ( Buffer &buf );
void TranslateLrocNacLabels ( Filename &labelFile, Cube *ocube );

// Global variables for processing functions
Cube *g_ocube;
std::vector< double> g_xterm, g_bterm, g_mterm;
bool g_flip = false;

void IsisMain () {
    // Initialize variables
    ResetGlobals();

    //Check that the file comes from the right camera
    UserInterface &ui = Application::GetUserInterface();
    Filename inFile = ui.GetFilename("FROM");
    iString id;
    int sumMode;
    try {
        Pvl lab(inFile.Expanded());

        if (lab.HasKeyword("DATA_SET_ID"))
            id = (string) lab.FindKeyword("DATA_SET_ID");
        else {
            string msg = "Unable to read [DATA_SET_ID] from input file [" + inFile.Expanded() + "]";
            throw iException::Message(iException::Io, msg, _FILEINFO_);
        }

        //Checks if in file is rdr
        bool projected = lab.HasObject("IMAGE_MAP_PROJECTION");
        if (projected) {
            string msg = "[" + inFile.Name() + "] appears to be an rdr file.";
            msg += " Use pds2isis.";
            throw iException::Message(iException::User, msg, _FILEINFO_);
        }

        sumMode = (int) lab.FindKeyword("CROSSTRACK_SUMMING");

        // Store the decompanding information
        PvlKeyword xtermKeyword = lab.FindKeyword("LRO:XTERM"),
                   mtermKeyword = lab.FindKeyword("LRO:MTERM"),
                   btermKeyword = lab.FindKeyword("LRO:BTERM");

        if (mtermKeyword.Size() != xtermKeyword.Size() || btermKeyword.Size() != xtermKeyword.Size()) {
            string msg = "The decompanding terms do not have the same dimensions";
            throw iException::Message(iException::Io, msg, _FILEINFO_);
        }

        for (int i = 0; i < xtermKeyword.Size(); i++) {
            g_xterm.push_back(xtermKeyword[i]);
            g_mterm.push_back(mtermKeyword[i]);
            g_bterm.push_back(btermKeyword[i]);
        }

        double versionId = (double)(lab.FindKeyword("PRODUCT_VERSION_ID")[0]).Trim("v");

        if (lab.FindKeyword("FRAME_ID")[0] == "RIGHT" && versionId < 1.30)
            g_flip = true;
        else
            g_flip = false;
    }
    catch (iException &e) {
        string msg = "The PDS header is missing important keyword(s).";
        throw iException::Message(iException::Io, msg, _FILEINFO_);
    }

    id.ConvertWhiteSpace();
    id.Compress();
    id.Trim(" ");
    if (id.substr(13,3) != "EDR") {
        string msg = "Input file [" + inFile.Expanded() + "] does not appear to be "
                + "in LROC-NAC EDR format. DATA_SET_ID is [" + id + "]";
        throw iException::Message(iException::Io, msg, _FILEINFO_);
    }

    //Process the file
    Pvl pdsLab;
    ProcessImportPds p;
    p.SetPdsFile(inFile.Expanded(), "", pdsLab);

    // Set the output bit type to Real
    CubeAttributeOutput &outAtt = ui.GetOutputAttribute("TO");

    g_ocube = new Cube();
    g_ocube->SetByteOrder(outAtt.ByteOrder());
    g_ocube->SetCubeFormat(outAtt.FileFormat());
    g_ocube->SetMinMax((double) VALID_MIN2, (double) VALID_MAX2);
    if (outAtt.DetachedLabel()) g_ocube->SetDetached();
    if (outAtt.AttachedLabel()) g_ocube->SetAttached();
    g_ocube->SetDimensions(p.Samples(), p.Lines(), p.Bands());
    g_ocube->SetPixelType(Isis::Real);
    g_ocube->Create(ui.GetFilename("TO"));

    // Do 8 bit to 12 bit conversion
    // And if NAC-R, flip the frame
    p.StartProcess(Import);

    // Then translate the labels
    TranslateLrocNacLabels(inFile, g_ocube);
    p.EndProcess();

    // Add History
    History history("IsisCube");
    history.AddEntry();
    g_ocube->Write(history);

    // Add original label
    OriginalLabel origLabel(pdsLab);
    g_ocube->Write(origLabel);

    g_ocube->Close();
    delete g_ocube;
}

// The input buffer has a raw 16 bit buffer but the values are still 0 to 255
// See "Appendix B - NAC and WAC Companding Schemes" of the LROC_SOC_SPEC document for reference
void Import ( Buffer &in ) {
    LineManager outLines(*g_ocube);
    outLines.SetLine(in.Line(), in.Band());
    Buffer buf(in.SampleDimension(), in.LineDimension(), in.BandDimension(), g_ocube->PixelType());

    // Do the decompanding
    for (int pixin = 0; pixin < in.size(); pixin++) {
        // if pixin < xtermo0, then it is in "segment 0"
        if (in[pixin] < g_xterm[0])
            buf[pixin] = (int) in[pixin];

        // otherwise, it is in segments 1 to 5
        else {
            unsigned int segment = 1;
            while (segment < g_xterm.size() && (in[pixin]- g_bterm[segment - 1])/g_mterm[segment - 1] >= g_xterm[segment])
                segment++;

            // Compute the upper and lower bin values
            double upper = (in[pixin] + 1 - g_bterm[segment - 1])/g_mterm[segment - 1] - 1;
            double lower = (in[pixin]- g_bterm[segment - 1])/g_mterm[segment - 1];

            // Check if the bin is on the upper boundary of the last segment
            if (upper > MAX_INPUT_VALUE)
                upper = MAX_INPUT_VALUE;
            else if (segment < g_xterm.size() && upper >= g_xterm[segment])
                upper = g_xterm[segment]-1;

            // Check if it is on the lower boundary of a segment
            if (lower < g_xterm[segment-1])
                lower = g_xterm[segment-1];

            // Output the middle bin value
            buf[pixin] = (upper + lower)/2.0;
        }
    }

    // flip the NAC-R frame
    if (g_flip) {
        Buffer tmpbuf(buf);
        for (int i = 0; i < buf.size(); i++)
            buf[i] = tmpbuf[buf.size() - i - 1];
    }
    outLines.Copy(buf);
    g_ocube->Write(outLines);
}

//Function to translate the labels
void TranslateLrocNacLabels ( Filename &labelFile, Cube *ocube ) {

    //Pvl to store the labels
    Pvl outLabel;
    //Set up the directory where the translations are
    PvlGroup dataDir(Preference::Preferences().FindGroup("DataDirectory"));
    iString transDir = (string) dataDir["Lro"] + "/translations/";
    Pvl labelPvl(labelFile.Expanded());

    //Translate the Instrument group
    Filename transFile(transDir + "lronacInstrument.trn");
    PvlTranslationManager instrumentXlator(labelPvl, transFile.Expanded());
    instrumentXlator.Auto(outLabel);

    //Translate the Archive group
    transFile = transDir + "lronacArchive.trn";
    PvlTranslationManager archiveXlater(labelPvl, transFile.Expanded());
    archiveXlater.Auto(outLabel);

    //Translate the BandBin group
    transFile = transDir + "lronacBandBin.trn";
    PvlTranslationManager bandBinXlater(labelPvl, transFile.Expanded());
    bandBinXlater.Auto(outLabel);

    Pvl lab(labelFile.Expanded());

    //Set up the Kernels group
    PvlGroup kern("Kernels");
    if (lab.FindKeyword("FRAME_ID")[0] == "LEFT")
        kern += PvlKeyword("NaifFrameCode", -85600);
    else
        kern += PvlKeyword("NaifFrameCode", -85610);

    PvlGroup inst = outLabel.FindGroup("Instrument", Pvl::Traverse);
    if (lab.FindKeyword("FRAME_ID")[0] == "LEFT") {
        inst.FindKeyword("InstrumentId") = "NACL";
        inst.FindKeyword("InstrumentName") = "LUNAR RECONNAISSANCE ORBITER NARROW ANGLE CAMERA LEFT";
    }
    else {
        inst.FindKeyword("InstrumentId") = "NACR";
        inst.FindKeyword("InstrumentName") = "LUNAR RECONNAISSANCE ORBITER NARROW ANGLE CAMERA RIGHT";
    }

    //Add all groups to the output cube
    ocube->PutGroup(inst);
    ocube->PutGroup(outLabel.FindGroup("Archive", Pvl::Traverse));
    ocube->PutGroup(outLabel.FindGroup("BandBin", Pvl::Traverse));
    ocube->PutGroup(kern);
}

void ResetGlobals() {
    g_ocube = NULL;
    g_xterm.clear();
    g_mterm.clear();
    g_bterm.clear();
    g_flip = false;
}
