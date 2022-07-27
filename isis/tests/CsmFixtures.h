#ifndef CsmFixtures_h
#define CsmFixtures_h

#include "gmock/gmock.h"

#include <QString>
#include <QVector>

#include "csm/csm.h"
#include "csm/Ellipsoid.h"

#include "Camera.h"
#include "CubeFixtures.h"
#include "FileList.h"
#include "FileName.h"
#include "Mocks.h"

namespace Isis {


  ::testing::Matcher<const csm::ImageCoord&> MatchImageCoord(const csm::ImageCoord &expected);
  ::testing::Matcher<const csm::EcefCoord&> MatchEcefCoord(const csm::EcefCoord &expected);

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

}

#endif