#include "MosaicFileListWidget.h"

#include <iostream>

#include <QAction>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QSettings>

#include "CubeDisplayProperties.h"
#include "FileName.h"
#include "IException.h"
#include "MosaicTreeWidget.h"
#include "MosaicTreeWidgetItem.h"
#include "PvlObject.h"
#include "TextFile.h"

namespace Isis {
  MosaicFileListWidget::MosaicFileListWidget(QSettings &settings,
      QWidget *parent) : QWidget(parent) {
    QHBoxLayout *layout = new QHBoxLayout();

    p_tree = new MosaicTreeWidget();
    p_tree->setObjectName("Tree");
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
        IString key = MosaicTreeWidgetItem::treeColumnToString(col) + "Visible";
        key = key.Convert(" ", '_');

        bool visible = toBool(pvl[key][0]);

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

        QTreeWidgetItem *newCubeGrp = p_tree->addGroup(cubes.Name());

        if (cubes.HasKeyword("Expanded")) {
          bool expanded = (cubes["Expanded"][0] != "No");
          newCubeGrp->setExpanded(expanded);
        }

        for(int cubeFileNameIndex = 0;
            cubeFileNameIndex < cubes.Keywords();
            cubeFileNameIndex ++) {
          if (cubes[cubeFileNameIndex].IsNamed("Cube")) {
            QString cubeFileName = cubes[cubeFileNameIndex][0];
            newCubeGrp->addChild(takeItem(cubeFileName, allCubes));
          }
        }
      }

      if(allCubes.size()) {
        p_tree->topLevelItem(0)->addChildren(allCubes);

        IString msg = "Mosaic file list did not have a location for "
            "all of the cubes. Putting them in the first group";
        throw IException(IException::Programmer, msg, _FILEINFO_);
      }
    }
    else {
      throw IException(IException::Io, "Unable to read mosaic file's "
          "list widget settings from Pvl", _FILEINFO_);
    }
  }


  PvlObject MosaicFileListWidget::toPvl() const {
    PvlObject output("MosaicFileList");

    MosaicTreeWidgetItem::TreeColumn col =
        MosaicTreeWidgetItem::FootprintColumn;
    while(col < MosaicTreeWidgetItem::BlankColumn) {
      QString key = MosaicTreeWidgetItem::treeColumnToString(col) + "Visible";
      key = key.replace(" ", "_");
      bool visible = !p_tree->isColumnHidden(col);

      output += PvlKeyword(key, toString(visible));
      col = (MosaicTreeWidgetItem::TreeColumn)(col + 1);
    }

    output += PvlKeyword("SortColumn", toString(p_tree->sortColumn()));

    // Now store groups and the cubes that are in those groups
    for(int i = 0; i < p_tree->topLevelItemCount(); i++) {
      QTreeWidgetItem *group = p_tree->topLevelItem(i);
      PvlObject cubeGroup(
          group->text(MosaicTreeWidgetItem::NameColumn));
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
    QScrollArea *longHelpWidgetScrollArea = new QScrollArea;

    QWidget *longHelpWidget = new QWidget;
    longHelpWidgetScrollArea->setWidget(longHelpWidget);

    QVBoxLayout *longHelpLayout = new QVBoxLayout;
    longHelpLayout->setSizeConstraint(QLayout::SetFixedSize);
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

    QLabel *overview = new QLabel("The mosaic file list is designed to help "
        "to organize your files. The file list supports changing multiple "
        "files simultaneously using the right-click menus after selecting "
        "several images or groups.<br>"
        "<h3>Groups</h3>"
            "<p>Every cube must be inside of a group. These groups can be "
            "renamed by double clicking on them. To move a cube between groups "
            "just click and drag it to the group you want it in. This works "
            "for multiple cubes also. You can change all of the cubes in a "
            "group by right clicking on the group name. You can add a group "
            "by right clicking in the white space below the last cube or on "
            "an existing group.</p>"
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

    longHelpLayout->addWidget(overview);
    longHelpLayout->addStretch();

    return longHelpWidgetScrollArea;
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

    TextFile file(output, "overwrite");

    for(int i = 0; i < p_tree->topLevelItemCount(); i++) {
      QTreeWidgetItem *group = p_tree->topLevelItem(i);

      for(int j = 0; j < group->childCount(); j++) {
        QTreeWidgetItem *item = group->child(j);

        if(item->type() == QTreeWidgetItem::UserType) {
          MosaicTreeWidgetItem *cubeItem = (MosaicTreeWidgetItem *)item;
          file.PutLine(cubeItem->cubeDisplay()->fileName());
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

    throw IException(IException::Programmer, "Cannot find a cube in "
        "tree with filename matching [" + filename.toStdString() + "]",
         _FILEINFO_);
  }
}
