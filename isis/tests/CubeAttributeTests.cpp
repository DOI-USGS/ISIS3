#include <QTemporaryFile>
#include <QString>
#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "CubeAttribute.h"
#include "IEndian.h"
#include "PixelType.h"
#include "Preference.h"
#include "SpecialPixel.h"
#include "TestUtilities.h"

#include "gmock/gmock.h"

using namespace Isis;

bool checkInputAtt(const CubeAttributeInput &att) {
  return true;
}

bool checkOutputAtt(const CubeAttributeOutput &att, 
                    PixelType pixelType, 
                    bool propPixelType,
                    bool propMinMax,
                    bool propFileFormat,
                    double min,
                    double max,
                    Cube::Format fileFormat,
                    const std::string &byteOrder,
                    Cube::LabelAttachment attachStyle) 
                    {
  if (pixelType != att.pixelType()) {
    std::cerr << "Incorrect Pixel Type value [";
    std::cerr << att.pixelType() << "] expected [";
    std::cerr << pixelType << "]" << std::endl;
    return false;
  }
  if (propPixelType != att.propagatePixelType()) {
    std::cerr << "Incorrect Propagate Pixel Type value [";
    std::cerr << att.propagatePixelType() << "] expected [";
    std::cerr << propPixelType << "]" << std::endl;
    return false;
  }
  if (propMinMax != att.propagateMinimumMaximum()) {
    std::cerr << "Incorrect Propagate Min Max value [";
    std::cerr << att.propagateMinimumMaximum() << "] expected [";
    std::cerr << propMinMax << "]" << std::endl;
    return false;
  }
  if (propFileFormat != att.propagateFileFormat()) {
    std::cerr << "Incorrect Propagate File Format value [";
    std::cerr << att.propagateFileFormat() << "] expected [";
    std::cerr << propFileFormat << "]" << std::endl;
    return false;
  }
  if (min != att.minimum()) {
    std::cerr << "Incorrect Minimum value [";
    std::cerr << att.minimum() << "] expected [";
    std::cerr << min << "]" << std::endl;
    return false;
  }
  if (max != att.maximum()) {
    std::cerr << "Incorrect Maximum value [";
    std::cerr << att.maximum() << "] expected [";
    std::cerr << max << "]" << std::endl;
    return false;
  }
  if (fileFormat != att.fileFormat()) {
    std::cerr << "Incorrect file format string [";
    std::cerr << att.fileFormatString() << "] expected [";
    std::cerr << fileFormat << "]" << std::endl;
    return false;
  }
  ByteOrder byteOrderType;
  if(byteOrder == "SYS") {
    if(IsLsb()) {
      byteOrderType = Lsb;
    }
    else {
      byteOrderType = Msb;
    }
  }
  else {
    byteOrderType = ByteOrderEnumeration(QString::fromStdString(byteOrder));
  }
  if (byteOrderType != att.byteOrder()) {
    std::cerr << "Incorrect byte order [";
    std::cerr << att.byteOrder() << "] expected [";
    std::cerr << byteOrderType << "]" << std::endl;
    return false;
  }
  if (attachStyle != att.labelAttachment()) {
    std::cerr << "Incorrect label attachment setting [";
    std::cerr << att.labelAttachment() << "] expected [";
    std::cerr << attachStyle << "]" << std::endl;
    return false;
  }
  return true;
}

TEST(CubeInputAttribute, TestCubeInputAttributeContructor) {
  CubeAttributeInput att("sometext");
}

TEST(CubeInputAttribute, TestCubeInputAttributeBadContructor) {
  EXPECT_THROW(CubeAttributeInput att("+sometext"), IException);
}

TEST(CubeInputAttribute, TestCubeInputAttributeParse1) {
  CubeAttributeInput att("+3");
  EXPECT_EQ(att.toString(), "+3");

  std::vector<QString> bands = att.bands();
  EXPECT_EQ(bands.size(), 1);
  EXPECT_EQ(bands[0], "3");
}

TEST(CubeInputAttribute, TestCubeInputAttributeParse2) {
  CubeAttributeInput att("+3,5-9,99");
  EXPECT_EQ(att.toString(), "+3,5-9,99");

  std::vector<QString> bands = att.bands();
  ASSERT_EQ(bands.size(), 7);
  std::vector<QString> expected_bands = {"3", "5", "6", "7", "8", "9", "99"};
  for (int i = 0; i < bands.size(); i++) {
      EXPECT_EQ(bands[i], expected_bands[i]);
  }
}

TEST(CubeInputAttribute, TestCubeInputAttributeMutators) {
  CubeAttributeInput att("+1-3,4,5,6,99-32");
  std::vector<QString> bandsVector(1, "+1-99");
  att.setBands(bandsVector);

  EXPECT_EQ(att.toString(), "+1-99");
}

TEST(CubeOutputAttribute, TestCubeOutputAttributeContructor) {
  CubeAttributeOutput att;
  EXPECT_EQ(att.propagateMinimumMaximum(), true);
  EXPECT_EQ(att.propagatePixelType(), true);
  EXPECT_EQ(att.propagateFileFormat(), true);

  EXPECT_EQ(att.pixelType(), PixelType::None);
  EXPECT_EQ(att.minimum(), Null);
  EXPECT_EQ(att.maximum(), Null);
  EXPECT_EQ(att.fileFormat(), Cube::Format::Tile);
  EXPECT_EQ(att.byteOrder(), ByteOrder::Lsb);
  EXPECT_EQ(att.labelAttachment(), Cube::AttachedLabel);
}

TEST(CubeOutputAttribute, TestCubeOutputAttributeParse1) {
  CubeAttributeOutput att("+8bit+Tile+0.0:100.1+MSB");
  EXPECT_EQ(att.propagateMinimumMaximum(), false);
  EXPECT_EQ(att.propagatePixelType(), false);
  EXPECT_EQ(att.propagateFileFormat(), false);

  EXPECT_EQ(att.pixelType(), PixelType::UnsignedByte);
  EXPECT_EQ(att.minimum(), 0);
  EXPECT_EQ(att.maximum(), 100.1);
  EXPECT_EQ(att.fileFormat(), Cube::Format::Tile);
  EXPECT_EQ(att.byteOrder(), ByteOrder::Msb);
  EXPECT_EQ(att.labelAttachment(), Cube::AttachedLabel);
}

TEST(CubeOutputAttribute, TestCubeOutputAttributeParse2) {
  CubeAttributeOutput att("+16bit+Bsq+-100000.0:-100.1+lsb");
  EXPECT_EQ(att.propagateMinimumMaximum(), false);
  EXPECT_EQ(att.propagatePixelType(), false);
  EXPECT_EQ(att.propagateFileFormat(), false);

  EXPECT_EQ(att.pixelType(), PixelType::SignedWord);
  EXPECT_EQ(att.minimum(), -100000.0);
  EXPECT_EQ(att.maximum(), -100.1);
  EXPECT_EQ(att.fileFormat(), Cube::Format::Bsq);
  EXPECT_EQ(att.byteOrder(), ByteOrder::Lsb);
  EXPECT_EQ(att.labelAttachment(), Cube::AttachedLabel);
}

TEST(CubeOutputAttribute, TestCubeOutputAttributeParse3) {
  CubeAttributeOutput att("+32bit+tile+999:9999");
  EXPECT_EQ(att.propagateMinimumMaximum(), false);
  EXPECT_EQ(att.propagatePixelType(), false);
  EXPECT_EQ(att.propagateFileFormat(), false);

  EXPECT_EQ(att.pixelType(), PixelType::Real);
  EXPECT_EQ(att.minimum(), 999);
  EXPECT_EQ(att.maximum(), 9999);
  EXPECT_EQ(att.fileFormat(), Cube::Format::Tile);
  ByteOrder oh;
  if(IsLsb()) {
    oh = ByteOrder::Lsb;
  }
  else {
    oh = ByteOrder::Msb;
  }
  EXPECT_EQ(att.byteOrder(), oh);
  EXPECT_EQ(att.labelAttachment(), Cube::AttachedLabel);
}

TEST(CubeOutputAttribute, TestCubeOutputAttributeParse4) {
  CubeAttributeOutput att("+0.0:100.1+detached");
  EXPECT_EQ(att.propagateMinimumMaximum(), false);
  EXPECT_EQ(att.propagatePixelType(), true);
  EXPECT_EQ(att.propagateFileFormat(), true);

  EXPECT_EQ(att.pixelType(), PixelType::None);
  EXPECT_EQ(att.minimum(), 0.0);
  EXPECT_EQ(att.maximum(), 100.1);
  EXPECT_EQ(att.fileFormat(), Cube::Format::Tile);
  ByteOrder oh;
  if(IsLsb()) {
    oh = ByteOrder::Lsb;
  }
  else {
    oh = ByteOrder::Msb;
  }
  EXPECT_EQ(att.byteOrder(), oh);
  EXPECT_EQ(att.labelAttachment(), Cube::DetachedLabel);
}

TEST(CubeOutputAttribute, TestCubeOutputAttributeParse5) {
  CubeAttributeOutput att("+8bit+Tile");
  EXPECT_EQ(att.propagateMinimumMaximum(), true);
  EXPECT_EQ(att.propagatePixelType(), false);
  EXPECT_EQ(att.propagateFileFormat(), false);

  EXPECT_EQ(att.pixelType(), PixelType::UnsignedByte);
  EXPECT_EQ(att.minimum(), Null);
  EXPECT_EQ(att.maximum(), Null);
  EXPECT_EQ(att.fileFormat(), Cube::Format::Tile);
  ByteOrder oh;
  if(IsLsb()) {
    oh = ByteOrder::Lsb;
  }
  else {
    oh = ByteOrder::Msb;
  }
  EXPECT_EQ(att.byteOrder(), oh);
  EXPECT_EQ(att.labelAttachment(), Cube::AttachedLabel);
}

TEST(CubeOutputAttribute, TestCubeOutputAttributeSet) {
  CubeAttributeOutput att;
  att.setAttributes("+8-bit+Detached");
  EXPECT_EQ(att.propagateMinimumMaximum(), true);
  EXPECT_EQ(att.propagatePixelType(), false);
  EXPECT_EQ(att.propagateFileFormat(), true);

  EXPECT_EQ(att.pixelType(), PixelType::UnsignedByte);
  EXPECT_EQ(att.minimum(), Null);
  EXPECT_EQ(att.maximum(), Null);
  EXPECT_EQ(att.fileFormat(), Cube::Format::Tile);
  ByteOrder oh;
  if(IsLsb()) {
    oh = ByteOrder::Lsb;
  }
  else {
    oh = ByteOrder::Msb;
  }
  EXPECT_EQ(att.byteOrder(), oh);
  EXPECT_EQ(att.labelAttachment(), Cube::DetachedLabel);
}

TEST(CubeInputAttribute, TestCubeInputAttributeParse3) {
  CubeAttributeInput att("+7-10");
  EXPECT_EQ(att.toString(), "+7-10");

  std::vector<QString> bands = att.bands();
  ASSERT_EQ(bands.size(), 4);
  std::vector<QString> expected_bands = {"7", "8", "9", "10"};
  for (int i = 0; i < bands.size(); i++) {
      EXPECT_EQ(bands[i], expected_bands[i]);
  }
}

TEST(CubeOutputAttribute, TestCubeOutputAttributeMutators) {
  CubeAttributeOutput att;
  att.setFileFormat(Cube::Bsq);
  att.addAttributes(QString("8bit"));
  att.addAttributes("msb");
  att.setByteOrder(Msb);
  att.addAttributes(FileName("+dETacHEd"));
  att.setMinimum(1.0);
  att.setMaximum(2.0);
  att.setPixelType(UnsignedByte);
  EXPECT_EQ(att.toString(), "+BandSequential+UnsignedByte+MSB+dETacHEd+1.0:2.0");
  EXPECT_EQ(att.propagateMinimumMaximum(), false);
  EXPECT_EQ(att.propagatePixelType(), false);
  EXPECT_EQ(att.propagateFileFormat(), false);
  EXPECT_EQ(att.pixelType(), PixelType::UnsignedByte);
  EXPECT_EQ(att.minimum(), 1.0);
  EXPECT_EQ(att.maximum(), 2.0);
  EXPECT_EQ(att.fileFormat(), Cube::Format::Bsq);
  EXPECT_EQ(att.byteOrder(), ByteOrder::Msb);
  EXPECT_EQ(att.labelAttachment(), Cube::DetachedLabel);

  att.addAttributes("Attached");
  att.setMaximum(12.0);
  att.setPixelType(Real);
  EXPECT_EQ(att.toString(), "+BandSequential+Real+MSB+dETacHEd+1.0:12.0+Attached");
  EXPECT_EQ(att.propagateMinimumMaximum(), false);
  EXPECT_EQ(att.propagatePixelType(), false);
  EXPECT_EQ(att.propagateFileFormat(), false);
  EXPECT_EQ(att.pixelType(), PixelType::Real);
  EXPECT_EQ(att.minimum(), 1.0);
  EXPECT_EQ(att.maximum(), 12.0);
  EXPECT_EQ(att.fileFormat(), Cube::Format::Bsq);
  EXPECT_EQ(att.byteOrder(), ByteOrder::Msb);
  EXPECT_EQ(att.labelAttachment(), Cube::AttachedLabel);

  att.setLabelAttachment(Cube::DetachedLabel);
  EXPECT_EQ(att.toString(), "+BandSequential+Real+MSB+Detached+1.0:12.0");
  EXPECT_EQ(att.propagateMinimumMaximum(), false);
  EXPECT_EQ(att.propagatePixelType(), false);
  EXPECT_EQ(att.propagateFileFormat(), false);
  EXPECT_EQ(att.pixelType(), PixelType::Real);
  EXPECT_EQ(att.minimum(), 1.0);
  EXPECT_EQ(att.maximum(), 12.0);
  EXPECT_EQ(att.fileFormat(), Cube::Format::Bsq);
  EXPECT_EQ(att.byteOrder(), ByteOrder::Msb);
  EXPECT_EQ(att.labelAttachment(), Cube::DetachedLabel);
  
  att.setLabelAttachment(Cube::ExternalLabel);
  EXPECT_EQ(att.toString(), "+BandSequential+Real+MSB+External+1.0:12.0");
  EXPECT_EQ(att.propagateMinimumMaximum(), false);
  EXPECT_EQ(att.propagatePixelType(), false);
  EXPECT_EQ(att.propagateFileFormat(), false);
  EXPECT_EQ(att.pixelType(), PixelType::Real);
  EXPECT_EQ(att.minimum(), 1.0);
  EXPECT_EQ(att.maximum(), 12.0);
  EXPECT_EQ(att.fileFormat(), Cube::Format::Bsq);
  EXPECT_EQ(att.byteOrder(), ByteOrder::Msb);
  EXPECT_EQ(att.labelAttachment(), Cube::ExternalLabel);
}

// Checks that setting attributes from a file with no attributes
// does not remove existing set attributes
TEST(CubeOutputAttribute, TestCubeOutputAttributeFileWithNoAtts) {
  CubeAttributeOutput att;
  att.setAttributes("+real");
  att.addAttributes(FileName("output/makecubeTruth5.cub"));
  
  EXPECT_EQ(att.toString(), "+real");
  EXPECT_EQ(att.propagateMinimumMaximum(), true);
  EXPECT_EQ(att.propagatePixelType(), false);
  EXPECT_EQ(att.propagateFileFormat(), true);

  EXPECT_EQ(att.pixelType(), PixelType::Real);
  EXPECT_EQ(att.minimum(), Null);
  EXPECT_EQ(att.maximum(), Null);
  EXPECT_EQ(att.fileFormat(), Cube::Format::Tile);
  EXPECT_EQ(att.byteOrder(), ByteOrder::Lsb);
  EXPECT_EQ(att.labelAttachment(), Cube::AttachedLabel);
}

// Checks that no attributes are parsed along the file path
TEST(CubeOutputAttribute, TestCubeOutputAttributePathWithPluses) {
  CubeAttributeOutput att;
  att.setAttributes("+real+output/makecubeTruth5.cub");

  EXPECT_EQ(att.toString(), "");
  EXPECT_EQ(att.propagateMinimumMaximum(), true);
  EXPECT_EQ(att.propagatePixelType(), true);
  EXPECT_EQ(att.propagateFileFormat(), true);

  EXPECT_EQ(att.pixelType(), PixelType::None);
  EXPECT_EQ(att.minimum(), Null);
  EXPECT_EQ(att.maximum(), Null);
  EXPECT_EQ(att.fileFormat(), Cube::Format::Tile);
  EXPECT_EQ(att.byteOrder(), ByteOrder::Lsb);
  EXPECT_EQ(att.labelAttachment(), Cube::AttachedLabel);
}

TEST(CubeOutputAttribute, TestCubeOutputAttributeAutodetectFileFormat) {
  Pvl &prefs = Preference::Preferences();
  PvlGroup &cubePrefs = prefs.findGroup("CubeCustomization");
  QString &autodetectExtensionValue = cubePrefs["AutoExtension"][0];
  autodetectExtensionValue = "On";
  CubeAttributeOutput att("makecubeTruth5.tiff");
  autodetectExtensionValue = "Off";
  EXPECT_EQ(att.toString(), "+GTiff");
  EXPECT_EQ(att.propagateMinimumMaximum(), true);
  EXPECT_EQ(att.propagatePixelType(), true);
  EXPECT_EQ(att.propagateFileFormat(), false);

  EXPECT_EQ(att.pixelType(), PixelType::None);
  EXPECT_EQ(att.minimum(), Null);
  EXPECT_EQ(att.maximum(), Null);
  EXPECT_EQ(att.fileFormat(), Cube::Format::GTiff);
  EXPECT_EQ(att.byteOrder(), ByteOrder::Lsb);
  EXPECT_EQ(att.labelAttachment(), Cube::AttachedLabel);
}

TEST(CubeOutputAttribute, TestCubeOutputAttributeOverrideAutodetectFileFormat) {
  Pvl &prefs = Preference::Preferences();
  PvlGroup &cubePrefs = prefs.findGroup("CubeCustomization");
  QString &autodetectExtensionValue = cubePrefs["AutoExtension"][0];
  autodetectExtensionValue = "On";
  CubeAttributeOutput att("makecubeTruth5.tiff+Tile");
  autodetectExtensionValue = "Off";
  EXPECT_EQ(att.toString(), "+Tile");
  EXPECT_EQ(att.propagateMinimumMaximum(), true);
  EXPECT_EQ(att.propagatePixelType(), true);
  EXPECT_EQ(att.propagateFileFormat(), false);

  EXPECT_EQ(att.pixelType(), PixelType::None);
  EXPECT_EQ(att.minimum(), Null);
  EXPECT_EQ(att.maximum(), Null);
  EXPECT_EQ(att.fileFormat(), Cube::Format::Tile);
  EXPECT_EQ(att.byteOrder(), ByteOrder::Lsb);
  EXPECT_EQ(att.labelAttachment(), Cube::AttachedLabel);
}