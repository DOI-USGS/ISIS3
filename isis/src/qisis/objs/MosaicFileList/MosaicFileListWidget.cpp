#include "MosaicFileListWidget.h"

#include <iostream>

#include <QAction>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSettings>

#include "CubeDisplayProperties.h"
#include "Filename.h"
#include "iException.h"
#include "MosaicTreeWidget.h"
#include "MosaicTreeWidgetItem.h"
#include "PvlObject.h"
#include "TextFile.h"

namespace Isis {
  MosaicFileListWidget::MosaicFileListWidget(QSettings &settings,
      QWidget *parent) : QWidget(parent) {
    QHBoxLayout *layout = new QHBoxLayout();

    p_tree = new MosaicTreeWidget();
    layout->addWidget(p_tree);
    layout->setContentsMargins(0, 0, 0, 0);

    setWhatsThis("This is the mosaic file list. Opened "
        "cubes show up here. You can arrange your cubes into groups (that you "
        "name) to help keep track of them. Also, you can configure multiple "
        "files at once. Finally, you can sort your files by any of the visible "
        "columns (use the view menu to show/hide columns of data).");

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

      p_tree->updateViewActs();
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

        if (cubes.HasKeyword("Expanded")) {
          bool expanded = (cubes["Expanded"][0] != "No");
          newCubeGrp->setExpanded(expanded);
        }

        for(int cubeFilenameIndex = 0;
            cubeFilenameIndex < cubes.Keywords();
            cubeFilenameIndex ++) {
          if (cubes[cubeFilenameIndex].IsNamed("Cube")) {
            iString cubeFilename = cubes[cubeFilenameIndex][0];
            newCubeGrp->addChild(takeItem(cubeFilename, allCubes));
          }
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
      cubeGroup += PvlKeyword("Expanded", group->isExpanded() ? "Yes" : "No");

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


  QList<QAction *> MosaicFileListWidget::getExportActions() {
    QList<QAction *> exportActs;

    QAction *saveList = new QAction(this);
    saveList->setText(
        "Save Entire Cube List (ordered by &file list/groups)...");
    connect(saveList, SIGNAL(activated()), this, SLOT(saveList()));

    exportActs.append(saveList);

    return exportActs;
  }


  QWidget * MosaicFileListWidget::getLongHelp(QWidget * fileListContainer) {

    QWidget *longHelpWidget = new QWidget;
    QVBoxLayout *longHelpLayout = new QVBoxLayout;
    longHelpWidget->setLayout(longHelpLayout);

    QLabel *title = new QLabel("<h2>Mosaic File List</h2>");
    longHelpLayout->addWidget(title);


    QPixmap preview;
    if (!fileListContainer) {
      QSettings blankSettings;
      QWeakPointer<MosaicFileListWidget> tmp =
          new MosaicFileListWidget(blankSettings);
      tmp.data()->resize(QSize(500, 200));
      preview = QPixmap::grabWidget(tmp.data());
      delete tmp.data();
    }
    else {
      QPixmap previewPixmap = QPixmap::grabWidget(fileListContainer).scaled(
          QSize(500, 200), Qt::KeepAspectRatio, Qt::SmoothTransformation);

      QLabel *previewWrapper = new QLabel;
      previewWrapper->setPixmap(previewPixmap);
      longHelpLayout->addWidget(previewWrapper);
    }

    QLabel *previewWrapper = new QLabel;
    previewWrapper->setPixmap(preview);
    longHelpLayout->addWidget(previewWrapper);

    QLabel *overview = new QLabel("The mosaic file list is designed to help you "
        "organize your files.<br>"
        "<h3>Groups</h3>"
            "<p>Every cube must be inside of a group. These groups can be "
            "renamed by double clicking on them. To move a cube between groups "
            "just click and drag it to the group you want it in. This works "
            "for multiple cubes also. You can change all of the cubes in a "
            "group by right clicking on the group name.</p>"
        "<h3>Columns</h3>"
            "You can show and hide columns by using the view menu. These "
            "columns show relevant data about the cube, including statistical "
            "information. Some of this information will be blank if you did "
            "not run the application <i>camstats</i> before opening the cube."
        "<h3>Sorting</h3>"
            "You can sort cubes within each group by clicking on the column "
            "title of the column that you want to sort on. Clicking on the "
            "title again will reverse the sorting order. You can also drag and "
            "drop a cube between two other cubes to change where it is in the "
            "list.");
    overview->setWordWrap(true);

    // Qt doesn't calculate this well on its own
    overview->setMinimumHeight(
      (390 * QFontMetrics(overview->font()).averageCharWidth()) / 6);

    longHelpLayout->addWidget(overview);
    longHelpLayout->addStretch();

    return longHelpWidget;
  }


  void MosaicFileListWidget::addCubes(QList<CubeDisplayProperties *> cubes) {
    p_tree->addCubes(cubes);
  }



  void MosaicFileListWidget::saveList() {
    QString output =
        QFileDialog::getSaveFileName((QWidget *)parent(),
          "Choose output file",
          QDir::currentPath() + "/files.lis",
          QString("List File (*.lis);;Text File (*.txt);;All Files (*.*)"));
    if(output.isEmpty()) return;

    TextFile file(output.toStdString(), "overwrite");

    for(int i = 0; i < p_tree->topLevelItemCount(); i++) {
      QTreeWidgetItem *group = p_tree->topLevelItem(i);

      for(int j = 0; j < group->childCount(); j++) {
        QTreeWidgetItem *item = group->child(j);

        if(item->type() == QTreeWidgetItem::UserType) {
          MosaicTreeWidgetItem *cubeItem = (MosaicTreeWidgetItem *)item;
          file.PutLine(cubeItem->cubeDisplay()->fileName().toStdString());
        }
      }
    }
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

