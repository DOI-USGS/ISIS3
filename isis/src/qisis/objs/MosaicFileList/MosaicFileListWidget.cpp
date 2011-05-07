#include "MosaicFileListWidget.h"

#include <iostream>

#include <QHBoxLayout>
#include <QSettings>

#include "CubeDisplayProperties.h"
#include "Filename.h"
#include "iException.h"
#include "MosaicTreeWidget.h"
#include "MosaicTreeWidgetItem.h"
#include "PvlObject.h"

namespace Isis {
  MosaicFileListWidget::MosaicFileListWidget(QSettings &settings,
      QWidget *parent) : QWidget(parent) {
    QHBoxLayout *layout = new QHBoxLayout();

    p_tree = new MosaicTreeWidget();
    layout->addWidget(p_tree);
    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);
  }

  MosaicFileListWidget::~MosaicFileListWidget() {

  }


  QProgressBar *MosaicFileListWidget::getProgress() {
    return p_tree->getProgress();
  }


  void MosaicFileListWidget::fromPvl(PvlObject &pvl) {
    if(pvl.Name() == "MosaicFileList") {
      MosaicTreeWidgetItem::TreeColumn col =
          MosaicTreeWidgetItem::FootprintColumn;
      while(col < MosaicTreeWidgetItem::BlankColumn) {
        iString key = MosaicTreeWidgetItem::treeColumnToString(col) + "Visible";
        key = key.Convert(" ", '_');

        bool visible = (int)pvl[key][0];

        if(visible) {
          p_tree->showColumn(col);
        }
        else {
          p_tree->hideColumn(col);
        }

        col = (MosaicTreeWidgetItem::TreeColumn)(col + 1);
      }

      p_tree->sortItems(pvl["SortColumn"], Qt::AscendingOrder);

      QList<QTreeWidgetItem *> allCubes;

      // Take all of the cubes out of the tree
      while(p_tree->topLevelItemCount() > 0) {
        QTreeWidgetItem *group = p_tree->takeTopLevelItem(0);
        allCubes.append(group->takeChildren());

        delete group;
        group = NULL;
      }

      // Now re-build the tree items
      for(int cubeGrp = 0; cubeGrp < pvl.Objects(); cubeGrp ++) {
        PvlObject &cubes = pvl.Object(cubeGrp);

        QTreeWidgetItem *newCubeGrp = p_tree->addGroup(cubes.Name().c_str());

        for(int cubeFilenameIndex = 0;
            cubeFilenameIndex < cubes.Keywords();
            cubeFilenameIndex ++) {
          iString cubeFilename = cubes[cubeFilenameIndex][0];
          newCubeGrp->addChild(takeItem(cubeFilename, allCubes));
        }
      }

      if(allCubes.size()) {
        p_tree->topLevelItem(0)->addChildren(allCubes);

        iString msg = "Mosaic file list did not have a location for "
            "all of the cubes";
        throw iException::Message(iException::Programmer, msg, _FILEINFO_);
      }
    }
    else {
      throw iException::Message(iException::Io, "Unable to read mosaic file "
          "list widget settings from Pvl", _FILEINFO_);
    }
  }


  PvlObject MosaicFileListWidget::toPvl() const {
    PvlObject output("MosaicFileList");

    MosaicTreeWidgetItem::TreeColumn col =
        MosaicTreeWidgetItem::FootprintColumn;
    while(col < MosaicTreeWidgetItem::BlankColumn) {
      iString key = MosaicTreeWidgetItem::treeColumnToString(col) + "Visible";
      key = key.Convert(" ", '_');
      bool visible = !p_tree->isColumnHidden(col);

      output += PvlKeyword(key, visible);
      col = (MosaicTreeWidgetItem::TreeColumn)(col + 1);
    }

    output += PvlKeyword("SortColumn", p_tree->sortColumn());

    // Now store groups and the cubes that are in those groups
    for(int i = 0; i < p_tree->topLevelItemCount(); i++) {
      QTreeWidgetItem *group = p_tree->topLevelItem(i);
      PvlObject cubeGroup(
          group->text(MosaicTreeWidgetItem::NameColumn).toStdString());

      for(int j = 0; j < group->childCount(); j++) {
        QTreeWidgetItem *item = group->child(j);

        if(item->type() == QTreeWidgetItem::UserType) {
          MosaicTreeWidgetItem *cubeItem = (MosaicTreeWidgetItem *)item;

          cubeGroup += PvlKeyword("Cube", cubeItem->cubeDisplay()->fileName());
        }
      }

      output += cubeGroup;
    }

    return output;
  }


  QList<QAction *> MosaicFileListWidget::getViewActions() {
    return p_tree->getViewActions();
  }


  void MosaicFileListWidget::addCubes(QList<CubeDisplayProperties *> cubes) {
    p_tree->addCubes(cubes);
  }


  MosaicTreeWidgetItem *MosaicFileListWidget::takeItem(QString filename,
                                     QList<QTreeWidgetItem *> &items) {
    for(int i = 0; i < items.size(); i++) {
      if(items[i]->type() == QTreeWidgetItem::UserType) {
        MosaicTreeWidgetItem *item = (MosaicTreeWidgetItem *)items[i];
        if(item->cubeDisplay()->fileName() == filename) {
          items.takeAt(i);
          return item;
        }
      }
    }

    throw iException::Message(iException::Programmer, "Cannot find a cube in "
        "tree with filename matching [" + filename.toStdString() + "]",
         _FILEINFO_);
  }
}

