#include "BundleSettings.h"

#include <algorithm>

#include <QString>
#include <QList>

#include "BundleObservationSolveSettings.h"

#include <gtest/gtest.h>

bool observationSettingsComparison(
      BundleObservationSolveSettings a,
      BundleObservationSolveSettings b) {
  return (a.instrumentId() == b.instrumentId());
}

class BoolTest : public ::testing::TestWithParam<bool> {
  // Intentionally empty
};

class CoordinateTypeTest : public ::testing::TestWithParam<SurfacePoint::CoordinateType> {
  // Intentionally empty
};

TEST_P(BoolTest, validateNetwork) {
  BundleSettings testSettings;
  testSettings.setValidateNetwork(GetParam());
  EXPECT_EQ(GetParam(), testSettings.validateNetwork());
}

TEST_P(BoolTest, outlierRejection) {
  BundleSettings testSettings;
  testSettings.setOutlierRejection(GetParam());
  EXPECT_EQ(GetParam(), testSettings.outlierRejection());
}

TEST_P(BoolTest, inverseMatrix) {
  BundleSettings testSettings;
  testSettings.setCreateInverseMatrix(GetParam());
  EXPECT_EQ(GetParam(), testSettings.createInverseMatrix());
}

TEST_P(BoolTest, setBoolSolveOptions) {
  BundleSettings testSettings;
  testSettings.setSolveOptions(
        GetParam(),
        GetParam(),
        GetParam(),
        GetParam());
  EXPECT_EQ(GetParam(), testSettings.solveObservationMode());
  EXPECT_EQ(GetParam(), testSettings.updateCubeLabel());
  EXPECT_EQ(GetParam(), testSettings.errorPropagation());
  EXPECT_EQ(GetParam(), testSettings.solveRadius());
}

INSTANTIATE_TEST_CASE_P(
      BundleSettings,
      BoolTest,
      ::testing::Bool());

TEST_P(CoordinateTypeTest, setCoordinateTypeSolveOptions) {
  BundleSettings testSettings;
  testSettings.setSolveOptions(
        false,
        false,
        false,
        false,
        GetParam(),
        GetParam());
  EXPECT_EQ(GetParam(), testSettings.controlPointCoordTypeReports());
  EXPECT_EQ(GetParam(), testSettings.controlPointCoordTypeBundle());
}

INSTANTIATE_TEST_CASE_P(
      BundleSettings,
      CoordinateTypeTest,
      ::testing::Values(SurfacePoint::Latitudinal, SurfacePoint::Rectangular));

TEST(BundleSettings, outlierRejectionMultiplier) {
  BundleSettings testSettings;
  testSettings.setOutlierRejection(true, 8.0);
  EXPECT_EQ(8.0, testSettings.outlierRejectionMultiplier());
}

TEST(BundleSettings, observationSolveSettings) {
  BundleObservationSolveSettings firstObsSettings;
  BundleObservationSolveSettings secondObsSettings;
  QString firstInstrument("First Instrument");
  QString secondInstrument("Second Instrument");
  QString firstObservationNumber("First Observation");
  QString secondObservationNumber("Second Observation");
  firstObsSettings.setInstrumentId(firstInstrument);
  secondObsSettings.setInstrumentId(secondInstrument);
  firstObsSettings.addObservationNumber(firstObservationNumber);
  secondObsSettings.addObservationNumber(secondObservationNumber);
  QList<BundleObservationSolveSettings> optionsList = {
        firstObsSettings,
        secondObsSettings};

  BundleSettings testSettings;
  testSettings.setObservationSolveOptions(optionsList);

  EXPECT_EQ(testSettings.numberSolveSettings(), optionsList.size());
  EXPECT_TRUE(std::equal(
        optionsList.begin(),
        optionsList.end(),
        testSettings.observationSolveSettings().begin(),
        observationSettingsComparison));
  EXPECT_TRUE(observationSettingsComparison(
        testSettings.observationSolveSettings(secondObservationNumber),
        secondObsSettings));
  EXPECT_TRUE(observationSettingsComparison(
        testSettings.observationSolveSettings(1),
        secondObsSettings));
}
