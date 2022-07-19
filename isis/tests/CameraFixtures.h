#ifndef CameraFixtures_h
#define CameraFixtures_h

#include "gtest/gtest.h"

#include <memory>

#include <QString>

#include <nlohmann/json.hpp>

#include "Cube.h"
#include "Pvl.h"
#include "TempFixtures.h"

namespace Isis {

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
      nlohmann::json isd;

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
      nlohmann::json isd;

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

  class ClipperWacFcCube : public DefaultCube {
    protected:
      Cube *wacFcCube;
      Pvl label;
      nlohmann::json isd;
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

}

#endif