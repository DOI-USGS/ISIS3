#include <cmath>

#include "Apps.h"

using namespace Isis;

namespace Isis {

int ss, sl, sb;
int ns, nl, nb;
int linc, sinc;
Cube *cube = NULL;
LineManager *in = NULL;


// Line processing routine
void crop_process(Buffer &out) {
  // Read the input line
  int iline = sl + (out.Line() - 1) * linc;
  in->SetLine(iline, sb);
  cube->read(*in);

  // Loop and move appropriate samples
  for(int i = 0; i < out.size(); i++) {
    out[i] = (*in)[(ss - 1) + i * sinc];
  }

  if(out.Line() == nl) sb++;
}

void crop(QString from, QString to, int line, int sample, int nsamples, int nlines, int s_inc, int l_inc, bool propSpice) {
  ProcessByLine p;
  linc = l_inc;
  sinc = s_inc;
  // Open the input cube
  CubeAttributeInput inAtt(from);
  CubeAttributeOutput outAtt(to);
  cube = new Cube();
  cube->setVirtualBands(inAtt.bands());
  from = from;
  cube->open(from);

  // Determine the sub-area to extract
  ss = sample;
  sl = line;
  sb = 1;

  int origns = cube->sampleCount();
  int orignl = cube->lineCount();
  int es = cube->sampleCount();

  if (nsamples < 0)
    es = ss + nsamples - 1;

  int el = cube->lineCount();

  if (nlines < 0)
    el = sl + nlines - 1;

  int eb = cube->bandCount();

  // Make sure starting positions fall within the cube
  if (ss > cube->sampleCount()) {
    cube->close();
    QString msg = "[SAMPLE] exceeds number of samples in the [FROM] cube";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (sl > cube->lineCount()) {
    cube->close();
    QString msg = "[LINE] exceeds number of lines in the [FROM] cube";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Make sure the number of elements do not fall outside the cube
  if (es > cube->sampleCount()) {
    cube->close();
    QString msg = "[SAMPLE+NSAMPLES-1] exceeds number of ";
    msg += "samples in the [FROM] cube";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  if (el > cube->lineCount()) {
    cube->close();
    QString msg = "[LINE+NLINES-1] exceeds number of ";
    msg += "lines in the [FROM] cube";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  // Determine the size of the output cube and then set the output image size
  ns = ceil((double)(es - ss + 1) / sinc);
  nl = ceil((double)(el - sl + 1) / linc);
  nb = eb;
  //if (ns == 0) ns = 1;
  //if (nl == 0) nl = 1;
  es = ss + (ns - 1) * sinc;
  el = sl + (nl - 1) * linc;

  // Allocate the output file and make sure things get propogated nicely
  p.SetInputCube(from, inAtt);
  p.PropagateTables(false);
  Cube *ocube = p.SetOutputCube(to, outAtt, ns, nl, nb);
  p.ClearInputCubes();

  // propagate tables manually
  Pvl &inLabels = *cube->label();

  // Loop through the labels looking for object = Table
  for(int labelObj = 0; labelObj < inLabels.objects(); labelObj++) {
    PvlObject &obj = inLabels.object(labelObj);

    if(obj.name() != "Table") continue;

    // If we're not propagating spice data, dont propagate the following tables...
    if(!propSpice) {
      if((IString)obj["Name"][0] == "InstrumentPointing") continue;
      if((IString)obj["Name"][0] == "InstrumentPosition") continue;
      if((IString)obj["Name"][0] == "BodyRotation") continue;
      if((IString)obj["Name"][0] == "SunPosition") continue;
    }

    // Read the table into a table object
    Table table(obj["Name"], from);

    // We are not going to bother with line/sample associations; they apply
    //   only to the alpha cube at this time. I'm leaving this code here for the
    //   equations in case we try our hand at modifying these tables at a later date.

    /* Deal with associations, sample first
    if(table.IsSampleAssociated()) {
      int numDeleted = 0;
      for(int samp = 0; samp < cube->sampleCount(); samp++) {
        // This tests checks to see if we would include this sample.
        //   samp - (ss-1)) / sinc must be a whole number less than ns.
        if((samp - (ss-1)) % sinc != 0 || (samp - (ss-1)) / sinc >= ns || (samp - (ss-1)) < 0) {
          table.Delete(samp-numDeleted);
          numDeleted ++;
        }
      }
    }

    // Deal with line association
    if(table.IsLineAssociated()) {
      int numDeleted = 0;
      for(int line = 0; line < cube->lineCount(); line++) {
        // This tests checks to see if we would include this line.
        //   line - (sl-1)) / linc must be a whole number less than nl.
        if((line - (sl-1)) % linc != 0 || (line - (sl-1)) / linc >= nl || (line - (sl-1)) < 0) {
          table.Delete(line-numDeleted);
          numDeleted ++;
        }
      }
    }*/

    // Write the table
    ocube->write(table);
  }

  Pvl &outLabels = *ocube->label();
  if(!propSpice && outLabels.findObject("IsisCube").hasGroup("Kernels")) {
    PvlGroup &kerns = outLabels.findObject("IsisCube").findGroup("Kernels");

    QString tryKey = "NaifIkCode";
    if(kerns.hasKeyword("NaifFrameCode")) {
      tryKey = "NaifFrameCode";
    }

    if(kerns.hasKeyword(tryKey)) {
      PvlKeyword ikCode = kerns[tryKey];
      kerns = PvlGroup("Kernels");
      kerns += ikCode;
    }
  }

  // Create a buffer for reading the input cube
  in = new LineManager(*cube);

  // Crop the input cube
  p.StartProcess(crop_process);

  delete in;
  in = NULL;

  // Construct a label with the results/home/krodriguez/repos/ISIS3/isis/src/base/objs/Apps/Apps.cpp:7:1: error: expected ‘;’ before ‘namespace’

  PvlGroup results("Results");
  results += PvlKeyword("InputLines", toString(orignl));
  results += PvlKeyword("InputSamples", toString(origns));
  results += PvlKeyword("StartingLine", toString(sl));
  results += PvlKeyword("StartingSample", toString(ss));
  results += PvlKeyword("EndingLine", toString(el));
  results += PvlKeyword("EndingSample", toString(es));
  results += PvlKeyword("LineIncrement", toString(linc));
  results += PvlKeyword("SampleIncrement", toString(sinc));
  results += PvlKeyword("OutputLines", toString(nl));
  results += PvlKeyword("OutputSamples", toString(ns));

  // Update the Mapping, Instrument, and AlphaCube groups in the output
  // cube label
  SubArea *s;
  s = new SubArea;
  s->SetSubArea(orignl, origns, sl, ss, el, es, linc, sinc);
  s->UpdateLabel(cube, ocube, results);
  delete s;
  s = NULL;

  // Cleanup
  p.EndProcess();
  cube->close();
  delete cube;
  cube = NULL;

}



}
