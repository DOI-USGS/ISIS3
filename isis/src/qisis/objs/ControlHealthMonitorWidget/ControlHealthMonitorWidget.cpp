/**
 * @file
 * $Date$
 * $Revision$
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
#include "ControlHealthMonitorWidget.h"


#include "ControlNet.h"
#include "IString.h"
#include "Progress.h"
#include <QCheckBox>
#include <QToolBar>
#include <iostream>

#include <QApplication>
#include <QtCore>
#include <QLabel>
#include <QtGui>
#include <QPushButton>
#include <QProgressBar>
#include <QTabWidget>
#include <QPointer>
#include <QTableWidgetItem>
#include <QIcon>
#include <QLineEdit>
#include <QHeaderView>
#include <QDesktopWidget>
#include <QGridLayout>
#include <QWidgetAction>
#include <QMenu>
#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <ControlNet.h>
#include <ControlNetVitals.h>
#include <ControlPoint.h>


namespace Isis {
  QT_CHARTS_USE_NAMESPACE
  /**
   * Explanation
   *
   * @param parent (QWidget *) Pointer to parent widget
   */
  ControlHealthMonitorWidget::ControlHealthMonitorWidget(ControlNetVitals *vitals, QWidget *parent) : QWidget(parent) {
    createGui();
    m_vitals = vitals;

    connect (m_vitals, SIGNAL(networkChanged()),
            this, SLOT(update()));
    update();
  }

  void ControlHealthMonitorWidget::setVitals(ControlNetVitals *vitals) {
    delete m_vitals;
    m_vitals = vitals;
    connect (m_vitals, SIGNAL(networkChanged()),
             this, SLOT(update()));
  }

  void ControlHealthMonitorWidget::initializeEverything() {
    m_pointsFixedLabel       = NULL;
    m_pointsFreeLabel        = NULL;
    m_pointsConstrainedLabel = NULL;
    m_pointChartView         = NULL;
    m_vitals                 = NULL;
    m_statusBar              = NULL;
    m_sizeLabel              = NULL;
    m_numImagesLabel         = NULL;
    m_numPointsLabel         = NULL;
    m_numMeasuresLabel       = NULL;
    m_lastModLabel           = NULL;
    m_imagesMeasuresValue    = NULL;
    m_imagesHullValue        = NULL;
    m_imagesShowingLabel     = NULL;
    m_statusLabel            = NULL;
    m_statusDetails          = NULL;
    m_pointsIgnoredLabel     = NULL;
    m_pointsEditLockedLabel  = NULL;
    m_pointsFewMeasuresLabel = NULL;
    m_pointsShowingLabel     = NULL;
    m_historyTable           = NULL;
    m_imagesTable            = NULL;
    m_pointsTable            = NULL;
    activeImageList          = NULL;
    activePointsList         = NULL;
  }

  void ControlHealthMonitorWidget::createGui() {
    initializeEverything();
    setWindowTitle("Control Net Health Monitor");
    resize(725, 1100);

    // Parent
    QVBoxLayout *gridLayout = new QVBoxLayout;
    gridLayout->setAlignment(Qt::AlignTop);
    gridLayout->setSpacing(5);
    setLayout(gridLayout);

    // Title and net
    QLabel *titleLabel = new QLabel("Control Net Health Monitor");
    QFont font("Arial", 18, QFont::Bold);
    titleLabel->setFont(font);
    titleLabel->setAlignment(Qt::AlignTop);

    QWidget *netWidget = new QWidget;
    QHBoxLayout *netLayout = new QHBoxLayout;
    netLayout->setAlignment(Qt::AlignLeft);

    m_netLabel = new QLabel("Control Network:");
    QFont font2("Arial", 14);
    m_netLabel->setFont(font2);
    m_netLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    gridLayout->addWidget(titleLabel);

    QFont searchFont("Seqoe UI Symbol", 12);

    netLayout->addWidget(m_netLabel);
    netWidget->setLayout(netLayout);

    gridLayout->addWidget(netWidget);

    // 4 net details
    QWidget *stats = new QWidget;
    QHBoxLayout *netStatsLayout = new QHBoxLayout;
    netStatsLayout->setAlignment(Qt::AlignLeft);
    netStatsLayout->setSpacing(25);
    m_sizeLabel   = new QLabel("Size: 253M");
    m_numImagesLabel   = new QLabel("Images:");
    m_numPointsLabel   = new QLabel("Points:");
    m_numMeasuresLabel = new QLabel("Measures:");

    netStatsLayout->addWidget(m_sizeLabel);
    netStatsLayout->addWidget(m_numImagesLabel);
    netStatsLayout->addWidget(m_numPointsLabel);
    netStatsLayout->addWidget(m_numMeasuresLabel);

    stats->setLayout(netStatsLayout);
    gridLayout->addWidget(stats);

    // Status Bar
    m_statusBar = new QProgressBar();
    QPalette p = m_statusBar->palette();
    p.setColor(QPalette::Highlight, Qt::green);
    p.setColor(QPalette::Text, Qt::red);
    m_statusBar->setPalette(p);
    m_statusBar->setRange(0, 0);

    m_statusBar->setFormat("Loading...");
    gridLayout->addWidget(m_statusBar);

    QLabel *modificationLabel = new QLabel("Last Modification: 15:23:22 May 02, 2018");
    gridLayout->addWidget(modificationLabel);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    gridLayout->addSpacing(15);

    gridLayout->addWidget(line);
    gridLayout->addSpacing(15);

    // Tabs
    QTabWidget *tabs = new QTabWidget();

    QWidget *overviewTab = createOverviewTab();
    QWidget *imagesTab = createImagesTab();
    QWidget *pointsTab = createPointsTab();
    QWidget *graphTab = createGraphTab();

    tabs->insertTab(0, overviewTab, "Overview");
    tabs->insertTab(1, imagesTab, "Images");
    tabs->insertTab(2, pointsTab, "Points");
    tabs->insertTab(3, graphTab, "Graph");
    gridLayout->addWidget(tabs);


  }

  QWidget* ControlHealthMonitorWidget::createGraphTab() {
    QWidget *graph = new QWidget();

    QVBoxLayout *graphLayout = new QVBoxLayout;
    graphLayout->setAlignment(Qt::AlignTop);
    graphLayout->setSpacing(5);

    m_pointChartView = new QChartView;
    m_pointChartView->resize(200, 200);
    m_pointChartView->setRenderHint(QPainter::Antialiasing);

    QChart *chart = new QChart();
    chart->setTitle("Point Breakdown");
    chart->setTheme(QChart::ChartThemeBlueCerulean);
    chart->legend()->setAlignment(Qt::AlignRight);
    m_pointChartView->setChart(chart);
    graphLayout->addWidget(m_pointChartView);

    graph->setLayout(graphLayout);
    return graph;

  }

  QWidget* ControlHealthMonitorWidget::createOverviewTab() {
    QWidget *overview = new QWidget();

    QVBoxLayout *overviewLayout = new QVBoxLayout;
    overviewLayout->setAlignment(Qt::AlignTop);
    overviewLayout->setSpacing(5);

    m_statusLabel = new QLabel("Healthy!");
    QFont font3("Arial", 16, QFont::Bold);
    m_statusLabel->setFont(font3);
    m_statusLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_statusDetails = new QLabel("Your network is healthy.");
    QFont font4("Arial", 14);
    m_statusDetails->setFont(font4);
    m_statusDetails->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    QFont font5("Arial", 12);
    m_statusDetails->setFont(font4);
    m_statusDetails->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    overviewLayout->addWidget(m_statusLabel);
    overviewLayout->addWidget(m_statusDetails);
    overviewLayout->addSpacing(50);


    QPushButton *health = new QPushButton("Healthy");
    QPushButton *weak   = new QPushButton("Weak");
    QPushButton *broken = new QPushButton("Broken");

    connect (health, SIGNAL(clicked()),
            this, SLOT(healthy()));
    connect (weak, SIGNAL(clicked()),
            this, SLOT(weak()));
    connect (broken, SIGNAL(clicked()),
            this, SLOT(breakNet()));

    QHBoxLayout *temp = new QHBoxLayout;
    temp->setSpacing(15);
    temp->addWidget(health);
    temp->addWidget(weak);
    temp->addWidget(broken);

    QWidget *temp2 = new QWidget;
    temp2->setLayout(temp);

    overviewLayout->addWidget(temp2);

    QLabel *modLabel = new QLabel("Modification History");
    modLabel->setFont(font5);
    overviewLayout->addWidget(modLabel);

    QStringList headers;
    headers.append("#");
    headers.append("Action");
    headers.append("Timestamp");

    m_historyTable = new QTableWidget();
    m_historyTable->setColumnCount(3);
    m_historyTable->setHorizontalHeaderLabels(headers);
    m_historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    m_historyTable->horizontalHeader()->setStretchLastSection(true);
    m_historyTable->verticalHeader()->setVisible(false);
    m_historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_historyTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_historyTable->setShowGrid(true);
    m_historyTable->setGeometry(QApplication::desktop()->screenGeometry());

    // DUMMY DATA
    for (int i = 0; i < 100; i++) {
      m_historyTable->insertRow(i);
      m_historyTable->setItem(i, 0, new QTableWidgetItem(toString(i)) );

      m_historyTable->setItem(i, 1, new QTableWidgetItem("Point Modified") );
      m_historyTable->setItem(i, 2, new QTableWidgetItem("15:22:61 May 02, 2018"));

    }

    m_historyTable->setColumnWidth(0, 100);
    m_historyTable->setColumnWidth(1, 400);

    overviewLayout->addWidget(m_historyTable);
    overview->setLayout(overviewLayout);
    return overview;
  }

  void ControlHealthMonitorWidget::update() {
    m_numImagesLabel->setText("Images:  " + toString(m_vitals->numImages()));
    m_numPointsLabel->setText("Points " + toString(m_vitals->numPoints()));
    m_numMeasuresLabel->setText("Measures: " + toString(m_vitals->numMeasures()));
    m_netLabel->setText("Control Network: " + m_vitals->getNetworkId());
    m_statusLabel->setText(m_vitals->getStatus());
    m_statusDetails->setText(m_vitals->getStatusDetails());
    m_imagesMeasuresValue->setText(toString(m_vitals->numImagesBelowMeasureThreshold()));
    m_imagesHullValue->setText(toString(m_vitals->numImagesBelowHullTolerance()));
    m_pointsIgnoredLabel->setText(toString(m_vitals->numIgnoredPoints()));
    m_pointsFreeLabel->setText(toString(m_vitals->numFreePoints()));
    m_pointsFixedLabel->setText(toString(m_vitals->numFixedPoints()));
    m_pointsConstrainedLabel->setText(toString(m_vitals->numConstrainedPoints()));

    m_pointsEditLockedLabel->setText(toString(m_vitals->numLockedPoints()));
    m_pointsFewMeasuresLabel->setText(toString(m_vitals->numPointsBelowMeasureThreshold()));
    viewImageAll();
    viewPointAll();

    if (m_vitals->getStatus() == "Broken!") updateStatus(0);
    else if (m_vitals->getStatus() == "Weak!") updateStatus(1);
    else if(m_vitals->getStatus() == "Healthy!") updateStatus(2);

    QPieSeries *series = new QPieSeries();
  series->append("Ignored", m_vitals->numIgnoredPoints());
  series->append("Locked", m_vitals->numLockedPoints());
  series->append("Free", m_vitals->numFreePoints());
  series->append("Constrained", m_vitals->numConstrainedPoints());
  series->append("Fixed", m_vitals->numFixedPoints());
  foreach (QPieSlice *slice, series->slices()) {

    // slice->setExploded();
    // slice->setPen(QPen(Qt::darkGreen, 2));
    double percent = slice->percentage() * 100;
    percent = ( (int) (percent * 100) ) / 100.0;
    // std::cout << percent / 100.0 << std::endl;

    QString label = slice->label() + " " + toString(percent) + "%";

    if (percent > 0.0) {
      slice->setLabelVisible();
    }
    slice->setLabel(label);
    // slice->setBrush(Qt::green);
  }

  m_pointChartView->chart()->removeAllSeries();
  m_pointChartView->chart()->addSeries(series);

  }

  void ControlHealthMonitorWidget::updateImageTable(QList<QString> serials) {
    m_imagesTable->setRowCount(0);
    for (int i = 0; i < serials.size(); i++) {
      m_imagesTable->insertRow(i);
      m_imagesTable->setItem(i, 0, new QTableWidgetItem(toString(i + 1)));
      m_imagesTable->setItem(i, 1, new QTableWidgetItem(serials.at(i)));
    }
  }

  void ControlHealthMonitorWidget::updatePointTable(QList<ControlPoint*> points) {
    m_pointsTable->setRowCount(0);
    for (int i = 0; i < points.size(); i++) {
      ControlPoint *point = points.at(i);
      m_pointsTable->insertRow(i);
      m_pointsTable->setItem(i, 0, new QTableWidgetItem(toString(i + 1)));
      m_pointsTable->setItem(i, 1, new QTableWidgetItem(point->GetId()));
      m_pointsTable->setItem(i, 2, new QTableWidgetItem(point->GetPointTypeString()));
      m_pointsTable->setItem(i, 3, new QTableWidgetItem(toString(point->IsIgnored())));
      m_pointsTable->setItem(i, 4, new QTableWidgetItem(toString(point->IsRejected())));
      m_pointsTable->setItem(i, 5, new QTableWidgetItem(toString(point->IsEditLocked())));
    }
  }

  QWidget* ControlHealthMonitorWidget::createPointsTab() {
    QFont font5("Arial", 12);
    QFont searchFont("Seqoe UI Symbol", 12);

     QWidget *pointsTab = new QWidget();
     QVBoxLayout *pointsLayout = new QVBoxLayout;
     pointsLayout->setAlignment(Qt::AlignTop);
     pointsLayout->setSpacing(15);
     pointsLayout->addSpacing(10);

     QWidget *viewWidget = new QWidget;
     QGridLayout *viewLayout = new QGridLayout;
     QLabel *pointsIgnored = new QLabel("Points Ignored:");
     m_pointsIgnoredLabel  = new QLabel("0");
     QPushButton *viewIgnoredButton = new QPushButton("View");
     connect(viewIgnoredButton, SIGNAL(clicked()),
             this, SLOT(viewPointIgnored()));
     pointsIgnored->setFont(font5);
     m_pointsIgnoredLabel->setFont(font5);

     QLabel *pointsLocked = new QLabel("Points Edit Locked:");
     m_pointsEditLockedLabel  = new QLabel("0");
     QPushButton *viewLockedButton = new QPushButton("View");
     connect(viewLockedButton, SIGNAL(clicked()),
             this, SLOT(viewPointEditLocked()));
     pointsLocked->setFont(font5);
     m_pointsEditLockedLabel->setFont(font5);


     QLabel *pointsMeasure = new QLabel("Less than 3 valid Measures:");
     m_pointsFewMeasuresLabel  = new QLabel("0");
     pointsMeasure->setFont(font5);
     m_pointsFewMeasuresLabel->setFont(font5);

     QPushButton *viewMeasureButton = new QPushButton("View");
     connect(viewMeasureButton, SIGNAL(clicked()),
             this, SLOT(viewPointFewMeasures()));

     QLabel *freePoints = new QLabel("Points Free:");
     m_pointsFreeLabel  = new QLabel("0");
     freePoints->setFont(font5);
     m_pointsFreeLabel->setFont(font5);

     QPushButton *viewFreePoints = new QPushButton("View");
     connect(viewFreePoints, SIGNAL(clicked()),
             this, SLOT(viewPointFree()));

     QLabel *fixedPoints = new QLabel("Points Fixed:");
     m_pointsFixedLabel  = new QLabel("0");
     fixedPoints->setFont(font5);
     m_pointsFixedLabel->setFont(font5);

     QPushButton *viewFixedPoints = new QPushButton("View");
     connect(viewFixedPoints, SIGNAL(clicked()),
             this, SLOT(viewPointFixed()));


     QLabel *constrainedPoints = new QLabel("Points Constrained:");
     m_pointsConstrainedLabel  = new QLabel("0");
     constrainedPoints->setFont(font5);
     m_pointsConstrainedLabel->setFont(font5);

     QPushButton *viewConstrainedPoints = new QPushButton("View");
     connect(viewConstrainedPoints, SIGNAL(clicked()),
             this, SLOT(viewPointConstrained()));


     viewLayout->addWidget(freePoints, 0, 0);
     viewLayout->addWidget(m_pointsFreeLabel, 0, 1);
     viewLayout->addWidget(viewFreePoints, 0, 2);

     viewLayout->addWidget(fixedPoints, 1, 0);
     viewLayout->addWidget(m_pointsFixedLabel, 1, 1);
     viewLayout->addWidget(viewFixedPoints, 1, 2);

     viewLayout->addWidget(constrainedPoints, 2, 0);
     viewLayout->addWidget(m_pointsConstrainedLabel, 2, 1);
     viewLayout->addWidget(viewConstrainedPoints, 2, 2);

     viewLayout->addWidget(pointsIgnored, 3, 0);
     viewLayout->addWidget(m_pointsIgnoredLabel, 3, 1);
     viewLayout->addWidget(viewIgnoredButton, 3, 2);


     viewLayout->addWidget(pointsLocked, 4, 0);
     viewLayout->addWidget(m_pointsEditLockedLabel, 4, 1);
     viewLayout->addWidget(viewLockedButton, 4, 2);

     viewLayout->addWidget(pointsMeasure, 5, 0);
     viewLayout->addWidget(m_pointsFewMeasuresLabel, 5, 1);
     viewLayout->addWidget(viewMeasureButton, 5, 2);

     viewWidget->setLayout(viewLayout);

     pointsLayout->addWidget(viewWidget);

     m_pointsTable = new QTableWidget();
     QStringList headers3;
     headers3.append("#");
     headers3.append("Point ID");
     headers3.append("Type");
     headers3.append("Ignored");
     headers3.append("Rejected");
     headers3.append("Edit Locked");

     m_pointsTable->setColumnCount(6);
     m_pointsTable->setHorizontalHeaderLabels(headers3);
     m_pointsTable->horizontalHeader()->setStretchLastSection(true);
     m_pointsTable->verticalHeader()->setVisible(false);
     m_pointsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
     m_pointsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
     m_pointsTable->setSelectionMode(QAbstractItemView::SingleSelection);
     m_pointsTable->setShowGrid(true);
     m_pointsTable->setGeometry(QApplication::desktop()->screenGeometry());
     m_pointsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

     pointsLayout->addSpacing(30);

     QLineEdit *searchField = new QLineEdit;
     searchField->setFont(searchFont);
     searchField->setPlaceholderText("🔍");

     searchField->setClearButtonEnabled(true);

     m_pointsShowingLabel = new QLabel("");
     m_pointsShowingLabel->setFont(font5);

     pointsLayout->addWidget(m_pointsShowingLabel);
     pointsLayout->addWidget(searchField);

     pointsLayout->addWidget(m_pointsTable);



   pointsTab->setLayout(pointsLayout);
   return pointsTab;

  }

  QWidget* ControlHealthMonitorWidget::createImagesTab() {

    QWidget *imagesTab = new QWidget(this);
    QVBoxLayout *imagesLayout = new QVBoxLayout;
    imagesTab->setLayout(imagesLayout);
    imagesLayout->setAlignment(Qt::AlignTop);
    imagesLayout->setSpacing(15);
    imagesLayout->addSpacing(10);

    QWidget *temp = new QWidget;
    QGridLayout *tempLayout = new QGridLayout;

    QFont font5("Arial", 12);

    QLabel *threeMeasure = new QLabel("Less than 3 valid Measures:");

    m_imagesMeasuresValue = new QLabel("");

    m_imagesMeasuresValue->setFont(font5);

    QPushButton *button = new QPushButton("View");

    connect(button, SIGNAL(clicked()),
            this, SLOT(viewImageFewMeasures()));

    tempLayout->addWidget(threeMeasure, 0, 0);
    tempLayout->addWidget(m_imagesMeasuresValue, 0, 1);
    tempLayout->addWidget(button, 0, 2);
    temp->setLayout(tempLayout);
    threeMeasure->setFont(font5);

    QLabel *withoutMeasures = new QLabel("Exceeding convex hull tolerance:");
    withoutMeasures->setFont(font5);

    m_imagesHullValue = new QLabel("");
    m_imagesHullValue->setFont(font5);
    QPushButton *button2 = new QPushButton("View");
    connect(button2, SIGNAL(clicked()),
            this, SLOT(viewImageHullTolerance()));

    tempLayout->addWidget(withoutMeasures, 1, 0);
    tempLayout->addWidget(m_imagesHullValue, 1, 1);
    tempLayout->addWidget(button2, 1, 2);

    imagesLayout->addWidget(temp);

    m_imagesTable = new QTableWidget();
    m_imagesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QStringList headers2;
    headers2.append("#");
    headers2.append("Cube Serial");

    m_imagesTable->setColumnCount(2);
    m_imagesTable->setHorizontalHeaderLabels(headers2);
    m_imagesTable->horizontalHeader()->setStretchLastSection(true);
    m_imagesTable->verticalHeader()->setVisible(false);
    m_imagesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_imagesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_imagesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_imagesTable->setShowGrid(true);
    m_imagesTable->setGeometry(QApplication::desktop()->screenGeometry());

    imagesLayout->addSpacing(30);

    QLineEdit *searchField = new QLineEdit;
    QFont searchFont("Seqoe UI Symbol", 12);
    searchField->setFont(searchFont);
    searchField->setPlaceholderText("🔍");

    searchField->setClearButtonEnabled(true);

    m_imagesShowingLabel = new QLabel("");
    m_imagesShowingLabel->setFont(font5);

    imagesLayout->addWidget(m_imagesShowingLabel);
    imagesLayout->addWidget(searchField);
    imagesLayout->addWidget(m_imagesTable);

    imagesTab->setLayout(imagesLayout);
    return imagesTab;
  }

  void ControlHealthMonitorWidget::viewPointAll() {
    updatePointTable(m_vitals->getAllPoints());
    m_pointsShowingLabel->setText("Showing: All Points <sup>" +
                                  toString(m_vitals->numPoints()) +
                                  " / " + toString(m_vitals->numPoints()) + "</sup>");
  }

  void ControlHealthMonitorWidget::viewPointIgnored() {
    updatePointTable(m_vitals->getIgnoredPoints());
    m_pointsShowingLabel->setText("Showing: Ignored Points <sup>" +
                                  toString(m_vitals->numIgnoredPoints()) +
                                  " / " + toString(m_vitals->numPoints()) + "</sup>");
  }

  void ControlHealthMonitorWidget::viewPointFree() {
    updatePointTable(m_vitals->getFreePoints());
    m_pointsShowingLabel->setText("Showing: Free Points <sup>" +
                                  toString(m_vitals->numFreePoints()) +
                                  " / " + toString(m_vitals->numPoints()) + "</sup>");
  }

  void ControlHealthMonitorWidget::viewPointFixed() {
    updatePointTable(m_vitals->getFixedPoints());
    m_pointsShowingLabel->setText("Showing: Fixed Points <sup>" +
                                  toString(m_vitals->numFixedPoints()) +
                                  " / " + toString(m_vitals->numPoints()) + "</sup>");
  }

  void ControlHealthMonitorWidget::viewPointConstrained() {
    updatePointTable(m_vitals->getConstrainedPoints());
    m_pointsShowingLabel->setText("Showing: Constrained Points <sup>" +
                                  toString(m_vitals->numConstrainedPoints()) +
                                  " / " + toString(m_vitals->numPoints()) + "</sup>");
  }

  void ControlHealthMonitorWidget::viewPointEditLocked() {
    updatePointTable(m_vitals->getLockedPoints());
    m_pointsShowingLabel->setText("Showing: Locked Points <sup>" +
                                  toString(m_vitals->numLockedPoints()) +
                                  " / " + toString(m_vitals->numPoints()) + "</sup>");

  }

  void ControlHealthMonitorWidget::viewPointFewMeasures() {
    updatePointTable(m_vitals->getPointsBelowMeasureThreshold());
    m_pointsShowingLabel->setText("Showing: Points with less than 3 Measures <sup>" +
                                  toString(m_vitals->numPointsBelowMeasureThreshold()) +
                                  " / " + toString(m_vitals->numPoints()) + "</sup>");
  }

  void ControlHealthMonitorWidget::viewImageAll() {
    updateImageTable(m_vitals->getAllImageSerials());
    m_imagesShowingLabel->setText("Showing: All Images <sup>" +
                                  toString(m_vitals->numImages()) +
                                  " / " + toString(m_vitals->numImages()) + "</sup>");
  }

  void ControlHealthMonitorWidget::viewImageFewMeasures() {
    updateImageTable(m_vitals->getImagesBelowMeasureThreshold());
    m_imagesShowingLabel->setText("Showing: Images with less than 3 Measures <sup>" +
                                  toString(m_vitals->numImagesBelowMeasureThreshold()) +
                                  " / " + toString(m_vitals->numImages()) + "</sup>");
  }

  void ControlHealthMonitorWidget::viewImageHullTolerance() {
    updateImageTable(m_vitals->getImagesBelowHullTolerance());
    m_imagesShowingLabel->setText("Showing: Images below a hull tolerance of 75% <sup>" +
                                  toString(m_vitals->numImagesBelowHullTolerance()) +
                                  " / " + toString(m_vitals->numImages()) + "</sup>");
  }


  /**
   * Destructor
   */
  ControlHealthMonitorWidget::~ControlHealthMonitorWidget() {
    // delete m_sizeLabel;
    // delete m_numImagesLabel;
    // delete m_numPointsLabel;
    // delete m_numMeasuresLabel;
    // delete m_lastModLabel;
    //
    // delete m_imagesMeasuresValue;
    // delete m_imagesHullValue;
    // delete m_imagesShowingLabel;
    // delete m_statusLabel;
    // delete m_statusDetails;
    //
    // delete m_pointsIgnoredLabel;
    // delete m_pointsEditLockedLabel;
    // delete m_pointsFewMeasuresLabel;
    // delete m_pointsShowingLabel;
    //
    // delete m_historyTable;
    // delete m_imagesTable;
    // delete m_pointsTable;
    //
    // delete m_ignoredPoints;
    // delete m_editLockedPoints;
    // delete m_pointsFewMeasures;
    // delete m_imagesFewMeasures;
    // delete m_imagesHullTolerance;
    // delete activeImageList;
    // delete activePointsList;
    //
    // m_sizeLabel = NULL;
    // m_numImagesLabel = NULL;
    // m_numPointsLabel = NULL;
    // m_numMeasuresLabel = NULL;
    // m_lastModLabel = NULL;
    //
    // m_imagesMeasuresValue = NULL;
    // m_imagesHullValue = NULL;
    // m_imagesShowingLabel = NULL;
    // m_statusLabel = NULL;
    // m_statusDetails = NULL;
    //
    // m_pointsIgnoredLabel = NULL;
    // m_pointsEditLockedLabel = NULL;
    // m_pointsFewMeasuresLabel = NULL;
    // m_pointsShowingLabel = NULL;
    //
    // m_historyTable = NULL;
    // m_imagesTable = NULL;
    // m_pointsTable = NULL;
    //
    // m_ignoredPoints = NULL;
    // m_editLockedPoints = NULL;
    // m_pointsFewMeasures = NULL;
    // m_imagesFewMeasures = NULL;
    // m_imagesHullTolerance = NULL;
    // activeImageList = NULL;
    // activePointsList = NULL;

  }

  void ControlHealthMonitorWidget::breakNet() {

    QPalette p = m_statusBar->palette();
    m_statusLabel->setText("Broken!");
    m_statusDetails->setText("This Control Network has " + toString(m_vitals->numIslands()) + " island(s).");
    p.setColor(QPalette::Highlight, Qt::red);
    p.setColor(QPalette::Text, Qt::black);
    m_statusBar->setPalette(p);

  }

  void ControlHealthMonitorWidget::weak() {
    QPalette p = m_statusBar->palette();
    m_statusLabel->setText("Weak!");
    m_statusDetails->setText("This Control Network has " + toString(m_vitals->numPointsBelowMeasureThreshold()) + " point(s) with less than 3 valid measures.");
    p.setColor(QPalette::Highlight, Qt::yellow);
    p.setColor(QPalette::Text, Qt::black);
    m_statusBar->setPalette(p);
  }

  void ControlHealthMonitorWidget::healthy() {
    QPalette p = m_statusBar->palette();
    m_statusLabel->setText("Healthy!");
    m_statusDetails->setText("This Control Network is Healthy.");
    p.setColor(QPalette::Highlight, Qt::green);
    p.setColor(QPalette::Text, Qt::white);
    m_statusBar->setPalette(p);
  }

  void ControlHealthMonitorWidget::updateStatus(int code) {
    QPalette p = m_statusBar->palette();
    switch(code) {
      case 0:
        p.setColor(QPalette::Highlight, Qt::red);
        p.setColor(QPalette::Text, Qt::black);
        break;
      case 1:
        p.setColor(QPalette::Highlight, Qt::yellow);
        p.setColor(QPalette::Text, Qt::black);
        break;
      case 2:
        p.setColor(QPalette::Highlight, Qt::green);
        p.setColor(QPalette::Text, Qt::white);
        break;
    }
    m_statusBar->setPalette(p);
  }

}
