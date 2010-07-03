#include "UniversalGroundMap.h"
#include "Projection.h"
#include "Camera.h"
#include "CameraFactory.h"
#include "ProjectionFactory.h"

namespace Isis {
 /**
  * Constructs a UniversalGroundMap object from a pvl
  * 
  * @param pvl The Pvl file to create the UniversalGroundMap from
  */
  UniversalGroundMap::UniversalGroundMap(Pvl &pvl) {
    Init(pvl);
  }

  /**
   * Constructs a UniversalGroundMap object from a cube
   * 
   * @param cube The Cube to create the UniversalGroundMap from
   */
  UniversalGroundMap::UniversalGroundMap(Cube &cube) {
    Init(*(cube.Label()));
  }

 /**
  * Creates the UniversalGroundMap
  * 
  * @param pvl The Pvl file to create the UniversalGroundMap from
  * 
  * @throws Isis::iException::Camera - Could not create camera or projection
  */
  void UniversalGroundMap::Init(Pvl &pvl) {
    p_camera = NULL;
    p_projection = NULL;

    try {
      p_camera = CameraFactory::Create(pvl);
    }
    catch (iException &e) {
      e.Clear();
      p_camera = NULL;
      try {
        p_projection = Isis::ProjectionFactory::CreateFromCube(pvl);
      }
      catch (iException &e) {
        p_projection = NULL;
        std::string msg = "Could not create camera or projection for [" +
          pvl.Filename() + "]";
        throw iException::Message(iException::Camera, msg, _FILEINFO_);
      }
    }
  }

 /**
  * Set the image band number
  * 
  * @param[in] band   (int)  Image band number
  * 
  */
  void UniversalGroundMap::SetBand(const int band) {
    if (p_camera != NULL) p_camera->SetBand(band);
  }



  //! Destroys the UniversalGroundMap object
  UniversalGroundMap::~UniversalGroundMap() {
    if (p_camera != NULL) delete p_camera;
    if (p_projection != NULL) delete p_projection;
  }

 /**
  * Returns whether the lat/lon position was set successfully in the camera 
  * model or projection 
  *  
  * @param lat The universal latitude 
  * @param lon The universal longitude 
  * 
  * @return Returns true if the lat/lon position was set successfully, and false
  *         if it was not
  */
  bool UniversalGroundMap::SetUniversalGround (double lat, double lon) {
    if (p_camera != NULL) {
      if(p_camera->SetUniversalGround(lat,lon)){
        return p_camera->InCube();
      } else {
        return false;
      }
    }
    else {
      return p_projection->SetUniversalGround(lat,lon);
    }
  }

 /**
  * Returns the current line value of the camera model or projection
  * 
  * @return Sample value
  */
  double UniversalGroundMap::Sample () const {
    if (p_camera != NULL) {
      return p_camera->Sample();
    }
    else {
      return p_projection->WorldX();
    }
  }

 /**
  * Returns the current line value of the camera model or projection
  * 
  * @return Line value 
  */
  double UniversalGroundMap::Line() const {
    if (p_camera != NULL) {
      return p_camera->Line();
    }
    else {
      return p_projection->WorldY();
    }
  }

 /**
  * Returns whether the sample/line postion was set successfully in the camera 
  * model or projection
  *  
  * @param sample The sample position 
  * @param line The line position 
  *  
  * @return Returns true if the sample/line position was set successfully, and 
  *         false if it was not
  */
  bool UniversalGroundMap::SetImage (double sample, double line) {
    if (p_camera != NULL) {
      return p_camera->SetImage(sample,line);
    }
    else {
      return p_projection->SetWorld(sample,line);
    }
  }

 /**
  * Returns the universal latitude of the camera model or projection
  * 
  * @return Universal Latitude
  */
  double UniversalGroundMap::UniversalLatitude () const {
    if (p_camera != NULL) {
      return p_camera->UniversalLatitude();
    }
    else {
      return p_projection->UniversalLatitude();
    }
  }

 /**
  * Returns the universal longitude of the camera model or projection
  * 
  * @return Universal Longitude
  */
  double UniversalGroundMap::UniversalLongitude() const {
    if (p_camera != NULL) {
      return p_camera->UniversalLongitude();
    }
    else {
      return p_projection->UniversalLongitude();
    }
  }

 /**
  * Returns the resolution of the camera model or projection
  * 
  * @return Resolution
  */
  double UniversalGroundMap::Resolution() const {
    if (p_camera != NULL) {
      return p_camera->PixelResolution();
    }
    else {
      return p_projection->Resolution();
    }
  }
}
