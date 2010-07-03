#include <iostream>

#include "Preference.h"
#include "Progress.h"
#include "iException.h"

using namespace std;
int main () {
  Isis::Preference &pref = Isis::Preference::Preferences(true);
  Isis::PvlGroup &uip = pref.FindGroup("UserInterface");
  uip["ProgressBarPercent"] = 5;
  Isis::Progress p;
  
  // Check normal operation
  p.SetMaximumSteps(1000);
  p.CheckStatus();
  for (int i=1; i<=1000; i++) p.CheckStatus();
  cout << endl;

  // Check again but test the text report
  p.SetText("Drinking Coffee");
  p.SetMaximumSteps(5);
  p.CheckStatus();
  for (int i=1; i<=5; i++) p.CheckStatus();
  cout << endl;

  // Check for error by going too many steps
  try {
    p.CheckStatus();
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }

  // Check for error on setting the Maximum Steps
  try {
    p.SetMaximumSteps(0);
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }

  // Check for error on bad creation
  try {
    uip["ProgressBarPercent"] = 3;
    Isis::Progress p2;
  }
  catch (Isis::iException &e) {
    e.Report(false);
  }
}
