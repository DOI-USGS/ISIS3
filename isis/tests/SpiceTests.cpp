#include "Spice.h"
#include "IException.h"
#include "Pvl.h"
#include "Distance.h"
#include "iTime.h"
#include "Longitude.h"

#include <QString>
#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "gmock/gmock.h"

using namespace Isis;

class ConstVelIsd : public ::testing::Test {
  protected:
  json constVelIsdStr; 
  Pvl isisLabel; 

  void SetUp() {
    constVelIsdStr = json::parse(R"(
    {"CameraVersion": 2,
         "NaifKeywords": {
           "BODY301_RADII": [ 1000, 2000, 3000 ],
           "BODY_FRAME_CODE": 31001,
           "BODY_CODE": 301,
           "INS-85600_FOCAL_LENGTH" : 699.62,
           "INS-85600_CK_FRAME_ID": -85000,
           "FRAME_-85600_NAME": "LRO_LROCNACL"
       },
    "InstrumentPointing": {
      "TimeDependentFrames": [-85600, -85000, 1],
      "CkTableStartTime": 100,
      "CkTableEndTime": 100.1,
      "CkTableOriginalSize": 2,
      "EphemerisTimes": [
        100,
        100.1
      ],
      "Quaternions": [
        [0.0, -0.660435174378928, 0, 0.750883067090392],
        [0.0, -0.660435174378928, 0, 0.750883067090392]
      ],
      "AngularVelocity": [
        [0, 0, 0],
        [0, 0, 0]
      ],
      "ConstantFrames": [-85600],
      "ConstantRotation": [1, 0, 0, 0, 1, 0, 0, 0, 1]
    },
    "BodyRotation": {
      "TimeDependentFrames": [31006, 1],
      "CkTableStartTime": 100,
      "CkTableEndTime": 100.1,
      "CkTableOriginalSize": 2,
      "EphemerisTimes": [
        100,
        100.1
      ],
      "Quaternions": [
        [ 0, 0.8509035, 0, 0.525322 ],
        [ 0, 0.8509035, 0, 0.525322 ]
      ],
      "AngularVelocity": [
        [0, 0, 0],
        [0, 0, 0]
      ],
      "ConstantFrames": [31001, 31007, 31006],
      "ConstantRotation": [-0.4480736,  0,  0.8939967, 0,  1,  0, -0.8939967,  0, -0.4480736]
    },
    "InstrumentPosition": {
      "SpkTableStartTime": 100,
      "SpkTableEndTime": 100.1,
      "SpkTableOriginalSize": 2,
      "EphemerisTimes": [
        100,
        100.1
      ],
      "Positions": [
        [1000, 0, 0],
        [1000, 0, 0]
      ],
      "Velocities": [
        [0, 0, 0],
        [0, 0, 0]
      ]
    },
    "SunPosition": {
      "SpkTableStartTime": 100,
      "SpkTableEndTime": 100.1,
      "SpkTableOriginalSize": 2,
      "EphemerisTimes": [
        100,
        100.1
      ],
      "Positions": [
        [0, 20, 0]
      ],
      "Velocities": [
        [10,10,10]
      ]
    }
  })");

  std::istringstream isisLabelStr(R"(
    Object = IsisCube
      Object = Core
        StartByte   = 65537
        Format      = Tile
        TileSamples = 128
        TileLines   = 128
        
        Group = Dimensions
          Samples = 126
          Lines   = 126
          Bands   = 2
        End_Group

        Group = Pixels
          Type       = Real
          ByteOrder  = Lsb
          Base       = 0.0
          Multiplier = 1.0
        End_Group
      End_Object

      Group = Kernels
          NaifFrameCode = 310019
          LeapSecond                = NULL
          TargetAttitudeShape       = NULL
          TargetPosition            = NULL
          InstrumentPointing        = NULL
          Instrument                = NULL 
          SpacecraftClock           = NULL 
          InstrumentPosition        = NULL
          InstrumentAddendum        = NULL
          ShapeModel                = NULL
          InstrumentPositionQuality = NULL
          InstrumentPointingQuality = NULL
          CameraVersion             = NULL
      End_Group

      Group = Instrument
          SpacecraftName = NULL  
          InstrumentId   = NULL 
          TargetName     = NULL 
      End_Group
    End_Object

    Object = Label
      Bytes = 65536
    End_Object

    Object = History
      Name      = IsisCube
      StartByte = 196609
      Bytes     = 695
    End_Object
    End
  )"); 
  
  isisLabelStr >> isisLabel;

  }
};

TEST_F(ConstVelIsd, TestSpiceFromIsd) {
  Spice testSpice(isisLabel, constVelIsdStr); 
  testSpice.setTime(100); 
  
  EXPECT_DOUBLE_EQ(testSpice.time().Et(), 100);

  EXPECT_DOUBLE_EQ(testSpice.getDouble("INS-85600_FOCAL_LENGTH"), 699.62);
  EXPECT_STREQ(testSpice.getString("FRAME_-85600_NAME").toStdString().c_str(), "LRO_LROCNACL");
  EXPECT_EQ(testSpice.getInteger("INS-85600_CK_FRAME_ID"), -85000);
  
  Distance radii[3];
  testSpice.radii(radii);
  EXPECT_DOUBLE_EQ(radii[0].kilometers(), 1000);
  EXPECT_DOUBLE_EQ(radii[1].kilometers(), 2000);
  EXPECT_DOUBLE_EQ(radii[2].kilometers(), 3000);
  
  EXPECT_DOUBLE_EQ(testSpice.solarLongitude().positiveEast(), 3.1415926535897931);
  
}

TEST_F(ConstVelIsd, SunToBodyDist) {   
  Spice testSpice(isisLabel, constVelIsdStr); 
  EXPECT_DOUBLE_EQ(testSpice.sunToBodyDist(), 20);
}

