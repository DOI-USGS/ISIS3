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

void DoWrap (Buffer &in);

Cube *ocube;

int leftPad;
int rightPad;
int topPad;
int bottomPad;

void IsisMain() {
  // We will be use a mosaic technique so get the size of the input file
  ProcessByLine p;
  Cube *icube = p.SetInputCube ("FROM");
  int ins = icube->Samples();
  int inl = icube->Lines();
  int inb = icube->Bands();

  // Retrieve the padding parameters
  UserInterface &ui = Application::GetUserInterface();
  leftPad = ui.GetInteger("LEFT");
  rightPad = ui.GetInteger("RIGHT");
  topPad = ui.GetInteger("TOP");
  bottomPad = ui.GetInteger("BOTTOM");

  // Compute the output size
  int ns = ins + leftPad + rightPad;
  int nl = inl + topPad + bottomPad;
  int nb = inb;

  if(leftPad > ins || rightPad > ins || topPad > inl || bottomPad > inl) {
    iString message = "The padding must be less than or equal to the image dimensions";
    throw iException::Message(iException::User, message, _FILEINFO_);
  }

  Projection *proj = icube->Projection();
  if(proj == NULL) {
    iString message = "The input cube must be a DEM file, which means it has a projection";
    throw iException::Message(iException::User, message, _FILEINFO_);
  }

  if(!proj->IsEquatorialCylindrical()) {
    iString message = "The input cube must have an equatorial cylindrical projection";
    throw iException::Message(iException::User, message, _FILEINFO_);
  }

  PvlGroup mapgrp = icube->Label()->FindGroup("Mapping", Pvl::Traverse);

  double upperLeftCorner = mapgrp["UpperLeftCornerX"];
  upperLeftCorner -= leftPad * proj->Resolution();
  mapgrp.AddKeyword(PvlKeyword("UpperLeftCornerX", upperLeftCorner, "meters"),
                       Pvl::Replace);

  upperLeftCorner = mapgrp["UpperLeftCornerY"];
  upperLeftCorner += topPad * proj->Resolution();
  mapgrp.AddKeyword(PvlKeyword("UpperLeftCornerY", upperLeftCorner, "meters"),
                                      Pvl::Replace);


  p.SetOutputCube ("TO", ns, nl, nb);
  // Make sure everything is propagated and closed
  p.EndProcess();

  // Now we'll really be processing our input cube
  p.SetInputCube("FROM");
  
  // We need to create the output file
  ocube = new Cube();
  ocube->Open(Filename(ui.GetFilename("TO")).Expanded(), "rw");

  p.StartProcess(DoWrap);

  // update mapping grp
  ocube->PutGroup(mapgrp);

  p.EndProcess();
  ocube->Close();
  delete ocube;
}

void DoWrap (Buffer &in) {
  LineManager outMan(*ocube);
  
  outMan.SetLine(in.Line() + topPad);
  int inputSize = in.size();
  int outputSize = outMan.size();

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
  }

  ocube->Write(outMan);

  // first line goes the top n lines
  if(in.Line() == 1) {
    for(int outLine = 1; outLine <= topPad; outLine++) {
      outMan.SetLine(outLine);
      ocube->Write(outMan);
    } 
  }

  // last line goes the bottom n lines
  int inl = ocube->Lines() - topPad - bottomPad;
  if(in.Line() == inl) {
    for(int outLine = 1; outLine <= bottomPad; outLine++) {
      outMan.SetLine(outLine + topPad + inl);
      ocube->Write(outMan);
    } 
  }
}
