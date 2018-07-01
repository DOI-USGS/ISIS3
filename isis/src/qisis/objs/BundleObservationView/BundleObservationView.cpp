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
#include <QFontDatabase>
#include <QHeaderView>
#include <QSizePolicy>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QTableView>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>


namespace Isis {

  /** 
   * Creates a view showing the CSV or text files from BundleSolutionInfo.
   * 
   * @param FileItemQsp fileItem QSharedPointer to the fileItem from the ProjectItemModel
   */
  BundleObservationView::BundleObservationView(FileItemQsp fileItem, QWidget *parent):
                         AbstractProjectItemView(parent) {

    if (fileItem->fileName().contains(".csv")) {
      displayCsvFile(fileItem);
    }
    else if (fileItem->fileName().contains(".txt")) {
      displayTextFile(fileItem);
    }
  }


  /**
   * Creates a view showing the CSV file from BundleSolutionInfo.
   *
   * @param FileItemQsp fileItem QSharedPointer to the fileItem from the ProjectItemModel
   */
  void BundleObservationView::displayCsvFile(FileItemQsp fileItem) {
    QStandardItemModel *model = new QStandardItemModel;

    if (!QFile::exists(fileItem->fileName())) {
      return;
    }

    QFile file(fileItem->fileName());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      return;
    }

    int numHeaderLines = 3;
    if (fileItem->fileName().contains("images")) {
      numHeaderLines = 2;
    }

    QTextStream in(&file);                 // read to text stream

    // read and populate header from first two or three lines
    QString header1;
    QString header2;
    QString header3;
    QStringList lineToken1;
    QStringList lineToken2;
    QStringList lineToken3;

    header1 = in.readLine();
    lineToken1 = header1.split(",");
    header2 = in.readLine();
    lineToken2 = header2.split(",");

    if (numHeaderLines == 2) {
      for (int i = 0; i < lineToken1.size(); i++) {
        QString t1 = lineToken1.at(i);
        QString t2 = lineToken2.at(i);
        QString head = t1 + "\n" + t2;
        QStandardItem *v1 = new QStandardItem(head);
        model->setHorizontalHeaderItem(i,v1);
      }
    }
    if (numHeaderLines == 3) {
      header3 = in.readLine();
      lineToken3 = header3.split(",");

      lineToken1.append("");
      lineToken2.append("");

      for (int i = 0; i < lineToken3.size(); i++) {
        QString t1 = lineToken1.at(i);
        QString t2 = lineToken2.at(i);
        QString t3 = lineToken3.at(i);
        QString head = t1 + "\n" + t2 + "\n" + t3;
        QStandardItem *v1 = new QStandardItem(head);
        model->setHorizontalHeaderItem(i,v1);
      }
    }

    // populate remainder of table
    int lineindex = 0;
    while (!in.atEnd()) {
      QString fileLine = in.readLine();

      // parse line into separate pieces(tokens) with "," as the delimiter
      QStringList lineToken = fileLine.split(",", QString::SkipEmptyParts);

      bool rejected = false;
      if (lineToken.at(lineToken.size()-1) == "*") {
        rejected = true;
      }

      // load parsed data to model accordingly
      for (int i = 0; i < lineToken.size(); i++) {
        QString value = lineToken.at(i);

        QStandardItem *item = new QStandardItem(value);

        if (rejected) {
          item->setData(QColor(200,0,0), Qt::BackgroundRole);
        }

        model->setItem(lineindex, i, item);
      }
      lineindex++;
    }

    file.close();

    QTableView *qtable=new QTableView();
    qtable->setModel(model);
    qtable->setSortingEnabled(true);

    // resizes to contents based on entire column
    // NOTE: QHeaderView::ResizeToContents does not allow user to resize by dragging column divider
    qtable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
    layout->addWidget(qtable);

    QSizePolicy policy = sizePolicy();
    policy.setHorizontalPolicy(QSizePolicy::Expanding);
    policy.setVerticalPolicy(QSizePolicy::Expanding);
    setSizePolicy(policy);
  }


  /**
   * Creates a view showing a text file from BundleSolutionInfo.
   *
   * @param FileItemQsp fileItem QSharedPointer to the fileItem from the ProjectItemModel
   */
  void BundleObservationView::displayTextFile(FileItemQsp fileItem) {

    if (!QFile::exists(fileItem->fileName())) {
      return;
    }

    QFile file(fileItem->fileName());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      return;
    }

    QTextStream in(&file);
    QTextEdit *qText=new QTextEdit();

    // From QFontDatabase::systemFont(SystemFont type) method description: returns most adequate
    //      font for a given typecase (here FixedFont) for proper integration with system's look and
    //      feel.
    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    qText->setFontFamily(fixedFont.family());

    while (!in.atEnd()) {
      qText->append(in.readLine());
    }

    file.close();

    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
    layout->addWidget(qText);

    qText->moveCursor(QTextCursor::Start);

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



