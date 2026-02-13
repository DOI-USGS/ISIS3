// SPDX-License-Identifier: CC0-1.0

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>
#include <QTemporaryDir>
#include <QVector>
#include <QIODevice>

#include "FileName.h"
#include "IException.h"
#include "Preference.h"
#include "UserInterface.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlObject.h"
#include "PvlKeyword.h"

#include "gdal.h"
#include "cpl_conv.h"

#include "equalizer.h"

#include "gtest/gtest.h"

using namespace Isis;

namespace {

static const QString EQUALIZER_XML =
    FileName("$ISISROOT/bin/xml/equalizer.xml").expanded();

QString joinPath(const QString &a, const QString &b) {
  return QDir(a).filePath(b);
}

void copyFileOrFail(const QString &src, const QString &dst) {
  QFileInfo dstInfo(dst);
  QDir().mkpath(dstInfo.absolutePath());
  QFile::remove(dst);

  ASSERT_TRUE(QFile::copy(src, dst))
      << "Failed to copy " << src.toStdString()
      << " -> " << dst.toStdString();

  ASSERT_TRUE(QFileInfo::exists(dst))
      << "Missing after copy: " << dst.toStdString();
}

void writeListFile(const QString &path, const QStringList &lines) {
  QFile f(path);
  ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text))
      << "Failed to write list file: " << path.toStdString();

  for (const QString &l : lines) {
    f.write((l + "\n").toUtf8());
  }
}

QStringList readListFile(const QString &path) {
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    ADD_FAILURE() << "Failed to read list file: " << path.toStdString();
    return {};
  }

  QStringList lines;
  while (!f.atEnd()) {
    const QString line = QString::fromUtf8(f.readLine()).trimmed();
    if (line.isEmpty() || line.startsWith("#")) continue;
    lines.append(line);
  }
  return lines;
}


void initGdalOrFail() {
  // Ensure drivers are registered inside the test process (ctest env can differ from your shell).
  GDALAllRegister();
  ASSERT_GT(GDALGetDriverCount(), 0) << "GDAL has 0 registered drivers in-process";
}

PvlGroup findNormalizationGroupByBaseName(const PvlObject &root, const QString &baseName) {
  for (int i = 0; i < root.groups(); i++) {
    const PvlGroup &g = root.group(i);
    if (g.name() != "Normalization") continue;
    if (!g.hasKeyword("FileName")) continue;

    const QString fn = g["FileName"][0];
    if (fn.endsWith("/" + baseName) || fn.endsWith("\\" + baseName) || fn == baseName) {
      return g;
    }
  }

  ADD_FAILURE() << "Missing Normalization group for " << baseName.toStdString();
  return PvlGroup("Normalization");
}

bool isFinite(double x) {
  return std::isfinite(x);
}

void expectBandTripletWellFormed(const PvlGroup &norm, const QString &bandKey) {
  ASSERT_TRUE(norm.hasKeyword(bandKey))
      << "Missing " << bandKey.toStdString();

  const PvlKeyword &kw = norm.findKeyword(bandKey);
  ASSERT_EQ(kw.size(), 3) << bandKey.toStdString() << " must have 3 values";

  const double gain   = kw[0].toDouble();
  const double offset = kw[1].toDouble();
  const double avg    = kw[2].toDouble();

  EXPECT_TRUE(isFinite(gain))   << bandKey.toStdString() << " gain not finite";
  EXPECT_TRUE(isFinite(offset)) << bandKey.toStdString() << " offset not finite";
  EXPECT_TRUE(isFinite(avg))    << bandKey.toStdString() << " avg not finite";

  // Functional sanity: gain should be positive for a meaningful normalization.
  EXPECT_GT(gain, 0.0) << bandKey.toStdString() << " gain must be > 0";
}

}  // namespace

TEST(Equalizer, NonOverlapRetryBoth) {
  Preference::Preferences(true);
  initGdalOrFail();

  const QString dataBase =
      QString(_SOURCE_PREFIX) + "/data/equalizer/nonOverlapRetryBoth";

  const QStringList allCubes = {
    "I10047011EDR.proj.reduced.cub",
    "I25685003EDR.crop.proj.reduced.cub",
    "I51718010EDR.crop.proj.reduced.cub",
    "I56969027EDR.proj.reduced.cub",
    "I50695002EDR.proj.reduced.cub"
  };

  const QStringList nonOverlap = {
    "I10047011EDR.proj.reduced.cub",
    "I25685003EDR.crop.proj.reduced.cub",
    "I51718010EDR.crop.proj.reduced.cub",
    "I56969027EDR.proj.reduced.cub"
  };

  // --- Working directory ---
  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  const QString work      = tempDir.path();
  const QString inputAbs  = joinPath(work, "input");
  const QString outputAbs = joinPath(work, "output");
  QDir().mkpath(inputAbs);
  QDir().mkpath(outputAbs);

  // --- Stage inputs ---
  for (const auto &n : allCubes) {
    copyFileOrFail(joinPath(dataBase, n), joinPath(inputAbs, n));
  }

  copyFileOrFail(joinPath(dataBase, "toList.lis"),
                 joinPath(inputAbs, "toList.lis"));

  const QStringList toListNames = readListFile(joinPath(inputAbs, "toList.lis"));
  ASSERT_EQ(toListNames.size(), 5);

  // --- Write lists relative to output/ (matches legacy intent) ---
  {
    QStringList rel;
    for (const auto &n : nonOverlap) rel.append("../input/" + n);
    writeListFile(joinPath(outputAbs, "nonOverlap.lis"), rel);
  }
  {
    QStringList rel;
    for (const auto &n : allCubes) rel.append("../input/" + n);
    writeListFile(joinPath(outputAbs, "fixed.lis"), rel);
  }

  // Run from output/ so list files resolve.
  ASSERT_TRUE(QDir::setCurrent(outputAbs));

  // -------- Phase A: expect failure on non-overlaps --------
  {
    QVector<QString> args = {
      "fromlist=nonOverlap.lis",
      "outstats=nonOverlapStats.pvl",
      "process=CALCULATE"
    };

    try {
      UserInterface ui(EQUALIZER_XML, args);
      equalizer(ui);
      FAIL() << "Expected CALCULATE to throw on non-overlaps";
    }
    catch (IException &) {
      // expected
    }
  }

  // Legacy produced an output stats file even on failure; keep this check lightweight.
  EXPECT_TRUE(QFileInfo::exists("nonOverlapStats.pvl"));

  // -------- Phase B: use checked-in truth instats (approved approach) --------
  {
    const QString truthInstats = joinPath(dataBase, "nonOverlapStats.pvl");
    ASSERT_TRUE(QFileInfo::exists(truthInstats))
        << "Missing instats truth file: " << truthInstats.toStdString();

    QVector<QString> args = {
      "fromlist=fixed.lis",
      "tolist=../input/toList.lis",
      "instats=" + truthInstats,
      "outstats=recalculatedStats.pvl",
      "process=RETRYBOTH"
    };

    UserInterface ui(EQUALIZER_XML, args);
    ASSERT_NO_THROW(equalizer(ui));
  }

  ASSERT_TRUE(QFileInfo::exists("recalculatedStats.pvl"));

  // -------- Validate PVL invariants (functional correctness only) --------
  Pvl stats("recalculatedStats.pvl");
  ASSERT_TRUE(stats.hasObject("EqualizationInformation"));

  PvlObject eq = stats.findObject("EqualizationInformation");
  ASSERT_TRUE(eq.hasGroup("General"));
  PvlGroup general = eq.findGroup("General");

  EXPECT_EQ(general["TotalOverlaps"][0].toInt(), 40);
  EXPECT_EQ(general["ValidOverlaps"][0].toInt(), 40);
  EXPECT_EQ(general["InvalidOverlaps"][0].toInt(), 0);
  EXPECT_EQ(general["MinCount"][0].toInt(), 1000);
  EXPECT_NEAR(general["SamplingPercent"][0].toDouble(), 100.0, 1e-12);
  EXPECT_EQ(general["SolutionType"][0].toInt(), 2);
  EXPECT_EQ(general["Weighted"][0].toLower(), QString("false"));
  EXPECT_EQ(general["HasCorrections"][0].toLower(), QString("true"));

  // Normalizations exist and are well-formed for each cube.
  for (const auto &cube : allCubes) {
    PvlGroup n = findNormalizationGroupByBaseName(eq, cube);

    // Must be present and resolvable back to the cube.
    ASSERT_TRUE(n.hasKeyword("FileName"));
    const QString fn = n["FileName"][0];
    EXPECT_TRUE(fn.endsWith(cube)) << "Normalization FileName mismatch: " << fn.toStdString();

    for (int band = 1; band <= 8; band++) {
      expectBandTripletWellFormed(n, "Band" + QString::number(band));
    }
  }

  // Outputs listed in toList.lis should exist (legacy mv *.cub to OUTPUT).
  for (const auto &outName : toListNames) {
    EXPECT_TRUE(QFileInfo::exists(joinPath(outputAbs, outName)))
        << "Missing output cube: " << outName.toStdString();
  }
}

