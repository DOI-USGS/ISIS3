#include "BundleSettings.h"

#include <algorithm>

#include <QString>
#include <QList>
#include <QPair>

#include "BundleObservationSolveSettings.h"
#include "BundleTargetBody.h"
#include "IException.h"
#include "MaximumLikelihoodWFunctions.h"

#include "gmock/gmock.h"

bool observationSettingsComparison(
      BundleObservationSolveSettings m,
      BundleObservationSolveSettings n
) {
  return (m.instrumentId() == n.instrumentId());
}

::testing::AssertionResult assertObservationSettingsEqual(
      const char* m_expr,
      const char* n_expr,
      BundleObservationSolveSettings m,
      BundleObservationSolveSettings n
) {
  if (observationSettingsComparison(m, n)) return ::testing::AssertionSuccess();

  return ::testing::AssertionFailure() << m_expr << " and " << n_expr
        << " are different because they have different InstrumentIds ("
        << m.instrumentId().toStdString() << " and "
        << n.instrumentId().toStdString() << ")";
}

class MockBundleTargetBody : public BundleTargetBody {
  public:
    MOCK_METHOD0(numberParameters, int());
    MOCK_METHOD0(solvePoleRA, bool());
    MOCK_METHOD0(solvePoleRAVelocity, bool());
    MOCK_METHOD0(solvePoleDec, bool());
    MOCK_METHOD0(solvePoleDecVelocity, bool());
    MOCK_METHOD0(solvePM, bool());
    MOCK_METHOD0(solvePMVelocity, bool());
    MOCK_METHOD0(solvePMAcceleration, bool());
    MOCK_METHOD0(solveTriaxialRadii, bool());
    MOCK_METHOD0(solveMeanRadius, bool());
};

class BoolTest : public ::testing::TestWithParam<bool> {
  // Intentionally empty
};

class CoordinateTypeTest : public ::testing::TestWithParam<SurfacePoint::CoordinateType> {
  // Intentionally empty
};

class ConvergenceCriteriaTest : public ::testing::TestWithParam<BundleSettings::ConvergenceCriteria> {
  // Intentionally empty
};

class MaximumLikelihoodFunctionTest : public ::testing::TestWithParam<MaximumLikelihoodWFunctions::Model> {
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
  testSettings.setSolveOptions(
        testSettings.solveObservationMode(),
        testSettings.updateCubeLabel(),
        true,
        testSettings.solveRadius()
  );
  testSettings.setCreateInverseMatrix(GetParam());
  EXPECT_EQ(GetParam(), testSettings.createInverseMatrix());
}

TEST_P(BoolTest, setBoolSolveOptions) {
  BundleSettings testSettings;
  testSettings.setSolveOptions(
        GetParam(),
        GetParam(),
        GetParam(),
        GetParam()
  );
  EXPECT_EQ(GetParam(), testSettings.solveObservationMode());
  EXPECT_EQ(GetParam(), testSettings.updateCubeLabel());
  EXPECT_EQ(GetParam(), testSettings.errorPropagation());
  EXPECT_EQ(GetParam(), testSettings.solveRadius());
}

INSTANTIATE_TEST_CASE_P(
      BundleSettings,
      BoolTest,
      ::testing::Bool()
);

TEST_P(CoordinateTypeTest, setCoordinateTypeSolveOptions) {
  BundleSettings testSettings;
  testSettings.setSolveOptions(
        testSettings.solveObservationMode(),
        testSettings.updateCubeLabel(),
        testSettings.errorPropagation(),
        testSettings.solveRadius(),
        GetParam(),
        GetParam()
  );
  EXPECT_EQ(GetParam(), testSettings.controlPointCoordTypeReports());
  EXPECT_EQ(GetParam(), testSettings.controlPointCoordTypeBundle());
}

INSTANTIATE_TEST_CASE_P(
      BundleSettings,
      CoordinateTypeTest,
      ::testing::Values(SurfacePoint::Latitudinal, SurfacePoint::Rectangular)
);

TEST(BundleSettings, setGlobalSigmas) {
  BundleSettings testSettings;
  testSettings.setSolveOptions(
    testSettings.solveObservationMode(),
    testSettings.updateCubeLabel(),
    testSettings.errorPropagation(),
    true,
    testSettings.controlPointCoordTypeBundle(),
    testSettings.controlPointCoordTypeReports(),
    2.0,
    8.0,
    32.0
  );

  EXPECT_EQ(2.0, testSettings.globalPointCoord1AprioriSigma());
  EXPECT_EQ(8.0, testSettings.globalPointCoord2AprioriSigma());
  EXPECT_EQ(32.0, testSettings.globalPointCoord3AprioriSigma());
}

TEST(BundleSettings, setBadGlobalSigmas) {
  BundleSettings testSettings;
  testSettings.setSolveOptions(
    testSettings.solveObservationMode(),
    testSettings.updateCubeLabel(),
    testSettings.errorPropagation(),
    true,
    testSettings.controlPointCoordTypeBundle(),
    testSettings.controlPointCoordTypeReports(),
    -2.0,
    -8.0,
    -32.0
  );

  EXPECT_EQ(Isis::Null, testSettings.globalPointCoord1AprioriSigma());
  EXPECT_EQ(Isis::Null, testSettings.globalPointCoord2AprioriSigma());
  EXPECT_EQ(Isis::Null, testSettings.globalPointCoord3AprioriSigma());
}

TEST(BundleSettings, setGlobalSigmasNoRadius) {
  BundleSettings testSettings;
  testSettings.setSolveOptions(
    testSettings.solveObservationMode(),
    testSettings.updateCubeLabel(),
    testSettings.errorPropagation(),
    false,
    testSettings.controlPointCoordTypeBundle(),
    testSettings.controlPointCoordTypeReports(),
    Isis::Null,
    Isis::Null,
    32.0
  );

  EXPECT_EQ(Isis::Null, testSettings.globalPointCoord3AprioriSigma());
}

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
        secondObsSettings
  };

  BundleSettings testSettings;
  testSettings.setObservationSolveOptions(optionsList);

  EXPECT_EQ(testSettings.numberSolveSettings(), optionsList.size());
  EXPECT_TRUE(std::equal(
        optionsList.begin(),
        optionsList.end(),
        testSettings.observationSolveSettings().begin(),
        observationSettingsComparison)
  );
  EXPECT_PRED_FORMAT2(
        assertObservationSettingsEqual,
        testSettings.observationSolveSettings(secondObservationNumber),
        secondObsSettings
  );
  EXPECT_PRED_FORMAT2(
        assertObservationSettingsEqual,
        testSettings.observationSolveSettings(1),
        secondObsSettings
  );
}

TEST_P(ConvergenceCriteriaTest, convergenceCriteriaStrings) {
  QString criteriaString = BundleSettings::convergenceCriteriaToString(GetParam());
  BundleSettings::ConvergenceCriteria criteria =
        BundleSettings::stringToConvergenceCriteria(criteriaString);
  EXPECT_EQ(GetParam(), criteria);
}

TEST_P(ConvergenceCriteriaTest, convergenceCriteria) {
  BundleSettings testSettings;
  testSettings.setConvergenceCriteria(
        GetParam(),
        2.0,
        50
  );
  EXPECT_EQ(GetParam(), testSettings.convergenceCriteria());
  EXPECT_EQ(2.0, testSettings.convergenceCriteriaThreshold());
  EXPECT_EQ(50, testSettings.convergenceCriteriaMaximumIterations());
}

INSTANTIATE_TEST_CASE_P(
      BundleSettings,
      ConvergenceCriteriaTest,
      ::testing::Values(BundleSettings::Sigma0, BundleSettings::ParameterCorrections)
);

TEST(BundleSettings, maximumLikelihoodHuber) {
  BundleSettings testSettings;
  testSettings.addMaximumLikelihoodEstimatorModel(
        MaximumLikelihoodWFunctions::Huber,
        64.0
  );
  QList< QPair< MaximumLikelihoodWFunctions::Model, double > > functions =
      testSettings.maximumLikelihoodEstimatorModels();
  ASSERT_EQ(1, functions.size());
  EXPECT_EQ(MaximumLikelihoodWFunctions::Huber, functions.front().first);
  EXPECT_EQ(64.0, functions.front().second);
}

TEST(BundleSettings, maximumLikelihoodHuberModified) {
  BundleSettings testSettings;
  testSettings.addMaximumLikelihoodEstimatorModel(
        MaximumLikelihoodWFunctions::HuberModified,
        64.0
  );
  QList< QPair< MaximumLikelihoodWFunctions::Model, double > > functions =
      testSettings.maximumLikelihoodEstimatorModels();
  ASSERT_EQ(1, functions.size());
  EXPECT_EQ(MaximumLikelihoodWFunctions::HuberModified, functions.front().first);
  EXPECT_EQ(64.0, functions.front().second);
}

// I am not sure why this throws an exceptions. Unforunately, the people who wrote
// this are now gone and no one knows if they should throw or not without doing
// more research. For now, we are testing existing functionality - JAM 2018/11/13
TEST(BundleSettings, maximumLikelihoodWelsch) {
  BundleSettings testSettings;
  try {
    testSettings.addMaximumLikelihoodEstimatorModel(
          MaximumLikelihoodWFunctions::Welsch,
          64.0
    );
    FAIL() << "Expected an exception";
  }
  catch (IException &e) {
    EXPECT_THAT(
          e.toString().toStdString(),
          ::testing::HasSubstr("the first model must be of type HUBER or HUBER_MODIFIED.")
    );
  }
  catch (...) {
    FAIL() << "Expected an ISIS exception";
  }
}

// I am not sure why this throws an exceptions. Unforunately, the people who wrote
// this are now gone and no one knows if they should throw or not without doing
// more research. For now, we are testing existing functionality - JAM 2018/11/13
TEST(BundleSettings, maximumLikelihoodChen) {
  BundleSettings testSettings;
  try {
    testSettings.addMaximumLikelihoodEstimatorModel(
          MaximumLikelihoodWFunctions::Chen,
          64.0
    );
    FAIL() << "Expected an exception";
  }
  catch (IException &e) {
    EXPECT_THAT(
          e.toString().toStdString(),
          ::testing::HasSubstr("the first model must be of type HUBER or HUBER_MODIFIED.")
    );
  }
  catch (...) {
    FAIL() << "Expected an ISIS exception";
  }
}

TEST(BundleSettings, multipleMaximumLikelihoodModels) {
  BundleSettings testSettings;
  testSettings.addMaximumLikelihoodEstimatorModel(
        MaximumLikelihoodWFunctions::Huber,
        64.0
  );
  testSettings.addMaximumLikelihoodEstimatorModel(
        MaximumLikelihoodWFunctions::HuberModified,
        32.0
  );
  testSettings.addMaximumLikelihoodEstimatorModel(
        MaximumLikelihoodWFunctions::Welsch,
        16.0
  );
  testSettings.addMaximumLikelihoodEstimatorModel(
        MaximumLikelihoodWFunctions::Chen,
        8.0
  );
  QList< QPair< MaximumLikelihoodWFunctions::Model, double > > functions =
      testSettings.maximumLikelihoodEstimatorModels();
  ASSERT_EQ(4, functions.size());
  EXPECT_EQ(MaximumLikelihoodWFunctions::Huber, functions[0].first);
  EXPECT_EQ(64.0, functions[0].second);
  EXPECT_EQ(MaximumLikelihoodWFunctions::HuberModified, functions[1].first);
  EXPECT_EQ(32.0, functions[1].second);
  EXPECT_EQ(MaximumLikelihoodWFunctions::Welsch, functions[2].first);
  EXPECT_EQ(16.0, functions[2].second);
  EXPECT_EQ(MaximumLikelihoodWFunctions::Chen, functions[3].first);
  EXPECT_EQ(8.0, functions[3].second);
}

TEST(BundleSettings, OutputFilePrefix) {
  BundleSettings testSettings;
  QString testPrefix("test/file/prefix");
  testSettings.setOutputFilePrefix(testPrefix);
  EXPECT_EQ(testPrefix, testSettings.outputFilePrefix());
}

TEST(BundleSettings, setBundleTargetBody) {
  BundleTargetBodyQsp testTarget(new BundleTargetBody);
  BundleSettings testSettings;
  testSettings.setBundleTargetBody(testTarget);
  EXPECT_EQ(testTarget, testSettings.bundleTargetBody());
}

TEST(BundleSettings, BundleTargetBodyAccesors) {
  MockBundleTargetBody *mockBody = new MockBundleTargetBody;
  EXPECT_CALL(*mockBody, numberParameters())
        .Times(3)
        .WillOnce(::testing::Return(0))
        .WillRepeatedly(::testing::Return(5));
  EXPECT_CALL(*mockBody, solvePoleRA())
        .Times(1)
        .WillOnce(::testing::Return(true));
  EXPECT_CALL(*mockBody, solvePoleRAVelocity())
        .Times(1)
        .WillOnce(::testing::Return(true));
  EXPECT_CALL(*mockBody, solvePoleDec())
        .Times(1)
        .WillOnce(::testing::Return(true));
  EXPECT_CALL(*mockBody, solvePoleDecVelocity())
        .Times(1)
        .WillOnce(::testing::Return(true));
  EXPECT_CALL(*mockBody, solvePM())
        .Times(1)
        .WillOnce(::testing::Return(true));
  EXPECT_CALL(*mockBody, solvePMVelocity())
        .Times(1)
        .WillOnce(::testing::Return(true));
  EXPECT_CALL(*mockBody, solvePMAcceleration())
        .Times(1)
        .WillOnce(::testing::Return(true));
  EXPECT_CALL(*mockBody, solveTriaxialRadii())
        .Times(1)
        .WillOnce(::testing::Return(true));
  EXPECT_CALL(*mockBody, solveMeanRadius())
        .Times(1)
        .WillOnce(::testing::Return(true));

  BundleSettings testSettings;
  BundleTargetBodyQsp testTarget(mockBody);
  testSettings.setBundleTargetBody(testTarget);
  EXPECT_FALSE(testSettings.solveTargetBody());
  EXPECT_TRUE(testSettings.solveTargetBody());
  EXPECT_EQ(5, testSettings.numberTargetBodyParameters());
  EXPECT_TRUE(testSettings.solvePoleRA());
  EXPECT_TRUE(testSettings.solvePoleRAVelocity());
  EXPECT_TRUE(testSettings.solvePoleDec());
  EXPECT_TRUE(testSettings.solvePoleDecVelocity());
  EXPECT_TRUE(testSettings.solvePM());
  EXPECT_TRUE(testSettings.solvePMVelocity());
  EXPECT_TRUE(testSettings.solvePMAcceleration());
  EXPECT_TRUE(testSettings.solveTriaxialRadii());
  EXPECT_TRUE(testSettings.solveMeanRadius());
}

// TODO test IPCE save method
