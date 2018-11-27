#include "Statistics.h" 

#include "FileName.h"
#include "IException.h"
#include "Preference.h"
#include "Project.h"
#include "Statistics.h"
#include "XmlStackedHandlerReader.h"

#include <QDebug>
#include <QDataStream>
#include <QFile>
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



class StatisticsXmlHandlerTester : public Statistics {
    public:
      StatisticsXmlHandlerTester(Project *project, XmlStackedHandlerReader *reader, 
                                     FileName xmlFile) : Statistics(project, reader) {

        QString xmlPath(xmlFile.expanded());
        QFile file(xmlPath);

        if (!file.open(QFile::ReadOnly) ) {
          throw IException(IException::Io,
                           QString("Unable to open xml file, [%1],  with read access").arg(xmlPath),
                           _FILEINFO_);
        }

        QXmlInputSource xmlInputSource(&file);
        bool success = reader->parse(xmlInputSource);
        if (!success) {
          throw IException(IException::Unknown, 
                           QString("Failed to parse xml file, [%1]").arg(xmlPath),
                            _FILEINFO_);
        }

      }

      ~StatisticsXmlHandlerTester() {
      }

  };




TEST(Statistics, HandCalculations) {

    Statistics s;
    
    s.AddData(-10.0);
    s.AddData(20.0);
    s.AddData(30.0);
    

    //Values were computed in R
    double mu = 13.333333333333334;
    double std = 20.816659994661325;
    double var = 433.33333333333331;
    
    double rms = 21.602468994692867;


    EXPECT_DOUBLE_EQ(s.Average(),mu);
    EXPECT_DOUBLE_EQ(s.Variance(),var);
    EXPECT_DOUBLE_EQ(s.StandardDeviation(),std);
    EXPECT_DOUBLE_EQ(s.Sum(),40.0);
    EXPECT_DOUBLE_EQ(s.SumSquare(),1400.0);
    EXPECT_DOUBLE_EQ(s.ValidPixels(),3.0);
    EXPECT_DOUBLE_EQ(s.Rms(),rms);    
    EXPECT_DOUBLE_EQ(s.Minimum(),-10.0);
    EXPECT_DOUBLE_EQ(s.Maximum(),30.0);
    EXPECT_DOUBLE_EQ(s.ZScore(1.0),(1-mu)/std);    
    double percent = 99.5;    
    double k = sqrt(1.0 / (1.0 - percent / 100.0));
    EXPECT_DOUBLE_EQ(s.ChebyshevMinimum(),mu -k*std);
    EXPECT_DOUBLE_EQ(s.ChebyshevMaximum(),mu +k*std);

   
}

TEST(Statistics,SpecialPixels) {

     Statistics t;

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
    EXPECT_EQ(t.AboveRange(7.0),true);
    EXPECT_EQ(t.AboveRange(6.0),false);
    EXPECT_EQ(t.BelowRange(0.0),true);
    EXPECT_EQ(t.BelowRange(1.0),false);

    EXPECT_DOUBLE_EQ(t.ValidPixels(),5.0);
    EXPECT_DOUBLE_EQ(t.Average(),mu1);
    EXPECT_DOUBLE_EQ(t.Rms(),rms1);
    EXPECT_DOUBLE_EQ(t.Variance(),var1);
    EXPECT_DOUBLE_EQ(t.StandardDeviation(),std1);


    EXPECT_DOUBLE_EQ(t.Minimum(),-1.0);
    EXPECT_DOUBLE_EQ(t.Maximum(),10.0);

    EXPECT_DOUBLE_EQ(t.Sum(),15.0);
    EXPECT_DOUBLE_EQ(t.SumSquare(),115.0);
    double percent = 99.5;    
    double k = sqrt(1.0 / (1.0 - percent / 100.0));
    EXPECT_DOUBLE_EQ(t.ChebyshevMinimum(),mu1 -k*std1);
    EXPECT_DOUBLE_EQ(t.ChebyshevMaximum(),mu1 +k*std1);


    
    EXPECT_DOUBLE_EQ(t.BestMinimum(),-1.0);
    EXPECT_DOUBLE_EQ(t.BestMaximum(),10.0);

    EXPECT_DOUBLE_EQ(t.ValidMinimum(),1.0);
    EXPECT_DOUBLE_EQ(t.ValidMaximum(),6.0);

    EXPECT_DOUBLE_EQ(t.TotalPixels(),10.0);
    EXPECT_DOUBLE_EQ(t.ValidPixels(),5.0);
    EXPECT_DOUBLE_EQ(t.NullPixels(),1.0);
    EXPECT_DOUBLE_EQ(t.LisPixels(),1.0);
    EXPECT_DOUBLE_EQ(t.LrsPixels(),1.0);
    EXPECT_DOUBLE_EQ(t.HisPixels(),1.0);
    EXPECT_DOUBLE_EQ(t.HrsPixels(),1.0);

    EXPECT_DOUBLE_EQ(t.OutOfRangePixels(),0.0);
    EXPECT_DOUBLE_EQ(t.OverRangePixels(),0.0);
    EXPECT_DOUBLE_EQ(t.UnderRangePixels(),0.0);
 
}



TEST(Statistics,XMLReadWrite) {


    Statistics s;    
    s.AddData(-10.0);
    s.AddData(20.0);
    s.AddData(30.0);

    //Values were computed in R
    double mu = 13.333333333333334;
    double std = 20.816659994661325;
    double var = 433.33333333333331;
    
    double rms = 21.602468994692867;
 
    FileName xmlFile("./Statistics.xml");
    QString xmlPath = xmlFile.expanded();
    QFile qXmlFile(xmlPath);

    ASSERT_EQ(qXmlFile.open(QIODevice::WriteOnly|QIODevice::Text),true);

    QXmlStreamWriter writer(&qXmlFile);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    Project *project = NULL;
    s.save(writer, project);
    writer.writeEndDocument();
    qXmlFile.close();

    XmlStackedHandlerReader reader;
    StatisticsXmlHandlerTester statsFromXml(project, &reader, xmlFile);

    EXPECT_DOUBLE_EQ(s.Average(),mu);
    EXPECT_DOUBLE_EQ(s.Variance(),var);
    EXPECT_DOUBLE_EQ(s.StandardDeviation(),std);
    EXPECT_DOUBLE_EQ(s.Sum(),40.0);
    EXPECT_DOUBLE_EQ(s.SumSquare(),1400.0);
    EXPECT_DOUBLE_EQ(s.ValidPixels(),3.0);
    EXPECT_DOUBLE_EQ(s.Rms(),rms);    
    EXPECT_DOUBLE_EQ(s.Minimum(),-10.0);
    EXPECT_DOUBLE_EQ(s.Maximum(),30.0);
    EXPECT_DOUBLE_EQ(s.ZScore(1.0),(1-mu)/std);    
    double percent = 99.5;    
    double k = sqrt(1.0 / (1.0 - percent / 100.0));
    EXPECT_DOUBLE_EQ(s.ChebyshevMinimum(),mu -k*std);
    EXPECT_DOUBLE_EQ(s.ChebyshevMaximum(),mu +k*std);

}




