#include "MosaicSceneWidgetTester.h"

#include <iostream>
#include <iomanip>

#include <QMetaType>
#include <QSettings>
#include <QSignalSpy>
#include <QtGui>
#include <QTreeWidgetItem>

#include "FileName.h"
#include "Image.h"
#include "ImageList.h"
#include "MosaicSceneItem.h"
#include "MosaicSceneWidget.h"

namespace Isis {
  /**
   * This is a very basic functionality test.
   */
  void MosaicSceneWidgetTester::testBasicFunctionality() {
    QStatusBar status;

    MosaicSceneWidget widget(&status, true, true, NULL);
    widget.show();
    QTest::qWaitForWindowShown(&widget);
    QVERIFY(widget.getProgress());
    QVERIFY(widget.getView());
    QVERIFY(widget.getScene());
    QVERIFY(widget.getProjection() == NULL);

    Image *image = new Image(QString("./lub3994m.342.lev1.cub"));
    QScopedPointer<QMutex> cameraMutex(new QMutex);
    image->initFootprint(cameraMutex.data());

    ImageList images;
    images.append(image);

    widget.addImages(images);

    QCOMPARE(widget.allMosaicSceneItems().count(), 1);
    QCOMPARE(image, widget.allMosaicSceneItems().first()->image());
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

    MosaicSceneWidget widget(&status, true, true, NULL);
    widget.show();
    QTest::qWaitForWindowShown(&widget);

    Image *image = new Image(QString("./lub3994m.342.lev1.cub"));
    QScopedPointer<QMutex> cameraMutex(new QMutex);
    image->initFootprint(cameraMutex.data());

    ImageList images;
    images.append(image);

    widget.addImages(images);

    MosaicSceneItem *sceneItem = widget.allMosaicSceneItems().first();

    ImageDisplayProperties *displayProperties = image->displayProperties();
    QCOMPARE(sceneItem->color(),
             displayProperties->getValue(ImageDisplayProperties::Color).value<QColor>());
    QCOMPARE(sceneItem->isSelected(),
             displayProperties->getValue(ImageDisplayProperties::Selected).toBool());

    sceneItem->setSelected(true);

    QVERIFY(sceneItem->isSelected());
    QCOMPARE(sceneItem->isSelected(),
             displayProperties->getValue(ImageDisplayProperties::Selected).toBool());

    displayProperties->setSelected(false);

    QVERIFY(!displayProperties->getValue(ImageDisplayProperties::Selected).toBool());
    QCOMPARE(sceneItem->isSelected(),
             displayProperties->getValue(ImageDisplayProperties::Selected).toBool());
  }
}
