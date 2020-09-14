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

#include "Cube.h"
#include "FileName.h"
#include "IException.h"
#include "LineScanCamera.h"
#include "Preference.h"
#include "Pvl.h"

using namespace std;
using namespace Isis;

class MyCamera : public LineScanCamera {
  public:
    MyCamera(Cube &cube) : LineScanCamera(cube) { }

    virtual int CkFrameId() const {
      string msg = "CK Frame ID is unqiue to mission-specific cameras";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    virtual int CkReferenceId() const {
      string msg = "CK Reference ID is unique to mission-specific cameras";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    virtual int SpkReferenceId() const {
      string msg = "SPK Reference ID is unique to mission-specific cameras";
      throw IException(IException::Unknown, msg, _FILEINFO_);
    }
    
    // These are pure virtual within Sensor that must be overriden
    virtual QString instrumentNameLong() const { return QString("Line Scan"); }
    virtual QString instrumentNameShort() const { return QString("LS"); }
    virtual QString spacecraftNameLong() const { return QString("Line Scan 1"); }
    virtual QString spacecraftNameShort() const { return QString("LS1"); }
};

int main() {
  Preference::Preferences(true);
  Cube cube("$ISISTESTDATA/isis/src/mgs/unitTestData/ab102401.lev2.cub", "r");
  MyCamera cam(cube);

  cout << "Camera = Framing?   " << (cam.GetCameraType() == Camera::Framing) << endl;
  cout << "Camera = LineScan?  " << (cam.GetCameraType() == Camera::LineScan) << endl;
  cout << "Camera = PushFrame? " << (cam.GetCameraType() == Camera::PushFrame) << endl;
  cout << "Camera = Radar?     " << (cam.GetCameraType() == Camera::Radar) << endl;
}
