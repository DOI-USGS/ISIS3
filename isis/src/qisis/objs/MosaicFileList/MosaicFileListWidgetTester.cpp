#include "MosaicFileListWidgetTester.h"

#include <QMetaType>
#include <QSettings>
#include <QSignalSpy>
#include <QtGui>
#include <QTreeWidgetItem>

#include "CubeDisplayProperties.h"
#include "FileName.h"
#include "MosaicFileListWidget.h"
#include "MosaicTreeWidget.h"
#include "MosaicTreeWidgetItem.h"

Q_DECLARE_METATYPE(QTreeWidgetItem *);

namespace Isis {
  void MosaicFileListWidgetTester::initTestCase() {
    qRegisterMetaType<QTreeWidgetItem *>("QTreeWidgetItem *");
  }


  /**
   * This is a very basic functionality test.
   */
  void MosaicFileListWidgetTester::testBasicFunctionality() {
    QSettings tmp("./testSettings", QSettings::NativeFormat);

    MosaicFileListWidget widget(tmp);
    widget.show();
    QTest::qWaitForWindowShown(&widget);
    QVERIFY(widget.getProgress());

    QMutex lock;
    CubeDisplayProperties *image = new CubeDisplayProperties(
        QString("./lub3994m.342.lev1.cub"), &lock);

    QList<CubeDisplayProperties *> images;
    images.append(image);

    widget.addCubes(images);

    // This widget is set up such that it has a tree; most of the functionality is there
    MosaicTreeWidget *tree = widget.findChild<MosaicTreeWidget *>("Tree");
    QVERIFY(tree);
  
    // Grab "Group1" and verify it's text
    QTreeWidgetItem *group1 = tree->topLevelItem(0);
    QVERIFY2(group1, "The first element in the tree ought to be the initial group for images "
        "to go into; i.e. Group1. There are no groups.");

    // This corresponds to the image we added
    QTreeWidgetItem *imageTreeItem = group1->child(0);
    
    for (int i = 0; i < group1->columnCount(); i++) {
      if (i == (int)MosaicTreeWidgetItem::NameColumn) {
        // The text in the name column ought to be Group1
        QCOMPARE(group1->text(i), QString("Group1"));
      }
      else {
        // Other columns in Group1 ought to be blank
        QCOMPARE(group1->text(i), QString());
      }

      // Group1 should have no background fill
      QCOMPARE(group1->background(i), QBrush());
      // Group1 should have no checked columns
      QCOMPARE(group1->checkState(i), Qt::Unchecked);
    }

    // The default for Group1 needs to be expanded
    QCOMPARE(group1->isExpanded(), true);
    // Group1 should contain 1 image (as a QTreeWidgetItem)
    QCOMPARE(group1->childCount(), 1);


    for (int i = 0; i < imageTreeItem->columnCount(); i++) {
      // Verify the appropriate columns have correct text values
      if (i == (int)MosaicTreeWidgetItem::NameColumn) {
        QCOMPARE(imageTreeItem->text(i), image->displayName());
        // Image name should have a background fill
        QCOMPARE(imageTreeItem->background(i),
            QBrush(image->getValue(CubeDisplayProperties::Color).value<QColor>()));
      }
      else if (i == (int)MosaicTreeWidgetItem::ResolutionColumn) {
        QCOMPARE(imageTreeItem->text(i), QString("138.268"));
      }
      else if (i == (int)MosaicTreeWidgetItem::EmissionAngleColumn) {
        QCOMPARE(imageTreeItem->text(i), QString("52.4593"));
      }
      else if (i == (int)MosaicTreeWidgetItem::IncidenceAngleColumn) {
        QCOMPARE(imageTreeItem->text(i), QString("36.9066"));
      }
      else if (i == (int)MosaicTreeWidgetItem::PhaseAngleColumn) {
        QCOMPARE(imageTreeItem->text(i), QString("80.1092"));
      }
      else {
        QCOMPARE(imageTreeItem->text(i), QString());
      }
      
      // Image other columns should not have a background fill
      if (i != (int)MosaicTreeWidgetItem::NameColumn) {
        QCOMPARE(imageTreeItem->background(i), QBrush());
      }

      CubeDisplayProperties::Property checkEquivalent = (CubeDisplayProperties::Property)0;
      
      if (i == (int)MosaicTreeWidgetItem::FootprintColumn)
        checkEquivalent = CubeDisplayProperties::ShowFill;
      else if (i == (int)MosaicTreeWidgetItem::OutlineColumn)
        checkEquivalent = CubeDisplayProperties::ShowOutline;
      else if (i == (int)MosaicTreeWidgetItem::LabelColumn)
        checkEquivalent = CubeDisplayProperties::ShowLabel;

      if (checkEquivalent != (CubeDisplayProperties::Property)0 &&
          image->supports(checkEquivalent)) {

        // Image should have appropriately checked columns
        Qt::CheckState expected = Qt::Unchecked;

        if (image->getValue(checkEquivalent).toBool())
          expected = Qt::Checked;

        QCOMPARE(imageTreeItem->checkState(i), expected);
      }
    }

    // Items aren't selected by default 
    QCOMPARE(imageTreeItem->isSelected(), false);
    QCOMPARE(image->getValue(CubeDisplayProperties::Selected).toBool(), false);

    // Mouse events don't seem to work right... so ctrl+a to select all
    QTest::keyClick(tree, 'a', Qt::ControlModifier);
    
    // Verify the tree items and the image are now selected
    QCOMPARE(group1->isSelected(), true);
    QCOMPARE(imageTreeItem->isSelected(), true);
    QCOMPARE(image->getValue(CubeDisplayProperties::Selected).toBool(), true);

    // Unselect the image in the tree; the group is still selected so the image is
    //   technically still selected.
    imageTreeItem->setSelected(false);    
    
    QCOMPARE(group1->isSelected(), true);
    QCOMPARE(imageTreeItem->isSelected(), false);
    QCOMPARE(image->getValue(CubeDisplayProperties::Selected).toBool(), true);

    // Unselect the group in the tree; now nothing is selected once again
    group1->setSelected(false);    
    
    QCOMPARE(group1->isSelected(), false);
    QCOMPARE(imageTreeItem->isSelected(), false);
    QCOMPARE(image->getValue(CubeDisplayProperties::Selected).toBool(), false);
  }
  
  
  void MosaicFileListWidgetTester::testSynchronization() {
    QSettings tmp("./testSettings", QSettings::NativeFormat);

    MosaicFileListWidget widget(tmp);
    widget.show();
    QTest::qWaitForWindowShown(&widget);

    QList<CubeDisplayProperties *> images;
    
    QMutex lock;
    for (int i = 0; i < 10; i++)
      images.append(new CubeDisplayProperties(
        QString("./lub3994m.342.lev1.cub"), &lock));

    widget.addCubes(images);

    foreach (CubeDisplayProperties *image, images) {
      QCOMPARE(image->supports(CubeDisplayProperties::Selected), true);
      QCOMPARE(image->supports(CubeDisplayProperties::Color), true);
    }

    // This widget is set up such that it has a tree; most of the functionality is there
    MosaicTreeWidget *tree = widget.findChild<MosaicTreeWidget *>("Tree");
    QTreeWidgetItem *group1 = tree->topLevelItem(0);

    QCOMPARE(images.count(), group1->childCount());

    QList<MosaicTreeWidgetItem *> treeItems;
    for (int i = 0; i < group1->childCount(); i++) {
      treeItems.append(dynamic_cast<MosaicTreeWidgetItem *>(group1->child(i)));
    }

    for (int imgIndex = 0; imgIndex < images.count(); imgIndex++) {
      int foundTreeIndex = -1;
      for (int treeIndex = 0; foundTreeIndex == -1 && treeIndex < treeItems.count(); treeIndex++) {
        if (treeItems[treeIndex]->cubeDisplay() == images[imgIndex])
          foundTreeIndex = treeIndex;
      }

      QVERIFY(foundTreeIndex != -1);

      treeItems.insert(imgIndex, treeItems.takeAt(foundTreeIndex));
      QCOMPARE(treeItems[imgIndex]->cubeDisplay(),
               images[imgIndex]);
    }

    foreach (CubeDisplayProperties *image, images) {
      image->setSelected(true);
      for (int i = 0; i < images.count(); i++) {
        QCOMPARE(images[i]->getValue(CubeDisplayProperties::Selected).toBool(),
                 treeItems[i]->isSelected());
      }

      image->setColor(CubeDisplayProperties::randomColor());

      for (int i = 0; i < images.count(); i++) {
        QCOMPARE(images[i]->getValue(CubeDisplayProperties::Color).value<QColor>(),
                 treeItems[i]->background(MosaicTreeWidgetItem::NameColumn).color());
      }
    }
  }
}
