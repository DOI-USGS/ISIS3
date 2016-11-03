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
    char file[1024] = "$dawn/testData/FC21B0001010_09049002212F5D.cub";
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

