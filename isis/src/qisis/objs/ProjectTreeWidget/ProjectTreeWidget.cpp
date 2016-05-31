#include "ProjectTreeWidget.h"

#include <QAction>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>

#include "BundleSolutionInfo.h"
#include "BundleSolutionInfoTreeWidgetItem.h"
#include "BundleSettings.h"
#include "BundleSettingsTreeWidgetItem.h"
#include "BundleResults.h"
#include "BundleResultsTreeWidgetItem.h"
#include "Control.h"
#include "ControlGroupTreeWidgetItem.h"
#include "ControlList.h"
#include "ControlTreeWidgetItem.h"
#include "CorrelationMatrix.h"
#include "CorrMatTreeWidgetItem.h"
#include "Directory.h"
#include "FileName.h"
#include "GuiCameraTreeWidgetItem.h"
#include "IException.h"
#include "ImageGroupTreeWidgetItem.h"
#include "ImageList.h"
#include "IString.h"
#include "Project.h"
#include "RenameProjectWorkOrder.h"
#include "TargetBody.h"
#include "TargetBodyTreeWidgetItem.h"

namespace Isis {

  /**
   * ProjectTreeWidget constructor.
   * ProjectTreeWidget is derived from QTreeWidget
   *
   *
   * @param parent
   */
  ProjectTreeWidget::ProjectTreeWidget(Directory *directory, QWidget *parent) :
      QTreeWidget(parent) {

    m_projectItem = NULL;
    m_cnetsParentItem = NULL;
    m_imagesParentItem = NULL;
    m_shapeParentItem = NULL;
    m_targetParentItem = NULL;
    m_sensorsParentItem = NULL;
    m_spacecraftParentItem = NULL;
    m_resultsParentItem = NULL;
    m_ignoreEdits = false;

    setHeaderHidden(true);

    m_directory = directory;
    initProjectTree();

    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)),
            this, SLOT(onItemChanged(QTreeWidgetItem *, int)));

    connect(m_directory->project(), SIGNAL(nameChanged(QString)),
            this, SLOT(onProjectNameChanged()));
    connect(m_directory->project(), SIGNAL(controlListAdded(ControlList *)),
            this, SLOT(addControlGroup(ControlList *)));
    connect(m_directory->project(), SIGNAL(controlAdded(Control *)),
            this, SLOT(addControl(Control *)));
    connect(m_directory->project(), SIGNAL(bundleSolutionInfoAdded(BundleSolutionInfo *)),
            this, SLOT(addBundleSolutionInfo(BundleSolutionInfo *)));

    connect(this, SIGNAL(delayedEnableEditing(QTreeWidgetItem *)),
            this, SLOT(enableEditing(QTreeWidgetItem *)),
            Qt::QueuedConnection);
  }


  ProjectTreeWidget::~ProjectTreeWidget() {
    m_cnetsParentItem = NULL;
    m_imagesParentItem = NULL;
    m_resultsParentItem = NULL;
    m_sensorsParentItem = NULL;
    m_shapeParentItem = NULL;
    m_spacecraftParentItem = NULL;
    m_targetParentItem = NULL;
    m_projectItem = NULL;
  }


  void ProjectTreeWidget::contextMenuEvent(QContextMenuEvent *event) {
    QList<QTreeWidgetItem *> selected = selectedItems();

    QMenu contextMenu;

    // bool indicates whether if we made a local copy
    bool selectedProject = false;
    QPair<bool, ImageList *> selectedImageList(false, NULL);
    QList<Control *> selectedControls;
    QList<BundleSolutionInfo *> selectedBundleSolutionInfo;
    CorrMatTreeWidgetItem *corrMatItem = NULL;
    GuiCameraTreeWidgetItem *guiCameraItem = NULL;
    TargetBodyTreeWidgetItem *targetBodyItem = NULL;


    foreach (QTreeWidgetItem *item, selected) {
      if (item == m_projectItem) {
        selectedProject = true;
      }

      ImageGroupTreeWidgetItem *imageGroupItem = dynamic_cast<ImageGroupTreeWidgetItem *>(item);

      if (imageGroupItem) {
        if (!selectedImageList.second) {
          selectedImageList.first = false;
          selectedImageList.second = imageGroupItem->imageList();
        }
        else if (!selectedImageList.first) {
          selectedImageList.first = true;
          selectedImageList.second = new ImageList(*selectedImageList.second);
          selectedImageList.second->append(*imageGroupItem->imageList());
        }
        else {
          selectedImageList.second->append(*imageGroupItem->imageList());
        }
      }

      ControlTreeWidgetItem *controlItem = dynamic_cast<ControlTreeWidgetItem *>(item);

      if (controlItem) {
        selectedControls.append(controlItem->control());
      }

      ControlGroupTreeWidgetItem *controlGroupItem =
          dynamic_cast<ControlGroupTreeWidgetItem *>(item);

      if (controlGroupItem) {
        for (int childIndex = 0; childIndex < controlGroupItem->childCount(); childIndex++) {
          ControlTreeWidgetItem *controlItem =
              dynamic_cast<ControlTreeWidgetItem *>(controlGroupItem->child(childIndex));

          if (controlItem) {
            selectedControls.append(controlItem->control());
          }
        }
      }

//      ControlTreeWidgetItem *controlItem = dynamic_cast<ControlTreeWidgetItem *>(item);

//      if (controlItem) {
//        selectedControls.append(controlItem->control());
//      }

//      ControlGroupTreeWidgetItem *controlGroupItem =
//          dynamic_cast<ControlGroupTreeWidgetItem *>(item);

//      if (controlGroupItem) {
//        for (int childIndex = 0; childIndex < controlGroupItem->childCount(); childIndex++) {
//          ControlTreeWidgetItem *controlItem =
//              dynamic_cast<ControlTreeWidgetItem *>(controlGroupItem->child(childIndex));

//          if (controlItem) {
//            selectedControls.append(controlItem->control());
//          }
//        }
//      }
      
      corrMatItem = dynamic_cast<CorrMatTreeWidgetItem *>(item);

      guiCameraItem = dynamic_cast<GuiCameraTreeWidgetItem *>(item);

      targetBodyItem = dynamic_cast<TargetBodyTreeWidgetItem *>(item);
    }

    QList<QAction *> workOrders;
    if (selectedProject) {
      workOrders.append(m_directory->supportedActions(WorkOrder::ProjectContext));
    }

    if (selectedImageList.second) {
      workOrders.append(selectedImageList.second->supportedActions(m_directory->project()));
      workOrders.append(NULL);
      workOrders.append(m_directory->supportedActions(selectedImageList.second));
    }

    if (selectedControls.count()) {
      workOrders.append(m_directory->supportedActions(selectedControls));
    }

    if (corrMatItem) {
      workOrders.append( m_directory->supportedActions( corrMatItem->correlationMatrix() ) );
    }
    
    if (guiCameraItem) {
      workOrders.append( m_directory->supportedActions(guiCameraItem->guiCamera().data()));
    }


    if (targetBodyItem) {
      workOrders.append( m_directory->supportedActions(targetBodyItem->targetBody().data()));
    }

    foreach(QAction *action, workOrders) {
      if (action != NULL) {
        contextMenu.addAction(action);
      }
      else {
        contextMenu.addSeparator();
      }
    }

    if (workOrders.count()) {
      contextMenu.exec(event->globalPos());
    }
  }


  void ProjectTreeWidget::initProjectTree() {
    m_projectItem = new QTreeWidgetItem(this);
    m_projectItem->setFlags(Qt::ItemIsEditable |
                           Qt::ItemIsEnabled |
                           Qt::ItemIsSelectable);
    m_projectItem->setExpanded(true);
    m_projectItem->setIcon(0, QIcon(":data"));
//  m_projectItem->setToolTip(
    onProjectNameChanged();
    insertTopLevelItem(0, m_projectItem);

    m_cnetsParentItem = new QTreeWidgetItem(m_projectItem);
    m_cnetsParentItem->setText(0, tr("Control Networks"));
    m_cnetsParentItem->setFlags(Qt::ItemIsEnabled |
                                Qt::ItemIsSelectable);
    m_cnetsParentItem->setExpanded(true);
    m_cnetsParentItem->setIcon(0, QIcon(":layers"));

    m_imagesParentItem = new QTreeWidgetItem(m_projectItem);
    m_imagesParentItem->setText(0, tr("Images"));
    m_imagesParentItem->setFlags(Qt::ItemIsEnabled |
                                 Qt::ItemIsSelectable);
    m_imagesParentItem->setExpanded(true);
    m_imagesParentItem->setIcon(0, QIcon(":pictures"));

    m_shapeParentItem = new QTreeWidgetItem(m_projectItem);
    m_shapeParentItem->setText(0, tr("Shape Models"));
    m_shapeParentItem->setFlags(Qt::ItemIsEnabled |
                                Qt::ItemIsSelectable);
    m_shapeParentItem->setExpanded(true);
    m_shapeParentItem->setIcon(0, QIcon(":dem"));

    m_targetParentItem = new QTreeWidgetItem(m_projectItem);
    m_targetParentItem->setText(0, tr("Target Body"));
    m_targetParentItem->setFlags(Qt::ItemIsEnabled |
                                 Qt::ItemIsSelectable);
    m_targetParentItem->setExpanded(true);
    m_targetParentItem->setIcon(0, QIcon(":moonPhase"));

    m_sensorsParentItem = new QTreeWidgetItem(m_projectItem);
    m_sensorsParentItem->setText(0, tr("Sensors"));
    m_sensorsParentItem->setFlags(Qt::ItemIsEnabled |
                                     Qt::ItemIsSelectable);
    m_sensorsParentItem->setExpanded(true);
    m_sensorsParentItem->setIcon(0, QIcon(":camera"));

    m_spacecraftParentItem = new QTreeWidgetItem(m_projectItem);
    m_spacecraftParentItem->setText(0, tr("Spacecraft"));
    m_spacecraftParentItem->setFlags(Qt::ItemIsEnabled |
                                     Qt::ItemIsSelectable);
    m_spacecraftParentItem->setExpanded(true);
    m_spacecraftParentItem->setIcon(0, QIcon(":spacecraft"));

    m_sensorsParentItem = new QTreeWidgetItem(m_projectItem);
    m_sensorsParentItem->setText(0, tr("Sensors"));
    m_sensorsParentItem->setFlags(Qt::ItemIsEnabled |
                             Qt::ItemIsSelectable);
    m_sensorsParentItem->setExpanded(true);
    m_sensorsParentItem->setIcon(0, QIcon(":camera"));

    m_resultsParentItem = new QTreeWidgetItem(m_projectItem);
    m_resultsParentItem->setText(0, tr("Results"));
    m_resultsParentItem->setFlags(Qt::ItemIsEnabled |
                             Qt::ItemIsSelectable);
    m_resultsParentItem->setExpanded(true);
    m_resultsParentItem->setIcon(0, QIcon(":results"));
  }


  /**
   * This slot exists to re-create the textual editor on an item after Qt has destroyed the editor.
   *   This happens when an invalid project name is entered, for example. The validation occurs when
   *   Qt has not yet destroyed the text editor, so the only way to preserve the editing mode is a
   *   delayed re-creation of it.
   *
   * @param item The item that will become editable (always column 0)
   */
  void ProjectTreeWidget::enableEditing(QTreeWidgetItem *item) {
    editItem(item);
  }


  void ProjectTreeWidget::onItemChanged(QTreeWidgetItem *item, int column) {
    if (!m_ignoreEdits) {
      if (item == m_projectItem && column == 0) {
        QString newName = item->text(0);

        m_ignoreEdits = true;
        item->setText(0, m_directory->project()->name());
        m_ignoreEdits = false;

        if (!RenameProjectWorkOrder::isNameValid(newName)) {
          QMessageBox::critical(NULL, tr("Invalid Project Name"),
                                tr("Project name [%1] is not valid").arg(newName));
          emit delayedEnableEditing(item);
        }
        else {
          RenameProjectWorkOrder *workOrder = new RenameProjectWorkOrder(
              newName, m_directory->project());
          m_directory->project()->addToProject(workOrder);
        }
      }
    }
  }


  void ProjectTreeWidget::onProjectNameChanged() {
    m_ignoreEdits = true;
    m_projectItem->setText(0, m_directory->project()->name());
    m_projectItem->setToolTip(0, m_directory->project()->projectRoot());
    m_ignoreEdits = false;
  }


  void ProjectTreeWidget::onSelectionChanged() {
    QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::UserFlag);

    while (*it) {
      QTreeWidgetItem *item = *it;

      ImageGroupTreeWidgetItem *imageGroupItem = dynamic_cast<ImageGroupTreeWidgetItem *>(item);
      if (imageGroupItem) {
        imageGroupItem->selectionChanged();
      }

      ControlGroupTreeWidgetItem *controlGroupItem =
          dynamic_cast<ControlGroupTreeWidgetItem *>(item);
      if (controlGroupItem) {
        controlGroupItem->selectionChanged();
      }

      ControlTreeWidgetItem *controlItem = dynamic_cast<ControlTreeWidgetItem *>(item);
      if (controlItem) {
        controlItem->selectionChanged();
      }

      it++;
    }

  }


  void ProjectTreeWidget::addControlGroup(ControlList *controlList) {

    QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::UserFlag);

    bool existsAlready = false;
    while (*it && !existsAlready) {
      QTreeWidgetItem *item = *it;

      ControlGroupTreeWidgetItem *controlGroupItem =
          dynamic_cast<ControlGroupTreeWidgetItem *>(item);
      if (controlGroupItem && controlGroupItem->controlList() == controlList) {
        existsAlready = true;
      }

      it++;
    }

    if (!existsAlready) {
      ControlGroupTreeWidgetItem *item =
          new ControlGroupTreeWidgetItem(controlList);

      m_cnetsParentItem->addChild(item);
    }
  }


  void ProjectTreeWidget::addControl(Control *control) {

    QString group = FileName(control->fileName()).dir().dirName();
    QList<QTreeWidgetItem *> found = findItems(group, Qt::MatchRecursive, 0);

    //  Create new TreeWidgetItem
    if (!found.isEmpty()) {
      QTreeWidgetItem *item = new ControlTreeWidgetItem(control);
      found.at(0)->addChild(item);
    }
  }


  void ProjectTreeWidget::addImageGroup(ImageList *imageList) {
    QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::UserFlag);

    bool existsAlready = false;
    while (*it && !existsAlready) {
      QTreeWidgetItem *item = *it;

      ImageGroupTreeWidgetItem *imageGroupItem = dynamic_cast<ImageGroupTreeWidgetItem *>(item);
      if (imageGroupItem && imageGroupItem->imageList() == imageList) {
        existsAlready = true;
      }

      it++;
    }

    if (!existsAlready) {
      ImageGroupTreeWidgetItem *item = new ImageGroupTreeWidgetItem(imageList);
      m_imagesParentItem->addChild(item);
    }
  }

  void ProjectTreeWidget::addTargets(TargetBodyList *targets) {

    m_targetParentItem->takeChildren();
    foreach (TargetBodyQsp newTarget, *targets) {
      QTreeWidgetItem *item = new TargetBodyTreeWidgetItem(newTarget);
      m_targetParentItem->addChild(item);
    }
  }


  void ProjectTreeWidget::addGuiCameras(GuiCameraList *guiCameras) {

    m_sensorsParentItem->takeChildren();
    foreach (GuiCameraQsp newGuiCamera, *guiCameras) {
      QTreeWidgetItem *item = new GuiCameraTreeWidgetItem(newGuiCamera);
      m_sensorsParentItem->addChild(item);
    }
  }


  // TODO - kle - this is hacked, needs to be cleaned up
  // This isn't working anymore...
  void ProjectTreeWidget::addBundleSolutionInfo(BundleSolutionInfo *bundleSolutionInfo) {

//     QString group = bundleSolutionInfo->id();
//     QList<QTreeWidgetItem *> found = findItems(group, Qt::MatchRecursive, 0);
/*
    QTreeWidgetItem *item = new BundleSolutionInfoTreeWidgetItem(bundleSolutionInfo);
    m_resultsParentItem->addChild(item);

    QString cnetfname = bundleSolutionInfo->controlNetworkFileName();
    Control *newControl = new Control(cnetfname);

    QTreeWidgetItem *controlItem = new ControlTreeWidgetItem(newControl);

    item->addChild(controlItem);

    BundleSettingsQsp bundleSettings = bundleSolutionInfo->bundleSettings();
    BundleResults bundleResults = bundleSolutionInfo->bundleResults();
    QTreeWidgetItem *corrMatItem = new CorrMatTreeWidgetItem( bundleResults.correlationMatrix() );

    QTreeWidgetItem *settingsItem = new BundleSettingsTreeWidgetItem(bundleSettings);
    QTreeWidgetItem *statisticsItem = new BundleResultsTreeWidgetItem(&bundleResults);
    statisticsItem->addChild(corrMatItem);

    //
    // At this point the covariance (actually the inverse) matrix may exist but the correlation
    // matrix shouldn't have been created yet. The correlation matrix will be created when
    // "View Correlation Matrix" is selected in the context menu of the following tree item.
    //
    if ( bundleResults.correlationMatrix().hasCovMat() ) {
      //qDebug() << "CovMatFileName:" << bundleResults.correlationMatrix().covarianceFileName().name();
      //qDebug() << "imgs and parmas:" << bundleResults.correlationMatrix().imagesAndParameters()->size();
    }

    item->addChild(settingsItem);
    item->addChild(statisticsItem);

    QTreeWidgetItem *imagesParentItem = new QTreeWidgetItem(item);
    imagesParentItem->setText( 0, tr("Images") );
    imagesParentItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    imagesParentItem->setExpanded(true);
    imagesParentItem->setIcon( 0, QIcon(":pictures") );

    expandItem(item);
//     m_resultsParentItem->addChild(item);
//qDebug() << "Done adding bundleSolutionInfo";
    //  Create new TreeWidgetItem
//    if (!found.isEmpty()) {
//      QTreeWidgetItem *item = new BundleSolutionInfoTreeWidgetItem(bundleSolutionInfo);
//      found.at(0)->addChild(item);
//    }
*/
  }

}
