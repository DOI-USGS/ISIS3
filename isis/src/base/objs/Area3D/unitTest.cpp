/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include "Area3D.h"

#include <iostream>

#include "IException.h"
#include "Displacement.h"
#include "Distance.h"
#include "Preference.h"

using std::cerr;
using std::endl;

using namespace Isis;

void printArea(const Area3D &area);

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cerr.precision(14);

  cerr << "----- Testing Constructors -----" << endl << endl;

  try {
    cerr << "Empty constructor" << endl;
    Area3D area;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "Constructor given a values in meters" << endl;
    Displacement a(1500.5, Displacement::Meters);
    Displacement b(1500.5, Displacement::Meters);
    Area3D area(a, a, a, b, b, b);
    printArea(area);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "Constructor given distances" << endl;
    Displacement a(1500.5, Displacement::Meters);
    Distance b(0, Distance::Meters);
    Area3D area(a, a, a, b, b, b);
    printArea(area);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cerr << "Copy constructor" << endl;
    Displacement a(1500.5, Displacement::Meters);
    Distance b(0, Distance::Meters);
    Area3D area(a, a, a, b, b, b);
    Area3D copiedArea(area);
    printArea(copiedArea);
  }
  catch(IException &e) {
    e.print();
  }

  cerr << endl << "----- Testing Operators -----" << endl << endl;

  // Very typical case
  try {
    Displacement a(0, Displacement::Meters);
    Distance b(1, Distance::Meters);
    Distance c(5, Distance::Meters);
    Area3D area1(a, a, a, b, b, b);
    Area3D area2(a, a, a, c, c, c);

    cerr << endl;
    cerr << "Area3D 1: ";
    printArea(area1);
    cerr << "Area3D 2: ";
    printArea(area2);

    cerr << "Area3D 1 == Area3D 2 ? " << (area1 == area2) << endl;
    cerr << "Area3D 1 != Area3D 2 ? " << (area1 != area2) << endl;

    cerr << "Area3D 1 intersect Area3D 2... ";
    printArea( area1.intersect(area2) );

    area1 = area2;
    cerr << "Area3D 1 = Area3D 2... Area3D 1 = ";
    printArea(area1);
  }
  catch(IException &e) {
    e.print();
  }

  // General intersect test
  try {
    Displacement a(0, Displacement::Meters);
    Displacement b(1, Displacement::Meters);
    Displacement c(2, Displacement::Meters);
    Distance d(1, Distance::Meters);
    Distance e(5, Distance::Meters);
    Distance f(3, Distance::Meters);
    Area3D area1(a, b, c, d, e, f);
    Area3D area2(b, a, c, e, f, d);

    cerr << endl;
    cerr << "Area3D 1: ";
    printArea(area1);
    cerr << "Area3D 2: ";
    printArea(area2);

    cerr << "Area3D 1 == Area3D 2 ? " << (area1 == area2) << endl;
    cerr << "Area3D 1 != Area3D 2 ? " << (area1 != area2) << endl;

    cerr << "Area3D 1 intersect Area3D 2... ";
    printArea( area1.intersect(area2) );

    area1 = area2;
    cerr << "Area3D 1 = Area3D 2... Area3D 1 = ";
    printArea(area1);
  }
  catch(IException &e) {
    e.print();
  }

  // Intersect has no overlap
  try {
    Displacement a(-5, Displacement::Meters);
    Displacement b(0, Displacement::Meters);
    Distance c(1, Distance::Meters);
    Distance d(5, Distance::Meters);
    Area3D area1(a, a, a, c, c, c);
    Area3D area2(b, b, b, d, d, d);

    cerr << endl;
    cerr << "Area3D 1: ";
    printArea(area1);
    cerr << "Area3D 2: ";
    printArea(area2);

    cerr << "Area3D 1 == Area3D 2 ? " << (area1 == area2) << endl;
    cerr << "Area3D 1 != Area3D 2 ? " << (area1 != area2) << endl;

    cerr << "Area3D 1 intersect Area3D 2... ";
    printArea( area1.intersect(area2) );

    area1 = area2;
    cerr << "Area3D 1 = Area3D 2... Area3D 1 = ";
    printArea(area1);
  }
  catch(IException &e) {
    e.print();
  }

  // Test with invalid displacements/distances in constructors
  try {
    Displacement a(0, Displacement::Meters);
    Distance b(0, Distance::Meters);
    Distance c(5, Distance::Meters);
    Area3D area1(a, Displacement(), a, b, b, b);
    Area3D area2(a, a, a, c, c, Distance());

    cerr << endl;
    cerr << "Area3D 1: ";
    printArea(area1);
    cerr << "Area3D 2: ";
    printArea(area2);

    cerr << "Area3D 1 == Area3D 2 ? " << (area1 == area2) << endl;
    cerr << "Area3D 1 != Area3D 2 ? " << (area1 != area2) << endl;

    area1 = area2;
    cerr << "Area3D 1 intersect Area3D 2... ";
    printArea( area1.intersect(area2) );

    area1 = area2;
    cerr << "Area3D 1 = Area3D 2... Area3D 1 = ";
    printArea(area1);
  }
  catch(IException &e) {
    e.print();
  }

  // Test displacement v. distance in constructors
  try {
    Displacement a(0, Displacement::Meters);
    Displacement b(0, Displacement::Meters);
    Distance c(5, Distance::Meters);
    Area3D area1(a, Displacement(), a, b, b, b);
    Area3D area2(a, a, a, c, c, Distance());

    cerr << endl;
    cerr << "Area3D 1: ";
    printArea(area1);
    cerr << "Area3D 2: ";
    printArea(area2);

    cerr << "Area3D 1 == Area3D 2 ? " << (area1 == area2) << endl;
    cerr << "Area3D 1 != Area3D 2 ? " << (area1 != area2) << endl;

    area1 = area2;
    cerr << "Area3D 1 intersect Area3D 2... ";
    printArea( area1.intersect(area2) );

    area1 = area2;
    cerr << "Area3D 1 = Area3D 2... Area3D 1 = ";
    printArea(area1);
  }
  catch(IException &e) {
    e.print();
  }

  // Test setStartY
  try {
    Displacement a(0, Displacement::Meters);
    Displacement b(0, Displacement::Meters);
    Distance c(5, Distance::Meters);
    Area3D area1(a, Displacement(), a, b, b, b);
    Area3D area2(a, a, a, c, c, Distance());
    area1.setStartY(a);

    cerr << endl;
    cerr << "Area3D 1: ";
    printArea(area1);
    cerr << "Area3D 2: ";
    printArea(area2);

    cerr << "Area3D 1 == Area3D 2 ? " << (area1 == area2) << endl;
    cerr << "Area3D 1 != Area3D 2 ? " << (area1 != area2) << endl;

    area1 = area2;
    cerr << "Area3D 1 intersect Area3D 2... ";
    printArea( area1.intersect(area2) );

    area1 = area2;
    cerr << "Area3D 1 = Area3D 2... Area3D 1 = ";
    printArea(area1);
  }
  catch(IException &e) {
    e.print();
  }

  // Test moveStartY
  try {
    Displacement a(0, Displacement::Meters);
    Displacement b(0, Displacement::Meters);
    Distance c(5, Distance::Meters);
    Area3D area1(a, Displacement(), a, b, b, b);
    Area3D area2(a, a, a, c, c, Distance());
    area1.moveStartY(a);

    cerr << endl;
    cerr << "Area3D 1: ";
    printArea(area1);
    cerr << "Area3D 2: ";
    printArea(area2);

    cerr << "Area3D 1 == Area3D 2 ? " << (area1 == area2) << endl;
    cerr << "Area3D 1 != Area3D 2 ? " << (area1 != area2) << endl;

    area1 = area2;
    cerr << "Area3D 1 intersect Area3D 2... ";
    printArea( area1.intersect(area2) );

    area1 = area2;
    cerr << "Area3D 1 = Area3D 2... Area3D 1 = ";
    printArea(area1);
  }
  catch(IException &e) {
    e.print();
  }

  // Test invalid displacements
  try {
    Area3D area1;
    Area3D area2;

    cerr << endl;
    cerr << "Area3D 1: ";
    printArea(area1);
    cerr << "Area3D 2: ";
    printArea(area2);

    cerr << "Area3D 1 == Area3D 2 ? " << (area1 == area2) << endl;
    cerr << "Area3D 1 != Area3D 2 ? " << (area1 != area2) << endl;

    area1 = area2;
    cerr << "Area3D 1 intersect Area3D 2... ";
    printArea( area1.intersect(area2) );

    area1 = area2;
    cerr << "Area3D 1 = Area3D 2... Area3D 1 = ";
    printArea(area1);
  }
  catch(IException &e) {
    e.print();
  }

  // Test partially invalid displacements
  try {
    Displacement a(0, Displacement::Meters);
    Displacement c;
    Area3D area1(c, a, c, a, a, c);
    Area3D area2(c, a, c, a, a, c);

    cerr << endl;
    cerr << "Area3D 1: ";
    printArea(area1);
    cerr << "Area3D 2: ";
    printArea(area2);

    cerr << "Area3D 1 == Area3D 2 ? " << (area1 == area2) << endl;
    cerr << "Area3D 1 != Area3D 2 ? " << (area1 != area2) << endl;

    area1 = area2;
    cerr << "Area3D 1 intersect Area3D 2... ";
    printArea( area1.intersect(area2) );

    area1 = area2;
    cerr << "Area3D 1 = Area3D 2... Area3D 1 = ";
    printArea(area1);
  }
  catch(IException &e) {
    e.print();
  }


  cerr << endl << "----- Testing Error Checking -----" << endl << endl;


  try {
    Displacement a(0, Displacement::Meters);
    Displacement b(1, Displacement::Meters);
    Area3D area(b, a, a, a, b, b);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Displacement a(0, Displacement::Meters);
    Displacement b(1, Displacement::Meters);
    Area3D area(a, b, a, b, a, b);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Displacement a(0, Displacement::Meters);
    Displacement b(1, Displacement::Meters);
    Area3D area(a, a, b, b, b, a);
  }
  catch(IException &e) {
    e.print();
  }
}


void printArea(const Area3D &area) {
  cerr << area.getStartX().meters() << ","
        << area.getStartY().meters() << ","
        << area.getStartZ().meters() << " -> "
        << area.getEndX().meters() << ","
        << area.getEndY().meters() << ","
        << area.getEndZ().meters()
        << " && Valid = " << area.isValid() << "\n";
}
