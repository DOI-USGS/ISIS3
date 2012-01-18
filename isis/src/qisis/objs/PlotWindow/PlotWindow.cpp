#include "IsisDebug.h"

#include "PlotWindow.h"

#include <algorithm>
#include <iostream>

#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_symbol.h>
#include <qwt_scale_engine.h>

#include <QAction>
#include <QIcon>
#include <QMap>
#include <QSet>

#include "Cube.h"
#include "CubePlotCurve.h"
#include "CubeViewport.h"
#include "Filename.h"
#include "Interpolator.h"
#include "MainWindow.h"
#include "MdiCubeViewport.h"
#include "PlotWindowBestFitDialog.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlKeyword.h"
#include "QHistogram.h"
#include "Stretch.h"
#include "TableMainWindow.h"


using namespace std;

namespace Isis {
  /**
   * This constructs a plot window. The plot window graphs any
   * curve sent to it via the addPlotCurve() method.
   *
   *
   * @param title
   * @param parent
   */
  PlotWindow::PlotWindow(QString title, PlotCurve::Units xAxisUnits,
                         PlotCurve::Units yAxisUnits, QWidget *parent,
                         MenuOptions optionsToProvide) :
        MainWindow(title, parent) {
    m_toolBar = NULL;
    m_menubar = NULL;
    m_tableWindow = NULL;
    m_pasteAct = NULL;
    m_allowUserToAddCurves = true;
    m_autoscaleAxes = true;

    m_parent = parent;
    m_xAxisUnits = xAxisUnits;
    m_yAxisUnits = yAxisUnits;

    if (!m_parent) {
      iString msg = "PlotWindow cannot be instantiated with a NULL parent";
      throw iException::Message(iException::Programmer, msg, _FILEINFO_);
    }

    installEventFilter(this);
    setAcceptDrops(true);

    createWidgets(optionsToProvide);
    setWindowTitle(title);

    setPlotBackground(Qt::black);

    connect(QApplication::clipboard(), SIGNAL(changed(QClipboard::Mode)),
            this, SLOT(onClipboardChanged()));
    connect(this, SIGNAL(plotChanged()),
            this, SLOT(fillTable()));

    QMap<PlotCurve::Units, QString> unitLabels;
    unitLabels.insert(PlotCurve::Band, "Band");
    unitLabels.insert(PlotCurve::Percentage, "Percentage");
    unitLabels.insert(PlotCurve::PixelNumber, "Pixel Number");
    unitLabels.insert(PlotCurve::CubeDN, "Pixel Value");
    unitLabels.insert(PlotCurve::Elevation, "Elevation");
    unitLabels.insert(PlotCurve::Wavelength, "Wavelength");

    plot()->setAxisTitle(QwtPlot::xBottom, unitLabels[xAxisUnits]);
    plot()->setAxisTitle(QwtPlot::yLeft,   unitLabels[yAxisUnits]);
    setPlotTitle(title);

    onClipboardChanged();

    readSettings();

#ifdef __APPLE__
    setWindowFlags(Qt::Tool);
#else
    setWindowFlags(Qt::Dialog);
#endif
  }



  PlotWindow::~PlotWindow() {
  }


  /**
   * This method is called by the constructor to create the
   * plot, legend. zoomer, and main window
   */
  void PlotWindow::createWidgets(MenuOptions optionsToProvide) {
    /*Create plot*/
    m_plot = new QwtPlot();
    m_plot->installEventFilter(this);
    m_plot->setAxisMaxMinor(QwtPlot::yLeft, 5);
    m_plot->setAxisMaxMajor(QwtPlot::xBottom, 30);
    m_plot->setAxisMaxMinor(QwtPlot::xBottom, 5);
    m_plot->setAxisLabelRotation(QwtPlot::xBottom, 45);
    m_plot->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignRight);

    /*Plot Legend*/
    QwtLegend *legend = new QwtLegend();
    legend->setItemMode(QwtLegend::ClickableItem);
    legend->setWhatsThis("Right Click on a legend item to display the context "
                         "menu.");
    m_plot->insertLegend(legend, QwtPlot::RightLegend, 1.0);
    legend->installEventFilter(this);

    /*Plot Grid*/
    m_grid = new QwtPlotGrid;
    m_grid->enableXMin(true);
    m_grid->setMajPen(QPen(Qt::white, 1, Qt::DotLine));
    m_grid->setMinPen(QPen(Qt::gray, 1, Qt::DotLine));
    m_grid->attach(m_plot);
    m_grid->setVisible(false);

    /*Plot Zoomer*/
    m_zoomer = new QwtPlotZoomer(m_plot->canvas());
    m_zoomer->setRubberBandPen(QPen(Qt::lightGray));
    m_zoomer->setTrackerPen(QPen(Qt::lightGray));

    setCentralWidget(m_plot);
    setupDefaultMenu(optionsToProvide);
  }


  /**
   * Shows the plot window, and raises it to the front of any
   * overlapping sibling widgets.
  */
  void PlotWindow::showWindow() {
    raise();
    show();
  }


  void PlotWindow::update(MdiCubeViewport *activeViewport) {
  }


  /**
   * Sets the plots given axis title to the given string.
   *
   *
   * @param axisId
   * @param title
   */
  void PlotWindow::setAxisLabel(int axisId, QString title) {
    m_plot->setAxisTitle(axisId, title);
  }


  /**
   * Returns the axis title of the given axis.
   *
   *
   * @param axisId
   *
   * @return QwtText
   */
//   QwtText PlotWindow::getAxisLabel(int axisId) {
//     return m_plot->axisTitle(axisId);
//   }


  /**
   * Sets the plot title to the given string.
   *
   *
   * @param pt
   */
  void PlotWindow::setPlotTitle(QString pt) {
    m_plot->setTitle(pt);
  }


  void PlotWindow::setUserCanAddCurves(bool userHasControl) {
    m_allowUserToAddCurves = userHasControl;
  }

  /**
   * Returns the plot title.
   *
   *
   * @return QwtText
   */
  QString PlotWindow::plotTitle() const {
    return m_plot->title().text();
  }


  bool PlotWindow::userCanAddCurves() const {
    return m_allowUserToAddCurves;
  }


  PlotCurve::Units PlotWindow::xAxisUnits() const {
    return m_xAxisUnits;
  }


  PlotCurve::Units PlotWindow::yAxisUnits() const {
    return m_yAxisUnits;
  }


  /**
   * Sets the plot background color to the given color.
   *
   *
   * @param c
   */
  void PlotWindow::setPlotBackground(QColor c) {
    m_plot->setCanvasBackground(c);
  }

  bool PlotWindow::canAdd(CubePlotCurve *curveToTest) const {
    return (curveToTest->xUnits() == m_xAxisUnits &&
            curveToTest->yUnits() == m_yAxisUnits);
  }


  /**
   * Returns the plot's background color.
   *
   *
   * @return QColor
   */
  QColor PlotWindow::plotBackgroundColor() const {
    return m_plot->canvasBackground();
  }


  QList<CubePlotCurve *> PlotWindow::plotCurves() {
    QList<CubePlotCurve *> foundCurves;

    const QwtPlotItemList &plotItems = m_plot->itemList();

    for (int itemIndex = plotItems.size()- 1; itemIndex >= 0; itemIndex --) {
      QwtPlotItem *item = plotItems[itemIndex];

      if (item->rtti() == QwtPlotItem::Rtti_PlotCurve) {
        CubePlotCurve *curve = dynamic_cast<CubePlotCurve *>(item);

        if (curve && curve->color().alpha() != 0)
          foundCurves.append(curve);
      }
    }

    return foundCurves;
  }


  QList<const CubePlotCurve *> PlotWindow::plotCurves() const {
    QList<const CubePlotCurve *> foundCurves;

    const QwtPlotItemList &plotItems = m_plot->itemList();

    for (int itemIndex = plotItems.size()- 1; itemIndex >= 0; itemIndex --) {
      const QwtPlotItem *item = plotItems[itemIndex];

      if (item->rtti() == QwtPlotItem::Rtti_PlotCurve) {
        const CubePlotCurve *curve = dynamic_cast<const CubePlotCurve *>(item);

        if (curve)
          foundCurves.append(curve);
      }
    }

    return foundCurves;
  }


  QList<QwtPlotSpectrogram *> PlotWindow::plotSpectrograms() {
    QList<QwtPlotSpectrogram *> foundSpectrograms;

    const QwtPlotItemList &plotItems = m_plot->itemList();

    for (int itemIndex = plotItems.size()- 1; itemIndex >= 0; itemIndex --) {
      QwtPlotItem *item = plotItems[itemIndex];

      if (item->rtti() == QwtPlotItem::Rtti_PlotSpectrogram) {
        QwtPlotSpectrogram *spectrogram =
            dynamic_cast<QwtPlotSpectrogram *>(item);

        if (spectrogram)
          foundSpectrograms.append(spectrogram);
      }
    }

    return foundSpectrograms;
  }


  QList<const QwtPlotSpectrogram *> PlotWindow::plotSpectrograms() const {
    QList<const QwtPlotSpectrogram *> foundSpectrograms;

    const QwtPlotItemList &plotItems = m_plot->itemList();

    for (int itemIndex = plotItems.size()- 1; itemIndex >= 0; itemIndex --) {
      const QwtPlotItem *item = plotItems[itemIndex];

      if (item->rtti() == QwtPlotItem::Rtti_PlotSpectrogram) {
        const QwtPlotSpectrogram *spectrogram =
            dynamic_cast<const QwtPlotSpectrogram *>(item);

        if (spectrogram)
          foundSpectrograms.append(spectrogram);
      }
    }

    return foundSpectrograms;
  }


  /**
   * This method adds the curves to the plot.
   *
   *
   * @param pc
   */
  void PlotWindow::add(CubePlotCurve *pc) {
    if (!canAdd(pc)) {
      QMessageBox::warning(NULL, "Failed to add plot curve",
          "Can not add plot curves with x/y units that do not match the plot's "
          "x/y units");
    }
    else {
      QString curveTitle = pc->title().text();

      bool titleAccepted = false;
      int titleTryCount = 0;
      while (!titleAccepted) {
        if (titleTryCount > 0) {
          curveTitle = pc->title().text() + " (" +
              QString::number(titleTryCount + 1) + ")";
        }

        titleTryCount++;
        titleAccepted = true;

        const QwtPlotItemList &plotItems = m_plot->itemList();

        for (int itemIndex = 0; itemIndex < plotItems.size(); itemIndex ++) {
          QwtPlotItem *item = plotItems[itemIndex];

          if (item->title().text() == curveTitle)
            titleAccepted = false;
        }
      }

      pc->setTitle(curveTitle);
      pc->attach(m_plot);
      pc->attachMarkers();
      fillTable();

      updateVisibility(pc);

      connect(pc, SIGNAL(needsRepaint()),
              this, SIGNAL(plotChanged()));
      connect(pc, SIGNAL(destroyed(QObject *)),
              this, SLOT(resetScale()));

      replot();
    }
  }


  /**
  * This method completely clears the plot of all plot items.
  * i.e. curves and markers, which also deletes the legend also
  * calls the necessary method to delete the table stuff
  */
  void PlotWindow::clearPlot() {
    clearPlotCurves();

    /*Table Stuff if table is open*/
    if (m_tableWindow != NULL && m_tableWindow->isVisible()) {
      m_tableWindow->table()->setColumnCount(1);
      m_tableWindow->table()->setRowCount(0);
//       deleteFromTable();
    }
  }


  void PlotWindow::createBestFitLine() {
    PlotWindowBestFitDialog *dialog = new PlotWindowBestFitDialog(this, plot());
    dialog->show();
  }


  /**
  * This method also clears the plot of all plot items, but does
  * not call the table delete stuff This method is called from
  * plotTool each time the changePlot() method is called.
  *
  *
  * @param keepScale
  */
  void PlotWindow::clearPlotCurves() {
    const QwtPlotItemList &plotItems = m_plot->itemList();

    for (int itemIndex = plotItems.size()- 1; itemIndex >= 0; itemIndex --) {
      QwtPlotItem *item = plotItems[itemIndex];

      if (item->rtti() == QwtPlotItem::Rtti_PlotCurve ||
          item->rtti() == QwtPlotItem::Rtti_PlotHistogram) {
        delete item;
      }
    }

    replot();
  }


  /**
   * Enables the plot mouse tracker.
   *
   */
  void PlotWindow::trackerEnabled() {
    if (m_zoomer->trackerMode() == QwtPicker::ActiveOnly) {
      m_zoomer->setTrackerMode(QwtPicker::AlwaysOn);
    }
    else {
      m_zoomer->setTrackerMode(QwtPicker::ActiveOnly);
    }
  }


  /**
   * Provids printing support of the plot image.
   *
   */
  void PlotWindow::printPlot() {
    QPixmap pixmap;
    /* Initialize a printer*/
    static QPrinter *printer = NULL;
    if (printer == NULL) printer = new QPrinter;
    printer->setPageSize(QPrinter::Letter);
    printer->setColorMode(QPrinter::Color);

    QPrintDialog printDialog(printer, (QWidget *)parent());

    if (printDialog.exec() == QDialog::Accepted) {
      /* Get display widget as a pixmap and convert to an image*/
      pixmap = QPixmap::grabWidget(m_plot);
      QImage img = pixmap.toImage();
      /* C++ Gui Programming with Qt, page 201*/
      QPainter painter(printer);
      QRect rect = painter.viewport();
      QSize size = img.size();
      size.scale(rect.size(), Qt::KeepAspectRatio);
      painter.setViewport(rect.x(), rect.y(),
                          size.width(), size.height());
      painter.setWindow(img.rect());
      painter.drawImage(0, 0, img);
    }

  }


  /**
   * This method allows the user to save the plot as a png,
   * jpg, or tif image file.
   */
  void PlotWindow::savePlot() {
    QPixmap pixmap;
    QString output =
    QFileDialog::getSaveFileName((QWidget *)parent(),
                                 "Choose output file",
                                 "./",
                                 QString("Images (*.png *.jpg *.tif)"));
    if (output.isEmpty()) return;
    //Make sure the filename is valid
    if (!output.isEmpty()) {
      if (!output.endsWith(".png") && !output.endsWith(".jpg") && !output.endsWith(".tif")) {
        output = output + ".png";
      }
    }

    QString format = QFileInfo(output).suffix();
    pixmap = QPixmap::grabWidget(m_plot);

    std::string formatString = format.toStdString();
    if (!pixmap.save(output, formatString.c_str())) {
      QMessageBox::information((QWidget *)parent(), "Error", "Unable to save " + output);
      return;
    }
  }


  /**
   * This method toggles the plot background color between
   * black and white.
   */
  void PlotWindow::switchBackground() {
    QPen *pen = new QPen(Qt::white);

    if (m_plot->canvasBackground() == Qt::white) {
      m_plot->setCanvasBackground(Qt::black);
      m_grid->setMajPen(QPen(Qt::white, 1, Qt::DotLine));
    }
    else {
      m_plot->setCanvasBackground(Qt::white);
      pen->setColor(Qt::black);
      m_grid->setMajPen(QPen(Qt::black, 1, Qt::DotLine));
    }

    m_zoomer->setRubberBandPen(*pen);
    m_zoomer->setTrackerPen(*pen);
    pen->setWidth(2);
    /*Replot with the new background and pen colors*/
    m_plot->replot();
  }


  /**
   * Sets plot scale back to the defaults.
   *
   */
  void PlotWindow::resetScale() {
    m_zoomer->zoom(0);

    if (m_autoscaleAxes) {
      if (m_xAxisUnits != PlotCurve::Band) {
        m_plot->setAxisAutoScale(QwtPlot::xBottom);
      }
      else {
        QPair<double, double> calculatedXRange = findDataRange(
            QwtPlot::xBottom);
        m_plot->setAxisScale(QwtPlot::xBottom, calculatedXRange.first,
                             calculatedXRange.second);
      }

      if (m_yAxisUnits != PlotCurve::Band) {
        m_plot->setAxisAutoScale(QwtPlot::yLeft);
      }
      else {
        QPair<double, double> calculatedYRange = findDataRange(
            QwtPlot::yLeft);
        m_plot->setAxisScale(QwtPlot::yLeft, calculatedYRange.first,
                             calculatedYRange.second);
      }
    }

    m_zoomer->setZoomBase();
    m_plot->replot();
  }


  /**
   * This method sets the scale for the axis according to the
   * user specified numbers.
   */
  void PlotWindow::setUserValues() {
    if (m_xLogCheckBox->isChecked()) {
      m_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine);
    }
    else {
      m_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
    }

    if (m_yLogCheckBox->isChecked()) {
      m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
    }
    else {
      m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
    }

    m_autoscaleAxes = m_autoScaleCheckBox->isChecked();

    if (!m_autoscaleAxes) {
      double xMin = m_xMinEdit->text().toDouble();
      double xMax = m_xMaxEdit->text().toDouble();
  //  QwtScaleDiv xAxisScale =
  //      m_plot->axisScaleEngine(QwtPlot::xBottom)->divideScale(xMin, xMax,
  //                                                             25, 100);
  //  m_plot->setAxisScaleDiv(QwtPlot::xBottom, xAxisScale);
      m_plot->setAxisScale(QwtPlot::xBottom, xMin, xMax);

      double yMin = m_yMinEdit->text().toDouble();
      double yMax = m_yMaxEdit->text().toDouble();
  //  QwtScaleDiv yAxisScale =
  //      m_plot->axisScaleEngine(QwtPlot::yLeft)->divideScale(yMin, yMax,
  //                                                             25, 100);
  //  m_plot->setAxisScaleDiv(QwtPlot::yLeft, yAxisScale);
      m_plot->setAxisScale(QwtPlot::yLeft, yMin, yMax);

      m_zoomer->setZoomBase();
    }

    replot();
  }


  /**
   * Resets the x/y min/max to the defaults.
   *
   */
  void PlotWindow::setDefaultRange() {
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Set Display Range");

    QGridLayout *dialogLayout = new QGridLayout;

    int row = 0;

    QLabel *autoLabel = new QLabel("Auto-Scale: ");
    dialogLayout->addWidget(autoLabel, row, 0);

    m_autoScaleCheckBox = new QCheckBox("Scale X/Y Axes Automatically");
    m_autoScaleCheckBox->setChecked(m_autoscaleAxes);
    connect(m_autoScaleCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(autoScaleCheckboxToggled()));
    dialogLayout->addWidget(m_autoScaleCheckBox, row, 1);
    row++;

    QLabel *xLabel = new QLabel("<h3>X-Axis</h3>");
    dialogLayout->addWidget(xLabel, row, 0, 1, 2);
    row++;

    QLabel *xMinLabel = new QLabel("Minimum: ");
    dialogLayout->addWidget(xMinLabel, row, 0);

    double xMin = plot()->axisScaleDiv(QwtPlot::xBottom)->lowerBound();
    m_xMinEdit = new QLineEdit(QString::number(xMin));
    dialogLayout->addWidget(m_xMinEdit, row, 1);
    row++;

    QLabel *xMaxLabel = new QLabel("Maximum: ");
    dialogLayout->addWidget(xMaxLabel, row, 0);

    double xMax = plot()->axisScaleDiv(QwtPlot::xBottom)->upperBound();
    m_xMaxEdit = new QLineEdit(QString::number(xMax));
    dialogLayout->addWidget(m_xMaxEdit, row, 1);
    row++;

    QLabel *xLogLabel = new QLabel("Logarithmic Scale");
    dialogLayout->addWidget(xLogLabel, row, 0);

    m_xLogCheckBox = new QCheckBox;
    m_xLogCheckBox->setChecked(
        m_plot->axisScaleEngine(QwtPlot::xBottom)->transformation()->type() ==
        QwtScaleTransformation::Log10);
    dialogLayout->addWidget(m_xLogCheckBox, row, 1);
    row++;

    QLabel *yLabel = new QLabel("<h3>Y-Axis</h3>");
    dialogLayout->addWidget(yLabel, row, 0, 1, 2);
    row++;

    QLabel *yMinLabel = new QLabel("Minimum: ");
    dialogLayout->addWidget(yMinLabel, row, 0);

    double yMin = plot()->axisScaleDiv(QwtPlot::yLeft)->lowerBound();
    m_yMinEdit = new QLineEdit(QString::number(yMin));
    dialogLayout->addWidget(m_yMinEdit, row, 1);
    row++;

    QLabel *yMaxLabel = new QLabel("Maximum: ");
    dialogLayout->addWidget(yMaxLabel, row, 0);

    double yMax = plot()->axisScaleDiv(QwtPlot::yLeft)->upperBound();
    m_yMaxEdit = new QLineEdit(QString::number(yMax));
    dialogLayout->addWidget(m_yMaxEdit, row, 1);
    row++;

    QLabel *yLogLabel = new QLabel("Logarithmic Scale");
    dialogLayout->addWidget(yLogLabel, row, 0);

    m_yLogCheckBox = new QCheckBox;
    m_yLogCheckBox->setChecked(
        m_plot->axisScaleEngine(QwtPlot::yLeft)->transformation()->type() ==
        QwtScaleTransformation::Log10);
    dialogLayout->addWidget(m_yLogCheckBox, row, 1);
    row++;

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();

    QPushButton *okButton = new QPushButton("&Ok");
    okButton->setIcon(QIcon::fromTheme("dialog-ok"));
    connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    connect(dialog, SIGNAL(accepted()), this, SLOT(setUserValues()));
    okButton->setShortcut(Qt::Key_Enter);
    buttonsLayout->addWidget(okButton);

    QPushButton *cancelButton = new QPushButton("&Cancel");
    cancelButton->setIcon(QIcon::fromTheme("dialog-cancel"));
    connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));
    buttonsLayout->addWidget(cancelButton);

    QWidget *buttonsWrapper = new QWidget;
    buttonsWrapper->setLayout(buttonsLayout);
    dialogLayout->addWidget(buttonsWrapper, row, 0, 1, 2);
    row++;

    autoScaleCheckboxToggled();

    dialog->setLayout(dialogLayout);
    dialog->show();
  }


  /**
   * This method creates the dialog box which allows the user
   * to relabel the plot window
   */
  void PlotWindow::changePlotLabels() {
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Name Plot Labels");

    QGridLayout *dialogLayout = new QGridLayout;

    int row = 0;
    QLabel *plotLabel = new QLabel("Plot Title: ");
    dialogLayout->addWidget(plotLabel, row, 0);

    m_plotTitleText = new QLineEdit(plot()->title().text());
    dialogLayout->addWidget(m_plotTitleText, row, 1);
    row++;

    QLabel *xAxisLabel = new QLabel("X-Axis Label: ");
    dialogLayout->addWidget(xAxisLabel, row, 0);

    m_xAxisText = new QLineEdit(m_plot->axisTitle(QwtPlot::xBottom).text());
    dialogLayout->addWidget(m_xAxisText, row, 1);
    row++;

    QLabel *yAxisLabel = new QLabel("Y-Axis Label: ");
    dialogLayout->addWidget(yAxisLabel, row, 0);

    m_yAxisText = new QLineEdit(m_plot->axisTitle(QwtPlot::yLeft).text());
    dialogLayout->addWidget(m_yAxisText, row, 1);
    row++;

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();

    QPushButton *okButton = new QPushButton("&Ok");
    okButton->setIcon(QIcon::fromTheme("dialog-ok"));
    connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    connect(dialog, SIGNAL(accepted()), this, SLOT(setLabels()));
    okButton->setShortcut(Qt::Key_Enter);
    buttonsLayout->addWidget(okButton);

    QPushButton *cancelButton = new QPushButton("&Cancel");
    cancelButton->setIcon(QIcon::fromTheme("dialog-cancel"));
    connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));
    buttonsLayout->addWidget(cancelButton);

    QWidget *buttonsWrapper = new QWidget;
    buttonsWrapper->setLayout(buttonsLayout);
    dialogLayout->addWidget(buttonsWrapper, row, 0, 1, 2);
    row++;

    dialog->setLayout(dialogLayout);
    dialog->show();
  }


  /**
   * Makes the user specified changes to the plot labels.
   */
  void PlotWindow::setLabels() {
    m_plot->setTitle(m_plotTitleText->text());
    m_plot->setAxisTitle(QwtPlot::xBottom, m_xAxisText->text());
    m_plot->setAxisTitle(QwtPlot::yLeft, m_yAxisText->text());
    /*Replot with new labels.*/
    m_plot->replot();
  }


  /**
   * This method hides/shows the grid on the plotWindow and
   * changes the text for the action
   */
  void PlotWindow::showHideGrid() {
    m_grid->setVisible(!m_grid->isVisible());

    if (m_grid->isVisible()) {
      m_showHideGrid->setText("Hide Grid");
    }
    else {
      m_showHideGrid->setText("Show Grid");
    }
    m_plot->replot();
  }


  /**
   *Shows/Hides all the markers(symbols)
   */
  void PlotWindow::showHideAllMarkers() {
    void (QwtPlotItem::*method)();
    if (m_showHideAllMarkers->text() == "Hide All Symbols") {
      method = &QwtPlotItem::hide;

      m_showHideAllMarkers->setText("Show All Symbols");

    }
    else {
      method = &QwtPlotItem::show;

      m_showHideAllMarkers->setText("Hide All Symbols");
    }

    for (int i = 0; i < m_plot->itemList().size(); i ++) {
      QwtPlotItem *plotItem = m_plot->itemList()[i];
      if (plotItem->rtti() == QwtPlotItem::Rtti_PlotMarker) 
        (plotItem->*method)();
    }
    /*Replot with all symbols hidden*/
    m_plot->replot();
  }


  /**
   * This method shows or hides all of the curves in the
   * plotWindow
   */
  void PlotWindow::showHideAllCurves() {
    void (QwtPlotItem::*method)();
    if (m_showHideAllCurves->text() == "Hide All Curves") {
      method = &QwtPlotItem::hide;

      m_showHideAllCurves->setText("Show All Curves");
      m_showHideAllCurves->setIcon(
          QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_showCurves.png"));

    }
    else {
      method = &QwtPlotItem::show;

      m_showHideAllCurves->setText("Hide All Curves");
      m_showHideAllCurves->setIcon(
          QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_hideCurves.png"));
    }

    for (int i = 0; i < m_plot->itemList().size(); i ++) {
      QwtPlotItem *plotItem = m_plot->itemList()[i];
      if (plotItem->rtti() == QwtPlotItem::Rtti_PlotCurve) 
        (plotItem->*method)();
    }
    /*Replot with all curves hidden*/
    m_plot->replot();
  }


  /**
   * This method creates and shows the help dialog box for the
   * plot window.  this is called from the Help-->Basic Help menu.
   */
  void PlotWindow::showHelp() {
    QDialog *d = new QDialog(m_plot);
    d->setWindowTitle("Basic Help");

    QLabel *zoomLabel = new QLabel("<U>Zoom Options:</U>");
    QLabel *zoomIn = new
                     QLabel("  <b>Left click</b> on the mouse, drag, and release to select an area to zoom in on");
    QLabel *zoomOut = new
                      QLabel("  <b>Middle click</b> on the mouse to zoom out one level");
    QLabel *zoomReset = new
                        QLabel("  <b>Right click</b> on the mouse and select <I>Reset  Scale</I> to clear the zoom and return to the original plot");

    QLabel *curveConfigLabel = new QLabel("<br><U>Curve Configuration:</U>");
    QLabel *configDirections = new
                               QLabel("  <b>To configure the curve properties</b>  Right click on the legend and select <I>Configure</I> from <br>  the menu"
                                      " or click on the configure icon in the tool bar.");
    QLabel *config = new QLabel();
    config->setPixmap(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_configure.png"));

    QLabel *tableLabel = new QLabel("<br><U>Table Options:</U>");
    QLabel *tableDirections = new
                              QLabel("  <b>To view the table</b> Click on the File menu and select <I>Show Table</I> or click on the table icon in the <br>   tool bar.");
    QLabel *table = new QLabel();
    table->setPixmap(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_table.png"));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(zoomLabel);
    layout->addWidget(zoomIn);
    layout->addWidget(zoomOut);
    layout->addWidget(zoomReset);
    layout->addWidget(curveConfigLabel);
    layout->addWidget(config);
    layout->addWidget(configDirections);
    layout->addWidget(tableLabel);
    layout->addWidget(table);
    layout->addWidget(tableDirections);

    d->setLayout(layout);
    d->show();
  }


  /**
  * The user can add menu items from parent classes, but there are
  * some menu items required (default) for the plotWindow
  */
  void PlotWindow::setupDefaultMenu(MenuOptions optionsToProvide) {
    QList<QMenu *> menu;
    QList<QAction *> actions;

    QMenu *fileMenu = new QMenu("&File");
    QMenu *editMenu = new QMenu("&Edit");
    QMenu *optionsMenu = new QMenu("&Options");
    QMenu *helpMenu = new QMenu("&Help");

    if ((optionsToProvide & SaveMenuOption) == SaveMenuOption) {
      QAction *save = new QAction(m_plot);
      save->setText("&Save Plot As");
      save->setIcon(QIcon::fromTheme("document-save-as"));
      QString text =
          "<b>Function:</b>  Save the plot as a png, jpg, or tif file.";
      save->setWhatsThis(text);
      connect(save, SIGNAL(activated()), this, SLOT(savePlot()));
      fileMenu->addAction(save);
      actions.push_back(save);
    }


    if ((optionsToProvide & PrintMenuOption) == PrintMenuOption) {
      QAction *prt = new QAction(m_plot);
      prt->setText("&Print Plot");
      prt->setIcon(QIcon::fromTheme("document-print"));
      QString text =
          "<b>Function:</b>  Sends the plot image to the printer";
      prt->setWhatsThis(text);
      connect(prt, SIGNAL(activated()), this, SLOT(printPlot()));
      fileMenu->addAction(prt);
      actions.push_back(prt);
    }

    if ((optionsToProvide & ShowTableMenuOption) == ShowTableMenuOption) {
      QAction *table = new QAction(m_plot);
      table->setText("Show Table");
      table->setIcon(
          QPixmap(Filename("$base/icons/plot_table.png").Expanded()));
      QString text =
          "<b>Function:</b>  Activates the table which displays the data of the "
                            "current plot";
      table->setWhatsThis(text);
      connect(table, SIGNAL(activated()), this, SLOT(showTable()));
      fileMenu->addAction(table);
      actions.push_back(table);
    }

    QAction *close = new QAction(QIcon::fromTheme("document-close"), "&Close",
                                 m_plot);
    connect(close, SIGNAL(activated()), this, SLOT(close()));
    fileMenu->addAction(close);

    if ((optionsToProvide & TrackMenuOption) == TrackMenuOption) {
      QAction *track = new QAction(m_plot);
      track->setText("Show Mouse &Tracking");
      track->setIcon(
          QPixmap(Filename("$base/icons/goto.png").Expanded()));
      track->setCheckable(true);
      QString text =
          "<b>Function:</b>  Displays the x,y coordinates as the cursor moves "
          "around on the plot.";
      track->setWhatsThis(text);
      connect(track, SIGNAL(activated()), this, SLOT(trackerEnabled()));
      optionsMenu->addAction(track);
    }

    if ((optionsToProvide & BackgroundSwitchMenuOption) ==
        BackgroundSwitchMenuOption) {
      QAction *backgrdSwitch = new QAction(m_plot);
      backgrdSwitch->setText("White/Black &Background");
      backgrdSwitch->setIcon(
          QPixmap(Filename("$base/icons/plot_switchBackgrd.png").Expanded()));
      QString text =
          "<b>Function:</b>  Switch the background color between black and "
                            "white.";
      backgrdSwitch->setWhatsThis(text);
      connect(backgrdSwitch, SIGNAL(activated()),
              this, SLOT(switchBackground()));
      optionsMenu->addAction(backgrdSwitch);
      actions.push_back(backgrdSwitch);
    }

    if ((optionsToProvide & ShowHideGridMenuOption) == ShowHideGridMenuOption) {
      m_showHideGrid = new QAction(m_plot);
      m_showHideGrid->setText("Show Grid");
      m_showHideGrid->setIcon(
          QPixmap(Filename("$base/icons/plot_grid.png").Expanded()));
      QString text =
          "<b>Function:</b>  Display grid lines on the plot.";
      m_showHideGrid->setWhatsThis(text);
      connect(m_showHideGrid, SIGNAL(activated()), this, SLOT(showHideGrid()));
      optionsMenu->addAction(m_showHideGrid);
      actions.push_back(m_showHideGrid);
    }

    if ((optionsToProvide & RenameLabelsMenuOption) == RenameLabelsMenuOption) {
      QAction *changeLabels = new QAction(m_plot);
      changeLabels->setText("Rename Plot &Labels");
      changeLabels->setIcon(
          QPixmap(Filename("$base/icons/plot_renameLabels.png").Expanded()));
      QString text =
          "<b>Function:</b>  Edit the plot title, x and y axis labels.";
      changeLabels->setWhatsThis(text);
      connect(changeLabels, SIGNAL(activated()),
              this, SLOT(changePlotLabels()));
      optionsMenu->addAction(changeLabels);
      actions.push_back(changeLabels);
    }

    if ((optionsToProvide & SetDisplayRangeMenuOption) ==
        SetDisplayRangeMenuOption) {
      QAction *changeScale = new QAction(m_plot);
      changeScale->setText("Set &Display Range");
      changeScale->setIcon(
          QPixmap(Filename("$base/icons/plot_setScale.png").Expanded()));
      QString text =
          "<b>Function:</b>  Adjust the scale for the x and y axis on the "
          "plot.";
      changeScale->setWhatsThis(text);
      connect(changeScale, SIGNAL(activated()), this, SLOT(setDefaultRange()));
      optionsMenu->addAction(changeScale);
      actions.push_back(changeScale);
    }

    if ((optionsToProvide & ShowHideCurvesMenuOption) ==
        ShowHideCurvesMenuOption) {
      m_showHideAllCurves = new QAction(m_plot);
      m_showHideAllCurves->setText("Hide All Curves");
      m_showHideAllCurves->setIcon(
          QPixmap(Filename("$base/icons/plot_showCurves.png").Expanded()));
      QString text =
          "<b>Function:</b>  Displays or hides all the curves currently "
                            "displayed on the plot.";
      m_showHideAllCurves->setWhatsThis(text);
      connect(m_showHideAllCurves, SIGNAL(activated()),
              this, SLOT(showHideAllCurves()));
      optionsMenu->addAction(m_showHideAllCurves);
      actions.push_back(m_showHideAllCurves);
    }

    if ((optionsToProvide & ShowHideMarkersMenuOption) ==
        ShowHideMarkersMenuOption) {
      m_showHideAllMarkers = new QAction(m_plot);
      m_showHideAllMarkers->setText("Hide All Symbols");
      m_showHideAllMarkers->setIcon(
          QPixmap(Filename("$base/icons/plot_markers.png").Expanded()));
      QString text = "<b>Function:</b>  Displays or hides a symbol for each "
                     "data point plotted on a plot.";
      m_showHideAllMarkers->setWhatsThis(text);
      connect(m_showHideAllMarkers, SIGNAL(activated()),
              this, SLOT(showHideAllMarkers()));
      optionsMenu->addAction(m_showHideAllMarkers);
      actions.push_back(m_showHideAllMarkers);
    }

    if ((optionsToProvide & ResetScaleMenuOption) == ResetScaleMenuOption) {
      QAction *resetScaleButton = new QAction(m_plot);
      resetScaleButton->setText("Reset Scale");
      resetScaleButton->setIcon(
          QPixmap(Filename("$base/icons/plot_resetscale.png").Expanded()));
      QString text =
          "<b>Function:</b>  Reset the plot's scale.";
      resetScaleButton->setWhatsThis(text);
      connect(resetScaleButton, SIGNAL(activated()), this, SLOT(resetScale()));
      actions.push_back(resetScaleButton);
    }

    if ((optionsToProvide & ClearPlotMenuOption) == ClearPlotMenuOption) {
      QAction *clear = new QAction(m_plot);
      clear->setText("Clear Plot");
      clear->setIcon(
          QPixmap(Filename("$base/icons/plot_clear.png").Expanded()));
      QString text =
          "<b>Function:</b>  Removes all the curves from the plot.";
      clear->setWhatsThis(text);
      connect(clear, SIGNAL(activated()), this, SLOT(clearPlot()));
      actions.push_back(clear);
    }

    if ((optionsToProvide & LineFitMenuOption) == LineFitMenuOption) {
      QAction *lineFit = new QAction(m_plot);
      lineFit->setText("Create Best Fit Line");
      lineFit->setIcon(
          QPixmap(Filename("$base/icons/linefit.png").Expanded()));
      QString text = "<b>Function:</b>  Calculates a best fit line from an "
                     "existing curve.";
      lineFit->setWhatsThis(text);
      connect(lineFit, SIGNAL(activated()), this, SLOT(createBestFitLine()));
      optionsMenu->addAction(lineFit);
      actions.push_back(lineFit);
    }

    /*setup menus*/
    m_pasteAct = new QAction(QIcon::fromTheme("edit-paste"),
                             "&Paste Curve", m_plot);
    m_pasteAct->setEnabled(false);
    m_pasteAct->setShortcut(Qt::CTRL | Qt::Key_V);
    connect(m_pasteAct, SIGNAL(triggered()),
            this, SLOT(pasteCurve()));
    editMenu->addAction(m_pasteAct);

    menu.push_back(fileMenu);
    menu.push_back(editMenu);

    if (optionsMenu->actions().size()) {
      menu.push_back(optionsMenu);
    }
    else {
      delete optionsMenu;
      optionsMenu = NULL;
    }

    if (helpMenu->actions().size()) {
      menu.push_back(helpMenu);
    }
    else {
      delete helpMenu;
      helpMenu = NULL;
    }

    setMenus(menu, actions);
  }


  bool PlotWindow::userCanAddCurve(const QMimeData *curve) {
    bool userCanAdd = false;

    if (m_allowUserToAddCurves &&
        curve->hasFormat("application/isis3-plot-curve")) {

      CubePlotCurve * testCurve = new CubePlotCurve(
          curve->data("application/isis3-plot-curve"));

      userCanAdd = canAdd(testCurve);
    }

    return userCanAdd;
  }


  /**
   * This method sets the visibility states in the curve (and it's symbols) to 
   *   match with this window's current visibility settings. Every means of
   *   adding a curve to the window needs to call this with the curve that is
   *   being added, otherwise visibility states will not be consistent.
   * 
   * @param curve Curve (with symbols) to set the visibility states on
   */
  void PlotWindow::updateVisibility(PlotCurve *curve) {
    if (m_showHideAllCurves) {
      if (m_showHideAllCurves->text() == "Hide All Curves") {
        curve->show();
      }
      else {
        curve->hide();
      }

      if (m_showHideAllMarkers->text() == "Hide All Symbols") {
        curve->setMarkerVisible(true);
      }
      else {
        curve->setMarkerVisible(false);
      }
    }

    emit plotChanged();
  }


  /**
   * Sets up the menus added from a parent object.
   *
   *
   * @param menu
   * @param actions
   */
  void PlotWindow::setMenus(QList<QMenu *> menu, QList<QAction *> actions) {
    if (m_toolBar == NULL) {
      m_toolBar = new QToolBar(this);
      m_toolBar->setObjectName("PlotWindow");
      m_toolBar->setAllowedAreas(Qt::LeftToolBarArea | Qt::RightToolBarArea | Qt::TopToolBarArea);
      addToolBar(Qt::TopToolBarArea, m_toolBar);
    }
    else {
      m_toolBar->clear();
    }

    m_menubar = menuBar();
    m_menubar->clear();

    for (int i = 0; i < menu.size(); i++) {
      m_menubar->addMenu(menu[i]);
    }

    for (int i = 0; i < actions.size(); i++) {
      m_toolBar->addAction(actions[i]);
    }

  }


  QwtPlotZoomer *PlotWindow::zoomer() {
    return m_zoomer;
  }


  /**
   * Fills in the table with the data from the current curves
   * in the plotWindow
   */
  void PlotWindow::fillTable() {
    if (m_tableWindow == NULL) return;
    m_tableWindow->listWidget()->clear();
    m_tableWindow->table()->clear();
    m_tableWindow->table()->setRowCount(0);
    m_tableWindow->table()->setColumnCount(0);

    m_tableWindow->addToTable(true,
                              m_plot->axisTitle(QwtPlot::xBottom).text(),
                              m_plot->axisTitle(QwtPlot::xBottom).text());

    QList<CubePlotCurve *> curves = plotCurves();
    foreach (CubePlotCurve *curve, curves) {
      m_tableWindow->addToTable(true,
                                curve->title().text(),
                                curve->title().text());
    }

    // We really need all of the x-values associated with the curves,
    //   but qwt doesn't seem to want to give this to us. It'll give us the
    //   axis scale, but that isn't quite what we want (especially when zooming)
    //   So let's find the list of x-points ourselves.
    //
    // This is what I tried and it did NOT work:
    // QwtScaleDiv *xAxisScaleDiv = m_plot->axisScaleDiv(QwtPlot::xBottom);
    // QList<double> xAxisPoints = xAxisScaleDiv->ticks(QwtScaleDiv::MajorTick);
    //
    // Also, QSet<double> does not work (Qt can't do it as of version 4.6).
    QList<double> xAxisPoints;

    foreach (CubePlotCurve *curve, curves) {
      // Loop backwards because our insertion sort will have a much better
      //   chance of success on it's first try this way.
      for (int dataIndex = (int)curve->data().size() - 1;
            dataIndex >= 0;
            dataIndex--) {
        double xValue = curve->data().x(dataIndex);

        // It turns out that qBinaryFind(container, value) is NOT the same as
        //   qBinaryFind(container.begin(), container.end(), value). Use the one
        //   that works right.
        QList<double>::const_iterator foundPos =
            qBinaryFind(xAxisPoints.begin(), xAxisPoints.end(), xValue);

        if (foundPos == xAxisPoints.end()) {
          bool inserted = false;
          for (int searchIndex = 0;
             searchIndex < xAxisPoints.size() && !inserted;
             searchIndex++) {
            if (xAxisPoints[searchIndex] > xValue) {
              inserted = true;
              xAxisPoints.insert(searchIndex, xValue);
            }
          }
          if (!inserted)
            xAxisPoints.prepend(xValue);
        }
      }
    }

    m_tableWindow->table()->setRowCount(xAxisPoints.size());

    for (int row = 0; row < m_tableWindow->table()->rowCount(); row++) {
      double xAxisValue = xAxisPoints[row];

      QTableWidgetItem *xAxisItem = new QTableWidgetItem(
          iString(xAxisValue).ToQt());
      m_tableWindow->table()->setItem(row, 0, xAxisItem);

      if (row == m_tableWindow->table()->rowCount() - 1) {
        m_tableWindow->table()->resizeColumnToContents(0);
      }

      // Now search for the x-axis points in the curves to fill in data
      for (int col = 1; col < m_tableWindow->table()->columnCount(); col++) {
        CubePlotCurve *curve = curves[col - 1];

        double y = Null;

        for (int dataIndex = 0;
             dataIndex < (int)curve->data().size() && y == Null;
             dataIndex++) {
          if (curve->data().x(dataIndex) == xAxisValue) {
            y = curve->data().y(dataIndex);
          }
        }

        QTableWidgetItem *item = NULL;

        if (IsSpecial(y))
          item = new QTableWidgetItem(QString("N/A"));
        else
          item = new QTableWidgetItem(iString(y).ToQt());

        m_tableWindow->table()->setItem(row, col, item);

        if (row == m_tableWindow->table()->rowCount() - 1) {
          m_tableWindow->table()->resizeColumnToContents(col);
        }
      }
    }
  }


  /**
  * This method is called from the showTable action on the tool
  * bar There are some checks done to make sure there are data to
  * fill the table
  */
  void PlotWindow::showTable() {
    if (plotCurves().size()) {
      if (m_tableWindow == NULL) {
        //m_tableWindow = new TableMainWindow("Plot Table", this);
        m_tableWindow = new TableMainWindow("Plot Table", m_parent);
        m_tableWindow->setTrackListItems(false);
      }

      fillTable();
      m_tableWindow->show();
      m_tableWindow->syncColumns();
    }
  }


  /**
   * This method filters the events of the objects it is connected
   * to.  In this case, the eventFilter has been installed on the m_plot and 
   * m_legend. 
   * @param o
   * @param e
   *
   * @return bool
   */
  bool PlotWindow::eventFilter(QObject *o, QEvent *e) {
    bool blockWidgetFromEvent = false;

    switch (e->type()) {
      case QEvent::Close:
        if (o == this || o == plot())
          writeSettings();
        break;

      case QEvent::MouseButtonPress:
        if (o == this &&
            childAt(((QMouseEvent *)e)->pos()) != plot()->canvas()) {
          mousePressEvent(o, (QMouseEvent *)e);
          blockWidgetFromEvent = true;
        }
        break;

      default:
        break;
    }

    bool stopHandlingEvent = false;
    if (!blockWidgetFromEvent && o == this) {
      stopHandlingEvent = MainWindow::eventFilter(o, e);

      if (e->type() == QEvent::Close && !stopHandlingEvent) {
        emit closed();
      }
      else if (e->type() == QEvent::Hide) {
        ASSERT(0);
      }
    }

    return stopHandlingEvent || blockWidgetFromEvent;
  }


  void PlotWindow::mousePressEvent(QObject *object, QMouseEvent *event) {
    if (qobject_cast<QWidget *>(object) &&
        event->button() == Qt::RightButton &&
        userCanAddCurve(QApplication::clipboard()->mimeData())) {
      QMenu contextMenu;

      QAction *pasteAct = new QAction(QIcon::fromTheme("edit-paste"), "Paste",
                                      this);
      contextMenu.addAction(pasteAct);

      QAction *chosenAct = contextMenu.exec(
          qobject_cast<QWidget *>(object)->mapToGlobal(event->pos()));

      if (chosenAct == pasteAct) {
        pasteCurve();
      }
    }
  }


  QwtPlot *PlotWindow::plot() {
    return m_plot;
  }


  /**
   * This overridden method is called from the constructor so that
   * when the mainwindow is created, it knows it's size
   * and location and the tool bar location.
   *
   */
  void PlotWindow::readSettings() {
    /*Call the base class function to read the size and location*/
    MainWindow::readSettings();
    QString appName = QCoreApplication::applicationName();
    /*Now read the settings that are specific to this window.*/
    QString instanceName = windowTitle();
    Isis::Filename config(
        iString("$HOME/.Isis/" + appName + "/" + instanceName + ".config"));

    QSettings settings(QString::fromStdString(config.Expanded()), QSettings::NativeFormat);
    QByteArray state = settings.value("state", QByteArray("0")).toByteArray();
    restoreState(state);
  }


  void PlotWindow::disableAxisAutoScale() {
    m_autoscaleAxes = false;
  }


  void PlotWindow::autoScaleCheckboxToggled() {
    m_xMinEdit->setEnabled(!m_autoScaleCheckBox->isChecked());
    m_xMaxEdit->setEnabled(!m_autoScaleCheckBox->isChecked());
    m_yMinEdit->setEnabled(!m_autoScaleCheckBox->isChecked());
    m_yMaxEdit->setEnabled(!m_autoScaleCheckBox->isChecked());
  }


  /**
   * This slot will be called when the system clipboard is changed.
   */
  void PlotWindow::onClipboardChanged() {
    m_pasteAct->setEnabled(
        userCanAddCurve(QApplication::clipboard()->mimeData()));
  }


  void PlotWindow::pasteCurve() {
    if (m_allowUserToAddCurves) {
      QClipboard *globalClipboard = QApplication::clipboard();
      const QMimeData *globalData = globalClipboard->mimeData();

      if (globalData->hasFormat("application/isis3-plot-curve")) {
        CubePlotCurve * newCurve = new CubePlotCurve(
                globalData->data("application/isis3-plot-curve"));
        // add curve to plot
        add(newCurve);
        emit plotChanged();
      }
    }
  }


  QPair<double, double> PlotWindow::findDataRange(int axisId) const {
    QList<const CubePlotCurve *> curves = plotCurves();

    bool foundDataValue = false;
    QPair<double, double> rangeMinMax;

    foreach(const CubePlotCurve *curve, curves) {
      for (int dataIndex = 0; dataIndex < curve->dataSize(); dataIndex++) {
        if (axisId == QwtPlot::xBottom) {
          if (!foundDataValue) {
            rangeMinMax.first = curve->x(dataIndex);
            rangeMinMax.second = curve->x(dataIndex);
            foundDataValue = true;
          }
          else {
            rangeMinMax.first = qMin(rangeMinMax.first, curve->x(dataIndex));
            rangeMinMax.second = qMax(rangeMinMax.second, curve->x(dataIndex));
          }
        }
        else if (axisId == QwtPlot::yLeft) {
          if (!foundDataValue) {
            rangeMinMax.first = curve->y(dataIndex);
            rangeMinMax.second = curve->y(dataIndex);
            foundDataValue = true;
          }
          else {
            rangeMinMax.first = qMin(rangeMinMax.first, curve->y(dataIndex));
            rangeMinMax.second = qMax(rangeMinMax.second, curve->y(dataIndex));
          }
        }
      }
    }

    if (!foundDataValue) {
      rangeMinMax.first = 1;
      rangeMinMax.second = 10;
    }
    else if(rangeMinMax.first == rangeMinMax.second) {
      rangeMinMax.first -= 0.5;
      rangeMinMax.second += 0.5;
    }

    return rangeMinMax;
  }


  /**
   * This overridden method is called when the mainwindow
   * is closed or hidden to write the size and location settings
   * (and tool bar location) to a config file in the user's home
   * directory.
   *
   */
  void PlotWindow::writeSettings() {
    /*Call the base class function to write the size and location*/
    MainWindow::writeSettings();
    std::string appName = QCoreApplication::applicationName().toStdString();
    /*Now write the settings that are specific to this window.*/
    std::string instanceName = windowTitle().toStdString();
    Isis::Filename config("$HOME/.Isis/" + appName + "/" + instanceName + ".config");

    QSettings settings(QString::fromStdString(config.Expanded()), QSettings::NativeFormat);
    settings.setValue("state", saveState());
  }


  void PlotWindow::paint(MdiCubeViewport *vp, QPainter *painter) {
    foreach (CubePlotCurve *curve, plotCurves()) {
      curve->paint(vp, painter);
    }
  }


  /**
   * replot the plot
   */
  void PlotWindow::replot() {
    resetScale();
    emit plotChanged();
  }



  QString PlotWindow::defaultWindowTitle() {
    return "Plot";
  }


  void PlotWindow::dragEnterEvent(QDragEnterEvent *event) {
    QWidget *source = event->source();

    if (source != m_plot->legend()->contentsWidget() &&
        userCanAddCurve(event->mimeData())) {
      event->acceptProposedAction();
    }
  }


  void PlotWindow::dropEvent(QDropEvent *event) {
    if (m_allowUserToAddCurves &&
        event->mimeData()->hasFormat("application/isis3-plot-curve")) {
      Qt::DropActions possibleActions = event->possibleActions();
      Qt::DropAction actionToTake = event->proposedAction();

      QFont boldFont;
      boldFont.setBold(true);
      QMenu dropActionsMenu;

      QAction *copyAct = new QAction("&Copy Here", this);
      if (possibleActions.testFlag(Qt::CopyAction)) {
        dropActionsMenu.addAction(copyAct);

        if (actionToTake == Qt::CopyAction)
          copyAct->setFont(boldFont);
      }

      QAction *moveAct = new QAction("&Move Here", this);
      if (possibleActions.testFlag(Qt::MoveAction)) {
        dropActionsMenu.addAction(moveAct);

        if (actionToTake == Qt::MoveAction)
          moveAct->setFont(boldFont);
      }

      if (dropActionsMenu.actions().size() > 1) {
        dropActionsMenu.addSeparator();

        QAction *cancelAct = new QAction("&Cancel", this);
        dropActionsMenu.addAction(cancelAct);

        QAction *chosenAct = dropActionsMenu.exec(mapToGlobal(event->pos()));

        if (chosenAct == copyAct) {
          actionToTake = Qt::CopyAction;
        }
        else if (chosenAct == moveAct) {
          actionToTake = Qt::MoveAction;
        }
        else {
          actionToTake = Qt::IgnoreAction;
        }
      }

      if (actionToTake != Qt::IgnoreAction) {
        CubePlotCurve * newCurve = new CubePlotCurve(
                event->mimeData()->data("application/isis3-plot-curve"));
        // add curve to plot
        add(newCurve);
        emit plotChanged();

        event->setDropAction(actionToTake);
        event->accept();
      }
    }
  }

}

