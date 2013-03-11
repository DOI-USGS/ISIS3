#include "Isis.h"
#include "TextFile.h"
#include "Statistics.h"
#include "ProcessByLine.h"
#include "SpecialPixel.h"
#include "Stretch.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"

using namespace std;
using namespace Isis;

void stretch(Buffer &in, Buffer &out);
Stretch str;
Statistics stats;

void IsisMain() {
  ProcessByLine p;
  Cube *inCube = p.SetInputCube("FROM");

  UserInterface &ui = Application::GetUserInterface();

  QString pairs;

  // first just get the pairs from where ever and worry about
  // whether they are dn values or %'s later
  if(ui.GetBoolean("READFILE")) {
    FileName pairsFileName = ui.GetFileName("INPUTFILE");
    TextFile pairsFile;
    pairsFile.SetComment("#");
    pairsFile.Open(pairsFileName.expanded());

    // concat all non-comment lines into one string (pairs)
    QString line = "";
    while(pairsFile.GetLine(line, true)) {
      pairs += " " + line;
    }
    pairs += line;
  }
  else {
    if(ui.WasEntered("PAIRS"))
      pairs = ui.GetString("PAIRS");
  }

  if(ui.GetBoolean("USEPERCENTAGES"))
    str.Parse(pairs, inCube->histogram());
  else
    str.Parse(pairs);

  // Setup new mappings for special pixels if necessary
  if(ui.WasEntered("NULL"))
    str.SetNull(StringToPixel(ui.GetString("NULL")));
  if(ui.WasEntered("LIS"))
    str.SetLis(StringToPixel(ui.GetString("LIS")));
  if(ui.WasEntered("LRS"))
    str.SetLrs(StringToPixel(ui.GetString("LRS")));
  if(ui.WasEntered("HIS"))
    str.SetHis(StringToPixel(ui.GetString("HIS")));
  if(ui.WasEntered("HRS"))
    str.SetHrs(StringToPixel(ui.GetString("HRS")));

  p.SetOutputCube("TO");

  // Start the processing
  p.StartProcess(stretch);
  p.EndProcess();

  PvlKeyword dnPairs = PvlKeyword("StretchPairs");
  dnPairs.addValue(str.Text());

  PvlGroup results = PvlGroup("Results");
  results.addKeyword(dnPairs);

  Application::Log(results);

}

// Line processing routine
void stretch(Buffer &in, Buffer &out) {
  for(int i = 0; i < in.size(); i++) {
    out[i] = str.Map(in[i]);
  }
}
