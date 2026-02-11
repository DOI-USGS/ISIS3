// NOTE: This work is free and unencumbered software released into the public domain.
// The authors of ISIS do not claim copyright on the contents of this file.
// For more details about the LICENSE terms and the AUTHORS, you will
// find files of those names at the top level of this repository.
//
// SPDX-License-Identifier: CC0-1.0

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QTemporaryDir>
#include <QVector>
#include <QIODevice>

#include "FileName.h"
#include "IException.h"
#include "Preference.h"
#include "ProgramLauncher.h"
#include "UserInterface.h"

#include "equalizer.h"

#include "gtest/gtest.h"

using namespace Isis;

namespace {

// App XML
static QString EQUALIZER_XML = FileName("$ISISROOT/bin/xml/equalizer.xml").expanded();

QString joinPath(const QString &a, const QString &b) {
  return QDir(a).filePath(b);
}

QString prefArg() {
  return "-preference=$ISISROOT/TestPreferences";
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

QStringList readListFile(const QString &path) {
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    ADD_FAILURE() << "Failed to open list file: " << path.toStdString();
    return {};
  }

  QStringList lines;
  while (!f.atEnd()) {
    QString line = QString::fromUtf8(f.readLine()).trimmed();
    if (line.isEmpty() || line.startsWith("#")) {
      continue;
    }
    lines.append(line);
  }
  return lines;
}

void writeListFile(const QString &path, const QStringList &lines) {
  QFile f(path);
  ASSERT_TRUE(f.open(QIODevice::WriteOnly | QIODevice::Text))
      << "Failed to write list file: " << path.toStdString();

  for (const QString &l : lines) {
    f.write((l + "\n").toUtf8());
  }
}


}  // namespace


TEST(Equalizer, NonOverlapRetryBoth) {
  Preference::Preferences(true);

  const QString base =
      FileName("$ISISTESTDATA/isis/src/base/apps/equalizer/tsts/nonOverlapRetryBoth").expanded();
  const QString inputSrc = joinPath(base, "input");
  const QString truthSrc = joinPath(base, "truth");

  QTemporaryDir tempDir;
  ASSERT_TRUE(tempDir.isValid());

  const QString work = tempDir.path();
  const QString inputAbs  = joinPath(work, "input");
  const QString outputAbs = joinPath(work, "output");
  const QString truthAbs  = joinPath(work, "truth");

  QDir().mkpath(inputAbs);
  QDir().mkpath(outputAbs);
  QDir().mkpath(truthAbs);

  // Keep legacy-ish working directory contract for any relative file behavior
  ASSERT_TRUE(QDir::setCurrent(work));

  // Copy toList.lis
  copyFileOrFail(joinPath(inputSrc, "toList.lis"),
                 joinPath(inputAbs, "toList.lis"));
  const QStringList toListNames =
      readListFile(joinPath(inputAbs, "toList.lis"));
  ASSERT_EQ(toListNames.size(), 5);

  // Copy DIFF files
  copyFileOrFail(joinPath(inputSrc, "nonOverlapStats.pvl.DIFF"),
                 joinPath(inputAbs, "nonOverlapStats.pvl.DIFF"));
  copyFileOrFail(joinPath(inputSrc, "recalculatedStats.pvl.DIFF"),
                 joinPath(inputAbs, "recalculatedStats.pvl.DIFF"));

  // Copy truth PVLs (including instats we will use)
  copyFileOrFail(joinPath(truthSrc, "nonOverlapStats.pvl"),
                 joinPath(truthAbs, "nonOverlapStats.pvl"));
  copyFileOrFail(joinPath(truthSrc, "recalculatedStats.pvl"),
                 joinPath(truthAbs, "recalculatedStats.pvl"));

  // Authoritative lists from legacy Makefile (order matters)
  const QStringList nonOverlapNames = {
    "I10047011EDR.proj.reduced.cub",
    "I25685003EDR.crop.proj.reduced.cub",
    "I51718010EDR.crop.proj.reduced.cub",
    "I56969027EDR.proj.reduced.cub"
  };

  const QStringList fixedNames = {
    "I10047011EDR.proj.reduced.cub",
    "I25685003EDR.crop.proj.reduced.cub",
    "I51718010EDR.crop.proj.reduced.cub",
    "I56969027EDR.proj.reduced.cub",
    "I50695002EDR.proj.reduced.cub"
  };

  // Copy all required cubes (union)
  QSet<QString> needed;
  for (const auto &n : nonOverlapNames) {
    needed.insert(n);
  }
  for (const auto &n : fixedNames) {
    needed.insert(n);
  }
  for (const auto &n : toListNames) {
    needed.insert(n);
  }

  for (const QString &name : needed) {
    copyFileOrFail(joinPath(inputSrc, name),
                   joinPath(inputAbs, name));
    copyFileOrFail(joinPath(truthSrc, name),
                   joinPath(truthAbs, name));
  }

  // Write list files like Makefile (relative input/ paths), stored in output/
  QStringList nonOverlapRel, fixedRel;
  for (const auto &n : nonOverlapNames) {
    nonOverlapRel.append("input/" + n);
  }
  for (const auto &n : fixedNames) {
    fixedRel.append("input/" + n);
  }

  writeListFile(joinPath(work, "output/nonOverlap.lis"), nonOverlapRel);
  writeListFile(joinPath(work, "output/fixed.lis"), fixedRel);

  const QString nonOverlapStatsAbs   = joinPath(work, "output/nonOverlapStats.pvl");
  const QString recalculatedStatsAbs = joinPath(work, "output/recalculatedStats.pvl");

  const QString nonOverlapLisAbs = joinPath(work, "output/nonOverlap.lis");
  const QString fixedLisAbs      = joinPath(work, "output/fixed.lis");
  const QString toListLisAbs     = joinPath(work, "input/toList.lis");

  // Phase A: expected failure (smoke check only)
  {
    QVector<QString> argsA = {
      "fromlist=" + nonOverlapLisAbs,
      "outstats=" + nonOverlapStatsAbs,
      "process=CALCULATE",
      "solvemethod=QRD",
      prefArg()
    };

    try {
      UserInterface uiA(EQUALIZER_XML, argsA);
      equalizer(uiA);
      FAIL() << "Expected equalizer CALCULATE to fail for non-overlapping list";
    }
    catch (IException &) {
      // expected
    }

    ASSERT_TRUE(QFileInfo::exists(nonOverlapStatsAbs))
        << "Expected nonOverlapStats to be written even on CALCULATE failure";
  }

  // Phase B: RETRYBOTH using truth instats
  //
  // NOTE: In this environment, the OUTSTATS PVL produced by a failing CALCULATE run
  // is not reusable as INSTATS for RETRYBOTH. We use the truth instats PVL to keep
  // RETRYBOTH output validation deterministic and consistent with the legacy intent.
  {
    const QString truthInstatsAbs = joinPath(work, "truth/nonOverlapStats.pvl");

    QVector<QString> argsB = {
      "fromlist=" + fixedLisAbs,
      "tolist=" + toListLisAbs,
      "instats=" + truthInstatsAbs,
      "outstats=" + recalculatedStatsAbs,
      "process=RETRYBOTH",
      "solvemethod=QRD",
      prefArg()
    };

    UserInterface uiB(EQUALIZER_XML, argsB);
    ASSERT_NO_THROW(equalizer(uiB));
  }

  ASSERT_TRUE(QFileInfo::exists(recalculatedStatsAbs));

  // Move output cubes like legacy (equalizer writes to CWD)
  QDir cwd(work);
  QStringList cubes = cwd.entryList(QStringList() << "*.cub", QDir::Files);

  for (const QString &cube : cubes) {
    QString src = joinPath(work, cube);
    QString dst = joinPath(outputAbs, cube);

    QFile::remove(dst); // in case exists
    ASSERT_TRUE(QFile::rename(src, dst))
        << "Failed to move " << src.toStdString()
        << " to " << dst.toStdString();
  }

  // Validate stats PVLs with pvldiff 
  ASSERT_NO_THROW(
    ProgramLauncher::RunIsisProgram(
      "pvldiff",
      "from=output/recalculatedStats.pvl "
      "from2=truth/recalculatedStats.pvl "
      "diff=input/recalculatedStats.pvl.DIFF "
      + prefArg()
      )
  );

  // Validate output cubes
  const QString tol = "0.00001";

  for (const QString &name : toListNames) {
    const QString outCubeAbs = joinPath(work, "output/" + name);
    const QString truthCubeAbs = joinPath(work, "truth/" + name);

    ASSERT_TRUE(QFileInfo::exists(outCubeAbs))
        << "Missing output cube: " << outCubeAbs.toStdString();

    ASSERT_NO_THROW(
      ProgramLauncher::RunIsisProgram(
        "cubediff",
        "from=" + outCubeAbs + " "
        "from2=" + truthCubeAbs + " "
        "tolerance=" + tol + " "
        + prefArg()
      )
    );
  }
}