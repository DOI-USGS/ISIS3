#include <iostream>

#include <QList>

#include "ExportDescription.h"
#include "PixelType.h"
#include "Preference.h"
#include "iString.h"

using namespace Isis;
using std::cout;
using std::endl;


const iString className = "ExportDescription";


void printFailure(iString methodName, iString message="") {
  cout <<
    "FAIL : " << className << "::" << methodName <<
    " : " << message << endl;
}


template <class T>
void compareEqual(iString methodName, T expected, T found,
    iString expectedString="", iString foundString="") {

  bool equal = expected == found;
  cout <<
    (equal ? "PASS : " : "FAIL : ") <<
    className << "::" << methodName << " : " <<
    (expectedString != "" ? expectedString : iString(expected)) <<
    (equal ? " == " : " != ") <<
    (foundString != "" ? foundString : iString(found)) << endl;
}


int main() {
  Preference::Preferences(true);

  cout << "********* Start testing of " << className << " *********" << endl;
  ExportDescription desc;

  compareEqual("getPixelType()", None, desc.getPixelType(), "None");

  desc.setPixelType(SignedWord);
  compareEqual("setPixelType()", SignedWord, desc.getPixelType(), "SignedWord");

  compareEqual("getOutputMinimum()", -32752.0, desc.getOutputMinimum());
  compareEqual("getOutputMaximum()", 32767.0, desc.getOutputMaximum());
  compareEqual("getOutputNull()", -32768.0, desc.getOutputNull());

  QList<FileName> filenames;
  filenames.append(FileName("red.cub"));
  filenames.append(FileName("green.cub"));
  filenames.append(FileName("blue.cub"));
  try {
    CubeAttributeInput att;
    att.Set("+1");

    int index = desc.addChannel(filenames[0], att);
    compareEqual("addChannel()", 0, index);

    index = desc.addChannel(filenames[1], att, 100.0, 500.0);
    compareEqual("addChannel()", 1, index);

    index = desc.addChannel(filenames[2], att, 500.0, 1000.0);
    compareEqual("addChannel()", 2, index);
  }
  catch (IException &e) {
    printFailure("addChannel()", e.toString());
  }

  iString innerName = "ChannelDescription";
  compareEqual("channelCount()", 3, desc.channelCount());
  for (int i = 0; i < desc.channelCount(); i++) {
    const ExportDescription::ChannelDescription &channel = desc.getChannel(i);
    compareEqual(innerName + "::filename()",
        filenames[i].name(), channel.filename().name());
    compareEqual(innerName + "::attributes()",
        iString("1"), iString(channel.attributes().BandsStr()));

    if (i == 0) {
      compareEqual(innerName + "::hasCustomRange()",
          false, channel.hasCustomRange(), "false");
    }
    else {
      compareEqual(innerName + "::hasCustomRange()",
          true, channel.hasCustomRange(), "true");

      if (i == 2) {
        compareEqual(innerName + "::getInputMinimum()",
            500.0, channel.getInputMinimum());
        compareEqual(innerName + "::getInputMaximum()",
            1000.0, channel.getInputMaximum());
      }
    }
  }

  cout << "********* Finished testing of " << className << " *********" << endl;
  return 0;
}

