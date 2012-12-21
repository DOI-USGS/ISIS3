#include "Isis.h"

#include <cstdio>
#include <string>

#include "ProcessImportPds.h"
#include "LineManager.h"
#include "UserInterface.h"
#include "CubeAttribute.h"
#include "FileName.h"
#include "IException.h"
#include "iTime.h"
#include "ProcessByBrick.h"
#include "Brick.h"
#include "OriginalLabel.h"

using namespace std;
using namespace Isis;

vector<Cube *> outputCubes;
int frameletLines;
void TranslateLabels(Pvl &labelFile, Pvl &isis3, int numBands);
void separateFrames(Buffer &in);

void IsisMain() {
  // Grab the file to import
  ProcessImportPds p;
  outputCubes.clear();
  frameletLines = 192;
  UserInterface &ui = Application::GetUserInterface();
  FileName in = ui.GetFileName("FROM");

  // Make sure it is a Themis EDR/RDR
  bool projected;
  try {
    Pvl lab(in.expanded());
    projected = lab.HasObject("IMAGE_MAP_PROJECTION");
    QString id;
    id = (QString)lab["DATA_SET_ID"];
    id = id.simplified().trimmed();
    if(!id.startsWith("ODY-M-THM")) {
      QString msg = "Invalid DATA_SET_ID [" + id + "]";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
  }
  catch(IException &e) {
    QString msg = "Input file [" + in.expanded() +
                  "] does not appear to be " +
                  "in Themis EDR/RDR format";
    throw IException(IException::Io, msg, _FILEINFO_);
  }

  //Checks if in file is rdr
  if(projected) {
    QString msg = "[" + in.name() + "] appears to be an rdr file.";
    msg += " Use pds2isis.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Ok looks good ... set it as the PDS file
  Pvl pdsLab;
  p.SetPdsFile(in.expanded(), "", pdsLab);

  OriginalLabel origLabels(pdsLab);

  Pvl isis3Lab;
  TranslateLabels(pdsLab, isis3Lab, p.Bands());

  // Set up the output cube
  FileName outFile(ui.GetFileName("TO"));
  PvlGroup &inst = isis3Lab.FindGroup("Instrument", Pvl::Traverse);

  if((QString)inst["InstrumentId"] == "THEMIS_VIS") {
    Cube *even = new Cube();
    Cube *odd = new Cube();

    even->setDimensions(p.Samples(), p.Lines(), p.Bands());
    even->setPixelType(Isis::Real);
    odd->setDimensions(p.Samples(), p.Lines(), p.Bands());
    odd->setPixelType(Isis::Real);

    QString evenFile = outFile.path() + "/" + outFile.baseName() + ".even.cub";
    QString oddFile = outFile.path() + "/" + outFile.baseName() + ".odd.cub";

    even->create(evenFile);
    odd->create(oddFile);

    frameletLines = 192 / ((int)inst["SpatialSumming"]);

    outputCubes.push_back(odd);
    outputCubes.push_back(even);
  }
  else {
    Cube *outCube = new Cube();
    outCube->setDimensions(p.Samples(), p.Lines(), p.Bands());
    outCube->setPixelType(Isis::Real);

    outCube->create(outFile.expanded());
    outputCubes.push_back(outCube);
  }

  // Import the file and then translate labels
  p.StartProcess(separateFrames);
  p.EndProcess();

  for(int i = 0; i < (int)outputCubes.size(); i++) {
    for(int grp = 0; grp < isis3Lab.Groups(); grp++) {

      // vis image?
      if(outputCubes.size() != 1) {
        int numFramelets = p.Lines() / frameletLines;
        isis3Lab.FindGroup("Instrument").AddKeyword(
          PvlKeyword("NumFramelets", toString(numFramelets)), Pvl::Replace
        );

        QString frameletType = ((i == 0) ? "Odd" : "Even");
        isis3Lab.FindGroup("Instrument").AddKeyword(
          PvlKeyword("Framelets", frameletType), Pvl::Replace
        );
      }

      outputCubes[i]->putGroup(isis3Lab.Group(grp));
    }

    outputCubes[i]->write(origLabels);
    outputCubes[i]->close();
    delete outputCubes[i];
  }

  outputCubes.clear();
}

//! Separates each of the individual VIS frames into their own file
void separateFrames(Buffer &in) {
  // (line-1)/frameletHeight % numOutImages
  int outputCube = (in.Line() - 1) / frameletLines % outputCubes.size();
  LineManager mgr(*outputCubes[outputCube]);
  mgr.SetLine(in.Line(), in.Band());

  // mgr.Copy(in); doesn't work because the raw buffers dont match
  for(int i = 0; i < mgr.size(); i++)
    mgr[i] = in[i];

  outputCubes[outputCube]->write(mgr);

  // Null out every other cube
  for(int i = 0; i < (int)outputCubes.size(); i++) {
    if(i == outputCube) continue;

    LineManager mgr(*outputCubes[i]);
    mgr.SetLine(in.Line(), in.Band());

    for(int j = 0; j < mgr.size(); j++) {
      mgr[j] = Isis::Null;
    }

    outputCubes[i]->write(mgr);
  }
}

void TranslateLabels(Pvl &pdsLab, Pvl &isis3, int numBands) {
  // Create the Instrument Group
  PvlGroup inst("Instrument");
  inst += PvlKeyword("SpacecraftName", "MARS_ODYSSEY");
  QString instId = (QString) pdsLab["InstrumentId"] + "_" +
                  (QString) pdsLab["DetectorId"];
  inst += PvlKeyword("InstrumentId", instId);
  inst += PvlKeyword("TargetName", (QString) pdsLab["TargetName"]);
  inst += PvlKeyword("MissionPhaseName", (QString) pdsLab["MissionPhaseName"]);
  inst += PvlKeyword("StartTime", (QString)pdsLab["StartTime"]);
  inst += PvlKeyword("StopTime", (QString)pdsLab["StopTime"]);
  inst += PvlKeyword("SpacecraftClockCount",
                     (QString) pdsLab["SpacecraftClockStartCount"]);

  PvlObject &sqube = pdsLab.FindObject("SPECTRAL_QUBE");
  if(instId == "THEMIS_IR") {
    inst += PvlKeyword("GainNumber", (QString)sqube["GainNumber"]);
    inst += PvlKeyword("OffsetNumber", (QString)sqube["OffsetNumber"]);
    inst += PvlKeyword("MissingScanLines", (QString)sqube["MissingScanLines"]);
    inst += PvlKeyword("TimeDelayIntegration",
                       (QString)sqube["TimeDelayIntegrationFlag"]);
    if(sqube.HasKeyword("SpatialSumming")) {
      inst += PvlKeyword("SpatialSumming", (QString)sqube["SpatialSumming"]);
    }
  }
  else {
    inst += PvlKeyword("ExposureDuration", (QString)sqube["ExposureDuration"]);
    inst += PvlKeyword("InterframeDelay", (QString)sqube["InterframeDelay"]);
    inst += PvlKeyword("SpatialSumming", (QString)sqube["SpatialSumming"]);
  }

  // Add at time offset to the Instrument group
  UserInterface &ui = Application::GetUserInterface();
  double spacecraftClockOffset = ui.GetDouble("TIMEOFFSET");
  inst += PvlKeyword("SpacecraftClockOffset", toString(spacecraftClockOffset), "seconds");

  isis3.AddGroup(inst);

  // Create the Band bin Group
  PvlGroup bandBin("BandBin");
  PvlKeyword originalBand("OriginalBand");
  for(int i = 1; i <= numBands; i++) {
    originalBand.AddValue(toString(i));
  }
  bandBin += originalBand;
  bandBin += sqube.FindGroup("BandBin")["BandBinCenter"];
  bandBin += sqube.FindGroup("BandBin")["BandBinWidth"];
  bandBin += sqube.FindGroup("BandBin")["BandBinFilterNumber"];
  bandBin["BandBinCenter"].SetName("Center");
  bandBin["BandBinWidth"].SetName("Width");
  bandBin["BandBinFilterNumber"].SetName("FilterNumber");
  isis3.AddGroup(bandBin);

  // Create the archive Group
  PvlGroup arch("Archive");
  arch += PvlKeyword("DataSetId", (QString)pdsLab["DataSetId"]);
  arch += PvlKeyword("ProducerId", (QString)pdsLab["ProducerId"]);
  arch += PvlKeyword("ProductId", (QString)pdsLab["ProductId"]);
  arch += PvlKeyword("ProductCreationTime",
                     (QString)pdsLab["ProductCreationTime"]);
  arch += PvlKeyword("ProductVersionId", (QString)pdsLab["ProductVersionId"]);
//  arch += PvlKeyword("ReleaseId",(string)pdsLab["ReleaseId"]);
  arch += PvlKeyword("OrbitNumber", (QString)pdsLab["OrbitNumber"]);

  arch += PvlKeyword("FlightSoftwareVersionId",
                     (QString)sqube["FlightSoftwareVersionId"]);
  arch += PvlKeyword("CommandSequenceNumber",
                     (QString)sqube["CommandSequenceNumber"]);
  arch += PvlKeyword("Description", (QString)sqube["Description"]);
  isis3.AddGroup(arch);

  // Create the Kernel Group
  PvlGroup kerns("Kernels");
  if(instId == "THEMIS_IR") {
    kerns += PvlKeyword("NaifFrameCode", toString(-53031));
  }
  else {
    kerns += PvlKeyword("NaifFrameCode", toString(-53032));
  }
  isis3.AddGroup(kerns);
}
