/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QDebug>

#include "BundleAdjust.h"
#include "IException.h"
#include "Preference.h"

using namespace std;
using namespace Isis;

int main(int argc, char *argv[]) {
  qDebug() << "This class is currently tested by the jigsaw application";
  try {
    Preference::Preferences(true);
  }
  catch (IException &e) {
    e.print();
  }
}

#if 0
Code not currently covered by jigsaw app tests

These methods are never called
 static void cholmodErrorHandler();
 BundleAdjust(BundleSettings bundleSettings, QString &cnet, SerialNumberList &snlist, bool bPrintSummary);
 BundleAdjust(BundleSettings bundleSettings, Control &cnet, SerialNumberList &snlist, bool bPrintSummary);
 BundleAdjust(BundleSettings bundleSettings, ControlNet &cnet, SerialNumberList &snlist, bool bPrintSummary);
 BundleAdjust(BundleSettings bundleSettings, Control &cnet, QList<ImageList *> &imgLists, bool bPrintSummary);
 ~BundleAdjust();
 bool BundleAdjust::freeCHOLMODLibraryVariables();


These have partial coverage
 void BundleAdjust::init(Progress *progress) {
    (pCamera==false) for Camera *pCamera = m_controlNet->Camera(0);
    (m_bodyRadii[0] < 0 && pCamera==true)
    (image==false) for BundleImage* image = new BundleImage(camera, serialNumber, fileName);
    (m_bundleSettings.validateNetwork()==false) -- this is never the case for jigsaw, need to test elsewhere


Errors:
 void BundleAdjust::init(Progress *progress) {
    (image==false) for BundleImage* image = new BundleImage(camera, serialNumber, fileName);
          QString msg = "In BundleAdjust::init(): image " + fileName + "is null" + "\n";
    (observation==false) for BundleObservation *observation = m_bundleObservations.addNew(image, observationNumber, instrumentId, m_bundleSettings);
          QString msg = "In BundleAdjust::init(): observation " + observationNumber + "is null" + "\n";


 bool BundleAdjust::validateNetwork() {
    ( numMeasures <= 1 ) for int numMeasures = m_controlNet->GetNumberOfValidMeasuresInImage(m_serialNumberList->SerialNumber(i));
    ( nimagesWithInsufficientMeasures > 0 )
          QString msg = "Images with one or less measures:\n";

 void BundleAdjust::checkHeldList() {
    (m_serialNumberList->HasSerialNumber(m_heldImageSerialNumberList->SerialNumber(ih))==false)
          QString msg = "Held image not in FROMLIST";

#endif
