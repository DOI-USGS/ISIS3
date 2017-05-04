#include "AbstractProjectItemView.h"
#include "BundleObservationView.h"
#include "BundleObservation.h"
#include <QAction>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFile>
#include <QHBoxLayout>
#include <QMap>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QModelIndex>
#include <QSize>
#include <QSizePolicy>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QStatusBar>

#include <QHeaderView>
#include <QStringList>
#include <QTableView>
#include <QTextStream>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QXmlStreamWriter>


#include "ControlPoint.h"
#include "Directory.h"
#include "Cube.h"
#include "Image.h"
#include "MosaicGraphicsView.h"
#include "MosaicSceneWidget.h"
#include "ProjectItem.h"
#include "ProjectItemModel.h"
#include "Shape.h"
#include "ToolPad.h"


  namespace Isis {



  BundleObservationView::BundleObservationView(BundleObservation * bundleObservation,QWidget *parent):
                         AbstractProjectItemView(parent) {

    QStandardItemModel *model = new QStandardItemModel;

    QFile file("AS15_16_test_bundleout_images.csv");
    if (file.open(QIODevice::ReadOnly)) {

      int lineindex = 0;                     // file line counter
      QTextStream in(&file);                 // read to text stream

      while (!in.atEnd()) {

        // read one line from textstream(separated by "\n")
        QString fileLine = in.readLine();

        // parse the read line into separate pieces(tokens) with "," as the delimiter
        QStringList lineToken = fileLine.split(",", QString::SkipEmptyParts);

        // load parsed data to model accordingly
        for (int j = 0; j < lineToken.size(); j++) {
          QString value = lineToken.at(j);

          //  First 2 lines are header, load into model header data
          if (lineindex < 2) {
            //qDebug()<<"header = "<<value;
            //model->setHeaderData(j, Qt::Horizontal, value);
            //qDebug()<<"header = "<<value;
            QStandardItem *v1 = new QStandardItem(value);

            model->setHorizontalHeaderItem(j,v1);
            //model->setHeaderData(j, Qt::Horizontal, value);
          }
          else {
            QStandardItem *item = new QStandardItem(value);
            model->setItem(lineindex, j, item);
          }
        }

        lineindex++;
      }

      file.close();
    }

    QTableView *qtable=new QTableView();
    qtable->setModel(model);
    qtable->setSortingEnabled(true);
    
    QHeaderView *headerView = qtable->horizontalHeader();
    headerView->setStretchLastSection(true);

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
    layout->addWidget(qtable);

    QSizePolicy policy = sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    policy.setVerticalPolicy(QSizePolicy::Expanding);
    setSizePolicy(policy);
    }


  /**
   * Destructor
   */
  BundleObservationView::~BundleObservationView() {

  }


}



