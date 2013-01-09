#include "Isis.h"

#include <cmath>

#include "Angle.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "GroundGrid.h"
#include "IException.h"
#include "Latitude.h"
#include "Longitude.h"
#include "ProcessByLine.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "UniversalGroundMap.h"

using namespace std;
using namespace Isis;

void imageGrid(Buffer &in, Buffer &out);
void groundGrid(Buffer &in, Buffer &out);

void createGroundImage(Camera *cam, Projection *proj);

bool outline, ticks, diagonalTicks;
int baseLine, baseSample, lineInc, sampleInc, tickSize, lineWidth;
Latitude baseLat;
Longitude baseLon;
Angle latInc, lonInc;
double lineValue;
bool image;
double bkgndValue;

int inputSamples, inputLines;
GroundGrid *latLonGrid;

void IsisMain() {
  latLonGrid = NULL;

  // We will be processing by line
  ProcessByLine p;
  Cube *icube = p.SetInputCube("FROM");

  UserInterface &ui = Application::GetUserInterface();
  QString mode = ui.GetString("MODE");

  outline = ui.GetBoolean("OUTLINE");
  ticks = ui.GetBoolean("TICKS");

  if (ticks) {
    tickSize = ui.GetInteger("TICKSIZE") / 2;
    diagonalTicks = ui.GetBoolean("DIAGONALTICKS");
  }

  lineWidth = ui.GetInteger("LINEWIDTH") / 2;
 
  QString bval = ui.GetString("BKGNDVALUE").toUpper();

  image = (bval == "IMAGE");
  bkgndValue = Null;

  if (bval == "HRS") {
    bkgndValue = Hrs;
  }
  else if (bval == "LRS") {
    bkgndValue = Lrs;
  }
  else if (bval == "DN") {
    bkgndValue = ui.GetDouble("BKGNDDNVALUE");
  }

  QString lval = ui.GetString("LINEVALUE").toUpper();
  if (lval == "HRS") {
    lineValue = Hrs;
  }
  else if (lval == "LRS") {
    lineValue = Lrs;
  }
  else if (lval == "NULL") {
    lineValue = Null;
  }
  else if (lval == "DN") {
    if (ui.WasEntered("DNVALUE")) {
      lineValue = ui.GetDouble("DNVALUE");
    }
    else {
      throw IException(IException::User, "Must enter value in DNVALUE", _FILEINFO_);
    }
  }
  else {
    IString msg = "Invalid LINEVALUE string [" + ui.GetString("LINEVALUE");
    msg += "], must be one of HRS, LRS, NULL, or DN.";
    throw IException(IException::User, msg, _FILEINFO_);
  }

  inputSamples = icube->sampleCount();
  inputLines   = icube->lineCount();

  // Line & sample based grid
  if (mode == "IMAGE") {
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
    p.SetOutputCube(ui.GetFileName("TO"), oatt, icube->sampleCount(),
                    icube->lineCount(), icube->bandCount());

    UniversalGroundMap *gmap = new UniversalGroundMap(*icube,
        UniversalGroundMap::ProjectionFirst);
    latLonGrid = new GroundGrid(gmap, ticks, icube->sampleCount(), icube->lineCount());

    baseLat = Latitude(ui.GetDouble("BASELAT"),
        *latLonGrid->GetMappingGroup(), Angle::Degrees);
    baseLon = Longitude(ui.GetDouble("BASELON"),
        *latLonGrid->GetMappingGroup(), Angle::Degrees);
    latInc = Angle(ui.GetDouble("LATINC"), Angle::Degrees);
    lonInc = Angle(ui.GetDouble("LONINC"), Angle::Degrees);

    Progress progress;
    progress.SetText("Calculating Grid");

    Latitude minLat, maxLat;

    if (ui.WasEntered("MINLAT"))
      minLat = Latitude(ui.GetDouble("MINLAT"),
        *latLonGrid->GetMappingGroup(), Angle::Degrees);

    if (ui.WasEntered("MAXLAT"))
      maxLat = Latitude(ui.GetDouble("MAXLAT"),
        *latLonGrid->GetMappingGroup(), Angle::Degrees);

    Longitude minLon, maxLon;

    if (ui.WasEntered("MINLON"))
      minLon = Longitude(ui.GetDouble("MINLON"),
        *latLonGrid->GetMappingGroup(), Angle::Degrees);

    if (ui.WasEntered("MAXLON"))
      maxLon = Longitude(ui.GetDouble("MAXLON"),
        *latLonGrid->GetMappingGroup(), Angle::Degrees);

    latLonGrid->SetGroundLimits(minLat, minLon, maxLat, maxLon);

    latLonGrid->CreateGrid(baseLat, baseLon, latInc, lonInc, &progress);

    if (ui.GetBoolean("BOUNDARY"))
      latLonGrid->WalkBoundary();

    p.StartProcess(groundGrid);
    p.EndProcess();

    delete latLonGrid;
    latLonGrid = NULL;

    delete gmap;
    gmap = NULL;
  }
}

bool imageDrawLine(int line) {
  bool drawLine = false;

  for (int y = line - lineWidth; y <= line + lineWidth; y ++) {
    drawLine = drawLine || (y % lineInc == baseLine % lineInc);
  }

  return drawLine;
}

bool imageDrawSample(int samp) {
  bool drawSamp = false;

  for (int x = samp - lineWidth; x <= samp + lineWidth; x ++) {
    drawSamp = drawSamp || (x % sampleInc == baseSample % sampleInc);
  }

  return drawSamp;
}

// Line processing routine
void imageGrid(Buffer &in, Buffer &out) {
  for (int samp = 1; samp <= in.size(); samp ++) {
    if (image)
      out[samp - 1] = in[samp - 1];
    else 
      out[samp - 1] = bkgndValue;

    if (!ticks) {
      if (imageDrawSample(samp) || imageDrawLine(in.Line())) {
        out[samp - 1] = lineValue;
      }
    }
    // ticks!
    else {
      // tickSize is the width or height divided by 2, so integer rounding
      //   takes care of the current sample/line while doing +/- tickSize
      //   creates the appropriate width and height.

      // Vertical/Horizontal Ticks
      if (!diagonalTicks) {
        // horizontal test
        for (int sampleTest = samp - tickSize;
            (sampleTest <= samp + tickSize) && (out[samp - 1] != lineValue);
            sampleTest ++) {
          if (imageDrawLine(in.Line()) && imageDrawSample(sampleTest)) {
            out[samp - 1] = lineValue;
          }
        }

        // vertical test
        for (int lineTest = in.Line() - tickSize;
            (lineTest <= in.Line() + tickSize) && (out[samp - 1] != lineValue);
            lineTest ++) {
          if (imageDrawLine(lineTest) && imageDrawSample(samp)) {
            out[samp - 1] = lineValue;
          }
        }
      }
      // Diagonal Ticks
      else {
        // top left to bottom right
        int sampleTest = samp - tickSize;
        int lineTest = in.Line() - tickSize;

        while ((out[samp - 1] != lineValue) &&
              (lineTest <= in.Line() + tickSize) &&
              (sampleTest <= samp + tickSize)) {
          if (imageDrawLine(lineTest) && imageDrawSample(sampleTest)) {
            out[samp - 1] = lineValue;
          }

          sampleTest ++;
          lineTest ++;
        }

        // top right to bottom left
        sampleTest = samp + tickSize;
        lineTest = in.Line() - tickSize;

        while ((out[samp - 1] != lineValue) &&
              (lineTest <= in.Line() + tickSize) &&
              (sampleTest >= samp - tickSize)) {
          if (imageDrawLine(lineTest) && imageDrawSample(sampleTest)) {
            out[samp - 1] = lineValue;
          }

          sampleTest --;
          lineTest ++;
        }
      }
    }
  }

  // draw outline
  if (outline) {
    if (in.Line() - 1 <= lineWidth * 2 ||
       in.Line() >= inputLines - lineWidth * 2) {
      for (int i = 0; i < in.size(); i++) {
        out[i] = Hrs;
      }
    }
    else {
      for (int i = 0; i <= lineWidth * 2; i++)
        out[i] = Hrs;

      for (int i = inputSamples - lineWidth * 2 - 1; i < in.size(); i++)
        out[i] = Hrs;
    }
  }
}

bool groundDrawPoint(int samp, int line, bool latGrid = true) {
  bool drawPoint = false;

  for (int x = samp - lineWidth; x <= samp + lineWidth; x ++) {
    drawPoint = drawPoint || latLonGrid->PixelOnGrid(x - 1, line - 1, latGrid);
  }

  for (int y = line - lineWidth; y <= line + lineWidth; y ++) {
    drawPoint = drawPoint || latLonGrid->PixelOnGrid(samp - 1, y - 1, latGrid);
  }

  return drawPoint;
}

// Line processing routine
void groundGrid(Buffer &in, Buffer &out) {
  for (int samp = 1; samp <= in.SampleDimension(); samp++) {
    if (!ticks) {
      if (groundDrawPoint(samp, in.Line())) {
        out[samp - 1] = Hrs;
      }
      else {
        if (image)
          out[samp - 1] = in[samp - 1];
        else
          out[samp - 1] = bkgndValue;
      }
    }
    else {
      // We need to check the grids for overlaps near current point
      out[samp - 1] = in[samp - 1];

      // tickSize is the width or height divided by 2, so integer rounding
      //   takes care of the current sample/line while doing +/- tickSize
      //   creates the appropriate width and height.

      // Vertical/Horizontal Ticks
      if (!diagonalTicks) {
        // horizontal test
        for (int sampleTest = samp - tickSize;
            (sampleTest <= samp + tickSize) && (out[samp - 1] != Hrs);
            sampleTest ++) {
          if (groundDrawPoint(sampleTest, in.Line(), true) &&
              groundDrawPoint(sampleTest, in.Line(), false)) {
            out[samp - 1] = Hrs;
          }
        }

        // vertical test
        for (int lineTest = in.Line() - tickSize;
            (lineTest <= in.Line() + tickSize) && (out[samp - 1] != Hrs);
            lineTest ++) {
          if (groundDrawPoint(samp, lineTest, true) &&
              groundDrawPoint(samp, lineTest, false)) {
            out[samp - 1] = Hrs;
          }
        }
      }
      // Diagonal ticks
      else {
        // top left to bottom right
        int sampleTest = samp - tickSize;
        int lineTest = in.Line() - tickSize;


        while ((out[samp - 1] != Hrs) &&
              (lineTest <= in.Line() + tickSize) &&
              (sampleTest <= samp + tickSize)) {
          if (groundDrawPoint(sampleTest, lineTest, true) &&
              groundDrawPoint(sampleTest, lineTest, false)) {
            out[samp - 1] = Hrs;
          }

          sampleTest ++;
          lineTest ++;
        }

        // top right to bottom left
        sampleTest = samp + tickSize;
        lineTest = in.Line() - tickSize;

        while ((out[samp - 1] != Hrs) &&
              (lineTest <= in.Line() + tickSize) &&
              (sampleTest >= samp - tickSize)) {
          if (groundDrawPoint(sampleTest, lineTest, true) &&
              groundDrawPoint(sampleTest, lineTest, false)) {
            out[samp - 1] = Hrs;
          }

          sampleTest --;
          lineTest ++;
        }
      }
    }
  }

  // draw outline
  if (outline) {
    if (in.Line() - 1 <= lineWidth * 2 ||
       in.Line() >= inputLines - lineWidth * 2) {
      for (int i = 0; i < in.size(); i++) {
        out[i] = Hrs;
      }
    }
    else {
      for (int i = 0; i <= lineWidth * 2; i++)
        out[i] = Hrs;

      for (int i = inputSamples - lineWidth * 2 - 1; i < in.size(); i++)
        out[i] = Hrs;
    }
  }
}
