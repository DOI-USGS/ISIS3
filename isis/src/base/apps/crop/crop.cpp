#include <cmath>

#include "crop.h"

using namespace std;

namespace Isis {



  /**
   * Crop a cube along a line, sample range. This is the programmatic interface to
   * the ISIS stats application.
   *
   * @param ui The User Interface to parse the parameters from
   */
  PvlGroup crop(UserInterface &ui) {
    Cube *icube = new Cube();
    icube->open(ui.GetCubeName("FROM"));
    return crop(icube, ui);
  }


  /**
   * Compute the stats for an ISIS cube. This is the programmatic interface to
   * the ISIS stats application.
   *
   * @param cube input cube to be cropped
   * @param ui The User Interface to parse the parameters from
   */
  PvlGroup crop(Cube *cube, UserInterface &ui) {
    // Globals and prototypes
    int startSamp, startLine;
    int numSamps, numLines;
    int endSamp, endLine;
    int sinc, linc;  // sample/line increments 
    int curBand, numBands;
    int minPixel;
    int negOffsetLine, negOffsetSamp;
    int posOffsetLine, posOffsetSamp;
    
    bool allowOverhang, padOverhang, hasOverhang;

    LineManager *in = NULL;

    // Line processing routine (normal)
    auto cropProccess = [&](Buffer &out)->void {
      // This is run for every line of the output cube

      // Read the input line
      int iline = startLine + (out.Line() - 1) * linc;
      in->SetLine(iline, curBand);
      cube->read(*in);

      // Loop and move appropriate samples
      for(int i = 0; i < out.size(); i++) {
        out[i] = (*in)[(startSamp - 1) + i * sinc];
      }

      if(out.Line() == numLines) curBand++;
    };

    // Line processing routine with padding
    auto cropProcessPad = [&](Buffer &out)->void {
      // This is run for every line of the output cube, out.Line() gives which line.

      // if padding above input cube first line or below last line
      if ( (out.Line() - 1) * linc < negOffsetLine || posOffsetLine <= (out.Line() - 1) * linc) {
        for(int i = 0; i < out.size(); i++) {
          out[i] = NULL8;
        }
      }

      // lines between input cube first and last line.
      else {

        // Read the input line
        int iline = startLine + (out.Line() - 1) * linc;
        in->SetLine(iline, curBand);
        cube->read(*in);

        for(int i = 0; i < out.size(); i++) {
          if (i * sinc < negOffsetSamp || posOffsetSamp <= i * sinc) {
            out[i] = NULL8;
          }
          else {
            out[i] = (*in)[(startSamp - 1) + i * sinc];
          }
        }
      }

      if(out.Line() == numLines) curBand++;
    };

    ProcessByLine p;

    // Open the input cube
    QString from = ui.GetAsString("FROM");
    CubeAttributeInput inAtt(from);
    cube = new Cube();
    cube->setVirtualBands(inAtt.bands());
    from = ui.GetCubeName("FROM");
    cube->open(from);

    // Behavior for overhanging crops
    QString OverhangBehavior = ui.GetString("OVERHANG");

    if(OverhangBehavior == "PAD") {
      allowOverhang = true;
      padOverhang = true;
    }
    else if (OverhangBehavior == "SHRINK") {
      allowOverhang = true;
      padOverhang = false;
    }
    else {
      allowOverhang = false;
    }

    // Determine the sub-area to extract
    startSamp = ui.GetInteger("SAMPLE");
    startLine = ui.GetInteger("LINE");
    curBand = 1;                        
    minPixel = 1;

    int origns = cube->sampleCount();
    int orignl = cube->lineCount();
    endSamp = cube->sampleCount();                                                       // endSamp = ending sample: edge of cube, 
    if (ui.WasEntered("NSAMPLES")) endSamp = startSamp + ui.GetInteger("NSAMPLES") - 1; //            or startSample plus numSamples if numSamples was entered
    endLine = cube->lineCount();
    if (ui.WasEntered("NLINES")) endLine = startLine + ui.GetInteger("NLINES") - 1;   // endLine = ending line: ''
    numBands = cube->bandCount();

    sinc = ui.GetInteger("SINC");
    linc = ui.GetInteger("LINC");

    // Determine the size of the output cube and then set the output image size
    numSamps = ceil((double)(endSamp - startSamp + 1) / sinc);
    numLines = ceil((double)(endLine - startLine + 1) / linc);
    //if (numSamps == 0) numSamps = 1;
    //if (numLines == 0) numLines = 1;
    endSamp = startSamp + (numSamps - 1) * sinc;
    endLine = startLine + (numLines - 1) * linc;

    // Overhang Calculations
    hasOverhang = (minPixel > startSamp || minPixel > startLine || endSamp > cube->sampleCount() || endLine > cube->lineCount());

    if (allowOverhang && hasOverhang) {

      if (padOverhang) {
        negOffsetLine = minPixel - startLine;
        negOffsetSamp = minPixel - startSamp;
        posOffsetLine = negOffsetLine + cube->lineCount();
        posOffsetSamp = negOffsetSamp + cube->sampleCount();
      }

      // If need to shrink an overhanging crop, adjust down the dimensions.
      else {
        if (startLine < minPixel) { // Overhangs Top
          numLines -= (minPixel - startLine);
          startLine = minPixel;
        }
        if (startSamp < minPixel) { // Overhangs Left
          numSamps -= (minPixel - startSamp);
          startSamp = minPixel;
        }
        if (cube->lineCount() < endLine) { // Overhangs Bottom
          numLines -= (endLine - cube->lineCount());
          endLine = cube->lineCount();
        }
        if (cube->sampleCount() < endSamp) { // Overhangs Right
          numSamps -= (endSamp - cube->sampleCount());
          endSamp = cube->sampleCount();
        }
        hasOverhang = false;
      }
    }
    
    // Some overhang, but not allowed. Throw exceptions.
    else {

      if (startLine < minPixel) {
        cube->close();
        QString msg = "[LINE] less than position of first line in the [FROM] cube";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      if (startSamp < minPixel) {
        cube->close();
        QString msg = "[SAMPLE] less than position of first sample in the [FROM] cube";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      if (startSamp > cube->sampleCount()) {
        cube->close();
        QString msg = "[SAMPLE] exceeds number of samples in the [FROM] cube";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      
      if (startLine > cube->lineCount()) {
        cube->close();
        QString msg = "[LINE] exceeds number of lines in the [FROM] cube";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      
      // Make sure the number of elements do not fall outside the cube
      if (endSamp > cube->sampleCount()) {
        cube->close();
        QString msg = "[SAMPLE+NSAMPLES-1] exceeds number of ";
        msg += "samples in the [FROM] cube";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      
      if (endLine > cube->lineCount()) {
        cube->close();
        QString msg = "[LINE+NLINES-1] exceeds number of ";
        msg += "lines in the [FROM] cube";
        throw IException(IException::User, msg, _FILEINFO_);
      }
    }

    // Allocate the output file and make sure things get propogated nicely
    CubeAttributeInput &inputAtt =ui.GetInputAttribute("FROM");
    p.SetInputCube(ui.GetCubeName("FROM"), inputAtt);
    CubeAttributeOutput &att = ui.GetOutputAttribute("TO");
    Cube *ocube = p.SetOutputCube(ui.GetCubeName("TO"), att, numSamps, numLines, numBands);
    p.PropagateTables(false);
    p.ClearInputCubes();

    // propagate tables manually
    Pvl &inLabels = *cube->label();

    // Loop through the labels looking for object = Table
    for(int labelObj = 0; labelObj < inLabels.objects(); labelObj++) {
      PvlObject &obj = inLabels.object(labelObj);

      if(obj.name() != "Table") continue;

      // If we're not propagating spice data, dont propagate the following tables...
      if(!ui.GetBoolean("PROPSPICE")) {
        if((IString)obj["Name"][0] == "InstrumentPointing") continue;
        if((IString)obj["Name"][0] == "InstrumentPosition") continue;
        if((IString)obj["Name"][0] == "BodyRotation") continue;
        if((IString)obj["Name"][0] == "SunPosition") continue;
      }

      // Read the table into a table object
      Table table = cube->readTable(obj["Name"]);

      /*
      // We are not going to bother with line/sample associations; they apply
      //   only to the alpha cube at this time. I'm leaving this code here for the
      //   equations in case we try our hand at modifying these tables at a later date.

      // Deal with associations, sample first
      if(table.IsSampleAssociated()) {
        int numDeleted = 0;
        for(int samp = 0; samp < cube->sampleCount(); samp++) {
          // This tests checks to see if we would include this sample.
          //   samp - (startSamp-1)) / sinc must be a whole number less than numSamps.
          if((samp - (startSamp-1)) % sinc != 0 || (samp - (startSamp-1)) / sinc >= numSamps || (samp - (startSamp-1)) < 0) {
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
          //   line - (startLine-1)) / linc must be a whole number less than numLines.
          if((line - (startLine-1)) % linc != 0 || (line - (startLine-1)) / linc >= numLines || (line - (startLine-1)) < 0) {
            table.Delete(line-numDeleted);
            numDeleted ++;
          }
        }
      }*/

      // Write the table
      ocube->write(table);
    }

    Pvl &outLabels = *ocube->label();
    if(!ui.GetBoolean("PROPSPICE") && outLabels.findObject("IsisCube").hasGroup("Kernels")) {
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

    if (hasOverhang) {
      // crop for overhang, with extra checks/logic

      p.StartProcess(cropProcessPad);
    } else {
      // regular crop
      p.StartProcess(cropProccess);
    }

    delete in;
    in = NULL;

    // Construct a label with the results
    PvlGroup results("Results");
    results += PvlKeyword("InputLines", toString(orignl));
    results += PvlKeyword("InputSamples", toString(origns));
    results += PvlKeyword("StartingLine", toString(startLine));
    results += PvlKeyword("StartingSample", toString(startSamp));
    results += PvlKeyword("EndingLine", toString(endLine));
    results += PvlKeyword("EndingSample", toString(endSamp));
    results += PvlKeyword("LineIncrement", toString(linc));
    results += PvlKeyword("SampleIncrement", toString(sinc));
    results += PvlKeyword("OutputLines", toString(numLines));
    results += PvlKeyword("OutputSamples", toString(numSamps));

    // Update the Mapping, Instrument, and AlphaCube groups in the output
    // cube label
    SubArea *s;
    s = new SubArea;
    s->SetSubArea(orignl, origns, startLine, startSamp, endLine, endSamp, linc, sinc);
    s->UpdateLabel(cube, ocube, results);
    delete s;
    s = NULL;

    // Cleanup
    p.EndProcess();
    cube->close();
    delete cube;
    cube = NULL;

    // Write the results to the log
    return results;
  }
}
