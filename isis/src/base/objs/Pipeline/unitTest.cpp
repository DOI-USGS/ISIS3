#include "Isis.h"
#include "Pipeline.h"
#include "UserInterface.h"

using namespace Isis;

void PipeBranched();
void PipeMultiBranched();
void PipeSimple();
void PipeListed();

void IsisMain() {
  UserInterface &ui = Application::GetUserInterface();
  ui.PutFilename("FROM", "$ISIS3DATA/odyssey/testData/I00831002RDR.even.cub");
  ui.PutFilename("FROM2", "$ISIS3DATA/odyssey/testData/I00831002RDR.odd.cub");
  ui.PutFilename("TO", "/work1/out.cub");
  ui.PutString("SHAPE", "ELLIPSOID");

  ui.Clear("MAPPING");
  ui.PutBoolean("MAPPING", true);
  std::cout << "Simple Pipe" << std::endl;
  PipeSimple();
  std::cout << "Simple Pipe 2" << std::endl;
  ui.Clear("MAPPING");
  ui.PutBoolean("MAPPING", false);
  ui.PutString("BANDS", "2,4-5");
  PipeSimple();

  ui.Clear("MAPPING");
  ui.PutBoolean("MAPPING", true);
  std::cout << "Non-Merging Branching Pipe" << std::endl;
  PipeBranched();
  std::cout << "Standard Branching Pipe" << std::endl;
  ui.Clear("MAPPING");
  ui.PutBoolean("MAPPING", false);
  PipeBranched();

  std::cout << "Complicated Branching Pipe" << std::endl;
  PipeMultiBranched();

  ui.Clear("FROM");
  ui.Clear("TO");
  ui.PutFilename("FROM", "unitTest.lis");
  std::cout << "Testing listing methods" << std::endl;
  PipeListed();
}

void PipeBranched() {
  UserInterface &ui = Application::GetUserInterface();
  Pipeline p("unitTest1");

  p.SetInputFile("FROM", "BANDS");
  p.SetOutputFile("TO");
  p.KeepTemporaryFiles(!ui.GetBoolean("REMOVE"));

  p.AddToPipeline("thm2isis");
  p.Application("thm2isis").SetInputParameter("FROM", false);
  p.Application("thm2isis").SetOutputParameter("TO", "lev1");
  p.Application("thm2isis").AddBranch("even", PipelineApplication::ConstantStrings);
  p.Application("thm2isis").AddBranch("odd", PipelineApplication::ConstantStrings);

  cout << p << endl;

  p.AddToPipeline("spiceinit");
  p.Application("spiceinit").SetInputParameter("FROM", false);
  p.Application("spiceinit").AddParameter("PCK", "PCK");
  p.Application("spiceinit").AddParameter("CK", "CK");
  p.Application("spiceinit").AddParameter("SPK", "SPK");
  p.Application("spiceinit").AddParameter("SHAPE", "SHAPE");
  p.Application("spiceinit").AddParameter("MODEL", "MODEL");
  p.Application("spiceinit").AddParameter("CKNADIR", "CKNADIR");

  cout << p << endl;

  p.AddToPipeline("thmvisflat");
  p.Application("thmvisflat").SetInputParameter("FROM", true);
  p.Application("thmvisflat").SetOutputParameter("TO", "flat");

  cout << p << endl;

  p.AddToPipeline("thmvistrim");
  p.Application("thmvistrim").SetInputParameter("FROM", true);
  p.Application("thmvistrim").SetOutputParameter("TO", "cal");

  if(!ui.GetBoolean("VISCLEANUP")) {
    p.Application("thmvisflat").Disable();
    p.Application("thmvistrim").Disable();
  }

  cout << p << endl;

  p.AddToPipeline("cam2map");
  p.Application("cam2map").SetInputParameter("FROM", true);
  p.Application("cam2map").SetOutputParameter("TO", "lev2");

  p.Application("cam2map").AddParameter("even", "MAP", "MAP");
  p.Application("cam2map").AddParameter("even", "PIXRES", "RESOLUTION");

  if(ui.WasEntered("PIXRES")) {
    p.Application("cam2map").AddConstParameter("even", "PIXRES", "MPP");
  }

  cout << p << endl;

  p.Application("cam2map").AddParameter("odd", "MAP", PipelineApplication::LastOutput);
  p.Application("cam2map").AddConstParameter("odd", "PIXRES", "MAP");
  p.Application("cam2map").AddConstParameter("odd", "DEFAULTRANGE", "MAP");

  cout << p << endl;

  p.AddToPipeline("automos");
  p.Application("automos").SetInputParameter("FROMLIST", PipelineApplication::LastAppOutputList, false);
  p.Application("automos").SetOutputParameter("TO", "mos");

  cout << p << endl;

  if(ui.GetBoolean("INGESTION")) {
    p.SetFirstApplication("thm2isis");
  }
  else{
    p.SetFirstApplication("spiceinit");
  }

  cout << p << endl;

  if(ui.GetBoolean("MAPPING")) {
    p.SetLastApplication("automos");
  }
  else {
    p.SetLastApplication("thmvistrim");
  }

  cout << p << endl;
}

void PipeSimple() {
  UserInterface &ui = Application::GetUserInterface();
  Pipeline p("unitTest2");

  p.SetInputFile("FROM", "BANDS");
  p.SetOutputFile("TO");

  p.KeepTemporaryFiles(!ui.GetBoolean("REMOVE"));

  p.AddToPipeline("thm2isis");
  p.Application("thm2isis").SetInputParameter("FROM", false);
  p.Application("thm2isis").SetOutputParameter("TO", "lev1");

  cout << p << endl;

  p.AddToPipeline("spiceinit");
  p.Application("spiceinit").SetInputParameter("FROM", false);
  p.Application("spiceinit").AddParameter("PCK", "PCK");
  p.Application("spiceinit").AddParameter("CK", "CK");
  p.Application("spiceinit").AddParameter("SPK", "SPK");
  p.Application("spiceinit").AddParameter("SHAPE", "SHAPE");
  p.Application("spiceinit").AddParameter("MODEL", "MODEL");
  p.Application("spiceinit").AddParameter("CKNADIR", "CKNADIR");

  cout << p << endl;

  p.AddToPipeline("cam2map");
  p.Application("cam2map").SetInputParameter("FROM", true);
  p.Application("cam2map").SetOutputParameter("TO", "lev2");
  p.Application("cam2map").AddParameter("MAP", "MAP");
  p.Application("cam2map").AddParameter("PIXRES", "RESOLUTION");

  cout << p << endl;

  if(ui.WasEntered("PIXRES")) {
    p.Application("cam2map").AddConstParameter("PIXRES", "MPP");
  }

  cout << p << endl;

  if(ui.GetBoolean("INGESTION")) {
    p.SetFirstApplication("thm2isis");
  }
  else{
    p.SetFirstApplication("spiceinit");
  }

  cout << p << endl;

  if(ui.GetBoolean("MAPPING")) {
    p.SetLastApplication("cam2map");
  }
  else {
    p.SetLastApplication("spiceinit");
  }

  cout << p << endl;
}

void PipeMultiBranched() {
  Pipeline p("unitTest3");

  p.SetInputFile("FROM", "BANDS");
  p.SetInputFile("FROM2", "BANDS");
  p.SetOutputFile("TO");

  p.KeepTemporaryFiles(false);

  p.AddToPipeline("fft");
  p.Application("fft").SetInputParameter("FROM", true);
  p.Application("fft").AddBranch("mag", PipelineApplication::ConstantStrings);
  p.Application("fft").AddBranch("phase", PipelineApplication::ConstantStrings);
  p.Application("fft").SetOutputParameter("FROM.mag", "MAGNITUDE", "fft", "cub");
  p.Application("fft").SetOutputParameter("FROM.phase", "PHASE", "fft", "cub");
  p.Application("fft").SetOutputParameter("FROM2.mag", "MAGNITUDE", "fft", "cub");
  p.Application("fft").SetOutputParameter("FROM2.phase", "PHASE", "fft", "cub");

  cout << p << endl;

  p.AddToPipeline("fx");
  p.Application("fx").SetInputParameter("FILELIST", PipelineApplication::LastAppOutputListNoMerge, false);
  p.Application("fx").SetOutputParameter("FROM.mag", "TO", "fx2", "cub");
  p.Application("fx").SetOutputParameter("FROM2.phase", "TO", "fx2", "cub");
  p.Application("fx").AddConstParameter("FROM.mag", "equation", "1+2");
  p.Application("fx").AddConstParameter("MODE", "list");
  p.Application("fx").AddConstParameter("FROM2.phase", "equation", "1+3");

  cout << p << endl;

  p.AddToPipeline("ifft");
  p.Application("ifft").SetInputParameter("MAGNITUDE", true);
  p.Application("ifft").AddParameter("PHASE", PipelineApplication::LastOutput);
  p.Application("ifft").SetOutputParameter("FROM.mag", "TO", "untranslated", "cub");

  cout << p << endl;

  p.AddToPipeline("translate");
  p.Application("translate").SetInputParameter("FROM", true);
  p.Application("translate").AddConstParameter("STRANS", "-1");
  p.Application("translate").AddConstParameter("LTRANS", "-1");
  p.Application("translate").AddConstParameter("INTERP", "near");
  p.Application("translate").SetOutputParameter("FROM.mag", "TO", "final", "cub");

  cout << p << endl;
}

void PipeListed() {
  Pipeline p("unitTest4");

  p.SetInputListFile("FROM");
  p.SetOutputListFile("TO");

  p.KeepTemporaryFiles(false);

  p.AddToPipeline("cubeatt");
  p.Application("cubeatt").SetInputParameter("FROM", true);
  p.Application("cubeatt").SetOutputParameter("TO", "copy");

  p.AddToPipeline("spiceinit");
  p.Application("spiceinit").SetInputParameter("FROM", false);
  p.Application("spiceinit").AddConstParameter("ATTACH", "NO");

  p.AddToPipeline("appjit");
  p.Application("appjit").SetInputParameter("FROMLIST", PipelineApplication::LastAppOutputListNoMerge, false);

  p.Application("appjit").AddConstParameter("MASTER", "MASTER.cub");
  p.Application("appjit").AddConstParameter("DEGREE", "1");
  
  p.AddToPipeline("noproj");
  p.Application("noproj").SetInputParameter("FROM", true);
  p.Application("noproj").AddConstParameter("MATCH", "MATCH.cub");
  p.Application("noproj").SetOutputParameter("TO", "noproj");
  p.Application("noproj").SetOutputParameter("TO", "jitter");

  std::cout << p << std::endl;
}
