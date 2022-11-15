#include "Column.h"
#include "TestUtilities.h"

#include <gtest/gtest.h>

using namespace Isis;

//Tests that the default constructor works as intended
TEST(Column, DefaultConstructor) {
  Column column;

  EXPECT_DOUBLE_EQ(column.Precision(), 4);
  EXPECT_DOUBLE_EQ(column.Width(), 0);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, column.Name(), QString(""));
  EXPECT_EQ(column.Alignment(), Column::NoAlign);
  EXPECT_EQ(column.DataType(), Column::NoType);
}

//Tests that the initialization constructor works as intended
TEST(Column, InitConstructor) {
  Column column("col1", 25, Column::Pixel, Column::Left);

  EXPECT_DOUBLE_EQ(column.Precision(), 4);
  EXPECT_DOUBLE_EQ(column.Width(), 25);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, column.Name(), QString("col1"));
  EXPECT_EQ(column.Alignment(), Column::Left);
  EXPECT_EQ(column.DataType(), Column::Pixel);
}

//Tests SetName & Name functions
TEST(Column, Name) {
  Column column;
  column.SetName("Test Column");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, column.Name(), QString("Test Column"));
}

//Tests SetWidth & Width functions
TEST(Column, Width) {
  Column column;
  column.SetWidth(100);

  EXPECT_DOUBLE_EQ(column.Width(), 100);
}

//Tests SetType & DataType functions with every member of the Type enum
TEST(Types, Type) {
  std::list<Column::Type> list = {Column::NoType, Column::Integer, Column::Real, Column::String, Column::Pixel};

  for (Column::Type type : list) {
    Column column;
    column.SetType(type);

    EXPECT_EQ(column.DataType(), type);
  }
  
}

//Tests SetAlignment & Alignment functions with every member of the Align enum
TEST(Align, Alignment) {
  std::list<Column::Align> list = {Column::NoAlign, Column::Right, Column::Left, Column::Decimal};

  for (Column::Align alignment : list) {
    Column column;
    column.SetAlignment(alignment);

    EXPECT_EQ(column.Alignment(), alignment);
  }
  
}

//Tests SetPrecision & Precision functions with Real type and Pixel type.
//These are the only two types expected to work with SetPrecision
TEST(Column, Precision) {
  Column column;
  column.SetType(Column::Real);
  column.SetPrecision(10);
  EXPECT_DOUBLE_EQ(column.Precision(), 10);

  column.SetType(Column::Pixel);
  column.SetPrecision(15);
  EXPECT_DOUBLE_EQ(column.Precision(), 15);
}

//Tests that SetName's exceptions are working correctly
//Should throw an error when SetName is called with a string
//whose length is greater than Width.
TEST(Column, SetNameError) {
  QString message = "Name [Test Column] is wider than width";
  Column column;
  column.SetWidth(1);
  try {
    column.SetName("Test Column");
  }
  catch(IException &e) {
    EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
  }
  catch(...) {
    FAIL() << "Expected an IException with message \""
    << message.toStdString() <<"\"";
  }
}

//Tests that SetWidth's exceptions are working correctly
//Should throw an error when SetWidth is called with a value less than
//the length of Name
TEST(Column, SetWidthError) {
  QString message = "Width is insufficient to contain name[Test Column]";
  Column column;
  column.SetName("Test Column");
  try {
    column.SetWidth(1);
  }
  catch(IException &e) {
    EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
  }
  catch(...) {
    FAIL() << "Expected an IException with message \""
    << message.toStdString() <<"\"";
  }
}

//Tests that SetTypes' exceptions are working correctly
//Should throw an error when Alignment is Decimal and
//SetType is called with String or Integer
TEST(TypeError, SetTypeError) {
  QString message = "Integer or string type is not sensible if alignment is Decimal.";

  std::list<Column::Type> list = {Column::Integer, Column::String};

  for (Column::Type type : list) {
    Column column;
    column.SetAlignment(Column::Decimal);
    try {
      column.SetType(type);
    }
    catch(IException &e) {
      EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
    }
    catch(...) {
      FAIL() << "Expected an IException with message \""
      << message.toStdString() <<"\"";
    }
  }
}

//Tests that SetAlignment's exceptions are working correctly
//Should throw an error when Type is String or Integer and
//SetAllignment is called with Decimal
TEST(TypeError, SetAlignmentError) {
  QString message = "Decimal alignment does not make sense for integer or string values.";

  std::list<Column::Type> list = {Column::Integer, Column::String};

  for (Column::Type type : list) {
    Column column;
    column.SetType(type);
    try {
      column.SetAlignment(Column::Decimal);
    }
    catch(IException &e) {
      EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
    }
    catch(...) {
      FAIL() << "Expected an IException with message \""
      << message.toStdString() <<"\"";
    }
  }
}


//Tests that Precision's exceptions are working correctly
//Should throw an error when SetPrecision is called and Alignment is not Decimal
TEST(PrecisionError, SetPrecisionError) {
  QString message = "Setting precision only makes sense for Decimal Alignment";
  std::list<Column::Align> list = {Column::NoAlign, Column::Right, Column::Left};

  for (Column::Align alignment : list) {
    Column column;
    column.SetAlignment(alignment);

    try{
      column.SetPrecision(10);
    }
    catch(IException &e) {
      EXPECT_PRED_FORMAT2(Isis::AssertIExceptionMessage, e, message);
    }
    catch(...) {
      FAIL() << "Expected an IException with message \""
      << message.toStdString() <<"\"";
    }
  }
}
