#include <iostream>
#include <QTemporaryFile>

#include "cam2map.h"

#include "Cube.h"
#include "CubeAttribute.h"
#include "PixelType.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "TestUtilities.h"
#include "FileName.h"

#include "Fixtures.h"

#include "gmock/gmock.h"

using namespace Isis;

static QString APP_XML = FileName("$ISISROOT/bin/xml/cam2map.xml").expanded();

TEST_F(DefaultCube, FunctionalTestCam2mapDefault) {
}
