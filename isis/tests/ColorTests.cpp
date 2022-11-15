#include "gtest/gtest.h"

#include "Color.h"
#include "IException.h"
#include "TestUtilities.h"

#include <QDebug>
#include <QColor>


namespace Isis{


  TEST (QStringQColorPair, ValidColorToString) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, QString("#000000ff"), Color::toRGBAString(QColor(0, 0, 0)));
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, QString("#00000000"), Color::toRGBAString(QColor(0, 0, 0, 0)));
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, QString("#ff000000"), Color::toRGBAString(QColor(255, 0, 0, 0)));
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, QString("#00ff0000"), Color::toRGBAString(QColor(0, 255, 0, 0)));
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, QString("#0000ff00"), Color::toRGBAString(QColor(0, 0, 255, 0)));
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, QString("#000000ff"), Color::toRGBAString(QColor(0, 0, 0, 255)));
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, QString("#ffffffff"), Color::toRGBAString(QColor(255, 255, 255, 255)));
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, QString("#0a141e28"), Color::toRGBAString(QColor(10, 20, 30, 40)));
  }


  TEST (ColorTests, InvalidColorToString) {
    QString message = "Can not convert an invalid color to an RGBA string.  "
                      "There is no string representation of an invalid color";
    try {
      Color::toRGBAString(QColor());
      FAIL() << "Expected an IException";
    }
    catch(IException &e) {
      EXPECT_PRED_FORMAT2(
            AssertIExceptionMessage,
            e,
            message);
      EXPECT_PRED_FORMAT2(
            AssertIExceptionError,
            e,
            IException::ErrorType::Unknown);
    }
    catch(...) {
      FAIL() << "Expected error message: \"" << message.toStdString() << "\"";
    }
  }


  TEST (QStringQColorPair, ValidStringToColor) {
    EXPECT_EQ(QColor(0, 0, 0), Color::fromRGBAString(QString("#000000ff")));
    EXPECT_EQ(QColor(0, 0, 0, 0), Color::fromRGBAString(QString("#00000000")));
    EXPECT_EQ(QColor(255, 0, 0, 0), Color::fromRGBAString(QString("#ff000000")));
    EXPECT_EQ(QColor(0, 255, 0, 0), Color::fromRGBAString(QString("#00ff0000")));
    EXPECT_EQ(QColor(0, 0, 255, 0), Color::fromRGBAString(QString("#0000ff00")));
    EXPECT_EQ(QColor(0, 0, 0, 255), Color::fromRGBAString(QString("#000000ff")));
    EXPECT_EQ(QColor(255, 255, 255, 255), Color::fromRGBAString(QString("#ffffffff")));
    EXPECT_EQ(QColor(10, 20, 30, 40), Color::fromRGBAString(QString("#0a141e28")));

  }


  TEST (InvalidColorString, InvalidStringToColor) {
    EXPECT_EQ(QColor(QColor::Invalid), Color::fromRGBAString("#rrggbbaa"));
    EXPECT_EQ(QColor(QColor::Invalid), Color::fromRGBAString(" 00112233"));
    EXPECT_EQ(QColor(QColor::Invalid), Color::fromRGBAString(""));
    EXPECT_EQ(QColor(QColor::Invalid), Color::fromRGBAString("#001122"));
  }


  TEST (QStringQColorPair, ValidColorRGBAFormat) {
    EXPECT_TRUE(Color::colorRGBAFormat().exactMatch(QString("#000000ff")));
    EXPECT_TRUE(Color::colorRGBAFormat().exactMatch(QString("#00000000")));
    EXPECT_TRUE(Color::colorRGBAFormat().exactMatch(QString("#ff000000")));
    EXPECT_TRUE(Color::colorRGBAFormat().exactMatch(QString("#00ff0000")));
    EXPECT_TRUE(Color::colorRGBAFormat().exactMatch(QString("#0000ff00")));
    EXPECT_TRUE(Color::colorRGBAFormat().exactMatch(QString("#000000ff")));
    EXPECT_TRUE(Color::colorRGBAFormat().exactMatch(QString("#ffffffff")));
    EXPECT_TRUE(Color::colorRGBAFormat().exactMatch(QString("#0a141e28")));
  }


  TEST (InvalidColorString, InvalidColorRGBAFormat) {
    EXPECT_FALSE(Color::colorRGBAFormat().exactMatch(QString("#rrggbbaa")));
    EXPECT_FALSE(Color::colorRGBAFormat().exactMatch(QString(" 00112233")));
    EXPECT_FALSE(Color::colorRGBAFormat().exactMatch(QString("")));
    EXPECT_FALSE(Color::colorRGBAFormat().exactMatch(QString("#001122")));
  }
}
