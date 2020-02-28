#include "StretchType.h"

#include <iostream>

#include <QColor>
#include <QGridLayout>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QTableWidget>
#include <QTextStream>

#include "Stretch.h"
#include "Histogram.h"
#include "HistogramWidget.h"

namespace Isis {
  /**
   * This constructs a stretch type. It provides a main layout,
   * sizing policies, and a few widgets. Children should insert
   * themselves at the main layout grid row 1 column 0.
   *
   * @param hist
   * @param stretch
   * @param name
   * @param color
   */
  StretchType::StretchType(const Histogram &hist,
                           const Stretch &stretch,
                           const QString &name, const QColor &color) {
    p_stretch = NULL;
    p_table = NULL;
    p_cubeHist = NULL;
    p_graph = NULL;
    p_mainLayout = NULL;

    p_cubeHist = new Histogram(hist);

    p_stretch = new Stretch();

    p_graph = new HistogramWidget(QString("Visible ") + name + QString(" Hist"),
                                  color.lighter(110), color.darker(110));
    p_graph->setHistogram(*p_cubeHist);
    p_graph->setSizePolicy(QSizePolicy(QSizePolicy::Minimum,
                                       QSizePolicy::Minimum));
    p_graph->setMinimumSize(QSize(100, 50));

    p_mainLayout = new QGridLayout();
    p_mainLayout->addWidget(p_graph, 0, 0);

    p_table = createStretchTable();
    connect(this, SIGNAL(stretchChanged()), this, SLOT(updateGraph()));
    connect(this, SIGNAL(stretchChanged()), this, SLOT(updateTable()));
    p_mainLayout->addWidget(p_table, 2, 0);

    QPushButton *saveAsButton = new QPushButton("Save Stretch Pairs to File...");
    connect(saveAsButton, SIGNAL(clicked(bool)), this, SLOT(savePairs()));
    p_mainLayout->addWidget(saveAsButton, 3, 0);

    // Save/Restore strech only supported for Grayscale images. Hide buttons if in RGB.
    if (name.compare("Gray") == 0) {
      QPushButton *saveToCubeButton = new QPushButton("Save Stretch Pairs to Cube..."); 
      connect(saveToCubeButton, SIGNAL(clicked(bool)), this, SIGNAL(saveToCube()));
      p_mainLayout->addWidget(saveToCubeButton, 4, 0);

      QPushButton *deleteFromCubeButton = new QPushButton("Delete Stretch Pairs from Cube...");
      connect(deleteFromCubeButton, SIGNAL(clicked(bool)), this, SIGNAL(deleteFromCube()));
      p_mainLayout->addWidget(deleteFromCubeButton, 5, 0);

      QPushButton *loadStretchButton = new QPushButton("Load Saved Stretch from Cube...");
      connect(loadStretchButton, SIGNAL(clicked(bool)), this, SIGNAL(loadStretch()));
      p_mainLayout->addWidget(loadStretchButton, 6, 0);
    }

    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy.setHeightForWidth(true);
    p_graph->setSizePolicy(sizePolicy);
  }


  /**
   * Destructor
   */
  StretchType::~StretchType() {
    if(p_cubeHist) {
      delete p_cubeHist;
      p_cubeHist = NULL;
    }

    if(p_stretch) {
      delete p_stretch;
      p_stretch = NULL;
    }
  }


  /**
   * This should be called when the visible area changes. It
   * updates the graph and calls setStretch() so that the children
   * have a chance to update their GUI elements too.
   *
   * @param hist
   */
  void StretchType::setHistogram(const Histogram &hist) {
    p_graph->setHistogram(hist);
    *p_cubeHist = hist;
    setStretch(*p_stretch);
  }


  /**
   * This creates the stretch pairs table.
   *
   * @return QTableWidget*
   */
  QTableWidget *StretchType::createStretchTable() {
    QTableWidget *table = new QTableWidget(0, 2);

    QStringList labels;
    labels << "Input" << "Output";
    table->setHorizontalHeaderLabels(labels);

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setMinimumSize(QSize(50, 20));

    return table;
  }


  /**
   * This updates the graph with the current stretch object.
   */
  void StretchType::updateGraph() {
    p_graph->setStretch(*p_stretch);
  }


  /**
   * This updates the table with the current stretch pairs.
   */
  void StretchType::updateTable() {
    Stretch stretch = getStretch();
    p_table->setRowCount(stretch.Pairs());

    for(int i = 0; i < stretch.Pairs(); i++) {
      QTableWidgetItem *inputItem = new QTableWidgetItem(QString("%1").arg(
            stretch.Input(i)));
      inputItem->setTextAlignment(Qt::AlignCenter);
      QTableWidgetItem *outputItem = new QTableWidgetItem(QString("%1").arg(
            stretch.Output(i)));
      outputItem->setTextAlignment(Qt::AlignCenter);

      p_table->setItem(i, 0, inputItem);
      p_table->setItem(i, 1, outputItem);
    }
  }


  /**
   * This asks the user for a file and saves the current stretch
   * pairs to that file.
   */
  void StretchType::savePairs() {
    QString filename = QFileDialog::getSaveFileName((QWidget *)parent(),
                       "Choose filename to save under", ".", "Text Files (*.txt)");
    if(filename.isEmpty()) return;

    QFile outfile(filename);
    bool success = outfile.open(QIODevice::WriteOnly);

    if(!success) {
      QMessageBox::critical((QWidget *)parent(),
                            "Error", "Cannot open file, please check permissions");
      return;
    }

    QString currentText;
    QTextStream stream(&outfile);

    Stretch stretch = getStretch();

    //Add the pairs to the file
    stream << stretch.Text() << endl;

    outfile.close();
  }


  /**
   * Returns the current stretch object
   *
   * @return Stretch
   */
  Stretch StretchType::getStretch() {
    return *p_stretch;
  }
}
