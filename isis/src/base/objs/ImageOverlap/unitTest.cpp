#include <iostream>

#include "Preference.h"
#include "ImageOverlap.h"
#include "PolygonTools.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/Polygon.h"
#include "geos/geom/MultiPolygon.h"
#include "geos/util/GEOSException.h"


using namespace std;

int main() {
  Isis::Preference::Preferences(true);
  void PrintImageOverlap(Isis::ImageOverlap & poi);

  {
    cout << "One ----------------------------------------------" << endl;

    // Create coordinate sequence so we can later create a multi polygon
    geos::geom::CoordinateSequence *pts = new geos::geom::CoordinateArraySequence();
    pts->add(geos::geom::Coordinate(0, 0));
    pts->add(geos::geom::Coordinate(0, 10));
    pts->add(geos::geom::Coordinate(5, 15));
    pts->add(geos::geom::Coordinate(8, -5));
    pts->add(geos::geom::Coordinate(0, 0));

    // Write the original coordinates to the screen
    cout << "Original coordinates:" << endl;
    cout << pts->toString() << endl;

    // Create a multi polygon
    vector<geos::geom::Geometry *> polys;
    polys.push_back(Isis::globalFactory.createPolygon(
                      Isis::globalFactory.createLinearRing(pts), NULL));
    geos::geom::MultiPolygon *mPolygon = Isis::globalFactory.createMultiPolygon(polys);

    // Add more coordinates so we can make sure we did a deep copy when we
    // created the multipolygon
    pts->add(geos::geom::Coordinate(-1, -1));
    pts->add(geos::geom::Coordinate(-2, -2));

    Isis::ImageOverlap a("idOne", *mPolygon);

    PrintImageOverlap(a);

    // Add some serial numbers and print it again
    cout << "Two ----------------------------------------------" << endl;
    std::string st = "sn2";
    a.Add(st);
    st = "sn3";
    a.Add(st);
    st = "sn4";
    a.Add(st);

    PrintImageOverlap(a);

    // Create a new multipolygon and test the SetPolygon method
    cout << "Three --------------------------------------------" << endl;
    geos::geom::CoordinateSequence *pts3 = new geos::geom::CoordinateArraySequence();
    pts3->add(geos::geom::Coordinate(0.123456789, 0.123456789));
    pts3->add(geos::geom::Coordinate(0.123456789, 10.123456789));
    pts3->add(geos::geom::Coordinate(5.123456789, 15.123456789));
    pts3->add(geos::geom::Coordinate(8.123456789, -5.123456789));
    pts3->add(geos::geom::Coordinate(0.123456789, 0.123456789));

    try {
      vector<geos::geom::Geometry *> polys3;
      polys3.push_back(Isis::globalFactory.createPolygon(
                         Isis::globalFactory.createLinearRing(pts3), NULL));
      geos::geom::MultiPolygon *mPolygon3 = Isis::globalFactory.createMultiPolygon(polys3);
      a.SetPolygon(*mPolygon3);
      delete mPolygon3;
    }
    catch(geos::util::GEOSException *exc) {
      cout << "GEOS Exception: " << exc->what() << endl;
      delete exc;
    }

    PrintImageOverlap(a);


    geos::geom::CoordinateSequence *pts4 = new geos::geom::DefaultCoordinateSequence();
    pts4->add(geos::geom::Coordinate(10.123456789, 10.123456789));
    pts4->add(geos::geom::Coordinate(10.123456789, 110.123456789));
    pts4->add(geos::geom::Coordinate(15.123456789, 115.123456789));
    pts4->add(geos::geom::Coordinate(18.123456789, -15.123456789));
    pts4->add(geos::geom::Coordinate(10.123456789, 10.123456789));

    try {
      vector<geos::geom::Geometry *> polys4;
      polys4.push_back(Isis::globalFactory.createPolygon(
                         Isis::globalFactory.createLinearRing(pts4), NULL));
      geos::geom::MultiPolygon *mPolygon4 = Isis::globalFactory.createMultiPolygon(polys4);
      a.SetPolygon(mPolygon4);
      delete mPolygon4;
    }
    catch(geos::util::GEOSException *exc) {
      cout << "GEOS Exception: " << exc->what() << endl;
      delete exc;
    }

    PrintImageOverlap(a);


  }

  {
    //
    cout << "Four ---------------------------------------------" << endl;
    geos::geom::CoordinateSequence *pts = new geos::geom::CoordinateArraySequence();
    pts->add(geos::geom::Coordinate(0.123456789, 0.123456789));
    pts->add(geos::geom::Coordinate(0.123456789, 10.123456789));
    pts->add(geos::geom::Coordinate(5.123456789, 15.123456789));
    pts->add(geos::geom::Coordinate(8.123456789, -5.123456789));
    pts->add(geos::geom::Coordinate(0.123456789, 0.123456789));

    try {
      vector<geos::geom::Geometry *> polys;
      polys.push_back(Isis::globalFactory.createPolygon(
                        Isis::globalFactory.createLinearRing(pts), NULL));

      geos::geom::MultiPolygon *mPolygon = Isis::globalFactory.createMultiPolygon(polys);

      Isis::ImageOverlap a("idFour", *mPolygon);
      PrintImageOverlap(a);
    }
    catch(geos::util::GEOSException *exc) {
      cout << "GEOS Exception: " << exc->what() << endl;
      delete exc;
    }
  }

}

// Print a ImageOverlap
void PrintImageOverlap(Isis::ImageOverlap &poi) {

  // Write the wkt version of the multi polygon to the screen
  const geos::geom::MultiPolygon *mp = poi.Polygon();
  cout << "Well Known Text version of the multi polygon" << endl;
  cout << mp->toString() << endl << endl;

  cout << "Area of the polygon " << poi.Area() << endl;
  cout << "Number of serial numbers: " << poi.Size() << endl;;
  cout << "Serial numbers: " << endl;
  for(int i = 0; i < poi.Size(); i++) {
    cout << "  " << poi[i] << endl;
  }
  cout << endl;
  return;
}
