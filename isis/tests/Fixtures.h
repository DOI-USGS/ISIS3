#ifndef Fixtures_h
#define Fixtures_h

#include "gtest/gtest.h"

#include <memory>
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

  class SmallGapCube : public TempTestingFiles {
    protected:
      Cube *horzCube;
      Cube *vertCube;
      Cube *bandCube;

      void SetUp() override;
      void TearDown() override;
  };

  class PushFramePair : public TempTestingFiles {
    protected:
      std::shared_ptr<Cube> evenCube;
      std::shared_ptr<Cube> oddCube;
      int numSamps;
      int numBands;
      int frameHeight;
      int numFrames;

      void SetUp() override;
  };

  class FlippedPushFramePair : public TempTestingFiles {
    protected:
      std::shared_ptr<Cube> evenCube;
      std::shared_ptr<Cube> oddCube;
      int numSamps;
      int numBands;
      int frameHeight;
      int numFrames;

      void SetUp() override;
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
      void resizeCube(int samples, int lines, int bands);
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

  class OffBodyCube : public TempTestingFiles {
    protected:
      Cube *testCube;

      void SetUp() override;
      void TearDown() override;
  };

  class MiniRFCube : public TempTestingFiles {
    protected:
      Cube *testCube;

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


  class MroCtxCube : public DefaultCube {
    protected:
      std::unique_ptr<Cube> testCube;

      void SetUp() override;
      void TearDown() override;
  };

  class GalileoSsiCube : public DefaultCube {
    protected:
      void SetUp() override;
      void TearDown() override;
  };

  class MgsMocCube : public DefaultCube {
    protected:
      std::unique_ptr<Cube> testCube;

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


  class OsirisRexCube : public DefaultCube {
    protected:
      void setInstrument(QString ikid, QString instrumentId);
  };


  class ApolloCube : public LargeCube {
    protected:
      std::vector<std::pair<int, int>> reseaus;
      int reseauSize;

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

class HistoryBlob : public TempTestingFiles {
  protected:
    Blob historyBlob;
    PvlObject historyPvl;

    void SetUp() override;
};


class NullPixelCube : public TempTestingFiles {
  protected:
    Cube *testCube;
    void SetUp() override;
    void TearDown() override;
};


class MiniRFNetwork : public TempTestingFiles {
  protected:
    Cube *testCube1;
    Cube *testCube2;
    Cube *testCube3;

    FileList *cubeList;
    QString cubeListFile;

    ControlNet *network;
    QString controlNetPath;

    void SetUp() override;
    void TearDown() override;
};

class VikThmNetwork : public TempTestingFiles {
  protected:
    Cube *testCube1;
    Cube *testCube2;
    Cube *testCube3;
    Cube *testCube4;

    FileList *cubeList;
    QString cubeListFile;

    ControlNet *network;
    QString controlNetPath;

    void SetUp() override;
    void TearDown() override;
};

class CSMNetwork : public TempTestingFiles {
  protected:

    QVector<FileName> stateStringFiles;
    QVector<FileName> labelFiles;
    QVector<Cube*> cubes;

    FileList *cubeList;
    QString cubeListFile;

    void SetUp() override;
    void TearDown() override;
};

class ClipperWacFcCube : public DefaultCube {
  protected:
    Cube *wacFcCube;
    Pvl label;
    json isd;
    void SetUp() override;
    void TearDown() override;
};

class ClipperNacRsCube : public DefaultCube {
  protected:
    void SetUp() override;
    void TearDown() override;
};

class ClipperPbCube : public TempTestingFiles {
  protected:
    Cube *testCube;
    void setInstrument(QString instrumentId);
};

class NearMsiCameraCube : public TempTestingFiles {
  protected:
    // Cube *testCube;
    std::unique_ptr<Cube> testCube;
    void SetUp() override;
    void TearDown() override;
};

class TgoCassisModuleKernels : public ::testing::Test {

  protected:
    // You can define per-test set-up logic as usual.
    void SetUp() override;

    // You can define per-test tear-down logic as usual.
    void TearDown() override;

    QTemporaryDir kernelPrefix;

    QVector<QString> binaryCkKernels;
    QVector<QString> binarySpkKernels;

    QString binaryCkKernelsAsString;
    QString binarySpkKernelsAsString;
};

}

#endif
