#include "Angle.h"
#include "BundleControlPoint.h"
#include "BundleSettings.h"
#include "ControlMeasure.h"
#include "ControlPoint.h"
#include "Distance.h"
#include "Latitude.h"
#include "Longitude.h"
#include "SpecialPixel.h"
#include "SurfacePoint.h"

#include <memory>

#include "gmock/gmock.h"

class BundleControlPointPopulated : public ::testing::Test {
  protected:
    BundleControlPointQsp bundlePoint;
    std::unique_ptr<ControlPoint> freePoint;
    BundleSettingsQsp settings;

    void SetUp() override {
      settings = BundleSettingsQsp(new BundleSettings);
      freePoint = std::make_unique<ControlPoint>("FreePoint");
      ControlMeasure *cm1 = new ControlMeasure;
      cm1->SetCubeSerialNumber("Ignored");
      cm1->SetIgnored(true);
      freePoint->Add(cm1);
      ControlMeasure *cm2 = new ControlMeasure;
      cm2->SetCubeSerialNumber("NotIgnored");
      cm2->SetIgnored(false);
      cm2->SetCoordinate(1.0, 2.0);
      cm2->SetResidual(-3.0, 4.0);
      freePoint->Add(cm2);

      SurfacePoint aprioriPoint(
        Latitude(45, Angle::Degrees),
        Longitude(90, Angle::Degrees),
        Distance(1000, Distance::Kilometers));
      freePoint->SetAprioriSurfacePoint(aprioriPoint);

      bundlePoint.reset(new BundleControlPoint(settings, freePoint.get()));
    }
};

TEST_F(BundleControlPointPopulated, FreePoint) {
  EXPECT_EQ(bundlePoint->rawControlPoint(), freePoint.get());
  EXPECT_FALSE(bundlePoint->isRejected());
  EXPECT_EQ(bundlePoint->numberOfMeasures(), freePoint->GetNumValidMeasures());
  EXPECT_EQ(bundlePoint->numberOfRejectedMeasures(), freePoint->GetNumberOfRejectedMeasures());
  EXPECT_EQ(bundlePoint->residualRms(), freePoint->GetResidualRms());
  EXPECT_EQ(bundlePoint->adjustedSurfacePoint(), freePoint->GetAdjustedSurfacePoint());
  EXPECT_FALSE(bundlePoint->id().isEmpty());
  EXPECT_EQ(bundlePoint->type(), freePoint->GetType());
  EXPECT_EQ(bundlePoint->coordTypeReports(), settings->controlPointCoordTypeReports());
  EXPECT_EQ(bundlePoint->coordTypeBundle(), settings->controlPointCoordTypeBundle());
  boost::numeric::ublas::bounded_vector< double, 3 > corrections, aprioriSigmas, adjustedSigmas, weights, nicVector;
  corrections = bundlePoint->corrections();
  aprioriSigmas = bundlePoint->aprioriSigmas();
  adjustedSigmas = bundlePoint->adjustedSigmas();
  weights = bundlePoint->weights();
  nicVector = bundlePoint->nicVector();
  EXPECT_EQ(corrections[0], 0.0);
  EXPECT_EQ(corrections[1], 0.0);
  EXPECT_EQ(corrections[2], 0.0);
  EXPECT_EQ(aprioriSigmas[0], Isis::Null);
  EXPECT_EQ(aprioriSigmas[1], Isis::Null);
  EXPECT_EQ(aprioriSigmas[2], Isis::Null);
  EXPECT_EQ(adjustedSigmas[0], Isis::Null);
  EXPECT_EQ(adjustedSigmas[1], Isis::Null);
  EXPECT_EQ(adjustedSigmas[2], Isis::Null);
  EXPECT_EQ(weights[0], 0.0);
  EXPECT_EQ(weights[1], 0.0);
  EXPECT_EQ(weights[2], 1.0e+50);
  EXPECT_EQ(nicVector[0], 0.0);
  EXPECT_EQ(nicVector[1], 0.0);
  EXPECT_EQ(nicVector[2], 0.0);
}


TEST_F(BundleControlPointPopulated, Mutators) {
  BundleSettingsQsp newSettings = BundleSettingsQsp(new BundleSettings);
  newSettings->setSolveOptions(
        false, false, false, true,
        SurfacePoint::Latitudinal, SurfacePoint::Latitudinal,
        10,
        100,
        1000);

  SurfacePoint newPoint(
        Latitude(45, Angle::Degrees),
        Longitude(90, Angle::Degrees),
        Distance(1000, Distance::Kilometers));
  freePoint->SetAprioriSurfacePoint(newPoint);

  bundlePoint->setAdjustedSurfacePoint(newPoint);
  bundlePoint->setNumberOfRejectedMeasures(1);
  bundlePoint->setWeights(newSettings);
  bundlePoint->setRejected(true);

  EXPECT_TRUE(bundlePoint->isRejected());
  EXPECT_EQ(bundlePoint->numberOfRejectedMeasures(), 1);
  EXPECT_EQ(bundlePoint->adjustedSurfacePoint(), newPoint);
  boost::numeric::ublas::bounded_vector< double, 3 > weights, aprioriSigmas;
  // These get converted from m to radians so they are a different from 1 / sigma
  weights = bundlePoint->weights();
  EXPECT_DOUBLE_EQ(weights[0], 1.0e+10);
  EXPECT_DOUBLE_EQ(weights[1], 5.0e+7);
  EXPECT_EQ(weights[2], 1.0);
  // These are the original input sigmas
  aprioriSigmas = bundlePoint->aprioriSigmas();
  EXPECT_EQ(aprioriSigmas[0], 10.0);
  EXPECT_EQ(aprioriSigmas[1], 100.0);
  EXPECT_EQ(aprioriSigmas[2], 1000.0);

  bundlePoint->zeroNumberOfRejectedMeasures();
  EXPECT_EQ(bundlePoint->numberOfRejectedMeasures(), 0);
}


TEST_F(BundleControlPointPopulated, OutputStrings) {
  QString summaryString = bundlePoint->formatBundleOutputSummaryString(false);
  EXPECT_THAT(
        summaryString.toStdString(),
        ::testing::HasSubstr("FreePoint           FREE    1 of 1  3.54            Null            Null            Null             N/A             N/A             N/A"));

  QString detailString = bundlePoint->formatBundleOutputDetailString(false);
  // Info line
  EXPECT_THAT(
        detailString.toStdString(),
        ::testing::HasSubstr(" Label: FreePoint\n"
                             "Status: FREE\n"
                             "  Rays: 1 of 1"));
  // Header
  EXPECT_THAT(
        detailString.toStdString(),
        ::testing::HasSubstr("     Point         Initial               Total               Total              Final             Initial             Final\n"
                             "Coordinate          Value             Correction          Correction            Value             Accuracy          Accuracy\n"
                             "                 (dd/dd/km)           (dd/dd/km)           (Meters)           (dd/dd/km)          (Meters)          (Meters)"));
  // Contents
  EXPECT_THAT(
        detailString.toStdString(),
        ::testing::HasSubstr("  LATITUDE             Null           0.00000000          0.00000000                Null              FREE               N/A\n"
                             " LONGITUDE             Null           0.00000000          0.00000000                Null              FREE               N/A\n"
                             "    RADIUS             Null           0.00000000          0.00000000                Null               N/A               N/A"));

  // Populate with full data

  BundleSettingsQsp newSettings = BundleSettingsQsp(new BundleSettings);
  newSettings->setSolveOptions(
        false, false, false, true,
        SurfacePoint::Latitudinal, SurfacePoint::Latitudinal,
        10,
        100,
        1000);
  bundlePoint->setWeights(newSettings);

  SurfacePoint adjustedPoint(
        Latitude(55, Angle::Degrees),
        Longitude(80, Angle::Degrees),
        Distance(1100, Distance::Kilometers),
        Angle(5.0, Angle::Degrees),
        Angle(50.0, Angle::Degrees),
        Distance(500, Distance::Kilometers));
  freePoint->SetAdjustedSurfacePoint(adjustedPoint);
  bundlePoint->corrections()[0] += 10.0 / RAD2DEG;
  bundlePoint->corrections()[1] -= 10.0 / RAD2DEG;
  bundlePoint->corrections()[2] += 100.0;

  detailString = bundlePoint->formatBundleOutputDetailString(false);
  EXPECT_THAT(
      detailString.toStdString(),
      ::testing::HasSubstr("  LATITUDE      45.00000000          10.00000000     191986.21771938         55.00000000       10.00000000               N/A\n"
                           " LONGITUDE      90.00000000         -10.00000000    -110118.77058800         80.00000000      100.00000000               N/A\n"
                           "    RADIUS    1000.00000000         100.00000000     100000.00000000       1100.00000000     1000.00000000               N/A"));

  detailString = bundlePoint->formatBundleOutputDetailString(true);
  EXPECT_THAT(
      detailString.toStdString(),
      ::testing::Not(::testing::HasSubstr("N/A")));

  // Change point to constrained
  freePoint->SetType(ControlPoint::Constrained);
  detailString = bundlePoint->formatBundleOutputDetailString(false, true);
  EXPECT_THAT(
        detailString.toStdString(),
        ::testing::HasSubstr("Status: CONSTRAINED"));

  // Change point to fixed and fix sigmas
  freePoint->SetType(ControlPoint::Fixed);
  bundlePoint->aprioriSigmas()[0] = Isis::Null;
  bundlePoint->aprioriSigmas()[1] = Isis::Null;
  bundlePoint->aprioriSigmas()[2] = Isis::Null;

  detailString = bundlePoint->formatBundleOutputDetailString(false, true);
  EXPECT_THAT(
        detailString.toStdString(),
        ::testing::HasSubstr("Status: FIXED"));
  EXPECT_THAT(
      detailString.toStdString(),
      ::testing::HasSubstr("  LATITUDE      45.00000000          10.00000000     191986.21771938         55.00000000             FIXED               N/A\n"
                           " LONGITUDE      90.00000000         -10.00000000    -110118.77058800         80.00000000             FIXED               N/A\n"
                           "    RADIUS    1000.00000000         100.00000000     100000.00000000       1100.00000000             FIXED               N/A"));

  detailString = bundlePoint->formatBundleOutputDetailString(false, false);
  EXPECT_THAT(
      detailString.toStdString(),
      ::testing::HasSubstr("  LATITUDE      45.00000000          10.00000000     191986.21771938         55.00000000             FIXED               N/A\n"
                           " LONGITUDE      90.00000000         -10.00000000    -110118.77058800         80.00000000             FIXED               N/A\n"
                           "    RADIUS    1000.00000000         100.00000000     100000.00000000       1100.00000000               N/A               N/A"));

  summaryString = bundlePoint->formatBundleOutputSummaryString(false);
  EXPECT_THAT(
        summaryString.toStdString(),
        ::testing::HasSubstr("FIXED"));
}


TEST(BundleControlPoint, Rectangular) {
  BundleSettingsQsp settings(new BundleSettings);
  settings->setSolveOptions(
      false, false, false, false,
      SurfacePoint::Rectangular, SurfacePoint::Rectangular,
      2.0, 3.0, 4.0);
  std::unique_ptr<ControlPoint> freePoint = std::make_unique<ControlPoint>("FreePoint");
  ControlMeasure *cm1 = new ControlMeasure;
  cm1->SetCubeSerialNumber("Ignored");
  cm1->SetIgnored(true);
  freePoint->Add(cm1);
  ControlMeasure *cm2 = new ControlMeasure;
  cm2->SetCubeSerialNumber("NotIgnored");
  cm2->SetIgnored(false);
  cm2->SetCoordinate(1.0, 2.0);
  cm2->SetResidual(-3.0, 4.0);
  freePoint->Add(cm2);

  SurfacePoint aprioriPoint(
    Latitude(45, Angle::Degrees),
    Longitude(90, Angle::Degrees),
    Distance(1000, Distance::Kilometers));
  SurfacePoint adjustedPoint(
        Latitude(55, Angle::Degrees),
        Longitude(80, Angle::Degrees),
        Distance(1100, Distance::Kilometers),
        Angle(5.0, Angle::Degrees),
        Angle(50.0, Angle::Degrees),
        Distance(500, Distance::Kilometers));
  freePoint->SetAdjustedSurfacePoint(adjustedPoint);
  freePoint->SetAprioriSurfacePoint(aprioriPoint);

  BundleControlPointQsp bundlePoint(new BundleControlPoint(settings, freePoint.get()));

  boost::numeric::ublas::bounded_vector< double, 3 > weights = bundlePoint->weights();
  EXPECT_DOUBLE_EQ(weights[0], 250000);
  EXPECT_DOUBLE_EQ(weights[1], 111111.11111111111);
  EXPECT_DOUBLE_EQ(weights[2], 62500);

  QString summaryString = bundlePoint->formatBundleOutputSummaryString(false);
  EXPECT_THAT(
        summaryString.toStdString(),
        ::testing::HasSubstr("FreePoint           FREE    1 of 1  3.54    109.56055322    621.34877361    901.06724872             N/A             N/A             N/A"));

  QString detailString = bundlePoint->formatBundleOutputDetailString(false);
  // Header
  EXPECT_THAT(
        detailString.toStdString(),
        ::testing::HasSubstr("        Point         Initial              Total              Final             Initial              Final\n"
                             "   Coordinate         Value             Correction            Value             Accuracy          Accuracy\n"
                             "                    (km/km/km)             (km)           (km/km/km)          (Meters)          (Meters)"));

  // Contents
  EXPECT_THAT(
        detailString.toStdString(),
        ::testing::HasSubstr(" BODY-FIXED-X     109.56055322          0.00000000        109.56055322        2.00000000               N/A\n"
                             " BODY-FIXED-Y     621.34877361          0.00000000        621.34877361        3.00000000               N/A\n"
                             " BODY-FIXED-Z     901.06724872          0.00000000        901.06724872        4.00000000               N/A"));
}