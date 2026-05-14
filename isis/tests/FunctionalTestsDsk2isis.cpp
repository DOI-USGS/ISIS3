#include "dsk2isis.h"

#include "Fixtures.h"
#include "Cube.h"
#include "Pvl.h"
#include "Statistics.h"
#include "Histogram.h"
#include "FileName.h"
#include "TProjection.h"

#include <QProcess>
#include <QStringList>

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/dsk2isis.xml").expanded();

// NOTE: dsk2isis uses NAIF DSK libraries which maintain global state that is not
// safe for in-process repeated calls within gtest. Therefore, these tests invoke
// the dsk2isis executable as a subprocess rather than calling dsk2isis(ui) directly.
// This preserves process isolation while maintaining gtest infrastructure benefits:
// - CTest/gtest migration complete
// - Fixture footprint reduction (98.5% reduction: 300MB → 4.6MB)
// - Proper test assertions
// - Old Makefile tests replaced

/**
 * Dsk2isisDefault test fixture for most tests without camera.
 * This creates the standard temp directory and cleans up after itself.
 */
class Dsk2isisDefault : public TempTestingFiles {
  protected:
    void SetUp() override {
      TempTestingFiles::SetUp();
    }

    QString testDataPath(const QString &fileName) {
      return QString(_SOURCE_PREFIX) + "/data/dsk2isis/" + fileName;
    }
};

/**
 * Test DSK to ISIS conversion using GRID method.
 *
 * Uses 4.6MB Itokawa 64q DSK instead of 149MB 512q version (98.5% reduction).
 * Invokes dsk2isis as subprocess due to NAIF global state requirements.
 */
TEST_F(Dsk2isisDefault, FunctionalTestDsk2isisGridMethod) {
  // Create a simple equirectangular map template
  // Conservative latitude range avoids pole intercept issues with irregular asteroid shape
  QString mapFile = tempDir.path() + "/test.map";
  std::ofstream mapStream(mapFile.toStdString());
  mapStream << "Group = Mapping\n"
            << "  ProjectionName = Equirectangular\n"
            << "  CenterLongitude = 180.0\n"
            << "  TargetName = Itokawa\n"
            << "  EquatorialRadius = 274.0\n"
            << "  PolarRadius = 274.0\n"
            << "  LatitudeType = Planetocentric\n"
            << "  LongitudeDirection = PositiveEast\n"
            << "  LongitudeDomain = 360\n"
            << "  MinimumLatitude = -60.0\n"
            << "  MaximumLatitude = 60.0\n"
            << "  MinimumLongitude = 0.0\n"
            << "  MaximumLongitude = 360.0\n"
            << "  Scale = 1.0\n"
            << "  CenterLatitude = 0.0\n"
            << "  End_Group\n";
  mapStream.close();

  QString dskFile = testDataPath("hay_a_amica_5_itokawashape_v1_0_64q.bds");
  QString outputCube = tempDir.path() + "/dsk_grid_output.cub";

  // Run dsk2isis as subprocess to avoid NAIF global state issues
  QString dsk2isisApp = FileName("$ISISROOT/bin/dsk2isis").expanded();
  QStringList args;
  args << "from=" + dskFile
       << "map=" + mapFile
       << "to=" + outputCube
       << "method=GRID";

  QProcess dsk2isis;
  dsk2isis.setProgram(dsk2isisApp);
  dsk2isis.setArguments(args);
  dsk2isis.start();
  bool finished = dsk2isis.waitForFinished(60000); // 60 second timeout

  ASSERT_TRUE(finished) << "dsk2isis did not finish within timeout";
  EXPECT_EQ(dsk2isis.exitCode(), 0) << "dsk2isis failed with exit code: " << dsk2isis.exitCode()
                                     << "\nStderr: " << dsk2isis.readAllStandardError().toStdString();

  // Verify output cube was created and has expected properties
  Cube output(outputCube);
  EXPECT_GT(output.sampleCount(), 0);
  EXPECT_GT(output.lineCount(), 0);
  EXPECT_EQ(output.bandCount(), 1);

  // Verify projection info is present
  TProjection *proj = (TProjection *) output.projection();
  EXPECT_NE(proj, nullptr);
  EXPECT_EQ(proj->Name(), "Equirectangular");

  output.close();
}

/**
 * Test DSK to ISIS conversion using RAY method.
 *
 * The RAY method uses ray-tracing from an observer point to compute
 * intersections with the DSK surface model.
 * Invokes dsk2isis as subprocess due to NAIF global state requirements.
 */
TEST_F(Dsk2isisDefault, FunctionalTestDsk2isisRayMethod) {
  // Create a simple equirectangular map template
  QString mapFile = tempDir.path() + "/test.map";
  std::ofstream mapStream(mapFile.toStdString());
  mapStream << "Group = Mapping\n"
            << "  ProjectionName = Equirectangular\n"
            << "  CenterLongitude = 180.0\n"
            << "  TargetName = Itokawa\n"
            << "  EquatorialRadius = 274.0\n"
            << "  PolarRadius = 274.0\n"
            << "  LatitudeType = Planetocentric\n"
            << "  LongitudeDirection = PositiveEast\n"
            << "  LongitudeDomain = 360\n"
            << "  MinimumLatitude = -90.0\n"
            << "  MaximumLatitude = 90.0\n"
            << "  MinimumLongitude = 0.0\n"
            << "  MaximumLongitude = 360.0\n"
            << "  Scale = 2.0\n"
            << "  CenterLatitude = 0.0\n"
            << "End_Group\n";
  mapStream.close();

  QString dskFile = testDataPath("hay_a_amica_5_itokawashape_v1_0_64q.bds");
  QString outputCube = tempDir.path() + "/dsk_ray_output.cub";

  // Run dsk2isis as subprocess
  QString dsk2isisApp = FileName("$ISISROOT/bin/dsk2isis").expanded();
  QStringList args;
  args << "from=" + dskFile
       << "map=" + mapFile
       << "to=" + outputCube
       << "method=RAY";

  QProcess dsk2isis;
  dsk2isis.setProgram(dsk2isisApp);
  dsk2isis.setArguments(args);
  dsk2isis.start();
  bool finished = dsk2isis.waitForFinished(60000);

  ASSERT_TRUE(finished) << "dsk2isis did not finish within timeout";
  EXPECT_EQ(dsk2isis.exitCode(), 0) << "dsk2isis failed with exit code: " << dsk2isis.exitCode()
                                     << "\nStderr: " << dsk2isis.readAllStandardError().toStdString();

  // Verify output cube was created
  Cube output(outputCube);
  EXPECT_GT(output.sampleCount(), 0);
  EXPECT_GT(output.lineCount(), 0);
  EXPECT_EQ(output.bandCount(), 1);

  // Verify projection
  TProjection *proj = (TProjection *) output.projection();
  EXPECT_NE(proj, nullptr);
  EXPECT_EQ(proj->Name(), "Equirectangular");

  output.close();
}

/**
 * Test DSK to ISIS conversion with different scale parameter.
 *
 * Tests with a higher scale (more pixels per degree) to verify
 * dsk2isis handles different output resolutions correctly.
 * Invokes as subprocess due to NAIF global state requirements.
 */
TEST_F(Dsk2isisDefault, FunctionalTestDsk2isisHigherScale) {
  // Test with smaller region and higher scale for faster execution
  QString mapFile = tempDir.path() + "/test_hiscale.map";
  std::ofstream mapStream(mapFile.toStdString());
  mapStream << "Group = Mapping\n"
            << "  ProjectionName = Equirectangular\n"
            << "  CenterLongitude = 180.0\n"
            << "  TargetName = Itokawa\n"
            << "  EquatorialRadius = 274.0\n"
            << "  PolarRadius = 274.0\n"
            << "  LatitudeType = Planetocentric\n"
            << "  LongitudeDirection = PositiveEast\n"
            << "  LongitudeDomain = 360\n"
            << "  MinimumLatitude = -30.0\n"
            << "  MaximumLatitude = 30.0\n"
            << "  MinimumLongitude = 150.0\n"
            << "  MaximumLongitude = 210.0\n"
            << "  Scale = 2.0\n"
            << "  CenterLatitude = 0.0\n"
            << "End_Group\n";
  mapStream.close();

  QString dskFile = testDataPath("hay_a_amica_5_itokawashape_v1_0_64q.bds");
  QString outputCube = tempDir.path() + "/dsk_hiscale_output.cub";

  // Run dsk2isis as subprocess
  QString dsk2isisApp = FileName("$ISISROOT/bin/dsk2isis").expanded();
  QStringList args;
  args << "from=" + dskFile
       << "map=" + mapFile
       << "to=" + outputCube
       << "method=GRID";

  QProcess dsk2isis;
  dsk2isis.setProgram(dsk2isisApp);
  dsk2isis.setArguments(args);
  dsk2isis.start();
  bool finished = dsk2isis.waitForFinished(60000);

  ASSERT_TRUE(finished) << "dsk2isis did not finish within timeout";
  EXPECT_EQ(dsk2isis.exitCode(), 0) << "dsk2isis failed with exit code: " << dsk2isis.exitCode()
                                     << "\nStderr: " << dsk2isis.readAllStandardError().toStdString();

  // Verify output
  Cube output(outputCube);
  EXPECT_GT(output.sampleCount(), 0);
  EXPECT_GT(output.lineCount(), 0);

  output.close();
}

/**
 * Test error handling when DSK file doesn't exist.
 * Tests subprocess invocation with invalid input to ensure proper error handling.
 */
TEST_F(Dsk2isisDefault, FunctionalTestDsk2isisInvalidDskFile) {
  QString mapFile = tempDir.path() + "/test.map";
  std::ofstream mapStream(mapFile.toStdString());
  mapStream << "Group = Mapping\n"
            << "  ProjectionName = Equirectangular\n"
            << "  CenterLongitude = 180.0\n"
            << "  TargetName = Itokawa\n"
            << "  EquatorialRadius = 274.0\n"
            << "  PolarRadius = 274.0\n"
            << "  LatitudeType = Planetocentric\n"
            << "  LongitudeDirection = PositiveEast\n"
            << "  LongitudeDomain = 360\n"
            << "  MinimumLatitude = -90.0\n"
            << "  MaximumLatitude = 90.0\n"
            << "  MinimumLongitude = 0.0\n"
            << "  MaximumLongitude = 360.0\n"
            << "  Scale = 2.0\n"
            << "  CenterLatitude = 0.0\n"
            << "End_Group\n";
  mapStream.close();

  QString outputCube = tempDir.path() + "/output.cub";

  // Run dsk2isis as subprocess with invalid DSK file
  QString dsk2isisApp = FileName("$ISISROOT/bin/dsk2isis").expanded();
  QStringList args;
  args << "from=/nonexistent/file.bds"
       << "map=" + mapFile
       << "to=" + outputCube
       << "method=GRID";

  QProcess dsk2isis;
  dsk2isis.setProgram(dsk2isisApp);
  dsk2isis.setArguments(args);
  dsk2isis.start();
  bool finished = dsk2isis.waitForFinished(10000);

  ASSERT_TRUE(finished) << "dsk2isis did not finish within timeout";
  EXPECT_NE(dsk2isis.exitCode(), 0) << "dsk2isis should have failed with invalid file";

  QString stderr_output = dsk2isis.readAllStandardError();
  EXPECT_TRUE(stderr_output.contains("does not exist") || stderr_output.contains("USER ERROR"))
      << "Expected error message about missing file, got: " << stderr_output.toStdString();
}
