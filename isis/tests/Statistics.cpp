#include "Statistics.h" 

#include "FileName.h"
#include "IException.h"
#include "Preference.h"
#include "Statistics.h"
#include "XmlStackedHandlerReader.h"

#include <QDebug>
#include <QDataStream>
#include <QXmlStreamWriter>
#include <QXmlInputSource>

#include <float.h>

#include <gtest/gtest.h>

using namespace Isis;
using namespace std;

#define TEST_EPSILON 1e-8

/**
 * @author 2018-11-20 Tyler Wilson
 *
 * @internal
 *   @history 2018-11-20 Tyler Wilson - Coverage of existing
 *            statistical routines using truth data generated
 *            in R.
 */



TEST(Statistics, HandCalculations) {

    Statistics s;
    
    s.AddData(-10.0);
    s.AddData(20.0);
    s.AddData(30.0);
    

    //Values were computed in R
    double mu =13.3333333333333;
    double std = 20.8166599946613;
    double var = 433.333333333333;
    
    double rms = 21.6024689946929;   

    EXPECT_NEAR(s.Average(),mu,TEST_EPSILON);
    EXPECT_NEAR(s.Variance(),var,TEST_EPSILON);
    EXPECT_NEAR(s.StandardDeviation(),std,TEST_EPSILON);
    EXPECT_NEAR(s.Sum(),40.0,TEST_EPSILON);
    EXPECT_NEAR(s.SumSquare(),1400.0,TEST_EPSILON);
    EXPECT_NEAR(s.ValidPixels(),3.0,TEST_EPSILON);
    EXPECT_NEAR(s.Rms(),rms,TEST_EPSILON);    
    EXPECT_NEAR(s.Minimum(),-10.0,TEST_EPSILON);
    EXPECT_NEAR(s.Maximum(),30.0,TEST_EPSILON);
    EXPECT_NEAR(s.ZScore(1.0),(1-mu)/std,TEST_EPSILON);    
    double percent = 99.5;    
    double k = sqrt(1.0 / (1.0 - percent / 100.0));
    EXPECT_NEAR(s.ChebyshevMinimum(),mu -k*std,TEST_EPSILON);
    EXPECT_NEAR(s.ChebyshevMaximum(),mu +k*std,TEST_EPSILON);

   
}

TEST(Statistics,SpecialPixels) {

  Isis::Statistics t;

    double a[10];
    a[0] = 1.0;
    a[1] = 2.0;
    a[2] = 3.0;
    a[3] = NULL8;
    a[4] = HIGH_REPR_SAT8;
    a[5] = LOW_REPR_SAT8;
    a[6] = HIGH_INSTR_SAT8;
    a[7] = LOW_INSTR_SAT8;
    a[8] = 10.0;
    a[9] = -1.0;

    //if only pixels in the valid range are considered:
    double mu = 2.0;
    double std = 1.0;
    double rms = 2.16024689946929;
    double var = 1.0;
    t.AddData(a,10);

    //if all pixels (including invalid ones) are considered:
    double mu1 = 3.0;
    double std1 = 4.18330013267038;
    double rms1  = 4.79583152331272;
    double var1 = 17.5;


    t.SetValidRange(1.0,6.0);

    EXPECT_EQ(t.InRange(0.0),false);
    EXPECT_EQ(t.InRange(2.0),true);
    EXPECT_NEAR(t.ValidPixels(),5.0,TEST_EPSILON);
    EXPECT_NEAR(t.Average(),mu1,TEST_EPSILON);
    EXPECT_NEAR(t.Rms(),rms1,TEST_EPSILON);
    EXPECT_NEAR(t.Variance(),var1,TEST_EPSILON);
    EXPECT_NEAR(t.StandardDeviation(),std1,TEST_EPSILON);


    EXPECT_NEAR(t.Minimum(),-1.0,TEST_EPSILON);
    EXPECT_NEAR(t.Maximum(),10.0,TEST_EPSILON);

    EXPECT_NEAR(t.Sum(),15.0,TEST_EPSILON);
    EXPECT_NEAR(t.SumSquare(),115.0,TEST_EPSILON);
    double percent = 99.5;    
    double k = sqrt(1.0 / (1.0 - percent / 100.0));
    EXPECT_NEAR(t.ChebyshevMinimum(),mu1 -k*std1,TEST_EPSILON);
    EXPECT_NEAR(t.ChebyshevMaximum(),mu1 +k*std1,TEST_EPSILON);


    
    EXPECT_NEAR(t.BestMinimum(),-1.0,TEST_EPSILON);
    EXPECT_NEAR(t.BestMaximum(),10.0,TEST_EPSILON);

    EXPECT_NEAR(t.ValidMinimum(),1.0,TEST_EPSILON);
    EXPECT_NEAR(t.ValidMaximum(),6.0,TEST_EPSILON);

    EXPECT_NEAR(t.TotalPixels(),10.0,TEST_EPSILON);
    EXPECT_NEAR(t.ValidPixels(),5.0,TEST_EPSILON);
    EXPECT_NEAR(t.NullPixels(),1.0,TEST_EPSILON);
    EXPECT_NEAR(t.LisPixels(),1.0,TEST_EPSILON);
    EXPECT_NEAR(t.LrsPixels(),1.0,TEST_EPSILON);
    EXPECT_NEAR(t.HisPixels(),1.0,TEST_EPSILON);
    EXPECT_NEAR(t.HrsPixels(),1.0,TEST_EPSILON);

    EXPECT_NEAR(t.OutOfRangePixels(),0.0,TEST_EPSILON);
    EXPECT_NEAR(t.OverRangePixels(),0.0,TEST_EPSILON);
    EXPECT_NEAR(t.UnderRangePixels(),0.0,TEST_EPSILON);
 
}




