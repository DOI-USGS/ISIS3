#include <iostream>
#include <exception>

#include "iException.h"
#include "SerialNumberList.h"
#include "PolygonTools.h"
#include "ImageOverlapSet.h"
#include "Preference.h"
#include "geos/util/GEOSException.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/CoordinateArraySequence.h"

using namespace std;

int main () {
  Isis::Preference::Preferences(true);
  void PrintImageOverlap (const Isis::ImageOverlap *poi);

  // Create 6 multi polygons
  //     01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
  // 10        +--------------------------+
  //           |              B           |
  // 09  +-----|--------+              +--|--------+
  //     | A   | a      |              |c |   C    |
  // 08  |     |     +--|--------------|--|-----+  |
  //     |     |     |g |     d        |h |     |  |
  // 07  |     +-----|--|--------------|--+  e  |  |
  //     |           |  |              |        |  |
  // 06  |           |b |    D       +-|-----+  |  | 
  //     |           |  |            | |  f  |  |  |
  // 05  |           |  |            | +-----|--|--+
  //     |           |  |            |   E   |  |  
  // 04  +-----------|--+            +-------+  |
  //                 |                          |
  // 03              |                          |
  //                 |                          |
  // 02  +--------+  +--------------------------+
  //     |   F    |
  // 01  +--------+
  //
  //  Name   Comes From
  //  A      A-a-b               <---First start
  //  B      B-a-c-d
  //  C      C-c-e-f
  //  D      D-b-d-e
  //  E      E-f
  //  F      F                   <---First stop
  //  a      AnB-g               <---Second start
  //  b      AnD-g
  //  c      BnC-h
  //  d      BnD-h
  //  e      CnD
  //  f      CnE
  //         DnE                * Equivalent to E so throw it away
  //  g      anb                <---Third start
  //  h      cnd                <-- Second stop
  //
  //-----------------------------------------------------------------

  cout << "Test 1" << endl;

  // Fill a vector of MultiPolygon* and serial numbers
  vector<geos::geom::MultiPolygon*> boundaries;
  vector<string> sns;

  // Reusable variables
  geos::geom::CoordinateSequence *pts;
  vector<geos::geom::Geometry*> polys;

  // Create the A polygon
  pts = new geos::geom::CoordinateArraySequence ();
  pts->add (geos::geom::Coordinate (1,9));
  pts->add (geos::geom::Coordinate (6,9));
  pts->add (geos::geom::Coordinate (6,4));
  pts->add (geos::geom::Coordinate (1,4));
  pts->add (geos::geom::Coordinate (1,9));

  polys.push_back (Isis::globalFactory.createPolygon (
                   Isis::globalFactory.createLinearRing(pts),NULL));

  boundaries.push_back(Isis::globalFactory.createMultiPolygon (polys));

  for (unsigned int i=0; i<polys.size(); ++i) delete polys[i];
  polys.clear();
  sns.push_back("A");

  // Create the B polygon
  pts = new geos::geom::DefaultCoordinateSequence ();
  pts->add (geos::geom::Coordinate (3,10));
  pts->add (geos::geom::Coordinate (12,10));
  pts->add (geos::geom::Coordinate (12,7));
  pts->add (geos::geom::Coordinate (3,7));
  pts->add (geos::geom::Coordinate (3,10));

  polys.push_back (Isis::globalFactory.createPolygon (
                    Isis::globalFactory.createLinearRing (pts),NULL));
  boundaries.push_back(Isis::globalFactory.createMultiPolygon (polys));

  for (unsigned int i=0; i<polys.size(); ++i) delete polys[i];
  polys.clear();
  sns.push_back("B");

  // Create the C polygon
  pts = new geos::geom::CoordinateArraySequence ();
  pts->add (geos::geom::Coordinate (11,5));
  pts->add (geos::geom::Coordinate (11,9));
  pts->add (geos::geom::Coordinate (15,9));
  pts->add (geos::geom::Coordinate (15,5));
  pts->add (geos::geom::Coordinate (11,5));

  polys.push_back (Isis::globalFactory.createPolygon (
                    Isis::globalFactory.createLinearRing (pts),NULL));
  boundaries.push_back(Isis::globalFactory.createMultiPolygon (polys));

  for (unsigned int i=0; i<polys.size(); ++i) delete polys[i];
  polys.clear();
  sns.push_back("C");

  // Create the D polygon
  pts = new geos::geom::CoordinateArraySequence ();
  pts->add (geos::geom::Coordinate (14,8));
  pts->add (geos::geom::Coordinate (14,2));
  pts->add (geos::geom::Coordinate (5,2));
  pts->add (geos::geom::Coordinate (5,8));
  pts->add (geos::geom::Coordinate (14,8));

  polys.push_back (Isis::globalFactory.createPolygon (
                    Isis::globalFactory.createLinearRing (pts),NULL));
  boundaries.push_back(Isis::globalFactory.createMultiPolygon (polys));

  for (unsigned int i=0; i<polys.size(); ++i) delete polys[i];
  polys.clear();
  sns.push_back("D");

  // Create the E polygon
  pts = new geos::geom::CoordinateArraySequence ();
  pts->add (geos::geom::Coordinate (10,6));
  pts->add (geos::geom::Coordinate (13,6));
  pts->add (geos::geom::Coordinate (13,4));
  pts->add (geos::geom::Coordinate (10,4));
  pts->add (geos::geom::Coordinate (10,6));

  polys.push_back (Isis::globalFactory.createPolygon (
                    Isis::globalFactory.createLinearRing (pts),NULL));
  boundaries.push_back(Isis::globalFactory.createMultiPolygon (polys));

  for (unsigned int i=0; i<polys.size(); ++i) delete polys[i];
  polys.clear();
  sns.push_back("E");

  // Create the F polygon
  pts = new geos::geom::CoordinateArraySequence ();
  pts->add (geos::geom::Coordinate (1,1));
  pts->add (geos::geom::Coordinate (1,2));
  pts->add (geos::geom::Coordinate (4,2));
  pts->add (geos::geom::Coordinate (4,1));
  pts->add (geos::geom::Coordinate (1,1));

  polys.push_back (Isis::globalFactory.createPolygon (
                    Isis::globalFactory.createLinearRing (pts),NULL));
  boundaries.push_back(Isis::globalFactory.createMultiPolygon (polys));

  for (unsigned int i=0; i<polys.size(); ++i) delete polys[i];
  polys.clear();
  sns.push_back("F");

  // Create a ImageOverlapSet object with the multipolys and sns from above
  Isis::ImageOverlapSet overlapSet1(true);
  Isis::ImageOverlapSet overlapSet2(true);
  overlapSet1.FindImageOverlaps(sns, boundaries);

  // Test read/write methods
  overlapSet1.WriteImageOverlaps("unitTest.tmp");
  overlapSet2.ReadImageOverlaps("unitTest.tmp");

  remove("unitTest.tmp");

  // Print each overlap area
  for (int i=0; i<overlapSet2.Size(); i++) {
    PrintImageOverlap(overlapSet2[i]);
  }

  cout << endl;
}




// Print an ImageOverlap
void PrintImageOverlap (const Isis::ImageOverlap *poi) {

  // Write the wkt version of the multi polygon to the screen
  const geos::geom::MultiPolygon *mp = poi->Polygon();
  cout << "Well Known Text" << endl;
  cout << "  " << mp->toString() << endl;
  cout << "  Number of serial numbers: " << poi->Size() << endl;;
  cout << "  Serial numbers: " << endl;
  for (int i=0; i<poi->Size(); i++) {
    cout << "    " << (*poi)[i] << endl;
  }
  cout << endl;
  return;
}

