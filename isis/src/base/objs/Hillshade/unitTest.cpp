/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iostream>

#include <QDebug>

#include "Angle.h"
#include "Buffer.h"
#include "IException.h"
#include "Hillshade.h"
#include "PixelType.h"
#include "Preference.h"
#include "SpecialPixel.h"

using namespace Isis;
using namespace std;

int main(int argc, char *argv[]) {
  Preference::Preferences(true);
  cerr.precision(8);

  // Test syntaxes and lack of errors
  {
    Hillshade shade;
    qDebug() << "Default constructed:" << shade;
  }

  {
    Hillshade shade(Angle(), Angle(), Null);
    qDebug() << "Empty constructed:" << shade;
    Hillshade copy(shade);
    qDebug() << "Copied empty:" << copy;
    Hillshade assigned;
    assigned = copy;
    qDebug() << "Assigned empty:" << assigned;

    Hillshade validShade(Angle(0, Angle::Degrees), Angle(15, Angle::Degrees), 5.0);
    qDebug() << "Valid:" << validShade;
    Hillshade validCopy(validShade);
    qDebug() << "Copied valid:" << validCopy;
    Hillshade validAssigned;
    validAssigned = validCopy;
    qDebug() << "Assigned valid:" << validAssigned;
  }

  cerr << endl << endl;
  // Test computation
  cerr << "Test computations" << endl;
  {
    Hillshade shade(Angle(0, Angle::Degrees), Angle(0, Angle::Degrees), 3.0);
    Buffer buf(3, 3, 1, Real);
    buf[0] = 1.0;
    buf[1] = 1.0;
    buf[2] = 1.0;

    buf[3] = 1.0;
    buf[4] = 1.0;
    buf[5] = 1.0;

    buf[6] = 1.0;
    buf[7] = 1.0;
    buf[8] = 1.0;

    cerr << "Shaded value (flat surface, sun directly up): " << shade.shadedValue(buf) << endl;

    // Azimuth should have no effect here
    shade.setAzimuth(Angle(180, Angle::Degrees));
    cerr << "Shaded value (rotated sun's direction):       " << shade.shadedValue(buf) << endl;

    shade.setZenith(Angle(10, Angle::Degrees));
    cerr << "Shaded value (lowered solar elevation):       " << shade.shadedValue(buf) << endl;

    shade.setZenith(Angle(89, Angle::Degrees));
    cerr << "Shaded value (sun is next to the horizon):    " << shade.shadedValue(buf) << endl;

    shade.setZenith(Angle(45, Angle::Degrees));
    cerr << "Shaded value (solar elevation is 45):         " << shade.shadedValue(buf) << endl;

    buf[0] = 3.0;
    buf[1] = 3.0;
    buf[2] = 3.0;

    buf[3] = 2.0;
    buf[4] = 2.0;
    buf[5] = 2.0;
    cerr << "Shaded value (surface tilted towards sun):    " << shade.shadedValue(buf) << endl;

    buf[0] = 4.0;
    buf[1] = 3.0;
    buf[2] = 4.0;

    buf[3] = 3.0;
    buf[4] = 2.0;
    buf[5] = 3.0;

    buf[6] = 2.0;
    buf[7] = 1.0;
    buf[8] = 2.0;
    cerr << "Shaded value (surface folded inwards):        " << shade.shadedValue(buf) << endl;

    buf[0] = 3.0;
    buf[1] = 4.0;
    buf[2] = 3.0;

    buf[3] = 2.0;
    buf[4] = 3.0;
    buf[5] = 2.0;

    buf[6] = 1.0;
    buf[7] = 2.0;
    buf[8] = 1.0;
    cerr << "Shaded value (surface folded outwards):       " << shade.shadedValue(buf) << endl;

    buf[0] = 1.0;
    buf[1] = 1.0;
    buf[2] = 1.0;

    buf[3] = 5.0;
    buf[4] = 5.0;
    buf[5] = 5.0;

    buf[6] = 10.0;
    buf[7] = 10.0;
    buf[8] = 10.0;
    cerr << "Shaded value (surface facing away):          " << shade.shadedValue(buf) << endl;

    buf[0] = Null;
    cerr << "Shaded value special (special pixel in surface): "
         << IsSpecial(shade.shadedValue(buf)) << endl;
  }
  cerr << endl;

  cerr << "Test error handling" << endl << endl;

  // Test errors
  cerr << "Test no resolution" << endl;
  try {
    Hillshade shade(Angle(0, Angle::Degrees), Angle(45, Angle::Degrees), Null);
    Buffer buf(3, 3, 1, Real);
    buf[0] = 1.0;
    buf[1] = 1.0;
    buf[2] = 1.0;

    buf[3] = 1.0;
    buf[4] = 1.0;
    buf[5] = 1.0;

    buf[6] = 1.0;
    buf[7] = 1.0;
    buf[8] = 1.0;

    shade.shadedValue(buf);
  }
  catch (IException &e) {
    e.print();
  }

  cerr << "Test zero resolution" << endl;
  try {
    Hillshade shade(Angle(0, Angle::Degrees), Angle(45, Angle::Degrees), 0.0);
    Buffer buf(3, 3, 1, Real);
    buf[0] = 1.0;
    buf[1] = 1.0;
    buf[2] = 1.0;

    buf[3] = 1.0;
    buf[4] = 1.0;
    buf[5] = 1.0;

    buf[6] = 1.0;
    buf[7] = 1.0;
    buf[8] = 1.0;

    shade.shadedValue(buf);
  }
  catch (IException &e) {
    e.print();
  }

  cerr << "Test no zenith" << endl;
  try {
    Hillshade shade(Angle(0, Angle::Degrees), Angle(), 1.0);
    Buffer buf(3, 3, 1, Real);
    buf[0] = 1.0;
    buf[1] = 1.0;
    buf[2] = 1.0;

    buf[3] = 1.0;
    buf[4] = 1.0;
    buf[5] = 1.0;

    buf[6] = 1.0;
    buf[7] = 1.0;
    buf[8] = 1.0;

    shade.shadedValue(buf);
  }
  catch (IException &e) {
    e.print();
  }

  cerr << "Test no azimuth" << endl;
  try {
    Hillshade shade(Angle(), Angle(45, Angle::Degrees), 1.0);
    Buffer buf(3, 3, 1, Real);
    buf[0] = 1.0;
    buf[1] = 1.0;
    buf[2] = 1.0;

    buf[3] = 1.0;
    buf[4] = 1.0;
    buf[5] = 1.0;

    buf[6] = 1.0;
    buf[7] = 1.0;
    buf[8] = 1.0;

    shade.shadedValue(buf);
  }
  catch (IException &e) {
    e.print();
  }

  cerr << "Test out of range azimuth 1" << endl;
  try {
    Hillshade shade(Angle(-90, Angle::Degrees), Angle(45, Angle::Degrees), 1.0);
    Buffer buf(3, 3, 1, Real);
    buf[0] = 1.0;
    buf[1] = 1.0;
    buf[2] = 1.0;

    buf[3] = 1.0;
    buf[4] = 1.0;
    buf[5] = 1.0;

    buf[6] = 1.0;
    buf[7] = 1.0;
    buf[8] = 1.0;

    shade.shadedValue(buf);
  }
  catch (IException &e) {
    e.print();
  }

  cerr << "Test out of range azimuth 2" << endl;
  try {
    Hillshade shade(Angle(380, Angle::Degrees), Angle(45, Angle::Degrees), 1.0);
    Buffer buf(3, 3, 1, Real);
    buf[0] = 1.0;
    buf[1] = 1.0;
    buf[2] = 1.0;

    buf[3] = 1.0;
    buf[4] = 1.0;
    buf[5] = 1.0;

    buf[6] = 1.0;
    buf[7] = 1.0;
    buf[8] = 1.0;

    shade.shadedValue(buf);
  }
  catch (IException &e) {
    e.print();
  }

  cerr << "Test out of range zenith 1" << endl;
  try {
    Hillshade shade(Angle(0, Angle::Degrees), Angle(-45, Angle::Degrees), 1.0);
    Buffer buf(3, 3, 1, Real);
    buf[0] = 1.0;
    buf[1] = 1.0;
    buf[2] = 1.0;

    buf[3] = 1.0;
    buf[4] = 1.0;
    buf[5] = 1.0;

    buf[6] = 1.0;
    buf[7] = 1.0;
    buf[8] = 1.0;

    shade.shadedValue(buf);
  }
  catch (IException &e) {
    e.print();
  }

  cerr << "Test out of range zenith 2" << endl;
  try {
    Hillshade shade(Angle(0, Angle::Degrees), Angle(95, Angle::Degrees), 1.0);
    Buffer buf(3, 3, 1, Real);
    buf[0] = 1.0;
    buf[1] = 1.0;
    buf[2] = 1.0;

    buf[3] = 1.0;
    buf[4] = 1.0;
    buf[5] = 1.0;

    buf[6] = 1.0;
    buf[7] = 1.0;
    buf[8] = 1.0;

    shade.shadedValue(buf);
  }
  catch (IException &e) {
    e.print();
  }

  cerr << "Test bad buffer 1" << endl;
  try {
    Hillshade shade(Angle(0, Angle::Degrees), Angle(45, Angle::Degrees), 1.0);
    Buffer buf(1, 2, 3, Real);
    buf[0] = 1.0;
    buf[1] = 1.0;

    buf[2] = 1.0;
    buf[3] = 1.0;

    buf[4] = 1.0;
    buf[5] = 1.0;

    shade.shadedValue(buf);
  }
  catch (IException &e) {
    e.print();
  }

  cerr << "Test bad buffer 2" << endl;
  try {
    Hillshade shade(Angle(0, Angle::Degrees), Angle(45, Angle::Degrees), 1.0);
    Buffer buf(3, 2, 3, Real);
    buf[0] = 1.0;
    buf[1] = 1.0;

    buf[2] = 1.0;
    buf[3] = 1.0;

    buf[4] = 1.0;
    buf[5] = 1.0;

    shade.shadedValue(buf);
  }
  catch (IException &e) {
    e.print();
  }

  cerr << "Test bad buffer 3" << endl;
  try {
    Hillshade shade(Angle(0, Angle::Degrees), Angle(45, Angle::Degrees), 1.0);
    Buffer buf(3, 3, 3, Real);
    buf[0] = 1.0;
    buf[1] = 1.0;

    buf[2] = 1.0;
    buf[3] = 1.0;

    buf[4] = 1.0;
    buf[5] = 1.0;

    shade.shadedValue(buf);
  }
  catch (IException &e) {
    e.print();
  }
}
