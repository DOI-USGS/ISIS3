#include <QTemporaryFile>
#include <QString>
#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "Blob.h"
#include "Cube.h"
#include "Camera.h"

#include "CubeFixtures.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"

using namespace Isis;

TEST(CubeTest, TestCubeAttachSpiceFromIsd) {
  std::istringstream labelStrm(R"(
    Object = IsisCube
      Object = Core
        StartByte   = 65537
        Format      = Tile
        TileSamples = 128
        TileLines   = 128

        Group = Dimensions
          Samples = 1204
          Lines   = 1056
          Bands   = 1
        End_Group

        Group = Pixels
          Type       = UnsignedByte
          ByteOrder  = Lsb
          Base       = 0.0
          Multiplier = 1.0
        End_Group
      End_Object

      Group = Instrument
        SpacecraftName       = VIKING_ORBITER_1
        InstrumentId         = VISUAL_IMAGING_SUBSYSTEM_CAMERA_B
        TargetName           = MARS
        StartTime            = 1977-07-09T20:05:51
        ExposureDuration     = 0.008480 <seconds>
        SpacecraftClockCount = 33322515
        FloodModeId          = ON
        GainModeId           = HIGH
        OffsetModeId         = ON
      End_Group

      Group = Archive
        DataSetId       = VO1/VO2-M-VIS-2-EDR-V2.0
        ProductId       = 387A06
        MissonPhaseName = EXTENDED_MISSION
        ImageNumber     = 33322515
        OrbitNumber     = 387
      End_Group

      Group = BandBin
        FilterName = CLEAR
        FilterId   = 4
      End_Group

      Group = Kernels
        NaifFrameCode             = -27002
        LeapSecond                = $base/kernels/lsk/naif0012.tls
        TargetAttitudeShape       = $base/kernels/pck/pck00009.tpc
        TargetPosition            = (Table, $base/kernels/spk/de430.bsp,
                                     $base/kernels/spk/mar097.bsp)
        InstrumentPointing        = (Table, $viking1/kernels/ck/vo1_sedr_ck2.bc,
                                     $viking1/kernels/fk/vo1_v10.tf)
        Instrument                = Null
        SpacecraftClock           = ($viking1/kernels/sclk/vo1_fict.tsc,
                                     $viking1/kernels/sclk/vo1_fsc.tsc)
        InstrumentPosition        = (Table, $viking1/kernels/spk/viking1a.bsp)
        InstrumentAddendum        = $viking1/kernels/iak/vikingAddendum003.ti
        ShapeModel                = $base/dems/molaMarsPlanetaryRadius0005.cub
        InstrumentPositionQuality = Reconstructed
        InstrumentPointingQuality = Reconstructed
        CameraVersion             = 1
      End_Group

      Group = Reseaus
        Line     = (5, 6, 8, 9, 10, 11, 12, 13, 14, 14, 15, 133, 134, 135, 137,
                    138, 139, 140, 141, 141, 142, 143, 144, 263, 264, 266, 267,
                    268, 269, 269, 270, 271, 272, 273, 393, 393, 395, 396, 397,
                    398, 399, 399, 400, 401, 402, 403, 523, 524, 525, 526, 527,
                    527, 528, 529, 530, 530, 532, 652, 652, 654, 655, 656, 657,
                    657, 658, 659, 660, 661, 662, 781, 783, 784, 785, 786, 787,
                    788, 788, 789, 790, 791, 911, 912, 913, 914, 915, 916, 917,
                    918, 918, 919, 920, 921, 1040, 1041, 1043, 1044, 1045, 1045,
                    1046, 1047, 1047, 1048, 1050)
        Sample   = (24, 142, 259, 375, 491, 607, 723, 839, 954, 1070, 1185, 24,
                    84, 201, 317, 433, 549, 665, 780, 896, 1011, 1127, 1183, 25,
                    142, 259, 375, 492, 607, 722, 838, 953, 1068, 1183, 25, 84,
                    201, 317, 433, 549, 665, 779, 895, 1010, 1125, 1182, 25, 143,
                    259, 375, 491, 607, 722, 837, 952, 1067, 1182, 25, 84, 201,
                    317, 433, 548, 664, 779, 894, 1009, 1124, 1181, 25, 142, 258,
                    374, 490, 605, 720, 835, 951, 1066, 1180, 24, 83, 200, 316,
                    431, 547, 662, 776, 892, 1007, 1122, 1179, 23, 140, 257, 373,
                    488, 603, 718, 833, 948, 1063, 1179)
        Type     = (1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                    5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5,
                    5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5,
                    5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5,
                    5, 5, 5, 5, 5, 5, 5, 6, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6)
        Valid    = (0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
        Template = $viking1/reseaus/vo1.visb.template.cub
        Status   = Nominal
      End_Group
    End_Object
  End
  )");


  json isd = json::parse(R"(
    {"isis_camera_version": 2,
       "naif_keywords": {
            "BODY_CODE" : 499,
            "BODY499_RADII" : [3396.19, 3396.19, 3376.2],
            "BODY_FRAME_CODE" : 10014,
            "CLOCK_ET_-27999_33322515_COMPUTED" : "d5b9203a4d24c5c1",
            "INS-27002_TRANSX" : [0.0, 0.011764705882353, 0.0],
            "INS-27002_TRANSY" : [0.0, 0.0, 0.01176470588235],
            "INS-27002_ITRANSS" : [0.0, 85.0, 0.0],
            "INS-27002_ITRANSL" : [0.0, 0.0, 85.0]
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
        100
      ],
      "positions": [
        [0, 20, 0]
      ],
      "velocities": [
        [10,10,10]
      ]
    }
  })");

  Pvl label;
  labelStrm >> label;

  QTemporaryFile tempFile;
  Cube testCube;
  testCube.fromIsd(tempFile.fileName() + ".cub", label, isd, "rw");

  PvlGroup kernels = testCube.group("Kernels");

  EXPECT_TRUE(kernels.hasKeyword("InstrumentPointing"));
  EXPECT_TRUE(kernels.hasKeyword("LeapSecond"));
  EXPECT_TRUE(kernels.hasKeyword("TargetAttitudeShape"));
  EXPECT_TRUE(kernels.hasKeyword("TargetPosition"));
  EXPECT_TRUE(kernels.hasKeyword("InstrumentPointing"));
  EXPECT_TRUE(kernels.hasKeyword("Instrument"));
  EXPECT_TRUE(kernels.hasKeyword("SpacecraftClock"));
  EXPECT_TRUE(kernels.hasKeyword("InstrumentPosition"));
  EXPECT_TRUE(kernels.hasKeyword("InstrumentAddendum"));
  EXPECT_TRUE(kernels.hasKeyword("ShapeModel"));
  EXPECT_TRUE(kernels.hasKeyword("InstrumentPositionQuality"));
  EXPECT_TRUE(kernels.hasKeyword("InstrumentPointingQuality"));
  EXPECT_TRUE(kernels.hasKeyword("CameraVersion"));

  Camera *cam = testCube.camera();

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, cam->instrumentNameLong(), "Visual Imaging Subsystem Camera B");
}

TEST_F(SmallCube, TestCubeHasBlob) {
  Blob testBlob("TestBlob", "SomeBlob");
  testCube->write(testBlob);
  EXPECT_TRUE(testCube->hasBlob("TestBlob", "SomeBlob"));
  EXPECT_FALSE(testCube->hasBlob("SomeOtherTestBlob", "SomeBlob"));
}
