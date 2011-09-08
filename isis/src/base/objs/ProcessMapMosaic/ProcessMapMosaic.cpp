/**
 * @file
 * $Revision: 1.11 $
 * $Date: 2010/06/21 18:38:30 $
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
#include "ProcessMapMosaic.h"

#include <QTime>

#include "SpecialPixel.h"
#include "iException.h"
#include "Application.h"
#include "ProcessByLine.h"
#include "Pvl.h"
#include "Preference.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "UniqueIOCachingAlgorithm.h"

using namespace std;

namespace Isis {
  ProcessMapMosaic::ProcessMapMosaic() {
    p_createMosaic = true;
  }

  bool ProcessMapMosaic::StartProcess(std::string inputFile) {
    if(InputCubes.size() != 0) {
      std::string msg = "Input cubes already exist; do not call SetInputCube when using ";
      msg += "ProcessMosaic::StartProcess(std::string)";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if(OutputCubes.size() == 0) {
      std::string msg = "An output cube must be set before calling StartProcess";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    CubeAttributeInput inAtt(inputFile);
    Cube *inCube = ProcessMosaic::SetInputCube(inputFile, inAtt);

    Cube *mosaicCube = OutputCubes[0];
    Projection *iproj = inCube->getProjection();
    Projection *oproj = mosaicCube->getProjection();
    int nsMosaic = mosaicCube->getSampleCount();
    int nlMosaic = mosaicCube->getLineCount();

    if(*iproj != *oproj) {
      string msg = "Mapping groups do not match between cube [" + inputFile + "] and mosaic";
      throw iException::Message(iException::User, msg, _FILEINFO_);
    }

    int outSample, outSampleEnd, outLine, outLineEnd;
    outSample = (int)(oproj->ToWorldX(iproj->ToProjectionX(1.0)) + 0.5);
    outLine   = (int)(oproj->ToWorldY(iproj->ToProjectionY(1.0)) + 0.5);

    int ins = InputCubes[0]->getSampleCount();
    int inl =  InputCubes[0]->getLineCount();
    outSampleEnd = outSample + ins;
    outLineEnd   = outLine + inl;

    bool wrapPossible = iproj->IsEquatorialCylindrical();
    int worldSize = 0;
    if(wrapPossible) {
      // Figure out how many samples 360 degrees is
      wrapPossible = wrapPossible && oproj->SetUniversalGround(0, 0);
      int worldStart = (int)(oproj->WorldX() + 0.5);
      wrapPossible = wrapPossible && oproj->SetUniversalGround(0, 180);
      int worldEnd = (int)(oproj->WorldX() + 0.5);

      worldSize = abs(worldEnd - worldStart) * 2;

      wrapPossible = wrapPossible && (worldSize > 0);

      // This is EquatorialCylindrical, so shift to the left all the way
      if(wrapPossible) {
        // While some data would still be put in the mosaic, move left
        //  >1 for end because 0 still means no data, whereas 1 means 1 line of data
        while(outSampleEnd - worldSize > 1) {
          outSample -= worldSize;
          outSampleEnd -= worldSize;
        }
        // Now we have the sample range to the furthest left
      }
    }

    // Check overlaps of input image along the mosaic edges before 
    // calling ProcessMosaic::StartProcess
    // Left edge
    if(outSample < 1) {
      ins = ins + outSample - 1;
    }
    
    // Top edge
    if(outLine < 1) {
      inl = inl + outLine - 1;
    }
    
    // Right edge
    if((outSample + ins - 1) > nsMosaic) {
      ins = nsMosaic - outSample + 1;
    }
    
    // Bottom edge
    if((outLine + inl - 1) > nlMosaic) {
      inl = nlMosaic - outLine + 1;
    }

    if(outSampleEnd < 1 || outLineEnd < 1 || outSample > nsMosaic || outLine > nlMosaic || ins < 1 || inl < 1) {
      // Add a PvlKeyword naming which files are not included in output mosaic
      ClearInputCubes();
      return false;
    }
    else {
      // Place the input in the mosaic
      Progress()->SetText("Mosaicking " + Filename(inputFile).Name());

      try {
        do {
          int outBand = 1;
          ProcessMosaic::StartProcess(outSample, outLine, outBand);

          // Increment for projections where occurrances may happen multiple times
          outSample += worldSize;
          outSampleEnd += worldSize;
        }
        while(wrapPossible && outSample < nsMosaic);
      }
      catch(iException &e) {
        string msg = "Unable to mosaic cube [" + Filename(inputFile).Name() + "]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }
    }

    WriteHistory(*mosaicCube);

    // Don't propagate any more histories now that we've done one
    p_propagateHistory = false;

    ClearInputCubes();

    return true;
  }

  //*************************************************************************************************

  Isis::Cube *ProcessMapMosaic::SetOutputCube(FileList &propagationCubes, CubeAttributeOutput &oAtt,
      const std::string &mosaicFile) {
    int bands = 0;
    double xmin = DBL_MAX;
    double xmax = -DBL_MAX;
    double ymin = DBL_MAX;
    double ymax = -DBL_MAX;
    double slat = DBL_MAX;
    double elat = -DBL_MAX;
    double slon = DBL_MAX;
    double elon = -DBL_MAX;

    Projection *proj = NULL;

    if(propagationCubes.size() < 1) {
      string msg = "The list does not contain any data";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    for(unsigned int i = 0; i < propagationCubes.size(); i++) {
      // Open the cube and get the maximum number of band in all cubes
      Cube cube;
      cube.open(propagationCubes[i]);
      bands = max(bands, cube.getBandCount());

      // See if the cube has a projection and make sure it matches
      // previous input cubes
      Projection *projNew =
          Isis::ProjectionFactory::CreateFromCube(*(cube.getLabel()));
      if((proj != NULL) && (*proj != *projNew)) {
        string msg = "Mapping groups do not match between cubes [" +
                     propagationCubes[0] + "] and [" + propagationCubes[i] + "]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }

      // Figure out the x/y range as it may be needed later
      double x = projNew->ToProjectionX(0.5);
      double y = projNew->ToProjectionY(0.5);
      if(x < xmin) xmin = x;
      if(y < ymin) ymin = y;
      if(x > xmax) xmax = x;
      if(y > ymax) ymax = y;

      x = projNew->ToProjectionX(cube.getSampleCount() + 0.5);
      y = projNew->ToProjectionY(cube.getLineCount() + 0.5);
      if(x < xmin) xmin = x;
      if(y < ymin) ymin = y;
      if(x > xmax) xmax = x;
      if(y > ymax) ymax = y;

      slat = min(slat, projNew->MinimumLatitude());
      elat = max(elat, projNew->MaximumLatitude());
      slon = min(slon, projNew->MinimumLongitude());
      elon = max(elon, projNew->MaximumLongitude());

      // Cleanup
      cube.close();
      if(proj) delete proj;
      proj = projNew;
    }

    if(proj) delete proj;

    return SetOutputCube(propagationCubes[0], xmin, xmax, ymin, ymax,
                         slat, elat, slon, elon, bands, oAtt, mosaicFile);
  }

  //*************************************************************************************************

  Isis::Cube *ProcessMapMosaic::SetOutputCube(FileList &propagationCubes,
      double slat, double elat, double slon, double elon,
      CubeAttributeOutput &oAtt, const std::string &mosaicFile) {
    if(propagationCubes.size() < 1) {
      string msg = "The list does not contain any data";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    int samples, lines, bands = 0;
    Pvl label;
    label.Read(propagationCubes[0]);
    PvlGroup mGroup = label.FindGroup("Mapping", Pvl::Traverse);
    mGroup.AddKeyword(PvlKeyword("MinimumLatitude", slat), Pvl::Replace);
    mGroup.AddKeyword(PvlKeyword("MaximumLatitude", elat), Pvl::Replace);
    mGroup.AddKeyword(PvlKeyword("MinimumLongitude", slon), Pvl::Replace);
    mGroup.AddKeyword(PvlKeyword("MaximumLongitude", elon), Pvl::Replace);

    if(mGroup.HasKeyword("UpperLeftCornerX"))
      mGroup.DeleteKeyword("UpperLeftCornerX");

    if(mGroup.HasKeyword("UpperLeftCornerY"))
      mGroup.DeleteKeyword("UpperLeftCornerY");

    Pvl mapPvl;
    mapPvl += mGroup;

    // Use CreateForCube because our range differs from any of the cubes (manually specified)
    Projection *proj = Isis::ProjectionFactory::CreateForCube(mapPvl, samples, lines, false);

    double xmin, xmax, ymin, ymax;
    proj->XYRange(xmin, xmax, ymin, ymax);

    // The xmin/ymax should be rounded for the labels
    xmin = mapPvl.FindGroup("Mapping")["UpperLeftCornerX"];
    ymax = mapPvl.FindGroup("Mapping")["UpperLeftCornerY"];

    for(unsigned int i = 0; i < propagationCubes.size(); i++) {
      Cube cube;
      cube.open(propagationCubes[i]);
      bands = max(cube.getBandCount(), bands);

      // See if the cube has a projection and make sure it matches
      // previous input cubes
      Projection *projNew =
          Isis::ProjectionFactory::CreateFromCube(*(cube.getLabel()));

      if(proj == NULL) {
      }
      else if(*proj != *projNew) {
        string msg = "Mapping groups do not match between cube [" + propagationCubes[i] +
                     "] and [" + propagationCubes[0] + "]";
        throw iException::Message(iException::User, msg, _FILEINFO_);
      }

      if(proj) delete proj;
      proj = projNew;
    }

    if(proj) delete proj;

    return SetOutputCube(propagationCubes[0], xmin, xmax, ymin, ymax,
                         slat, elat, slon, elon, bands, oAtt, mosaicFile);
  }

  //*************************************************************************************************

  Isis::Cube *ProcessMapMosaic::SetOutputCube(const std::string &inputFile,
      double xmin, double xmax, double ymin, double ymax,
      double slat, double elat, double slon, double elon, int nbands,
      CubeAttributeOutput &oAtt, const std::string &mosaicFile) {
    Pvl fileLab(inputFile);
    PvlGroup &mapping = fileLab.FindGroup("Mapping", Pvl::Traverse);

    mapping["UpperLeftCornerX"] = xmin;
    mapping["UpperLeftCornerY"] = ymax;
    mapping.AddKeyword(PvlKeyword("MinimumLatitude", slat), Pvl::Replace);
    mapping.AddKeyword(PvlKeyword("MaximumLatitude", elat), Pvl::Replace);
    mapping.AddKeyword(PvlKeyword("MinimumLongitude", slon), Pvl::Replace);
    mapping.AddKeyword(PvlKeyword("MaximumLongitude", elon), Pvl::Replace);

    Projection *firstProj = ProjectionFactory::CreateFromCube(fileLab);
    int samps = (int)(ceil(firstProj->ToWorldX(xmax) - firstProj->ToWorldX(xmin)) + 0.5);
    int lines = (int)(ceil(firstProj->ToWorldY(ymin) - firstProj->ToWorldY(ymax)) + 0.5);
    delete firstProj;

    if(p_createMosaic) {
      Pvl newMap;
      newMap.AddGroup(mapping);

      // Initialize the mosaic
      CubeAttributeInput inAtt;

      ProcessByLine p;
      p.SetInputCube(inputFile, inAtt);
      p.PropagateHistory(false);
      p.PropagateLabels(false);
      p.PropagateTables(false);
      p.PropagatePolygons(false);
      p.PropagateOriginalLabel(false);

      // If track set, create the origin band
      if(GetTrackFlag()) {
        nbands += 1;
      }
      // For average priority, get the new band count
      else if(GetPriority() == average) {
        nbands *= 2;
      }

      Cube *ocube = p.SetOutputCube(mosaicFile, oAtt, samps, lines, nbands);
      p.Progress()->SetText("Initializing mosaic");
      p.ClearInputCubes();
      p.StartProcess(ProcessMapMosaic::FillNull);

      // CreateForCube created some keywords in the mapping group that needs to be added
      ocube->putGroup(newMap.FindGroup("Mapping", Pvl::Traverse));
      p.EndProcess();
    }

    Cube *mosaicCube = new Cube();
    mosaicCube->open(mosaicFile, "rw");
    mosaicCube->addCachingAlgorithm(new UniqueIOCachingAlgorithm(2));

    OutputCubes.push_back(mosaicCube);
    return mosaicCube;
  }

  //*************************************************************************************************

  Isis::Cube *ProcessMapMosaic::SetOutputCube(const std::string &inputFile, PvlGroup mapping,
      CubeAttributeOutput &oAtt, const std::string &mosaicFile) {
    if(OutputCubes.size() != 0) {
      std::string msg = "You can only specify one output cube and projection";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    if(mapping.HasKeyword("UpperLeftCornerX"))
      mapping.DeleteKeyword("UpperLeftCornerX");

    if(mapping.HasKeyword("UpperLeftCornerY"))
      mapping.DeleteKeyword("UpperLeftCornerY");

    if(p_createMosaic) {
      Pvl newMap;
      newMap.AddGroup(mapping);
      int samps, lines, bands;
      delete ProjectionFactory::CreateForCube(newMap, samps, lines, false);

      // Initialize the mosaic
      ProcessByLine p;
      CubeAttributeInput inAtt(inputFile);
      Cube *propCube = p.SetInputCube(inputFile, inAtt);
      bands = propCube->getBandCount();

      // If track set, create the origin band
      if(GetTrackFlag()) {
        bands += 1;
      }
      // For average priority, get the new band count
      else if(GetPriority() == average) {
        bands *= 2;
      }

      p.PropagateHistory(false);
      p.PropagateLabels(false);
      Cube *ocube = p.SetOutputCube(mosaicFile, oAtt, samps, lines, bands);
      p.Progress()->SetText("Initializing mosaic");
      p.ClearInputCubes();

      p.StartProcess(ProcessMapMosaic::FillNull);

      // CreateForCube created some keywords in the mapping group that needs to be added
      ocube->putGroup(newMap.FindGroup("Mapping", Pvl::Traverse));
      p.EndProcess();
    }

    Cube *mosaicCube = new Cube();
    OutputCubes.push_back(mosaicCube);
    mosaicCube->open(mosaicFile, "rw");
    mosaicCube->addCachingAlgorithm(new UniqueIOCachingAlgorithm(2));

    return mosaicCube;
  }

  //*************************************************************************************************

  Cube *ProcessMapMosaic::SetOutputCube(const std::string &mosaicFile) {
    p_createMosaic = false;
    Cube mosaic;
    mosaic.open(mosaicFile);

    PvlGroup &mapping = mosaic.getLabel()->FindGroup("Mapping", Pvl::Traverse);
    CubeAttributeOutput oAtt;
    // The other SetOutput will not use the attribute or filename
    Cube *ocube = SetOutputCube("", mapping, oAtt, mosaicFile);
    p_createMosaic = true;

    return ocube;
  }

  //*************************************************************************************************
  void ProcessMapMosaic::FillNull(Buffer &data) {
    for(int i = 0; i < data.size(); i++) data[i] = Isis::Null;
  }
} // end namespace isis
