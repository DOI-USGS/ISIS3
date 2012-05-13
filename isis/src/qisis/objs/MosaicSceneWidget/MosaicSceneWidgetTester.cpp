#include "MosaicSceneWidgetTester.h"

#include <iostream>
#include <iomanip>

#include <QMetaType>
#include <QSettings>
#include <QSignalSpy>
#include <QtGui>
#include <QTreeWidgetItem>

#include "CubeDisplayProperties.h"
#include "FileName.h"
#include "MosaicSceneItem.h"
#include "MosaicSceneWidget.h"

namespace Isis {
  /**
   * This is a very basic functionality test.
   */
  void MosaicSceneWidgetTester::testBasicFunctionality() {
    QStatusBar status;

    MosaicSceneWidget widget(&status);
    widget.show();
    QTest::qWaitForWindowShown(&widget);
    QVERIFY(widget.getProgress());
    QVERIFY(widget.getView());
    QVERIFY(widget.getScene());
    QVERIFY(widget.getProjection() == NULL);

    QMutex lock;
    CubeDisplayProperties *image = new CubeDisplayProperties(
        QString("./lub3994m.342.lev1.cub"), &lock);

    QList<CubeDisplayProperties *> images;
    images.append(image);

    widget.addCubes(images);
    
    QCOMPARE(widget.allMosaicSceneItems().count(), 1);
    QCOMPARE(image, widget.allMosaicSceneItems().first()->cubeDisplay());
    QVERIFY(widget.cubesSelectable());

    // Check that the bounding rect is approx. the same
    QRectF expected(QPointF(2376269.37351469, -964957.418535598), QSize(109739.587494429, 48049.2250501961));

    QVERIFY(qAbs(widget.cubesBoundingRect().top() - expected.top()) < 0.0001);
    QVERIFY(qAbs(widget.cubesBoundingRect().left() - expected.left()) < 0.0001);
    QVERIFY(qAbs(widget.cubesBoundingRect().bottom() - expected.bottom()) < 1);
    QVERIFY(qAbs(widget.cubesBoundingRect().right() - expected.right()) < 1);
  }
  
  
  void MosaicSceneWidgetTester::testSynchronization() {
    QStatusBar status;

    MosaicSceneWidget widget(&status);
    widget.show();
    QTest::qWaitForWindowShown(&widget);

    QMutex lock;
    CubeDisplayProperties *image = new CubeDisplayProperties(
        QString("./lub3994m.342.lev1.cub"), &lock);

    QList<CubeDisplayProperties *> images;
    images.append(image);

    widget.addCubes(images);
   
    MosaicSceneItem *sceneItem = widget.allMosaicSceneItems().first(); 

    QCOMPARE(sceneItem->color(), image->getValue(CubeDisplayProperties::Color).value<QColor>());
    QCOMPARE(sceneItem->isSelected(), image->getValue(CubeDisplayProperties::Selected).toBool());

    sceneItem->setSelected(true);
   
    QVERIFY(sceneItem->isSelected()); 
    QCOMPARE(sceneItem->isSelected(), image->getValue(CubeDisplayProperties::Selected).toBool());
    
    image->setSelected(false);
   
    QVERIFY(!image->getValue(CubeDisplayProperties::Selected).toBool()); 
    QCOMPARE(sceneItem->isSelected(), image->getValue(CubeDisplayProperties::Selected).toBool());
  }
}
