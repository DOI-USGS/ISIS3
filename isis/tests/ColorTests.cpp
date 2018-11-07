#include "gtest/gtest.h"

#include "Color.h"
#include "IException.h"

#include <QColor>


TEST (ColorTests, ValidColorToString) {
  EXPECT_EQ("#000000ff", Isis::Color::toRGBAString(QColor(0, 0, 0)));
  EXPECT_EQ("#00000000", Isis::Color::toRGBAString(QColor(0, 0, 0, 0)));
  EXPECT_EQ("#ff000000", Isis::Color::toRGBAString(QColor(255, 0, 0, 0)));
  EXPECT_EQ("#00ff0000", Isis::Color::toRGBAString(QColor(0, 255, 0, 0)));
  EXPECT_EQ("#0000ff00", Isis::Color::toRGBAString(QColor(0, 0, 255, 0)));
  EXPECT_EQ("#000000ff", Isis::Color::toRGBAString(QColor(0, 0, 0, 255)));
  EXPECT_EQ("#ffffffff", Isis::Color::toRGBAString(QColor(255, 255, 255, 255)));
  EXPECT_EQ("#0a141e28", Isis::Color::toRGBAString(QColor(10, 20, 30, 40)));
}

TEST (ColorTests, ValidStringToColor) {
  EXPECT_EQ(QColor(0, 0, 0), Isis::Color::fromRGBAString("#000000ff"));
  EXPECT_EQ(QColor(0, 0, 0, 0), Isis::Color::fromRGBAString("#00000000"));
  EXPECT_EQ(QColor(255, 0, 0, 0), Isis::Color::fromRGBAString("#ff000000"));
  EXPECT_EQ(QColor(0, 255, 0, 0), Isis::Color::fromRGBAString("#00ff0000"));
  EXPECT_EQ(QColor(0, 0, 255, 0), Isis::Color::fromRGBAString("#0000ff00"));
  EXPECT_EQ(QColor(0, 0, 0, 255), Isis::Color::fromRGBAString("#000000ff"));
  EXPECT_EQ(QColor(255, 255, 255, 255), Isis::Color::fromRGBAString("#ffffffff"));
  EXPECT_EQ(QColor(10, 20, 30, 40), Isis::Color::fromRGBAString("#0a141e28"));
}

TEST (ColorTests, InvalidColorToString) {
  try {
    Isis::Color::toRGBAString(QColor());
    FAIL() << "Expected an IException";
  }
  catch(Isis::IException &e) {
    EXPECT_EQ(e.toString().toLatin1(), "**USER ERROR** Can not convert an invalid color to an RGBA string.  There is no string representation of an invalid color.");
  }
  catch(...) {
    FAIL() << "Expected error message: **USER ERROR** Can not convert an invalid color to an RGBA string.  There is no string representation of an invalid color.";
  }
}

TEST (ColorTests, InvalidStringToColor) {
  EXPECT_EQ(QColor(QColor::Invalid), Isis::Color::fromRGBAString("#rrggbbaa"));
  EXPECT_EQ(QColor(QColor::Invalid), Isis::Color::fromRGBAString(" 00112233"));
  EXPECT_EQ(QColor(QColor::Invalid), Isis::Color::fromRGBAString(""));
  EXPECT_EQ(QColor(QColor::Invalid), Isis::Color::fromRGBAString("#001122"));
}
