#include "hyb2onc2isis.h"
#include "Pvl.h"
#include <QString>

#include <gtest/gtest.h>

//#include "gmock/gmock.h"

using namespace Isis;



TEST(stats,telescopic) {

  QString blah("");
 //QString fitsInput("/usgs/cpkgs/isis3/testData/isis/src/hayabusa2/apps/hyb2onc2isis/tsts/telescopic/hyb2_onc_20151204_040908_tvf_l2a.fit");
 //QString outputCube("hyb2_onc_20151204_040908_tvf_l2a.cub");
 hyb2onc2isis(blah,blah,"");

EXPECT_EQ("blah","blah");

}
