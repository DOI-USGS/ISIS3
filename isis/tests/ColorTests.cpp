#include "gtest/gtest.h"

#include "Color.h"
#include "IException.h"

#include <QDebug>
#include <QColor>


namespace Isis{
  
  
  class QStringQColorPair: public ::testing::TestWithParam<std::pair<QString, QColor> > {
    // Intentionally left empty
  };


  TEST_P(QStringQColorPair, ValidColorToString) {
    EXPECT_EQ(GetParam().first, Color::toRGBAString(GetParam().second));
  }
  
  
  TEST_P(QStringQColorPair, ValidStringToColor) {
    EXPECT_EQ(GetParam().second, Color::fromRGBAString(GetParam().first));
  }


  INSTANTIATE_TEST_CASE_P(QStringQColorPairInstantiation, 
                          QStringQColorPair, 
                          ::testing::Values(std::make_pair(QString("#000000ff"), QColor(0, 0, 0)), 
                                            std::make_pair(QString("#00000000"), QColor(0, 0, 0, 0)),
                                            std::make_pair(QString("#ff000000"), QColor(255, 0, 0, 0)),
                                            std::make_pair(QString("#00ff0000"), QColor(0, 255, 0, 0)),
                                            std::make_pair(QString("#0000ff00"), QColor(0, 0, 255, 0)),
                                            std::make_pair(QString("#000000ff"), QColor(0, 0, 0, 255)),
                                            std::make_pair(QString("#ffffffff"), QColor(255, 255, 255, 255)),
                                            std::make_pair(QString("#0a141e28"), QColor(10, 20, 30, 40))));


  // TEST (ColorTests, ValidColorToString) {
  //   EXPECT_EQ("#000000ff", Color::toRGBAString(QColor(0, 0, 0)));
  //   EXPECT_EQ("#00000000", Color::toRGBAString(QColor(0, 0, 0, 0)));
  //   EXPECT_EQ("#ff000000", Color::toRGBAString(QColor(255, 0, 0, 0)));
  //   EXPECT_EQ("#00ff0000", Color::toRGBAString(QColor(0, 255, 0, 0)));
  //   EXPECT_EQ("#0000ff00", Color::toRGBAString(QColor(0, 0, 255, 0)));
  //   EXPECT_EQ("#000000ff", Color::toRGBAString(QColor(0, 0, 0, 255)));
  //   EXPECT_EQ("#ffffffff", Color::toRGBAString(QColor(255, 255, 255, 255)));
  //   EXPECT_EQ("#0a141e28", Color::toRGBAString(QColor(10, 20, 30, 40)));
  // }

  // TEST (ColorTests, ValidStringToColor) {
  //   EXPECT_EQ(QColor(0, 0, 0), Color::fromRGBAString("#000000ff"));
  //   EXPECT_EQ(QColor(0, 0, 0, 0), Color::fromRGBAString("#00000000"));
  //   EXPECT_EQ(QColor(255, 0, 0, 0), Color::fromRGBAString("#ff000000"));
  //   EXPECT_EQ(QColor(0, 255, 0, 0), Color::fromRGBAString("#00ff0000"));
  //   EXPECT_EQ(QColor(0, 0, 255, 0), Color::fromRGBAString("#0000ff00"));
  //   EXPECT_EQ(QColor(0, 0, 0, 255), Color::fromRGBAString("#000000ff"));
  //   EXPECT_EQ(QColor(255, 255, 255, 255), Color::fromRGBAString("#ffffffff"));
  //   EXPECT_EQ(QColor(10, 20, 30, 40), Color::fromRGBAString("#0a141e28"));
  // }

  TEST (ColorTests, InvalidColorToString) {
    try {
      Color::toRGBAString(QColor());
      FAIL() << "Expected an IException";
    }
    catch(IException &e) {
      EXPECT_TRUE(e.toString().toLatin1().contains("Can not convert an invalid color to an RGBA string.  There is no string representation of an invalid color"));
    }
    catch(...) {
      FAIL() << "Expected error message: \"Can not convert an invalid color to an RGBA string.  There is no string representation of an invalid color.\"";
    }
  }


  class InvalidColorString: public ::testing::TestWithParam<QString> {
  // Intentionally left empty
  };
  
  
  TEST_P(InvalidColorString, InvalidStringToColor) {
    EXPECT_EQ(QColor(QColor::Invalid), Color::fromRGBAString(GetParam()));
  }
  
  
  INSTANTIATE_TEST_CASE_P(InvalidColorStringInstantiation, 
                          InvalidColorString, 
                          ::testing::Values(QString("#rrggbbaa"),
                                            QString(" 00112233"),
                                            QString(""),
                                            QString("#001122")));


  // TEST (ColorTests, InvalidStringToColor) {
  //   EXPECT_EQ(QColor(QColor::Invalid), Color::fromRGBAString("#rrggbbaa"));
  //   EXPECT_EQ(QColor(QColor::Invalid), Color::fromRGBAString(" 00112233"));
  //   EXPECT_EQ(QColor(QColor::Invalid), Color::fromRGBAString(""));
  //   EXPECT_EQ(QColor(QColor::Invalid), Color::fromRGBAString("#001122"));
  // }
}
