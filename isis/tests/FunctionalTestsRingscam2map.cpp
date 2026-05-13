// SPDX-License-Identifier: CC0-1.0

#include <QProcess>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QElapsedTimer>
#include <QDir>

#include "Fixtures.h"
#include "Cube.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "FileName.h"
#include "IException.h"
#include "gtest/gtest.h"

using namespace Isis;

/**
 * ringscam2map functional tests use subprocess invocation due to ProcessRubberSheet
 * and camera/SPICE initialization requirements that cause segfaults when called
 * directly in the gtest environment. Similar to dsk2isis, the CLI execution matches
 * production behavior and isolates Application singleton state per test.
 */

static QString APP_XML = FileName("$ISISROOT/bin/xml/ringscam2map.xml").expanded();

// Helper function to get test data path in ISISTESTDATA
QString testDataPath(const QString &fileName) {
  return FileName("$ISISTESTDATA/isis/src/base/apps/ringscam2map/" + fileName).expanded();
}

// Helper to run ringscam2map via subprocess
struct RingscamResult {
  int exitCode;
  QString stdout;
  QString stderr;
};

RingscamResult runRingscam2map(const QStringList &args, int timeoutMs = 300000, bool captureOutput = false) {
  QString appPath = FileName("$ISISROOT/bin/ringscam2map").expanded();

  // Create temp log files for stdout/stderr
  QTemporaryFile stdoutLog;
  QTemporaryFile stderrLog;
  stdoutLog.setAutoRemove(true);
  stderrLog.setAutoRemove(true);

  if (!stdoutLog.open() || !stderrLog.open()) {
    RingscamResult result;
    result.exitCode = -1;
    result.stderr = "Failed to create temporary log files";
    return result;
  }

  QString stdoutPath = stdoutLog.fileName();
  QString stderrPath = stderrLog.fileName();

  QProcess process;

  // Set up environment and working directory
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  process.setProcessEnvironment(env);
  process.setWorkingDirectory(QDir::currentPath());

  // Redirect output to temp files to avoid pipe buffer blocking and log flooding
  process.setStandardOutputFile(stdoutPath);
  process.setStandardErrorFile(stderrPath);

  process.setProgram(appPath);
  process.setArguments(args);

  process.start();
  process.closeWriteChannel();  // Close stdin to prevent process from waiting for input

  // Wait for process to finish
  bool finished = process.waitForFinished(timeoutMs);

  RingscamResult result;

  // Read log files
  auto readLogTail = [](const QString &path, int maxLines = 50) -> QString {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      return QString();
    }

    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
      lines.append(in.readLine());
    }

    // Return last maxLines
    if (lines.size() > maxLines) {
      return lines.mid(lines.size() - maxLines).join("\n");
    }
    return lines.join("\n");
  };

  if (!finished) {
    // Timeout reached - kill and report details
    process.kill();
    process.waitForFinished(5000);

    result.exitCode = -1;
    result.stderr = QString("Process timed out after %1 ms\n"
                           "Command: %2\n"
                           "Arguments: %3\n"
                           "Exit status: %4\n"
                           "\n=== Last 50 lines of stderr ===\n%5\n"
                           "\n=== Last 50 lines of stdout ===\n%6")
                    .arg(timeoutMs)
                    .arg(appPath)
                    .arg(args.join(" "))
                    .arg(process.exitStatus() == QProcess::NormalExit ? "Normal" : "Crashed")
                    .arg(readLogTail(stderrPath))
                    .arg(readLogTail(stdoutPath));
  }
  else if (process.exitStatus() == QProcess::CrashExit) {
    // Process crashed - report details
    result.exitCode = -1;
    result.stderr = QString("Process crashed\n"
                           "Command: %1\n"
                           "Arguments: %2\n"
                           "\n=== Last 50 lines of stderr ===\n%3\n"
                           "\n=== Last 50 lines of stdout ===\n%4")
                    .arg(appPath)
                    .arg(args.join(" "))
                    .arg(readLogTail(stderrPath))
                    .arg(readLogTail(stdoutPath));
  }
  else if (process.exitCode() != 0) {
    // Non-zero exit code - report details
    result.exitCode = process.exitCode();
    result.stderr = QString("Process exited with code %1\n"
                           "Command: %2\n"
                           "Arguments: %3\n"
                           "\n=== Last 50 lines of stderr ===\n%4\n"
                           "\n=== Last 50 lines of stdout ===\n%5")
                    .arg(result.exitCode)
                    .arg(appPath)
                    .arg(args.join(" "))
                    .arg(readLogTail(stderrPath))
                    .arg(readLogTail(stdoutPath));
  }
  else {
    // Success - keep output clean, but read full output if captureOutput requested
    result.exitCode = 0;

    if (captureOutput) {
      // Read full output for error tests that validate error messages
      QFile stdoutFile(stdoutPath);
      QFile stderrFile(stderrPath);

      if (stdoutFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        result.stdout = QTextStream(&stdoutFile).readAll();
      }

      if (stderrFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        result.stderr = QTextStream(&stderrFile).readAll();
      }
    }
    else {
      // Success with no output capture - keep clean
      result.stdout = "";
      result.stderr = "";
    }
  }

  return result;
}

// Test Default: default parameters (matchmap=false, pixres=camera, defaultrange=minimize, etc.)
TEST_F(TempTestingFiles, FunctionalTestRingscam2mapDefault) {
  QString inputFile = testDataPath("tsts/default/input/v1496720128_1_vis_cal.cub");
  QString mapFile = testDataPath("tsts/default/input/planar.map");
  QString outputFile = tempDir.path() + "/vimsVIS.cub";

  QStringList args = {
    "from=" + inputFile,
    "map=" + mapFile,
    "to=" + outputFile
  };

  RingscamResult result = runRingscam2map(args);

  if (result.exitCode != 0) {
    FAIL() << "ringscam2map failed with exit code " << result.exitCode
           << "\nstdout: " << result.stdout.toStdString()
           << "\nstderr: " << result.stderr.toStdString();
  }

  ASSERT_TRUE(QFile::exists(outputFile)) << "Output file not created";

  Cube output(outputFile);

  // Validate exact output dimensions
  EXPECT_EQ(output.sampleCount(), 98);
  EXPECT_EQ(output.lineCount(), 87);
  EXPECT_EQ(output.bandCount(), 96);

  // Validate Mapping group and ring-plane projection keywords
  Pvl *label = output.label();
  ASSERT_TRUE(label->hasObject("IsisCube"));
  PvlObject &cube = label->findObject("IsisCube");
  ASSERT_TRUE(cube.hasGroup("Mapping")) << "Missing Mapping group in output";

  PvlGroup &mapping = cube.findGroup("Mapping");
  ASSERT_TRUE(mapping.hasKeyword("ProjectionName"));
  EXPECT_EQ(mapping["ProjectionName"][0].toStdString(), "Planar");

  ASSERT_TRUE(mapping.hasKeyword("TargetName"));
  EXPECT_EQ(mapping["TargetName"][0].toStdString(), "Saturn");

  ASSERT_TRUE(mapping.hasKeyword("RingLongitudeDirection"));
  EXPECT_EQ(mapping["RingLongitudeDirection"][0].toStdString(), "CounterClockwise");

  // Ring-plane specific keywords
  EXPECT_TRUE(mapping.hasKeyword("CenterRingRadius"));
  EXPECT_TRUE(mapping.hasKeyword("CenterRingLongitude"));
  EXPECT_TRUE(mapping.hasKeyword("MinimumRingRadius"));
  EXPECT_TRUE(mapping.hasKeyword("MaximumRingRadius"));
  EXPECT_TRUE(mapping.hasKeyword("MinimumRingLongitude"));
  EXPECT_TRUE(mapping.hasKeyword("MaximumRingLongitude"));
  EXPECT_TRUE(mapping.hasKeyword("PixelResolution"));

  // Validate pixel type
  PvlObject &core = cube.findObject("Core");
  PvlGroup &pixels = core.findGroup("Pixels");
  EXPECT_EQ(pixels["Type"][0].toStdString(), "Real");
}

// Test MatchMap: matchmap=yes
TEST_F(TempTestingFiles, FunctionalTestRingscam2mapMatchmap) {
  QString inputFile = testDataPath("tsts/matchmap/input/v1496720128_1_vis_cal.cub");
  QString mapFile = testDataPath("tsts/matchmap/input/planar.map");
  QString outputFile = tempDir.path() + "/vimsVIS.cub";

  QStringList args = {
    "from=" + inputFile,
    "map=" + mapFile,
    "to=" + outputFile,
    "matchmap=yes"
  };

  RingscamResult result = runRingscam2map(args);

  EXPECT_EQ(result.exitCode, 0) << "stderr: " << result.stderr.toStdString();
  ASSERT_TRUE(QFile::exists(outputFile));

  Cube output(outputFile);

  // Validate output dimensions (matchmap should match map template dimensions)
  EXPECT_EQ(output.sampleCount(), 214);
  EXPECT_EQ(output.lineCount(), 214);
  EXPECT_EQ(output.bandCount(), 96);

  // Validate Mapping group
  Pvl *label = output.label();
  PvlObject &cube = label->findObject("IsisCube");
  ASSERT_TRUE(cube.hasGroup("Mapping")) << "Missing Mapping group in matchmap output";

  PvlGroup &mapping = cube.findGroup("Mapping");
  EXPECT_TRUE(mapping.hasKeyword("ProjectionName"));
  EXPECT_EQ(mapping["ProjectionName"][0].toStdString(), "Planar");
  EXPECT_TRUE(mapping.hasKeyword("TargetName"));
  EXPECT_EQ(mapping["TargetName"][0].toStdString(), "Saturn");
}

// Test Combination1: map=*.lev2.cub, pixres=map, defaultrange=camera, trim=yes, etc.
TEST_F(TempTestingFiles, FunctionalTestRingscam2mapCombination1) {
  QString inputFile = testDataPath("tsts/combination1/input/v1496720128_1_ir_cal.cub");
  QString mapFile = testDataPath("tsts/combination1/input/v1496720128_1_ir_cal.lev2.cub");
  QString outputFile = tempDir.path() + "/vimsIR.cub";

  QStringList args = {
    "from=" + inputFile,
    "map=" + mapFile,
    "to=" + outputFile,
    "pixres=map",
    "defaultrange=camera",
    "ringlonseam=error",
    "trim=yes",
    "interp=bilinear",
    "warpalgorithm=reversepatch",
    "patchsize=5"
  };

  RingscamResult result = runRingscam2map(args);

  EXPECT_EQ(result.exitCode, 0) << "stderr: " << result.stderr.toStdString();
  ASSERT_TRUE(QFile::exists(outputFile));

  Cube output(outputFile);

  // Validate output dimensions (VIMS IR with trim=yes)
  EXPECT_EQ(output.sampleCount(), 114);
  EXPECT_EQ(output.lineCount(), 95);
  EXPECT_EQ(output.bandCount(), 256);

  // Validate Mapping group
  Pvl *label = output.label();
  PvlObject &cube = label->findObject("IsisCube");
  ASSERT_TRUE(cube.hasGroup("Mapping")) << "Missing Mapping group in combination1 output";

  PvlGroup &mapping = cube.findGroup("Mapping");
  EXPECT_TRUE(mapping.hasKeyword("ProjectionName"));
  EXPECT_TRUE(mapping.hasKeyword("TargetName"));
  EXPECT_EQ(mapping["TargetName"][0].toStdString(), "Saturn");
}

// Test Combination2: pixres=mpp resolution=1703.2815626544, defaultrange=minimize, etc.
TEST_F(TempTestingFiles, FunctionalTestRingscam2mapCombination2) {
  QString inputFile = testDataPath("tsts/combination2/input/N1495328431_1_ct.cub");
  QString mapFile = testDataPath("tsts/combination2/input/planar.map");
  QString outputFile = tempDir.path() + "/issNAC.cub";

  QStringList args = {
    "from=" + inputFile,
    "map=" + mapFile,
    "to=" + outputFile,
    "pixres=mpp",
    "resolution=1703.2815626544",
    "defaultrange=minimize",
    "ringlonseam=continue",
    "interp=nearestneighbor",
    "warpalgorithm=forwardpatch",
    "patchsize=4"
  };

  RingscamResult result = runRingscam2map(args);

  EXPECT_EQ(result.exitCode, 0) << "stderr: " << result.stderr.toStdString();
  ASSERT_TRUE(QFile::exists(outputFile));

  Cube output(outputFile);

  // Validate output dimensions (ISS NAC with specified resolution)
  EXPECT_EQ(output.sampleCount(), 4257);
  EXPECT_EQ(output.lineCount(), 2953);
  EXPECT_EQ(output.bandCount(), 1);

  // Validate Mapping group
  Pvl *label = output.label();
  PvlObject &cube = label->findObject("IsisCube");
  ASSERT_TRUE(cube.hasGroup("Mapping")) << "Missing Mapping group in combination2 output";

  PvlGroup &mapping = cube.findGroup("Mapping");
  EXPECT_TRUE(mapping.hasKeyword("ProjectionName"));
  EXPECT_TRUE(mapping.hasKeyword("TargetName"));
  EXPECT_EQ(mapping["TargetName"][0].toStdString(), "Saturn");

  // Validate specified resolution was used
  ASSERT_TRUE(mapping.hasKeyword("PixelResolution"));
  double pixelRes = toDouble(mapping["PixelResolution"][0]);
  EXPECT_NEAR(pixelRes, 1703.2815626544, 0.01);
}

// Test Combination3: pixres=ppd resolution=23.7, defaultrange=map with explicit range
TEST_F(TempTestingFiles, FunctionalTestRingscam2mapCombination3) {
  QString inputFile = testDataPath("tsts/combination3/input/W1591159850_1_cal.cub");
  QString mapFile = testDataPath("tsts/combination3/input/planar.map");
  QString outputFile = tempDir.path() + "/issWAC.cub";

  QStringList args = {
    "from=" + inputFile,
    "map=" + mapFile,
    "to=" + outputFile,
    "pixres=ppd",
    "resolution=23.7",
    "defaultrange=map",
    "minringrad=87899725.492461",
    "maxringrad=165123259.31231",
    "minringlon=-180.0",
    "maxringlon=-139.0",
    "interp=nearestneighbor"
  };

  RingscamResult result = runRingscam2map(args);

  EXPECT_EQ(result.exitCode, 0) << "stderr: " << result.stderr.toStdString();
  ASSERT_TRUE(QFile::exists(outputFile));

  Cube output(outputFile);

  // Validate output dimensions (ISS WAC with explicit range)
  EXPECT_EQ(output.sampleCount(), 1061);
  EXPECT_EQ(output.lineCount(), 1163);
  EXPECT_EQ(output.bandCount(), 1);

  // Validate Mapping group
  Pvl *label = output.label();
  PvlObject &cube = label->findObject("IsisCube");
  ASSERT_TRUE(cube.hasGroup("Mapping")) << "Missing Mapping group in combination3 output";

  PvlGroup &mapping = cube.findGroup("Mapping");
  EXPECT_TRUE(mapping.hasKeyword("ProjectionName"));
  EXPECT_TRUE(mapping.hasKeyword("TargetName"));
  EXPECT_EQ(mapping["TargetName"][0].toStdString(), "Saturn");

  // Validate explicit ring range was respected
  EXPECT_TRUE(mapping.hasKeyword("MinimumRingRadius"));
  EXPECT_TRUE(mapping.hasKeyword("MaximumRingRadius"));
  EXPECT_TRUE(mapping.hasKeyword("MinimumRingLongitude"));
  EXPECT_TRUE(mapping.hasKeyword("MaximumRingLongitude"));
}

// Test Combination4: Multiple tests with various parameter combinations
TEST_F(TempTestingFiles, FunctionalTestRingscam2mapCombination4Test1) {
  QString inputFile = testDataPath("tsts/combination4/input/v1496720128_1_vis_cal.cub");
  QString mapFile = testDataPath("tsts/combination4/input/planar.map");
  QString outputFile = tempDir.path() + "/vimsVIS_1.cub";

  QStringList args = {
    "from=" + inputFile,
    "map=" + mapFile,
    "to=" + outputFile,
    "pixres=camera",
    "defaultrange=camera",
    "ringlonseam=auto",
    "warpalgorithm=forwardpatch",
    "patchsize=1"
  };

  RingscamResult result = runRingscam2map(args);

  EXPECT_EQ(result.exitCode, 0) << "stderr: " << result.stderr.toStdString();
  ASSERT_TRUE(QFile::exists(outputFile));

  Cube output(outputFile);

  // Validate output dimensions (pixres=camera, forwardpatch)
  EXPECT_EQ(output.sampleCount(), 109);
  EXPECT_EQ(output.lineCount(), 91);
  EXPECT_EQ(output.bandCount(), 96);

  // Validate Mapping group
  Pvl *label = output.label();
  PvlObject &cube = label->findObject("IsisCube");
  ASSERT_TRUE(cube.hasGroup("Mapping")) << "Missing Mapping group in combination4test1 output";

  PvlGroup &mapping = cube.findGroup("Mapping");
  EXPECT_TRUE(mapping.hasKeyword("ProjectionName"));
  EXPECT_TRUE(mapping.hasKeyword("TargetName"));
}

TEST_F(TempTestingFiles, FunctionalTestRingscam2mapCombination4Test2) {
  QString inputFile = testDataPath("tsts/combination4/input/v1496720128_1_vis_cal.cub");
  QString mapFile = testDataPath("tsts/combination4/input/planar.map");
  QString outputFile = tempDir.path() + "/vimsVIS_2.cub";

  QStringList args = {
    "from=" + inputFile,
    "map=" + mapFile,
    "to=" + outputFile,
    "pixres=mpp",
    "resolution=646251.0",
    "defaultrange=minimize",
    "ringlonseam=continue",
    "warpalgorithm=reversepatch",
    "patchsize=3"
  };

  RingscamResult result = runRingscam2map(args, 300000, true);  // Capture output to debug

  if (result.exitCode != 0) {
    FAIL() << "ringscam2map failed with exit code " << result.exitCode
           << "\nstdout: " << result.stdout.toStdString()
           << "\nstderr: " << result.stderr.toStdString();
  }

  ASSERT_TRUE(QFile::exists(outputFile));

  Cube output(outputFile);

  // Validate output dimensions (specified mpp resolution, reversepatch)
  EXPECT_EQ(output.sampleCount(), 104);
  EXPECT_EQ(output.lineCount(), 93);
  EXPECT_EQ(output.bandCount(), 96);

  // Validate Mapping group
  Pvl *label = output.label();
  PvlObject &cube = label->findObject("IsisCube");
  ASSERT_TRUE(cube.hasGroup("Mapping")) << "Missing Mapping group in combination4test2 output";

  PvlGroup &mapping = cube.findGroup("Mapping");
  EXPECT_TRUE(mapping.hasKeyword("ProjectionName"));
  EXPECT_TRUE(mapping.hasKeyword("TargetName"));

  // Validate specified resolution was used
  ASSERT_TRUE(mapping.hasKeyword("PixelResolution"));
  double pixelRes = toDouble(mapping["PixelResolution"][0]);
  EXPECT_NEAR(pixelRes, 646251.0, 1.0);
}

TEST_F(TempTestingFiles, FunctionalTestRingscam2mapCombination4Test3) {
  QString inputFile = testDataPath("tsts/combination4/input/v1496720128_1_vis_cal.cub");
  QString mapFile = testDataPath("tsts/combination4/input/planar.map");
  QString outputFile = tempDir.path() + "/vimsVIS_3.cub";

  QStringList args = {
    "from=" + inputFile,
    "map=" + mapFile,
    "to=" + outputFile,
    "pixres=ppd",
    "resolution=0.5",
    "defaultrange=camera"
  };

  RingscamResult result = runRingscam2map(args);

  EXPECT_EQ(result.exitCode, 0) << "stderr: " << result.stderr.toStdString();
  ASSERT_TRUE(QFile::exists(outputFile));

  Cube output(outputFile);

  // Validate output dimensions (ppd resolution=0.5)
  EXPECT_EQ(output.sampleCount(), 16);
  EXPECT_EQ(output.lineCount(), 13);
  EXPECT_EQ(output.bandCount(), 96);

  // Validate Mapping group
  Pvl *label = output.label();
  PvlObject &cube = label->findObject("IsisCube");
  ASSERT_TRUE(cube.hasGroup("Mapping")) << "Missing Mapping group in combination4test3 output";

  PvlGroup &mapping = cube.findGroup("Mapping");
  EXPECT_TRUE(mapping.hasKeyword("ProjectionName"));
  EXPECT_TRUE(mapping.hasKeyword("TargetName"));

  // Validate Scale keyword for ppd resolution
  EXPECT_TRUE(mapping.hasKeyword("Scale"));
}

// Error Test: Sky target should throw error
TEST_F(TempTestingFiles, FunctionalTestRingscam2mapErrorSkyTarget) {
  QString inputFile = testDataPath("tsts/errors/input/targetSky.cub");
  QString mapFile = testDataPath("tsts/errors/input/planarCassiniVIMS.map");
  QString outputFile = tempDir.path() + "/error.cub";

  QStringList args = {
    "from=" + inputFile,
    "map=" + mapFile,
    "to=" + outputFile
  };

  RingscamResult result = runRingscam2map(args, 300000, true);  // Capture output for error checking

  EXPECT_NE(result.exitCode, 0) << "Expected non-zero exit code for sky target error";
  QString output = result.stdout + result.stderr;
  EXPECT_TRUE(output.contains("sky", Qt::CaseInsensitive) ||
              output.contains("skymap", Qt::CaseInsensitive))
    << "Expected 'sky' or 'skymap' in error message, got: " << output.toStdString();
}

// Error Test: Seam crossing with ringlonseam=error should throw
TEST_F(TempTestingFiles, FunctionalTestRingscam2mapErrorSeam) {
  QString inputFile = testDataPath("tsts/errors/input/seam.cub");
  QString mapFile = testDataPath("tsts/errors/input/seam.map");
  QString outputFile = tempDir.path() + "/error.cub";

  QStringList args = {
    "from=" + inputFile,
    "map=" + mapFile,
    "to=" + outputFile,
    "ringlonseam=error",
    "defaultrange=camera"
  };

  RingscamResult result = runRingscam2map(args, 300000, true);  // Capture output for error checking

  EXPECT_NE(result.exitCode, 0) << "Expected non-zero exit code for seam crossing error";
  QString output = result.stdout + result.stderr;
  EXPECT_TRUE(output.contains("seam", Qt::CaseInsensitive) ||
              output.contains("longitude", Qt::CaseInsensitive))
    << "Expected 'seam' or 'longitude' in error message, got: " << output.toStdString();
}

// Error Test: Not a ring plane should throw error
TEST_F(TempTestingFiles, FunctionalTestRingscam2mapErrorNotRingPlane) {
  QString inputFile = testDataPath("tsts/errors/input/notRingPlane.cub");
  QString mapFile = testDataPath("tsts/errors/input/sinusoidal.map");
  QString outputFile = tempDir.path() + "/error.cub";

  QStringList args = {
    "from=" + inputFile,
    "map=" + mapFile,
    "to=" + outputFile
  };

  RingscamResult result = runRingscam2map(args);

  // This should fail because the cube is not spiceinit'd with ringplane shape
  EXPECT_NE(result.exitCode, 0) << "Expected non-zero exit code for non-ring-plane error";
  // The error message may vary depending on how spice is initialized
  EXPECT_FALSE(result.stderr.isEmpty()) << "Expected error message in stderr";
}
