#include "Isis.h"

#include "Application.h"
#include "AutoReg.h"
#include "AutoRegFactory.h"
#include "Brick.h"
#include "Chip.h"
#include "Cube.h"
#include "History.h"
#include "IException.h"
#include "Progress.h"
#include "PvlGroup.h"
#include "UserInterface.h"

using namespace std;
using namespace Isis;

void writeHistory(Cube &cube);

void IsisMain() {
  // Import cube data & PVL information
  Cube cube;
  UserInterface &ui = Application::GetUserInterface();
  cube.open(ui.GetFileName("FROM"), "rw");
  Pvl *regdef;
  // If regdef was supplied by the user, use it. else, use the template.
  if (ui.WasEntered("REGDEF")) {
    regdef = new Pvl(ui.GetFileName("REGDEF"));
  }
  else {
    regdef = new Pvl("$base/templates/autoreg/findrx.def");
  }
  PvlGroup &reseaus = cube.label()->findGroup("Reseaus", Pvl::Traverse);

  // If the Keyword sizes don't match up, throw errors.
  int nres = reseaus["Line"].size();
  if (nres != reseaus["Sample"].size()) {
    QString msg = "Sample size incorrect [Sample size " +
                 toString(reseaus["Sample"].size()) + " != " + " Line size " +
                 toString(reseaus["Line"].size()) + "]";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }
  if (nres != reseaus["Type"].size()) {
    QString msg = "Type size incorrect [Type size " +
                 toString(reseaus["Type"].size()) + " != " + " Line size " +
                 toString(reseaus["Line"].size()) + "]";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }
  if (nres != reseaus["Valid"].size()) {
    QString msg = "Valid size incorrect [Valid size " +
                 toString(reseaus["Valid"].size()) + " != " + " Line size " +
                 toString(reseaus["Line"].size()) + "]";
    throw IException(IException::Unknown, msg, _FILEINFO_);
  }

  // Auto Registration
  AutoReg *ar = AutoRegFactory::Create(*regdef);
  Cube pattern;
  pattern.open(reseaus["Template"][0], "r");
  ar->PatternChip()->TackCube(5.0, 5.0);

  // Display the progress...10% 20% etc.
  Progress prog;
  prog.SetMaximumSteps(nres);
  prog.CheckStatus();


  //If the mark reseaus option is set...then create a brick.
  Brick *white = NULL;
  if (ui.GetBoolean("MARK") == true) {
    white = new Brick(1, 1, 1, Isis::UnsignedByte);
    (*white)[0] = Isis::Hrs;
  }

  double patternValidPercent = ar->PatternValidPercent();
  double subsearchValidPercent = ar->SubsearchValidPercent();

  // And the loop...
  for (int res = 0; res < nres; ++res) {
    // Output chips
    ar->SearchChip()->TackCube(toDouble(reseaus["Sample"][res]), toDouble(reseaus["Line"][res]));
    ar->SearchChip()->Load(cube);
    ar->PatternChip()->Load(pattern, 0, 1.0, res + 1);
    int type = toInt(reseaus["Type"][res]);
    // If the reseaus is in the center (type 5) use full percent value
    if (type == 5) {
      ar->SetPatternValidPercent(patternValidPercent);
      ar->SetSubsearchValidPercent(subsearchValidPercent);
    }
    // else if the reseaus is on an edge (type 2,4,6, or 8) use half percent value
    else if (type % 2 == 0) {
      ar->SetPatternValidPercent(patternValidPercent / 2.0);
      ar->SetSubsearchValidPercent(subsearchValidPercent / 2.0);
    }
    // else the reseaus on a corner (type 1,3,7, or 9) use a quarter percent value
    else {
      ar->SetPatternValidPercent(patternValidPercent / 4.0);
      ar->SetSubsearchValidPercent(subsearchValidPercent / 4.0);
    }

    ar->Register();

    if (ar->Success()) {
      reseaus["Sample"][res] = toString(ar->CubeSample());
      reseaus["Line"][res] = toString(ar->CubeLine());
      reseaus["Valid"][res] = "1";
    }
    else {
      reseaus["Valid"][res] = "0";
    }

    // And if the reseaus are to be marked...mark em
    if (white != NULL) {
      double line = toDouble(reseaus["Line"][res]);
      double sample = toDouble(reseaus["Sample"][res]);
      white->SetBasePosition(int(sample), int(line), 1);
      cube.write(*white);
    }
    prog.CheckStatus();

  }

  // Change status to "Refined", corrected!
  reseaus["Status"] = "Refined";

  pattern.close();
  writeHistory(cube);
  cube.close();
}


/**
 * Writes out the History blob to a cube
 *
 * @param cube Cube to add History blob to
 */
void writeHistory(Cube &cube) {
  bool addedHist = false;
  Isis::Pvl *inlabel = cube.label();
  for (int i = 0; i < inlabel->objects(); i++) {
    if (inlabel->object(i).isNamed("History") && Isis::iApp != NULL) {
      Isis::History h( (QString)inlabel->object(i)["Name"] );
      cube.read(h);
      h.AddEntry();
      cube.write(h);
      addedHist = true;
    }
  }

  if (!addedHist && Isis::iApp != NULL) {
    Isis::History h("IsisCube");
    h.AddEntry();
    cube.write(h);
  }
}
