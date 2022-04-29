#include "framestitch.h"

#include <QString>

#include "Brick.h"
#include "Buffer.h"
#include "Cube.h"
#include "CubeAttribute.h"
#include "Camera.h"
#include "Histogram.h"
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
   * Helper function to count the size of frames in a cube
   */
  int computeFrameSize(Cube *cube) {
    // Use a histogram to accumulate frame counts
    Histogram frameSizes(0.0, 1024.0, 1024);
    LineManager cubeLine(*cube);
    int currentFrameSize = 0;
    for (int line = 1; line <= cube->lineCount(); line++) {
      cubeLine.SetLine(line);
      cube->read(cubeLine);
      Statistics lineStats;
      lineStats.AddData(cubeLine.DoubleBuffer(), cubeLine.size());
      // If the line is all NULL add it to the current frame count
      if (lineStats.TotalPixels() == lineStats.NullPixels()) {
        ++currentFrameSize;
      }
      // If the line has non-NULL pixels and we have previously counted
      // some all null lines add the count to our histogram and reset
      else if (currentFrameSize > 0) {
        frameSizes.AddData(currentFrameSize);
        currentFrameSize = 0;
      }
    }

    // If the last line is part of a NULL frame handle it now
    if (currentFrameSize > 0) {
      frameSizes.AddData(currentFrameSize);
    }

    // Special case where there are no all NULL lines
    if (frameSizes.TotalPixels() == 0) {
      return 0;
    }

    // Use the median to handle any outliers
    return frameSizes.Median();
  }

  /**
   * Compute the stats for an ISIS cube. This is the programmatic interface to
   * the ISIS stats application.
   *
   * @param ui The User Interface to parse the parameters from
   */
  void framestitch(UserInterface &ui) {
    ProcessByBrick process;

    // It is very important that the even cube gets added first as later on
    // we'll use frameNumber % 2 to index the input cube vector
    QString evenCubeFile = ui.GetCubeName("EVEN");
    Cube* evenCube = process.SetInputCube(evenCubeFile,
                                          CubeAttributeInput(evenCubeFile));
    QString oddCubeFile = ui.GetCubeName("ODD");
    Cube* oddCube = process.SetInputCube(oddCubeFile,
                                         CubeAttributeInput(oddCubeFile));


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

    int frameSize = 1;
    if (ui.WasEntered("FRAMESIZE")) {
      frameSize = ui.GetInteger("FRAMESIZE");
    }
    else {
      // User didn't pass the size of the frames so attempt to infer it from
      // the cubes
      int evenFrameSize = computeFrameSize(evenCube);
      int oddFrameSize = computeFrameSize(oddCube);

      if (evenFrameSize == 0) {
        QString msg = "Failed to find any NULL frames in even cube when attempting "
                      "to compute frame size.";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      if (oddFrameSize == 0) {
        QString msg = "Failed to find any NULL frames in odd cube when attempting "
                      "to compute frame size.";
        throw IException(IException::User, msg, _FILEINFO_);
      }
      if (evenFrameSize != oddFrameSize) {
        QString msg = "Computed frame sizes for even cube [" + toString(evenFrameSize)
                    + "] and odd cube [" + toString(oddFrameSize) + "] do not match.";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      frameSize = evenFrameSize;
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
    int numFrames = evenCube->lineCount() / frameSize;
    bool swapInputCubes = inputFlipped && numFrames % 2 == 0;

    // Processing Setup
    process.SetBrickSize(evenCube->sampleCount(), frameSize, evenCube->bandCount());
    process.PropagateTables(false);
    process.PropagatePolygons(false);

    /**
     * Frame stitching processing function
     */
    auto interweaveFrames = [&](std::vector<Buffer *> &in, std::vector<Buffer *> &out)->void {
      // Assumes even is first and odd is second
      int inIndex = (in[0]->Line() / frameSize) % 2;
      if (swapInputCubes) {
        inIndex = 1 - inIndex;
      }
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

    // ProcessByBrick requires that all buffers move through the cubes the same
    // way so we have to do a second manual pass to flip the output cube if requested.
    if (ui.GetBoolean("FLIP")) {
      Brick topBuff(outCube->sampleCount(), frameSize, outCube->bandCount(), outCube->pixelType());
      Brick bottomBuff(outCube->sampleCount(), frameSize, outCube->bandCount(), outCube->pixelType());
      Brick tempBuff(outCube->sampleCount(), frameSize, outCube->bandCount(), outCube->pixelType());

      for (int frame = 0; frame < numFrames / 2; frame++) {
        topBuff.SetBasePosition(1,frame * frameSize + 1,1);
        outCube->read(topBuff);
        bottomBuff.SetBasePosition(1,outCube->lineCount() - (frame + 1) * frameSize + 1,1);
        outCube->read(bottomBuff);

        tempBuff.Copy(topBuff);
        topBuff.Copy(bottomBuff);
        outCube->write(topBuff);
        bottomBuff.Copy(tempBuff);
        outCube->write(bottomBuff);
      }

      if (!outInst.hasKeyword("DataFlipped")) {
        outInst.addKeyword(PvlKeyword("DataFlipped"));
      }
      outInst["DataFlipped"].setValue(toString(!inputFlipped));
    }

    process.EndProcess();

  }

}
