#include "NaifDskPlateModel.h"

#include <math.h>

#include <QString>

#include "FileName.h"
#include "IException.h"
#include "Intercept.h"
#include "Latitude.h"
#include "Longitude.h"
#include "NaifDskApi.h"
#include "SurfacePoint.h"

#include <gtest/gtest.h>

using namespace Isis;

class NaifDskPlateModelTest_ItokawaTest : public ::testing::Test {
  protected:
    NaifDskPlateModel dsk;
    QString dskFileName;

    void SetUp() override {
      FileName dskFile("$hayabusa/kernels/dsk/hay_a_amica_5_itokawashape_v1_0_512q.bds");
      dskFileName = dskFile.expanded();
      dsk = NaifDskPlateModel(dskFileName);
    }
};

class NaifDskPlateModelTest_MultiSegmentTest : public ::testing::Test {
  protected:
    NaifDskPlateModel dsk;
    QString dskFileName;

    void SetUp() override {
      FileName dskFile("$base/testData/test_shape.bds");
      dskFileName = dskFile.expanded();
      dsk = NaifDskPlateModel(dskFileName);
    }
};

TEST(NaifDskPlateModelTest, DefaultValid) {
  NaifDskPlateModel dsk;
  EXPECT_FALSE(dsk.isValid());
}

TEST(NaifDskPlateModelTest, DefaultFileName) {
  NaifDskPlateModel dsk;
  EXPECT_TRUE(dsk.filename().isEmpty());
}

TEST(NaifDskPlateModelTest, DefaultSize) {
  NaifDskPlateModel dsk;
  EXPECT_EQ(0, dsk.size());
}

TEST(NaifDskPlateModelTest, DefaultPlateCount) {
  NaifDskPlateModel dsk;
  EXPECT_EQ(0, dsk.numberPlates());
}

TEST(NaifDskPlateModelTest, DefaultVertexCount) {
  NaifDskPlateModel dsk;
  EXPECT_EQ(0, dsk.numberVertices());
}

TEST(NaifDskPlateModelTest, DefaultLatLonIntersect) {
  NaifDskPlateModel dsk;
  Latitude lat(0.0, Angle::Degrees);
  Longitude lon(0.0, Angle::Degrees);
  try {
    SurfacePoint *sp = dsk.point(lat, lon);
    delete sp;
    sp = nullptr;
    FAIL() << "Expected an error";
  }
  catch(Isis::IException &e) {
    EXPECT_TRUE(e.toString().contains(
        "NAIF DSK file not opened/valid!"))
        << e.toString().toStdString();
  }
  catch(...) {
    FAIL() << "Expected IException.";
  }
}

TEST(NaifDskPlateModelTest, DefaultObserverLookIntersection) {
  NaifDskPlateModel dsk;
  NaifVertex obsPos(3);
  NaifVector rayDir(3);
  obsPos[0] = 1000.0;   obsPos[1] = 0.0;   obsPos[2] = 0.0;
  rayDir[0] = -1.0;   rayDir[1] = 0.0;   rayDir[2] = 0.0;
  try {
    Intercept *intercept = dsk.intercept(obsPos, rayDir);
    delete intercept;
    intercept = nullptr;
    FAIL() << "Expected an error";
  }
  catch(Isis::IException &e) {
    EXPECT_TRUE(e.toString().contains(
        "NAIF DSK file not opened/valid!"))
        << e.toString().toStdString();
  }
  catch(...) {
    FAIL() << "Expected IException.";
  }
}

TEST_F(NaifDskPlateModelTest_ItokawaTest, Valid) {
  EXPECT_TRUE(dsk.isValid());
}

TEST_F(NaifDskPlateModelTest_ItokawaTest, FileName) {
  EXPECT_EQ(dskFileName, dsk.filename());
}

TEST_F(NaifDskPlateModelTest_ItokawaTest, Size) {
  EXPECT_EQ(3145728, dsk.size());
}

TEST_F(NaifDskPlateModelTest_ItokawaTest, PlateCount) {
  EXPECT_EQ(3145728, dsk.numberPlates());
}

TEST_F(NaifDskPlateModelTest_ItokawaTest, VertexCount) {
  EXPECT_EQ(1579014, dsk.numberVertices());
}

TEST_F(NaifDskPlateModelTest_ItokawaTest, LatLonIntersect) {
  Latitude lat(0.0, Angle::Degrees);
  Longitude lon(0.0, Angle::Degrees);
  SurfacePoint *sp = dsk.point(lat, lon);
  ASSERT_NE(sp, nullptr);
  EXPECT_NEAR(sp->GetX().meters(), 289.1103069767442, 1e-10);
  EXPECT_DOUBLE_EQ(sp->GetY().meters(), 0.0);
  EXPECT_DOUBLE_EQ(sp->GetZ().meters(), 0.0);
  delete sp;
  sp = nullptr;
}

TEST_F(NaifDskPlateModelTest_ItokawaTest, ObserverLookNoIntersection) {
  NaifVertex obsPos(3);
  NaifVector rayDir(3);
  obsPos[0] = 0.0;   obsPos[1] = 0.0;   obsPos[2] = 0.0;
  rayDir[0] = 1.0;   rayDir[1] = 1.0;   rayDir[2] = 1.0;
  Intercept *intercept = dsk.intercept(obsPos, rayDir);
  ASSERT_EQ(intercept, nullptr);
}

TEST_F(NaifDskPlateModelTest_ItokawaTest, BadObserver) {
  NaifVertex obsPos(2);
  NaifVector rayDir(3);
  obsPos[0] = 0.0;   obsPos[1] = 0.0;
  rayDir[0] = 1.0;   rayDir[1] = 1.0;   rayDir[2] = 1.0;
  try {
    Intercept *intercept = dsk.intercept(obsPos, rayDir);
    delete intercept;
    intercept = nullptr;
    FAIL() << "Expected an error";
  }
  catch(Isis::IException &e) {
    EXPECT_TRUE(e.toString().contains(
        "Invalid/bad dimensions on intercept source point"))
        << e.toString().toStdString();
  }
  catch(...) {
    FAIL() << "Expected IException.";
  }
}

TEST_F(NaifDskPlateModelTest_ItokawaTest, BadLook) {
  NaifVertex obsPos(3);
  NaifVector rayDir(2);
  obsPos[0] = 0.0;   obsPos[1] = 0.0;   obsPos[2] = 0.0;
  rayDir[0] = 1.0;   rayDir[1] = 1.0;
  try {
    Intercept *intercept = dsk.intercept(obsPos, rayDir);
    delete intercept;
    intercept = nullptr;
    FAIL() << "Expected an error";
  }
  catch(Isis::IException &e) {
    EXPECT_TRUE(e.toString().contains(
        "Invalid/bad dimensions on ray direction vector"))
        << e.toString().toStdString();
  }
  catch(...) {
    FAIL() << "Expected IException.";
  }
}

TEST_F(NaifDskPlateModelTest_ItokawaTest, ObserverLookIntersection) {
  NaifVertex obsPos(3);
  NaifVector rayDir(3);
  obsPos[0] = 1000.0;   obsPos[1] = 0.0;   obsPos[2] = 0.0;
  rayDir[0] = -1.0;   rayDir[1] = 0.0;   rayDir[2] = 0.0;
  Intercept *intercept = dsk.intercept(obsPos, rayDir);
  ASSERT_NE(intercept, nullptr);
  EXPECT_TRUE(intercept->isValid());
  EXPECT_EQ(intercept->shape()->name(), "TriangularPlate");
  EXPECT_EQ(intercept->observer()[0], obsPos[0]);
  EXPECT_EQ(intercept->observer()[1], obsPos[1]);
  EXPECT_EQ(intercept->observer()[2], obsPos[2]);
  EXPECT_EQ(intercept->lookDirectionRay()[0], rayDir[0]);
  EXPECT_EQ(intercept->lookDirectionRay()[1], rayDir[1]);
  EXPECT_EQ(intercept->lookDirectionRay()[2], rayDir[2]);
  EXPECT_NEAR(intercept->location().GetX().meters(), 289.1103069767442, 1e-10);
  EXPECT_DOUBLE_EQ(intercept->location().GetY().meters(), 0.0);
  EXPECT_DOUBLE_EQ(intercept->location().GetZ().meters(), 0.0);
  delete intercept;
  intercept = nullptr;
}

TEST_F(NaifDskPlateModelTest_MultiSegmentTest, Valid) {
  EXPECT_TRUE(dsk.isValid());
}

TEST_F(NaifDskPlateModelTest_MultiSegmentTest, FileName) {
  EXPECT_EQ(dskFileName, dsk.filename());
}

TEST_F(NaifDskPlateModelTest_MultiSegmentTest, Size) {
  EXPECT_EQ(28, dsk.size());
}

TEST_F(NaifDskPlateModelTest_MultiSegmentTest, PlateCount) {
  EXPECT_EQ(28, dsk.numberPlates());
}

TEST_F(NaifDskPlateModelTest_MultiSegmentTest, VertexCount) {
  EXPECT_EQ(26, dsk.numberVertices());
}

TEST_F(NaifDskPlateModelTest_MultiSegmentTest, LatLonIntersectOverlap) {
  Latitude lat(M_PI/2 - atan2(sqrt(2*2 + 2*2),2), Angle::Radians);
  Longitude lon(45.0, Angle::Degrees);
  SurfacePoint *sp = dsk.point(lat, lon);
  ASSERT_NE(sp, nullptr);
  EXPECT_NEAR(sp->GetX().kilometers(), 2.0, 1e-10);
  EXPECT_NEAR(sp->GetY().kilometers(), 2.0, 1e-10);
  EXPECT_NEAR(sp->GetZ().kilometers(), 2.0, 1e-10);
  delete sp;
  sp = nullptr;
}

TEST_F(NaifDskPlateModelTest_MultiSegmentTest, LatLonIntersect) {
  Latitude lat(M_PI/2 - atan2(sqrt(0.5*0.5 + 0.5*0.5),5), Angle::Radians);
  Longitude lon(45.0, Angle::Degrees);
  SurfacePoint *sp = dsk.point(lat, lon);
  ASSERT_NE(sp, nullptr);
  EXPECT_NEAR(sp->GetX().kilometers(), 0.5, 1e-10);
  EXPECT_NEAR(sp->GetY().kilometers(), 0.5, 1e-10);
  EXPECT_NEAR(sp->GetZ().kilometers(), 5.0, 1e-10);
  delete sp;
  sp = nullptr;
}

TEST_F(NaifDskPlateModelTest_MultiSegmentTest, ObserverLookIntersectionOverlap) {
  NaifVertex obsPos(3);
  NaifVector rayDir(3);
  obsPos[0] = 100.0;   obsPos[1] = 100.0;   obsPos[2] = 100.0;
  rayDir[0] = -1.0;   rayDir[1] = -1.0;   rayDir[2] = -1.0;
  Intercept *intercept = dsk.intercept(obsPos, rayDir);
  ASSERT_NE(intercept, nullptr);
  EXPECT_TRUE(intercept->isValid());
  EXPECT_EQ(intercept->shape()->name(), "TriangularPlate");
  EXPECT_EQ(intercept->observer()[0], obsPos[0]);
  EXPECT_EQ(intercept->observer()[1], obsPos[1]);
  EXPECT_EQ(intercept->observer()[2], obsPos[2]);
  EXPECT_EQ(intercept->lookDirectionRay()[0], rayDir[0]);
  EXPECT_EQ(intercept->lookDirectionRay()[1], rayDir[1]);
  EXPECT_EQ(intercept->lookDirectionRay()[2], rayDir[2]);
  EXPECT_NEAR(intercept->location().GetX().kilometers(), 2.0, 1e-10);
  EXPECT_NEAR(intercept->location().GetY().kilometers(), 2.0, 1e-10);
  EXPECT_NEAR(intercept->location().GetZ().kilometers(), 2.0, 1e-10);
  delete intercept;
  intercept = nullptr;
}

TEST_F(NaifDskPlateModelTest_MultiSegmentTest, ObserverLookIntersection) {
  NaifVertex obsPos(3);
  NaifVector rayDir(3);
  obsPos[0] = 98.5;   obsPos[1] = 98.5;   obsPos[2] = 103.0;
  rayDir[0] = -1.0;   rayDir[1] = -1.0;   rayDir[2] = -1.0;
  Intercept *intercept = dsk.intercept(obsPos, rayDir);
  ASSERT_NE(intercept, nullptr);
  EXPECT_TRUE(intercept->isValid());
  EXPECT_EQ(intercept->shape()->name(), "TriangularPlate");
  EXPECT_EQ(intercept->observer()[0], obsPos[0]);
  EXPECT_EQ(intercept->observer()[1], obsPos[1]);
  EXPECT_EQ(intercept->observer()[2], obsPos[2]);
  EXPECT_EQ(intercept->lookDirectionRay()[0], rayDir[0]);
  EXPECT_EQ(intercept->lookDirectionRay()[1], rayDir[1]);
  EXPECT_EQ(intercept->lookDirectionRay()[2], rayDir[2]);
  EXPECT_NEAR(intercept->location().GetX().kilometers(), 0.5, 1e-10);
  EXPECT_NEAR(intercept->location().GetY().kilometers(), 0.5, 1e-10);
  EXPECT_NEAR(intercept->location().GetZ().kilometers(), 5, 1e-10);
  delete intercept;
  intercept = nullptr;
}
