#include "Area3D.h"

#include <iostream>

#include "iException.h"
#include "Displacement.h"
#include "Distance.h"
#include "Preference.h"

using std::cerr;
using std::endl;

using Isis::Area3D;
using Isis::iException;
using Isis::Displacement;
using Isis::Distance;
using Isis::Preference;

void printArea(const Area3D &area);

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cerr.precision(14);

  cerr << "----- Testing Constructors -----" << endl << endl;

  try {
    cerr << "Empty constructor" << endl;
    Area3D area;
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cerr << "Constructor given a values in meters" << endl;
    Displacement a(1500.5, Displacement::Meters);
    Displacement b(1500.5, Displacement::Meters);
    Area3D area(a, a, a, b, b, b);
    printArea(area);
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cerr << "Constructor given distances" << endl;
    Displacement a(1500.5, Displacement::Meters);
    Distance b(0, Distance::Meters);
    Area3D area(a, a, a, b, b, b);
    printArea(area);
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    cerr << "Copy constructor" << endl;
    Displacement a(1500.5, Displacement::Meters);
    Distance b(0, Distance::Meters);
    Area3D area(a, a, a, b, b, b);
    Area3D copiedArea(area);
    printArea(copiedArea);
  }
  catch(iException &e) {
    e.Report(false);
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
  catch(iException &e) {
    e.Report(false);
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
  catch(iException &e) {
    e.Report(false);
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
  catch(iException &e) {
    e.Report(false);
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
  catch(iException &e) {
    e.Report(false);
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
  catch(iException &e) {
    e.Report(false);
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
  catch(iException &e) {
    e.Report(false);
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
  catch(iException &e) {
    e.Report(false);
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
  catch(iException &e) {
    e.Report(false);
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
  catch(iException &e) {
    e.Report(false);
  }


  cerr << endl << "----- Testing Error Checking -----" << endl << endl;


  try {
    Displacement a(0, Displacement::Meters);
    Displacement b(1, Displacement::Meters);
    Area3D area(b, a, a, a, b, b);
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Displacement a(0, Displacement::Meters);
    Displacement b(1, Displacement::Meters);
    Area3D area(a, b, a, b, a, b);
  }
  catch(iException &e) {
    e.Report(false);
  }

  try {
    Displacement a(0, Displacement::Meters);
    Displacement b(1, Displacement::Meters);
    Area3D area(a, a, b, b, b, a);
  }
  catch(iException &e) {
    e.Report(false);
  }
}


void printArea(const Area3D &area) {
  cerr << area.getStartX().GetMeters() << ","
        << area.getStartY().GetMeters() << ","
        << area.getStartZ().GetMeters() << " -> "
        << area.getEndX().GetMeters() << ","
        << area.getEndY().GetMeters() << ","
        << area.getEndZ().GetMeters()
        << " && Valid = " << area.isValid() << "\n";
}
