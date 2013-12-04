/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */
#include <iomanip>
#include <iostream>
#include <vector>
#include <utility>

#include <QList>
#include <QPointF>

#include "Camera.h"
#include "CameraFactory.h"
#include "FileName.h"
#include "IException.h"
#include "PixelIfov.h"
#include "Preference.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

/**
 * @internal
 */
int main(void) {
  Preference::Preferences(true);

  cout << "Unit Test for PixelIfov..." << endl;
  try {
//  char file[1024] = "$cassini/testData/CM_1515951157_1.ir.cub";
    char file[1024] = "$dawn/testData/FC21B0001010_09049002212F5D.cub";
    QList<double> knownLat;
    knownLat << 48.366139970 <<  48.365525166 << 48.366769868 << 48.367384602;
    QList<double> knownLon;
    knownLon << 277.953830179 << 277.951849380 << 277.951061539 << 277.953042126;

    Pvl p(file);
    Camera *cam = CameraFactory::Create(p);
    cout << "FileName: " << FileName(p.fileName()).name() << endl;

    cout.setf(std::ios::fixed);
    cout << setprecision(9);
    PixelIfov pifov;

    //  Test center pixel
    cout << "Pixel IFOV for center pixel position ..." << endl;
    double samp = cam->Samples() / 2;
    double line = cam->Lines() / 2;

    if (!cam->SetImage(samp, line)) {
      cout << "ERROR" << endl;
      return 0;
    }
    QList<QPointF> pifovVertices = pifov.latLonVertices(*cam);
    int numVertices = pifovVertices.size();
    if (numVertices != knownLat.size()) {
      cout << "ERROR - PixelIfov returning " << numVertices << " vertices.  Should be returning " <<
         knownLat.size() << " vertices.";
      return 0;
    }
    //  Get lat/lon for each vertex of the ifov
    for (int j = 0; j < numVertices; j++) {
      if (abs(pifovVertices.at(j).x() - knownLat.at(j)) < 1E-8) {
        cout << "Vertex " << j+1 << " Latitude OK" << endl;
      }
      if (abs(pifovVertices.at(j).y() - knownLon.at(j)) < 1E-8) {
        cout << "Vertex " << j+1 << " Longitude OK" << endl;
      }
    }
    cout << endl;
  }
  catch(IException &e) {
    e.print();
  }
}

