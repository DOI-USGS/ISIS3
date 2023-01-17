#include <nlohmann/json.hpp>

#include "FitsToJson.h"

#include "gmock/gmock.h"
#include "IException.h"

using json = nlohmann::json;
using namespace Isis;


TEST(FitsToJson, FitsConversion) {
  FileName fileTemplate = "data/lorri2isis/lor_0034974377_0x630_sci_1_cropped.fit";
  json jsonData;
  try {
    jsonData = fitsToJson(fileTemplate);
  }
  catch (IException &e) {
    FAIL() << "Unable to convert lorri fits label to json " << e.toString().toStdString().c_str() << std::endl;
  }
  // Instrument info
  EXPECT_EQ(jsonData["FitsLabels"]["HOSTNAME"]["Value"], "NEW HORIZONS");
  EXPECT_EQ(jsonData["FitsLabels"]["INSTRU"]["Value"], "lor");
  EXPECT_EQ(jsonData["FitsLabels"]["TARGET"]["Value"], "IO");
  EXPECT_EQ(jsonData["FitsLabels"]["SPCUTCAL"]["Value"], "2007-02-28T13:14:19.358");
  EXPECT_EQ(jsonData["FitsLabels"]["SPCSCLK"]["Value"], "1/0034974376:48500");
  EXPECT_EQ(jsonData["FitsLabels"]["SPCSCDP"]["Value"], "1748718848500.0");
  EXPECT_EQ(jsonData["FitsLabels"]["EXPTIME"]["Value"], "0.02");

  // Archive info
  EXPECT_EQ(jsonData["FitsLabels"]["HSCOMPR"]["Value"], "LOSSLESS");
  EXPECT_EQ(jsonData["FitsLabels"]["OBSCOMPL"]["Value"], "COMPLETE");
  EXPECT_EQ(jsonData["FitsLabels"]["REQDESC"]["Value"], "Jupiter shine");

  // Kernels info
  EXPECT_EQ(jsonData["FitsLabels"]["FORMAT"]["Value"], "0");


  fileTemplate = "data/leisa2isis/lsb_0034933739_0x53c_sci_1_cropped.fit";
  try {
    jsonData = fitsToJson(fileTemplate);
  }
  catch (IException &e) {
    FAIL() << "Unable to convert fits leisa label to json " << e.toString().toStdString().c_str() << std::endl;
  }

  // Instrument info
  EXPECT_EQ(jsonData["FitsLabels"]["HOSTNAME"]["Value"], "NEW HORIZONS");
  EXPECT_EQ(jsonData["FitsLabels"]["INSTRU"]["Value"], "lei");
  EXPECT_EQ(jsonData["FitsLabels"]["TARGET"]["Value"], "EUROPA");
  EXPECT_EQ(jsonData["FitsLabels"]["SPCSCLK0"]["Value"], "1/0034931099:00000");
  EXPECT_EQ(jsonData["FitsLabels"]["RALPHEXP"]["Value"], "0.676");

  // Archive info
  EXPECT_EQ(jsonData["FitsLabels"]["SPCSCET"]["Value"], "225897372.0736388");
  EXPECT_EQ(jsonData["FitsLabels"]["DURMET"]["Value"], "251.0");
  EXPECT_EQ(jsonData["FitsLabels"]["DETECTOR"]["Value"], "LEISA");
  EXPECT_EQ(jsonData["FitsLabels"]["SCANTYPE"]["Value"], "LEISA");

  // BandBin info
  EXPECT_EQ(jsonData["FitsLabels"]["FILTER"]["Value"], "WEDGE");

  // Kernels info
  EXPECT_EQ(jsonData["FitsLabels"]["SPCINSID"]["Value"], "-98201");
}
