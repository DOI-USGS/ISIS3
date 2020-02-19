#include <QTemporaryDir>
#include <iostream>

#include "Pvl.h"
#include "cnetbin2pvl.h"
#include "Fixtures.h"
#include "Progress.h"
#include "gmock/gmock.h"
#include "FileName.h"
#include "UserInterface.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/cnetbin2pvl.xml").expanded();

TEST_F(ThreeImageNetwork, FunctionalTestCnetbin2pvlDefault) {
  QTemporaryDir prefix;
  ASSERT_TRUE(prefix.isValid());

  QString pvlOut = prefix.path()+"/cnetbin2pvlNetwork.pvl";
  QString cnetFilePath = prefix.path()+"/cnetbin2pvl_network.net";
  FILE* cnetFile;
  cnetFile = fopen(cnetFilePath.toStdString().c_str(), "wb");
  fwrite(network, sizeof(char), sizeof(*network), cnetFile);
  fclose(cnetFile);

  QVector<QString> args = {"from="+cnetFilePath,
                           "to="+pvlOut};
  UserInterface ui(APP_XML, args);


  Progress progress;
  cnetbin2pvl(*network, progress, ui);

  Pvl pvl;
  try{
    pvl.read(cnetFilePath);
  } catch (IException &e){
    FAIL() << "Unable to read PVL file" << std::endl;
  }
}
