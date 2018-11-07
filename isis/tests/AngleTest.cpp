#include "Angle.h" 
#include "SpecialPixel.h"
#include "Constants.h" 
#include "IException.h"

#include <QString>
#include <QDebug> 

#include <gtest/gtest.h>

#define TEST_EPSILON 1e-8

TEST(AngleTest, DefaultConstructor) {
  Isis::Angle angle;
  EXPECT_FALSE(angle.isValid());
  EXPECT_EQ(angle.radians(), Isis::Null);
  EXPECT_EQ(angle.degrees(), Isis::Null); 
  EXPECT_STREQ(angle.toString().toLatin1(), ""); 
}


TEST(AngleTest, NullInput_DegreeOutput) {
  Isis::Angle angle;
  EXPECT_NEAR(angle.degrees(), Isis::Null, TEST_EPSILON);
  EXPECT_FALSE(angle.isValid());
  EXPECT_STREQ(angle.toString().toLatin1(), ""); 
}


TEST(AngleTest, DegreeInput_RadianOutput) {
  Isis::Angle angle(30., Isis::Angle::Degrees );
  EXPECT_NEAR(angle.radians(), 30 * Isis::PI/180.0, TEST_EPSILON);
  EXPECT_TRUE(angle.isValid()); 
  EXPECT_STREQ(angle.toString().toLatin1(), "30.0 degrees");  
}


TEST(AngleTest, RadianInput_DegreeOutput) {
  Isis::Angle angle(30. * Isis::PI / 180.0, Isis::Angle::Radians );
  EXPECT_NEAR(angle.degrees(), 30.0, TEST_EPSILON);
  EXPECT_STREQ(angle.toString(false).toLatin1(), "30.0");  
}


TEST(AngleTest, CopyConstructor) {
  Isis::Angle angle(30., Isis::Angle::Degrees );
  Isis::Angle angleCopy(angle);
  EXPECT_NEAR(angleCopy.degrees(), 30.0,TEST_EPSILON); 
}


TEST(AngleTest, Mutators) {
  Isis::Angle angle(30.0, Isis::Angle::Degrees );
  angle.setDegrees(180);
  EXPECT_NEAR(angle.degrees(), 180.0, TEST_EPSILON);
  angle.setRadians(Isis::PI);
  EXPECT_NEAR(angle.radians(), Isis::PI, TEST_EPSILON); 
}

TEST(AngleOperators, QDebug) {
  // Stream qDebug output to a string and then do a string comparison?
  Isis::Angle angle(45.0, Isis::Angle::Degrees);
  QString qdebug_output;
  QDebug debug(&qdebug_output);
  debug << angle;
  EXPECT_STREQ(qdebug_output.toLatin1(), "0.785398 <radians> (45 <degrees>) "); 
}


TEST(AngleOperators, Assignment) {
  Isis::Angle angle(30.0, Isis::Angle::Degrees );
  angle = Isis::Angle(45.0, Isis::Angle::Degrees);
  EXPECT_NEAR(angle.degrees(), 45.0, TEST_EPSILON); 
}


TEST(AngleOperators, AddSubtract) {
  Isis::Angle angle1(30.0, Isis::Angle::Degrees );
  Isis::Angle angle2(60.0, Isis::Angle::Degrees );
  angle1 = angle1 + angle2;  
  EXPECT_NEAR(angle1.degrees(), 90.0, TEST_EPSILON);
  angle1 += angle2;
  EXPECT_NEAR(angle1.degrees(), 150.0, TEST_EPSILON);
  angle1 -= angle2;
  EXPECT_NEAR(angle1.degrees(), 90.0, TEST_EPSILON);    
  angle1 = angle1 - angle2;
  EXPECT_NEAR(angle1.degrees(), 30.0, TEST_EPSILON);
}


TEST(AngleOperators, MultiplyDivide) {
  Isis::Angle angle(30., Isis::Angle::Degrees );
  angle = 2. * angle;
  EXPECT_NEAR(angle.degrees(), 60.0, TEST_EPSILON);
  angle *= 2;
  EXPECT_NEAR(angle.degrees(), 120.0, TEST_EPSILON);
  angle /= 2;
  EXPECT_NEAR(angle.degrees(), 60.0, TEST_EPSILON);    
  angle = angle / 2;
  EXPECT_NEAR(angle.degrees(), 30.0, TEST_EPSILON);
}  


TEST(AngleOperators, Logical) {
  Isis::Angle angle1(30.0, Isis::Angle::Degrees );
  Isis::Angle angle2(45.0, Isis::Angle::Degrees );
  EXPECT_FALSE(angle1 == angle2);
  EXPECT_TRUE(Isis::Angle() == Isis::Angle()); 
  EXPECT_FALSE(Isis::Angle() == angle2); 
}


TEST(AngleOperators, LessThan) {
  Isis::Angle angle1(30.0, Isis::Angle::Degrees );
  Isis::Angle angle2(45.0, Isis::Angle::Degrees );
  EXPECT_TRUE(angle1 < angle2);
  EXPECT_FALSE(angle2 < angle1);
  Isis::Angle epsilon(1.1920929e-10, Isis::Angle::Degrees);
  EXPECT_TRUE(angle1 < angle1 + epsilon); 
}

// Added good and consistent test for "greater than" 
TEST(AngleOperators, GreaterThan) {
  Isis::Angle angle1(30.0, Isis::Angle::Degrees );
  Isis::Angle angle2(45.0, Isis::Angle::Degrees );
  EXPECT_TRUE(angle2 > angle1);
  EXPECT_FALSE(angle1 > angle2); 
  Isis::Angle epsilon(1.1920929e-10, Isis::Angle::Degrees);
  EXPECT_TRUE(angle2 > angle2 - epsilon);
}
  

TEST(AngleOperators, LessThanOrEqualTo) {
  Isis::Angle angle1(30.0, Isis::Angle::Degrees );
  Isis::Angle angle2(45.0, Isis::Angle::Degrees );
  EXPECT_TRUE(angle1 <= angle1);
  EXPECT_TRUE(angle1 <= angle2);
  EXPECT_FALSE(angle2 <= angle1); 
}


TEST(AngleExceptions, LessThanNullAngle){
  Isis::Angle angle1(30., Isis::Angle::Degrees );
  
  try {
    angle1 < Isis::Angle();
    FAIL() << "Expected an error";
   }
   catch(Isis::IException &e) {
     EXPECT_EQ(e.toString().toLatin1(), "**PROGRAMMER ERROR** Cannot compare a invalid angles with the < operator." );
   }
   catch(...) {
     FAIL() << "Expected IException: **PROGRAMMER ERROR** Cannot compare a invalid angles with the < operator, but got something else "; 
   }
}
    
TEST(AngleExceptions, NullAngleLessThan){
  Isis::Angle angle1(30., Isis::Angle::Degrees );

  try {
    Isis::Angle() < angle1;
    FAIL() << "Expected an error";
   }
   catch(Isis::IException &e) {
     EXPECT_EQ(e.toString().toLatin1(), "**PROGRAMMER ERROR** Cannot compare a invalid angles with the < operator." );
   }
   catch(...) {
     FAIL() << "Expected IException: **PROGRAMMER ERROR** Cannot compare a invalid angles with the < operator, but got something else "; 
   }
}

TEST(AngleExceptions, NullAngleLessThanOrEqual){
  Isis::Angle angle1(30., Isis::Angle::Degrees );

  try {
    Isis::Angle() <= angle1;
    FAIL() << "Expected an error";
   }
   catch(Isis::IException &e) {
     EXPECT_EQ(e.toString().toLatin1(), "**PROGRAMMER ERROR** Cannot compare a invalid angles with the < operator." );
   }
   catch(...) {
     FAIL() << "Expected IException: **PROGRAMMER ERROR** Cannot compare a invalid angles with the < operator, but got something else "; 
   }
}


TEST(AngleExceptions, GreaterThanNullAngle){
  Isis::Angle angle1(30., Isis::Angle::Degrees );
  
  try {
    angle1 > Isis::Angle();
    FAIL() << "Expected an error";
   }
   catch(Isis::IException &e) {
     EXPECT_EQ(e.toString().toLatin1(), "**PROGRAMMER ERROR** Cannot compare a invalid angles with the > operator." );
   }
   catch(...) {
     FAIL() << "Expected IException: **PROGRAMMER ERROR** Cannot compare a invalid angles with the > operator, but got something else "; 
   }
}


TEST(AngleExceptions, GreaterThanOrEqualToNullAngle){
  Isis::Angle angle1(30., Isis::Angle::Degrees );
  
  try {
    angle1 >= Isis::Angle();
    FAIL() << "Expected an error";
   }
   catch(Isis::IException &e) {
     EXPECT_EQ(e.toString().toLatin1(), "**PROGRAMMER ERROR** Cannot compare a invalid angles with the > operator." );
   }
   catch(...) {
     FAIL() << "Expected IException: **PROGRAMMER ERROR** Cannot compare a invalid angles with the > operator, but got something else "; 
   }
}


TEST(AngleOperators, Multiplication) {
  Isis::Angle angle(30.0, Isis::Angle::Degrees );
  angle = 2.0 * angle; 
  EXPECT_NEAR(angle.degrees(), 60.0, TEST_EPSILON); 
}



