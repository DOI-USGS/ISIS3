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
    {"isis_camera_version": 2,
     "naif_keywords": {
           "BODY301_RADII": [ 1000, 2000, 3000 ],
           "BODY_FRAME_CODE": 31001,
           "BODY_CODE": 301,
           "INS-85600_FOCAL_LENGTH" : 699.62,
           "INS-85600_CK_FRAME_ID": -85000,
           "FRAME_-85600_NAME": "LRO_LROCNACL"
      },
    "instrument_pointing": {
      "time_dependent_frames": [-85600, -85000, 1],
      "ck_table_start_time": 100,
      "ck_table_end_time": 100.1,
      "ck_table_original_size": 2,
      "ephemeris_times": [
        100,
        100.1
      ],
      "quaternions": [
        [0.0, -0.660435174378928, 0, 0.750883067090392],
        [0.0, -0.660435174378928, 0, 0.750883067090392]
      ],
      "angular_velocity": [
        [0, 0, 0],
        [0, 0, 0]
      ],
      "constant_frames": [-85600],
      "constant_rotation": [1, 0, 0, 0, 1, 0, 0, 0, 1]
    },
    "body_rotation": {
      "time_dependent_frames": [31006, 1],
      "ck_table_start_time": 100,
      "ck_table_end_time": 100.1,
      "ck_table_original_size": 2,
      "ephemeris_times": [
        100,
        100.1
      ],
      "quaternions": [
        [ 0, 0.8509035, 0, 0.525322 ],
        [ 0, 0.8509035, 0, 0.525322 ]
      ],
      "angular_velocity": [
        [0, 0, 0],
        [0, 0, 0]
      ],
      "constant_frames": [31001, 31007, 31006],
      "constant_rotation": [-0.4480736,  0,  0.8939967, 0,  1,  0, -0.8939967,  0, -0.4480736]
    },
    "instrument_position": {
      "spk_table_start_time": 100,
      "spk_table_end_time": 100.1,
      "spk_table_original_size": 2,
      "ephemeris_times": [
        100,
        100.1
      ],
      "positions": [
        [1000, 0, 0],
        [1000, 0, 0]
      ],
      "velocities": [
        [0, 0, 0],
        [0, 0, 0]
      ]
    },
    "sun_position": {
      "spk_table_start_time": 100,
      "spk_table_end_time": 100.1,
      "spk_table_original_size": 2,
      "ephemeris_times": [
        100,
        100.1
      ],
      "positions": [
        [0, 20, 0]
      ],
      "velocities": [
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

