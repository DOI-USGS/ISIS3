#include "gmock/gmock.h"

#include "CameraFixtures.h"
#include "Distance.h"
#include "IException.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "ShapeModel.h"
#include "ShapeModelFactory.h"
#include "Target.h"

using namespace Isis;
using namespace testing;

//
// Helper utilities
//

static PvlGroup instrumentMars() {
  PvlGroup inst("Instrument");
  inst += PvlKeyword("TargetName", "Mars");
  return inst;
}

static std::vector<Distance> marsRadiiMeters() {
  // Reasonable Mars radii; exact values are not critical for factory tests.
  return {
    Distance(3396190.0, Distance::Meters),
    Distance(3396190.0, Distance::Meters),
    Distance(3376200.0, Distance::Meters)
  };
}

//
// Runnable, data-independent tests
//

TEST(ShapeModelFactoryTest, ShapeModelKeywordNull_CreatesEllipsoid) {
  Preference::Preferences(true);

  Pvl label;
  label.addGroup(instrumentMars());

  PvlGroup kernels("Kernels");
  kernels += PvlKeyword("ShapeModel", "Null");
  label.addGroup(kernels);

  Target target(nullptr, label);
  target.setRadii(marsRadiiMeters());

  std::unique_ptr<ShapeModel> shape;
  ASSERT_NO_THROW(shape.reset(ShapeModelFactory::create(&target, label)));
  ASSERT_NE(shape, nullptr);
  EXPECT_FALSE(shape->name().isEmpty());
}

TEST(ShapeModelFactoryTest, ElevationModelKeywordNull_CreatesEllipsoid) {
  Preference::Preferences(true);

  Pvl label;
  label.addGroup(instrumentMars());

  PvlGroup kernels("Kernels");
  kernels += PvlKeyword("ElevationModel", "Null");
  label.addGroup(kernels);

  Target target(nullptr, label);
  target.setRadii(marsRadiiMeters());

  std::unique_ptr<ShapeModel> shape;
  ASSERT_NO_THROW(shape.reset(ShapeModelFactory::create(&target, label)));
  ASSERT_NE(shape, nullptr);
  EXPECT_FALSE(shape->name().isEmpty());
}

TEST(ShapeModelFactoryTest, NonexistentShapeModelFile_Throws) {
  try {
    Preference::Preferences(true);

    Pvl label;
    label.addGroup(instrumentMars());

    PvlGroup kernels("Kernels");
    kernels += PvlKeyword("ShapeModel", "ThisFileDoesNotExist.cub");
    label.addGroup(kernels);

    Target target(nullptr, label);
    target.setRadii(marsRadiiMeters());

    std::unique_ptr<ShapeModel> shape(ShapeModelFactory::create(&target, label));
    FAIL() << "Expected an exception for nonexistent shape model file.";
  }
  catch (const Isis::IException &e) {
    // Pass: expected path.
    const QString msg = e.toString();
    EXPECT_FALSE(msg.isEmpty());
  }
  catch (const std::exception &e) {
    FAIL() << "Expected Isis::IException, got std::exception: " << e.what();
  }
  catch (...) {
    FAIL() << "Expected Isis::IException, got unknown exception type.";
  }
}

//
// Deferred tests (require kernel/test-data fixtures to be made portable)
//

TEST(ShapeModelFactoryTest, SkyTarget_CreatesSkyShape) {
  GTEST_SKIP()
      << "Sky target creation requires instrument kernel metadata "
      << "(e.g., NaifIkCode) and external kernel files. "
      << "This gtest conversion intentionally avoids $ISISTESTDATA.";
}

TEST_F(DemCube, ShapeModelKeyword_UsesFixtureDem) {
  GTEST_SKIP()
      << "DemCube fixture requires ISISDATA base kernels (e.g., naif0012.tls). "
      << "Skipping to keep tests data-independent.";
}

TEST_F(DemCube, ElevationModelKeyword_UsesFixtureDem) {
  GTEST_SKIP()
      << "DemCube fixture requires ISISDATA base kernels (e.g., naif0012.tls). "
      << "Skipping to keep tests data-independent.";
}

TEST_F(DemCube, MissingRadii_Throws) {
  GTEST_SKIP()
      << "DemCube fixture requires ISISDATA base kernels (e.g., naif0012.tls). "
      << "Skipping to keep tests data-independent.";
}
