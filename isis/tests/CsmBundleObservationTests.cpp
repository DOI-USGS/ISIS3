#include <QString>
#include <iostream>

#include "CsmBundleObservation.h"
#include "CSMCamera.h"
#include "Fixtures.h"
#include "MockCsmPlugin.h"
#include "Mocks.h"
#include "TestUtilities.h"
#include "SerialNumber.h"
#include "BundleTargetBody.h"
#include "BundleImage.h"

#include "gmock/gmock.h"


using namespace Isis;

TEST_F(CSMCameraFixture, CsmBundleOutputString) {
  QString outfile = tempDir.path() + "/bundleout.txt";
  std::ofstream fpOut(outfile.toLatin1().data(), std::ios::out);

  QString sn = SerialNumber::Compose(*testCube);

  BundleImage bi(testCam, sn, testCube->fileName());
  BundleImageQsp bi2 = BundleImageQsp(new BundleImage(bi));
  BundleTargetBodyQsp bundleTargetBody = BundleTargetBodyQsp(new BundleTargetBody);

  CsmBundleObservation observation(bi2,
                                   "ObservationNumber",
                                   "InstrumentId",
                                   bundleTargetBody);


  observation.bundleOutputString(fpOut, false);


}
