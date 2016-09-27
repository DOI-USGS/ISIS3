#include "Displacement.h"

#include <iostream>

#include "IException.h"
#include "Preference.h"

using std::cout;
using std::endl;

using namespace Isis;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);

  cout.precision(14);

  cout << "----- Testing Constructors -----" << endl << endl;

  try {
    cout << "Empty constructor" << endl;
    Displacement disp;
    cout << disp.meters() << " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << "Constructor given a value in meters" << endl;
    Displacement disp(1500.5, Displacement::Meters);
    cout << disp.meters() << " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << "Constructor given a value in kilometers" << endl;
    Displacement disp(1500.5, Displacement::Kilometers);
    cout << disp.meters() << " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << "Copy constructor" << endl;
    Displacement disp(1500.5, Displacement::Meters);
    Displacement copiedDisp(disp);
    cout << copiedDisp.meters() << " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << "----- Testing Accessors -----" << endl << endl;

  try {
    cout << "Meters (redundant)" << endl;
    Displacement disp(1, Displacement::Meters);
    cout << disp.meters() << " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << "Kilometers" << endl;
    Displacement disp(1, Displacement::Kilometers);
    cout << disp.kilometers() << " kilometers" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << "----- Testing Operators -----" << endl << endl;

  try {
    Displacement disp1(1, Displacement::Meters);
    Displacement disp2(1, Displacement::Meters);
    cout << endl;
    cout << "Displacement 1: " << disp1.meters() << " meters" << endl;
    cout << "Displacement 2: " << disp2.meters() << " meters" << endl;

    cout << "Displacement 1 > Displacement 2 ? " << (disp1 > disp2) << endl;
    cout << "Displacement 1 >= Displacement 2 ? " << (disp1 >= disp2) << endl;
    cout << "Displacement 1 == Displacement 2 ? " << (disp1 == disp2) << endl;
    cout << "Displacement 1 <= Displacement 2 ? " << (disp1 <= disp2) << endl;
    cout << "Displacement 1 < Displacement 2 ? " << (disp1 < disp2) << endl;
    cout << "Displacement 1 + Displacement 2 ? " <<
        (disp1 + disp2).meters() << " meters" << endl;
    cout << "Displacement 1 - Displacement 2 ? " <<
        (disp1 - disp2).meters() << " meters" << endl;

    disp1 = disp2;
    cout << "Displacement 1 = Displacement 2... Displacement 1 = " <<
        disp1.meters() << " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Displacement disp1(1, Displacement::Meters);
    Displacement disp2(10, Displacement::Meters);
    cout << endl;
    cout << "Displacement 1: " << disp1.meters() << " meters" << endl;
    cout << "Displacement 2: " << disp2.meters() << " meters" << endl;

    cout << "Displacement 1 > Displacement 2 ? " << (disp1 > disp2) << endl;
    cout << "Displacement 1 >= Displacement 2 ? " << (disp1 >= disp2) << endl;
    cout << "Displacement 1 == Displacement 2 ? " << (disp1 == disp2) << endl;
    cout << "Displacement 1 <= Displacement 2 ? " << (disp1 <= disp2) << endl;
    cout << "Displacement 1 < Displacement 2 ? " << (disp1 < disp2) << endl;
    cout << "Displacement 1 + Displacement 2 ? " << (disp1 + disp2).meters() <<
        " meters" << endl;

    // This should throw an exception and never cout
    try {
      (disp1 - disp2).meters();
    }
    catch(IException &e) {
      e.print();
    }

    disp1 = disp2;
    cout << "Displacement 1 = Displacement 2... Displacement 1 = " << disp1.meters() <<
        " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Displacement disp1(10, Displacement::Meters);
    Displacement disp2(1, Displacement::Meters);
    cout << endl;
    cout << "Displacement 1: " << disp1.meters() << " meters" << endl;
    cout << "Displacement 2: " << disp2.meters() << " meters" << endl;

    cout << "Displacement 1 > Displacement 2 ? " << (disp1 > disp2) << endl;
    cout << "Displacement 1 >= Displacement 2 ? " << (disp1 >= disp2) << endl;
    cout << "Displacement 1 == Displacement 2 ? " << (disp1 == disp2) << endl;
    cout << "Displacement 1 <= Displacement 2 ? " << (disp1 <= disp2) << endl;
    cout << "Displacement 1 < Displacement 2 ? " << (disp1 < disp2) << endl;
    cout << "Displacement 1 + Displacement 2 ? " << (disp1 + disp2).meters() <<
        " meters" << endl;
    cout << "Displacement 1 - Displacement 2 ? " << (disp1 - disp2).meters() <<
        " meters" << endl;

    disp1 = disp2;
    cout << "Displacement 1 = Displacement 2... Displacement 1 = " << disp1.meters() <<
        " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Displacement disp1(1000, Displacement::Meters);
    Displacement disp2(1, Displacement::Kilometers);
    cout << endl;
    cout << "Displacement 1: " << disp1.meters() << " meters" << endl;
    cout << "Displacement 2: " << disp2.meters() << " meters" << endl;

    cout << "Displacement 1 > Displacement 2 ? " << (disp1 > disp2) << endl;
    cout << "Displacement 1 >= Displacement 2 ? " << (disp1 >= disp2) << endl;
    cout << "Displacement 1 == Displacement 2 ? " << (disp1 == disp2) << endl;
    cout << "Displacement 1 <= Displacement 2 ? " << (disp1 <= disp2) << endl;
    cout << "Displacement 1 < Displacement 2 ? " << (disp1 < disp2) << endl;
    cout << "Displacement 1 + Displacement 2 ? " << (disp1 + disp2).meters() <<
        " meters" << endl;
    cout << "Displacement 1 - Displacement 2 ? " << (disp1 - disp2).meters() <<
        " meters" << endl;

    disp1 = disp2;
    cout << "Displacement 1 = Displacement 2... Displacement 1 = " << disp1.meters() <<
        " meters" << endl;
    disp1 += disp2;
    cout << "Displacement 1 += Displacement 2... Displacement 1 = " << disp1.meters() <<
        " meters" << endl;
    disp1 -= disp2;
    cout << "Displacement 1 -= Displacement 2... Displacement 1 = " << disp1.meters() <<
        " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Displacement disp1(10, Displacement::Pixels);
    Displacement disp2(100, 10.0);
    cout << endl;
    cout << "Displacement 1: " << disp1.meters() << " meters" << endl;
    cout << "Displacement 2: " << disp2.meters() << " meters" << endl;
    cout << "Displacement 1: " << disp1.pixels() << " pixels" << endl;
    cout << "Displacement 2: " << disp2.pixels(10.0) << " pixels" << endl;

    cout << "Displacement 1 > Displacement 2 ? " << (disp1 > disp2) << endl;
    cout << "Displacement 1 >= Displacement 2 ? " << (disp1 >= disp2) << endl;
    cout << "Displacement 1 == Displacement 2 ? " << (disp1 == disp2) << endl;
    cout << "Displacement 1 <= Displacement 2 ? " << (disp1 <= disp2) << endl;
    cout << "Displacement 1 < Displacement 2 ? " << (disp1 < disp2) << endl;
    cout << "Displacement 1 + Displacement 2 ? " << (disp1 + disp2).meters() <<
        " meters" << endl;
    cout << "Displacement 1 - Displacement 2 ? " << (disp1 - disp2).meters() <<
        " meters" << endl;

    disp1 = disp2;
    cout << "Displacement 1 = Displacement 2... Displacement 1 = " << disp1.meters() <<
        " meters" << endl;
  }
  catch(IException &e) {
    e.print();
  }

  cout << endl << "----- Testing Error Checking -----" << endl << endl;

  try {
    Displacement disp(-1, Displacement::Meters);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Displacement disp(-1, Displacement::Kilometers);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Displacement disp(1, Displacement::Kilometers);
    disp.setMeters(-1);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Displacement disp(1, Displacement::Kilometers);
    disp.setMeters(-1);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    Displacement disp(1, Displacement::Kilometers);
    disp.setKilometers(-1);
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << (Displacement() > Displacement()) << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << (Displacement() > Displacement()) << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << (Displacement() < Displacement()) << endl;
  }
  catch(IException &e) {
    e.print();
  }

  try {
    cout << (Displacement() < Displacement()) << endl;
  }
  catch(IException &e) {
    e.print();
  }
}
