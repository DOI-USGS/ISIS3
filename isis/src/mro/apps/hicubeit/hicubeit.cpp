/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "hicubeit.h"

#include "TextFile.h"
#include "Pvl.h"
#include "Cube.h"
#include "OriginalLabel.h"
#include "ProgramLauncher.h"
#include <QString>

using namespace std;

namespace Isis {

  void hicubeit(UserInterface &ui) {
    QString redFile = ui.GetCubeName("RED");
    QString irFile  = ui.GetCubeName("IR");
    QString bgFile  = ui.GetCubeName("BG");

    FileName tempFile = FileName::createTempFile("$TEMPORARY/hicubeit.temp.lis");
    TextFile tf;
    tf.Open(tempFile.expanded(), "output");
    tf.PutLine(irFile + "\n");
    tf.PutLine(redFile + "\n");
    tf.PutLine(bgFile + "\n");
    tf.Close();

    QString parameters = QString(" FROMLIST = ")    + tempFile.expanded() +
                        QString(" TO = ")      + ui.GetCubeName("TO") +
                        QString(" PROPLAB = ") + redFile;
    ProgramLauncher::RunIsisProgram("cubeit", parameters);
    remove(tempFile.expanded().toLatin1().data());

    // Get the instrument group from each file
    Pvl redLab(redFile);
    Pvl irLab(irFile);
    Pvl bgLab(bgFile);

    PvlGroup redInst = redLab.findGroup("Instrument", Pvl::Traverse);
    PvlGroup irInst  = irLab.findGroup("Instrument", Pvl::Traverse);
    PvlGroup bgInst  = bgLab.findGroup("Instrument", Pvl::Traverse);

    // Error check to make sure the proper ccds are stacked
    if((int)redInst["CpmmNumber"] == 5) {
      if(((int)irInst["CpmmNumber"] != 6) || ((int)bgInst["CpmmNumber"] != 4)) {
        QString msg = "You can only stack color images with RED4, IR10, and BG12 ";
        msg += "or RED5, IR11, and BG13";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    else if((int)redInst["CpmmNumber"] == 8) {
      if(((int)irInst["CpmmNumber"] != 7) || ((int)bgInst["CpmmNumber"] != 9)) {
        QString msg = "You can only stack color images with RED4, IR10, and BG12 ";
        msg += "or RED5, IR11, and BG13";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }
    else {
      QString msg = "You can only stack color images with RED4, IR10, and BG12 ";
      msg += "or RED5, IR11, and BG13";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // Concatenate all the source products into one keyword
    PvlKeyword sourceProductId("SourceProductId");
    sourceProductId += (QString)bgInst["StitchedProductIds"][0];
    if(bgInst["StitchedProductIds"].size() > 1) {
      sourceProductId += (QString)bgInst["StitchedProductIds"][1];
    }
    sourceProductId += (QString)redInst["StitchedProductIds"][0];
    if(redInst["StitchedProductIds"].size() > 1) {
      sourceProductId += (QString)redInst["StitchedProductIds"][1];
    }
    sourceProductId += (QString)irInst["StitchedProductIds"][0];
    if(irInst["StitchedProductIds"].size() > 1) {
      sourceProductId += (QString)irInst["StitchedProductIds"][1];
    }

    // Get min start and max stop time
    PvlKeyword startTime = redInst["StartTime"];
    PvlKeyword stopTime  = redInst["StopTime"];
    PvlKeyword startClk  = redInst["SpacecraftClockStartCount"];
    PvlKeyword stopClk   = redInst["SpacecraftClockStopCount"];

    if((QString) irInst["StartTime"] < (QString)startTime) {
      startTime = irInst["StartTime"];
    }
    if((QString) bgInst["StartTime"] < (QString)startTime) {
      startTime = bgInst["StartTime"];
    }

    if((QString) irInst["StopTime"] > (QString)stopTime) {
      stopTime = irInst["StopTime"];
    }
    if((QString) bgInst["StopTime"] > (QString)stopTime) {
      stopTime = bgInst["StopTime"];
    }

    if((QString) irInst["SpacecraftClockStartCount"] < (QString)startClk) {
      startClk = irInst["SpacecraftClockStartCount"];
    }
    if((QString) bgInst["SpacecraftClockStartCount"] < (QString)startClk) {
      startClk = bgInst["SpacecraftClockStartCount"];
    }

    if((QString) irInst["SpacecraftClockStopCount"] > (QString)stopClk) {
      stopClk = irInst["SpacecraftClockStopCount"];
    }
    if((QString) bgInst["SpacecraftClockStopCount"] > (QString)stopClk) {
      stopClk = bgInst["SpacecraftClockStopCount"];
    }

    // Concatenate all TDIs into one keyword
    OriginalLabel redOrgLab(redFile);
    OriginalLabel irOrgLab(irFile);
    OriginalLabel bgOrgLab(bgFile);

    PvlGroup redGrp = redOrgLab.ReturnLabels().findGroup("INSTRUMENT_SETTING_PARAMETERS", Pvl::Traverse);
    PvlGroup irGrp = irOrgLab.ReturnLabels().findGroup("INSTRUMENT_SETTING_PARAMETERS", Pvl::Traverse);
    PvlGroup bgGrp = bgOrgLab.ReturnLabels().findGroup("INSTRUMENT_SETTING_PARAMETERS", Pvl::Traverse);

    PvlKeyword cpmmTdiFlag("cpmmTdiFlag");
    for(int i = 0; i < 14; i++) {
      cpmmTdiFlag += (QString) "";
    }
    cpmmTdiFlag[(int)redInst["CpmmNumber"]] = (QString) redGrp["MRO:TDI"];
    cpmmTdiFlag[(int)irInst["CpmmNumber"]] = (QString) irGrp["MRO:TDI"];
    cpmmTdiFlag[(int)bgInst["CpmmNumber"]] = (QString) bgGrp["MRO:TDI"];

    // Concatenate all summing modes into one keyword
    PvlKeyword cpmmSummingFlag("cpmmSummingFlag");
    for(int i = 0; i < 14; i++) {
      cpmmSummingFlag += (QString) "";
    }
    cpmmSummingFlag[(int)redInst["CpmmNumber"]] = (QString) redGrp["MRO:BINNING"];
    cpmmSummingFlag[(int)irInst["CpmmNumber"]] = (QString) irGrp["MRO:BINNING"];
    cpmmSummingFlag[(int)bgInst["CpmmNumber"]] = (QString) bgGrp["MRO:BINNING"];

    //Concatenate all the Special_Processing_Flag into one keyword
    PvlKeyword specialProcessingFlag("SpecialProcessingFlag");
    for(int i = 0; i < 14; i++) {
      specialProcessingFlag += (QString) "";
    }
    //keyword Special_Processing_Flag may not be present so need to test
    // if not present set to NOMINAL
    if(redInst.hasKeyword("Special_Processing_Flag")) {
      specialProcessingFlag[redInst["CpmmNumber"]] = (QString) redInst["Special_Processing_Flag"];
    }
    else {
      specialProcessingFlag[redInst["CpmmNumber"]] = "NOMINAL";
    }
    if(irInst.hasKeyword("Special_Processing_Flag")) {
      specialProcessingFlag[irInst["CpmmNumber"]] = (QString) irInst["Special_Processing_Flag"];
    }
    else {
      specialProcessingFlag[irInst["CpmmNumber"]] = "NOMINAL";
    }
    if(bgInst.hasKeyword("Special_Processing_Flag")) {
      specialProcessingFlag[bgInst["CpmmNumber"]] = (QString) bgInst["Special_Processing_Flag"];
    }
    else {
      specialProcessingFlag[bgInst["CpmmNumber"]] = "NOMINAL";
    }
    // Put them in a group
    PvlGroup mos("Mosaic");
    mos += sourceProductId;
    mos += startTime;
    mos += stopTime;
    mos += startClk;
    mos += stopClk;
    mos += cpmmTdiFlag;
    mos += cpmmSummingFlag;
    mos += specialProcessingFlag;

    // Add the group to the output cube
    Cube c;
    c.open(ui.GetCubeName("TO"), "rw");
    c.label()->findObject("IsisCube", Pvl::Traverse).addGroup(mos);
    c.close();
  }
}
