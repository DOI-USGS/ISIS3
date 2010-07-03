#include "Isis.h"

#include <cstdio>
#include <string>

#include "ProcessImportPds.h"
#include "LineManager.h"
#include "UserInterface.h"
#include "CubeAttribute.h"
#include "Filename.h"
#include "iException.h"
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
  Filename in = ui.GetFilename("FROM");

  // Make sure it is a Themis EDR/RDR
  bool projected;
  try {
    Pvl lab(in.Expanded());
    projected = lab.HasObject("IMAGE_MAP_PROJECTION");
    iString id;
    id = (string)lab["DATA_SET_ID"];
    id.ConvertWhiteSpace();
    id.Compress();
    id.Trim(" ");
    if(id.find("ODY-M-THM") == string::npos) {
      string msg = "Invalid DATA_SET_ID [" + id + "]";
      throw iException::Message(iException::Pvl, msg, _FILEINFO_);
    }
  }
  catch(iException &e) {
    string msg = "Input file [" + in.Expanded() +
                 "] does not appear to be " +
                 "in Themis EDR/RDR format";
    throw iException::Message(iException::Io, msg, _FILEINFO_);
  }

  //Checks if in file is rdr
  if(projected) {
    string msg = "[" + in.Name() + "] appears to be an rdr file.";
    msg += " Use pds2isis.";
    throw iException::Message(iException::User, msg, _FILEINFO_);
  }

  // Ok looks good ... set it as the PDS file
  Pvl pdsLab;
  p.SetPdsFile(in.Expanded(), "", pdsLab);

  OriginalLabel origLabels(pdsLab);

  Pvl isis3Lab;
  TranslateLabels(pdsLab, isis3Lab, p.Bands());

  // Set up the output cube
  Filename outFile(ui.GetFilename("TO"));
  PvlGroup &inst = isis3Lab.FindGroup("Instrument", Pvl::Traverse);

  if((string)inst["InstrumentId"] == "THEMIS_VIS") {
    Cube *even = new Cube();
    Cube *odd = new Cube();

    even->SetDimensions(p.Samples(), p.Lines(), p.Bands());
    even->SetPixelType(Isis::Real);
    odd->SetDimensions(p.Samples(), p.Lines(), p.Bands());
    odd->SetPixelType(Isis::Real);

    string evenFile = outFile.Path() + "/" + outFile.Basename() + ".even.cub";
    string oddFile = outFile.Path() + "/" + outFile.Basename() + ".odd.cub";

    even->Create(evenFile);
    odd->Create(oddFile);

    frameletLines = 192 / ((int)inst["SpatialSumming"]);

    outputCubes.push_back(odd);
    outputCubes.push_back(even);
  }
  else {
    Cube *outCube = new Cube();
    outCube->SetDimensions(p.Samples(), p.Lines(), p.Bands());
    outCube->SetPixelType(Isis::Real);

    outCube->Create(outFile.Expanded());
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
          PvlKeyword("NumFramelets", numFramelets), Pvl::Replace
        );

        string frameletType = ((i == 0) ? "Odd" : "Even");
        isis3Lab.FindGroup("Instrument").AddKeyword(
          PvlKeyword("Framelets", frameletType), Pvl::Replace
        );
      }

      outputCubes[i]->PutGroup(isis3Lab.Group(grp));
    }

    outputCubes[i]->Write(origLabels);
    outputCubes[i]->Close();
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

  outputCubes[outputCube]->Write(mgr);

  // Null out every other cube
  for(int i = 0; i < (int)outputCubes.size(); i++) {
    if(i == outputCube) continue;

    LineManager mgr(*outputCubes[i]);
    mgr.SetLine(in.Line(), in.Band());

    for(int j = 0; j < mgr.size(); j++) {
      mgr[j] = Isis::Null;
    }

    outputCubes[i]->Write(mgr);
  }
}

void TranslateLabels(Pvl &pdsLab, Pvl &isis3, int numBands) {
  // Create the Instrument Group
  PvlGroup inst("Instrument");
  inst += PvlKeyword("SpacecraftName", "MARS_ODYSSEY");
  string instId = (string) pdsLab["InstrumentId"] + "_" +
                  (string) pdsLab["DetectorId"];
  inst += PvlKeyword("InstrumentId", instId);
  inst += PvlKeyword("TargetName", (string) pdsLab["TargetName"]);
  inst += PvlKeyword("MissionPhaseName", (string) pdsLab["MissionPhaseName"]);
  inst += PvlKeyword("StartTime", (string)pdsLab["StartTime"]);
  inst += PvlKeyword("StopTime", (string)pdsLab["StopTime"]);
  inst += PvlKeyword("SpacecraftClockCount",
                     (string) pdsLab["SpacecraftClockStartCount"]);

  PvlObject &sqube = pdsLab.FindObject("SPECTRAL_QUBE");
  if(instId == "THEMIS_IR") {
    inst += PvlKeyword("GainNumber", (string)sqube["GainNumber"]);
    inst += PvlKeyword("OffsetNumber", (string)sqube["OffsetNumber"]);
    inst += PvlKeyword("MissingScanLines", (string)sqube["MissingScanLines"]);
    inst += PvlKeyword("TimeDelayIntegration",
                       (string)sqube["TimeDelayIntegrationFlag"]);
    if(sqube.HasKeyword("SpatialSumming")) {
      inst += PvlKeyword("SpatialSumming", (string)sqube["SpatialSumming"]);
    }
  }
  else {
    inst += PvlKeyword("ExposureDuration", (string)sqube["ExposureDuration"]);
    inst += PvlKeyword("InterframeDelay", (string)sqube["InterframeDelay"]);
    inst += PvlKeyword("SpatialSumming", (string)sqube["SpatialSumming"]);
  }

  // Add at time offset to the Instrument group
  UserInterface &ui = Application::GetUserInterface();
  double spacecraftClockOffset = ui.GetDouble("TIMEOFFSET");
  inst += PvlKeyword("SpacecraftClockOffset", spacecraftClockOffset, "seconds");

  isis3.AddGroup(inst);

  // Create the Band bin Group
  PvlGroup bandBin("BandBin");
  PvlKeyword originalBand("OriginalBand");
  for(int i = 1; i <= numBands; i++) {
    originalBand.AddValue(i);
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
  arch += PvlKeyword("DataSetId", (string)pdsLab["DataSetId"]);
  arch += PvlKeyword("ProducerId", (string)pdsLab["ProducerId"]);
  arch += PvlKeyword("ProductId", (string)pdsLab["ProductId"]);
  arch += PvlKeyword("ProductCreationTime",
                     (string)pdsLab["ProductCreationTime"]);
  arch += PvlKeyword("ProductVersionId", (string)pdsLab["ProductVersionId"]);
//  arch += PvlKeyword("ReleaseId",(string)pdsLab["ReleaseId"]);
  arch += PvlKeyword("OrbitNumber", (string)pdsLab["OrbitNumber"]);

  arch += PvlKeyword("FlightSoftwareVersionId",
                     (string)sqube["FlightSoftwareVersionId"]);
  arch += PvlKeyword("CommandSequenceNumber",
                     (string)sqube["CommandSequenceNumber"]);
  arch += PvlKeyword("Description", (string)sqube["Description"]);
  isis3.AddGroup(arch);

  // Create the Kernel Group
  PvlGroup kerns("Kernels");
  if(instId == "THEMIS_IR") {
    kerns += PvlKeyword("NaifFrameCode", -53031);
  }
  else {
    kerns += PvlKeyword("NaifFrameCode", -53032);
  }
  isis3.AddGroup(kerns);
}
