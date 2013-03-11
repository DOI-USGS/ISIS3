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

#include "Application.h"
#include "IException.h"
#include "ProcessByLine.h"
#include "Preference.h"
#include "Projection.h"
#include "ProjectionFactory.h"
#include "Pvl.h"
#include "SpecialPixel.h"
#include "UniqueIOCachingAlgorithm.h"

using namespace std;

namespace Isis {
  //! Constructs a Mosaic object
  ProcessMapMosaic::ProcessMapMosaic() {
    p_createMosaic = true;
  }

  //! Destructor
  ProcessMapMosaic::~ProcessMapMosaic() { }

 /**
  * Input cube cannot be set here
  */
  Isis::Cube *ProcessMapMosaic::SetInputCube() {
    throw IException(IException::Programmer,
                     "ProcessMapMosaic does not support the SetInputCube method",
                     _FILEINFO_);
  }

  /**
   * Mosaic Processing method, returns false if the cube is not inside the mosaic
   */
  bool ProcessMapMosaic::StartProcess(QString inputFile) {
    if (InputCubes.size() != 0) {
      QString msg = "Input cubes already exist; do not call SetInputCube when using ";
      msg += "ProcessMosaic::StartProcess(QString)";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (OutputCubes.size() == 0) {
      QString msg = "An output cube must be set before calling StartProcess";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    CubeAttributeInput inAtt(inputFile);
    Cube *inCube = ProcessMosaic::SetInputCube(inputFile, inAtt);

    Cube *mosaicCube = OutputCubes[0];
    Projection *iproj = inCube->projection();
    Projection *oproj = mosaicCube->projection();
    int nsMosaic = mosaicCube->sampleCount();
    int nlMosaic = mosaicCube->lineCount();

    if (*iproj != *oproj) {
      QString msg = "Mapping groups do not match between cube [" + inputFile + "] and mosaic";
      throw IException(IException::User, msg, _FILEINFO_);
    }

    int outSample, outSampleEnd, outLine, outLineEnd;
    outSample = (int)(oproj->ToWorldX(iproj->ToProjectionX(1.0)) + 0.5);
    outLine   = (int)(oproj->ToWorldY(iproj->ToProjectionY(1.0)) + 0.5);

    int ins = InputCubes[0]->sampleCount();
    int inl =  InputCubes[0]->lineCount();
    outSampleEnd = outSample + ins;
    outLineEnd   = outLine + inl;

    bool wrapPossible = iproj->IsEquatorialCylindrical();
    int worldSize = 0;
    if (wrapPossible) {
      // Figure out how many samples 360 degrees is
      wrapPossible = wrapPossible && oproj->SetUniversalGround(0, 0);
      int worldStart = (int)(oproj->WorldX() + 0.5);
      wrapPossible = wrapPossible && oproj->SetUniversalGround(0, 180);
      int worldEnd = (int)(oproj->WorldX() + 0.5);

      worldSize = abs(worldEnd - worldStart) * 2;

      wrapPossible = wrapPossible && (worldSize > 0);

      // This is EquatorialCylindrical, so shift to the left all the way
      if (wrapPossible) {
        // While some data would still be put in the mosaic, move left
        //  >1 for end because 0 still means no data, whereas 1 means 1 line of data
        while (outSampleEnd - worldSize > 1) {
          outSample -= worldSize;
          outSampleEnd -= worldSize;
        }
        // Now we have the sample range to the furthest left
      }
    }

    // Check overlaps of input image along the mosaic edges before
    // calling ProcessMosaic::StartProcess
    // Left edge
    if (outSample < 1) {
      ins = ins + outSample - 1;
    }

    // Top edge
    if (outLine < 1) {
      inl = inl + outLine - 1;
    }

    // Right edge
    if ((outSample + ins - 1) > nsMosaic) {
      ins = nsMosaic - outSample + 1;
    }

    // Bottom edge
    if ((outLine + inl - 1) > nlMosaic) {
      inl = nlMosaic - outLine + 1;
    }

    if (outSampleEnd < 1 || outLineEnd < 1 || outSample > nsMosaic || outLine > nlMosaic || ins < 1 || inl < 1) {
      // Add a PvlKeyword naming which files are not included in output mosaic
      ClearInputCubes();
      return false;
    }
    else {
      // Place the input in the mosaic
      Progress()->SetText("Mosaicking " + FileName(inputFile).name());

      try {
        do {
          int outBand = 1;
          ProcessMosaic::StartProcess(outSample, outLine, outBand);

          // Increment for projections where occurrances may happen multiple times
          outSample += worldSize;
          outSampleEnd += worldSize;
        }
        while (wrapPossible && outSample < nsMosaic);
      }
      catch (IException &e) {
        QString msg = "Unable to mosaic cube [" + FileName(inputFile).name() + "]";
        throw IException(e, IException::User, msg, _FILEINFO_);
      }
    }

    WriteHistory(*mosaicCube);

    // Don't propagate any more histories now that we've done one
    p_propagateHistory = false;

    ClearInputCubes();

    return true;
  }

  //*************************************************************************************************
  /**
   * Set the output cube to specified file name and specified input images
   * and output attributes
   */
  Isis::Cube *ProcessMapMosaic::SetOutputCube(FileList &propagationCubes, CubeAttributeOutput &oAtt,
      const QString &mosaicFile) {
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

    if (propagationCubes.size() < 1) {
      QString msg = "The list does not contain any data";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    for (int i = 0; i < propagationCubes.size(); i++) {
      // Open the cube and get the maximum number of band in all cubes
      Cube cube;
      cube.open(propagationCubes[i].toString());
      bands = max(bands, cube.bandCount());

      // See if the cube has a projection and make sure it matches
      // previous input cubes
      Projection *projNew =
          Isis::ProjectionFactory::CreateFromCube(*(cube.label()));
      if ((proj != NULL) && (*proj != *projNew)) {
        QString msg = "Mapping groups do not match between cubes [" +
                     propagationCubes[0].toString() + "] and [" + propagationCubes[i].toString() + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      // Figure out the x/y range as it may be needed later
      double x = projNew->ToProjectionX(0.5);
      double y = projNew->ToProjectionY(0.5);
      if (x < xmin) xmin = x;
      if (y < ymin) ymin = y;
      if (x > xmax) xmax = x;
      if (y > ymax) ymax = y;

      x = projNew->ToProjectionX(cube.sampleCount() + 0.5);
      y = projNew->ToProjectionY(cube.lineCount() + 0.5);
      if (x < xmin) xmin = x;
      if (y < ymin) ymin = y;
      if (x > xmax) xmax = x;
      if (y > ymax) ymax = y;

      slat = min(slat, projNew->MinimumLatitude());
      elat = max(elat, projNew->MaximumLatitude());
      slon = min(slon, projNew->MinimumLongitude());
      elon = max(elon, projNew->MaximumLongitude());

      // Cleanup
      cube.close();
      if (proj) delete proj;
      proj = projNew;
    }

    if (proj) delete proj;

    return SetOutputCube(propagationCubes[0].toString(), xmin, xmax, ymin, ymax,
                         slat, elat, slon, elon, bands, oAtt, mosaicFile);
  }

  //*************************************************************************************************
  /**
   * Set the output cube to specified file name and specified input images
   * and output attributes and lat,lons
   */
  Isis::Cube *ProcessMapMosaic::SetOutputCube(FileList &propagationCubes,
      double slat, double elat, double slon, double elon,
      CubeAttributeOutput &oAtt, const QString &mosaicFile) {
    if (propagationCubes.size() < 1) {
      QString msg = "The list does not contain any data";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    int samples, lines, bands = 0;
    Pvl label;
    label.read(propagationCubes[0].toString());
    PvlGroup mGroup = label.findGroup("Mapping", Pvl::Traverse);
    mGroup.addKeyword(PvlKeyword("MinimumLatitude", toString(slat)), Pvl::Replace);
    mGroup.addKeyword(PvlKeyword("MaximumLatitude", toString(elat)), Pvl::Replace);
    mGroup.addKeyword(PvlKeyword("MinimumLongitude", toString(slon)), Pvl::Replace);
    mGroup.addKeyword(PvlKeyword("MaximumLongitude", toString(elon)), Pvl::Replace);

    if (mGroup.hasKeyword("UpperLeftCornerX"))
      mGroup.deleteKeyword("UpperLeftCornerX");

    if (mGroup.hasKeyword("UpperLeftCornerY"))
      mGroup.deleteKeyword("UpperLeftCornerY");

    Pvl mapPvl;
    mapPvl += mGroup;

    // Use CreateForCube because our range differs from any of the cubes (manually specified)
    Projection *proj = Isis::ProjectionFactory::CreateForCube(mapPvl, samples, lines, false);

    double xmin, xmax, ymin, ymax;
    proj->XYRange(xmin, xmax, ymin, ymax);

    // The xmin/ymax should be rounded for the labels
    xmin = mapPvl.findGroup("Mapping")["UpperLeftCornerX"];
    ymax = mapPvl.findGroup("Mapping")["UpperLeftCornerY"];

    for (int i = 0; i < propagationCubes.size(); i++) {
      Cube cube;
      cube.open(propagationCubes[i].toString());
      bands = max(cube.bandCount(), bands);

      // See if the cube has a projection and make sure it matches
      // previous input cubes
      Projection *projNew =
          Isis::ProjectionFactory::CreateFromCube(*(cube.label()));

      if (proj == NULL) {
      }
      else if (*proj != *projNew) {
        QString msg = "Mapping groups do not match between cube [" + propagationCubes[i].toString() +
                     "] and [" + propagationCubes[0].toString() + "]";
        throw IException(IException::User, msg, _FILEINFO_);
      }

      if (proj) delete proj;
      proj = projNew;
    }

    if (proj) delete proj;

    return SetOutputCube(propagationCubes[0].toString(), xmin, xmax, ymin, ymax,
                         slat, elat, slon, elon, bands, oAtt, mosaicFile);
  }

  //*************************************************************************************************

  /**
   * Set the output cube to specified file name and specified input images
   * and output attributes and lat,lons
   */
  Isis::Cube *ProcessMapMosaic::SetOutputCube(const QString &inputFile,
      double xmin, double xmax, double ymin, double ymax,
      double slat, double elat, double slon, double elon, int nbands,
      CubeAttributeOutput &oAtt, const QString &mosaicFile) {
    Pvl fileLab(inputFile);
    PvlGroup &mapping = fileLab.findGroup("Mapping", Pvl::Traverse);

    mapping["UpperLeftCornerX"] = toString(xmin);
    mapping["UpperLeftCornerY"] = toString(ymax);
    mapping.addKeyword(PvlKeyword("MinimumLatitude", toString(slat)), Pvl::Replace);
    mapping.addKeyword(PvlKeyword("MaximumLatitude", toString(elat)), Pvl::Replace);
    mapping.addKeyword(PvlKeyword("MinimumLongitude", toString(slon)), Pvl::Replace);
    mapping.addKeyword(PvlKeyword("MaximumLongitude", toString(elon)), Pvl::Replace);

    Projection *firstProj = ProjectionFactory::CreateFromCube(fileLab);
    int samps = (int)(ceil(firstProj->ToWorldX(xmax) - firstProj->ToWorldX(xmin)) + 0.5);
    int lines = (int)(ceil(firstProj->ToWorldY(ymin) - firstProj->ToWorldY(ymax)) + 0.5);
    delete firstProj;

    if (p_createMosaic) {
      Pvl newMap;
      newMap.addGroup(mapping);

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
      if (GetTrackFlag()) {
        nbands += 1;
      }
      // For average priority, get the new band count
      else if (GetImageOverlay() == AverageImageWithMosaic) {
        nbands *= 2;
      }

      Cube *ocube = p.SetOutputCube(mosaicFile, oAtt, samps, lines, nbands);
      p.Progress()->SetText("Initializing mosaic");
      p.ClearInputCubes();
      p.StartProcess(ProcessMapMosaic::FillNull);

      // CreateForCube created some keywords in the mapping group that needs to be added
      ocube->putGroup(newMap.findGroup("Mapping", Pvl::Traverse));
      p.EndProcess();
    }

    Cube *mosaicCube = new Cube();
    mosaicCube->open(mosaicFile, "rw");
    mosaicCube->addCachingAlgorithm(new UniqueIOCachingAlgorithm(2));

    AddOutputCube(mosaicCube);
    return mosaicCube;
  }

  //*************************************************************************************************

  /**
   * Set the output cube to specified file name and specified input images
   * and output attributes and lat,lons
   */
  Isis::Cube *ProcessMapMosaic::SetOutputCube(const QString &inputFile, PvlGroup mapping,
      CubeAttributeOutput &oAtt, const QString &mosaicFile) {
    if (OutputCubes.size() != 0) {
      QString msg = "You can only specify one output cube and projection";
      throw IException(IException::Programmer, msg, _FILEINFO_);
    }

    if (mapping.hasKeyword("UpperLeftCornerX"))
      mapping.deleteKeyword("UpperLeftCornerX");

    if (mapping.hasKeyword("UpperLeftCornerY"))
      mapping.deleteKeyword("UpperLeftCornerY");

    if (p_createMosaic) {
      Pvl newMap;
      newMap.addGroup(mapping);
      int samps, lines, bands;
      delete ProjectionFactory::CreateForCube(newMap, samps, lines, false);

      // Initialize the mosaic
      ProcessByLine p;
      CubeAttributeInput inAtt(inputFile);
      Cube *propCube = p.SetInputCube(inputFile, inAtt);
      bands = propCube->bandCount();

      // If track set, create the origin band
      if (GetTrackFlag()) {
        bands += 1;
      }
      // For average priority, get the new band count
      else if (GetImageOverlay() == AverageImageWithMosaic) {
        bands *= 2;
      }

      p.PropagateHistory(false);
      p.PropagateLabels(false);
      Cube *ocube = p.SetOutputCube(mosaicFile, oAtt, samps, lines, bands);
      p.Progress()->SetText("Initializing mosaic");
      p.ClearInputCubes();

      p.StartProcess(ProcessMapMosaic::FillNull);

      // CreateForCube created some keywords in the mapping group that needs to be added
      ocube->putGroup(newMap.findGroup("Mapping", Pvl::Traverse));
      p.EndProcess();
    }

    Cube *mosaicCube = new Cube();
    AddOutputCube(mosaicCube);
    mosaicCube->open(mosaicFile, "rw");
    mosaicCube->addCachingAlgorithm(new UniqueIOCachingAlgorithm(2));

    return mosaicCube;
  }

  //*************************************************************************************************

  /**
   * Mosaic output method for Mosaic Processing Method, this will use an existing mosaic
   */
  Cube *ProcessMapMosaic::SetOutputCube(const QString &mosaicFile) {
    p_createMosaic = false;
    Cube mosaic;
    mosaic.open(mosaicFile);

    PvlGroup &mapping = mosaic.label()->findGroup("Mapping", Pvl::Traverse);
    CubeAttributeOutput oAtt;
    // The other SetOutput will not use the attribute or filename
    Cube *ocube = SetOutputCube("", mapping, oAtt, mosaicFile);
    p_createMosaic = true;

    return ocube;
  }

  //*************************************************************************************************
  /**
   * Reset the buffer with NULL pixels
   */
  void ProcessMapMosaic::FillNull(Buffer &data) {
    for (int i = 0; i < data.size(); i++) data[i] = Isis::Null;
  }
} // end namespace isis
