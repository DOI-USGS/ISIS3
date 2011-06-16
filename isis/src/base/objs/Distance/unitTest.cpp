#include "Distance.h"

#include <iostream>

#include "Displacement.h"
#include "iException.h"
#include "Preference.h"

using std::cerr;
using std::endl;

using Isis::Distance;
using Isis::iException;
using Isis::Preference;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cerr.precision(14);

  cerr << "----- Testing Constructors -----" << endl << endl;

  try {
    cerr << "Empty constructor" << endl;
    Distance dist;
    cerr << dist.GetMeters() << " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cerr << "Constructor given a value in meters" << endl;
    Distance dist(1500.5, Distance::Meters);
    cerr << dist.GetMeters() << " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cerr << "Constructor given a value in kilometers" << endl;
    Distance dist(1500.5, Distance::Kilometers);
    cerr << dist.GetMeters() << " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cerr << "Copy constructor" << endl;
    Distance dist(1500.5, Distance::Meters);
    Distance copiedDist(dist);
    cerr << copiedDist.GetMeters() << " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cerr << endl << "----- Testing Accessors -----" << endl << endl;

  try {
    cerr << "Meters (redundant)" << endl;
    Distance dist(1, Distance::Meters);
    cerr << dist.GetMeters() << " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cerr << "Kilometers" << endl;
    Distance dist(1, Distance::Kilometers);
    cerr << dist.GetKilometers() << " kilometers" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cerr << endl << "----- Testing Operators -----" << endl << endl;

  try {
    Distance dist1(1, Distance::Meters);
    Distance dist2(1, Distance::Meters);
    cerr << endl;
    cerr << "Distance 1: " << dist1.GetMeters() << " meters" << endl;
    cerr << "Distance 2: " << dist2.GetMeters() << " meters" << endl;

    cerr << "Distance 1 > Distance 2 ? " << (dist1 > dist2) << endl;
    cerr << "Distance 1 >= Distance 2 ? " << (dist1 >= dist2) << endl;
    cerr << "Distance 1 == Distance 2 ? " << (dist1 == dist2) << endl;
    cerr << "Distance 1 <= Distance 2 ? " << (dist1 <= dist2) << endl;
    cerr << "Distance 1 < Distance 2 ? " << (dist1 < dist2) << endl;
    cerr << "Distance 1 + Distance 2 ? " << (dist1 + dist2).GetMeters() << 
        " meters" << endl;
    cerr << "Distance 1 - Distance 2 ? " << (dist1 - dist2).GetMeters() << 
        " meters" << endl;

    dist1 = dist2;
    cerr << "Distance 1 = Distance 2... Distance 1 = " << dist1.GetMeters() <<
        " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Distance dist1(1, Distance::Meters);
    Distance dist2(10, Distance::Meters);
    cerr << endl;
    cerr << "Distance 1: " << dist1.GetMeters() << " meters" << endl;
    cerr << "Distance 2: " << dist2.GetMeters() << " meters" << endl;

    cerr << "Distance 1 > Distance 2 ? " << (dist1 > dist2) << endl;
    cerr << "Distance 1 >= Distance 2 ? " << (dist1 >= dist2) << endl;
    cerr << "Distance 1 == Distance 2 ? " << (dist1 == dist2) << endl;
    cerr << "Distance 1 <= Distance 2 ? " << (dist1 <= dist2) << endl;
    cerr << "Distance 1 < Distance 2 ? " << (dist1 < dist2) << endl;
    cerr << "Distance 1 + Distance 2 ? " << (dist1 + dist2).GetMeters() << 
        " meters" << endl;

    // This should work since it returns a displacement 
    try {
      cerr << "Distance 1 - Distance 2 ? " << (dist1 - dist2).GetMeters() << 
          " meters" << endl;
      cerr << "Distance 1 -= Distance 2 ? " << endl;
      dist1 -= dist2;
    }
    catch(iException &e) {
      e.Report(false);
    }

    dist1 = dist2;
    cerr << "Distance 1 = Distance 2... Distance 1 = " << dist1.GetMeters() <<
        " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Distance dist1(10, Distance::Meters);
    Distance dist2(1, Distance::Meters);
    cerr << endl;
    cerr << "Distance 1: " << dist1.GetMeters() << " meters" << endl;
    cerr << "Distance 2: " << dist2.GetMeters() << " meters" << endl;

    cerr << "Distance 1 > Distance 2 ? " << (dist1 > dist2) << endl;
    cerr << "Distance 1 >= Distance 2 ? " << (dist1 >= dist2) << endl;
    cerr << "Distance 1 == Distance 2 ? " << (dist1 == dist2) << endl;
    cerr << "Distance 1 <= Distance 2 ? " << (dist1 <= dist2) << endl;
    cerr << "Distance 1 < Distance 2 ? " << (dist1 < dist2) << endl;
    cerr << "Distance 1 + Distance 2 ? " << (dist1 + dist2).GetMeters() << 
        " meters" << endl;
    cerr << "Distance 1 - Distance 2 ? " << (dist1 - dist2).GetMeters() << 
        " meters" << endl;

    dist1 = dist2;
    cerr << "Distance 1 = Distance 2... Distance 1 = " << dist1.GetMeters() <<
        " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Distance dist1(1000, Distance::Meters);
    Distance dist2(1, Distance::Kilometers);
    cerr << endl;
    cerr << "Distance 1: " << dist1.GetMeters() << " meters" << endl;
    cerr << "Distance 2: " << dist2.GetMeters() << " meters" << endl;

    cerr << "Distance 1 > Distance 2 ? " << (dist1 > dist2) << endl;
    cerr << "Distance 1 >= Distance 2 ? " << (dist1 >= dist2) << endl;
    cerr << "Distance 1 == Distance 2 ? " << (dist1 == dist2) << endl;
    cerr << "Distance 1 <= Distance 2 ? " << (dist1 <= dist2) << endl;
    cerr << "Distance 1 < Distance 2 ? " << (dist1 < dist2) << endl;
    cerr << "Distance 1 + Distance 2 ? " << (dist1 + dist2).GetMeters() << 
        " meters" << endl;
    cerr << "Distance 1 - Distance 2 ? " << (dist1 - dist2).GetMeters() << 
        " meters" << endl;

    dist1 = dist2;
    cerr << "Distance 1 = Distance 2... Distance 1 = " << dist1.GetMeters() <<
        " meters" << endl;
    dist1 += dist2;
    cerr << "Distance 1 += Distance 2... Distance 1 = " << dist1.GetMeters() <<
        " meters" << endl;
    dist1 -= dist2;
    cerr << "Distance 1 -= Distance 2... Distance 1 = " << dist1.GetMeters() <<
        " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Distance dist1(10, Distance::Pixels);
    Distance dist2(100, 10.0);
    cerr << endl;
    cerr << "Distance 1: " << dist1.GetMeters() << " meters" << endl;
    cerr << "Distance 2: " << dist2.GetMeters() << " meters" << endl;
    cerr << "Distance 1: " << dist1.GetPixels() << " pixels" << endl;
    cerr << "Distance 2: " << dist2.GetPixels(10.0) << " pixels" << endl;

    cerr << "Distance 1 > Distance 2 ? " << (dist1 > dist2) << endl;
    cerr << "Distance 1 >= Distance 2 ? " << (dist1 >= dist2) << endl;
    cerr << "Distance 1 == Distance 2 ? " << (dist1 == dist2) << endl;
    cerr << "Distance 1 <= Distance 2 ? " << (dist1 <= dist2) << endl;
    cerr << "Distance 1 < Distance 2 ? " << (dist1 < dist2) << endl;
    cerr << "Distance 1 + Distance 2 ? " << (dist1 + dist2).GetMeters() << 
        " meters" << endl;
    cerr << "Distance 1 - Distance 2 ? " << (dist1 - dist2).GetMeters() << 
        " meters" << endl;

    dist1 = dist2;
    cerr << "Distance 1 = Distance 2... Distance 1 = " << dist1.GetMeters() <<
        " meters" << endl;
  }
  catch(iException &e) {
    e.Report(false);
  }

  cerr << endl << "----- Testing Error Checking -----" << endl << endl;

  try {
    Distance dist(-1, Distance::Meters);
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
    Distance() > Distance();
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Distance() >= Distance();
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Distance() < Distance();
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Distance() <= Distance();
  }
  catch(iException &e) {
    e.Report(false);
  }
}
