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

#include <string>

#include "ShapeModelFactory.h"
#include "Cube.h"
#include "DemShape.h"
#include "EllipsoidShape.h"
#include "EquatorialCylindricalShape.h"
#include "IException.h"
#include "iString.h"
#include "FileName.h"
#include "Projection.h"
#include "PvlGroup.h"

namespace Isis {

  ShapeModel *ShapeModelFactory::Create(Target *target, Pvl &pvl) {
    
    // get kernels and instrument Pvl groups
    PvlGroup &kernelsPvlGroup = pvl.FindGroup("Kernels", Pvl::Traverse);
    // PvlGroup &instrumentPvlGroup = pvl.FindGroup("Instrument", Pvl::Traverse);
    
    // get target name from instrument Pvl group
    // iString targetName = instrumentPvlGroup["TargetName"][0];
    std::cout << "Target name = " << target->name() << std::endl;


    // handle "sky" target
    // TODO Deal with sky images.  Stuart suggested dealing with Sky images in a Target class.
    bool skyTarget = false; // TODO Do we need a sky shape model, member variable, or neither?
    // if (targetName.UpCase() == "SKY") {
    if (target->name().UpCase() == "SKY") {
      skyTarget = true;

      // right now, I'm just gonna quit if this is a sky thing
      iString msg =
          "In ShapeModelFactory::Create(Pvl &pvl), SKY target encountered, " \
          "not gonna mess with it for now.";
          throw IException(IException::Unknown, msg, _FILEINFO_);
    }

    // Determine if target is a plane??? target name has rings in it? 
    // Another keyword in label to indicate plane? What about lander/rovers?
    // bool planeTarget = false;
    
    // shape model file name
    std::string shapeModelFilenames = "";

    // TODO: We differentiate between "Elevation" and "Shape" models on the
    // labels, but we assign either one to the shapeModelFilename. Do we
    // need a shapeModelFilename AND an elevationModelFilename?
    // is this historical? Interchangeable?
    if (kernelsPvlGroup.HasKeyword("ElevationModel") &&
        !kernelsPvlGroup["ElevationModel"].IsNull() &&
        !skyTarget) {
      shapeModelFilenames = (std::string) kernelsPvlGroup["ElevationModel"];
    }
    else if(kernelsPvlGroup.HasKeyword("ShapeModel") &&
            !kernelsPvlGroup["ShapeModel"].IsNull() &&
            !skyTarget) {
      shapeModelFilenames = (std::string) kernelsPvlGroup["ShapeModel"];
    }

    // Create shape model
    ShapeModel *shapeModel = NULL;

    // TODO: If there is no shape model filename, the shape model type defaults to an
    // ellipsoid (should it?).
    if (shapeModelFilenames == "") {
      shapeModel = new EllipsoidShape(target);
    }
    // else if (shapeModelFilenames == "RingPlane") {
    //  shapeModel = new PlaneShape(target, pvl);
    // }
    else {
      // Is the shape model an Isis DEM?
      // TODO Deal with stacks -- this could be a list of DEMs
      try {
        // first, try to open the shape model file as an Isis3 cube
        Isis::Cube shapeModelCube;
        shapeModelCube.open(shapeModelFilenames, "r" );
        
        //Pvl *label = tmpCube.getLabel();
        //PvlGroup &mappingPvlGroup = label->FindGroup("Mapping", Isis::Pvl::Traverse);
        // std::string proj = mapGroup["ProjectionName"];
        
        // get projection of shape model cube
        Projection *projection = shapeModelCube.getProjection();
        if (projection == NULL) {
          iString msg = "Shape model cube must be a DEM file, meaning it must" \
                        "be map projected. This cube is NOT map projected.";
          throw IException(IException::User, msg, _FILEINFO_);
        }

        // Next, check if ISIS DEM cube is an equatorial cylindrical projection
        if (projection->IsEquatorialCylindrical())
          shapeModel = new EquatorialCylindricalShape(target, pvl);
        else 
          shapeModel = new DemShape(target, pvl);
      }
      catch (IException &e) {
        iString msg = "Shape file" + shapeModelFilenames + " is not supported";
        throw IException(e, IException::Unknown, msg, _FILEINFO_);
      }
    }

    // TODO Add Naif DSK shape and stack?

    if (shapeModel == NULL) {
      iString msg = "Unsupported shape model type";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    return shapeModel;
  }
} // end namespace isis
