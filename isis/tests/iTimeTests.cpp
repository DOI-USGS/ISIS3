#include <QString>

#include "IString.h"
#include "iTime.h"

#include "gmock/gmock.h"

using namespace Isis;

TEST(iTimeTests, TimeAccess) {
    iTime testTime("2003-01-02T12:15:01.1234");

    EXPECT_EQ(2003, testTime.Year());
    EXPECT_EQ(01, testTime.Month());
    EXPECT_EQ(02, testTime.Day());
    EXPECT_EQ(12, testTime.Hour());
    EXPECT_EQ(15, testTime.Minute());
    EXPECT_EQ(01.1234, testTime.Second());
    EXPECT_EQ(2, testTime.DayOfYear());
    EXPECT_NEAR(94781765.307363, testTime.Et(), 0.000001);
}


TEST(iTimeTests, TimeWithSlashes) {
    QString timeString("2003-01-02T12:15:01.1234");
    QString timeStringWithSlashes("2003/01/02 12:15:01.1234");
    iTime testTime(timeString);
    iTime testTimeWithSlashes(timeStringWithSlashes);

    EXPECT_EQ(testTimeWithSlashes.Year(), testTime.Year());
    EXPECT_EQ(testTimeWithSlashes.Month(), testTime.Month());
    EXPECT_EQ(testTimeWithSlashes.Day(), testTime.Day());
    EXPECT_EQ(testTimeWithSlashes.Hour(), testTime.Hour());
    EXPECT_EQ(testTimeWithSlashes.Minute(), testTime.Minute());
    EXPECT_EQ(testTimeWithSlashes.Second(), testTime.Second());
    EXPECT_EQ(testTimeWithSlashes.DayOfYear(), testTime.DayOfYear());
    EXPECT_EQ(testTimeWithSlashes.Et(), testTime.Et());
}


TEST(iTimeTests, FromEt) {
    QString timeString("2003-01-02T12:15:01.1234");
    iTime testTime(timeString);
    iTime testTimeFromEt(testTime.Et());

    EXPECT_EQ(testTimeFromEt.Year(), testTime.Year());
    EXPECT_EQ(testTimeFromEt.Month(), testTime.Month());
    EXPECT_EQ(testTimeFromEt.Day(), testTime.Day());
    EXPECT_EQ(testTimeFromEt.Hour(), testTime.Hour());
    EXPECT_EQ(testTimeFromEt.Minute(), testTime.Minute());
    EXPECT_EQ(testTimeFromEt.Second(), testTime.Second());
    EXPECT_EQ(testTimeFromEt.DayOfYear(), testTime.DayOfYear());
    EXPECT_EQ(testTimeFromEt.Et(), testTime.Et());
}


TEST(iTimeTests, StringTimes) {
    QString timeString("2003-01-02T12:15:01.1234");
    iTime testTime(timeString);

    EXPECT_EQ(toString(testTime.Year()).toStdString(), testTime.YearString().toStdString());
    EXPECT_EQ(toString(testTime.Month()).toStdString(), testTime.MonthString().toStdString());
    EXPECT_EQ(toString(testTime.Day()).toStdString(), testTime.DayString().toStdString());
    EXPECT_EQ(toString(testTime.Hour()).toStdString(), testTime.HourString().toStdString());
    EXPECT_EQ(toString(testTime.Minute()).toStdString(), testTime.MinuteString().toStdString());
    EXPECT_EQ(toString(testTime.Second(), 8).toStdString(), testTime.SecondString(8).toStdString());
    EXPECT_EQ(toString(testTime.DayOfYear()).toStdString(), testTime.DayOfYearString().toStdString());
    EXPECT_EQ(toString(testTime.Et()).toStdString(), testTime.EtString().toStdString());
    EXPECT_EQ(timeString.toStdString(), testTime.UTC().toStdString());
}


TEST(iTimeTests, Comparison) {
    QString beforeString("2003-01-02T12:15:01.1234");
    QString afterString("2010-04-03T16:32:56.2487");

    iTime beforeTime(beforeString);
    iTime afterTime(afterString);

    EXPECT_TRUE(beforeTime < afterTime);
    EXPECT_FALSE(afterTime < beforeTime);
    EXPECT_FALSE(beforeTime < beforeTime);

    EXPECT_TRUE(beforeTime <= afterTime);
    EXPECT_FALSE(afterTime <= beforeTime);
    EXPECT_TRUE(beforeTime <= beforeTime);

    EXPECT_FALSE(beforeTime > afterTime);
    EXPECT_TRUE(afterTime > beforeTime);
    EXPECT_FALSE(beforeTime > beforeTime);

    EXPECT_FALSE(beforeTime >= afterTime);
    EXPECT_TRUE(afterTime >= beforeTime);
    EXPECT_TRUE(beforeTime >= beforeTime);

    EXPECT_TRUE(beforeTime != afterTime);
    EXPECT_TRUE(afterTime != beforeTime);
    EXPECT_FALSE(beforeTime != beforeTime);

    EXPECT_FALSE(beforeTime == afterTime);
    EXPECT_FALSE(afterTime == beforeTime);
    EXPECT_TRUE(beforeTime == beforeTime);
}


TEST(iTimeTests, Arithmetic) {
    iTime testTime("2003-01-02T12:15:01.1234");

    EXPECT_EQ((testTime + 10.0).Et(), testTime.Et() + 10.0);
    EXPECT_EQ((10.0 + testTime).Et(), testTime.Et() + 10.0);
    EXPECT_EQ((testTime - 10.0).Et(), testTime.Et() - 10.0);
    // This doesn't make sense because subtraction isn't commutative
    // but that's how the function is written
    EXPECT_EQ((10.0 - testTime).Et(), testTime.Et() - 10.0);

    EXPECT_EQ(iTime(20.0) - iTime(10.0), 10.0);

    iTime moreTime = testTime;
    moreTime += 10.0;
    EXPECT_EQ(moreTime.Et(), testTime.Et() + 10.0);

    iTime lessTime = testTime;
    lessTime -= 10.0;
    EXPECT_EQ(lessTime.Et(), testTime.Et() - 10.0);

}