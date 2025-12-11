#include "gmock/gmock.h"

#include <functional>

#include "CameraFactory.h"
#include "IException.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"

using namespace Isis;

// Stable substrings taken from CameraFactory.cpp
//   - CameraVersion(Pvl&) outer catch
static const QString CAMERA_VERSION_ERROR =
    "Unable to locate latest camera model version number from group [Instrument]";

class CameraFactoryTests : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    // Standard ISIS test setup
    Preference::Preferences(true);
  }

  // Helper: ensure an IException is thrown and its message contains substring
  void expectIExceptionContaining(std::function<void()> fn,
                                  const QString &expectedSubstring) {
    try {
      fn();
      FAIL() << "Expected an IException to be thrown";
    }
    catch (IException &e) {
      QString msg = e.toString();
      EXPECT_TRUE(msg.contains(expectedSubstring))
          << "Expected exception message to contain:\n  ["
          << expectedSubstring.toStdString()
          << "]\nBut actual message was:\n  ["
          << msg.toStdString() << "]";
    }
    catch (...) {
      FAIL() << "Expected an IException, but a different exception type was thrown";
    }
  }
};


// 1) No Instrument group at all
TEST_F(CameraFactoryTests, MissingInstrumentGroup) {
  Pvl lab;  // empty: no Instrument

  expectIExceptionContaining(
      [&lab]() {
        (void) CameraFactory::CameraVersion(lab);
      },
      CAMERA_VERSION_ERROR);
}


// 2) Instrument group present, but missing SpacecraftName keyword
TEST_F(CameraFactoryTests, MissingSpacecraftName) {
  Pvl lab;
  PvlGroup inst("Instrument");
  // No SpacecraftName, no InstrumentId yet
  lab.addGroup(inst);

  expectIExceptionContaining(
      [&lab]() {
        (void) CameraFactory::CameraVersion(lab);
      },
      CAMERA_VERSION_ERROR);
}


// 3) Instrument group has bogus spacecraft/instrument; plugin cannot find a match
TEST_F(CameraFactoryTests, UnsupportedCameraModel) {
  Pvl lab;
  PvlGroup inst("Instrument");
  inst += PvlKeyword("SpacecraftName", "Bogus Spacecraft");
  inst += PvlKeyword("InstrumentId", "Bogus Instrument");
  lab.addGroup(inst);

  // CameraVersion will fail when trying to find a matching plugin group
  expectIExceptionContaining(
      [&lab]() {
        (void) CameraFactory::CameraVersion(lab);
      },
      CAMERA_VERSION_ERROR);
}
