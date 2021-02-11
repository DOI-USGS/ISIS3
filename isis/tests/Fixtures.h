#ifndef Fixtures_h
#define Fixtures_h

#include "gtest/gtest.h"

#include <string>

#include <QString>
#include <QTemporaryDir>
#include <QTemporaryFile>

#include <nlohmann/json.hpp>

#include "csm/csm.h"
#include "csm/Ellipsoid.h"

#include "Cube.h"
#include "IException.h"
#include "OriginalLabel.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "ImagePolygon.h"
#include "PolygonTools.h"
#include "Blob.h"
#include "MockCsmPlugin.h"
#include "Mocks.h"
#include "ControlNet.h"
#include "FileList.h"
#include "FileName.h"

#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/CoordinateSequence.h"
#include "geos/geom/LinearRing.h"
#include "geos/geom/Polygon.h"

using json = nlohmann::json;

namespace Isis {

  class TempTestingFiles : public ::testing::Test {
    protected:
      QTemporaryDir tempDir;

      void SetUp() override;
  };



  class SmallCube : public TempTestingFiles {
    protected:
      Cube *testCube;

      void SetUp() override;
      void TearDown() override;
  };


  class LargeCube : public TempTestingFiles {
    protected:
      Cube *testCube;

      void SetUp() override;
      void TearDown() override;
  };


  class SpecialSmallCube : public TempTestingFiles {
    protected:
      Cube *testCube;

      void SetUp() override;
      void TearDown() override;
  };


  class DefaultCube : public TempTestingFiles {
    protected:
      Cube *testCube;
      Cube *projTestCube;

      Pvl label;
      Pvl projLabel;
      json isd;

      void SetUp() override;
      void TearDown() override;
  };

  class LineScannerCube : public TempTestingFiles {
    protected:
      Cube *testCube;
      Cube *projTestCube;

      Pvl label;
      Pvl projLabel;
      json isd;

      void SetUp() override;
      void TearDown() override;
  };

  class ThreeImageNetwork : public TempTestingFiles {
    protected:

      ControlNet *network;
      QString networkFile;

      Cube *cube1;
      Cube *cube2;
      Cube *cube3;

      Cube *cube1map;
      Cube *cube2map;
      Cube *cube3map;

      FileName *isdPath1;
      FileName *isdPath2;
      FileName *isdPath3;

      FileName *threeImageOverlapFile;
      FileName *twoImageOverlapFile;

      FileList *cubeList;
      QString cubeListFile;
      QString twoCubeListFile;

      std::vector<std::vector<double>> coords;

      void SetUp() override;
      void AddFeatures();
      void TearDown() override;
  };

  class ObservationPair : public TempTestingFiles {
    protected:

      Cube *cubeL;
      Cube *cubeR;

      QString cubeLPath;
      QString cubeRPath;

      FileName *isdPathL;
      FileName *isdPathR;

      FileList *cubeList;
      QString cubeListFile;

      ControlNet *network;
      QString cnetPath;

      void SetUp() override;
      void TearDown() override;
  };

  class ApolloNetwork : public TempTestingFiles {
    protected:
      QVector<FileName> isdFiles;
      QVector<FileName> labelFiles;
      QVector<Cube*> cubes;

      FileList *cubeList;
      QString cubeListFile;

      ControlNet *network;
      QString controlNetPath;

      void SetUp() override;
      void TearDown() override;
  };

  class DemCube : public DefaultCube {
    protected:
      Cube *demCube;

      void SetUp() override;
      void TearDown() override;
  };

  class MroHiriseCube : public DefaultCube {
    protected:
      QString ckPath = "data/mroKernels/mroCK.bc";
      QString sclkPath = "data/mroKernels/mroSCLK.tsc";
      QString lskPath = "data/mroKernels/mroLSK.tls";
      Cube dejitteredCube;
      QString jitterPath;

      void SetUp() override;
      void setInstrument(QString ikid, QString instrumentId, QString spacecraftName);
  };

  class NewHorizonsCube : public DefaultCube {
    protected:
      void setInstrument(QString ikid, QString instrumentId, QString spacecraftName);
  };

  class ApolloCube : public LargeCube {
    protected:
      void SetUp() override;
  };

  class RingsCube : public TempTestingFiles {
    protected:

      // pixtures of Saturn's rings
      Cube *ring1;
      Cube *ring2;
      FileList cubeFileList;
      QString cubeListPath;

      void SetUp() override;
  };

  class CSMCubeFixture : public SmallCube {
  protected:
    QString filename;
    MockRasterGM mockModel;

    void SetUp() override;
};


class CSMCameraFixture : public CSMCubeFixture {
  protected:
    Camera *testCam;

    void SetUp() override;
};


class CSMCameraSetFixture : public CSMCameraFixture {
  protected:
    csm::Ellipsoid wgs84;
    csm::ImageCoord imagePt;
    csm::EcefCoord groundPt;
    csm::EcefLocus imageLocus;

    void SetUp() override;
};


class CSMCameraDemFixture : public CSMCubeFixture {
  protected:
    Camera *testCam;
    double demRadius;

    void SetUp() override;
};
}

#endif
