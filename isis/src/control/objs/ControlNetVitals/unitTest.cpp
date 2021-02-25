/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <string>
#include <iostream>

#include <QDebug>
#include <QString>
#include <QStringList>

#include "ControlNetVitals.h"
#include "FileName.h"
#include "IException.h"
#include "Preference.h"

using namespace Isis;
using namespace std;

/**
 * Unit test for ControlNetVitals class
 *
 * @author 2018-06-18 Adam Goins
 * @internal
 *   @history 2018-06-22 Kristin Berry - Upated after fix to numImagesBelowMeasureThreshold()
 */
int main() {
  try {
    Preference::Preferences(true);

    qDebug() << "Testing Control Net Vitals" << endl;

    qDebug() << "Loading Network";

    QString testNetFile("$ISISTESTDATA/isis/src/control/unitTestData/unitTest_ControlNetVersioner_ProtoNetwork2_ProtoV0002.net");
    ControlNet *testNet = new ControlNet(testNetFile);

    qDebug() << "Calculating Network Vitals";

    ControlNetVitals netVitals(testNet);

    qDebug() << "Network ID:" << netVitals.getNetworkId();
    qDebug() << "Network Status:" << netVitals.getStatus();
    qDebug() << "Status Details:" << netVitals.getStatusDetails();

    qDebug() << "Network has additional islands?" << (netVitals.hasIslands() ? "yes" : "no");
    qDebug() << "Number of islands in network:" << netVitals.numIslands();
    QList< QList<QString> > islandLists = netVitals.getIslands();
    for (int islandIndex = 0; islandIndex < islandLists.size(); islandIndex++) {
      QStringList islandSerials(islandLists[islandIndex]);
      islandSerials.sort();
      qDebug() << "Serials in island " << islandIndex;
      foreach(QString serial, islandSerials) {
        qDebug() << "  " << serial;
      }
    }

    int numImages       = netVitals.numImages();
    int numPoints       = netVitals.numPoints();
    int numMeasures     = netVitals.numMeasures();
    int numIgnored      = netVitals.numIgnoredPoints();
    int numFixed        = netVitals.numFixedPoints();
    int numFree         = netVitals.numFreePoints();
    int numLockedPoints = netVitals.numLockedPoints();
    int numConstrained  = netVitals.numConstrainedPoints();
    int pointsWithoutMeasures = netVitals.numPointsBelowMeasureThreshold(1);
    int pointsBelowMeasures   = netVitals.numPointsBelowMeasureThreshold(3);
    int imagesWithoutMeasures = netVitals.numImagesBelowMeasureThreshold(1);
    int imagesBelowMeasures   = netVitals.numImagesBelowMeasureThreshold(2);
    int numImagesBelowHull    = netVitals.numImagesBelowHullTolerance();


    qDebug() << "Number of images in network:" << numImages;
    qDebug() << "Number of points in network:" << numPoints;
    qDebug() << "Number of measures in network:" << numMeasures;
    qDebug() << "Number of ignored points in network:" << numIgnored;
    qDebug() << "Number of editlocked points in network:" << numLockedPoints;
    qDebug() << "Number of fixed points in network:" << numFixed;
    qDebug() << "Number of constrained points in network:" << numConstrained;
    qDebug() << "Number of free points in network:" << numFree;
    qDebug() << "Number of points without measures:" << pointsWithoutMeasures;
    qDebug() << "Number of points with less than 3 measures:" << pointsBelowMeasures;
    qDebug() << "Number of images without measures:" << imagesWithoutMeasures;
    qDebug() << "Number of images with less than 2 measures:" << imagesBelowMeasures;
    qDebug() << "Number of images with less 75 percent hull coverage:" << numImagesBelowHull;

    qDebug() << "Testing getters...";
    assert(numImages == netVitals.getCubeSerials().size());
    assert(numPoints == netVitals.getAllPoints().size());
    assert(numIgnored == netVitals.getIgnoredPoints().size());
    assert(numLockedPoints == netVitals.getLockedPoints().size());
    assert(numFixed == netVitals.getFixedPoints().size());
    assert(numConstrained == netVitals.getConstrainedPoints().size());
    assert(numFree == netVitals.getFreePoints().size());
    assert(pointsWithoutMeasures == netVitals.getPointsBelowMeasureThreshold(1).size());
    assert(pointsBelowMeasures == netVitals.getPointsBelowMeasureThreshold(3).size());
    assert(imagesWithoutMeasures == netVitals.getImagesBelowMeasureThreshold(1).size());
    assert(imagesBelowMeasures == netVitals.getImagesBelowMeasureThreshold(2).size());
    assert(numImagesBelowHull == netVitals.getImagesBelowHullTolerance().size());

    qDebug() << "\nTesting signal/slots...";
    ControlPoint *testPoint = new ControlPoint;
    testPoint->SetEditLock(true);
    testNet->AddPoint(testPoint);
    numPoints++;
    assert( netVitals.numPoints() == testNet->GetNumPoints() );
    testPoint->SetEditLock(false);

    qDebug() << "Setting type to Free...";
    testPoint->SetType(ControlPoint::Free);
    assert(netVitals.numFreePoints() == numFree + 1);
    qDebug() << "Free points incremented correctly";

    qDebug() << "Setting type to Constrained...";
    testPoint->SetType(ControlPoint::Constrained);
    assert(netVitals.numConstrainedPoints() == numConstrained + 1);
    qDebug() << "Constrained points incremented correctly";

    qDebug() << "Setting type to Fixed...";
    testPoint->SetType(ControlPoint::Fixed);
    assert(netVitals.numFixedPoints() == numFixed + 1);
    qDebug() << "Fixed points incremented correctly";

    qDebug() << "Locking the point...";
    testPoint->SetEditLock(true);
    assert(netVitals.numLockedPoints() == numLockedPoints + 1);
    testPoint->SetEditLock(true);
    assert(netVitals.numLockedPoints() == numLockedPoints + 1);
    testPoint->SetEditLock(false);
    assert(netVitals.numLockedPoints() == numLockedPoints);
    testPoint->SetEditLock(false);
    assert(netVitals.numLockedPoints() == numLockedPoints);
    qDebug() << "Locking the point works appropriately.";

    qDebug() << "Ignoring Point...";
    testPoint->SetIgnored(true);

    assert(netVitals.numIgnoredPoints() == numIgnored + 1);
    qDebug() << "Number of Ignored Points increments correctly.";

    assert(netVitals.numFixedPoints() == numFixed);
    qDebug() << "Ignored point no longer contributes to it's point type statistic correctly.";

    qDebug() << "Unignoring Point...";
    testPoint->SetIgnored(false);

    qDebug() << "Adding a measure...";
    ControlMeasure *newMeasure = new ControlMeasure();
    newMeasure->SetCubeSerialNumber("Hey.test");
    testPoint->Add(newMeasure);
    assert(netVitals.numMeasures() == numMeasures + 1);
    qDebug() << "Measure added correctly.";
    qDebug() << "Setting ignored...";

    newMeasure->SetIgnored(true);
    newMeasure->SetIgnored(true);
    assert(testNet->GetNumValidMeasures() == numMeasures);
    qDebug() << "Measure ignored correctly.";
    newMeasure->SetIgnored(false);
    newMeasure->SetIgnored(false);


    qDebug() << "Deleting Measure...";
    testPoint->Delete(newMeasure);
    assert(netVitals.numMeasures() == numMeasures);
    qDebug() << "Measure deleted correctly.";

    qDebug() << "Deleting point...";
    testPoint->SetEditLock(false);
    testNet->DeletePoint(testPoint);
    assert(netVitals.numPoints() == --numPoints);
    qDebug() << "Point deleted correctly.";

    qDebug() << "Adding dummy point...";
    ControlPoint *newPoint = new ControlPoint;
    testNet->AddPoint(newPoint);
    assert(netVitals.numPoints() == ++numPoints);

    qDebug() << "Deleting dummy point...";
    newPoint->SetIgnored(true);
    testNet->DeletePoint(newPoint);
    assert(netVitals.numPoints() == --numPoints);
    qDebug() << "Point deleted correctly";

    qDebug() << "Swapping Control Net...";
    ControlNet net;
    testNet->swap(net);

    assert(netVitals.numPoints() == 0);
    qDebug() << "Net swapped correctly.";

    delete testNet;
    testNet = NULL;

  }
  catch(IException &e) {
    qDebug() << "ControlNetVitals unit test failed!" << endl;
    e.print();
  }
}
