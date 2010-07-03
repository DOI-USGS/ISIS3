#include <iostream>
#include <iomanip>
#include "iException.h"
#include "iTime.h"

#include "Preference.h"

using namespace std;
int main (int argc, char *argv[]) {

  Isis::Preference::Preferences(true);

  cout << "Unit test for Isis::iTime" << endl;

  try {
    cout << setprecision(9);
    string test = "2003/01/02 12:15:01.1234";
    Isis::iTime *time = new Isis::iTime (test);
    cout << "  Test of date = " << test << endl;
    cout << "   Year        = " << time->YearString() << endl;
    cout << "   Year        = " << time->Year() << endl;
    cout << "   Month       = " << time->MonthString() << endl;
    cout << "   Month       = " << time->Month() << endl;
    cout << "   Day         = " << time->DayString() << endl;
    cout << "   Day         = " << time->Day() << endl;
    cout << "   Hour        = " << time->HourString() << endl;
    cout << "   Hour        = " << time->Hour() << endl;
    cout << "   Minute      = " << time->MinuteString() << endl;
    cout << "   Minute      = " << time->Minute() << endl;
    cout << "   Second      = " << time->SecondString() << endl;
    cout << "   Second      = " << time->Second() << endl;
    cout << "   Day of Year = " << time->DayOfYearString() << endl;
    cout << "   Day of Year = " << time->DayOfYear() << endl;
    cout << "   Et          = " << time->EtString() << endl;
    cout << "   Et          = " << time->Et() << endl;
    cout << "   UTC         = " << time->UTC() << endl;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }


  double saveEt = 0.0;
  try {
    cout << endl;
    cout << setprecision(9);
    string test = "2000-12-31T23:59:01.6789";
    Isis::iTime time;
    time = test;
    cout << "  Test of date = " << test << endl;
    cout << "   Year        = " << time.YearString() << endl;
    cout << "   Year        = " << time.Year() << endl;
    cout << "   Month       = " << time.MonthString() << endl;
    cout << "   Month       = " << time.Month() << endl;
    cout << "   Day         = " << time.DayString() << endl;
    cout << "   Day         = " << time.Day() << endl;
    cout << "   Hour        = " << time.HourString() << endl;
    cout << "   Hour        = " << time.Hour() << endl;
    cout << "   Minute      = " << time.MinuteString() << endl;
    cout << "   Minute      = " << time.Minute() << endl;
    cout << "   Second      = " << time.SecondString() << endl;
    cout << "   Second      = " << time.Second() << endl;
    cout << "   Day of Year = " << time.DayOfYearString() << endl;
    cout << "   Day of Year = " << time.DayOfYear() << endl;
    cout << "   Et          = " << time.EtString() << endl;
    cout << "   Et          = " << time.Et() << endl;
    cout << "   UTC         = " << time.UTC() << endl;
    saveEt = time.Et();
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }


  try {
    cout << endl;
    cout << setprecision(9);
    Isis::iTime time(saveEt);
    cout << "  Test of date = " << time.EtString() << endl;
    cout << "   Year        = " << time.YearString() << endl;
    cout << "   Year        = " << time.Year() << endl;
    cout << "   Month       = " << time.MonthString() << endl;
    cout << "   Month       = " << time.Month() << endl;
    cout << "   Day         = " << time.DayString() << endl;
    cout << "   Day         = " << time.Day() << endl;
    cout << "   Hour        = " << time.HourString() << endl;
    cout << "   Hour        = " << time.Hour() << endl;
    cout << "   Minute      = " << time.MinuteString() << endl;
    cout << "   Minute      = " << time.Minute() << endl;
    cout << "   Second      = " << time.SecondString() << endl;
    cout << "   Second      = " << time.Second() << endl;
    cout << "   Day of Year = " << time.DayOfYearString() << endl;
    cout << "   Day of Year = " << time.DayOfYear() << endl;
    cout << "   Et          = " << time.EtString() << endl;
    cout << "   Et          = " << time.Et() << endl;
    cout << "   UTC         = " << time.UTC() << endl;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }


  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of Isis::iTime operator>= member" << endl;
    string test = "2003/01/02 12:15:01.1234";
    Isis::iTime *t1 = new Isis::iTime (test);
    test = "2003/01/02 12:15:01.1234";
    Isis::iTime *t2 = new Isis::iTime (test);
    cout << "    " << t1->EtString() << " >= " << t2->EtString() << " = ";
    cout << (*t1 >= *t2) << endl;
    Isis::iTime *t3 = new Isis::iTime ("2003/01/02 12:15:01.12345");
    cout << "    " << t1->EtString() << " >= " << t3->EtString() << " = ";
    cout << (*t1 >= *t3) << endl;
    Isis::iTime *t4 = new Isis::iTime ("2003/01/02 12:15:01.1230");
    cout << "    " << t1->EtString() << " >= " << t4->EtString() << " = ";
    cout << (*t1 >= *t4) << endl;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }

  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of Isis::iTime operator<= member" << endl;
    string test = "2003/01/02 12:15:01.1234";
    Isis::iTime *t1 = new Isis::iTime (test);
    test = "2003/01/02 12:15:01.1234";
    Isis::iTime *t2 = new Isis::iTime (test);
    cout << "    " << t1->EtString() << " <= " << t2->EtString() << " = ";
    cout << (*t1 <= *t2) << endl;
    Isis::iTime *t3 = new Isis::iTime ("2003/01/02 12:15:01.12345");
    cout << "    " << t1->EtString() << " <= " << t3->EtString() << " = ";
    cout << (*t1 <= *t3) << endl;
    Isis::iTime *t4 = new Isis::iTime ("2003/01/02 12:15:01.1230");
    cout << "    " << t1->EtString() << " <= " << t4->EtString() << " = ";
    cout << (*t1 <= *t4) << endl;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }


  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of Isis::iTime operator> member" << endl;
    string test = "2003/01/02 12:15:01.1234";
    Isis::iTime *t1 = new Isis::iTime (test);
    test = "2003/01/02 12:15:01.1234";
    Isis::iTime *t2 = new Isis::iTime (test);
    cout << "    " << t1->EtString() << " > " << t2->EtString() << " = ";
    cout << (*t1 > *t2) << endl;
    Isis::iTime *t3 = new Isis::iTime ("2003/01/02 12:15:01.12345");
    cout << "    " << t1->EtString() << " > " << t3->EtString() << " = ";
    cout << (*t1 > *t3) << endl;
    Isis::iTime *t4 = new Isis::iTime ("2003/01/02 12:15:01.1230");
    cout << "    " << t1->EtString() << " > " << t4->EtString() << " = ";
    cout << (*t1 > *t4) << endl;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }

  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of Isis::iTime operator< member" << endl;
    string test = "2003/01/02 12:15:01.1234";
    Isis::iTime *t1 = new Isis::iTime (test);
    test = "2003/01/02 12:15:01.1234";
    Isis::iTime *t2 = new Isis::iTime (test);
    cout << "    " << t1->EtString() << " < " << t2->EtString() << " = ";
    cout << (*t1 < *t2) << endl;
    Isis::iTime *t3 = new Isis::iTime ("2003/01/02 12:15:01.12345");
    cout << "    " << t1->EtString() << " < " << t3->EtString() << " = ";
    cout << (*t1 < *t3) << endl;
    Isis::iTime *t4 = new Isis::iTime ("2003/01/02 12:15:01.1230");
    cout << "    " << t1->EtString() << " < " << t4->EtString() << " = ";
    cout << (*t1 < *t4) << endl;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }

  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of Isis::iTime operator!= member" << endl;
    string test = "2003/01/02 12:15:01.1234";
    Isis::iTime *t1 = new Isis::iTime (test);
    test = "2003/01/02 12:15:01.1234";
    Isis::iTime *t2 = new Isis::iTime (test);
    cout << "    " << t1->EtString() << " != " << t2->EtString() << " = ";
    cout << (*t1 != *t2) << endl;
    Isis::iTime *t3 = new Isis::iTime ("2003/01/02 12:15:01.12345");
    cout << "    " << t1->EtString() << " != " << t3->EtString() << " = ";
    cout << (*t1 != *t3) << endl;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }

  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of Isis::iTime operator== member" << endl;
    string test = "2003/01/02 12:15:01.1234";
    Isis::iTime *t1 = new Isis::iTime (test);
    test = "2003/01/02 12:15:01.1234";
    Isis::iTime *t2 = new Isis::iTime (test);
    cout << "    " << t1->EtString() << " == " << t2->EtString() << " = ";
    cout << (*t1 == *t2) << endl;
    Isis::iTime *t3 = new Isis::iTime ("2003/01/02 12:15:01.12345");
    cout << "    " << t1->EtString() << " == " << t3->EtString() << " = ";
    cout << (*t1 == *t3) << endl;
  }
  catch (Isis::iException &error) {
    error.Report (false);
  }





}



