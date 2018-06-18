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
   * This class is the front end representation of a ControlNetVitals object.
   * It will accept a ControlNetVitals object upon initialization and reflects the current
   * real-time status of the embedded ControlNet in the ControlNetVitals object.
   *
   * @param vitals (ControlNetVitals *) The ControlNetVitals object that contains the ControlNet.
   * @param parent (QWidget *) Pointer to parent widget
   */
  ControlHealthMonitorWidget::ControlHealthMonitorWidget(ControlNetVitals *vitals, QWidget *parent) : QWidget(parent) {

    createGui();
    m_vitals = vitals;
    connect (m_vitals, SIGNAL(networkChanged()),
            this, SLOT(update()));
    update();
  }

  /**
   *  This SLOT is called whenever the is a change made to the network embedded in the
   *  Global m_vitals object. Changes are detected via the networkChanged() signal which
   *  is emitted from the ControlNetVitals object which is triggered whenever
   *  networkStructureModified() is emitted from the embedded ControlNet.
   *
   */
  void ControlHealthMonitorWidget::update() {
    m_numImagesLabel->setText("Images: " + toString(m_vitals->numImages()));
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

    // We should enumerate the network state and do a comparison on enums here, not strings.
    if (m_vitals->getStatus() == "Broken!") updateStatus(0);
    else if (m_vitals->getStatus() == "Weak!") updateStatus(1);
    else if (m_vitals->getStatus() == "Healthy!") updateStatus(2);

    // QPieSeries series;
    // series.append("Free", m_vitals->numFreePoints());
    // series.append("Constrained", m_vitals->numConstrainedPoints());
    // series.append("Fixed", m_vitals->numFixedPoints());
    //
    // foreach (QPieSlice *slice, series->slices()) {
    //
    //   // Get the percent and round it to two decimal places.
    //   double percent = slice->percentage() * 100;
    //   percent = ( (int) (percent * 100) ) / 100.0;
    //
    //   QString label = slice->label() + " " + toString(percent) + "%";
    //
    //   if (percent > 0.0) {
    //     slice->setLabelVisible();
    //   }
    //   slice->setLabel(label);
    // }
    // //
    // m_pointChartView->chart()->removeAllSeries();
    // m_pointChartView->chart()->addSeries(series);

    viewImageAll();
    viewPointAll();
  }

  void ControlHealthMonitorWidget::broken() {
    updateStatus(0);
    m_statusLabel->setText("Broken!");
    m_statusDetails->setText("This network has 2 islands.");
  }

  void ControlHealthMonitorWidget::weak() {
    updateStatus(1);
    m_statusLabel->setText("Weak!");
    m_statusDetails->setText("This network has " + toString(m_vitals->numPointsBelowMeasureThreshold()) + " points "
                             + "with less than 3 measures.");
  }

  void ControlHealthMonitorWidget::healthy() {
    updateStatus(2);
    m_statusLabel->setText("Healthy!");
    m_statusDetails->setText("This network is healthy.");
  }

  /*
   *  This SLOT is designed to update the values in the gui to properly represent
   *  The current state of the Control Network. This SLOT is triggered whenever the
   *  projectStructureModified() signal is emitted from the Control Network, which triggers
   *  the "Update()" signal in the ControlNetVitals class in which this slot is connected.
   *
   *  The status bar will display the proper color with respect to the health of the network,
   *  And will display details related to that health as well.
   *
   *  @param code The status code. Should be an ENUM eventually for the 3 network states.
   */
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

  /**
   *  This method is responsible for creating all of the components that comprise the GUI.
   */
  void ControlHealthMonitorWidget::createGui() {

    initializeEverything();
    setWindowTitle("Control Net Health Monitor");
    resize(725, 1100);

    QFont fontBig("Arial", 18, QFont::Bold);
    QFont fontNormal("Arial", 14);
    QFont searchFont("Seqoe UI Symbol", 12);

    // Parent layout for this entire widget.
    QVBoxLayout *gridLayout = new QVBoxLayout;
    gridLayout->setAlignment(Qt::AlignTop);
    gridLayout->setSpacing(5);
    setLayout(gridLayout);

    // Title and net
    QLabel *titleLabel = new QLabel("Control Net Health Monitor");
    titleLabel->setFont(fontBig);
    titleLabel->setAlignment(Qt::AlignTop);

    QWidget *netWidget = new QWidget;
    QHBoxLayout *netLayout = new QHBoxLayout;
    netLayout->setAlignment(Qt::AlignLeft);

    m_netLabel = new QLabel("Control Network:");
    m_netLabel->setFont(fontNormal);
    m_netLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    gridLayout->addWidget(titleLabel);

    netLayout->addWidget(m_netLabel);
    netWidget->setLayout(netLayout);

    gridLayout->addWidget(netWidget);

    // 4 net details, size, images, points, measures.
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

    // We need to connect this properly.
    QLabel *modificationLabel = new QLabel("Last Modification:");
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
    tabs->insertTab(1, imagesTab,   "Images");
    tabs->insertTab(2, pointsTab,   "Points");
    tabs->insertTab(3, graphTab,    "Graph");

    gridLayout->addWidget(tabs);
  }

  /**
  *  Initializes all member variables to NULL.
  *
  */
  void ControlHealthMonitorWidget::initializeEverything() {
    m_historyTable           = NULL;
    m_imagesHullValue        = NULL;
    m_imagesMeasuresValue    = NULL;
    m_imagesShowingLabel     = NULL;
    m_imagesTable            = NULL;
    m_lastModLabel           = NULL;
    m_numImagesLabel         = NULL;
    m_numMeasuresLabel       = NULL;
    m_numPointsLabel         = NULL;
    m_pointChartView         = NULL;
    m_pointsEditLockedLabel  = NULL;
    m_pointsFewMeasuresLabel = NULL;
    m_pointsIgnoredLabel     = NULL;
    m_pointsShowingLabel     = NULL;
    m_pointsTable            = NULL;
    m_sizeLabel              = NULL;
    m_statusBar              = NULL;
    m_statusDetails          = NULL;
    m_statusLabel            = NULL;
    m_vitals                 = NULL;
  }

  /*
  *  This method creates the Overview tab.
  *
  */
  QWidget* ControlHealthMonitorWidget::createOverviewTab() {

    // Parent container for the overview tab.
    QWidget *overview = new QWidget();
    QVBoxLayout *overviewLayout = new QVBoxLayout;
    overviewLayout->setAlignment(Qt::AlignTop);
    overviewLayout->setSpacing(5);

    QFont fontBig("Arial", 16, QFont::Bold);
    QFont fontNormal("Arial", 14);
    QFont fontSmall("Arial", 12);

    m_statusLabel = new QLabel("Healthy!");
    m_statusLabel->setFont(fontBig);
    m_statusLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_statusDetails = new QLabel("Your network is healthy.");
    m_statusDetails->setFont(fontNormal);
    m_statusDetails->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_statusDetails->setFont(fontNormal);
    m_statusDetails->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    overviewLayout->addWidget(m_statusLabel);
    overviewLayout->addWidget(m_statusDetails);
    overviewLayout->addSpacing(50);

    QLabel *modLabel = new QLabel("Modification History");
    modLabel->setFont(fontSmall);
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
    m_historyTable->setGeometry(QApplication::desktop()->screenGeometry());

    overviewLayout->addWidget(m_historyTable);


    QWidget *tempWidget = new QWidget;
    QHBoxLayout *tempLayout = new QHBoxLayout;

    tempLayout->setSpacing(15);

    QPushButton *broken = new QPushButton("Broken");
    QPushButton *weak = new QPushButton("Weak");
    QPushButton *healthy = new QPushButton("Healthy");

    connect (broken,  SIGNAL(clicked()), this, SLOT(broken()));
    connect (weak,    SIGNAL(clicked()), this, SLOT(weak()));
    connect (healthy, SIGNAL(clicked()), this, SLOT(healthy()));

    tempLayout->addWidget(broken);
    tempLayout->addWidget(weak);
    tempLayout->addWidget(healthy);

    tempWidget->setLayout(tempLayout);
    overviewLayout->addWidget(tempWidget);



    overview->setLayout(overviewLayout);
    return overview;
  }

  /*
  *  This method creates the Images tab.
  *
  */
  QWidget* ControlHealthMonitorWidget::createImagesTab() {
    QFont fontSmall("Arial", 12);
    QFont fontMedium("Arial", 14);


    // This is the parent QWidget for the images tab.
    QWidget *imagesTab = new QWidget();
    QVBoxLayout *imagesLayout = new QVBoxLayout;
    imagesLayout->setAlignment(Qt::AlignTop);
    imagesLayout->setSpacing(15);
    imagesLayout->addSpacing(10);

    QWidget *temp = new QWidget;
    QGridLayout *tempLayout = new QGridLayout;

    // Create the labels
    QLabel *threeMeasure = new QLabel("Less than 3 valid Measures:");
    m_imagesMeasuresValue = new QLabel("");

    QLabel *withoutMeasures = new QLabel("Exceeding convex hull tolerance:");
    m_imagesHullValue = new QLabel("");

    // Set the fonts
    m_imagesMeasuresValue->setFont(fontSmall);
    threeMeasure->setFont(fontSmall);
    withoutMeasures->setFont(fontSmall);
    m_imagesHullValue->setFont(fontSmall);

    // Create the view buttons
    QPushButton *button = new QPushButton("View");
    QPushButton *button2 = new QPushButton("View");

    connect(button,  SIGNAL(clicked()), this, SLOT(viewImageFewMeasures()));
    connect(button2, SIGNAL(clicked()), this, SLOT(viewImageHullTolerance()));

    // Add everything in the right spot.
    tempLayout->addWidget(threeMeasure, 0, 0);
    tempLayout->addWidget(m_imagesMeasuresValue, 0, 1);
    tempLayout->addWidget(button, 0, 2);

    tempLayout->addWidget(withoutMeasures, 1, 0);
    tempLayout->addWidget(m_imagesHullValue, 1, 1);
    tempLayout->addWidget(button2, 1, 2);

    temp->setLayout(tempLayout);

    imagesLayout->addWidget(temp);

    // Create the table.
    m_imagesTable = new QTableWidget();

    QStringList headers;
    headers.append("#");
    headers.append("Cube Serial");

    m_imagesTable->setColumnCount(2);
    m_imagesTable->setHorizontalHeaderLabels(headers);
    m_imagesTable->horizontalHeader()->setStretchLastSection(true);
    m_imagesTable->verticalHeader()->setVisible(false);
    m_imagesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_imagesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_imagesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_imagesTable->setShowGrid(true);
    m_imagesTable->setGeometry(QApplication::desktop()->screenGeometry());
    m_imagesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    imagesLayout->addSpacing(30);

    m_imagesShowingLabel = new QLabel("");
    m_imagesShowingLabel->setFont(fontMedium);

    imagesLayout->addWidget(m_imagesShowingLabel);
    imagesLayout->addWidget(m_imagesTable);

    imagesTab->setLayout(imagesLayout);
    return imagesTab;
  }

  /*
  *  This method creates the Points tab.
  *
  */
  QWidget* ControlHealthMonitorWidget::createPointsTab() {

    QFont fontSmall("Arial", 12);
    QFont fontMedium("Arial", 14);

    QFont searchFont("Seqoe UI Symbol", 12);

    // This is the main parent widget for the points tab.
    QWidget *pointsTab = new QWidget();
    QVBoxLayout *pointsLayout = new QVBoxLayout;
    pointsLayout->setAlignment(Qt::AlignTop);
    pointsLayout->setSpacing(15);
    pointsLayout->addSpacing(10);

    QWidget *viewWidget     = new QWidget;
    QGridLayout *viewLayout = new QGridLayout;

    // Create the labels.
    QLabel *pointsIgnored = new QLabel("Points Ignored:");
    m_pointsIgnoredLabel = new QLabel("");

    QLabel *pointsLocked = new QLabel("Points Edit Locked:");
    m_pointsEditLockedLabel = new QLabel("");

    QLabel *pointsMeasure = new QLabel("Less than 3 valid Measures:");
    m_pointsFewMeasuresLabel = new QLabel("");

    QLabel *freePoints = new QLabel("Points Free:");
    m_pointsFreeLabel = new QLabel("");

    QLabel *fixedPoints = new QLabel("Points Fixed:");
    m_pointsFixedLabel = new QLabel("");

    QLabel *constrainedPoints = new QLabel("Points Constrained:");
    m_pointsConstrainedLabel = new QLabel("");

    // Set the font for the labels.
    pointsLocked->setFont(fontSmall);
    m_pointsEditLockedLabel->setFont(fontSmall);
    pointsMeasure->setFont(fontSmall);
    m_pointsFewMeasuresLabel->setFont(fontSmall);
    freePoints->setFont(fontSmall);
    m_pointsFreeLabel->setFont(fontSmall);
    fixedPoints->setFont(fontSmall);
    constrainedPoints->setFont(fontSmall);
    pointsIgnored->setFont(fontSmall);
    m_pointsFixedLabel->setFont(fontSmall);
    m_pointsConstrainedLabel->setFont(fontSmall);
    m_pointsIgnoredLabel->setFont(fontSmall);

    // Create the view buttons.
    QPushButton *viewIgnoredButton     = new QPushButton("View");
    QPushButton *viewLockedButton      = new QPushButton("View");
    QPushButton *viewMeasureButton     = new QPushButton("View");
    QPushButton *viewFreePoints        = new QPushButton("View");
    QPushButton *viewFixedPoints       = new QPushButton("View");
    QPushButton *viewConstrainedPoints = new QPushButton("View");

    // Connect the buttons.
    connect(viewIgnoredButton, SIGNAL(clicked()), this, SLOT(viewPointIgnored()));
    connect(viewLockedButton,  SIGNAL(clicked()), this, SLOT(viewPointEditLocked()));
    connect(viewMeasureButton, SIGNAL(clicked()), this, SLOT(viewPointFewMeasures()));
    connect(viewFreePoints,    SIGNAL(clicked()), this, SLOT(viewPointFree()));
    connect(viewFixedPoints,   SIGNAL(clicked()), this, SLOT(viewPointFixed()));
    connect(viewConstrainedPoints, SIGNAL(clicked()), this, SLOT(viewPointConstrained()));

    // Add the widgets in the proper place.
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

    // Create the table.
    m_pointsTable = new QTableWidget();
    QStringList headers;
    headers.append("#");
    headers.append("Point ID");
    headers.append("Type");
    headers.append("Ignored");
    headers.append("Rejected");
    headers.append("Edit Locked");

    m_pointsTable->setColumnCount(6);
    m_pointsTable->setHorizontalHeaderLabels(headers);
    m_pointsTable->horizontalHeader()->setStretchLastSection(true);
    m_pointsTable->verticalHeader()->setVisible(false);
    m_pointsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pointsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pointsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pointsTable->setShowGrid(true);
    m_pointsTable->setGeometry(QApplication::desktop()->screenGeometry());
    m_pointsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    pointsLayout->addSpacing(30);

    m_pointsShowingLabel = new QLabel("");
    m_pointsShowingLabel->setFont(fontMedium);

    pointsLayout->addWidget(m_pointsShowingLabel);
    pointsLayout->addWidget(m_pointsTable);

    pointsTab->setLayout(pointsLayout);
    return pointsTab;
  }

  /*
  *  This method creates the Graph tab.
  *
  */
  QWidget* ControlHealthMonitorWidget::createGraphTab() {
    QWidget *graph = new QWidget();

    QVBoxLayout *graphLayout = new QVBoxLayout;
    graphLayout->setAlignment(Qt::AlignTop);
    graphLayout->setSpacing(5);
    //
    // m_pointChartView = new QChartView;
    // m_pointChartView->resize(200, 200);
    // m_pointChartView->setRenderHint(QPainter::Antialiasing);
    //
    // QChart *chart = new QChart();
    // chart->setTitle("Point Breakdown");
    // chart->setTheme(QChart::ChartThemeBlueCerulean);
    // chart->legend()->setAlignment(Qt::AlignRight);
    // m_pointChartView->setChart(chart);
    // graphLayout->addWidget(m_pointChartView);

    graph->setLayout(graphLayout);
    return graph;

  }

  /*
  *  This method loads a QList of cube serials into the images table.
  *
  */
  void ControlHealthMonitorWidget::updateImageTable(QList<QString> serials) {
     m_imagesTable->setRowCount(0);
     for (int i = 0; i < serials.size(); i++) {
       m_imagesTable->insertRow(i);
       m_imagesTable->setItem(i, 0, new QTableWidgetItem(toString(i + 1)));
       m_imagesTable->setItem(i, 1, new QTableWidgetItem(serials.at(i)));
     }
   }

  /*
   *  This method loads a QList of ControlPoint* into the points table.
   *
   */
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

  /*
   *  This SLOT is designed to view all points in the Control Network.
   *
   */
  void ControlHealthMonitorWidget::viewPointAll() {
    updatePointTable(m_vitals->getAllPoints());
    m_pointsShowingLabel->setText("Showing: All Points <sup>" +
                                  toString(m_vitals->numPoints()) +
                                  " / " + toString(m_vitals->numPoints()) + "</sup>");
  }

  /*
   *  This SLOT is designed to view ignored points in the Control Network.
   *
   */
  void ControlHealthMonitorWidget::viewPointIgnored() {
    updatePointTable(m_vitals->getIgnoredPoints());
    m_pointsShowingLabel->setText("Showing: Ignored Points <sup>" +
                                  toString(m_vitals->numIgnoredPoints()) +
                                  " / " + toString(m_vitals->numPoints()) + "</sup>");
  }

  /*
   *  This SLOT is designed to view free points in the Control Network.
   *
   */
  void ControlHealthMonitorWidget::viewPointFree() {
    updatePointTable(m_vitals->getFreePoints());
    m_pointsShowingLabel->setText("Showing: Free Points <sup>" +
                                  toString(m_vitals->numFreePoints()) +
                                  " / " + toString(m_vitals->numPoints()) + "</sup>");
  }

  /*
   *  This SLOT is designed to view fixed points in the Control Network.
   *
   */
  void ControlHealthMonitorWidget::viewPointFixed() {
    updatePointTable(m_vitals->getFixedPoints());
    m_pointsShowingLabel->setText("Showing: Fixed Points <sup>" +
                                  toString(m_vitals->numFixedPoints()) +
                                  " / " + toString(m_vitals->numPoints()) + "</sup>");
  }

  /*
   *  This SLOT is designed to view constrained points in the Control Network.
   *
   */
  void ControlHealthMonitorWidget::viewPointConstrained() {
    updatePointTable(m_vitals->getConstrainedPoints());
    m_pointsShowingLabel->setText("Showing: Constrained Points <sup>" +
                                  toString(m_vitals->numConstrainedPoints()) +
                                  " / " + toString(m_vitals->numPoints()) + "</sup>");
  }

  /*
   *  This SLOT is designed to view locked points in the Control Network.
   *
   */
  void ControlHealthMonitorWidget::viewPointEditLocked() {
    updatePointTable(m_vitals->getLockedPoints());
    m_pointsShowingLabel->setText("Showing: Locked Points <sup>" +
                                  toString(m_vitals->numLockedPoints()) +
                                  " / " + toString(m_vitals->numPoints()) + "</sup>");

  }

  /*
   *  This SLOT is designed to view points with less than 3 valid measures in the Control Network.
   *
   */
  void ControlHealthMonitorWidget::viewPointFewMeasures() {
    updatePointTable(m_vitals->getPointsBelowMeasureThreshold());
    m_pointsShowingLabel->setText("Showing: Points with less than 3 Measures <sup>" +
                                  toString(m_vitals->numPointsBelowMeasureThreshold()) +
                                  " / " + toString(m_vitals->numPoints()) + "</sup>");
  }

  /*
   *  This SLOT is designed to view all images in the Control Network.
   *
   */
  void ControlHealthMonitorWidget::viewImageAll() {
    updateImageTable(m_vitals->getCubeSerials());
    m_imagesShowingLabel->setText("Showing: All Images <sup>" +
                                  toString(m_vitals->numImages()) +
                                  " / " + toString(m_vitals->numImages()) + "</sup>");
  }

  /*
   *  This SLOT is designed to view images with less than 3 valid measures in the Control Network.
   *
   */
  void ControlHealthMonitorWidget::viewImageFewMeasures() {
    updateImageTable(m_vitals->getImagesBelowMeasureThreshold());
    m_imagesShowingLabel->setText("Showing: Images with less than 3 Measures <sup>" +
                                  toString(m_vitals->numImagesBelowMeasureThreshold()) +
                                  " / " + toString(m_vitals->numImages()) + "</sup>");
  }

  /*
   *  This SLOT is designed to view images below the Convex Hull Tolerance in the Control Network.
   *
   */
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

    delete m_historyTable;
    delete m_imagesHullValue;
    delete m_imagesMeasuresValue;
    delete m_imagesShowingLabel;
    delete m_imagesTable;
    delete m_lastModLabel;
    delete m_numImagesLabel;
    delete m_numMeasuresLabel;
    delete m_numPointsLabel;
    delete m_pointChartView;
    delete m_pointsEditLockedLabel;
    delete m_pointsFewMeasuresLabel;
    delete m_pointsIgnoredLabel;
    delete m_pointsShowingLabel;
    delete m_pointsTable;
    delete m_sizeLabel;
    delete m_statusBar;
    delete m_statusDetails;
    delete m_statusLabel;
    delete m_vitals;

    m_historyTable           = NULL;
    m_imagesHullValue        = NULL;
    m_imagesMeasuresValue    = NULL;
    m_imagesShowingLabel     = NULL;
    m_imagesTable            = NULL;
    m_lastModLabel           = NULL;
    m_numImagesLabel         = NULL;
    m_numMeasuresLabel       = NULL;
    m_numPointsLabel         = NULL;
    m_pointChartView         = NULL;
    m_pointsEditLockedLabel  = NULL;
    m_pointsFewMeasuresLabel = NULL;
    m_pointsIgnoredLabel     = NULL;
    m_pointsShowingLabel     = NULL;
    m_pointsTable            = NULL;
    m_sizeLabel              = NULL;
    m_statusBar              = NULL;
    m_statusDetails          = NULL;
    m_statusLabel            = NULL;
    m_vitals                 = NULL;
  }
}
