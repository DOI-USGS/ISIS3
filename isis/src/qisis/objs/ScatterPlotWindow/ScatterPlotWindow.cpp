#include "ScatterPlotWindow.h"

#include <QVector>

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>


#include "MdiCubeViewport.h"
#include "ScatterPlotData.h"
#include "ScatterPlotTool.h"
#include "ViewportMainWindow.h"
#include "Workspace.h"

namespace Qisis {


  /**
   * ScatterPlotToolWindow constructor.
   *
   *
   * @param title
   * @param tool
   * @param parent
   */
  ScatterPlotWindow::ScatterPlotWindow(QString title, ScatterPlotTool *tool, QWidget *parent): Qisis::MainWindow(title, parent) {
    p_parent = parent;
    p_tool = tool;

    p_scatterPlotWindow = new MainWindow(title);
    p_scatterPlotWindow->setFixedSize(QSize(700, 700));
    p_plot = new QwtPlot();
    p_plot->plotLayout()->setAlignCanvasToScales(true);
    p_zoomer = new MyZoomer(p_plot->canvas());

    p_scatterPlotWindow->setCentralWidget(p_plot);

    setupMenus();
    createDialogs();
  }


  /**
   * This method creates all the dialog boxes required for the
   * scatter plot window.  Called from the ScatterPlotWindow
   * constructor.
   *
   */
  void ScatterPlotWindow::createDialogs() {
    QDialogButtonBox *configButtonBox;
    QLabel *label;
    QLabel *label2;
    QLabel *label3;

    p_configDialog = new QDialog();
    p_configDialog->setWindowTitle("Setup Scatter Plot");
    p_configDialog->setModal(true);

    configButtonBox = new QDialogButtonBox(p_configDialog);
    configButtonBox->setGeometry(QRect(30, 200, 341, 32));
    configButtonBox->setOrientation(Qt::Horizontal);
    configButtonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::NoButton | QDialogButtonBox::Ok);

    p_cube1Label = new QLabel(p_configDialog);
    p_cube1Label->setText("Cube 1:");
    p_cube1Label->setGeometry(QRect(10, 70, 75, 27));

    p_cube1BandComboBox = new QComboBox(p_configDialog);
    p_cube1BandComboBox->setGeometry(QRect(250, 70, 60, 22));

    p_numBinsOne = new QLineEdit(p_configDialog);
    p_numBinsOne->setGeometry(QRect(320, 70, 40, 22));
    p_numBinsOne->setText("255");

    p_numBinsTwo = new QLineEdit(p_configDialog);
    p_numBinsTwo->setGeometry(QRect(320, 130, 40, 22));
    p_numBinsTwo->setText("255");

    p_cube2BandComboBox = new QComboBox(p_configDialog);
    p_cube2BandComboBox->setGeometry(QRect(250, 130, 60, 22));

    p_cube1ComboBox = new QComboBox(p_configDialog);
    p_cube1ComboBox->setGeometry(QRect(55, 70, 181, 25));
    QObject::connect(p_cube1ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(fillBands()));

    p_cube2ComboBox = new QComboBox(p_configDialog);
    p_cube2ComboBox->setGeometry(QRect(55, 130, 181, 25));
    QObject::connect(p_cube2ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(fillBands()));

    p_cube2Label = new QLabel(p_configDialog);
    p_cube2Label->setText("Cube 2:");
    p_cube2Label->setGeometry(QRect(10, 130, 75, 27));

    label = new QLabel(p_configDialog);
    label->setObjectName(QString::fromUtf8("label"));
    label->setText("Band");
    label->setGeometry(QRect(260, 40, 81, 20));

    label2 = new QLabel(p_configDialog);
    label2->setObjectName(QString::fromUtf8("label2"));
    label2->setText("Select 2 Cubes (may be the same cube.)");
    label2->setGeometry(QRect(10, 40, 230, 20));

    label3 = new QLabel(p_configDialog);
    label3->setText("# Bins");
    label3->setGeometry(QRect(320, 40, 70, 20));


    QObject::connect(configButtonBox, SIGNAL(accepted()), this, SLOT(showScatterPlot()));
    QObject::connect(configButtonBox, SIGNAL(accepted()), p_configDialog, SLOT(accept()));
    QObject::connect(configButtonBox, SIGNAL(rejected()), this, SLOT(cancel()));

    // -------- End of p_configDialog

    QDialogButtonBox *minMaxButtonBox;
    QLabel *label_5;
    QLabel *label_4;
    QLabel *label_6;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_0;
    p_minMaxDialog = new QDialog();

    p_minMaxDialog->resize(283, 300);
    minMaxButtonBox = new QDialogButtonBox(p_minMaxDialog);
    minMaxButtonBox->setGeometry(QRect(20, 250, 211, 32));
    minMaxButtonBox->setOrientation(Qt::Horizontal);
    minMaxButtonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::NoButton | QDialogButtonBox::Ok);

    p_yMaxEdit = new QLineEdit(p_minMaxDialog);
    p_yMaxEdit->setGeometry(QRect(100, 200, 113, 25));
    p_yMaxEdit->setText(QString::number(p_MaxTwo));
    p_yMinEdit = new QLineEdit(p_minMaxDialog);
    p_yMinEdit->setGeometry(QRect(100, 160, 113, 25));
    p_yMinEdit->setText(QString::number(p_MinTwo));
    p_xMaxEdit = new QLineEdit(p_minMaxDialog);
    p_xMaxEdit->setGeometry(QRect(100, 90, 113, 25));
    p_xMaxEdit->setText(QString::number(p_MaxOne));
    p_xMinEdit = new QLineEdit(p_minMaxDialog);
    p_xMinEdit->setGeometry(QRect(100, 50, 113, 25));
    p_xMinEdit->setText(QString::number(p_MinOne));
    label_5 = new QLabel(p_minMaxDialog);
    label_5->setGeometry(QRect(25, 170, 56, 17));
    label_4 = new QLabel(p_minMaxDialog);
    label_4->setGeometry(QRect(120, 130, 56, 17));

    label_6 = new QLabel(p_minMaxDialog);
    label_6->setGeometry(QRect(25, 210, 56, 17));
    label_2 = new QLabel(p_minMaxDialog);
    label_2->setGeometry(QRect(25, 60, 56, 17));
    label_3 = new QLabel(p_minMaxDialog);
    label_3->setGeometry(QRect(25, 100, 56, 17));
    label_0 = new QLabel(p_minMaxDialog);
    label_0->setGeometry(QRect(120, 20, 56, 17));

    p_minMaxDialog->setWindowTitle("Set Display Range");
    label_5->setText("Minimum");
    label_6->setText("Maximum");
    label_2->setText("Minimum");
    label_3->setText("Maximum");
    label_0->setText("X-Axis");
    label_4->setText("Y-Axis");

    QObject::connect(minMaxButtonBox, SIGNAL(accepted()), this, SLOT(setUserValues()));
    QObject::connect(minMaxButtonBox, SIGNAL(accepted()), p_minMaxDialog, SLOT(accept()));
    QObject::connect(minMaxButtonBox, SIGNAL(rejected()), p_minMaxDialog, SLOT(reject()));

    // -------- End of p_minMaxDialog
  }


  /**
   * Displays the p_configDialog box.
   *
   */
  void ScatterPlotWindow::showConfig() {
    // ----------------------------------------------------
    // Populate the combo box with the filenames currently
    // open in Qview
    // ----------------------------------------------------
    QVector< MdiCubeViewport * > * cubeList = ((ViewportMainWindow *)(p_parent))->workspace()->cubeViewportList();
    for(int i = 0; i < cubeList->size(); i++) {
      std::string cubeFilename = cubeList->at(i)->cube()->Filename();
      QString str = QFileInfo(cubeFilename.c_str()).fileName();
      // ---------------------------------------------------------
      // Make sure we are not adding the same text more than once.
      // ---------------------------------------------------------
      if(p_cube1ComboBox->findText(str) == -1) {
        p_cube1ComboBox->addItem(str);
      }
    }

    cubeList = ((ViewportMainWindow *)(p_parent))->workspace()->cubeViewportList();
    for(int i = 0; i < cubeList->size(); i++) {
      std::string cubeFilename = cubeList->at(i)->cube()->Filename();
      QString str = QFileInfo(cubeFilename.c_str()).fileName();
      // ---------------------------------------------------------
      // Make sure we are not adding the same text more than once.
      // ---------------------------------------------------------
      if(p_cube2ComboBox->findText(str) == -1) {
        p_cube2ComboBox->addItem(str);
      }
    }

    fillBands();

    // ------------------------------------------------
    // Make sure the band combo boxes are already filled
    // with the last bands the user selected
    // -----------------------------------------------
    if(p_band1 > 0) {
      p_cube1BandComboBox->setCurrentIndex(p_band1 - 1);
      p_cube2BandComboBox->setCurrentIndex(p_band2 - 1);
    }

    p_configDialog->show();
  }


  /**
   * Fills the p_cubeXComboBox with the correct number of bands
   * based on the selected cube.
   *
   */
  void ScatterPlotWindow::fillBands() {
    int numBands1 = 0;
    int numBands2 = 0;
    QVector< MdiCubeViewport * > * cubeList = ((ViewportMainWindow *)(p_parent))->workspace()->cubeViewportList();
    for(int i = 0; i < cubeList->size(); i++) {
      std::string cubeFilename = cubeList->at(i)->cube()->Filename();
      QString str = QFileInfo(cubeFilename.c_str()).fileName();
      if(str.compare(p_cube1ComboBox->currentText()) == 0) {
        numBands1 = cubeList->at(i)->cubeBands();
      }
      if(str.compare(p_cube2ComboBox->currentText()) == 0) {
        numBands2 = cubeList->at(i)->cubeBands();
      }
    }
    p_cube1BandComboBox->clear();
    for(int j = 0; j < numBands1; j++) {
      p_cube1BandComboBox->addItem(QString::number(j + 1));
    }

    p_cube2BandComboBox->clear();
    for(int j = 0; j < numBands2; j++) {
      p_cube2BandComboBox->addItem(QString::number(j + 1));
    }
  }


  /**
   * Get the cubes from the config dialog box and figures out
   * which viewportmainwindow the cube is associated with and then
   * creates the ScatterPlotData and the QwtSpectrogram and
   * attaches it to the plot.  Once the plot is configured the
   * scatter plot window is shown.
   *
   */
  void ScatterPlotWindow::showScatterPlot() {
    QString cube1 = p_cube1ComboBox->currentText();
    QString cube2 = p_cube2ComboBox->currentText();
    if(cube1 == "" || cube2 == "") {
      p_tool->setActionChecked(false);
      return;
    }

    p_plot->setTitle(cube1 + " VS " + cube2);

    //------------------------------------------------------
    // Now we need the viewportmainwindow associated with
    // each cube.
    //------------------------------------------------------
    MdiCubeViewport *cube1Viewport = NULL;
    MdiCubeViewport *cube2Viewport = NULL;
    QVector< MdiCubeViewport * > * cubeList = ((ViewportMainWindow *)(p_parent))->workspace()->cubeViewportList();
    for(int i = 0; i < cubeList->size(); i++) {
      std::string cubeFilename = cubeList->at(i)->cube()->Filename();
      QString str = QFileInfo(cubeFilename.c_str()).fileName();
      if(str.compare(cube1) == 0) {
        cube1Viewport = cubeList->at(i);
      }
      if(str.compare(cube2) == 0) {
        cube2Viewport = cubeList->at(i);
      }
    }

    // -----------------------------------------------
    // Check to make sure the two cubes have the same
    // number of lines and samples
    // -----------------------------------------------
    double ssamp1, esamp1, sline1, eline1;
    double ssamp2, esamp2, sline2, eline2;
    cube1Viewport->viewportToCube(0, 0, ssamp1, sline1);
    cube1Viewport->viewportToCube(cube1Viewport->viewport()->width() - 1,
                                  cube1Viewport->viewport()->height() - 1,
                                  esamp1, eline1);

    cube2Viewport->viewportToCube(0, 0, ssamp2, sline2);
    cube2Viewport->viewportToCube(cube2Viewport->viewport()->width() - 1,
                                  cube2Viewport->viewport()->height() - 1,
                                  esamp2, eline2);

    if((int)(esamp1 - ssamp1) != (int)(esamp2 - ssamp2) || (int)(eline1 - sline1) != (int)(eline2 - sline2)) {
      QMessageBox::critical(p_configDialog, "Size Issue", "The visible area of the cubes must be the same size!", QMessageBox::Ok);
      p_tool->setActionChecked(false);
      return;
    }

    p_configDialog->setCursor(Qt::WaitCursor);

    //----------------------------------------------
    // Get the band the user selected for each cube
    // to call with the ScatterPlotData constructor
    //---------------------------------------------
    p_band1 = p_cube1BandComboBox->currentIndex() + 1;
    p_band2 = p_cube2BandComboBox->currentIndex() + 1;

    //------------------------------------------------------------
    // Instantiate the QwtPlotSpectrogram and the ScatterPlotData
    // then attach to the plot.
    //----------------------------------------------------------
    p_spectrogram = new QwtPlotSpectrogram();
    int numbins1 = p_numBinsOne->text().toInt();
    int numbins2 = p_numBinsTwo->text().toInt();
    ScatterPlotData *data = new ScatterPlotData(cube1Viewport, p_band1, numbins1, cube2Viewport, p_band2, numbins2);
    p_spectrogram->setData(*data);
    p_spectrogram->attach(p_plot);

    QwtValueList contourLevels;
    QwtDoubleInterval range = data->range();

    // -----------------------------------------------------
    // Setup the contour levels for the contour lines
    // on the spectrogram.
    // ------------------------------------------------------
    for(double level = 0.5; level < range.maxValue(); level += (range.maxValue() / 6))
      contourLevels += level;
    p_spectrogram->setContourLevels(contourLevels);

    if(p_colorize->text().compare("Colorize") == 0) {
      QwtLinearColorMap colorMap(Qt::black, Qt::white);
      p_spectrogram->setColorMap(colorMap);
    }
    else {
      QwtLinearColorMap colorMap(Qt::darkCyan, Qt::red);
      colorMap.addColorStop(0.05, Qt::cyan);
      colorMap.addColorStop(0.3, Qt::green);
      colorMap.addColorStop(0.50, Qt::yellow);
      p_spectrogram->setColorMap(colorMap);
    }

    // -------------------------------------------
    // Setup a color bar on the right axis
    // using the color map created above.
    // -------------------------------------------
    p_rightAxis = p_plot->axisWidget(QwtPlot::yRight);
    p_rightAxis->setTitle("Counts");
    p_rightAxis->setColorBarEnabled(true);
    p_rightAxis->setColorMap(p_spectrogram->data().range(),
                             p_spectrogram->colorMap());

    p_plot->setAxisScale(QwtPlot::yRight,
                         p_spectrogram->data().range().minValue(),
                         p_spectrogram->data().range().maxValue());
    p_plot->enableAxis(QwtPlot::yRight);

    // ----------------------------------------------------------------
    // Setup the plots min/max and both axes to be the min/max for the
    // data associated with those axes.
    // Also set the axes titles to the cube name and which band on that
    // cube.
    // -----------------------------------------------------------------
    p_MinOne = data->minOne();
    p_MaxOne = data->maxOne();
    p_MinTwo = data->minTwo();
    p_MaxTwo = data->maxTwo();
    p_plot->setAxisScale(QwtPlot::yLeft, p_MinTwo, p_MaxTwo);
    p_plot->setAxisScale(QwtPlot::xBottom, p_MinOne, p_MaxOne);
    p_plot->setAxisTitle(QwtPlot::xBottom, cube1 + "   Band " + QString::number(p_band1));
    p_plot->setAxisTitle(QwtPlot::yLeft, cube2 + "   Band " + QString::number(p_band2));
    p_plot->replot();
    p_zoomer->setZoomBase();

    p_scatterPlotWindow->show();
    p_configDialog->setCursor(Qt::ArrowCursor);
    p_tool->setActionChecked(false);
  }

  /**
   * Called when user clicks the cancel button on the
   * p_configdialog box.
   *
   */
  void ScatterPlotWindow::cancel() {
    p_configDialog->hide();
    p_tool->setActionChecked(false);
  }


  /**
   * Set up the menus for the ScatterPlotWindow.
   * Called from the constructor.
   */
  void ScatterPlotWindow::setupMenus() {
    p_menubar = p_scatterPlotWindow->menuBar();
    p_toolBar = new QToolBar(p_scatterPlotWindow);

    p_scatterPlotWindow->addToolBar(Qt::TopToolBarArea, p_toolBar);

    QAction *fitLine = new QAction(p_plot);
    fitLine->setText("Line Fit");
    fitLine->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/linefit.png"));
    QObject::connect(fitLine, SIGNAL(activated()), this, SLOT(showContour()));

    p_colorize = new QAction(p_plot);
    p_colorize->setText("Colorize");
    p_colorize->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/rgb.png"));
    QString text  =
      "Colorize";
    p_colorize->setWhatsThis(text);
    QObject::connect(p_colorize, SIGNAL(activated()), this, SLOT(colorPlot()));

    QAction *save = new QAction(p_plot);
    save->setText("&Save Plot As");
    save->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/filesaveas.png"));
    text  =
      "<b>Function:</b>  Save the plot as a png, jpg, or tif file.";
    save->setWhatsThis(text);
    QObject::connect(save, SIGNAL(activated()), this, SLOT(savePlot()));

    QAction *prt = new QAction(p_plot);
    prt->setText("&Print Plot");
    prt->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/fileprint.png"));
    text  =
      "<b>Function:</b>  Sends the plot image to the printer";
    prt->setWhatsThis(text);
    QObject::connect(prt, SIGNAL(activated()), this, SLOT(printPlot()));

    QAction *track = new QAction(p_plot);
    track->setText("Show Mouse &Tracking");
    track->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/goto.png"));
    track->setCheckable(true);
    text  =
      "<b>Function:</b>  Displays the x,y coordinates as the cursor moves \
      around on the plot.";
    track->setWhatsThis(text);
    QObject::connect(track, SIGNAL(activated()), this, SLOT(trackerEnabled()));

    QAction *changeLabels = new QAction(p_plot);
    changeLabels->setText("Rename Plot &Labels");
    changeLabels->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_renameLabels.png"));
    text  =
      "<b>Function:</b>  Edit the plot title, x and y axis labels.";
    changeLabels->setWhatsThis(text);
    QObject::connect(changeLabels, SIGNAL(activated()), this, SLOT(reLabel()));

    QAction *changeScale = new QAction(p_plot);
    changeScale->setText("Set &Display Range");
    changeScale->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_setScale.png"));
    text  =
      "<b>Function:</b>  Adjust the scale for the x and y axis on the plot.";
    changeScale->setWhatsThis(text);
    QObject::connect(changeScale, SIGNAL(activated()), this, SLOT(setDisplayRange()));

    QAction *resetScaleButton = new QAction(p_plot);
    resetScaleButton->setText("Reset Scale");
    resetScaleButton->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_resetscale.png"));
    text  =
      "<b>Function:</b>  Reset the plot's scale.";
    resetScaleButton->setWhatsThis(text);
    QObject::connect(resetScaleButton, SIGNAL(activated()), this, SLOT(resetScale()));

    QAction *close = new QAction(p_plot);
    close->setText("Close");
    QObject::connect(close, SIGNAL(activated()), p_scatterPlotWindow, SLOT(close()));

    /*setup menus*/
    QMenu *options = new QMenu("&Options");
    options->addAction(track);
    options->addAction(changeLabels);
    options->addAction(changeScale);

    QMenu *file = new QMenu("&File");
    file->addAction(save);
    file->addAction(prt);
    file->addAction(close);

    p_menubar->addMenu(file);
    p_menubar->addMenu(options);

    p_toolBar->addAction(track);
    p_toolBar->addAction(changeLabels);
    p_toolBar->addAction(changeScale);
    p_toolBar->addAction(p_colorize);
    p_toolBar->addAction(fitLine);
  }


  /**
   * This method allows the user to save the plot as a png,
   * jpg, or tif image file.
   */
  void ScatterPlotWindow::savePlot() {
    QPixmap pixmap;
    QString output =
      QFileDialog::getSaveFileName((QWidget *)parent(),
                                   "Choose output file",
                                   "./",
                                   QString("Images (*.png *.jpg *.tif)"));
    if(output.isEmpty()) return;
    //Make sure the filename is valid
    if(!output.isEmpty()) {
      if(!output.endsWith(".png") && !output.endsWith(".jpg") && !output.endsWith(".tif")) {
        output = output + ".png";
      }
    }

    QString format = QFileInfo(output).suffix();
    pixmap = QPixmap::grabWidget(p_plot);

    std::string formatString = format.toStdString();
    if(!pixmap.save(output, formatString.c_str())) {
      QMessageBox::information((QWidget *)parent(), "Error", "Unable to save " + output);
      return;
    }
  }


  /**
   * Provides printing support of the plot image.
   *
   */
  void ScatterPlotWindow::printPlot() {
    QPixmap pixmap;
    /* Initialize a printer*/
    static QPrinter *printer = NULL;
    if(printer == NULL) printer = new QPrinter;
    printer->setPageSize(QPrinter::Letter);
    printer->setColorMode(QPrinter::Color);

    QPrintDialog printDialog(printer, (QWidget *)parent());

    if(printDialog.exec() == QDialog::Accepted) {
      /* Get display widget as a pixmap and convert to an image*/
      pixmap = QPixmap::grabWidget(p_plot);
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
   * Sets plot scale back to the defaults.
   *
   */
  void ScatterPlotWindow::resetScale() {
    setScale(QwtPlot::xBottom, p_MinOne, p_MaxOne);
    setScale(QwtPlot::yLeft, p_MinTwo, p_MaxTwo);
  }


  /**
   * This method sets the scale of the axis on the plot
   *
   *
   * @param axisId
   * @param minimum
   * @param maximum
   */
  void ScatterPlotWindow::setScale(int axisId, double minimum, double maximum, double stepSize) {
    if(axisId == QwtPlot::xBottom) {
      p_MaxOne = maximum;
      p_MinOne = minimum;
    }

    if(axisId == QwtPlot::yLeft) {
      p_MaxTwo = maximum;
      p_MinTwo = minimum;
    }

    p_plot->setAxisScale(axisId, minimum, maximum, stepSize);
    p_plot->replot();
    p_zoomer->setZoomBase();
    p_scaled = true;
  }


  /**
   * Creates and brings up the dialog box which allows the user to
   * re-label the plot various labes.
   *
   */
  void ScatterPlotWindow::reLabel() {
    QDialog *dialog = new QDialog(p_scatterPlotWindow);
    dialog->setWindowTitle("Name Plot Labels");

    QWidget *buttons = new QWidget(dialog);
    QWidget *textAreas = new QWidget(dialog);
    QWidget *labels = new QWidget(dialog);
    QWidget *main = new QWidget(dialog);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(main, 0);
    layout->addWidget(buttons, 0);
    dialog->setLayout(layout);

    QToolButton *okButton = new QToolButton(dialog);
    connect(okButton, SIGNAL(released()), this, SLOT(setLabels()));
    connect(okButton, SIGNAL(released()), dialog, SLOT(hide()));
    okButton->setShortcut(Qt::Key_Enter);
    okButton->setText("Ok");

    QToolButton *cancelButton = new QToolButton(dialog);
    connect(cancelButton, SIGNAL(released()), dialog, SLOT(hide()));
    cancelButton->setText("Cancel");

    QLabel *plotLabel = new QLabel("Plot Title: ");
    QLabel *xAxisLabel = new QLabel("X-Axis Label: ");
    QLabel *yAxisLabel = new QLabel("Y-Axis Label: ");

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->addWidget(plotLabel);
    vlayout->addWidget(xAxisLabel);
    vlayout->addWidget(yAxisLabel);
    labels->setLayout(vlayout);

    p_plotTitleText = new QLineEdit(p_plot->title().text(), dialog);
    p_xAxisText = new QLineEdit(p_plot->axisTitle(QwtPlot::xBottom).text(), dialog);
    p_yAxisText = new QLineEdit(p_plot->axisTitle(QwtPlot::yLeft).text(), dialog);

    QVBoxLayout *v2layout = new QVBoxLayout();
    v2layout->addWidget(p_plotTitleText);
    v2layout->addWidget(p_xAxisText);
    v2layout->addWidget(p_yAxisText);
    textAreas->setLayout(v2layout);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(labels);
    mainLayout->addWidget(textAreas);
    main->setLayout(mainLayout);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(okButton);
    hlayout->addWidget(cancelButton);
    buttons->setLayout(hlayout);

    dialog->setFixedSize(400, 190);
    dialog->show();
  }


  /**
   * This method actually sets the plot's labels to the user
   * specified labels.
   *
   */
  void ScatterPlotWindow::setLabels() {
    p_plot->setTitle(p_plotTitleText->text());
    p_plot->setAxisTitle(QwtPlot::xBottom, p_xAxisText->text());
    p_plot->setAxisTitle(QwtPlot::yLeft, p_yAxisText->text());
    /*Replot with new labels.*/
    p_plot->replot();
  }


  /**
   * This method switches the color mode of the scatter plot from
   * black and white to color and visa versa.
   *
   */
  void ScatterPlotWindow::colorPlot() {
    if(p_colorize->text().compare("Colorize") == 0) {
      p_colorize->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/gray.png"));
      p_colorize->setText("Gray");
      QwtLinearColorMap colorMap(Qt::darkCyan, Qt::red);
      colorMap.addColorStop(0.05, Qt::cyan);
      colorMap.addColorStop(0.3, Qt::green);
      colorMap.addColorStop(0.50, Qt::yellow);
      p_spectrogram->setColorMap(colorMap);

    }
    else {
      p_colorize->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/rgb.png"));
      p_colorize->setText("Colorize");
      QwtLinearColorMap colorMap(Qt::black, Qt::white);
      p_spectrogram->setColorMap(colorMap);
    }

    p_rightAxis->setColorMap(p_spectrogram->data().range(),
                             p_spectrogram->colorMap());

    p_plot->replot();
  }


  /**
   * Enables x,y tracking on the plot canvas.
   *
   */
  void ScatterPlotWindow::trackerEnabled() {
    if(p_zoomer->trackerMode() == QwtPicker::ActiveOnly) {
      p_zoomer->setTrackerMode(QwtPicker::AlwaysOn);
    }
    else {
      p_zoomer->setTrackerMode(QwtPicker::ActiveOnly);
    }
  }


  /**
   * Sets the line edit boxes in the p_minMaxDialog to the current
   * x/y min/max then shows the dialog box.
   *
   */
  void ScatterPlotWindow::setDisplayRange() {
    p_yMaxEdit->setText(QString::number(p_MaxTwo));
    p_yMinEdit->setText(QString::number(p_MinTwo));
    p_xMaxEdit->setText(QString::number(p_MaxOne));
    p_xMinEdit->setText(QString::number(p_MinOne));
    p_minMaxDialog->show();
  }


  /**
   * This method sets the scale for the axis according to the
   * user specified numbers.
   */
  void ScatterPlotWindow::setUserValues() {
    p_MinOne = p_xMinEdit->text().toDouble();
    p_MaxOne = p_xMaxEdit->text().toDouble();
    p_MinTwo = p_yMinEdit->text().toDouble();
    p_MaxTwo = p_yMaxEdit->text().toDouble();
    setScale(QwtPlot::xBottom, p_MinOne, p_MaxOne);
    setScale(QwtPlot::yLeft, p_MinTwo, p_MaxTwo);
  }


  /**
   * This method hides or displays the contour lines on the
   * spectrogram.
   *
   */
  void ScatterPlotWindow::showContour() {
    if(p_colorize->text() == "Gray") {
      p_spectrogram->setDefaultContourPen(QPen());
    }
    else {
      p_spectrogram->setDefaultContourPen(QPen(QColor("white")));
    }

    p_spectrogram->setDisplayMode
    (QwtPlotSpectrogram::ContourMode,
     !p_spectrogram->testDisplayMode(QwtPlotSpectrogram::ContourMode));
    p_plot->replot();
  }


}

