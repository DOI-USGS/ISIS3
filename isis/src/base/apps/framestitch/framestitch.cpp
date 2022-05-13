#include "framestitch.h"

#include <QString>
#include <iostream>

#include "Brick.h"
#include "Buffer.h"
#include "Cube.h"
#include "CubeAttribute.h"
#include "Camera.h"
#include "IException.h"
#include "IString.h"
#include "LineManager.h"
#include "ProcessByBrick.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "Statistics.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

namespace Isis {

  /**
   * Helper function to count the height of frames in a cube.
   *
   * Iterates over the lines in the cube and counts sequential all NULL lines
   * assuming these are frames that have been removed from the cube.
   *
   * If different frame heights are computed within the cube or there are
   * no all null lines in the cube an error is raised.
   */
  int computeFrameHeight(Cube *cube) {
    // Use a Statistics object to track min/max height count
    Statistics frameHeights;
    LineManager cubeLine(*cube);
    int currentFrameHeight = 0;
    for (int line = 1; line <= cube->lineCount(); line++) {
      cubeLine.SetLine(line);
      cube->read(cubeLine);
      Statistics lineStats;
      lineStats.AddData(cubeLine.DoubleBuffer(), cubeLine.size());
      // If the line is all NULL add it to the current frame count
      if (lineStats.TotalPixels() == lineStats.NullPixels()) {
        ++currentFrameHeight;
      }
      // If the line has non-NULL pixels and we have previously counted
      // some all null lines add the count to our histogram and reset
      else if (currentFrameHeight > 0) {
        frameHeights.AddData(currentFrameHeight);
        currentFrameHeight = 0;
      }
    }

    // If the last line is part of a NULL frame handle it now
    if (currentFrameHeight > 0) {
      frameHeights.AddData(currentFrameHeight);
    }

    if (frameHeights.TotalPixels() == 0) {
      QString msg = "Failed to find any NULL frames in cube [" + cube->fileName() + "]."
                    "Please manually enter the frame height.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    if (frameHeights.Minimum() != frameHeights.Maximum()) {
      QString msg = "Found different frame heights between [" + toString((int)frameHeights.Minimum())
                  + "] and [" + toString((int)frameHeights.Maximum()) + "] lines in cube ["
                  + cube->fileName() + "]. Please manually enter the frame height.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    // The statistics object contains only a single value at this point
    // so minimum=average=maximum
    return frameHeights.Average();
  }

  /**
   * Combine even and odd cubes from a push frame image into a single cube.
   *
   * @param ui The User Interface to parse the parameters from
   */
  void framestitch(UserInterface &ui) {
    ProcessByBrick process;

    // It is very important that the odd cube gets added first as later on
    // we'll use frameNumber % 2 to index the input cube vector
    QString oddCubeFile = ui.GetCubeName("ODD");
    Cube* oddCube = process.SetInputCube(oddCubeFile,
                                         CubeAttributeInput(oddCubeFile));
    QString evenCubeFile = ui.GetCubeName("EVEN");
    Cube* evenCube = process.SetInputCube(evenCubeFile,
                                          CubeAttributeInput(evenCubeFile));


    // Check that all the inputs are valid
    if (evenCube->sampleCount() != oddCube->sampleCount() ||
        evenCube->lineCount() != oddCube->lineCount() ||
        evenCube->bandCount() != oddCube->bandCount()) {
      QString msg = "Even and odd cube dimensions must match.";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    bool inputFlipped = false;

    if (evenCube->hasGroup("Instrument") && oddCube->hasGroup("Instrument")) {
      const PvlGroup &evenInst = evenCube->group("Instrument");
      const PvlGroup &oddInst = oddCube->group("Instrument");
      // Use the start time as an indicator of being the same original image
      if (QString::compare(QString(evenInst["StartTime"]), QString(oddInst["StartTime"])) != 0) {
        QString msg = "Even and odd cube start times must match.";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      if (evenInst.hasKeyword("DataFlipped") && oddInst.hasKeyword("DataFlipped")) {
        if (toBool(evenInst["DataFlipped"]) != toBool(oddInst["DataFlipped"])) {
          QString msg = "Both input cubes must be flipped or not flipped. Cannot combine "
                        "a flipped and unflipped cube.";
          throw IException(IException::User, msg, _FILEINFO_);
        }
        inputFlipped = toBool(evenInst["DataFlipped"]);
      }
    }

    QString outCubeFile = ui.GetCubeName("TO");
    Cube *outCube = process.SetOutputCube(
          outCubeFile, CubeAttributeOutput(outCubeFile),
          evenCube->sampleCount(), evenCube->lineCount(), evenCube->bandCount());

    int frameHeight = 1;
    if (ui.WasEntered("frameHeight")) {
      frameHeight = ui.GetInteger("FRAMEHEIGHT");
    } else {
      // User didn't pass the size of the frames so attempt to infer it from
      // the cubes
      int evenFrameHeight = computeFrameHeight(evenCube);
      int oddFrameHeight = computeFrameHeight(oddCube);

      if (evenFrameHeight != oddFrameHeight) {
        QString msg = "Computed frame heights for even cube [" + toString(evenFrameHeight)
                    + "] and odd cube [" + toString(oddFrameHeight) + "] do not match.";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      frameHeight = evenFrameHeight;
    }

    int numLinesOverlap = 0; // The default value
    if (ui.WasEntered("NUM_LINES_OVERLAP")) {
      numLinesOverlap = ui.GetInteger("NUM_LINES_OVERLAP");
    }
    if (numLinesOverlap %2 != 0 || numLinesOverlap < 0) {
      QString msg = "Expecting a non-negative and even value for NUM_LINES_OVERLAP.";
      throw IException(IException::User, msg, _FILEINFO_);
    }
    
    // If there's an even number of frames and the inputs are flipped, we have to
    // swap even and odd because the first frame in the even cube is valid and the
    // first frame in the odd cube is now all NULL.
    //
    //  Before   --Flip-->  After
    // Even  Odd          Even  Odd
    // 0000  ####         ####  0000
    // ####  0000         0000  ####
    // 0000  ####         ####  0000
    // ####  0000         0000  ####
    //
    int numFrames = evenCube->lineCount() / frameHeight;
    bool swapInputCubes = inputFlipped && numFrames % 2 == 0;

    // Processing Setup
    process.SetBrickSize(evenCube->sampleCount(), frameHeight, evenCube->bandCount());
    process.PropagateTables(false);
    process.PropagatePolygons(false);

    /**
     * Frame stitching processing function
     */
    auto interweaveFrames = [&](std::vector<Buffer *> &in, std::vector<Buffer *> &out)->void {
      // Assumes odd is first and even is second
      int inIndex = (in[0]->Line() / frameHeight) % 2;
      if (swapInputCubes) {
        inIndex = 1 - inIndex;
      }
      // in[0] is first input cube and in[1] is the second. Copy from
      // one of these, depending on how inIndex changes, to out[0],
      // which is the only output cube.
      out[0]->Copy(*in[inIndex]);
    };

    process.StartProcess(interweaveFrames);

    // Update the output label
    outCube->deleteGroup("Kernels");
    if (!outCube->hasGroup("Instrument")) {
      outCube->putGroup(PvlGroup("Instrument"));
    }
    PvlGroup &outInst = outCube->group("Instrument");
    if (!outInst.hasKeyword("Framelets")) {
      outInst.addKeyword(PvlKeyword("Framelets"));
    }
    outInst["Framelets"].setValue("All");

    // At this stage the merging of the two input cubes has been scheduled,
    // with the result in outCube.
    
    // ProcessByBrick requires that all buffers move through the cubes
    // the same way so we have to do a second manual pass to flip the
    // output cube if requested.
    if (ui.GetBoolean("FLIP")) {
      // Need a temporary buffer to help with the swapping of top and
      // bottom portions of outCube.
      Brick topBuff(outCube->sampleCount(), frameHeight, outCube->bandCount(), outCube->pixelType());
      Brick botBuff(outCube->sampleCount(), frameHeight, outCube->bandCount(), outCube->pixelType());
      Brick tmpBuff(outCube->sampleCount(), frameHeight, outCube->bandCount(), outCube->pixelType());

      for (int frame = 0; frame < numFrames / 2; frame++) {
        topBuff.SetBasePosition(1,frame * frameHeight + 1,1);
        outCube->read(topBuff);
        botBuff.SetBasePosition(1,outCube->lineCount() - (frame + 1) * frameHeight + 1,1);
        outCube->read(botBuff);

        tmpBuff.Copy(topBuff);
        topBuff.Copy(botBuff);
        outCube->write(topBuff);
        botBuff.Copy(tmpBuff);
        outCube->write(botBuff);
      }

      if (!outInst.hasKeyword("DataFlipped")) {
        outInst.addKeyword(PvlKeyword("DataFlipped"));
      }
      outInst["DataFlipped"].setValue(toString(!inputFlipped));
    }
  
    // Wipe numLinesOverlap/2 lines at the top and same amount at the bottom
    // of each framelet to reduce their overlap.

    Brick srcBuff(outCube->sampleCount(), 1, outCube->bandCount(), outCube->pixelType());
    Brick dstBuff(outCube->sampleCount(), 1, outCube->bandCount(), outCube->pixelType());
    
    for (int line = 0; line < outCube->lineCount(); line++) {
      int frame = line / frameHeight; // block index
      int lineInFrame = line - frameHeight * frame; // line index in the current block
      
      if (lineInFrame < numLinesOverlap / 2) 
        continue; // these get wiped
      
      if (lineInFrame >= frameHeight - numLinesOverlap / 2) {
        continue; // gets wiped 
      }

      // Same book-keeping is needed to compute where the line will go
      int src = line;
      int dst = lineInFrame - numLinesOverlap / 2 + (frameHeight - numLinesOverlap) * frame;

      srcBuff.SetBasePosition(1, src + 1, 1);
      dstBuff.SetBasePosition(1, dst + 1, 1);

      outCube->read(srcBuff);  // read into srcBuf
      dstBuff.Copy(srcBuff);   // copy from srcBuff to dstBuff
      outCube->write(dstBuff); // write into outCube
    }
    
    // Wipe the last lines of the image after they are transferred
    // TODO: Figure out how to resize the cube.
    for (int line = outCube->lineCount() - numFrames * numLinesOverlap;
         line < outCube->lineCount(); line++) {

      srcBuff.SetBasePosition(1, line + 1, 1);
      outCube->read(srcBuff);

      for (int it = 0; it < srcBuff.size(); it++)
        srcBuff[it] = Isis::NULL8;
      
      outCube->write(srcBuff);
    }
    
    process.EndProcess();
    
    return;
  }

} // end namespace Isis
