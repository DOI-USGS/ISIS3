#include <iostream>
#include "Histogram.h"
#include "iException.h"
#include "Preference.h"

using namespace std;

int main (int argc, char *argv[])
{
  Isis::Preference::Preferences(true);
  Isis::Histogram h(-10.0,10.0,21);
  double low,high; 
  try {

  double a[9];
  a[0] = 1.0;
  a[1] = 2.0;
  a[2] = 3.0;
  a[3] = Isis::NULL8;
  a[4] = Isis::HIGH_REPR_SAT8;
  a[5] = Isis::LOW_REPR_SAT8;
  a[6] = Isis::HIGH_INSTR_SAT8;
  a[7] = Isis::LOW_INSTR_SAT8;
  a[8] = 2.0;

  h.AddData(a,9);

  cout << "Median:              " << h.Median() << endl;
  cout << "Mode:                " << h.Mode() << endl;
  cout << "Skew:                " << h.Skew() << endl;
  cout << "Percent(0.5):        " << h.Percent(0.5) << endl;
  cout << "Percent(99.5):       " << h.Percent(99.5) << endl;
  cout << "Bins:                " << h.Bins() << endl;
  cout << "BinSize:             " << h.BinSize() << endl;
  cout << "BinMiddle:           " << h.BinMiddle(0) << endl;

  h.BinRange(0,low,high);
  cout << "BinRange(0,low):     " << low << endl;
  cout << "BinRange(0,high):    " << high << endl;
  cout << "BinCount(0):         " << h.BinCount(0) << endl;
  h.BinRange(20,low,high);
  cout << "BinRange(20,low):  " << low << endl;
  cout << "BinRange(20,high): " << high << endl;
  cout << "BinCount(20):      " << h.BinCount(20) << endl;
  cout << endl;

  h.RemoveData (a,3);
  double b[4];
  b[0] = -11.0;
  b[1] = 11.0;
  b[2] = 5.0;
  b[3] = 5.0;
  h.AddData (b,4);

  cout << "Average:             " << h.Average() << endl;
  cout << "Median:              " << h.Median() << endl;
  cout << "Mode:                " << h.Mode() << endl;
  cout << "Skew:                " << h.Skew() << endl;
  cout << "Percent(0.5):        " << h.Percent(0.5) << endl;
  cout << "Percent(99.5):       " << h.Percent(99.5) << endl;
  cout << "BinCount(0):         " << h.BinCount(0) << endl;
  cout << "BinCount(20):      " << h.BinCount(20) << endl;
  cout << endl;
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }

  try {
    Isis::Histogram g(1.0,0.0);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }

  try {
    h.Percent (-1.0);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }

  try {
    h.Percent (101.0);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }
  
  try {
    h.BinCount (-1);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }
  
  try {
    h.BinCount (1024);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }
  
  try {
    h.BinMiddle (-1);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }
  
  try {
    h.BinMiddle (1024);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }

  try {
    h.BinRange (-1,low,high);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }
  
  try {
    h.BinRange (1024,low,high);
  }
  catch (Isis::iException &e) {
    e.Report (false);
  }
}
