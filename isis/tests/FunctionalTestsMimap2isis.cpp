#include <QString>
#include <QTemporaryDir>

#include "mimap2isis.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "TestUtilities.h"
#include "Histogram.h"

#include "gtest/gtest.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/mimap2isis.xml").expanded();

TEST(FunctionalTestMimap2Isis, Default) {
   QTemporaryDir prefix;
   QString cubeFileName = prefix.path() + "/mimap2isisTEMP.cub";
   QVector<QString> args = {"from=data/mimap2isis/MI_MAP_02_N65E328N64E329SC_cropped.img", "to=" + cubeFileName};

  UserInterface options(APP_XML, args);
  try {
   mimap2isis(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest MI MAP image: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Dimensions Group
  PvlGroup &dimensions = isisLabel->findGroup("Dimensions", Pvl::Traverse);
  EXPECT_EQ(int(dimensions["Samples"]), 5);
  EXPECT_EQ(int(dimensions["Lines"]), 5);
  EXPECT_EQ(int(dimensions["Bands"]), 9);

  // Pixels Group
  PvlGroup &pixels = isisLabel->findGroup("Pixels", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, pixels.findKeyword("Type"), "SignedWord");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, pixels.findKeyword("ByteOrder"), "Lsb");
  EXPECT_DOUBLE_EQ((double) pixels.findKeyword("Base"), 0.0);
  EXPECT_DOUBLE_EQ((double) pixels.findKeyword("Multiplier"), 2.0e-05);

  // Instrument Group
  PvlGroup &inst = isisLabel->findGroup("Instrument", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, inst.findKeyword("SpacecraftName"), "KAGUYA");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, inst.findKeyword("InstrumentName"), "Multiband Imager");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, inst.findKeyword("InstrumentId"), "MI");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, inst.findKeyword("TargetName"), "MOON");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, inst.findKeyword("ObservationModeId"), "NORMAL");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, inst.findKeyword("SensorDescription"),
                                           "MI is a multiband push-broom imaging camera consisting of VIS(V) and NIR(N) sensors (each has nadir-directed optics of f number 65 mm and F ratio 3.7). Detector pixel sizes in micron are 13(V) and 40(N).");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, inst.findKeyword("SensorDescription2"),
                                           "Physical band arrangement [from satellite -x to +x] are VIS1>VIS2>VIS5>VIS4>VIS3 and NIR3>NIR4>NIR1>NIR2. Parallax between nearest band sets [degree] are 2.7 for VIS and 2.6 for NIR. Sampling time [msec] are 13 for VIS and 39 for NIR.");

  // Archive Group
  PvlGroup &archive = isisLabel->findGroup("Archive", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SoftwareName"), "RGC_TC_MI");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SoftwareVersion"), "2.10.1");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProcessVersionId"), "MAP");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProductCreationTime"), "2011-10-25T04:31:02");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProgramStartTime"), "2011-10-25T04:25:07");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProducerId"), "LISM");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProductSetId"), "MI_MAP");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProductVersionId"), "02");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("RegisteredProduct"), "Y");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("Level2AFileName")[0], "MV52A0_02NM04884_004_0030.img");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("Level2AFileName")[1], "MV52A0_02NM04883_004_0030.img");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("Level2AFileName")[2], "MV52A0_02NM04884_004_0029.img");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("Level2AFileName")[3], "MV52A0_02NM04883_004_0029.img");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("Level2AFileName")[4], "MV52A0_02NM04884_004_0028.img");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("Level2AFileName")[5], "MV52A0_02NM04883_004_0028.img");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("Level2AFileName")[6], "{MV22A0_02NL01385_002_0045.img, MV22A0_02NL01385_002_0044.img}");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("Level2AFileName")[7], "{MV22A0_02NL01384_003_0045.img, MV22A0_02NL01384_003_0046.img}");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("Level2AFileName")[8], "{MV22A0_02NL01385_002_0046.img, MV22A0_02NL01385_002_0045.img}");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("Level2AFileName")[9], "{MV22A0_02NL01384_003_0047.img, MV22A0_02NL01384_003_0046.img}");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("Level2AFileName")[10], "{MV22A0_02NL01385_002_0046.img, MV22A0_02NL01385_002_0047.img}");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SpiceMetakernelFileName")[0], "RGC_INF_TCv401IK_MIv200IK_SPv105IK_RISE100i_05_LongCK_D_V02_de421_110706.mk");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SpiceMetakernelFileName")[1], "RGC_INF_TCv401IK_MIv200IK_SPv105IK_RISE100i_05_LongCK_D_V02_de421_110706.mk");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SpiceMetakernelFileName")[2], "RGC_INF_TCv401IK_MIv200IK_SPv105IK_RISE100i_05_LongCK_D_V02_de421_110706.mk");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SpiceMetakernelFileName")[3], "RGC_INF_TCv401IK_MIv200IK_SPv105IK_RISE100i_05_LongCK_D_V02_de421_110706.mk");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SpiceMetakernelFileName")[4], "RGC_INF_TCv401IK_MIv200IK_SPv105IK_RISE100i_05_LongCK_D_V02_de421_110706.mk");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SpiceMetakernelFileName")[5], "RGC_INF_TCv401IK_MIv200IK_SPv105IK_RISE100i_05_LongCK_D_V02_de421_110706.mk");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SpiceMetakernelFileName")[6], "RGC_INF_TCv401IK_MIv200IK_SPv105IK_RISE100i_04_LongCK_D_de421_101125.mk");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SpiceMetakernelFileName")[7], "RGC_INF_TCv401IK_MIv200IK_SPv105IK_RISE100i_04_LongCK_D_de421_101125.mk");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SpiceMetakernelFileName")[8], "RGC_INF_TCv401IK_MIv200IK_SPv105IK_RISE100i_04_LongCK_D_de421_101125.mk");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SpiceMetakernelFileName")[9], "RGC_INF_TCv401IK_MIv200IK_SPv105IK_RISE100i_04_LongCK_D_de421_101125.mk");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SpiceMetakernelFileName")[10], "RGC_INF_TCv401IK_MIv200IK_SPv105IK_RISE100i_04_LongCK_D_de421_101125.mk");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("DataSetId"), "MI_MAP");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ImageValueType"), "REFLECTANCE");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ImageUnit"), "ND");

  for (int i = 0; i < 9; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("MinForStatisticalEvaluation")[i].toDouble(), 0) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ((double) archive.findKeyword("MaxForStatisticalEvaluation")[i].toDouble(), 32767) << "Error at index: " << i;
  }

  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[0].toDouble(), 32268);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[1].toDouble(), 32178);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[2].toDouble(), 32562);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[3].toDouble(), 31727);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[4].toDouble(), 32684);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[5].toDouble(), 32528);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[6].toDouble(), 32179);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[7].toDouble(), 32293);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[8].toDouble(), 32433);

  for (int i = 0; i < 7; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[i].toDouble(), 0) << "Error at index: " << i;;
  }
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[8].toDouble(), 1);

  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[0].toDouble(), 3096.5);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[1].toDouble(), 5582.3);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[2].toDouble(), 5993.3);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[3].toDouble(), 6101.0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[4].toDouble(), 6480.5);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[5].toDouble(), 6425.4);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[6].toDouble(), 6767.4);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[7].toDouble(), 8075.3);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[8].toDouble(), 9526.2);

  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[0].toDouble(), 1232.1);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[1].toDouble(), 1933.4);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[2].toDouble(), 2080.3);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[3].toDouble(), 2053.8);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[4].toDouble(), 2138.1);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[5].toDouble(), 2049.9);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[6].toDouble(), 2110.4);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[7].toDouble(), 2367.1);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[8].toDouble(), 2629.7);

  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[0].toDouble(), 2980);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[1].toDouble(), 5635);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[2].toDouble(), 3);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[3].toDouble(), 6095);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[4].toDouble(), 6324);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[5].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[6].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[7].toDouble(), 7965);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[8].toDouble(), 9305);

  for (int i = 0; i < 9; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaMinimum")[i].toDouble(), 0) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaMaximum")[i].toDouble(), 327) << "Error at index: " << i;
  }

  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[0].toDouble(), 2);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[1].toDouble(), 2);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[2].toDouble(), 2);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[3].toDouble(), 2);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[4].toDouble(), 1);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[5].toDouble(), 3);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[6].toDouble(), 2);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[7].toDouble(), 1);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[8].toDouble(), 1);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidType")[0], "SATURATION");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidType")[1], "MINUS");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidType")[2], "DUMMY_DEFECT");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidType")[3], "OTHER");

  EXPECT_DOUBLE_EQ(archive.findKeyword("InvalidValue")[0].toDouble(), -20000);
  EXPECT_DOUBLE_EQ(archive.findKeyword("InvalidValue")[1].toDouble(), -21000);
  EXPECT_DOUBLE_EQ(archive.findKeyword("InvalidValue")[2].toDouble(), -22000);
  EXPECT_DOUBLE_EQ(archive.findKeyword("InvalidValue")[3].toDouble(), -23000);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[0], "(94, 365, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[1], "(176, 370, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[2], "(147, 378, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[3], "(228, 340, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[4], "(218, 386, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[5], "(161, 989, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[6], "(430, 774, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[7], "(162, 1245, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[8], "(585, 952, 0, 0)");

  EXPECT_DOUBLE_EQ(archive.findKeyword("OutOfImageBoundsValue"), -30000);

  for (int i = 0; i < 9; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("OutOfImageBoundsPixel")[i].toDouble(), 0) << "Error at index: " << i;
  }

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("StretchedFlag"), "FALSE");

  for (int i = 0; i < 6; i++) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("DarkFileName")[i], "{MIV_DRK_04724_05494_M___002.csv, MIN_DRK_04724_05494_M___002.csv}") << "Error at index: " << i;
  }
  for (int i = 6; i < 11; i++) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("DarkFileName")[i], "{MIN_DRK_01226_01571_L___002.csv, MIV_DRK_01226_01571_L___002.csv}") << "Error at index: " << i;
  }

  for (int i = 0; i < 11; i++) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("FtFileName")[i], "MIV_FTF_PRFLT_N___v01.csv") << "Error at index: " << i;
  }

  for (int i = 0; i < 6; i++) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("FlatFileName")[i], "{MIV_FLT_04724_05494_N___002.csv, MIN_FLT_04724_05494_N___002.csv}") << "Error at index: " << i;
  }
  for (int i = 6; i < 11; i++) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("FlatFileName")[i], "{MIV_FLT_01226_01571_L___002.csv, MIN_FLT_01226_01571_L___002.csv}") << "Error at index: " << i;
  }

  for (int i = 0; i < 11; i++) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("EfficFileName")[i], "{MIN_EFF_PRFLT_N___v01.csv, MIV_EFF_PRFLT_N___v01.csv}") << "Error at index: " << i;
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("NonlinFileName")[i], "{MIV_NLT_PRFLT_N___v01.csv, MIN_NLT_PRFLT_N___v01.csv}") << "Error at index: " << i;
  }

  for (int i = 0; i < 11; i++) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("RadCnvCoef")[i], "(1.470593, 2.204781, 2.244315, 2.734361, 1.885889, 3.04924, 3.312096, 4.788256, 7.969085)") << "Error at index: " << i;
  }

  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[0].toDouble(), 0.002353311);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[1].toDouble(), 0.002450451);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[2].toDouble(), 0.003549924);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[3].toDouble(), 0.003886012);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[4].toDouble(), 0.004316842);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[5].toDouble(), 0.004316842);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[6].toDouble(), 0.004893535);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[7].toDouble(), 0.007400877);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[8].toDouble(), 0.01218292);

  EXPECT_DOUBLE_EQ(archive.findKeyword("StandardGeometry")[0].toDouble(), 30.0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("StandardGeometry")[1].toDouble(), 0.0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("StandardGeometry")[2].toDouble(), 30.0);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrId"), "LISM ORIGINAL");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[0], "(0.5, -0.019, 0.000242, -1.46e-06, 0.05678, 1.913, 0.0643, 0.2448, 0.0, 0.0, 0.0, 0.06797, 1.3, -0.0144, 0.2441, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[1], "(0.5, -0.019, 0.000242, -1.46e-06, 0.06921, 1.487, -0.0382, 0.2122, 0.0, 0.0, 0.0, 0.08916, 0.997, -0.2526, 0.1986, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[2], "(0.5, -0.019, 0.000242, -1.46e-06, 0.05908, 1.43, 0.056, 0.227, 0.0, 0.0, 0.0, 0.09298, 0.918, -0.2251, 0.198, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[3], "(0.5, -0.019, 0.000242, -1.46e-06, 0.05345, 1.413, 0.1263, 0.2409, 0.0, 0.0, 0.0, 0.08705, 0.883, -0.1655, 0.2052, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[4], "(0.5, -0.019, 0.000242, -1.46e-06, 0.05096, 1.377, 0.0736, 0.2383, 0.0, 0.0, 0.0, 0.09746, 0.889, -0.2248, 0.1933, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[5], "(0.5, -0.019, 0.000242, -1.46e-06, 0.05096, 1.377, 0.0736, 0.2383, 0.0, 0.0, 0.0, 0.09746, 0.889, -0.2248, 0.1933, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[6], "(0.5, -0.019, 0.000242, -1.46e-06, 0.03968, 1.335, 0.1809, 0.2632, 0.0, 0.0, 0.0, 0.09486, 0.843, -0.2059, 0.1958, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[7], "(0.5, -0.019, 0.000242, -1.46e-06, 0.06407, 1.103, -0.0062, 0.2168, 0.0, 0.0, 0.0, 0.11201, 0.773, -0.3129, 0.175, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[8], "(0.5, -0.019, 0.000242, -1.46e-06, 0.09175, 0.954, 0.0111, 0.1967, 0.0, 0.0, 0.0, 0.12374, 0.692, -0.2914, 0.1648, -0.00265, 0.00174, -0.000381)");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ResamplingMethod"), "Bi-Linear");

  for (int i = 0; i < 11; i++) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("TcoMosaicFileName")[i], "N/A") << "Error at index: " << i;
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("DtmMosaicFileName")[i], "N/A") << "Error at index: " << i;
  }

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("OverlapSelectionId"),
                                           "Prioritized order : nominal mission period and phase angle closer to the standard geometry");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("MatchingMosaic"), "N/A");

  for (int i = 0; i < 5; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("L2aDeadPixelThreshold")[i].toDouble(), 35) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ(archive.findKeyword("L2aSaturationThreshold")[i].toDouble(), 1023) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ(archive.findKeyword("DarkValidMinimum")[i].toDouble(), -3) << "Error at index: " << i;
  }
  for (int i = 5; i < 9; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("L2aDeadPixelThreshold")[i].toDouble(), 200) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ(archive.findKeyword("L2aSaturationThreshold")[i].toDouble(), 4095) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ(archive.findKeyword("DarkValidMinimum")[i].toDouble(), -10) << "Error at index: " << i;
  }

  EXPECT_DOUBLE_EQ(archive.findKeyword("FtValidMinimum"), -2);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RadianceSaturationThreshold"), 425.971);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefSaturationThreshold"), 0.65534);

  // BandBin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  for (int i = 0; i < 5; i++) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandbin.findKeyword("FilterName")[i], "MV" + QString::number(i + 1)) << "Error at index: " << i;
  }
  for (int i = 5; i < 9; i++) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandbin.findKeyword("FilterName")[i], "MN" + QString::number(i - 4)) << "Error at index: " << i;
  }

  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[0].toDouble(), 414.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[1].toDouble(), 749.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[2].toDouble(), 901.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[3].toDouble(), 950.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[4].toDouble(), 1001.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[5].toDouble(), 1000.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[6].toDouble(), 1049.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[7].toDouble(), 1248.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[8].toDouble(), 1548.0);

  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[0].toDouble(), 20.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[1].toDouble(), 12.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[2].toDouble(), 21.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[3].toDouble(), 30.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[4].toDouble(), 42.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[5].toDouble(), 27.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[6].toDouble(), 28.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[7].toDouble(), 33.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[8].toDouble(), 48.0);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandbin.findKeyword("BaseBand"), "MV5");

  // Mappihg Group
  PvlGroup &mapping = isisLabel->findGroup("Mapping", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, mapping.findKeyword("ProjectionName"), "SimpleCylindrical");
  EXPECT_DOUBLE_EQ(mapping.findKeyword("CenterLongitude"), 0.0);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, mapping.findKeyword("TargetName"), "Moon");
  EXPECT_DOUBLE_EQ(mapping.findKeyword("EquatorialRadius"), 1737400);
  EXPECT_DOUBLE_EQ(mapping.findKeyword("PolarRadius"), 1737400);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, mapping.findKeyword("LatitudeType"), "Planetocentric");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, mapping.findKeyword("LongitudeDirection"), "PositiveEast");
  EXPECT_DOUBLE_EQ(mapping.findKeyword("LongitudeDomain"), 360);
  EXPECT_DOUBLE_EQ(mapping.findKeyword("MinimumLatitude"), 64.00048828);
  EXPECT_DOUBLE_EQ(mapping.findKeyword("MaximumLatitude"), 65.0);
  EXPECT_DOUBLE_EQ(mapping.findKeyword("MinimumLongitude"), 328.0);
  EXPECT_DOUBLE_EQ(mapping.findKeyword("MaximumLongitude"), 328.99951172);
  EXPECT_DOUBLE_EQ(mapping.findKeyword("UpperLeftCornerX"), -970354.39068);
  EXPECT_DOUBLE_EQ(mapping.findKeyword("UpperLeftCornerY"), 1971024.72156);
  EXPECT_DOUBLE_EQ(mapping.findKeyword("PixelResolution"), 14.80632);
  EXPECT_DOUBLE_EQ(mapping.findKeyword("Scale"), 2048.0);
}

TEST(FunctionalTestMimap2Isis, L3C) {
   QTemporaryDir prefix;
   QString cubeFileName = prefix.path() + "/mimap2isisTEMP.cub";
   QVector<QString> args = {"from=data/mimap2isis/MIA_3C5_03_01351S791E0024SC_cropped.img", "to=" + cubeFileName};

  UserInterface options(APP_XML, args);
  try {
   mimap2isis(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest MI MAP image: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Archive Group
  PvlGroup &archive = isisLabel->findGroup("Archive", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SoftwareName"), "RGC_TC_MI_PLUS");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SoftwareVersion"), "4.0.0");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProcessVersionId"), "L3C");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProductCreationTime"), "2014-11-15T12:08:44");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProgramStartTime"), "2014-11-13T04:40:05");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProducerId"), "LISM");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProductSetId"), "MI_Level3C5");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProductVersionId"), "03");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("RegisteredProduct"), "Y");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("Level2AFileName"), "MV52A0_02NL01351_003_0029.img");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SpiceMetakernelFileName"), "RGC_INF_TCv401IK_MIv200IK_SPv105IK_RISE100i_05_100h_02_LongCK_DS_V02_de421_131210.mk");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("DataSetId"), "MI_L3C");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ImageValueType"), "REFLECTANCE");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ImageUnit"), "ND");

  for (int i = 0; i < 9; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("MinForStatisticalEvaluation")[i].toDouble(), 0) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ((double) archive.findKeyword("MaxForStatisticalEvaluation")[i].toDouble(), 32767) << "Error at index: " << i;
  }

  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[0].toDouble(), 20866);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[1].toDouble(), 31905);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[2].toDouble(), 32710);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[3].toDouble(), 32352);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[4].toDouble(), 32589);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[5].toDouble(), 32644);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[6].toDouble(), 32670);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[7].toDouble(), 32720);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[8].toDouble(), 32673);

  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[0].toDouble(), 6);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[1].toDouble(), 14);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[2].toDouble(), 12);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[3].toDouble(), 18);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[4].toDouble(), 23);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[5].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[6].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[7].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[8].toDouble(), 11);

  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[0].toDouble(), 5313.7);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[1].toDouble(), 9108.2);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[2].toDouble(), 9735.2);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[3].toDouble(), 9950.5);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[4].toDouble(), 10454.1);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[5].toDouble(), 9948.5);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[6].toDouble(), 10312.5);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[7].toDouble(), 11676.5);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[8].toDouble(), 13360.7);

  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[0].toDouble(), 2756.2);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[1].toDouble(), 4579.0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[2].toDouble(), 5173.3);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[3].toDouble(), 4877.5);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[4].toDouble(), 5046.1);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[5].toDouble(), 5216.4);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[6].toDouble(), 5294.5);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[7].toDouble(), 5561.1);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[8].toDouble(), 5645.0);

  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[0].toDouble(), 4834);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[1].toDouble(), 8614);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[2].toDouble(), 8969);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[3].toDouble(), 9179);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[4].toDouble(), 9650);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[5].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[6].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[7].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[8].toDouble(), 13310);

  for (int i = 0; i < 9; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaMinimum")[i].toDouble(), 0) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaMaximum")[i].toDouble(), 500) << "Error at index: " << i;
  }

  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[0].toDouble(), 1);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[1].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[2].toDouble(), 1);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[3].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[4].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[5].toDouble(), 6);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[6].toDouble(), 5);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[7].toDouble(), 2);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[8].toDouble(), 0);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidType")[0], "SATURATION");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidType")[1], "MINUS");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidType")[2], "DUMMY_DEFECT");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidType")[3], "OTHER");

  EXPECT_DOUBLE_EQ(archive.findKeyword("InvalidValue")[0].toDouble(), -20000);
  EXPECT_DOUBLE_EQ(archive.findKeyword("InvalidValue")[1].toDouble(), -21000);
  EXPECT_DOUBLE_EQ(archive.findKeyword("InvalidValue")[2].toDouble(), -22000);
  EXPECT_DOUBLE_EQ(archive.findKeyword("InvalidValue")[3].toDouble(), -23000);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[0], "(100, 0, 0, 7)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[1], "(2956, 0, 0, 622)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[2], "(11715, 0, 0, 6064)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[3], "(7400, 0, 0, 2574)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[4], "(0, 0, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[5], "(19830, 2905, 0, 59224)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[6], "(22289, 890, 0, 61386)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[7], "(33821, 135, 0, 112625)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[8], "(51843, 162, 0, 209314)");

  EXPECT_DOUBLE_EQ(archive.findKeyword("OutOfImageBoundsValue"), -30000);

  for (int i = 0; i < 9; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("OutOfImageBoundsPixel")[i].toDouble(), 1642126) << "Error at index: " << i;
  }

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("DarkFileName")[0], "MIN_DRK_01313_01398_L___003.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("DarkFileName")[1], "MIV_DRK_01226_01571_L___002.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("FtFileName"), "MIV_FTF_PRFLT_N___v01.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("FlatFileName")[0], "MIN_FLT_01226_01571_L___003.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("FlatFileName")[1], "MIV_FLT_01226_01571_L___002.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("EfficFileName")[0], "MIV_EFF_PRFLT_N___v01.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("EfficFileName")[1], "MIN_EFF_PRFLT_N___v01.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("NonlinFileName")[0], "MIN_NLT_PRFLT_N___v01.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("NonlinFileName")[1], "MIV_NLT_PRFLT_N___v01.csv");

  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[0].toDouble(), 1.470593, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[1].toDouble(), 2.204781, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[2].toDouble(), 2.244315, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[3].toDouble(), 2.734361, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[4].toDouble(), 1.885889, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[5].toDouble(), 3.04924, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[6].toDouble(), 3.312096, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[7].toDouble(), 4.788256, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[8].toDouble(), 7.969085, .000001);


  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[0].toDouble(), 0.002353311);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[1].toDouble(), 0.002450451);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[2].toDouble(), 0.003549924);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[3].toDouble(), 0.003886012);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[4].toDouble(), 0.004316842);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[5].toDouble(), 0.004316842);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[6].toDouble(), 0.004893535);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[7].toDouble(), 0.007400877);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[8].toDouble(), 0.01218292);

  EXPECT_DOUBLE_EQ(archive.findKeyword("StandardGeometry")[0].toDouble(), 30.0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("StandardGeometry")[1].toDouble(), 0.0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("StandardGeometry")[2].toDouble(), 30.0);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrId"), "LISM ORIGINAL");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[0], "(1.0, -0.019, 0.000242, -1.46e-06, 0.05678, 1.913, 0.0643, 0.2448, 0.0, 0.0, 0.0, 0.06797, 1.3, -0.0144, 0.2441, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[1], "(1.0, -0.019, 0.000242, -1.46e-06, 0.06921, 1.487, -0.0382, 0.2122, 0.0, 0.0, 0.0, 0.08916, 0.997, -0.2526, 0.1986, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[2], "(1.0, -0.019, 0.000242, -1.46e-06, 0.05908, 1.43, 0.056, 0.227, 0.0, 0.0, 0.0, 0.09298, 0.918, -0.2251, 0.198, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[3], "(1.0, -0.019, 0.000242, -1.46e-06, 0.05345, 1.413, 0.1263, 0.2409, 0.0, 0.0, 0.0, 0.08705, 0.883, -0.1655, 0.2052, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[4], "(1.0, -0.019, 0.000242, -1.46e-06, 0.05096, 1.377, 0.0736, 0.2383, 0.0, 0.0, 0.0, 0.09746, 0.889, -0.2248, 0.1933, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[5], "(1.0, -0.019, 0.000242, -1.46e-06, 0.05096, 1.377, 0.0736, 0.2383, 0.0, 0.0, 0.0, 0.09746, 0.889, -0.2248, 0.1933, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[6], "(1.0, -0.019, 0.000242, -1.46e-06, 0.03968, 1.335, 0.1809, 0.2632, 0.0, 0.0, 0.0, 0.09486, 0.843, -0.2059, 0.1958, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[7], "(1.0, -0.019, 0.000242, -1.46e-06, 0.06407, 1.103, -0.0062, 0.2168, 0.0, 0.0, 0.0, 0.11201, 0.773, -0.3129, 0.175, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[8], "(1.0, -0.019, 0.000242, -1.46e-06, 0.09175, 0.954, 0.0111, 0.1967, 0.0, 0.0, 0.0, 0.12374, 0.692, -0.2914, 0.1648, -0.00265, 0.00174, -0.000381)");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ResamplingMethod"), "Bi-Linear");

  for (int i = 0; i < 5; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("L2aDeadPixelThreshold")[i].toDouble(), 35) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ(archive.findKeyword("L2aSaturationThreshold")[i].toDouble(), 1023) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ(archive.findKeyword("DarkValidMinimum")[i].toDouble(), -3) << "Error at index: " << i;
  }
  for (int i = 5; i < 9; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("L2aDeadPixelThreshold")[i].toDouble(), 200) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ(archive.findKeyword("L2aSaturationThreshold")[i].toDouble(), 4095) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ(archive.findKeyword("DarkValidMinimum")[i].toDouble(), -10) << "Error at index: " << i;
  }

  EXPECT_DOUBLE_EQ(archive.findKeyword("FtValidMinimum"), -2);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RadianceSaturationThreshold"), 425.971);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefSaturationThreshold"), 0.65534);

  // BandBin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  for (int i = 0; i < 5; i++) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandbin.findKeyword("FilterName")[i], "MV" + QString::number(i + 1)) << "Error at index: " << i;
  }
  for (int i = 5; i < 9; i++) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandbin.findKeyword("FilterName")[i], "MN" + QString::number(i - 4)) << "Error at index: " << i;
  }

  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[0].toDouble(), 414.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[1].toDouble(), 749.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[2].toDouble(), 901.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[3].toDouble(), 950.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[4].toDouble(), 1001.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[5].toDouble(), 1000.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[6].toDouble(), 1049.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[7].toDouble(), 1248.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[8].toDouble(), 1548.0);

  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[0].toDouble(), 20.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[1].toDouble(), 12.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[2].toDouble(), 21.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[3].toDouble(), 30.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[4].toDouble(), 42.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[5].toDouble(), 27.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[6].toDouble(), 28.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[7].toDouble(), 33.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[8].toDouble(), 48.0);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandbin.findKeyword("BaseBand"), "MV5");
}

TEST(FunctionalTestMimap2Isis, MAPv3) {
   QTemporaryDir prefix;
   QString cubeFileName = prefix.path() + "/mimap2isisTEMP.cub";
   QVector<QString> args = {"from=data/mimap2isis/MI_MAP_03_N51E124N50E125SC_cropped.lbl",
                            "image=data/mimap2isis/MI_MAP_03_N51E124N50E125SC_cropped.img",
                            "to=" + cubeFileName};

  UserInterface options(APP_XML, args);
  try {
   mimap2isis(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest MI MAP image: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube cube(cubeFileName);
  Pvl *isisLabel = cube.label();

  // Archive Group
  PvlGroup &archive = isisLabel->findGroup("Archive", Pvl::Traverse);
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SoftwareName"), "RGC_TC_MI_PLUS");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SoftwareVersion"), "4.0.0");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProcessVersionId"), "MAP");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProductCreationTime"), "2015-01-02T01:45:16");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProgramStartTime"), "2015-01-02T01:43:13");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProducerId"), "LISM");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProductSetId"), "MI_MAP");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ProductVersionId"), "03");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("RegisteredProduct"), "Y");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("SpiceMetakernelFileName"), "RGC_INF_TCv401IK_MIv200IK_SPv105IK_RISE100i_05_100h_02_LongCK_DS_V02_de421_131210.mk");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("DataSetId"), "SLN-L-MI-5-MAP-V3.0");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ImageValueType"), "REFLECTANCE");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ImageUnit"), "ND");

  for (int i = 0; i < 9; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("MinForStatisticalEvaluation")[i].toDouble(), 0) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ((double) archive.findKeyword("MaxForStatisticalEvaluation")[i].toDouble(), 32767) << "Error at index: " << i;
  }

  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[0].toDouble(), 11914);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[1].toDouble(), 18693);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[2].toDouble(), 22004);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[3].toDouble(), 21392);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[4].toDouble(), 22057);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[5].toDouble(), 19521);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[6].toDouble(), 20790);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[7].toDouble(), 22610);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMaximumDn")[8].toDouble(), 27149);

  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[0].toDouble(), 2336);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[1].toDouble(), 4236);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[2].toDouble(), 4020);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[3].toDouble(), 4643);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[4].toDouble(), 4957);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[5].toDouble(), 5559);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[6].toDouble(), 5804);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[7].toDouble(), 6892);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneMinimumDn")[8].toDouble(), 8121);

  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[0].toDouble(), 3739.9);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[1].toDouble(), 6790.8);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[2].toDouble(), 7729.2);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[3].toDouble(), 7954.9);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[4].toDouble(), 8312.9);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[5].toDouble(), 8080.9);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[6].toDouble(), 8416.3);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[7].toDouble(), 9692.3);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneAverageDn")[8].toDouble(), 11512.4);

  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[0].toDouble(), 528.9);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[1].toDouble(), 785.4);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[2].toDouble(), 860.7);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[3].toDouble(), 826.7);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[4].toDouble(), 837.4);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[5].toDouble(), 798.7);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[6].toDouble(), 837.1);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[7].toDouble(), 882.8);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneStdevDn")[8].toDouble(), 1005.1);

  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[0].toDouble(), 3495);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[1].toDouble(), 6483);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[2].toDouble(), 7484);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[3].toDouble(), 7645);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[4].toDouble(), 8086);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[5].toDouble(), 7789);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[6].toDouble(), 8103);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[7].toDouble(), 9429);
  EXPECT_DOUBLE_EQ(archive.findKeyword("SceneModeDn")[8].toDouble(), 11274);

  for (int i = 0; i < 9; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaMinimum")[i].toDouble(), 0) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaMaximum")[i].toDouble(), 500) << "Error at index: " << i;
  }

  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[0].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[1].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[2].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[3].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[4].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[5].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[6].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[7].toDouble(), 0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("ShadowedAreaPercentage")[8].toDouble(), 0);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidType")[0], "SATURATION");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidType")[1], "MINUS");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidType")[2], "DUMMY_DEFECT");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidType")[3], "OTHER");

  EXPECT_DOUBLE_EQ(archive.findKeyword("InvalidValue")[0].toDouble(), -20000);
  EXPECT_DOUBLE_EQ(archive.findKeyword("InvalidValue")[1].toDouble(), -21000);
  EXPECT_DOUBLE_EQ(archive.findKeyword("InvalidValue")[2].toDouble(), -22000);
  EXPECT_DOUBLE_EQ(archive.findKeyword("InvalidValue")[3].toDouble(), -23000);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[0], "(0, 0, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[1], "(0, 0, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[2], "(0, 0, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[3], "(0, 0, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[4], "(0, 0, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[5], "(0, 0, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[6], "(0, 0, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[7], "(0, 0, 0, 0)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("InvalidPixels")[8], "(0, 0, 0, 0)");

  EXPECT_DOUBLE_EQ(archive.findKeyword("OutOfImageBoundsValue"), -30000);

  for (int i = 0; i < 9; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("OutOfImageBoundsPixel")[i].toDouble(), 0) << "Error at index: " << i;
  }

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("DarkFileName")[0], "MIV_DRK_04375_04723_S___002.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("DarkFileName")[1], "MIN_DRK_04375_04460_S___003.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("FtFileName"), "MIV_FTF_PRFLT_N___v01.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("FlatFileName")[0], "MIV_FLT_04375_04723_N___002.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("FlatFileName")[1], "MIN_FLT_04375_04460_S___003.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("EfficFileName")[0], "MIV_EFF_PRFLT_N___v01.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("EfficFileName")[1], "MIN_EFF_PRFLT_N___v01.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("NonlinFileName")[0], "MIV_NLT_PRFLT_N___v01.csv");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("NonlinFileName")[1], "MIN_NLT_PRFLT_N___v01.csv");

  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[0].toDouble(), 1.470593, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[1].toDouble(), 2.204781, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[2].toDouble(), 2.244315, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[3].toDouble(), 2.734361, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[4].toDouble(), 1.885889, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[5].toDouble(), 3.04924, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[6].toDouble(), 3.312096, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[7].toDouble(), 4.788256, .000001);
  EXPECT_NEAR(archive.findKeyword("RadCnvCoef")[8].toDouble(), 7.969085, .000001);


  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[0].toDouble(), 0.002353311);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[1].toDouble(), 0.002450451);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[2].toDouble(), 0.003549924);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[3].toDouble(), 0.003886012);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[4].toDouble(), 0.004316842);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[5].toDouble(), 0.004316842);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[6].toDouble(), 0.004893535);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[7].toDouble(), 0.007400877);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefCnvCoef")[8].toDouble(), 0.01218292);

  EXPECT_DOUBLE_EQ(archive.findKeyword("StandardGeometry")[0].toDouble(), 30.0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("StandardGeometry")[1].toDouble(), 0.0);
  EXPECT_DOUBLE_EQ(archive.findKeyword("StandardGeometry")[2].toDouble(), 30.0);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrId"), "LISM ORIGINAL");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[0], "(1.0, -0.019, 0.000242, -1.46e-06, 0.05678, 1.913, 0.0643, 0.2448, 0.0, 0.0, 0.0, 0.06797, 1.3, -0.0144, 0.2441, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[1], "(1.0, -0.019, 0.000242, -1.46e-06, 0.06921, 1.487, -0.0382, 0.2122, 0.0, 0.0, 0.0, 0.08916, 0.997, -0.2526, 0.1986, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[2], "(1.0, -0.019, 0.000242, -1.46e-06, 0.05908, 1.43, 0.056, 0.227, 0.0, 0.0, 0.0, 0.09298, 0.918, -0.2251, 0.198, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[3], "(1.0, -0.019, 0.000242, -1.46e-06, 0.05345, 1.413, 0.1263, 0.2409, 0.0, 0.0, 0.0, 0.08705, 0.883, -0.1655, 0.2052, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[4], "(1.0, -0.019, 0.000242, -1.46e-06, 0.05096, 1.377, 0.0736, 0.2383, 0.0, 0.0, 0.0, 0.09746, 0.889, -0.2248, 0.1933, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[5], "(1.0, -0.019, 0.000242, -1.46e-06, 0.05096, 1.377, 0.0736, 0.2383, 0.0, 0.0, 0.0, 0.09746, 0.889, -0.2248, 0.1933, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[6], "(1.0, -0.019, 0.000242, -1.46e-06, 0.03968, 1.335, 0.1809, 0.2632, 0.0, 0.0, 0.0, 0.09486, 0.843, -0.2059, 0.1958, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[7], "(1.0, -0.019, 0.000242, -1.46e-06, 0.06407, 1.103, -0.0062, 0.2168, 0.0, 0.0, 0.0, 0.11201, 0.773, -0.3129, 0.175, -0.00265, 0.00174, -0.000381)");
  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("PhotoCorrCoef")[8], "(1.0, -0.019, 0.000242, -1.46e-06, 0.09175, 0.954, 0.0111, 0.1967, 0.0, 0.0, 0.0, 0.12374, 0.692, -0.2914, 0.1648, -0.00265, 0.00174, -0.000381)");

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, archive.findKeyword("ResamplingMethod"), "Bi-Linear");

  for (int i = 0; i < 5; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("L2aDeadPixelThreshold")[i].toDouble(), 35) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ(archive.findKeyword("L2aSaturationThreshold")[i].toDouble(), 1023) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ(archive.findKeyword("DarkValidMinimum")[i].toDouble(), -3) << "Error at index: " << i;
  }
  for (int i = 5; i < 9; i++) {
    EXPECT_DOUBLE_EQ(archive.findKeyword("L2aDeadPixelThreshold")[i].toDouble(), 200) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ(archive.findKeyword("L2aSaturationThreshold")[i].toDouble(), 4095) << "Error at index: " << i;
    EXPECT_DOUBLE_EQ(archive.findKeyword("DarkValidMinimum")[i].toDouble(), -10) << "Error at index: " << i;
  }

  EXPECT_DOUBLE_EQ(archive.findKeyword("FtValidMinimum"), -2);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RadianceSaturationThreshold"), 425.971);
  EXPECT_DOUBLE_EQ(archive.findKeyword("RefSaturationThreshold"), 0.65534);

  // BandBin Group
  PvlGroup &bandbin = isisLabel->findGroup("BandBin", Pvl::Traverse);
  for (int i = 0; i < 5; i++) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandbin.findKeyword("FilterName")[i], "MV" + QString::number(i + 1)) << "Error at index: " << i;
  }
  for (int i = 5; i < 9; i++) {
    EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandbin.findKeyword("FilterName")[i], "MN" + QString::number(i - 4)) << "Error at index: " << i;
  }

  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[0].toDouble(), 414.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[1].toDouble(), 749.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[2].toDouble(), 901.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[3].toDouble(), 950.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[4].toDouble(), 1001.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[5].toDouble(), 1000.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[6].toDouble(), 1049.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[7].toDouble(), 1248.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Center")[8].toDouble(), 1548.0);

  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[0].toDouble(), 20.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[1].toDouble(), 12.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[2].toDouble(), 21.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[3].toDouble(), 30.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[4].toDouble(), 42.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[5].toDouble(), 27.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[6].toDouble(), 28.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[7].toDouble(), 33.0);
  EXPECT_DOUBLE_EQ(bandbin.findKeyword("Width")[8].toDouble(), 48.0);

  EXPECT_PRED_FORMAT2(AssertQStringsEqual, bandbin.findKeyword("BaseBand"), "MV5");
}

TEST(FunctionalTestMimap2Isis, SpecialPixels) {
  QTemporaryDir prefix;
  QString cubeFileName = prefix.path() + "/mimap2isisTEMP.cub";
  QVector<QString> args = {"from=data/mimap2isis/MI_MAP_02_N65E328N64E329SC_cropped.img", "to=" + cubeFileName,
                            "setnullrange=yes", "nullmin=-31000", "nullmax=-20000", "sethrsrange=yes",
                            "hrsmin=-19000", "hrsmax=-10000", "setlrsrange=yes", "lrsmin=-9000", "lrsmax=0",
                            "setlisrange=yes", "lismin=1000", "lismax=10000", "sethisrange=yes", "hismin=11000", "hismax=20000"};

  UserInterface options(APP_XML, args);
  try {
   mimap2isis(options);
  }
  catch (IException &e) {
    FAIL() << "Unable to ingest MI MAP image: " << e.toString().toStdString().c_str() << std::endl;
  }

  Cube outCube(cubeFileName);
  std::unique_ptr<Histogram> hist(outCube.histogram());

  EXPECT_EQ(hist->LrsPixels(), 2);
  EXPECT_EQ(hist->HrsPixels(), 5);
  EXPECT_EQ(hist->NullPixels(), 4);
  EXPECT_EQ(hist->LisPixels(), 4);
  EXPECT_EQ(hist->HisPixels(), 4);
  EXPECT_NEAR(hist->Average(), 0.459313, .00001);
  EXPECT_NEAR(hist->Sum(), 2.75588, .00001);
  EXPECT_EQ(hist->ValidPixels(), 6);
  EXPECT_NEAR(hist->StandardDeviation(), 0.153348, .0001);
}
