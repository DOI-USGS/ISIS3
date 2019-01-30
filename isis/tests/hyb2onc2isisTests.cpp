#include "hyb2onc2isis.h"
#include "FileName.h"
#include "CubeAttribute.h"
#include "Pvl.h"

#include <QDebug>
#include <QString>


#include <cstdio>
#include <iostream>
#include <sstream>

#include <gtest/gtest.h>
//#include "gmock/gmock.h"

using namespace Isis;
using namespace std;


//QString testDataPath("/usgs/cpkgs/isis3/testData/isis/src");

QString testDataPath=("/scratch/isis3hayabusa2/tsts");


TEST(hyb2onc2isis,w1) {

  //QString w1Fits(testDataPath+"/hayabusa2/apps/hyb2onc2isis/tsts/w1/input/hyb2_onc_20151204_041027_w1f_l2a.fit");
  QString w1Fits=(testDataPath+"/w1/input/hyb2_onc_20151204_041027_w1f_l2a.fit");
  QString outputCube("temp.cub");

  CubeAttributeOutput att;
  att.addAttribute("real");
  Pvl outputLabel = hyb2onc2isis(w1Fits, outputCube,att);


  ofstream w1pvl;
  w1pvl << outputLabel;

  ifstream truthpvl(testDataPath.toStdString()+"/w1/truth/labels.pvl");
  stringstream buffer;
  buffer << w1pvl.rdbuf();
  string w1pvlstr = buffer.str();

  //Flush the buffer
  buffer.str(std::string());

  //Read the truth data Pvl file
  buffer << truthpvl.rdbuf();
  string truthpvlstr = buffer.str();

  remove("temp.cub");


  EXPECT_EQ(w1pvlstr,truthpvlstr);

}
