/* SPDX-License-Identifier: CC0-1.0 */

#include <memory>

#include <gtest/gtest.h>

#include "AbstractPlate.h"
#include "Angle.h"
#include "Displacement.h"
#include "Distance.h"
#include "Intercept.h"
#include "Preference.h"
#include "SurfacePoint.h"

using namespace Isis;

namespace {

class MyPlate : public AbstractPlate {
  public:
    MyPlate() : AbstractPlate() {}
    ~MyPlate() override = default;

    QString name() const override {
      return AbstractPlate::name();
    }

    Distance minRadius() const override {
      return Distance(1.0, Distance::Meters);
    }

    Distance maxRadius() const override {
      return Distance(2.0, Distance::Meters);
    }

    double area() const override {
      return 3.0;
    }

    NaifVector normal() const override {
      NaifVector v;
      return v;
    }

    Angle separationAngle(const NaifVector &raydir) const override {
      (void)raydir;
      Angle theta;
      return theta;
    }

    bool hasIntercept(const NaifVertex &vertex, const NaifVector &raydir) const override {
      (void)vertex;
      (void)raydir;
      return true;
    }

    bool hasPoint(const Latitude &lat, const Longitude &lon) const override {
      (void)lat;
      (void)lon;
      return false;
    }

    Intercept *intercept(const NaifVertex &vertex, const NaifVector &raydir) const override {
      (void)vertex;
      (void)raydir;
      return nullptr;
    }

    SurfacePoint *point(const Latitude &lat, const Longitude &lon) const override {
      (void)lat;
      (void)lon;
      return nullptr;
    }

    AbstractPlate *clone() const override {
      return new MyPlate();
    }

    Intercept *construct(const NaifVertex &vertex, const NaifVector &raydir,
                         SurfacePoint *ipoint) const {
      return AbstractPlate::construct(vertex, raydir, ipoint);
    }
};

class AbstractPlateFixture : public ::testing::Test {
  protected:
    void SetUp() override {
      Preference::Preferences(true);
    }
};

}  // namespace

TEST_F(AbstractPlateFixture, ConstructInterceptPopulatesFields) {
  MyPlate mp;

  NaifVertex vertex(3);
  vertex[0] = 0.0;
  vertex[1] = 0.0;
  vertex[2] = 0.0;

  NaifVector raydir(3);
  raydir[0] = 1.0;
  raydir[1] = 1.0;
  raydir[2] = 1.0;

  auto ipoint = std::make_unique<SurfacePoint>(
      Displacement(2.0, Displacement::Meters),
      Displacement(2.0, Displacement::Meters),
      Displacement(2.0, Displacement::Meters));

  SurfacePoint *rawPoint = ipoint.release();

  Intercept *rawIntercept = mp.construct(vertex, raydir, rawPoint);
  if (!rawIntercept) {
    delete rawPoint;
    FAIL() << "AbstractPlate::construct returned null Intercept";
  }

  std::unique_ptr<Intercept> intercept(rawIntercept);

  ASSERT_NE(intercept->shape(), nullptr);
  EXPECT_EQ(intercept->shape()->name(), mp.name());

  const NaifVertex obs = intercept->observer();
  EXPECT_DOUBLE_EQ(obs[0], 0.0);
  EXPECT_DOUBLE_EQ(obs[1], 0.0);
  EXPECT_DOUBLE_EQ(obs[2], 0.0);

  const NaifVector look = intercept->lookDirectionRay();
  EXPECT_DOUBLE_EQ(look[0], 1.0);
  EXPECT_DOUBLE_EQ(look[1], 1.0);
  EXPECT_DOUBLE_EQ(look[2], 1.0);

  EXPECT_DOUBLE_EQ(intercept->location().GetX().meters(), 2.0);
  EXPECT_DOUBLE_EQ(intercept->location().GetY().meters(), 2.0);
  EXPECT_DOUBLE_EQ(intercept->location().GetZ().meters(), 2.0);
}
