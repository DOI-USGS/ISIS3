/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "Isis.h"
#include "ProcessByTile.h"
#include "SpecialPixel.h"
#include "Pvl.h"
#include "UserInterface.h"
#include "Brick.h"
#include "Apollo.h"
#include <QString>
#include <cstdlib>

using namespace std;
using namespace Isis;

void cpy(Buffer &in, Buffer &out);
static int dim;
static bool resvalid;
static QString action;

void IsisMain() {
  // We will be processing by line
  ProcessByTile p;
  p.SetTileSize(128, 128);

  // Setup the input and output cubes
  Cube* info = p.SetInputCube("FROM");
  PvlKeyword &status = info ->group("RESEAUS")["STATUS"];
  UserInterface &ui = Application::GetUserInterface();
  QString in = ui.GetFileName("FROM");

  QString spacecraft = (info->group("Instrument")["SpacecraftName"]);
  QString instrument = (info->group("Instrument")["InstrumentId"]);
  Apollo apollo(spacecraft, instrument);
  if (spacecraft.mid(0,6) != "APOLLO") {
    QString msg = "This application is for use with Apollo spacecrafts only. ";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  // Check reseau status and make sure it is not nominal or removed
  if ((QString)status == "Nominal") {
    QString msg = "Input file [" + in +
          "] appears to have nominal reseau status. You must run findrx first.";
    throw IException(IException::User,msg, _FILEINFO_);
  }
  if ((QString)status == "Removed") {
    QString msg = "Input file [" + in +
          "] appears to already have reseaus removed.";
    throw IException(IException::User,msg, _FILEINFO_);
  }

  status = "Removed";

  p.SetOutputCube ("TO");

  // Start the processing
  p.StartProcess(cpy);
  p.EndProcess();

  dim = apollo.ReseauDimension();

  // Get other user entered options
  QString out= ui.GetFileName("TO");
  resvalid = ui.GetBoolean("RESVALID");
  action = ui.GetString("ACTION");

  // Open the output cube
  Cube cube;
  cube.open(out, "rw");

  PvlGroup &res = cube.label()->findGroup("RESEAUS",Pvl::Traverse);

  // Get reseau line, sample, type, and valid Keywords
  PvlKeyword lines = res.findKeyword("LINE");
  PvlKeyword samps = res.findKeyword("SAMPLE");
  PvlKeyword type = res.findKeyword("TYPE");
  PvlKeyword valid = res.findKeyword("VALID");
  int numres = lines.size();

  Brick brick(dim,dim,1,cube.pixelType());
  int width = ui.GetInteger("WIDTH");
  for (int res=0; res<numres; res++) {
    if ((resvalid == 0 || toInt(valid[res]) == 1)) {
      int baseSamp = (int)(toDouble(samps[res])+0.5) - (dim/2);
      int baseLine = (int)(toDouble(lines[res])+0.5) - (dim/2);
      brick.SetBasePosition(baseSamp,baseLine,1);
      cube.read(brick);
      if (action == "NULL") {
        // set the three pixels surrounding the reseau to null
        for (int i=0; i<dim; i++) {
          for (int j=(width-1)/-2; j<=width/2; j++) {
            // vertical lines
            brick[dim*i+dim/2+j] = Isis::Null;
            // horizontal lines
            brick[dim*(dim/2+j)+i] = Isis::Null;
          }
        }
      }
      else if (action == "PATCH") {
        for (int i = 0; i < dim; i++) {
          for (int j=(width-1)/-2; j<=width/2; j++) {
            // vertical lines
            brick[dim*i+dim/2+j] = (brick[dim*i+dim/2-width+j] + brick[dim*i+dim/2+width+j])/2.0;
            // horizontal lines
            brick[dim*(dim/2+j)+i] = (brick[dim*(dim/2-width+j)+i]+brick[dim*(dim/2+width+j)+i])/2.0;
          }
        }
      }
    }
    cube.write(brick);
  }
  cube.close();
}

// Copy the input cube to the output cube
void cpy(Buffer &in, Buffer &out) {
  for (int i=0; i<in.size(); i++) {
    out[i] = in[i];
  }
}
