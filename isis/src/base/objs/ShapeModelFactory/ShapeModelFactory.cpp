/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2008/06/19 23:35:38 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "ShapeModelFactory.h"

#include <string>

#include "Cube.h"
#include "DemShape.h"
#include "EllipsoidShape.h"
#include "EquatorialCylindricalShape.h"
#include "FileName.h"
#include "IException.h"
#include "IString.h"
#include "PlaneShape.h"
#include "Projection.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "Target.h"

using namespace std;

namespace Isis {

  ShapeModel *ShapeModelFactory::create(Target *target, Pvl &pvl) {
    
    // get kernels and instrument Pvl groups
    PvlGroup &kernelsPvlGroup = pvl.findGroup("Kernels", Pvl::Traverse);
    // Do we need a sky shape model, member variable, or neither? For now treat sky as ellipsoid
    bool skyTarget = target->isSky(); 

    // Determine if target is a plane??? target name has rings in it? 
    // Another keyword in label to indicate plane? What about lander/rovers?
    // bool planeTarget = false;
    
    // shape model file name
    QString shapeModelFilenames = "";

    // TODO: We differentiate between "Elevation" and "Shape" models on the
    // labels, but we assign either one to the shapeModelFilename. Do we
    // need a shapeModelFilename AND an elevationModelFilename?
    // is this historical? Interchangeable?
    if (skyTarget) {
      // Sky targets are ellipsoid shapes
    }
    else if (kernelsPvlGroup.hasKeyword("ElevationModel") &&
             !kernelsPvlGroup["ElevationModel"].isNull())  {
      shapeModelFilenames = (QString) kernelsPvlGroup["ElevationModel"];
    }
    else if (kernelsPvlGroup.hasKeyword("ShapeModel") &&
             !kernelsPvlGroup["ShapeModel"].isNull()) {
      shapeModelFilenames = (QString) kernelsPvlGroup["ShapeModel"];
    }

    // Create shape model
    ShapeModel *shapeModel = NULL;

    // TODO: If there is no shape model filename, the shape model type defaults to an
    // ellipsoid (should it?).
    if (shapeModelFilenames == "") {
      shapeModel = new EllipsoidShape(target);
    }
    else if (shapeModelFilenames == "RingPlane") {
      shapeModel = new PlaneShape(target, pvl);
    }
    else {
      try {
        // Is the shape model an Isis DEM?
        // TODO Deal with stacks -- this could be a list of DEMs
        Isis::Cube shapeModelCube;
        try {
          // first, try to open the shape model file as an Isis3 cube
          shapeModelCube.open(FileName(shapeModelFilenames).expanded(), "r" );
        }
        catch (IException &e) {
          IString msg = "Shape file " + shapeModelFilenames + " does not exist or is not an Isis cube";
          throw IException(e, IException::Unknown, msg, _FILEINFO_);
        }
        
        try {
          // get projection of shape model cube
          Projection *projection = shapeModelCube.projection();

          // Next, check if ISIS DEM cube is an equatorial cylindrical projection
          if (projection->IsEquatorialCylindrical())
            shapeModel = new EquatorialCylindricalShape(target, pvl);
          else 
            shapeModel = new DemShape(target, pvl);
        }
        catch (IException &e) {
          QString msg = "Shape model cube must be an Isis DEM file, meaning it must " \
                        "be map-projected. This cube is NOT map projected.";
          throw IException(IException::User, msg, _FILEINFO_);
        }
      }
      catch (IException &e) {
        IString msg = "Failed opening shape file " + shapeModelFilenames;
        throw IException(e, IException::Unknown, msg, _FILEINFO_);
      }
    }

    // TODO Add Naif DSK shape and stack?

    if (shapeModel == NULL) {
      IString msg = "Unsupported shape model type";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return shapeModel;
  }
} // end namespace isis
