/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Distance.h"

#include <iostream>

#include "Displacement.h"
#include "IException.h"
#include "Preference.h"

using std::cerr;
using std::endl;

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cerr.precision(14);

  cerr << "----- Testing Constructors -----" << endl << endl;

  try {
    cerr << "Empty constructor" << endl;
    Distance dist;
    cerr << dist.meters() << " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "Constructor given a value in meters" << endl;
    Distance dist(1500.5, Distance::Meters);
    cerr << dist.meters() << " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "Constructor given a value in kilometers" << endl;
    Distance dist(1500.5, Distance::Kilometers);
    cerr << dist.meters() << " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "Constructor given a value in solar radius" << endl;
    Distance dist(2, Distance::SolarRadii);
    cerr << dist.meters() << " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "Copy constructor" << endl;
    Distance dist(1500.5, Distance::Meters);
    Distance copiedDist(dist);
    cerr << copiedDist.meters() << " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  cerr << endl << "----- Testing Accessors -----" << endl << endl;

  try {
    cerr << "Meters (redundant)" << endl;
    Distance dist(1, Distance::Meters);
    cerr << dist.meters() << " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "Kilometers" << endl;
    Distance dist(1, Distance::Kilometers);
    cerr << dist.kilometers() << " kilometers" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "Solar Radius" << endl;
    Distance dist(1, Distance::SolarRadii);
    cerr << dist.solarRadii() << " solar radii" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  cerr << endl << "----- Testing Operators -----" << endl << endl;

  try {
    Distance dist1(1, Distance::Meters);
    Distance dist2(1, Distance::Meters);
    cerr << endl;
    cerr << "Distance 1: " << dist1.meters() << " meters" << endl;
    cerr << "Distance 2: " << dist2.meters() << " meters" << endl;

    cerr << "Distance 1 > Distance 2 ? " << (dist1 > dist2) << endl;
    cerr << "Distance 1 >= Distance 2 ? " << (dist1 >= dist2) << endl;
    cerr << "Distance 1 == Distance 2 ? " << (dist1 == dist2) << endl;
    cerr << "Distance 1 <= Distance 2 ? " << (dist1 <= dist2) << endl;
    cerr << "Distance 1 < Distance 2 ? " << (dist1 < dist2) << endl;
    cerr << "Distance 1 + Distance 2 ? " << (dist1 + dist2).meters() <<
        " meters" << endl;
    cerr << "Distance 1 - Distance 2 ? " << (dist1 - dist2).meters() <<
        " meters" << endl;

    dist1 = dist2;
    cerr << "Distance 1 = Distance 2... Distance 1 = " << dist1.meters() <<
        " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Distance dist1(1, Distance::Meters);
    Distance dist2(10, Distance::Meters);
    cerr << endl;
    cerr << "Distance 1: " << dist1.meters() << " meters" << endl;
    cerr << "Distance 2: " << dist2.meters() << " meters" << endl;

    cerr << "Distance 1 > Distance 2 ? " << (dist1 > dist2) << endl;
    cerr << "Distance 1 >= Distance 2 ? " << (dist1 >= dist2) << endl;
    cerr << "Distance 1 == Distance 2 ? " << (dist1 == dist2) << endl;
    cerr << "Distance 1 <= Distance 2 ? " << (dist1 <= dist2) << endl;
    cerr << "Distance 1 < Distance 2 ? " << (dist1 < dist2) << endl;
    cerr << "Distance 1 + Distance 2 ? " << (dist1 + dist2).meters() <<
        " meters" << endl;

    // This should work since it returns a displacement
    try {
      cerr << "Distance 1 - Distance 2 ? " << (dist1 - dist2).meters() <<
          " meters" << endl;
      cerr << "Distance 1 -= Distance 2 ? " << endl;
      dist1 -= dist2;
    }
    catch(IException &e) {
      e.print();
    }

    dist1 = dist2;
    cerr << "Distance 1 = Distance 2... Distance 1 = " << dist1.meters() <<
        " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Distance dist1(10, Distance::Meters);
    Distance dist2(1, Distance::Meters);
    cerr << endl;
    cerr << "Distance 1: " << dist1.meters() << " meters" << endl;
    cerr << "Distance 2: " << dist2.meters() << " meters" << endl;

    cerr << "Distance 1 > Distance 2 ? " << (dist1 > dist2) << endl;
    cerr << "Distance 1 >= Distance 2 ? " << (dist1 >= dist2) << endl;
    cerr << "Distance 1 == Distance 2 ? " << (dist1 == dist2) << endl;
    cerr << "Distance 1 <= Distance 2 ? " << (dist1 <= dist2) << endl;
    cerr << "Distance 1 < Distance 2 ? " << (dist1 < dist2) << endl;
    cerr << "Distance 1 + Distance 2 ? " << (dist1 + dist2).meters() <<
        " meters" << endl;
    cerr << "Distance 1 - Distance 2 ? " << (dist1 - dist2).meters() <<
        " meters" << endl;

    dist1 = dist2;
    cerr << "Distance 1 = Distance 2... Distance 1 = " << dist1.meters() <<
        " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Distance dist1(1000, Distance::Meters);
    Distance dist2(1, Distance::Kilometers);
    cerr << endl;
    cerr << "Distance 1: " << dist1.meters() << " meters" << endl;
    cerr << "Distance 2: " << dist2.meters() << " meters" << endl;

    cerr << "Distance 1 > Distance 2 ? " << (dist1 > dist2) << endl;
    cerr << "Distance 1 >= Distance 2 ? " << (dist1 >= dist2) << endl;
    cerr << "Distance 1 == Distance 2 ? " << (dist1 == dist2) << endl;
    cerr << "Distance 1 <= Distance 2 ? " << (dist1 <= dist2) << endl;
    cerr << "Distance 1 < Distance 2 ? " << (dist1 < dist2) << endl;
    cerr << "Distance 1 + Distance 2 ? " << (dist1 + dist2).meters() <<
        " meters" << endl;
    cerr << "Distance 1 - Distance 2 ? " << (dist1 - dist2).meters() <<
        " meters" << endl;

    dist1 = dist2;
    cerr << "Distance 1 = Distance 2... Distance 1 = " << dist1.meters() <<
        " meters" << endl;
    dist1 += dist2;
    cerr << "Distance 1 += Distance 2... Distance 1 = " << dist1.meters() <<
        " meters" << endl;
    dist1 -= dist2;
    cerr << "Distance 1 -= Distance 2... Distance 1 = " << dist1.meters() <<
        " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Distance dist1(10, Distance::Pixels);
    Distance dist2(100, 10.0);
    cerr << endl;
    cerr << "Distance 1: " << dist1.meters() << " meters" << endl;
    cerr << "Distance 2: " << dist2.meters() << " meters" << endl;
    cerr << "Distance 1: " << dist1.pixels() << " pixels" << endl;
    cerr << "Distance 2: " << dist2.pixels(10.0) << " pixels" << endl;

    cerr << "Distance 1 > Distance 2 ? " << (dist1 > dist2) << endl;
    cerr << "Distance 1 >= Distance 2 ? " << (dist1 >= dist2) << endl;
    cerr << "Distance 1 == Distance 2 ? " << (dist1 == dist2) << endl;
    cerr << "Distance 1 <= Distance 2 ? " << (dist1 <= dist2) << endl;
    cerr << "Distance 1 < Distance 2 ? " << (dist1 < dist2) << endl;
    cerr << "Distance 1 + Distance 2 ? " << (dist1 + dist2).meters() <<
        " meters" << endl;
    cerr << "Distance 1 - Distance 2 ? " << (dist1 - dist2).meters() <<
        " meters" << endl;

    dist1 = dist2;
    cerr << "Distance 1 = Distance 2... Distance 1 = " << dist1.meters() <<
        " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  cerr << endl << "----- Testing Error Checking -----" << endl << endl;

  try {
    Distance dist(-1, Distance::Meters);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Distance dist(-1, Distance::Kilometers);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Distance dist(1, Distance::Kilometers);
    dist.setMeters(-1);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Distance dist(1, Distance::Kilometers);
    dist.setMeters(-1);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Distance dist(1, Distance::Kilometers);
    dist.setKilometers(-1);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << (Distance() > Distance()) << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << (Distance() > Distance()) << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << (Distance() < Distance()) << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << (Distance() < Distance()) << endl;
  }
  catch(IException &e) {
    e.print();
  }
}
