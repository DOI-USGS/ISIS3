#include "Isis.h"
#include "Pipeline.h"
#include "Cube.h"
#include "LineManager.h"
#include "Progress.h"

using namespace Isis;

//! Create point spread function file
void CreatePsf(Pipeline &p);

//! Clean up anything done by CreatePsf
void CleanPsf();

//! Test to see if a number is a power of 2
bool IsPowerOf2(unsigned int num);

//! True if PSF was originally specified
bool manualPsf;

void IsisMain () {
  UserInterface &ui = Application::GetUserInterface();
 
  manualPsf = ui.WasEntered("PSF");

  // This is the equation that hisharpen needs to perform
  // Final magnitude/phase equations
  iString magniEq = "sqrt( oreal^2 + oimag^2 )";
  iString phaseEq = "atan2( oimag , oreal )";

  // Insert oreal/oimag
  magniEq.Replace("oreal", "((real*freal + imag*fimag)/(freal*freal + fimag*fimag))");
  phaseEq.Replace("oreal", "((real*freal + imag*fimag)/(freal*freal + fimag*fimag))");
  magniEq.Replace("oimag", "((imag*freal - real*fimag)/(freal*freal + fimag*fimag))");
  phaseEq.Replace("oimag", "((imag*freal - real*fimag)/(freal*freal + fimag*fimag))");

  // Insert real/freal (real/psf real), imag/fimag (imaginary/psf imaginary)
  magniEq.Replace("freal", "(f2*cos(f3))");
  phaseEq.Replace("freal", "(f2*cos(f3))");
  magniEq.Replace("real",  "(f1*cos(f4))");
  phaseEq.Replace("real",  "(f1*cos(f4))");
  magniEq.Replace("fimag", "(f2*sin(f3))");
  phaseEq.Replace("fimag", "(f2*sin(f3))");
  magniEq.Replace("imag",  "(f1*sin(f4))");
  phaseEq.Replace("imag",  "(f1*sin(f4))");

  // Strip spaces
  phaseEq.Replace(" ", "");
  magniEq.Replace(" ", "");

  Pipeline p;

  CreatePsf(p);

  p.SetInputFile("FROM");
  p.SetInputFile("PSF");
  p.SetOutputFile("TO");

  p.KeepTemporaryFiles(!ui.GetBoolean("CLEANUP"));

  p.AddToPipeline("fft");
  p.Application("fft").SetInputParameter("FROM", true);
  p.Application("fft").AddBranch("mag", PipelineApplication::ConstantStrings);
  p.Application("fft").AddBranch("phase", PipelineApplication::ConstantStrings);
  p.Application("fft").SetOutputParameter("FROM.mag", "MAGNITUDE", "fft", "cub");
  p.Application("fft").SetOutputParameter("FROM.phase", "PHASE", "fft", "cub");
  p.Application("fft").SetOutputParameter("PSF.mag", "MAGNITUDE", "fft", "cub");
  p.Application("fft").SetOutputParameter("PSF.phase", "PHASE", "fft", "cub");

  p.AddToPipeline("fx");
  p.Application("fx").SetInputParameter("FROMLIST", PipelineApplication::LastAppOutputListNoMerge, false);
  p.Application("fx").SetOutputParameter("FROM.mag", "TO", "fx", "cub");
  p.Application("fx").SetOutputParameter("PSF.phase", "TO", "fx", "cub");
  p.Application("fx").AddConstParameter("FROM.mag", "equation", magniEq);
  p.Application("fx").AddConstParameter("PSF.phase", "equation", phaseEq);
  p.Application("fx").AddConstParameter("MODE", "list");

  p.AddToPipeline("ifft");
  p.Application("ifft").SetInputParameter("MAGNITUDE", true);
  p.Application("ifft").AddParameter("PHASE", PipelineApplication::LastOutput);
  p.Application("ifft").SetOutputParameter("FROM.mag", "TO", "untranslated", "cub");

  p.AddToPipeline("translate");
  p.Application("translate").SetInputParameter("FROM", true);
  p.Application("translate").AddConstParameter("STRANS", "-1");
  p.Application("translate").AddConstParameter("LTRANS", "-1");
  p.Application("translate").AddConstParameter("INTERP", "near");
  p.Application("translate").SetOutputParameter("FROM.mag", "TO", "final", "cub");

  p.Run();

  CleanPsf();
}


/**
 * This method creates a point spread function file if one is needed. A 
 * point spread function file is a picture, taken by the instrument, of 
 * a point of light. This file needs to be normalized, and the point of light 
 * should be at the edges of the image: 
 *  
 *  Typical Picture of a point         Expected File
 *    ------------------           ------------------
 *    |                |           |*              *|
 *    |       *        |           |                |
 *    |      ***       |           |                |
 *    |       *        |   ===>    |                |
 *    |                |           |                |
 *    |                |           |*              *|
 *    ------------------           ------------------
 *  
 *  where the blank areas in these cubes are zeros, and the *'s are data with a
 *  sum of 1. What's meant by creating these cubes, and what this function does,
 *  is find a valid point spread function and pads the center of the image with
 *  zeros. This allows for the sum of the image to not change, and there
 *  shouldnt be more light away from the point of light.
 *  
 *  
 * @param p The pipeline; not modified, only used to get correct temp folder
 */
void CreatePsf(Pipeline &p) {
  if(manualPsf) return;

  UserInterface &ui = Application::GetUserInterface();

  // calculate the temp file filename
  iString tmpFile = p.TemporaryFolder() + "/";
  tmpFile += Filename(ui.GetAsString("TO")).Basename();
  tmpFile += ".psf.cub";

  // We need the base input and psf cubes to make the temporary psf cube
  Cube fromCube;
  fromCube.Open(ui.GetFilename("FROM"));

  // Verify the image looks like a hirise image
  try {
    const PvlGroup &instGrp = fromCube.Label()->FindGroup("Instrument", Pvl::Traverse);
    iString instrument = (std::string)instGrp["InstrumentId"];

    if(instrument != "HIRISE") {
      iString message = "This program is meant to be run on HiRISE images only, found "
                        "[InstrumentId] to be [" + instrument + "] and was expecting [HIRISE]";
      throw iException::Message(iException::User, message, _FILEINFO_);
    }
  }
  catch (iException &e) {
    iString message = "The [FROM] file is not a valid HIRISE cube. "
                      "Please make sure it was imported using hi2isis.";
    throw iException::Message(iException::User, message, _FILEINFO_);
  }

  if(fromCube.Lines() != fromCube.Samples()) {
    iString message = "This program only works on square cubes, the number of samples [" +
                      iString(fromCube.Samples()) + "] must match the number of lines [" +
                      iString(fromCube.Lines()) + "]";
    throw iException::Message(iException::User,message,_FILEINFO_);
  }

  // Let's figure out which point spread function we're supposed to be using
  iString psfFile = "$mro/calibration/psf/PSF_";
  iString filter = (std::string)fromCube.Label()->FindGroup("Instrument", Pvl::Traverse)["CcdId"];

  if(filter.find("RED") != string::npos) {
    psfFile += "RED";
  }
  else if(filter.find("BG") != string::npos) {
    psfFile += "BG";
  }
  else if(filter.find("IR") != string::npos) {
    psfFile += "IR";
  }
  else {
    iString message = "The filter [" + filter + "] does not have a default point spread function. Please provide one using the [PSF] parameter.";
    throw iException::Message(iException::Programmer, message, _FILEINFO_);
  }

  psfFile += ".cub";

  Cube psfCube;
  psfCube.Open(psfFile);

  if(psfCube.Lines() > fromCube.Lines()) {
    iString message = "The input cube dimensions must be at least [" + iString(psfCube.Lines());
    message += "] pixels in the line and sample dimensions";
    throw iException::Message(iException::User,message,_FILEINFO_);
  }

  if(!IsPowerOf2(fromCube.Lines())) {
    iString message = "The input cube dimensions must be a power of 2 (found [" + 
                       iString(fromCube.Lines()) + "])";
    throw iException::Message(iException::User,message,_FILEINFO_);
  }

  LineManager psfMgr(psfCube);
  psfMgr.SetLine(1,1);

  // We also need the output temp psf cube
  Cube outPsfCube;
  outPsfCube.SetDimensions(fromCube.Samples(), fromCube.Lines(), 1);
  outPsfCube.Create(tmpFile);

  LineManager outMgr(outPsfCube);
  outMgr.SetLine(1,1);

  Progress progress;
  progress.SetText("Creating PSF File");
  progress.SetMaximumSteps(fromCube.Lines());
  progress.CheckStatus();

  const int halfInSamples = psfCube.Samples() / 2;
  for(int line = 0; line < outPsfCube.Lines(); line++) {
    psfCube.Read(psfMgr);

    for(int sample = 0; sample < outPsfCube.Samples(); sample++) {
      if(sample < halfInSamples) {
        outMgr[sample] = psfMgr[sample];
      }
      else if(sample >= outPsfCube.Samples() - halfInSamples) {
        outMgr[sample] = psfMgr[psfCube.Samples() - (outPsfCube.Samples() - sample)];
      }
      else {
        outMgr[sample] = 0.0;
      }
    }

    outPsfCube.Write(outMgr);

    if(line < psfCube.Lines()/2) {
      psfMgr ++;
    }
    else if(line >= outPsfCube.Lines() - psfCube.Lines()/2) {
      psfMgr ++;
    }

    outMgr ++;
    progress.CheckStatus();
  }

  ui.PutAsString("PSF", tmpFile);
}


void CleanPsf() {
  UserInterface &ui = Application::GetUserInterface();

  if(!manualPsf) {
    iString psfTempFile = Filename(ui.GetAsString("PSF")).Expanded();

    if(ui.GetBoolean("CLEANUP")) {
      remove(psfTempFile.c_str());
    }

    ui.Clear("PSF");
  }
}

bool IsPowerOf2(unsigned int num) {
  if(num == 0) return false;

  while(num != 1) {
    if(num % 2 == 1) return false;
    num /= 2;
  }

  return true;
}
