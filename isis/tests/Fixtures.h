#ifndef Fixtures_h
#define Fixtures_h

#include "gtest/gtest.h"

#include <string>

#include <QString>
#include <QTemporaryDir>
#include <QTemporaryFile>

#include <nlohmann/json.hpp>

#include "Cube.h"
#include "IException.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "ImagePolygon.h"
#include "PolygonTools.h"
#include "Blob.h"
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

      Cube *cube1;
      Cube *cube2;
      Cube *cube3;

      FileName *isdPath1;
      FileName *isdPath2;
      FileName *isdPath3;

      FileName *threeImageOverlapFile;
      FileName *twoImageOverlapFile;

      FileList *cubeList;
      QString cubeListFile;

      std::vector<std::vector<double>> coords;

      void SetUp() override;
      void TearDown() override;
  };

  class ApolloNetwork: public TempTestingFiles {
    protected:
      Cube *cube1, *cube2, *cube3, *cube4, *cube5, *cube6, *cube7; 
      FileName *isdFile1, *isdFile2, *isdFile3, *isdFile4, *isdFile5, *isdFile6, *isdFile7,
               *cubeLabel, *label1, *label2, *label3, *label4, *label5, *label6, *label7;

      FileList *cubeList;
      QString cubeListFile;
      QString controlNetPath;
      
      void SetUp() override;
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

  class MroCube : public DefaultCube {
    protected:
      QString ckPath = "data/mroKernels/mroCK.bc";
      QString sclkPath = "data/mroKernels/mroSCLK.tsc";
      QString lskPath = "data/mroKernels/mroLSK.tls";
      QString jitterPath; 

      void setInstrument(QString ikid, QString instrumentId, QString spacecraftName); 
  };
}

#endif
