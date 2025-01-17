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

TEST(CubeTest, TestCubeAttachLineScanTableFromIsd) {
  std::istringstream labelStrm(R"(
   Object = IsisCube
    Object = Core
      StartByte   = 65537
      Format      = Tile
      TileSamples = 322
      TileLines   = 368

      Group = Dimensions
        Samples = 1288
        Lines   = 15088
        Bands   = 1
      End_Group

      Group = Pixels
        Type       = Real
        ByteOrder  = Lsb
        Base       = 0.0
        Multiplier = 1.0
      End_Group
    End_Object

    Group = Instrument
      SpacecraftName            = "MARS EXPRESS"
      InstrumentId              = HRSC
      StartTime                 = 2008-02-08T12:08:53.843
      StopTime                  = 2008-02-08T12:12:10.561
      SpacecraftClockStartCount = 1/0150552525.07284
      SpacecraftClockStopCount  = 1/0150552792.64947
      MissionPhaseName          = ME_Phase_11
      TargetName                = Mars
      Summing                   = 4
      FocalPlaneTemperature     = 7.9716 <degC>
      LensTemperature           = 8.1755 <degC>
      InstrumentTemperature     = 11.0301 <degC>
    End_Group

    Group = Archive
      DataSetId   = MEX-M-HRSC-3-RDR-V2.0
      DetectorId  = MEX_HRSC_IR
      EventType   = MARS-REGIONAL-MAPPING-Vo-Te-Im
      OrbitNumber = 5270
      ProductId   = H5270_0000_IR2.IMG
    End_Group

    Group = BandBin
      Width  = 81.0 <nm>
      Center = 955.5 <nm>
    End_Group

    Group = Kernels
      NaifIkCode                = -41218
      LeapSecond                = $base/kernels/lsk/naif0012.tls
      TargetAttitudeShape       = $base/kernels/pck/pck00009.tpc
      TargetPosition            = (Table, $base/kernels/spk/de430.bsp,
                                  $base/kernels/spk/mar097.bsp)
      InstrumentPointing        = (Table,
                                  $mex/kernels/ck/ATNM_MEASURED_080101_090101_V-
                                  03.BC, $mex/kernels/fk/MEX_V16.TF)
      Instrument                = $mex/kernels/ik/MEX_HRSC_V09.TI
      SpacecraftClock           = $mex/kernels/sclk/MEX_220705_STEP.TSC
      InstrumentPosition        = (Table,
                                  $mex/kernels/spk/ORMM__080201000000_00474.BSP)
      InstrumentAddendum        = $mex/kernels/iak/hrscAddendum004.ti
      ShapeModel                = $base/dems/molaMarsPlanetaryRadius0005.cub
      InstrumentPositionQuality = Reconstructed
      InstrumentPointingQuality = Reconstructed
      CameraVersion             = 1
      Source                    = isis
    End_Group
  End_Object

  Object = Label
    Bytes = 65536
  End_Object

  Object = Table
    Name      = LineScanTimes
    StartByte = 77798913
    Bytes     = 60
    Records   = 3
    ByteOrder = Lsb

    Group = Field
      Name = EphemerisTime
      Type = Double
      Size = 1
    End_Group

    Group = Field
      Name = ExposureTime
      Type = Double
      Size = 1
    End_Group

    Group = Field
      Name = LineStart
      Type = Integer
      Size = 1
    End_Group
  End_Object

  Object = NaifKeywords
    BODY_CODE                  = 499
    BODY499_RADII              = (3396.19, 3396.19, 3376.2)
    BODY_FRAME_CODE            = 10014
    INS-41218_FOCAL_LENGTH     = 174.82
    INS-41218_TRANSX           = (0.016461898406507, -0.006999999322408,
                                  3.079982431615e-06)
    INS-41218_TRANSY           = (49.791792756805, 3.079982431615e-06,
                                  0.006999999322408)
    INS-41218_ITRANSS          = (-0.77805243343811, -142.85712902873,
                                  0.062856784318668)
    INS-41218_ITRANSL          = (-7113.1135971726, 0.062856784318668,
                                  142.85712902873)
    INS-41218_BORESIGHT_SAMPLE = 2592.5
    INS-41218_BORESIGHT_LINE   = 0.0
  End_Object
  End
  )");

  json isd = json::parse(R"({
  "isis_camera_version": 1,
  "image_lines": 15088,
  "image_samples": 1288,
  "name_platform": "MARS EXPRESS",
  "name_sensor": "HIGH RESOLUTION STEREO CAMERA",
  "reference_height": {
    "maxheight": 1000,
    "minheight": -1000,
    "unit": "m"
  },
  "name_model": "USGS_ASTRO_LINE_SCANNER_SENSOR_MODEL",
  "interpolation_method": "lagrange",
  "line_scan_rate": [
    [
      0.5,
      -98.35948351025581,
      0.012800790786743165
    ],
    [
      6665.5,
      98.34625607728958,
      0.013227428436279297
    ]
  ],
  "starting_ephemeris_time": 255744599.02748165,
  "center_ephemeris_time": 255744697.38696516,
  "radii": {
    "semimajor": 3396.19,
    "semiminor": 3376.2,
    "unit": "km"
  },
  "body_rotation": {
    "time_dependent_frames": [
      10014,
      1
    ],
    "ck_table_start_time": 255744599.02748165,
    "ck_table_end_time": 255744684.34504557,
    "ck_table_original_size": 2,
    "ephemeris_times": [
      255744599.02748165,
      255744684.34504557
    ],
    "quaternions": [
      [
        -0.6525755651775765,
        -0.023151423894873242,
        0.3174415084303075,
        -0.6876336466682659
      ],
      [
        -0.654651809237426,
        -0.02219145654977925,
        0.31751006120710407,
        -0.6856572824309715
      ]
    ],
    "angular_velocities": [
      [
        3.162398161513709e-05,
        -2.8803031775991535e-05,
        5.6520727317788564e-05
      ],
      [
        3.162398161489585e-05,
        -2.8803031778668454e-05,
        5.652072731655937e-05
      ]
    ],
    "reference_frame": 1
  },
  "instrument_pointing": {
    "time_dependent_frames": [
      -41001,
      1
    ],
    "ck_table_start_time": 255744599.02748165,
    "ck_table_end_time": 255744684.34504557,
    "ck_table_original_size": 3,
    "ephemeris_times": [
      255744599.02748165,
      255744684.33197814,
      255744684.34504557
    ],
    "quaternions": [
      [
        -0.34147103254256284,
        0.4600620001156389,
        -0.4826410643063962,
        -0.662418367068051
      ],
      [
        -0.3659838104244632,
        0.44394513387664625,
        -0.4497912009709326,
        -0.6831225689022163
      ],
      [
        -0.3659874882249891,
        0.4439426152453299,
        -0.4497860930062306,
        -0.6831255985322837
      ]
    ],
    "angular_velocities": [
      [
        0.0003517633111319437,
        0.001015465002260446,
        0.00038771759258420565
      ],
      [
        0.00035178320677913466,
        0.001008400741447663,
        0.0003885676589327302
      ],
      [
        0.0003517842276725049,
        0.0010084000654699077,
        0.00038856810918190424
      ]
    ],
    "reference_frame": 1,
    "constant_frames": [
      -41210,
      -41200,
      -41000,
      -41001
    ],
    "constant_rotation": [
      -0.9999999844629888,
      1.027590578527487e-06,
      0.00017627525841189352,
      1.2246232944813223e-16,
      -0.9999830090976747,
      0.00582936668603668,
      0.0001762782535384808,
      0.0058293665954657434,
      0.9999829935609271
    ]
  },
  "naif_keywords": {
    "BODY499_RADII": [
      3396.19,
      3396.19,
      3376.2
    ],
    "BODY_FRAME_CODE": 10014,
    "BODY_CODE": 499,
    "INS-41210_FOV_FRAME": "MEX_HRSC_HEAD",
    "FRAME_-41210_NAME": "MEX_HRSC_HEAD",
    "INS-41210_CK_TIME_TOLERANCE": 1.0,
    "TKFRAME_-41210_AXES": [
      1.0,
      2.0,
      3.0
    ],
    "TKFRAME_-41210_SPEC": "ANGLES",
    "FRAME_-41210_CLASS": 4.0,
    "INS-41210_FOV_ANGULAR_SIZE": [
      0.2,
      0.659734
    ],
    "INS-41210_OD_K": [
      0.0,
      0.0,
      0.0
    ],
    "INS-41210_F/RATIO": 5.6,
    "INS-41210_PLATFORM_ID": -41000.0,
    "TKFRAME_-41210_ANGLES": [
      -0.334,
      0.0101,
      0.0
    ],
    "INS-41210_SPK_TIME_BIAS": 0.0,
    "FRAME_-41210_CENTER": -41.0,
    "TKFRAME_-41210_UNITS": "DEGREES",
    "INS-41210_BORESIGHT": [
      0.0,
      0.0,
      175.0
    ],
    "INS-41210_CK_TIME_BIAS": 0.0,
    "FRAME_-41210_CLASS_ID": -41210.0,
    "INS-41210_IFOV": 4e-05,
    "INS-41210_FOV_BOUNDARY_CORNERS": [
      18.187,
      60.0641,
      175.0,
      18.1281,
      -60.0399,
      175.0,
      -18.1862,
      -60.0435,
      175.0,
      -18.142
    ],
    "INS-41210_FOV_SHAPE": "RECTANGLE",
    "TKFRAME_-41210_RELATIVE": "MEX_HRSC_BASE",
    "INS-41210_PIXEL_PITCH": 0.007,
    "INS-41210_FOCAL_LENGTH": 175.0,
    "BODY499_POLE_DEC": [
      52.8865,
      -0.0609,
      0.0
    ],
    "BODY499_POLE_RA": [
      317.68143,
      -0.1061,
      0.0
    ],
    "BODY499_PM": [
      176.63,
      350.89198226,
      0.0
    ],
    "INS-41218_ITRANSL": [
      -7113.11359717265,
      0.062856784318668,
      142.857129028729
    ],
    "INS-41218_ITRANSS": [
      -0.778052433438109,
      -142.857129028729,
      0.062856784318668
    ],
    "INS-41218_FOV_SHAPE": "RECTANGLE",
    "INS-41218_PIXEL_SIZE": [
      7.0,
      7.0
    ],
    "INS-41218_CK_REFERENCE_ID": 1.0,
    "INS-41218_FOV_FRAME": "MEX_HRSC_HEAD",
    "INS-41218_CCD_CENTER": [
      2592.5,
      0.5
    ],
    "INS-41218_CK_FRAME_ID": -41001.0,
    "INS-41218_F/RATIO": 5.6,
    "INS-41218_PIXEL_SAMPLES": 5184.0,
    "INS-41218_BORESIGHT_SAMPLE": 2592.5,
    "INS-41218_FILTER_BANDWIDTH": 90.0,
    "INS-41218_BORESIGHT_LINE": 0.0,
    "INS-41218_PIXEL_LINES": 1.0,
    "INS-41218_FOCAL_LENGTH": 174.82,
    "INS-41218_FOV_ANGULAR_SIZE": [
      0.2,
      4e-05
    ],
    "INS-41218_FILTER_BANDCENTER": 970.0,
    "INS-41218_TRANSX": [
      0.016461898406507,
      -0.006999999322408,
      3.079982431615e-06
    ],
    "INS-41218_TRANSY": [
      49.7917927568053,
      3.079982431615e-06,
      0.006999999322408
    ],
    "INS-41218_FOV_BOUNDARY_CORNERS": [
      18.1982,
      49.9121,
      175.0,
      18.1982,
      49.9051,
      175.0,
      -18.1693,
      49.8901,
      175.0,
      -18.1693
    ],
    "INS-41218_BORESIGHT": [
      0.0151,
      49.9039,
      175.0
    ],
    "INS-41218_IFOV": 4e-05
  },
  "detector_sample_summing": 4,
  "detector_line_summing": 4,
  "focal_length_model": {
    "focal_length": 174.82
  },
  "detector_center": {
    "line": 0.0,
    "sample": 2592.0
  },
  "focal2pixel_lines": [
    -7113.11359717265,
    0.062856784318668,
    142.857129028729
  ],
  "focal2pixel_samples": [
    -0.778052433438109,
    -142.857129028729,
    0.062856784318668
  ],
  "optical_distortion": {
    "radial": {
      "coefficients": [
        0.0,
        0.0,
        0.0
      ]
    }
  },
  "starting_detector_line": 0,
  "starting_detector_sample": 0,
  "instrument_position": {
    "spk_table_start_time": 255744599.02748165,
    "spk_table_end_time": 255744684.34504557,
    "spk_table_original_size": 3,
    "ephemeris_times": [
      255744599.02748165,
      255744684.33197814,
      255744684.34504557
    ],
    "positions": [
      [
        3508.7678823246983,
        -1180.0905763309427,
        -404.65807247785085
      ],
      [
        3504.3243485204953,
        -1050.5094345826292,
        -743.2474663998983
      ],
      [
        3504.322049673533,
        -1050.4890790873103,
        -743.2990454930555
      ]
    ],
    "velocities": [
      [
        0.07204007846693458,
        1.4787375689455564,
        -3.9872650786018093
      ],
      [
        -0.17588977444273826,
        1.5577275009096896,
        -3.9471504887872113
      ],
      [
        -0.17592754964964083,
        1.5577388421738825,
        -3.947142539138405
      ]
    ],
    "reference_frame": 1
  },
  "sun_position": {
    "spk_table_start_time": 255744599.02748165,
    "spk_table_end_time": 255744684.34504557,
    "spk_table_original_size": 2,
    "ephemeris_times": [
      255744599.02748165,
      255744684.34504557
    ],
    "positions": [
      [
        99136929.53828166,
        -200428537.63629806,
        -94608622.07352574
      ],
      [
        99138738.20092882,
        -200427911.89904302,
        -94608383.92687146
      ]
    ],
    "velocities": [
      [
        21.199222172569485,
        7.334134527307004,
        2.791259509826079
      ],
      [
        21.199143743332474,
        7.334293076913804,
        2.791334350681398
      ]
    ],
    "reference_frame": 1
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

  EXPECT_TRUE(testCube.hasTable("LineScanTimes"));

}

TEST_F(SmallCube, TestCubeHasBlob) {
  Blob testBlob("TestBlob", "SomeBlob");
  testCube->write(testBlob);
  EXPECT_TRUE(testCube->hasBlob("TestBlob", "SomeBlob"));
  EXPECT_FALSE(testCube->hasBlob("SomeOtherTestBlob", "SomeBlob"));
}
