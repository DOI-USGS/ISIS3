/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <iomanip>
#include <iostream>
#include <vector>
#include <utility>

#include <QList>
#include <QPointF>

#include "Camera.h"
#include "CameraFactory.h"
#include "Cube.h"
#include "FileName.h"
#include "IException.h"
#include "PixelFOV.h"
#include "Preference.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

/**
 * Unit test for PixelFOV class
 *
 * @author ????-??-?? Unknown
 *
 * @internal
 *   @history 2016-10-27 Jesse Mapel - Adapted from old PixelIfov unit test.
 */   
int main(void) {
  Preference::Preferences(true);

  cout << "Unit Test for PixelFOV..." << endl;
  try {
    char file[1024] = "$ISISTESTDATA/isis/src/dawn/unitTestData/FC21B0001010_09049002212F5D.cub";
    QList<double> knownLat;
    knownLat << 48.366139970 <<  48.365525166 << 48.366769868 << 48.367384602;
    QList<double> knownLon;
    knownLon << 277.953830179 << 277.951849380 << 277.951061539 << 277.953042126;

    Cube cube;
    cube.open(file);
    Camera *cam = CameraFactory::Create(cube);
    cout << "FileName: " << file << endl;

    cout.setf(std::ios::fixed);
    cout << setprecision(9);
    PixelFOV pifov;

    //  Test center pixel
    cout << "Pixel IFOV for center pixel position ..." << endl;
    double samp = cam->Samples() / 2;
    double line = cam->Lines() / 2;

    if (!cam->SetImage(samp, line)) {
      cout << "ERROR" << endl;
      return 0;
    }
    QList< QList<QPointF> > pifovVertices = pifov.latLonVertices(*cam, samp, line);
    QList<QPointF> iFOVPointCloud = pifovVertices[0];
    int numVertices = iFOVPointCloud.size();
    if (numVertices != knownLat.size()) {
      cout << "ERROR - PixelFOV returning " << numVertices << " vertices.  Should be returning " <<
         knownLat.size() << " vertices.";
      return 0;
    }
    //  Get lat/lon for each vertex of the ifov
    for (int j = 0; j < numVertices; j++) {
      if (abs(iFOVPointCloud.at(j).x() - knownLat.at(j)) < 1E-8) {
        cout << "Vertex " << j+1 << " Latitude OK" << endl;
      }
      else {
        cout << "Vertex " << j+1 << " Latitude " << iFOVPointCloud.at(j).x() << ", expected latitude " << knownLat.at(j) << endl;
      }
      if (abs(iFOVPointCloud.at(j).y() - knownLon.at(j)) < 1E-8) {
        cout << "Vertex " << j+1 << " Longitude OK" << endl;
      }
      else {
        cout << "Vertex " << j+1 << " Longitude " << iFOVPointCloud.at(j).y() << ", expected longitude " << knownLon.at(j) << endl;
      }
    }
    cout << endl;

    cout << "Full FOV functionality is tested by pixel2map's app test." << endl;
  }
  catch(IException &e) {
    e.print();
  }
}

