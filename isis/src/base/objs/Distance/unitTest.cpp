#include "Distance.h"

#include <iostream>

#include "iException.h"
#include "Preference.h"

using std::cout;
using std::endl;

using Isis::Distance;
using Isis::iException;
using Isis::Preference;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout.precision(14);

  cout << "----- Testing Constructors -----" << endl << endl;

  /*
  // See Distance.cpp for why this is disabled.
  try {
    cout << "Empty constructor" << endl;
    Distance dist;
    cout << dist.GetMeters() << " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }
  */

  try {
    cout << "Constructor given a value in meters" << endl;
    Distance dist(1500.5);
    cout << dist.GetMeters() << " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Constructor given a value in kilometers" << endl;
    Distance dist(1500.5, Distance::Kilometers);
    cout << dist.GetMeters() << " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Copy constructor" << endl;
    Distance dist(1500.5);
    Distance copiedDist(dist);
    cout << copiedDist.GetMeters() << " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl << "----- Testing Accessors -----" << endl << endl;

  try {
    cout << "Meters (redundant)" << endl;
    Distance dist(1, Distance::Meters);
    cout << dist.GetMeters() << " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cout << "Kilometers" << endl;
    Distance dist(1, Distance::Kilometers);
    cout << dist.GetKilometers() << " kilometers" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl << "----- Testing Operators -----" << endl << endl;

  try {
    Distance dist1(1, Distance::Meters);
    Distance dist2(1, Distance::Meters);
    cout << endl;
    cout << "Distance 1: " << (double)dist1 << " meters" << endl;
    cout << "Distance 2: " << (double)dist2 << " meters" << endl;

    cout << "Distance 1 > Distance 2 ? " << (dist1 > dist2) << endl;
    cout << "Distance 1 >= Distance 2 ? " << (dist1 >= dist2) << endl;
    cout << "Distance 1 == Distance 2 ? " << (dist1 == dist2) << endl;
    cout << "Distance 1 <= Distance 2 ? " << (dist1 <= dist2) << endl;
    cout << "Distance 1 < Distance 2 ? " << (dist1 < dist2) << endl;
    cout << "Distance 1 + Distance 2 ? " << (dist1 + dist2).GetMeters() << 
        " meters" << endl;
    cout << "Distance 1 - Distance 2 ? " << (dist1 - dist2).GetMeters() << 
        " meters" << endl;

    dist1 = dist2;
    cout << "Distance 1 = Distance 2... Distance 1 = " << dist1.GetMeters() <<
        " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Distance dist1(1, Distance::Meters);
    Distance dist2(10, Distance::Meters);
    cout << endl;
    cout << "Distance 1: " << (double)dist1 << " meters" << endl;
    cout << "Distance 2: " << (double)dist2 << " meters" << endl;

    cout << "Distance 1 > Distance 2 ? " << (dist1 > dist2) << endl;
    cout << "Distance 1 >= Distance 2 ? " << (dist1 >= dist2) << endl;
    cout << "Distance 1 == Distance 2 ? " << (dist1 == dist2) << endl;
    cout << "Distance 1 <= Distance 2 ? " << (dist1 <= dist2) << endl;
    cout << "Distance 1 < Distance 2 ? " << (dist1 < dist2) << endl;
    cout << "Distance 1 + Distance 2 ? " << (dist1 + dist2).GetMeters() << 
        " meters" << endl;

    // This should throw an exception and never cout
    try {
      (dist1 - dist2).GetMeters();
    }
    catch(iException &e) {
      e.Report(false);
    }

    dist1 = dist2;
    cout << "Distance 1 = Distance 2... Distance 1 = " << dist1.GetMeters() <<
        " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Distance dist1(10, Distance::Meters);
    Distance dist2(1, Distance::Meters);
    cout << endl;
    cout << "Distance 1: " << (double)dist1 << " meters" << endl;
    cout << "Distance 2: " << (double)dist2 << " meters" << endl;

    cout << "Distance 1 > Distance 2 ? " << (dist1 > dist2) << endl;
    cout << "Distance 1 >= Distance 2 ? " << (dist1 >= dist2) << endl;
    cout << "Distance 1 == Distance 2 ? " << (dist1 == dist2) << endl;
    cout << "Distance 1 <= Distance 2 ? " << (dist1 <= dist2) << endl;
    cout << "Distance 1 < Distance 2 ? " << (dist1 < dist2) << endl;
    cout << "Distance 1 + Distance 2 ? " << (dist1 + dist2).GetMeters() << 
        " meters" << endl;
    cout << "Distance 1 - Distance 2 ? " << (dist1 - dist2).GetMeters() << 
        " meters" << endl;

    dist1 = dist2;
    cout << "Distance 1 = Distance 2... Distance 1 = " << dist1.GetMeters() <<
        " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Distance dist1(1000, Distance::Meters);
    Distance dist2(1, Distance::Kilometers);
    cout << endl;
    cout << "Distance 1: " << (double)dist1 << " meters" << endl;
    cout << "Distance 2: " << (double)dist2 << " meters" << endl;

    cout << "Distance 1 > Distance 2 ? " << (dist1 > dist2) << endl;
    cout << "Distance 1 >= Distance 2 ? " << (dist1 >= dist2) << endl;
    cout << "Distance 1 == Distance 2 ? " << (dist1 == dist2) << endl;
    cout << "Distance 1 <= Distance 2 ? " << (dist1 <= dist2) << endl;
    cout << "Distance 1 < Distance 2 ? " << (dist1 < dist2) << endl;
    cout << "Distance 1 + Distance 2 ? " << (dist1 + dist2).GetMeters() << 
        " meters" << endl;
    cout << "Distance 1 - Distance 2 ? " << (dist1 - dist2).GetMeters() << 
        " meters" << endl;

    dist1 = dist2;
    cout << "Distance 1 = Distance 2... Distance 1 = " << dist1.GetMeters() <<
        " meters" << endl;
    dist1 += dist2;
    cout << "Distance 1 += Distance 2... Distance 1 = " << dist1.GetMeters() <<
        " meters" << endl;
    dist1 -= dist2;
    cout << "Distance 1 -= Distance 2... Distance 1 = " << dist1.GetMeters() <<
        " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cout << endl << "----- Testing Error Checking -----" << endl << endl;

  try {
    Distance dist(-1);
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Distance dist(-1, Distance::Kilometers);
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Distance dist(1, Distance::Kilometers);
    dist.SetMeters(-1);
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Distance dist(1, Distance::Kilometers);
    dist.SetMeters(-1);
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Distance dist(1, Distance::Kilometers);
    dist.SetKilometers(-1);
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Distance dist(1, (Distance::Units)-1);
    dist.SetKilometers(-1);
  }
  catch(iException &e) {
    e.Report(false);
  }
}
