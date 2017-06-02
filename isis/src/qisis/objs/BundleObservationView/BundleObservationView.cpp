/**
 * @file
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include "BundleObservationView.h"

#include <QDebug>
#include <QFile>
#include <QHeaderView>
#include <QSizePolicy>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QTableView>
#include <QTextStream>
#include <QVBoxLayout>



namespace Isis {



  /** 
   * Creates a view showing the CSV file from BundleObservation. 
   * 
   * @param FileItemQsp fileItem  QSharedPointer to the fileItem from the ProjectItemModel
   */
  BundleObservationView::BundleObservationView(FileItemQsp fileItem, QWidget *parent):
                         AbstractProjectItemView(parent) {

    QStandardItemModel *model = new QStandardItemModel;
    QFile file(fileItem->fileName());

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



