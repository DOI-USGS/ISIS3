#include "GaussianDistribution.h"
#include "IException.h"
#include "TestUtilities.h"

#include <gtest/gtest.h>
#include <QString>


TEST(GaussianDistribution, DefaultConstructor) {
  Isis::GaussianDistribution dist;
  EXPECT_EQ(dist.Mean(), 0);
  EXPECT_EQ(dist.StandardDeviation(), 1.0);
}


TEST(GaussianDistribution, Constructor) {
  Isis::GaussianDistribution dist(1.0, 2.0);
  EXPECT_EQ(dist.Mean(), 1.0);
  EXPECT_EQ(dist.StandardDeviation(), 2.0);
}


TEST(GaussianDistribution, InvalidPercentage) {
  Isis::GaussianDistribution dist;
  QString message = "Argument percent outside of the range 0 to 100";
  try {
    dist.InverseCumulativeDistribution(110);
  }
  catch(Isis::IException &e) {
    EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
  }
  catch(...) {
    FAIL() << "Expected error message: \"" << message.toStdString() << "\"";
  }
}


TEST(DoubleTest, Distributions) {
  Isis::GaussianDistribution dist;

  EXPECT_NEAR(dist.CumulativeDistribution(-3.0), 0.13498980316295217, .000000000000000001);
  EXPECT_NEAR(dist.InverseCumulativeDistribution(0.13498980316295217), -3.0, .000001);

  EXPECT_NEAR(dist.CumulativeDistribution(-2.5), 0.62096653257757595, .000000000000000001);
  EXPECT_NEAR(dist.InverseCumulativeDistribution(0.62096653257757595), -2.5, .000001);

  EXPECT_NEAR(dist.CumulativeDistribution(-2.0), 2.2750131948179018, .000000000000000001);
  EXPECT_NEAR(dist.InverseCumulativeDistribution(2.2750131948179018), -2.0, .000001);

  EXPECT_NEAR(dist.CumulativeDistribution(-1.5),6.6807201268857881, .000000000000000001);
  EXPECT_NEAR(dist.InverseCumulativeDistribution(6.6807201268857881), -1.5, .000001);

  EXPECT_NEAR(dist.CumulativeDistribution(-1.0), 15.865525393145695, .000000000000000001);
  EXPECT_NEAR(dist.InverseCumulativeDistribution(15.865525393145695), -1.0, .000001);

  EXPECT_NEAR(dist.CumulativeDistribution(-0.5), 30.853753872598688, .000000000000000001);
  EXPECT_NEAR(dist.InverseCumulativeDistribution(30.853753872598688), -0.5, .000001);

  EXPECT_NEAR(dist.CumulativeDistribution(0.0), 50, .000000000000000001);
  EXPECT_NEAR(dist.InverseCumulativeDistribution(50), 0.0, .000001);

  EXPECT_NEAR(dist.CumulativeDistribution(0.5), 69.146246127401312, .000000000000000001);
  EXPECT_NEAR(dist.InverseCumulativeDistribution(69.146246127401312), 0.5, .000001);

  EXPECT_NEAR(dist.CumulativeDistribution(1.0), 84.134474606854297, .000000000000000001);
  EXPECT_NEAR(dist.InverseCumulativeDistribution(84.134474606854297), 1.0, .000001);

  EXPECT_NEAR(dist.CumulativeDistribution(1.5), 93.319279873114212, .000000000000000001);
  EXPECT_NEAR(dist.InverseCumulativeDistribution(93.319279873114212), 1.5, .000001);

  EXPECT_NEAR(dist.CumulativeDistribution(2.0), 97.724986805182098, .000000000000000001);
  EXPECT_NEAR(dist.InverseCumulativeDistribution(97.724986805182098), 2.0, .000001);

  EXPECT_NEAR(dist.CumulativeDistribution(2.5), 99.379033467422431, .000000000000000001);
  EXPECT_NEAR(dist.InverseCumulativeDistribution(99.379033467422431), 2.5, .000001);

  EXPECT_NEAR(dist.CumulativeDistribution(3.0), 99.865010196837048, .000000000000000001);
  EXPECT_NEAR(dist.InverseCumulativeDistribution(99.865010196837048), 3.0, .000001);
}
