#include <iostream>

#include <QList>

#include "ExportDescription.h"
#include "PixelType.h"
#include "Preference.h"
#include "IString.h"

using namespace Isis;
using std::cout;
using std::endl;


const IString className = "ExportDescription";


void printFailure(IString methodName, IString message="") {
  cout <<
    "FAIL : " << className << "::" << methodName <<
    " : " << message << endl;
}


template <class T>
void compareEqual(IString methodName, T expected, T found,
    IString expectedString="", IString foundString="") {

  bool equal = expected == found;
  cout <<
    (equal ? "PASS : " : "FAIL : ") <<
    className << "::" << methodName << " : " <<
    (expectedString != "" ? expectedString : IString(expected)) <<
    (equal ? " == " : " != ") <<
    (foundString != "" ? foundString : IString(found)) << endl;
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
    att.setAttributes("+1");

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

  IString innerName = "ChannelDescription";
  compareEqual("channelCount()", 3, desc.channelCount());
  for (int i = 0; i < desc.channelCount(); i++) {
    const ExportDescription::ChannelDescription &channel = desc.getChannel(i);
    compareEqual(innerName + "::filename()",
        filenames[i].name(), channel.filename().name());
    compareEqual(innerName + "::attributes()",
        IString("1"),
        IString(
            QString::fromStdString(channel.attributes().toString()).mid(1)));

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

