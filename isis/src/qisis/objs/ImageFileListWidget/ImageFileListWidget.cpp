#include "ImageFileListWidget.h"

#include <iostream>

#include <QAction>
#include <QApplication>
#include <QContextMenuEvent>
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QScrollArea>
#include <QSettings>
#include <QXmlStreamWriter>

#include "Directory.h"
#include "FileName.h"
#include "IException.h"
#include "ImageTreeWidget.h"
#include "ImageTreeWidgetItem.h"
#include "ProgressBar.h"
#include "Project.h"
#include "PvlObject.h"
#include "TextFile.h"
#include "XmlStackedHandlerReader.h"

namespace Isis {
  ImageFileListWidget::ImageFileListWidget(Directory *directory,
                                           QWidget *parent) : QWidget(parent) {
    m_directory = directory;
    QHBoxLayout *layout = new QHBoxLayout();

    m_tree = new ImageTreeWidget(directory);
    m_tree->setObjectName("Tree");
    layout->addWidget(m_tree);

    layout->setContentsMargins(0, 0, 0, 0);

    setWhatsThis("This is the image file list. Opened "
        "cubes show up here. You can arrange your cubes into groups (that you "
        "name) to help keep track of them. Also, you can configure multiple "
        "files at once. Finally, you can sort your files by any of the visible "
        "columns (use the view menu to show/hide columns of data).");

    setLayout(layout);

    m_progress = new ProgressBar;
    m_progress->setVisible(false);
  }


  ImageFileListWidget::~ImageFileListWidget() {
    delete m_progress;
    m_directory = NULL;
  }


  QProgressBar *ImageFileListWidget::getProgress() {
    return m_progress;
  }


  void ImageFileListWidget::fromPvl(PvlObject &pvl) {
    if(pvl.name() == "ImageFileList") {
      m_serialized.reset(new PvlObject(pvl));
      ImageTreeWidgetItem::TreeColumn col =
          ImageTreeWidgetItem::FootprintColumn;
      while(col < ImageTreeWidgetItem::BlankColumn) {
        IString key = ImageTreeWidgetItem::treeColumnToString(col) + "Visible";
        key = key.Convert(" ", '_');

        if (pvl.hasKeyword(key.ToQt())) {
          bool visible = toBool(pvl[key.ToQt()][0]);

          if(visible) {
            m_tree->showColumn(col);
          }
          else {
            m_tree->hideColumn(col);
          }
        }

        col = (ImageTreeWidgetItem::TreeColumn)(col + 1);
      }

      m_tree->updateViewActs();
      m_tree->sortItems(pvl["SortColumn"], Qt::AscendingOrder);

      QList<QTreeWidgetItem *> allCubes;

      // Take all of the cubes out of the tree
      while(m_tree->topLevelItemCount() > 0) {
        QTreeWidgetItem *group = m_tree->takeTopLevelItem(0);
        allCubes.append(group->takeChildren());

        delete group;
        group = NULL;
      }

      // Now re-build the tree items
      for(int cubeGrp = 0; cubeGrp < pvl.objects(); cubeGrp ++) {
        PvlObject &cubes = pvl.object(cubeGrp);

        QTreeWidgetItem *newCubeGrp = m_tree->addGroup("", cubes.name());

        if (cubes.hasKeyword("Expanded")) {
          bool expanded = (cubes["Expanded"][0] != "No");
          newCubeGrp->setExpanded(expanded);
        }
      }

      if(allCubes.size()) {
        m_tree->addGroup("", "Unknown")->addChildren(allCubes);
      }
    }
    else {
      throw IException(IException::Io, "Unable to read image file's "
          "list widget settings from Pvl", _FILEINFO_);
    }
  }


  PvlObject ImageFileListWidget::toPvl() const {
    PvlObject output("ImageFileList");

    ImageTreeWidgetItem::TreeColumn col =
        ImageTreeWidgetItem::FootprintColumn;
    while(col < ImageTreeWidgetItem::BlankColumn) {
      IString key = ImageTreeWidgetItem::treeColumnToString(col) + "Visible";
      key = key.Convert(" ", '_');
      bool visible = !m_tree->isColumnHidden(col);

      output += PvlKeyword(key.ToQt(), toString((int)visible));
      col = (ImageTreeWidgetItem::TreeColumn)(col + 1);
    }

    output += PvlKeyword("SortColumn", toString(m_tree->sortColumn()));

    // Now store groups and the cubes that are in those groups
    for(int i = 0; i < m_tree->topLevelItemCount(); i++) {
      QTreeWidgetItem *group = m_tree->topLevelItem(i);
      PvlObject cubeGroup(
          group->text(ImageTreeWidgetItem::NameColumn));
      cubeGroup += PvlKeyword("Expanded", group->isExpanded() ? "Yes" : "No");

      for(int j = 0; j < group->childCount(); j++) {
        QTreeWidgetItem *item = group->child(j);

        if(item->type() == QTreeWidgetItem::UserType) {
          ImageTreeWidgetItem *cubeItem = (ImageTreeWidgetItem *)item;

          cubeGroup += PvlKeyword("Image", cubeItem->image()->id());
        }
      }

      output += cubeGroup;
    }

    return output;
  }


  void ImageFileListWidget::load(XmlStackedHandlerReader *xmlReader) {
    xmlReader->pushContentHandler(new XmlHandler(this));
  }


  void ImageFileListWidget::save(QXmlStreamWriter &stream, Project *project,
                                 FileName newProjectRoot) const {
    stream.writeStartElement("imageFileList");

    ImageTreeWidgetItem::TreeColumn col =
        ImageTreeWidgetItem::FootprintColumn;
    while(col < ImageTreeWidgetItem::BlankColumn) {
      bool visible = !m_tree->isColumnHidden(col);
      bool sorted = (m_tree->sortColumn() == col);

      stream.writeStartElement("column");
      stream.writeAttribute("name", ImageTreeWidgetItem::treeColumnToString(col));
      stream.writeAttribute("visible", (visible? "true" : "false"));
      stream.writeAttribute("sorted", (sorted? "true" : "false"));
      stream.writeEndElement();

      col = (ImageTreeWidgetItem::TreeColumn)(col + 1);
    }

    // Now store groups and the cubes that are in those groups
    save(stream, NULL);

    stream.writeEndElement();
  }


  QList<QAction *> ImageFileListWidget::actions() {
    return m_tree->actions();
  }


  QList<QAction *> ImageFileListWidget::getViewActions() {
    return m_tree->getViewActions();
  }


  QList<QAction *> ImageFileListWidget::getExportActions() {
    QList<QAction *> exportActs;

    QAction *saveList = new QAction(this);
    saveList->setText(
        "Save Entire Cube List (ordered by &file list/groups)...");
    connect(saveList, SIGNAL(activated()), this, SLOT(saveList()));

    exportActs.append(saveList);

    return exportActs;
  }


  QWidget * ImageFileListWidget::getLongHelp(QWidget * fileListContainer) {
    QScrollArea *longHelpWidgetScrollArea = new QScrollArea;

    QWidget *longHelpWidget = new QWidget;
    longHelpWidgetScrollArea->setWidget(longHelpWidget);

    QVBoxLayout *longHelpLayout = new QVBoxLayout;
    longHelpLayout->setSizeConstraint(QLayout::SetFixedSize);
    longHelpWidget->setLayout(longHelpLayout);

    QLabel *title = new QLabel("<h2>Image File List</h2>");
    longHelpLayout->addWidget(title);

    QPixmap preview;
    if (!fileListContainer) {
      QWeakPointer<ImageFileListWidget> tmp = new ImageFileListWidget;
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

    QLabel *overview = new QLabel(tr("The mosaic file list is designed to help "
        "to organize your files within the %1 project. The file list supports changing multiple "
        "files simultaneously using the right-click menus after selecting "
        "several images or groups.<br>"
        "<h3>Groups</h3>"
            "<p>Every cube must be inside of a group. These groups can be "
            "renamed by double clicking on them. To move a cube between groups, "
            "click and drag it to the group you want it in. This works "
            "for multiple cubes also. You can change all of the cubes in a "
            "group by right clicking on the group name. You can add a group "
            "by right clicking in the white space below the last cube or on "
            "an existing group.</p>"
        "<h3>Columns</h3>"
            "Show and hide columns by using the view menu. These "
            "columns show relevant data about the cube, including statistical "
            "information. Some of this information will be blank if you do "
            "not run the application, <i>camstats</i>, before opening the cube."
        "<h3>Sorting</h3>"
            "Sort cubes within each group in ascending or descending order "
            "by clicking on the column "
            "title of the column that you want to sort on. Clicking on the "
            "title again will reverse the sorting order. You can also drag and "
            "drop a cube between two other cubes to change where it is in the "
            "list.").arg(QApplication::applicationName()));
    overview->setWordWrap(true);

    longHelpLayout->addWidget(overview);
    longHelpLayout->addStretch();

    return longHelpWidgetScrollArea;
  }


  void ImageFileListWidget::addImages(ImageList *images) {
    m_progress->setText("Loading file list");
    m_progress->setRange(0, images->size() - 1);
    m_progress->setValue(0);
    m_progress->setVisible(true);

    QList<QTreeWidgetItem *> selected = m_tree->selectedItems();

    ImageList alreadyViewedImages = m_tree->imagesInView();


    // Expanded states are forgotten when removed from the tree; hence the bool.
    QList<QTreeWidgetItem *> groups;


    // It's very slow to add/insertChild() on tree items when they are in the tree, so let's
    //   take them out of the tree, call add/insertChild() over and over, then give the
    //   groups (tree items) back to the tree. Expanded states are lost... so save/restore them
    QVariant expandedStates = saveExpandedStates(m_tree->invisibleRootItem());
    while (m_tree->topLevelItemCount()) {
      groups.append(m_tree->takeTopLevelItem(0));
    }

    QTreeWidgetItem *selectedGroup = NULL;

    foreach (Image *image, *images) {
      if (alreadyViewedImages.indexOf(image) == -1) {
        ImageTreeWidget::ImagePosition pos;

        if (!m_serialized.isNull()) {
          pos = find(image);
        }

        QTreeWidgetItem *newImageItem = m_tree->prepCube(images, image);

        if (!pos.isValid()) {
          if (m_tree->groupInList(selected)) {
            QListIterator<QTreeWidgetItem *> it(selected);
            while (it.hasNext() && !selectedGroup) {
              QTreeWidgetItem *item = it.next();
              if (item->data(0, Qt::UserRole).toInt() == ImageTreeWidget::ImageGroupType) {
                selectedGroup = item;
              }
            }
          }

          if (!selectedGroup) {
            QTreeWidgetItem *imageListNameItem = NULL;
            QString groupName;

            if (images->name().isEmpty()) {
              imageListNameItem = m_tree->invisibleRootItem();
              groupName = QString("Group %1").arg(groups.count() + 1);
            }
            else {
              for (int i = 0; !imageListNameItem && i < groups.count(); i++) {
                QTreeWidgetItem *group = groups[i];
                if (group->data(0, Qt::UserRole).toInt() == ImageTreeWidget::ImageListNameType &&
                    group->text(0) == images->name()) {
                  imageListNameItem = group;
                }
              }

              if (!imageListNameItem) {
                imageListNameItem = m_tree->createImageListNameItem(images->name());
                groups.append(imageListNameItem);
              }
            }

            selectedGroup = m_tree->createGroup(imageListNameItem, groupName);
          }

          selectedGroup->addChild(newImageItem);
        }
        else {
          QTreeWidgetItem *groupItem = groups[pos.group()];
          if (groupItem->childCount() < pos.index())
            groupItem->addChild(newImageItem);
          else
            groupItem->insertChild(pos.index(), newImageItem);
        }
      }

      m_progress->setValue(m_progress->value() + 1);
    }

    foreach (QTreeWidgetItem *group, groups) {
      m_tree->addTopLevelItem(group);
    }
    restoreExpandedStates(expandedStates, m_tree->invisibleRootItem());
    
    if (selectedGroup)
      selectedGroup->setSelected(true);

    m_tree->refit();
    m_progress->setVisible(false);
  }


  void ImageFileListWidget::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu;

    QList<QAction *> actions = m_tree->getViewActions();

    foreach (QAction *act, actions) {
      if (act) {
        menu.addAction(act);
      }
      else {
        menu.addSeparator();
      }
    }

    menu.exec(event->globalPos());
  }


  void ImageFileListWidget::saveList() {
    QString output =
        QFileDialog::getSaveFileName((QWidget *)parent(),
          "Choose output file",
          QDir::currentPath() + "/files.lis",
          QString("List File (*.lis);;Text File (*.txt);;All Files (*.*)"));
    if(output.isEmpty()) return;

    TextFile file(output, "overwrite");

    for(int i = 0; i < m_tree->topLevelItemCount(); i++) {
      QTreeWidgetItem *group = m_tree->topLevelItem(i);

      for(int j = 0; j < group->childCount(); j++) {
        QTreeWidgetItem *item = group->child(j);

        if(item->type() == QTreeWidgetItem::UserType) {
          ImageTreeWidgetItem *cubeItem = (ImageTreeWidgetItem *)item;
          file.PutLine(cubeItem->image()->fileName());
        }
      }
    }
  }


  void ImageFileListWidget::save(QXmlStreamWriter &stream, QTreeWidgetItem *itemToWrite) const {
    bool done = false;

    // Start the element - image or group with attributes
    if (!itemToWrite) {
      stream.writeStartElement("treeLayout");
    }
    else if(itemToWrite->type() == QTreeWidgetItem::UserType) {
      ImageTreeWidgetItem *imageItemToWrite = (ImageTreeWidgetItem *)itemToWrite;

      stream.writeStartElement("image");
      stream.writeAttribute("id", imageItemToWrite->image()->id());
    }
    else {
      bool groupIsImageList =
            (itemToWrite->data(0, Qt::UserRole).toInt() == ImageTreeWidget::ImageListNameType);

      stream.writeStartElement("group");
      stream.writeAttribute("name", itemToWrite->text(ImageTreeWidgetItem::NameColumn));
      stream.writeAttribute("expanded", itemToWrite->isExpanded() ? "true" : "false");
      stream.writeAttribute("isImageList", groupIsImageList? "true" : "false");
    }

    // Write any child XML elements (groups in groups)
    int i = 0;
    while (!done) {
      QTreeWidgetItem *childItemToWrite = NULL;

      if (itemToWrite == NULL && i < m_tree->topLevelItemCount()) {
        childItemToWrite = m_tree->topLevelItem(i);
      }
      else if (itemToWrite != NULL && i < itemToWrite->childCount()) {
        childItemToWrite = itemToWrite->child(i);
      }

      if (childItemToWrite) {
        save(stream, childItemToWrite);
      }

      done = (childItemToWrite == NULL);
      i++;
    }

    // Close the initial image or group element
    stream.writeEndElement();
  }


  ImageTreeWidget::ImagePosition ImageFileListWidget::find(const Image *image) const {
    QString id = image->id();

    ImageTreeWidget::ImagePosition result;
    for (int objIndex = 0; !result.isValid() && objIndex < m_serialized->objects(); objIndex++) {
      PvlObject &obj = m_serialized->object(objIndex);

      int imageKeyOffset = 0;
      for (int fileKeyIndex = 0;
           !result.isValid() && fileKeyIndex < obj.keywords();
           fileKeyIndex++) {
        PvlKeyword &key = obj[fileKeyIndex];

        if (key.isNamed("Image")) {
          if (key[0] == id) {
            result = ImageTreeWidget::ImagePosition(objIndex, imageKeyOffset);
          }
          else {
            imageKeyOffset++;
          }
        }
      }
    }

    return result;
  }


  void ImageFileListWidget::restoreExpandedStates(QVariant expandedStates, QTreeWidgetItem *item) {
    QMap<QString, QVariant> states = expandedStates.toMap();

    if (states.contains("Expanded")) {
      item->setExpanded(states["Expanded"].toBool());
    }
    else {
      item->setExpanded(true);
    }

    QList<QVariant> childrenStates = states["Children"].toList();

    int i = 0;
    while (i < item->childCount() && i < childrenStates.count()) {
      restoreExpandedStates(childrenStates[i], item->child(i));
      i++;
    }

    while (i < item->childCount()) {
      restoreExpandedStates(QVariant(), item->child(i));
      i++;
    }
  }


  QVariant ImageFileListWidget::saveExpandedStates(QTreeWidgetItem *item) {
    QMap<QString, QVariant> result;

    result["Expanded"] = item->isExpanded();

    if (item->childCount()) {
      QList<QVariant> childrenStates;

      for (int i = 0; i < item->childCount(); i++) {
        childrenStates.append(saveExpandedStates(item->child(i)));
      }

      result["Children"] = childrenStates;
    }

    return result;
  }


  ImageFileListWidget::XmlHandler::XmlHandler(ImageFileListWidget *fileList) {
    m_fileList = fileList;
    m_currentImageList = NULL;
    m_currentImageListItem = NULL;
    m_currentGroup = NULL;
  }


  ImageFileListWidget::XmlHandler::~XmlHandler() {
  }


  bool ImageFileListWidget::XmlHandler::startElement(const QString &namespaceURI,
      const QString &localName, const QString &qName, const QXmlAttributes &atts) {
    bool result = XmlStackedHandler::startElement(namespaceURI, localName, qName, atts);

    if (result) {
      if (localName == "column") {
        QString colName = atts.value("name");
        QString colVisibleStr = atts.value("visible");
        QString colSortedStr = atts.value("sorted");


        ImageTreeWidgetItem::TreeColumn col =
            ImageTreeWidgetItem::NameColumn;
        while(col < ImageTreeWidgetItem::BlankColumn) {
          QString curColName = ImageTreeWidgetItem::treeColumnToString(col);

          if (curColName == colName) {
            if (colVisibleStr != "false") {
              m_fileList->m_tree->showColumn(col);
            }
            else {
              m_fileList->m_tree->hideColumn(col);
            }

            if (colSortedStr == "true") {
              m_fileList->m_tree->sortItems(col, Qt::AscendingOrder);
            }
          }

          col = (ImageTreeWidgetItem::TreeColumn)(col + 1);
        }
      }

      else if (localName == "group") {
        if (atts.value("isImageList") == "true") {
          if (!m_currentImageList) {
            QString name = atts.value("name");
            m_currentImageListItem = m_fileList->m_tree->createImageListNameItem(name);
            m_currentImageList = m_fileList->m_directory->project()->imageList(name);
            m_fileList->m_tree->addTopLevelItem(m_currentImageListItem);
            m_currentImageListItem->setExpanded(true);
          }
        }
        else {
          m_currentGroup = m_fileList->m_tree->createGroup(m_currentImageListItem,
                                                           atts.value("name"));
        }
      }

      else if (localName == "image" && m_currentGroup) {
        Image *image = m_fileList->m_directory->project()->image(atts.value("id"));
        m_currentGroup->addChild(m_fileList->m_tree->prepCube(m_currentImageList, image));
      }

    }

    return result;
  }


  bool ImageFileListWidget::XmlHandler::endElement(const QString &namespaceURI,
      const QString &localName, const QString &qName) {
    bool result = XmlStackedHandler::endElement(namespaceURI, localName, qName);

    if (result) {
      if (localName == "group") {
        if (m_currentGroup) {
          m_currentGroup = NULL;
        }
        else {
          m_currentImageList = NULL;
          m_currentImageListItem = NULL;
        }
      }
    }

    return result;
  }
}
