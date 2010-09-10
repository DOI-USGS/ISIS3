#include "Isis.h"
#include "ProcessByLine.h"
#include "Projection.h"
#include "SpecialPixel.h"
#include "LineManager.h"
#include "OriginalLabel.h"
#include "History.h"
#include "Table.h"

using namespace std;
using namespace Isis;

void DoWrap(Buffer &in);

Cube *ocube;

int leftPad;
int rightPad;
int topPad;
int bottomPad;
int inl;
double minRadius;
double maxRadius;

void IsisMain() {
  // We will be using a mosaic technique so get the size of the input file
  ProcessByLine p;
  Cube *icube = p.SetInputCube("FROM");
  int ins = icube->Samples();
  inl = icube->Lines();
  int inb = icube->Bands();

  PvlGroup mapgrp = icube->Label()->FindGroup("Mapping", Pvl::Traverse);
  bool hasExtents = false;
  bool isGlobal = false;
  double minLat,maxLat,minLon,maxLon;
  if (mapgrp.HasKeyword("MinimumLatitude") && mapgrp.HasKeyword("MaximumLatitude") &&
      mapgrp.HasKeyword("MinimumLongitude") && mapgrp.HasKeyword("MaximumLongitude")) {
    hasExtents = true;
    minLat = mapgrp["MinimumLatitude"];
    maxLat = mapgrp["MaximumLatitude"];
    minLon = mapgrp["MinimumLongitude"];
    maxLon = mapgrp["MaximumLongitude"];
    if ((maxLat - minLat) >= 180.0 && (maxLon - minLon) >= 360.0) isGlobal = true;
  }

  Projection *proj = icube->Projection();
  if(proj == NULL) {
    iString message = "The input cube must be a DEM file, which means it must be projected. ";
    message += "This file is not map projected.";
    throw iException::Message(iException::User, message, _FILEINFO_);
  }

  if(!proj->IsEquatorialCylindrical()) {
    iString message = "The input cube must have an equatorial cylindrical projection.";
    throw iException::Message(iException::User, message, _FILEINFO_);
  }

  if (proj->LatitudeTypeString() != "Planetocentric") {
    iString message = "The input cube must have Planetocentric latitude type.";
    throw iException::Message(iException::User, message, _FILEINFO_);
  }

  // Determine if the file is global
  bool isPadded = false;
  int insideImage = 0;
  if (!hasExtents) {
    if (proj->LongitudeDomainString() == "360") {
      proj->SetGround(90.0,0.0);
      if (proj->IsGood()) {
        if (proj->WorldX() > 0.0 && proj->WorldX() < ins+1 &&
            proj->WorldY() > 0.0 && proj->WorldY() < inl+1) {
          insideImage = insideImage + 1;
        }
        proj->SetGround(90.0,360.0);
      }
      if (proj->IsGood()) {
        if (proj->WorldX() > 0.0 && proj->WorldX() < ins+1 &&
            proj->WorldY() > 0.0 && proj->WorldY() < inl+1) {
          insideImage = insideImage + 1;
        }
        proj->SetGround(-90.0,0.0);
      }
      if (proj->IsGood()) {
        if (proj->WorldX() > 0.0 && proj->WorldX() < ins+1 &&
            proj->WorldY() > 0.0 && proj->WorldY() < inl+1) {
          insideImage = insideImage + 1;
        }
        proj->SetGround(-90.0,360.0);
      }
      if (proj->IsGood()) {
        if (proj->WorldX() > 0.0 && proj->WorldX() < ins+1 &&
            proj->WorldY() > 0.0 && proj->WorldY() < inl+1) {
          insideImage = insideImage + 1;
        }
      }
      if (proj->IsGood() && insideImage == 4) isGlobal = true;
    } else {
      proj->SetGround(90.0,-180.0);
      if (proj->IsGood()) {
        if (proj->WorldX() > 0.0 && proj->WorldX() < ins+1 &&
            proj->WorldY() > 0.0 && proj->WorldY() < inl+1) {
          insideImage = insideImage + 1;
        }
        proj->SetGround(90.0,180.0);
      }
      if (proj->IsGood()) {
        if (proj->WorldX() > 0.0 && proj->WorldX() < ins+1 &&
            proj->WorldY() > 0.0 && proj->WorldY() < inl+1) {
          insideImage = insideImage + 1;
        }
        proj->SetGround(-90.0,-180.0);
      }
      if (proj->IsGood()) { 
        if (proj->WorldX() > 0.0 && proj->WorldX() < ins+1 &&
            proj->WorldY() > 0.0 && proj->WorldY() < inl+1) {
          insideImage = insideImage + 1;
        }
        proj->SetGround(-90.0,180.0);
      }
      if (proj->IsGood()) { 
        if (proj->WorldX() > 0.0 && proj->WorldX() < ins+1 &&
            proj->WorldY() > 0.0 && proj->WorldY() < inl+1) {
          insideImage = insideImage + 1;
        }
      }
      if (proj->IsGood() && insideImage == 4) isGlobal = true;
    }
  }

  if (isGlobal) {
    if (proj->LongitudeDomainString() == "360") {
      if (proj->LongitudeDirectionString() == "PositiveEast") {
        proj->SetGround(90.0,0.0);
      } else {
        proj->SetGround(90.0,360.0);
      }
      if (proj->WorldX() >= 1.0 && proj->WorldY() >= 1.0) isPadded = true;
    } else {
      if (proj->LongitudeDirectionString() == "PositiveEast") {
        proj->SetGround(90.0,-180.0);
      } else {
        proj->SetGround(90.0,180.0);
      }
      if (proj->WorldX() >= 1.0 && proj->WorldY() >= 1.0) isPadded = true;
    }
  }

  // If the file isn't global, then determine if it contains either
  // the south or north pole
  bool hasSPole = false;
  bool hasNPole = false;
  if (!isGlobal) {
    proj->SetGround(90.0,0.0);
    if (proj->IsGood()) {
      if (proj->WorldY() > 0.0 && proj->WorldY() < inl+1) {
        hasNPole = true;
        if (proj->WorldY() >= 1.0) isPadded = true;
      }
    }
    proj->SetGround(-90.0,0.0);
    if (proj->IsGood()) {
      if (proj->WorldY() > 0.0 && proj->WorldY() < inl+1) {
        hasSPole = true;
        if (proj->WorldY() <= inl) isPadded = true;
      }
    }
  }

  // Set the padding parameters
  leftPad = 0;
  rightPad = 0;
  topPad = 0;
  bottomPad = 0;

  if (isGlobal && !isPadded) {
    leftPad = 1;
    rightPad = 1;
    topPad = 1;
    bottomPad = 1;
  }
  if (!isGlobal && !isPadded) {
    leftPad = 0;
    rightPad = 0;
    if (hasNPole) {
      topPad = 1;
    } else {
      topPad = 0;
    }
    if (hasSPole) {
      bottomPad = 1;
    } else {
      bottomPad = 0;
    }
  }

  // Compute the output size
  int ns = ins + leftPad + rightPad;
  int nl = inl + topPad + bottomPad;
  int nb = inb;

  double upperLeftCorner = mapgrp["UpperLeftCornerX"];
  upperLeftCorner -= leftPad * proj->Resolution();
  mapgrp.AddKeyword(PvlKeyword("UpperLeftCornerX", upperLeftCorner, "meters"),
                    Pvl::Replace);

  upperLeftCorner = mapgrp["UpperLeftCornerY"];
  upperLeftCorner += topPad * proj->Resolution();
  mapgrp.AddKeyword(PvlKeyword("UpperLeftCornerY", upperLeftCorner, "meters"),
                    Pvl::Replace);


  p.SetOutputCube("TO", ns, nl, nb);
  // Make sure everything is propagated and closed
  p.EndProcess();

  // Now we'll really be processing our input cube
  p.SetInputCube("FROM");

  // We need to create the output file
  ocube = new Cube();
  UserInterface &ui = Application::GetUserInterface();
  ocube->Open(Filename(ui.GetFilename("TO")).Expanded(), "rw");

  minRadius = DBL_MAX;
  maxRadius = DBL_MIN;
  p.StartProcess(DoWrap);

  // Update mapping grp
  ocube->PutGroup(mapgrp);
  
  // Store min/max radii values in new ShapeModelStatistics table
  string shp_name = "ShapeModelStatistics";
  TableField fmin("MinimumRadius",Isis::TableField::Double);
  TableField fmax("MaximumRadius",Isis::TableField::Double);

  TableRecord record;
  record += fmin;
  record += fmax;

  Table table(shp_name,record);

  record[0] = minRadius/1000.0;
  record[1] = maxRadius/1000.0;
  table += record;

  ocube->Write(table);

  p.EndProcess();
  ocube->Close();
  delete ocube;
}

void DoWrap(Buffer &in) {
  LineManager outMan(*ocube);

  int inputSize = in.size();
  outMan.SetLine(in.Line() + topPad);
  int outputSize = outMan.size();
  double avg = 0.0;

  if ((topPad == 1 && in.Line() == 1) || (bottomPad == 1 && in.Line() == inl)) {
    for(int outputIndex = 0; outputIndex < outputSize; outputIndex++) {
      int inputIndex = outputIndex - leftPad;
      if(inputIndex < 0) {
        avg = avg + in[inputIndex + inputSize];
      }
      else if(inputIndex < inputSize) {
        avg = avg + in[inputIndex];
      }
      else {
        avg = avg + in[inputIndex - inputSize];
      }
    }
    avg = avg / outputSize;
    if (avg < minRadius) minRadius = avg;
    if (avg > maxRadius) maxRadius = avg;
    if (topPad == 1 && in.Line() == 1) {
      for(int outputIndex = 0; outputIndex < outputSize; outputIndex++) {
        outMan[outputIndex] = avg;
      }
      outMan.SetLine(1);
      ocube->Write(outMan);
      outMan.SetLine(2);
    }
  }

  for(int outputIndex = 0; outputIndex < outputSize; outputIndex++) {
    int inputIndex = outputIndex - leftPad;
    if(inputIndex < 0) {
      outMan[outputIndex] = in[inputIndex + inputSize];
    }
    else if(inputIndex < inputSize) {
      outMan[outputIndex] = in[inputIndex];
    }
    else {
      outMan[outputIndex] = in[inputIndex - inputSize];
    }
    if (outMan[outputIndex] < minRadius) minRadius = outMan[outputIndex];
    if (outMan[outputIndex] > maxRadius) maxRadius = outMan[outputIndex];
  }

  ocube->Write(outMan);

  if (bottomPad == 1 && in.Line() == inl) {
    for(int outputIndex = 0; outputIndex < outputSize; outputIndex++) {
      outMan[outputIndex] = avg;
    }
    outMan.SetLine(inl + topPad + bottomPad);
    ocube->Write(outMan);
  }
}
