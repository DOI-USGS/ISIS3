#include "Isis.h"

#include "Cube.h"
#include "Filename.h"
#include "OriginalLabel.h"
#include "Pipeline.h"
#include "ProcessExport.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "PvlKeyword.h"
#include "Table.h"

using namespace Isis;
using namespace std;

void IsisMain() {
  // Get user interface
  UserInterface &ui = Application::GetUserInterface();

  // Copy the input cube to specified output cube in BSQ format
  ProcessExport p;
  p.SetInputCube("FROM");
  p.SetOutputCube("TO");

   // default options 32bit
  p.SetOutputType(Isis::Real);
  p.SetOutputNull(Isis::NULL4);
  p.SetOutputLrs(Isis::LOW_REPR_SAT4);
  p.SetOutputLis(Isis::LOW_INSTR_SAT4);
  p.SetOutputHrs(Isis::HIGH_REPR_SAT4);
  p.SetOutputHis(Isis::HIGH_INSTR_SAT4);
  p.SetOutputRange(-DBL_MAX, DBL_MAX);

  p.SetOutputEndian(Isis::Msb);
  p.SetFormat(ProcessExport::BSQ);

  Filename outCubeFile(ui.GetFilename("TO"));
  string outFilename(outCubeFile.Expanded());
  ofstream ostr(outFilename.c_str());
  p.StartProcess(ostr);
  p.EndProcess();
  ostr.close();

  string inFile  = ui.GetAsString("FROM");

  // Create separate CK Spice Kernel File if selected
  if (ui.GetBoolean("CK")) {
    string ckFile = ui.GetAsString("CKFILE");
    if (ckFile == "None") {
      ckFile = outCubeFile.Basename() + ".ck.bc";
      ui.PutFilename("CKFILE", ckFile);
    }
    try {
      Pipeline p("ck");
      p.SetInputFile("FROM");
      p.SetOutputFile("CKFILE");
      p.KeepTemporaryFiles(false);

      p.AddToPipeline("ckwriter");
      p.Application("ckwriter").SetInputParameter("FROM", false);
      p.Application("ckwriter").SetOutputParameter("TO", "ck");
      p.Run();
    } catch (IException &e) {
      string msg = "Cube must be run with spiceinit to get CK Spice Kernel\n";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }
  }

  // Create separate SPK Spice Kernel File if selected
  if (ui.GetBoolean("SPK")) {
    string spkFile = ui.GetAsString("SPKFILE");
    if (spkFile == "None") {
      spkFile = outCubeFile.Basename() + ".spk.bsp";
      ui.PutFilename("SPKFILE", spkFile);
    }
    try {
      Pipeline p("spk");
      p.SetInputFile("FROM");
      p.SetOutputFile("SPKFILE");
      p.KeepTemporaryFiles(false);

      p.AddToPipeline("spkwriter");
      p.Application("spkwriter").SetInputParameter("FROM", false);
      p.Application("spkwriter").SetOutputParameter("TO", "spk");
      p.Application("spkwriter").AddParameter("SPKTYPE", "TYPE");
      p.Run();
    } catch (IException &e) {
      string msg = "Cube must be run with spiceinit to get SPK Spice Kernel\n";
      throw IException(e, IException::User, msg, _FILEINFO_);
    }
  }

  Pvl metaDataPvl;
  PvlObject metaObj("PDS_MetaData");

  Pvl isisPvl(outFilename);

  // Get the Isis Core Object to get the StartByte
  PvlObject & coreObj = isisPvl.FindObject("IsisCube").FindObject("Core");
  coreObj.FindKeyword("Format").SetValue("BSQ");
  coreObj -= coreObj.FindKeyword("TileSamples");
  coreObj -= coreObj.FindKeyword("TileLines");

  PvlGroup & pixGrp = coreObj.FindGroup("Pixels");
  pixGrp.FindKeyword("ByteOrder").SetValue("Msb");
  pixGrp.FindKeyword("Type").SetValue("Real");

  metaObj += coreObj;

  // Get the Isis Instrument Group
  metaObj += isisPvl.FindObject("IsisCube").FindGroup("Instrument");

  // Get the Kernels Group
  metaObj += isisPvl.FindObject("IsisCube").FindGroup("Kernels");

  // Get the OriginalInstrument Group
  metaObj += isisPvl.FindObject("IsisCube").FindGroup("OriginalInstrument");

  // Get the Original HiRise label
  OriginalLabel origLab(inFile);
  Pvl origPvl;
  origPvl += origLab.ReturnLabels() ;
  PvlObject origObj = origPvl.FindObject("Root");
  origObj.SetName("OriginalLabel");

  Isis::PvlObject::PvlObjectIterator it;

  for (it=isisPvl.BeginObject(); it != isisPvl.EndObject(); it++ ) {
    if (it->Name() == "Table") {
      if (it->FindKeyword("Name")[0] == "HiRISE Calibration Ancillary") {
        metaObj += *it;
      }

      if (it->FindKeyword("Name")[0] == "HiRISE Ancillary"){
        metaObj += *it;
      }

      if (it->FindKeyword("Name")[0] == "HiRISE Calibration Image"){
        metaObj += *it;
      }
    }
  }
  metaDataPvl += metaObj;
  metaDataPvl += origObj;
  metaDataPvl.Write(ui.GetAsString("PDS_METADATA_FILE"));
}
