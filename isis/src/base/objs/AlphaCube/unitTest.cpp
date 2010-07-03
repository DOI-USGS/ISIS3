#include <iostream>
#include "AlphaCube.h"
#include "iException.h"
#include "Preference.h"

using namespace std;

int main () {
  Isis::Preference::Preferences(true);
  Isis::AlphaCube c(4,8,2,3,1.5,2.5,3.5,5.5);
  cout << "1st Test Alpha" << endl;
  cout << c.AlphaSamples() << endl;
  cout << c.AlphaLines() << endl;
  cout << c.AlphaSample(1.0) << endl;
  cout << c.AlphaLine(1.0) << endl;
  cout << c.AlphaSample(c.BetaSamples()) << endl;
  cout << c.AlphaLine(c.BetaLines()) << endl;
  cout << c.AlphaSample(0.5) << endl;
  cout << c.AlphaLine(0.5) << endl;
  cout << c.AlphaSample(c.BetaSamples()+0.5) << endl;
  cout << c.AlphaLine(c.BetaLines()+0.5) << endl;
  cout << endl;

  cout << "1st Test Beta" << endl;
  cout << c.BetaSamples() << endl;
  cout << c.BetaLines() << endl;
  cout << c.BetaSample(1.0) << endl;
  cout << c.BetaLine(1.0) << endl;
  cout << c.BetaSample(c.AlphaSamples()) << endl;
  cout << c.BetaLine(c.AlphaLines()) << endl;
  cout << endl;
  
  Isis::AlphaCube d(2,3,2,4,1.5,1.5,2.5,3.5);
  cout << "2nd Alpha Test" << endl;
  cout << d.AlphaSamples() << endl;
  cout << d.AlphaLines() << endl;
  cout << d.AlphaSample(1.0) << endl;
  cout << d.AlphaLine(1.0) << endl;
  cout << d.AlphaSample(d.BetaSamples()) << endl;
  cout << d.AlphaLine(d.BetaLines()) << endl;
  cout << d.AlphaSample(0.5) << endl;
  cout << d.AlphaLine(0.5) << endl;
  cout << d.AlphaSample(d.BetaSamples()+0.5) << endl;
  cout << d.AlphaLine(d.BetaLines()+0.5) << endl;
  cout << endl;

  cout << "2nd Beta Test" << endl;
  cout << d.BetaSamples() << endl;
  cout << d.BetaLines() << endl;
  cout << d.BetaSample(1.0) << endl;
  cout << d.BetaLine(1.0) << endl;
  cout << d.BetaSample(d.AlphaSamples()) << endl;
  cout << d.BetaLine(d.AlphaLines()) << endl;
  cout << endl;

  c.Rehash(d);
  cout << "3rd Test Alpha" << endl;
  cout << c.AlphaSamples() << endl;
  cout << c.AlphaLines() << endl;
  cout << c.AlphaSample(1.0) << endl;
  cout << c.AlphaLine(1.0) << endl;
  cout << c.AlphaSample(c.BetaSamples()) << endl;
  cout << c.AlphaLine(c.BetaLines()) << endl;
  cout << c.AlphaSample(0.5) << endl;
  cout << c.AlphaLine(0.5) << endl;
  cout << c.AlphaSample(c.BetaSamples()+0.5) << endl;
  cout << c.AlphaLine(c.BetaLines()+0.5) << endl;
  cout << endl;

  cout << "3rd Test Beta" << endl;
  cout << c.BetaSamples() << endl;
  cout << c.BetaLines() << endl;
  cout << c.BetaSample(1.0) << endl;
  cout << c.BetaLine(1.0) << endl;
  cout << c.BetaSample(c.AlphaSamples()) << endl;
  cout << c.BetaLine(c.AlphaLines()) << endl;
  cout << endl;

  try {
  Isis::Pvl lab;
  lab.AddObject(Isis::PvlObject("IsisCube"));
  Isis::PvlObject &isiscube = lab.FindObject("IsisCube");
  isiscube.AddGroup(Isis::PvlGroup("Dimensions"));
  Isis::PvlGroup &dims = isiscube.FindGroup("Dimensions");
  dims += Isis::PvlKeyword("Samples",4);
  dims += Isis::PvlKeyword("Lines",8);
  c.UpdateGroup(lab);
  cout << lab << endl;
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  return 0;
}
