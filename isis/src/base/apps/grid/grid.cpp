#include "Isis.h"

#include <cmath>

#include "ProcessByLine.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "GroundGrid.h"
#include "UniversalGroundMap.h"

using namespace std;
using namespace Isis;

void imageGrid(Buffer &in, Buffer &out);
void groundGrid(Buffer &in, Buffer &out);

void createGroundImage(Camera *cam, Projection *proj);

bool outline, ticks, diagonalTicks;
int baseLine, baseSample, lineInc, sampleInc, tickSize;
double baseLat, baseLon, latInc, lonInc;

int inputSamples, inputLines;
GroundGrid *latLonGrid;

void IsisMain() {
  latLonGrid = NULL;

  // We will be processing by line
  ProcessByLine p;
  Cube *icube = p.SetInputCube("FROM");

  UserInterface &ui = Application::GetUserInterface();
  string mode = ui.GetString("MODE");

  outline = ui.GetBoolean("OUTLINE");
  ticks = ui.GetBoolean("TICKS");

  if(ticks) {
    tickSize = ui.GetInteger("TICKSIZE") / 2;
    diagonalTicks = ui.GetBoolean("DIAGONALTICKS");
  }

  inputSamples = icube->Samples();
  inputLines   = icube->Lines();

  // Line & sample based grid
  if(mode == "IMAGE") {
    p.SetOutputCube("TO");
    baseLine = ui.GetInteger("BASELINE");
    baseSample = ui.GetInteger("BASESAMPLE");
    lineInc = ui.GetInteger("LINC");
    sampleInc = ui.GetInteger("SINC");
    p.StartProcess(imageGrid);
    p.EndProcess();
  }
  // Lat/Lon based grid
  else {
    CubeAttributeOutput oatt("+32bit");
    p.SetOutputCube(ui.GetFilename("TO"), oatt, icube->Samples(),
                    icube->Lines(), icube->Bands());

    baseLat = ui.GetDouble("BASELAT");
    baseLon = ui.GetDouble("BASELON");
    latInc = ui.GetDouble("LATINC");
    lonInc = ui.GetDouble("LONINC");

    UniversalGroundMap *gmap = new UniversalGroundMap(*icube);
    latLonGrid = new GroundGrid(gmap, ticks, icube->Samples(), icube->Lines());

    Progress progress;
    progress.SetText("Calculating Grid");

    latLonGrid->CreateGrid(baseLat, baseLon, latInc, lonInc, &progress);

    p.StartProcess(groundGrid);
    p.EndProcess();

    delete latLonGrid;
    latLonGrid = NULL;

    delete gmap;
    gmap = NULL;
  }
}

bool imageDrawLine(int line) {
  return (line % lineInc == baseLine % lineInc);
}

bool imageDrawSample(int sample) {
  return (sample % sampleInc == baseSample % sampleInc);
}

// Line processing routine
void imageGrid(Buffer &in, Buffer &out) {
  for(int samp = 1; samp <= in.size(); samp ++) {
    out[samp - 1] = in[samp - 1];

    if(!ticks) {
      if(imageDrawSample(samp) || imageDrawLine(in.Line()))
        out[samp - 1] = Isis::Hrs;
    }
    // ticks!
    else {
      // tickSize is the width or height divided by 2, so integer rounding
      //   takes care of the current sample/line while doing +/- tickSize
      //   creates the appropriate width and height.

      // Vertical/Horizontal Ticks
      if(!diagonalTicks) {
        // horizontal test
        for(int sampleTest = samp - tickSize;
            (sampleTest <= samp + tickSize) && (out[samp-1] != Isis::Hrs);
            sampleTest ++) {
          if(imageDrawLine(in.Line()) && imageDrawSample(sampleTest)) {
            out[samp - 1] = Isis::Hrs;
          }
        }

        // vertical test
        for(int lineTest = in.Line() - tickSize;
            (lineTest <= in.Line() + tickSize) && (out[samp-1] != Isis::Hrs);
            lineTest ++) {
          if(imageDrawLine(lineTest) && imageDrawSample(samp)) {
            out[samp-1] = Isis::Hrs;
          }
        }
      }
      // Diagonal Ticks
      else {
        // top left to bottom right
        int sampleTest = samp - tickSize;
        int lineTest = in.Line() - tickSize;

        while((out[samp-1] != Isis::Hrs) && 
              (lineTest <= in.Line() + tickSize) &&
              (sampleTest <= samp + tickSize)) {
          if(imageDrawLine(lineTest) && imageDrawSample(sampleTest)) {
            out[samp-1] = Isis::Hrs;
          }

          sampleTest ++;
          lineTest ++;
        }

        // top right to bottom left
        sampleTest = samp + tickSize;
        lineTest = in.Line() - tickSize;

        while((out[samp-1] != Isis::Hrs) && 
              (lineTest <= in.Line() + tickSize) &&
              (sampleTest >= samp - tickSize)) {
          if(imageDrawLine(lineTest) && imageDrawSample(sampleTest)) {
            out[samp-1] = Isis::Hrs;
          }

          sampleTest --;
          lineTest ++;
        }
      }
    }
  }

  // draw outline
  if(outline) {
    if(in.Line() == 1 || in.Line() == inputLines) {
      for(int i = 0; i < in.size(); i++) {
        out[i] = Isis::Hrs;
      }
    }
    else {
      out[0] = Isis::Hrs;
      out[out.size()-1] = Isis::Hrs;
    }
  }
}

// Line processing routine
void groundGrid(Buffer &in, Buffer &out) {
  for(int samp = 1; samp <= in.SampleDimension(); samp++) {
    if(!ticks) {
      if(latLonGrid->PixelOnGrid(samp - 1, in.Line() - 1)) {
        out[samp-1] = Isis::Hrs;
      }
      else {
        out[samp-1] = in[samp-1];
      }
    }
    else {
      // We need to check the grids for overlaps near current point
      out[samp-1] = in[samp-1];

      // tickSize is the width or height divided by 2, so integer rounding
      //   takes care of the current sample/line while doing +/- tickSize
      //   creates the appropriate width and height.

      // Vertical/Horizontal Ticks
      if(!diagonalTicks) {
        // horizontal test
        for(int sampleTest = samp - 1 - tickSize;
            (sampleTest < samp + tickSize) && (out[samp-1] != Isis::Hrs);
            sampleTest ++) {
          if(latLonGrid->PixelOnGrid(sampleTest, in.Line() - 1, true) &&
              latLonGrid->PixelOnGrid(sampleTest, in.Line() - 1, false)) {
            out[samp-1] = Isis::Hrs;
          }
        }

        // vertical test
        for(int lineTest = in.Line() - 1 - tickSize;
            (lineTest < in.Line() + tickSize) && (out[samp-1] != Isis::Hrs);
            lineTest ++) {
          if(latLonGrid->PixelOnGrid(samp - 1, lineTest, true) &&
              latLonGrid->PixelOnGrid(samp - 1, lineTest, false)) {
            out[samp-1] = Isis::Hrs;
          }
        }
      }
      // Diagonal ticks
      else {
        // top left to bottom right
        int sampleTest = samp - 1 - tickSize;
        int lineTest = in.Line() - 1 - tickSize;


        while((out[samp-1] != Isis::Hrs) && (lineTest < in.Line() + tickSize) &&
              (sampleTest < samp + tickSize)) {
          if(latLonGrid->PixelOnGrid(sampleTest, lineTest, true) &&
              latLonGrid->PixelOnGrid(sampleTest, lineTest, false)) {
            out[samp-1] = Isis::Hrs;
          }

          sampleTest ++;
          lineTest ++;
        }

        // top right to bottom left
        sampleTest = samp - 1 + tickSize;
        lineTest = in.Line() - 1 - tickSize;

        while((out[samp-1] != Isis::Hrs) && (lineTest < in.Line() + tickSize) &&
              (sampleTest >= samp - 1 - tickSize)) {
          if(latLonGrid->PixelOnGrid(sampleTest, lineTest, true) &&
              latLonGrid->PixelOnGrid(sampleTest, lineTest, false)) {
            out[samp-1] = Isis::Hrs;
          }

          sampleTest --;
          lineTest ++;
        }
      }
    }
  }

  // draw outline
  if(outline) {
    if(in.Line() == 1 || in.Line() == inputLines) {
      for(int i = 0; i < in.size(); i++) {
        out[i] = Isis::Hrs;
      }
    }
    else {
      out[0] = Isis::Hrs;
      out[out.size()-1] = Isis::Hrs;
    }
  }
}
