#include <iostream>
#include <iomanip>
#include "IException.h"
#include "iTime.h"

#include "Preference.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {

  Preference::Preferences(true);

  cout << "Unit test for iTime" << endl;

  try {
    cout << setprecision(9);
    QString test = "2003/01/02 12:15:01.1234";
    iTime *time = new iTime(test);
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
  catch(IException &error) {
    error.print();
  }


  double saveEt = 0.0;
  try {
    cout << endl;
    cout << setprecision(9);
    QString test = "2000-12-31T23:59:01.6789";
    iTime time;
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
  catch(IException &error) {
    error.print();
  }


  try {
    cout << endl;
    cout << setprecision(9);
    iTime time(saveEt);
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
  catch(IException &error) {
    error.print();
  }


  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of iTime operator>= member" << endl;
    QString test = "2003/01/02 12:15:01.1234";
    iTime *t1 = new iTime(test);
    test = "2003/01/02 12:15:01.1234";
    iTime *t2 = new iTime(test);
    cout << "    " << t1->EtString() << " >= " << t2->EtString() << " = ";
    cout << (*t1 >= *t2) << endl;
    iTime *t3 = new iTime("2003/01/02 12:15:01.12345");
    cout << "    " << t1->EtString() << " >= " << t3->EtString() << " = ";
    cout << (*t1 >= *t3) << endl;
    iTime *t4 = new iTime("2003/01/02 12:15:01.1230");
    cout << "    " << t1->EtString() << " >= " << t4->EtString() << " = ";
    cout << (*t1 >= *t4) << endl;
  }
  catch(IException &error) {
    error.print();
  }

  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of iTime operator<= member" << endl;
    QString test = "2003/01/02 12:15:01.1234";
    iTime *t1 = new iTime(test);
    test = "2003/01/02 12:15:01.1234";
    iTime *t2 = new iTime(test);
    cout << "    " << t1->EtString() << " <= " << t2->EtString() << " = ";
    cout << (*t1 <= *t2) << endl;
    iTime *t3 = new iTime("2003/01/02 12:15:01.12345");
    cout << "    " << t1->EtString() << " <= " << t3->EtString() << " = ";
    cout << (*t1 <= *t3) << endl;
    iTime *t4 = new iTime("2003/01/02 12:15:01.1230");
    cout << "    " << t1->EtString() << " <= " << t4->EtString() << " = ";
    cout << (*t1 <= *t4) << endl;
  }
  catch(IException &error) {
    error.print();
  }


  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of iTime operator> member" << endl;
    QString test = "2003/01/02 12:15:01.1234";
    iTime *t1 = new iTime(test);
    test = "2003/01/02 12:15:01.1234";
    iTime *t2 = new iTime(test);
    cout << "    " << t1->EtString() << " > " << t2->EtString() << " = ";
    cout << (*t1 > *t2) << endl;
    iTime *t3 = new iTime("2003/01/02 12:15:01.12345");
    cout << "    " << t1->EtString() << " > " << t3->EtString() << " = ";
    cout << (*t1 > *t3) << endl;
    iTime *t4 = new iTime("2003/01/02 12:15:01.1230");
    cout << "    " << t1->EtString() << " > " << t4->EtString() << " = ";
    cout << (*t1 > *t4) << endl;
  }
  catch(IException &error) {
    error.print();
  }

  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of iTime operator< member" << endl;
    QString test = "2003/01/02 12:15:01.1234";
    iTime *t1 = new iTime(test);
    test = "2003/01/02 12:15:01.1234";
    iTime *t2 = new iTime(test);
    cout << "    " << t1->EtString() << " < " << t2->EtString() << " = ";
    cout << (*t1 < *t2) << endl;
    iTime *t3 = new iTime("2003/01/02 12:15:01.12345");
    cout << "    " << t1->EtString() << " < " << t3->EtString() << " = ";
    cout << (*t1 < *t3) << endl;
    iTime *t4 = new iTime("2003/01/02 12:15:01.1230");
    cout << "    " << t1->EtString() << " < " << t4->EtString() << " = ";
    cout << (*t1 < *t4) << endl;
  }
  catch(IException &error) {
    error.print();
  }

  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of iTime operator!= member" << endl;
    QString test = "2003/01/02 12:15:01.1234";
    iTime *t1 = new iTime(test);
    test = "2003/01/02 12:15:01.1234";
    iTime *t2 = new iTime(test);
    cout << "    " << t1->EtString() << " != " << t2->EtString() << " = ";
    cout << (*t1 != *t2) << endl;
    iTime *t3 = new iTime("2003/01/02 12:15:01.12345");
    cout << "    " << t1->EtString() << " != " << t3->EtString() << " = ";
    cout << (*t1 != *t3) << endl;
  }
  catch(IException &error) {
    error.print();
  }

  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of iTime operator== member" << endl;
    QString test = "2003/01/02 12:15:01.1234";
    iTime *t1 = new iTime(test);
    test = "2003/01/02 12:15:01.1234";
    iTime *t2 = new iTime(test);
    cout << "    " << t1->EtString() << " == " << t2->EtString() << " = ";
    cout << (*t1 == *t2) << endl;
    iTime *t3 = new iTime("2003/01/02 12:15:01.12345");
    cout << "    " << t1->EtString() << " == " << t3->EtString() << " = ";
    cout << (*t1 == *t3) << endl;
  }
  catch(IException &error) {
    error.print();
  }

  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of iTime operator+(double) member" << endl;
    iTime t1(0.0);
    t1 = t1 + 1.01;
    cout << "    " << t1.EtString() << endl;
  }
  catch(IException &error) {
    error.print();
  }

  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of iTime operator+=(double) member" << endl;
    iTime t1(0.0);
    t1 += 1.01;
    cout << "    " << t1.EtString() << endl;
  }
  catch(IException &error) {
    error.print();
  }

  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of iTime operator-(double) member" << endl;
    iTime t1(0.0);
    t1 = t1 - 1.01;
    cout << "    " << t1.EtString() << endl;
  }
  catch(IException &error) {
    error.print();
  }

  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of iTime operator-=(double) member" << endl;
    iTime t1(0.0);
    t1 -= 1.01;
    cout << "    " << t1.EtString() << endl;
  }
  catch(IException &error) {
    error.print();
  }

  try {
    cout << endl;
    cout << setprecision(9);
    cout << "  Test of double operator-(iTIme) member" << endl;
    iTime t1(0.0);
    iTime t2 = 100.001;
    double interval = t1 - t2;
    cout << "    " << interval << endl;
  }
  catch(IException &error) {
    error.print();
  }

}



