#include "PlotWindow.h"
#include "MainWindow.h"
#include "Stretch.h"
#include "Filename.h"
#include "QHistogram.h"
#include "Interpolator.h"
#include <qwt_symbol.h>
#include <qwt_scale_engine.h>
#include <algorithm>

namespace Qisis {
  /*Static variables*/
  PlotCurve *PlotWindow::p_dragCurve = NULL;
  bool PlotWindow::p_curveCopied = false;

  /**
   * This constructs a plot window. The plot window graphs any
   * curve sent to it via the addPlotCurve() method.
   * 
   * 
   * @param title 
   * @param parent 
   */
  PlotWindow::PlotWindow (QString title,QWidget *parent) : Qisis::MainWindow(title, parent) {
    p_selected = -1;
    p_toolBar = NULL;
    p_menubar = NULL;
    p_tableWindow = NULL;
    p_parent = parent;
    createWindow();
    createConfigDialog();
    createLegendMenu();
    p_scaled = false; 
    p_curvePropsSaved = false;
    p_xLogScale = false;
    p_yLogScale = false;
    p_plotCurves.clear();
    setWindowTitle(title);
    readSettings();
  }


/**
 * This method is called by the constructor to create the
 * plot, legend. zoomer, and main window
 */
  void PlotWindow::createWindow() {
    /*Create plot*/
    p_plot = new QwtPlot();
    p_plot->installEventFilter(this);
    p_plot->setAxisMaxMinor(QwtPlot::yLeft, 5);
    p_plot->setAxisMaxMajor(QwtPlot::xBottom, 30);
    p_plot->setAxisMaxMinor(QwtPlot::xBottom, 5);
    p_plot->setAxisLabelRotation(QwtPlot::xBottom, 45);
    p_plot->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignRight);
   
    /*Plot Legend*/
    p_legend = new QwtLegend();
    p_legend->setItemMode(QwtLegend::ClickableItem);
    p_legend->setWhatsThis("Right Click on a legend item to display the context menu.");
    p_plot->insertLegend(p_legend, QwtPlot::RightLegend, 1.0);

    /*Plot Grid*/
    p_grid = new QwtPlotGrid;
    p_grid->enableXMin(true);
    p_grid->setMajPen(QPen(Qt::white, 1, Qt::DotLine));
    p_grid->setMinPen(QPen(Qt::gray, 1, Qt::DotLine));
    p_grid->attach(p_plot);
    p_grid->setVisible(false);

    /*Plot Zoomer*/
    p_zoomer = new QwtPlotZoomer(p_plot->canvas());
    p_zoomer->setRubberBandPen(QPen(Qt::lightGray));
    p_zoomer->setTrackerPen(QPen(Qt::lightGray));
    
    /* Create the Plot window*/
    p_mainWindow = new Qisis::MainWindow(this->windowTitle(), p_parent);
    p_mainWindow->installEventFilter(this);
    p_mainWindow->setCentralWidget(p_plot);
    setupDefaultMenu();
    
    /*set max and min*/
    p_xMin = 0;
    p_yMin = 0;
    p_xMax = 1000;
    p_yMax = 1000;
  }


 /**
  * Shows the plot window, and raises it to the front of any
  * overlapping sibling widgets.
 */
  void PlotWindow::showWindow(){
    p_mainWindow->raise();
    p_mainWindow->show();
  }

  /**
   * Sets the plot to auto scale the given axis.
   * 
   * 
   * @param axisId 
   */
  void PlotWindow::setAutoScaleAxis(int axisId){
    p_plot->setAxisAutoScale(axisId);
  }


  /**
   * Sets the plots given axis title to the given string.
   * 
   * 
   * @param axisId 
   * @param title 
   */
  void PlotWindow::setAxisLabel(int axisId, QString title){
    p_plot->setAxisTitle(axisId, title);
  }


  /**
   * Returns the axis title of the given axis.
   * 
   * 
   * @param axisId 
   * 
   * @return QwtText 
   */
  QwtText PlotWindow::getAxisLabel(int axisId){
   return p_plot->axisTitle(axisId); 
 }


  /**
   * Sets the plot title to the given string.
   * 
   * 
   * @param pt 
   */
  void PlotWindow::setPlotTitle(QString pt){
    p_plot->setTitle(pt);
  }


  /**
   * Returns the plot title.
   * 
   * 
   * @return QwtText 
   */
  QwtText PlotWindow::getPlotTitle(){
    return p_plot->title();
  }


  /**
   * Sets the plot background color to the given color.
   * 
   * 
   * @param c 
   */
  void PlotWindow::setPlotBackground(QColor c){
    p_plot->setCanvasBackground(c);
  }


  /**
   * Returns the plot's background color.
   * 
   * 
   * @return QColor 
   */
  QColor PlotWindow::getPlotBackground(){
    return p_plot->canvasBackground();
  }


  /**
   * This method adds the curves to the plot.
   * 
   * 
   * @param pc 
   */
  void PlotWindow::add(PlotCurve *pc){ 
    p_plotCurves.push_back(pc);
    pc->attach(p_plot);
    fillTable();  
    pc->attachSymbols(p_plot);
    //check to see if the curve is visible.
    if(pc->isVisible()) {
      pc->setVisible(true);
    } else {
      pc->setVisible(false);
    }
    //check to see if the symbols are visible.
    if(pc->isSymbolVisible()) {
      pc->setSymbolVisible(true);
    } else {
      pc->setSymbolVisible(false);
    }
    p_plot->replot();
    /*The zoomer base needs to reset after the replot in order for the y-axis 
    scale to adjust after a zoom action*/
    p_zoomer->setZoomBase();
     /*Installing the event filter is what enables the user to cut and paste 
     curves from one plot window to another*/
    p_legend->find(pc)->installEventFilter(this);
  }


  /**
  * This method completely clears the plot of all plot items.
  * i.e. curves and markers, which also deletes the legend also
  * calls the necessary method to delete the table stuff
  */
  void PlotWindow::clearPlot() {
 
    for (int i = 0; i < p_plotCurves.size(); i++) {
      p_plotCurves[i]->detach();
    }
    /*Need to clear all the qlists associated with the plot items.*/
    p_plotCurves.clear();
    p_plot->replot();
    
    /*Table Stuff if table is open*/
    if(p_tableWindow != NULL && p_tableWindow->isVisible()) {
      p_tableWindow->table()->setColumnCount(1);
      p_tableWindow->table()->setRowCount(0);
      deleteFromTable();
    }

    emit plotChanged();
  }


  /**
  * This method also clears the plot of all plot items, but does
  * not call the table delete stuff This method is called from
  * plotTool each time the changePlot() method is called.
  * 
  * 
  * @param keepScale 
  */
  void PlotWindow::clearPlotCurves(bool keepScale) {

    //clearPlot();
    for (int i = 0; i<p_plotCurves.size(); i++) {
      p_plotCurves[i]->detach();
    }

    /*Need to clear all the qlists associated with the plot items.*/
    p_plotCurves.clear();
    p_plot->replot();

    if(!keepScale) {
      setScale(QwtPlot::xBottom, 0, 10);
      setScale(QwtPlot::yLeft, 0, 10);
    }
  }


  /**
   * Enables the plot mouse tracker.
   * 
   */
  void PlotWindow::trackerEnabled() {
    if (p_zoomer->trackerMode() == QwtPicker::ActiveOnly) {
      p_zoomer->setTrackerMode(QwtPicker::AlwaysOn);
    } else {
      p_zoomer->setTrackerMode(QwtPicker::ActiveOnly);
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
 
    QPrintDialog printDialog(printer,(QWidget *)parent());

    if (printDialog.exec() == QDialog::Accepted) {
      /* Get display widget as a pixmap and convert to an image*/
      pixmap = QPixmap::grabWidget(p_plot);
      QImage img = pixmap.toImage();
      /* C++ Gui Programming with Qt, page 201*/
      QPainter painter(printer);
      QRect rect = painter.viewport();
      QSize size = img.size();
      size.scale(rect.size(),Qt::KeepAspectRatio);
      painter.setViewport(rect.x(),rect.y(),
                          size.width(),size.height());
      painter.setWindow(img.rect());
      painter.drawImage(0,0,img);
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
      if(!output.endsWith(".png") && !output.endsWith(".jpg") && !output.endsWith(".tif")) {
        output = output + ".png";
      } 
    }

    QString format = QFileInfo(output).suffix();  
    pixmap = QPixmap::grabWidget(p_plot);
    
    std::string formatString = format.toStdString();
    if (!pixmap.save(output,formatString.c_str())) {
      QMessageBox::information((QWidget *)parent(),"Error","Unable to save "+ output);
      return;
    }
  }


  /**
   * This method toggles the plot background color between
   * black and white.
   */
  void PlotWindow::switchBackground() {
    QPen *pen = new QPen(Qt::white);

    if (p_plot->canvasBackground() == Qt::white) {
      p_plot->setCanvasBackground(Qt::black);
      p_grid->setMajPen(QPen(Qt::white, 1, Qt::DotLine));
    } else {
      p_plot->setCanvasBackground(Qt::white);
      pen->setColor(Qt::black);
      p_grid->setMajPen(QPen(Qt::black, 1, Qt::DotLine));
    }

    p_zoomer->setRubberBandPen(*pen);
    p_zoomer->setTrackerPen(*pen);
    pen->setWidth(2);
    /*Replot with the new background and pen colors*/
    p_plot->replot();
  }


  /**
   * Sets plot scale back to the defaults. 
   * 
   */
  void PlotWindow::resetScale(){
    setScale(QwtPlot::xBottom, p_xMin, p_xMax);
    setScale(QwtPlot::yLeft, p_yMin, p_yMax);
  }


  /**
   * This method sets the scale of the axis on the plot
   * 
   * 
   * @param axisId 
   * @param minimum 
   * @param maximum 
   */
  void PlotWindow::setScale(int axisId, double minimum, double maximum, double stepSize) {
    if(axisId == QwtPlot::xBottom){
      p_xMax = maximum;
      p_xMin = minimum;
    }

    if(axisId == QwtPlot::yLeft){
      p_yMax = maximum;
      p_yMin = minimum;
    }

    p_plot->setAxisScale(axisId,minimum,maximum, stepSize);

    p_plot->replot();
    p_zoomer->setZoomBase();
    p_scaled = true;
  }

  /**
   * 
   * 
   * 
   * @param axisId 
   * @param scaleDiv 
   */
  void PlotWindow::setScaleDiv(int axisId, QwtScaleDiv scaleDiv){
    p_plot->setAxisScaleDiv(axisId, scaleDiv);
    p_plot->replot();
  }


  /**
   * This method sets the scale for the axis according to the
   * user specified numbers.
   */
  void PlotWindow::setUserValues() {
    p_xMin = p_xMinEdit->text().toDouble();
    p_xMax = p_xMaxEdit->text().toDouble();
    p_yMin = p_yMinEdit->text().toDouble();
    p_yMax = p_yMaxEdit->text().toDouble();
    setScale(QwtPlot::xBottom, p_xMin, p_xMax);
    setScale(QwtPlot::yLeft, p_yMin, p_yMax);

    if(p_xLogCheckBox->isChecked()) {
      p_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLog10ScaleEngine);
      p_xLogScale = true;
    } else {
      p_plot->setAxisScaleEngine(QwtPlot::xBottom, new QwtLinearScaleEngine);
      p_xLogScale = false;
    }

    if(p_yLogCheckBox->isChecked()) {
      p_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
      p_yLogScale = true;
    } else {
      p_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
      p_yLogScale = false;
    }

  }


  /** 
   * This method is called when the user activates the curve title
   * drop down menu.  When a curve is selected, then the rest of 
   * the dialog box needs to be filled in with the information 
   * specific to that curve  
   * 
   * 
   * @param title 
   */
  void PlotWindow::fillInValues(int title){
    p_selected = title;

    if(p_selected > p_plotCurves.size() || p_selected < 0){
      return;
    }

    /*save dragCurve with all the properties before the change.*/
    p_dragCurve = new PlotCurve();
    p_dragCurve->copyCurveProperties(p_plotCurves[p_selected]);

    p_titleBox->setCurrentIndex(p_selected);
    p_titleLineEdit->setText(p_plotCurves[p_selected]->title().text());
    p_styleBox->setCurrentIndex(p_plotCurves[p_selected]->pen().style()-1);
    p_sizeBox->setCurrentIndex(p_plotCurves[p_selected]->pen().width());
    QPalette *palette = new QPalette();
    palette->setColor(QPalette::Button,p_plotCurves[p_selected]->pen().color());
    p_colorButton->setPalette(*palette);
    p_symbolBox->setCurrentIndex(p_plotCurves[p_selected]->symbolStyle().style());

    if(p_plotCurves[p_selected]->isVisible()) p_hideButton->setText("Hide Curve");
    if(!p_plotCurves[p_selected]->isVisible()) p_hideButton->setText("Show Curve");

    if(p_plotCurves[p_selected]->p_markerIsVisible) p_symbolsButton->setText("Hide Symbols");
    if(!p_plotCurves[p_selected]->p_markerIsVisible) p_symbolsButton->setText("Show Symbols");

 }



  /**
   * Method called from the configDialog box when the user
   * changes the line edit box for the curve title
   * 
   * 
   * @param s 
   */
  void PlotWindow::changeTitle(QString s){
    p_plotCurves[p_selected]->setTitle(p_titleLineEdit->text());
    p_legend->find(p_plotCurves[p_selected])->updateGeometry();
    p_plot->updateLayout();

    /*We need to change the title on the table.*/
    if(p_tableWindow != NULL && p_tableWindow->isVisible()) 
      p_tableWindow->table()->horizontalHeaderItem(p_selected)->setText(p_titleLineEdit->text());

    if(p_tableWindow != NULL && p_tableWindow->isVisible()) 
      p_tableWindow->listWidget()->item(p_selected)->setText(p_titleLineEdit->text());
  }


  /**
  *  Each time the user makes a change to the curve (and/or it's
  *  respective markers) we need to emit a signal for programmers
  *  who are using this class know when something has been changed
  *  in the plot window.
  */
  void PlotWindow::saveProperties(){
    emit plotChanged();

  }


 /**
  *  This is called from the constructor.  It creates the dialog
  * box and all the widgets in it but the dialog box is not show
  * until the user selects the config Plot action
  */
 void PlotWindow::createConfigDialog(){
    configDialog = new QDialog(p_mainWindow);
    configDialog->setWindowTitle("Configure Plot Curves");
    configDialog->setModal(true);

    QWidget *labels = new QWidget(configDialog);
    QWidget *textBoxes = new QWidget(configDialog);
    QWidget *buttons = new QWidget(configDialog);
    QWidget *mainButtons = new QWidget(configDialog);
    QWidget *main = new QWidget(configDialog);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(main);
    layout->addWidget(buttons);

    configDialog->setLayout(layout);

    p_titleBox = new QComboBox(configDialog);
    p_titleBox->setDuplicatesEnabled(true);
    connect(p_titleBox,SIGNAL(activated(int)),this,SLOT(fillInValues(int)));

    QVBoxLayout *labelLayout = new QVBoxLayout();
    QLabel *selectLabel = new QLabel("Select Curve: ");
    QLabel *titleLabel = new QLabel("Title: ");
    QLabel *colorLabel = new QLabel("Color: ");
    QLabel *styleLabel = new QLabel("Style:" );
    QLabel *sizeLabel = new QLabel("Size:" );
    QLabel *symbolLabel = new QLabel("Symbol:" );
    labelLayout->addWidget(selectLabel);
    labelLayout->addWidget(titleLabel);
    labelLayout->addWidget(colorLabel);
    labelLayout->addWidget(styleLabel);
    labelLayout->addWidget(sizeLabel);
    labelLayout->addWidget(symbolLabel);

    labels->setLayout(labelLayout);

    QVBoxLayout *textBoxLayout = new QVBoxLayout();
    p_titleLineEdit = new QLineEdit(configDialog);
    connect(p_titleLineEdit,SIGNAL(textChanged(QString)),this,SLOT(changeTitle(QString)));
    p_colorButton = new QPushButton(configDialog);
    connect(p_colorButton,SIGNAL(clicked()),this,SLOT(colorSelect()));
    p_colorButton->setFixedWidth(25);
    p_styleBox = new QComboBox(configDialog);
    connect(p_styleBox,SIGNAL(activated(int)),this,SLOT(changeCurveStyle(int)));
    p_styleBox->addItem("SolidLine");
    p_styleBox->addItem("DashLine");
    p_styleBox->addItem("DotLine");
    p_styleBox->addItem("DashDotLine");
    p_styleBox->addItem("DashDotDot Line");
    p_symbolBox = new QComboBox(configDialog);
    connect(p_symbolBox,SIGNAL(activated(int)),this,SLOT(changeMarkerStyle(int)));
    p_symbolBox->addItem("Ellipse");
    p_symbolBox->addItem("Rect");
    p_symbolBox->addItem("Diamond");
    p_symbolBox->addItem("Triangle");
    p_symbolBox->addItem("DTriangle");
    p_symbolBox->addItem("UTriangle");
    p_symbolBox->addItem("LTriangle");
    p_symbolBox->addItem("RTriangle");
    p_symbolBox->addItem("Cross");
    p_symbolBox->addItem("XCross");
    p_symbolBox->addItem("HLine");
    p_symbolBox->addItem("VLine");
    p_symbolBox->addItem("Star1");
    p_symbolBox->addItem("Star2");
    p_symbolBox->addItem("Hexagon");
    p_sizeBox = new QComboBox(configDialog);
    connect(p_sizeBox,SIGNAL(activated(int)),this,SLOT(changeCurveSize(int)));
    p_sizeBox->addItem("1");
    p_sizeBox->addItem("2");
    p_sizeBox->addItem("3");
    p_sizeBox->addItem("4");

    textBoxLayout->addWidget(p_titleBox);
    textBoxLayout->addWidget(p_titleLineEdit);
    textBoxLayout->addWidget(p_colorButton);
    textBoxLayout->addWidget(p_styleBox);
    textBoxLayout->addWidget(p_sizeBox);
    textBoxLayout->addWidget(p_symbolBox);

    textBoxes->setLayout(textBoxLayout);

    QVBoxLayout *mainButtonsLayout = new QVBoxLayout();
    p_deleteButton = new QPushButton(configDialog);
    p_deleteButton->setText("Delete Curve");
    connect(p_deleteButton,SIGNAL(clicked()),this,SLOT(deleteCurve()));
    mainButtonsLayout->addWidget(p_deleteButton);
    if (!p_deletable) p_deleteButton->setVisible(false);

    p_hideButton = new QPushButton(configDialog);
    p_hideButton->setText("Show Curve");
    connect(p_hideButton,SIGNAL(clicked()),this,SLOT(showCurve()));
    mainButtonsLayout->addWidget(p_hideButton);

    p_symbolsButton = new QPushButton(configDialog);
    p_symbolsButton->setText("Hide Symbols");
    connect(p_symbolsButton,SIGNAL(clicked()),this,SLOT(showSymbols()));
    mainButtonsLayout->addWidget(p_symbolsButton);

    mainButtons->setLayout(mainButtonsLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(labels);
    mainLayout->addWidget(textBoxes);
    mainLayout->addWidget(mainButtons);

    main->setLayout(mainLayout);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton(configDialog);
    connect(okButton,SIGNAL(clicked()),this,SLOT(saveProperties()));
    connect(okButton,SIGNAL(clicked()),configDialog,SLOT(hide()));
    okButton->setText("OK");
    QPushButton *cancelButton = new QPushButton(configDialog);
    connect(cancelButton,SIGNAL(clicked()),this,SLOT(cancelConfig()));
    cancelButton->setText("Cancel");
   
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);

    buttons->setLayout(buttonsLayout);

  }


 /**
 *  This method is called when the user selects the config plot
 *  action on the tool bar or the option menu
 */
  void PlotWindow::configPlot(){  

    if (p_plotCurves.size() < 1) return;

    p_curveTitleList.clear();
    p_titleBox->clear();

    /*add all the current curves to the drop down menu*/
    for (int i = 0; i<p_plotCurves.size(); i++) {
      p_curveTitleList.push_back(p_plotCurves[i]->title().text());
    }

    p_titleBox->addItems(p_curveTitleList);

    /*fill in the dialog box's values according to which curve is selected.*/
    if (p_selected >=  0) {
      fillInValues(p_selected);
    } else {
      fillInValues(0);
    }

    /*check to see if this plotWindow allows the user to delete curves.*/
    if (!p_deletable) {
      p_deleteButton->setVisible(false);
    } else {
      p_deleteButton->setVisible(true);
    }

    /*now show the dialog box.*/
    configDialog->show();
  }



   /**
    * This method allows the programmer to set a property of the
    * plot window which determines if a window will allow the
    * pasting of curves.  If paste is false, the paste option on
    * the context menu is disabled.
    * 
    * 
    * @param paste 
    */
 void PlotWindow::setPasteable(bool paste){
   p_pasteable= paste;
 }


 /**
  * This method allows a programmer to set a property of the plot
  *  window which determines whether the user is allowed to delete
  *  a curve from the plotWindow or not. If set to false, the
  *  delete button is not visible on the configDialog box
  *  and the delete option on the context menu is disabled.
  * 
  * 
  * @param d 
  */
 void PlotWindow::setDeletable(bool d){
   p_deletable = d;
 }


 /**
  * This method allows the programmer to set the property which
  * allows the user to copy a curve from the window.  If c is
  * false, the copy option on the context menu is disabled. 
  *  
  * @param c
  */
 void PlotWindow::setCopyEnable(bool c){
   p_copyable = c;
 }


 /**
  * This method allows the programmer set the property to destroy
  * the window upon a close event.  When this is set to true, the
  * class will emit a destroyed(this) signal.  See the
  * eventFilter method. p_destroyOnClose is set to false by
  *  
  * default.
  * @param destroy
  */
 void PlotWindow::setDestroyOnClose(bool destroy){
   p_destroyOnClose = destroy;
 }


 /**
 *  When the user deletes a curve, there is a table column and a
 *  QList of markers associated with the that curve that also need
 * to be deleted.
 */
  void PlotWindow::deleteCurve(){
    p_plotCurves[p_selected]->detach();   
    delete p_plotCurves[p_selected];
    p_curveTitleList.removeAt(p_selected);
    p_plotCurves.removeAt(p_selected);


    /*Replot with the p_selected curve deleted.*/
    p_plot->replot();

    /*deletes the column from the table if the table has been created.*/
    if (p_tableWindow != NULL) deleteFromTable();
    configDialog->hide();
  }


 /**
 * This method is called when user presses the p_colorButton on
 * the config plot dialog the color of the button itself is then
 * set to the selected color for visual feedback
 */
  void PlotWindow::colorSelect(){
    QColor c = QColorDialog::getColor(Qt::white,configDialog);

    if (c.isValid()) {
      QPalette *palette = new QPalette();
      palette->setColor(QPalette::Button, c);
      p_colorButton->setPalette(*palette);      
      QPen *pen = new QPen(c);
      pen->setWidth(p_plotCurves[p_selected]->pen().width());
      pen->setStyle(p_plotCurves[p_selected]->pen().style());
      p_plotCurves[p_selected]->setPen(*pen);

      /*Replot with new color.*/
      p_plot->replot();
    }

  }


  /**
   * Called from the "Size:" drop down menu on the configDialog
   * box. Changes the width of the line
   * 
   * 
   * @param size 
   */
  void PlotWindow::changeCurveSize(int size){
    QPen *pen = new QPen(p_colorButton->palette().color(QPalette::Button));
    pen->setWidth(size);
    pen->setStyle(p_plotCurves[p_selected]->pen().style());
    p_plotCurves[p_selected]->setPen(*pen);
    /*Replot with the new size.*/
    p_plot->replot();

  }


  /**
   * Called from the "Style:" drop down menu on the
   * configDialog box.
   * 
   * 
   * @param style 
   */
  void PlotWindow::changeCurveStyle(int style){
    QPen *pen = new QPen(p_colorButton->palette().color(QPalette::Button));
    pen->setWidth(p_plotCurves[p_selected]->pen().width());
    pen->setStyle(Qt::PenStyle(style+1));
    p_plotCurves[p_selected]->setPen(*pen);
    /*Replot with the new style*/
    p_plot->replot();
  }


  /**
   * Called from the "Symbol:" drop down menu on the
   * configDialog box
   * 
   * 
   * @param style 
   */
  void PlotWindow::changeMarkerStyle(int style){
    p_plotCurves[p_selected]->setSymbolStyle(QwtSymbol::Style(style));
    
    /*Replot with the new marker style.*/
    p_plot->replot();

  }


  /**
   * Resets the x/y min/max to the defaults.
   * 
   */
  void PlotWindow::setDefaultRange() {
    QDialog *dialog = new QDialog(p_mainWindow);
    dialog->setWindowTitle("Set Display Range");

    QWidget *buttons = new QWidget (dialog);
    QWidget *textAreas = new QWidget (dialog);
    QWidget *labels = new QWidget (dialog);
    QWidget *main = new QWidget (dialog);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(main,0);
    layout->addWidget(buttons,0);
    dialog->setLayout(layout);

    QToolButton *okButton = new QToolButton(dialog);
    connect(okButton,SIGNAL(released()),this,SLOT(setUserValues()));
    connect(okButton,SIGNAL(released()),dialog,SLOT(hide()));
    okButton->setShortcut(Qt::Key_Enter);
    okButton->setText("Ok");

    QToolButton *cancelButton = new QToolButton(dialog);
    connect(cancelButton,SIGNAL(released()),dialog,SLOT(hide()));
    cancelButton->setText("Cancel");

    QLabel *xLabel = new QLabel("X-Axis: ");
    QLabel *xMinLabel = new QLabel("Minimum: ");
    QLabel *xMaxLabel = new QLabel("Maximum: ");
    QLabel *yLabel = new QLabel("Y-Axis: ");
    QLabel *yMinLabel = new QLabel("Minimum: ");
    QLabel *yMaxLabel = new QLabel("Maximum: ");

    p_xLogCheckBox = new QCheckBox("x - Log Scale");
    p_xLogCheckBox->setChecked(p_xLogScale);
    p_yLogCheckBox = new QCheckBox("y - Log Scale");
    p_yLogCheckBox->setChecked(p_yLogScale);

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->addWidget(xLabel);
    vlayout->addWidget(xMinLabel);
    vlayout->addWidget(xMaxLabel);
    vlayout->addWidget(p_xLogCheckBox);
    vlayout->addWidget(yLabel);    
    vlayout->addWidget(yMinLabel);
    vlayout->addWidget(yMaxLabel);
    vlayout->addWidget(p_yLogCheckBox);
    labels->setLayout(vlayout);

    p_xMinEdit = new QLineEdit(QString::number(p_xMin),dialog);
    p_xMaxEdit = new QLineEdit(QString::number(p_xMax),dialog);
    p_yMinEdit = new QLineEdit(QString::number(p_yMin),dialog);
    p_yMaxEdit = new QLineEdit(QString::number(p_yMax),dialog);

    QVBoxLayout *v2layout = new QVBoxLayout();
    v2layout->addWidget(new QLabel(""));
    v2layout->addWidget(p_xMinEdit);
    v2layout->addWidget(p_xMaxEdit);
    v2layout->addWidget(new QLabel(""));
    v2layout->addWidget(new QLabel(""));
    v2layout->addWidget(p_yMinEdit);
    v2layout->addWidget(p_yMaxEdit);
    v2layout->addWidget(new QLabel(""));
    textAreas->setLayout(v2layout);


    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(labels);
    mainLayout->addWidget(textAreas);
    main->setLayout(mainLayout);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(okButton);
    hlayout->addWidget(cancelButton);
    buttons->setLayout(hlayout);

    dialog->setFixedSize(400,250);
    dialog->show();
  } 


  /**
   * This method creates the dialog box which allows the user
   * to relabel the plot window
   */
  void PlotWindow::reLabel() {
    QDialog *dialog = new QDialog(p_mainWindow);
    dialog->setWindowTitle("Name Plot Labels");

    QWidget *buttons = new QWidget (dialog);
    QWidget *textAreas = new QWidget (dialog);
    QWidget *labels = new QWidget (dialog);
    QWidget *main = new QWidget (dialog);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(main,0);
    layout->addWidget(buttons,0);
    dialog->setLayout(layout);

    QToolButton *okButton = new QToolButton(dialog);
    connect(okButton,SIGNAL(released()),this,SLOT(setLabels()));
    connect(okButton,SIGNAL(released()),dialog,SLOT(hide()));
    okButton->setShortcut(Qt::Key_Enter);
    okButton->setText("Ok");

    QToolButton *cancelButton = new QToolButton(dialog);
    connect(cancelButton,SIGNAL(released()),dialog,SLOT(hide()));
    cancelButton->setText("Cancel");

    QLabel *plotLabel = new QLabel("Plot Title: ");
    QLabel *xAxisLabel = new QLabel("X-Axis Label: ");
    QLabel *yAxisLabel = new QLabel("Y-Axis Label: ");

    QVBoxLayout *vlayout = new QVBoxLayout();
    vlayout->addWidget(plotLabel);
    vlayout->addWidget(xAxisLabel);
    vlayout->addWidget(yAxisLabel);    
    labels->setLayout(vlayout);

    p_plotTitleText = new QLineEdit(p_plot->title().text(),dialog);
    p_xAxisText = new QLineEdit(p_plot->axisTitle(QwtPlot::xBottom).text(),dialog);
    p_yAxisText = new QLineEdit(p_plot->axisTitle(QwtPlot::yLeft).text(),dialog);

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

    dialog->setFixedSize(400,190);
    dialog->show();
  }


  /**
   * Makes the user specified changes to the plot labels.
   */ 
  void PlotWindow::setLabels() {
    p_plot->setTitle(p_plotTitleText->text());
    p_plot->setAxisTitle(QwtPlot::xBottom,p_xAxisText->text());
    p_plot->setAxisTitle(QwtPlot::yLeft,p_yAxisText->text());
    /*Replot with new labels.*/
    p_plot->replot();
  }


  /**
   * This method hides/shows the grid on the plotWindow and
   * changes the text for the action
   */
  void PlotWindow::showGrid(){
    p_grid->setVisible(!p_grid->isVisible());

    if(p_grid->isVisible()){
      p_gridShow->setText("Hide Grid");
    } else{
      p_gridShow->setText("Show Grid");
    }
    p_plot->replot();
  }


  /**
   * Called from the configDialog box to Hide/Show selected
   * curve.
   */ 
  void PlotWindow::showCurve() {
    if(p_selected < 0) return;
    /*shows/hides item on the plot*/
    p_plotCurves[p_selected]->setVisible(!p_plotCurves[p_selected]->isVisible());
    /*Replot with selected curve showing or hidden*/
    p_plot->replot();
    /*Change the text on the button according to what this method will do next time it is pressed.*/
    if(p_plotCurves[p_selected]->isVisible()) p_hideButton->setText("Hide Curve");
    if(!p_plotCurves[p_selected]->isVisible()) p_hideButton->setText("Show Curve");

  }


  /**
   * This method hides/shows the symbols for the selected
   * curve.
   */ 
  void PlotWindow::showSymbols(){
  
    if (p_plotCurves[p_selected]->p_markerIsVisible) {
      p_symbolsButton->setText("Show Symbols");
      
      p_plotCurves[p_selected]->setSymbolVisible(false);
    } else {
      p_symbolsButton->setText("Hide Symbols");
      p_plotCurves[p_selected]->setSymbolVisible(true);
    }

    /*Replot with selected curve's symbols visible*/
    p_plot->replot();

  }


  /**
   *Shows/Hides all the markers(symbols) 
   */ 
  void PlotWindow::hideAllSymbols(){ 
    if (p_hideAllMarkers->text() == "Hide All Symbols") {

      for (int i = 0; i<p_plotCurves.size(); i++) {
        p_plotCurves[i]->setSymbolVisible(false);
      }

      p_hideAllMarkers->setText("Show All Symbols");

    } else {

      for (int i = 0; i<p_plotCurves.size(); i++) {
        p_plotCurves[i]->setSymbolVisible(true);
      }

      p_hideAllMarkers->setText("Hide All Symbols");
    }

    /*Replot with all symbols hidden*/
    p_plot->replot();    
  }


  /**
   * This method shows or hides all of the curves in the
   * plotWindow
   */ 
  void PlotWindow::hideAllCurves(){
    if (p_hideAllCurves->text() == "Hide All Curves") {

      for (int i = 0; i<p_plotCurves.size(); i++) {
        p_plotCurves[i]->hide();
      }

      p_hideButton->setText("Show Curve");
      p_hideAllCurves->setText("Show All Curves");
      p_hideAllCurves->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_showCurves.png"));

    } else {

      for (int i = 0; i<p_plotCurves.size(); i++) {
        p_plotCurves[i]->show();
      }

      p_hideButton->setText("Hide Curve");
      p_hideAllCurves->setText("Hide All Curves");
      p_hideAllCurves->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_hideCurves.png"));

    }

    /*Replot with all curves hidden*/
    p_plot->replot();
  }


  /**
   * This method cancels out any of the changes made on the config
   * dialog box, and copies in the old curve properties.
   * 
   */
  void PlotWindow::cancelConfig(){
    if(p_dragCurve != NULL) p_plotCurves[p_selected]->copyCurveProperties(p_dragCurve);

    configDialog->hide();
    p_plot->replot();
    /*We need to change the title on the table.*/
    if(p_tableWindow != NULL && p_tableWindow->isVisible()) 
      p_tableWindow->table()->horizontalHeaderItem(p_selected)->setText(p_plotCurves[p_selected]->title().text());

    if(p_tableWindow != NULL && p_tableWindow->isVisible()) 
      p_tableWindow->listWidget()->item(p_selected)->setText(p_plotCurves[p_selected]->title().text());
  }


  /**
   * This method creates and shows the help dialog box for the
   * plot window.  this is called from the Help-->Basic Help menu.
   */
  void PlotWindow::showHelp() {
    QDialog *d = new QDialog(p_plot);
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
  void PlotWindow::setupDefaultMenu(){
    QList<QMenu *> menu;
    QList<QAction *> actionButtons;
    getDefaultMenus(menu, actionButtons);
    setCustomMenu(menu, actionButtons);
  }


  /**
   * Sets up the menus added from a parent object.
   * 
   * 
   * @param menu 
   * @param actions 
   */
  void PlotWindow::setCustomMenu(QList<QMenu *> &menu, QList<QAction *> &actions) {
    if (p_toolBar == NULL) {
      p_toolBar = new QToolBar(p_mainWindow);
      p_toolBar->setObjectName("PlotWindow");
      p_toolBar->setAllowedAreas(Qt::LeftToolBarArea|Qt::RightToolBarArea | Qt::TopToolBarArea);
      p_mainWindow->addToolBar(Qt::TopToolBarArea,p_toolBar);
    } else {
      p_toolBar->clear();
    }

    p_menubar = p_mainWindow->menuBar();
    p_menubar->clear();

    for (int i = 0; i < menu.size(); i++) {
      p_menubar->addMenu(menu[i]);
    }

    for (int i = 0; i < actions.size(); i++) {
      p_toolBar->addAction(actions[i]);
    }

  }


  /**
   * Sets up the default menus
   * 
   * 
   * @param menu 
   * @param actions 
   */
  void PlotWindow::getDefaultMenus(QList<QMenu *> &menu, QList<QAction *> &actions) {
    /*setup actions*/
    p_hideAllMarkers = new QAction(p_plot);
    p_hideAllMarkers->setText("Hide All Symbols");
    p_hideAllMarkers->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_markers.png"));
    QString text  =
      "<b>Function:</b>  Displays or hides a symbol for each data point plotted on a plot.";
    p_hideAllMarkers->setWhatsThis(text);
    QObject::connect(p_hideAllMarkers,SIGNAL(activated()),this,SLOT(hideAllSymbols()));

    p_hideAllCurves = new QAction(p_plot);
    p_hideAllCurves->setText("Show All Curves");
    p_hideAllCurves->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_showCurves.png"));
    text  =
      "<b>Function:</b>  Displays or hides all the curves currently displayed on the plot.";
    p_hideAllCurves->setWhatsThis(text);
    QObject::connect(p_hideAllCurves,SIGNAL(activated()),this,SLOT(hideAllCurves()));

    QAction *table = new QAction(p_plot);
    table->setText("Show Table");
    table->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_table.png"));
    text  =
      "<b>Function:</b>  Activates the table which displays the data of the \
      current plot";
    table->setWhatsThis(text);
    QObject::connect(table,SIGNAL(activated()),this,SLOT(showTable()));

    QAction *configPlot = new QAction(p_plot);
    configPlot->setText("Configure Plot Curves");
    configPlot->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_configure.png"));
    text  =
      "<b>Function:</b>  Brings up a dialog box in which the plot curves can be \
      uniquely configured.";
    configPlot->setWhatsThis(text);
    QObject::connect(configPlot,SIGNAL(activated()),this,SLOT(configPlot()));

    QAction *save = new QAction(p_plot);
    save->setText("&Save Plot As");
    save->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/filesaveas.png"));
    text  =
      "<b>Function:</b>  Save the plot as a png, jpg, or tif file.";
    save->setWhatsThis(text);
    QObject::connect(save,SIGNAL(activated()),this,SLOT(savePlot()));

    QAction *prt = new QAction(p_plot);
    prt->setText("&Print Plot");
    prt->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/fileprint.png"));
    text  =
      "<b>Function:</b>  Sends the plot image to the printer";
    prt->setWhatsThis(text);
    QObject::connect(prt,SIGNAL(activated()),this,SLOT(printPlot()));

    QAction *track = new QAction(p_plot);
    track->setText("Show Mouse &Tracking");
    track->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/goto.png"));
    track->setCheckable(true);
    text  =
      "<b>Function:</b>  Displays the x,y coordinates as the cursor moves \
      around on the plot.";
    track->setWhatsThis(text);
    QObject::connect(track,SIGNAL(activated()),this,SLOT(trackerEnabled()));

    QAction *backgrdSwitch = new QAction(p_plot);
    backgrdSwitch->setText("White/Black &Background");
    backgrdSwitch->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_switchBackgrd.png"));
    text  =
      "<b>Function:</b>  Switch the background color between black and white.";
    backgrdSwitch->setWhatsThis(text);
    QObject::connect(backgrdSwitch,SIGNAL(activated()),this,SLOT(switchBackground()));

    p_gridShow = new QAction(p_plot);
    p_gridShow->setText("Show Grid");
    p_gridShow->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_grid.png"));
    text  =
      "<b>Function:</b>  Display grid lines on the plot.";
    p_gridShow->setWhatsThis(text);
    QObject::connect(p_gridShow, SIGNAL(activated()), this, SLOT(showGrid()));
    
    QAction *changeLabels = new QAction(p_plot);
    changeLabels->setText("Rename Plot &Labels");
    changeLabels->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_renameLabels.png"));
    text  =
      "<b>Function:</b>  Edit the plot title, x and y axis labels.";
    changeLabels->setWhatsThis(text);
    QObject::connect(changeLabels,SIGNAL(activated()),this,SLOT(reLabel()));

    QAction *changeScale = new QAction(p_plot);
    changeScale->setText("Set &Display Range");
    changeScale->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_setScale.png"));
    text  =
      "<b>Function:</b>  Adjust the scale for the x and y axis on the plot.";
    changeScale->setWhatsThis(text);
    QObject::connect(changeScale,SIGNAL(activated()),this, SLOT(setDefaultRange()));

    QAction *resetScaleButton = new QAction(p_plot);
    resetScaleButton->setText("Reset Scale");
    resetScaleButton->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_resetscale.png"));
    text  =
      "<b>Function:</b>  Reset the plot's scale.";
    resetScaleButton->setWhatsThis(text);
    QObject::connect(resetScaleButton, SIGNAL(activated()),this, SLOT(resetScale()));

    QAction *clear = new QAction(p_plot);
    clear->setText("Clear Plot");
    clear->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/plot_clear.png"));
    text  =
      "<b>Function:</b>  Removes all the curves from the plot.";
    clear->setWhatsThis(text);
    connect(clear,SIGNAL(activated()),this,SLOT(clearPlot()));

    QAction *getHelp = new QAction(p_plot);
    getHelp->setText("Basic Help");
    QObject::connect(getHelp,SIGNAL(activated()),this, SLOT(showHelp()));

    QAction *close = new QAction(p_plot);
    close->setText("Close");
    QObject::connect(close,SIGNAL(activated()),p_mainWindow, SLOT(close()));

    /*setup menus*/
    QMenu *options = new QMenu("&Options");
    options->addAction(configPlot);
    options->addAction(track);
    options->addAction(backgrdSwitch);
    options->addAction(p_gridShow);
    options->addAction(changeLabels);
    options->addAction(changeScale);
    options->addAction(p_hideAllCurves);
    options->addAction(p_hideAllMarkers);

    QMenu *help = new QMenu("&Help");
    help->addAction(getHelp);

    QMenu *file = new QMenu("&File");
    file->addAction(save);
    file->addAction(prt);
    file->addAction(table);
    file->addAction(close);

    actions.push_back(save);
    actions.push_back(prt);
    actions.push_back(table);
    actions.push_back(clear);
    actions.push_back(configPlot);
    actions.push_back(p_hideAllMarkers);
    actions.push_back(p_hideAllCurves);
    actions.push_back(backgrdSwitch);
    actions.push_back(changeLabels);
    actions.push_back(changeScale);
    actions.push_back(p_gridShow);
    actions.push_back(resetScaleButton);

    menu.push_back(file);
    menu.push_back(options);
    menu.push_back(help);

  }


  /**
   * Returns the number of curves in the plotWindow
   */ 
  int PlotWindow::getNumCurves(){
    return p_plotCurves.size();
  }


  /**
   * Fills in the table with the data from the current curves
   * in the plotWindow
   */ 
  void PlotWindow::fillTable(){
    if (p_tableWindow == NULL) return;
    p_tableWindow->listWidget()->clear();
    p_tableWindow->table()->clear();
    p_tableWindow->table()->setRowCount(0);
    p_tableWindow->table()->setColumnCount(0);

    /*resize rows if needed*/
    unsigned int rows = p_tableWindow->table()->rowCount();

    if (rows != p_plotCurves[p_plotCurves.size()-1]->data().size()) {
      int diff = p_plotCurves[p_plotCurves.size()-1]->data().size() - rows;
      for (int i = 0; i <= abs(diff); i++) {

        if (diff > 0) {
          p_tableWindow->table()->insertRow(rows + i);
        } else {
          p_tableWindow->table()->removeRow(rows - i);
        } 

      }
    }

    p_axisTitle = p_plot->axisTitle(QwtPlot::xBottom).text().toStdString();

    //write X axis
    if (p_tableWindow->table()->columnCount() == 0) {
      p_tableWindow->addToTable (true,p_plot->axisTitle(QwtPlot::xBottom).text()
                                 ,p_plot->axisTitle(QwtPlot::xBottom).text(),0);
      p_header = p_tableWindow->table()->horizontalHeaderItem(0)->text().toStdString();

    } else if (p_header.compare(p_axisTitle)) {

      p_tableWindow->table()->removeColumn(0);
      p_tableWindow->listWidget()->takeItem(0);
      p_tableWindow->addToTable (true,p_plot->axisTitle(QwtPlot::xBottom).text()
                                 ,p_plot->axisTitle(QwtPlot::xBottom).text(), 0);
      p_header = p_tableWindow->table()->horizontalHeaderItem(0)->text().toStdString();

    }

    for (int c = 0; c < p_plotCurves.size(); c++) {
      //adding columns to the table if more curves had been added to the plotWindow
      //columns
      if (c > p_tableWindow->table()->columnCount()-2 ) {
        p_tableWindow->addToTable (true,p_plotCurves[c]->title().text(),
                                   p_plotCurves[c]->title().text());          
      }
      //rows
      for (unsigned int r = 0; r < p_plotCurves[c]->data().size(); r++) {
        //creates a widget item with the data from the curves.
        QTableWidgetItem *item = new QTableWidgetItem
                                 (Isis::iString(p_plotCurves[c]->data().y(r)).ToQt());
        p_tableWindow->table()->setItem(r, c+1, item);
        //p_tableWindow->table()->resizeColumnToContents(c-1);

        QTableWidgetItem *xAxisItem = new QTableWidgetItem
                                      (Isis::iString(p_plotCurves[c]->data().x(r)).ToQt());
        p_tableWindow->table()->setItem(r, 0, xAxisItem);
        //p_tableWindow->table()->resizeColumnToContents(0);
      }//end rows 
    }//end columns
  }

  
  /**
   * This method is called when the user deletes a curve(s)
   * from the plot window.
   */ 
  void PlotWindow::deleteFromTable(){
   if(p_tableWindow == NULL) return;
   p_tableWindow->TableMainWindow::deleteColumn(p_selected);
   fillTable();
 }


  /**
  * This method is called from the showTable action on the tool
  * bar There are some checks done to make sure there are data to 
  * fill the table
  */
  void PlotWindow::showTable(){
    if(p_plotCurves.size() < 1) return;

    if(p_tableWindow == NULL){
       //p_tableWindow = new Qisis::TableMainWindow("Plot Table", this);
       p_tableWindow = new Qisis::TableMainWindow("Plot Table", p_parent);
       p_tableWindow->setTrackListItems(false);
    }
    fillTable();
    p_tableWindow->show();
    p_tableWindow->syncColumns();
  }


  /**
   * This method is called from the context menu which is shown on
   * the plot legend with a right mouse click.  It creates a new
   * curve, which is a static curve, then loads all of the
   * properties of the curve that was clicked into the new curve.
   * Now the new curve can be pasted into another plot window.
   */
  void PlotWindow::copyCurve(){
    if(p_selected < 0) return;
    emit curveCopied(p_plotCurves[p_selected]);
    p_curveCopied = true;    
  }


  /**
   * This method is called from the context menu which is shown on
   * the plot legend with a right mouse click.  (Only shown when
   * p_curveCopied is true.) Sets the p_curveCopied flag to false, so we know to
   * disable the paste option in the context menu.
   */
  void PlotWindow::pasteCurve(){
    emit curvePaste(this);
    p_curveCopied = false;
  }


  /**
   * Emits the curvePasteSpecial signal
   * 
   */
  void PlotWindow::pasteCurveSpecial(){
    emit curvePasteSpecial(this);
    p_curveCopied = false;
  }


  /**
   * This method creates the context menu which pops up when the
   * user right clicks on a legend item.
   */
  void PlotWindow::createLegendMenu(){
    p_legendMenu = new QMenu(p_mainWindow);
    QAction *configure = new QAction(p_plot);
    configure->setText("Configure");
    QObject::connect(configure,SIGNAL(activated()),this,SLOT(configPlot()));
    p_legendMenu->addAction(configure);

    p_copyCurveAction = new QAction(p_plot);
    p_copyCurveAction->setText("Copy Curve");
    QObject::connect(p_copyCurveAction,SIGNAL(activated()),this,SLOT(copyCurve()));
    p_legendMenu->addAction(p_copyCurveAction);

    p_pasteCurve = new QAction(p_plot);
    p_pasteCurve->setText("Paste Curve");
    QObject::connect(p_pasteCurve,SIGNAL(activated()),this,SLOT(pasteCurve()));
    p_legendMenu->addAction(p_pasteCurve);

    p_pasteSpecial = new QAction(p_plot);
    p_pasteSpecial->setText("Paste Special");
    QObject::connect(p_pasteSpecial,SIGNAL(activated()),this,SLOT(pasteCurveSpecial()));
    p_legendMenu->addAction(p_pasteSpecial);

    p_deleteCurve = new QAction(p_plot);
    p_deleteCurve->setText("Delete Curve");
    QObject::connect(p_deleteCurve,SIGNAL(activated()),this,SLOT(deleteCurve()));
    p_legendMenu->addAction(p_deleteCurve);

    QAction *resetScaleAction = new QAction(p_plot);
    resetScaleAction->setText("Reset Scale");
    QObject::connect(resetScaleAction, SIGNAL(activated()), this, SLOT(resetScale()));
    p_legendMenu->addAction(resetScaleAction);

    p_hideShowCurve = new QAction(p_plot);
    p_hideShowCurve->setText("Hide Curve");
    QObject::connect(p_hideShowCurve, SIGNAL(activated()), this, SLOT(showCurve()));
    p_legendMenu->addAction(p_hideShowCurve);
  }


  /**
   * This method filters the events of the objects it is connected
   * to.  In this case, the eventFilter has been installed on the
   * p_mainWindow, p_plot, and p_legend.
   * @param o
   * @param e
   * 
   * @return bool
   */
  bool PlotWindow::eventFilter(QObject *o,QEvent *e) {
    switch (e->type()) {
    case QEvent::ContextMenu:{
        p_eventObject = o;

        for (int i = 0; i<p_plotCurves.size(); i++) {

          if ((QwtPlotCurve *) p_legend->find((QWidget *)p_eventObject) == p_plotCurves[i]) {
            p_selected = i;
          }
        }

        QPoint *contextMenuPos = new QPoint(((QContextMenuEvent *)e)->globalPos());
        p_legendMenu->setGeometry(contextMenuPos->x(), contextMenuPos->y(), 110,80);
        p_legendMenu->adjustSize();

        /*disable/enable menu items depending on the state of the window.*/
        if (!p_curveCopied) {
          p_pasteCurve->setEnabled(false);
          p_pasteSpecial->setEnabled(false);
        } else {
          p_pasteCurve->setEnabled(true);
          p_pasteSpecial->setEnabled(true);
        }

        if (!p_pasteable)p_pasteCurve->setEnabled(false);
        if (!p_pasteable)p_pasteSpecial->setEnabled(false);

        if (o->inherits("QwtPlot")) {
          p_copyCurveAction->setEnabled(false);
          p_hideShowCurve->setEnabled(false);
        }
        if (o->inherits("QwtLegendItem")) {
          p_copyCurveAction->setEnabled(true);
          p_hideShowCurve->setEnabled(true);
        }

        if (p_selected > 0 && p_selected < p_plotCurves.size()) {
          if (p_plotCurves[p_selected]->isVisible()) p_hideShowCurve->setText("Hide Curve");
          if (!p_plotCurves[p_selected]->isVisible())p_hideShowCurve->setText("Show Curve");
        }

        if (!p_deletable || o->inherits("QwtPlot") || p_plotCurves.size() < 1) {
          p_deleteCurve->setEnabled(false);
        } else {
          p_deleteCurve->setEnabled(true);
        }
        p_legendMenu->show();

        return true;
      }

    case QEvent::Close:{
        writeSettings();
        if (p_destroyOnClose) {
          emit destroyed(this);
        }
      }
    default: {
        return FALSE;
      }
    }
  }


  /** 
   * This overridden method is called from the constructor so that 
   * when the mainwindow is created, it knows it's size 
   * and location and the tool bar location. 
   * 
   */
  void PlotWindow::readSettings(){
    /*Call the base class function to read the size and location*/
    this->MainWindow::readSettings();
    std::string appName = p_parent->windowTitle().toStdString();
    /*Now read the settings that are specific to this window.*/
    std::string instanceName = this->windowTitle().toStdString();
    Isis::Filename config("$HOME/.Isis/" + appName + "/" + instanceName + ".config");

    QSettings settings(QString::fromStdString(config.Expanded()), QSettings::NativeFormat);
    QByteArray state = settings.value("state", QByteArray("0")).toByteArray();
    p_mainWindow->restoreState(state);
  }


  /** 
   * This overridden method is called when the mainwindow 
   * is closed or hidden to write the size and location settings 
   * (and tool bar location) to a config file in the user's home 
   * directory. 
   * 
   */
  void PlotWindow::writeSettings(){
    /*Call the base class function to write the size and location*/
    this->MainWindow::writeSettings();
    std::string appName = p_parent->windowTitle().toStdString();
    /*Now write the settings that are specific to this window.*/
    std::string instanceName = this->windowTitle().toStdString();
    Isis::Filename config("$HOME/.Isis/" + appName + "/" + instanceName + ".config");  

    QSettings settings(QString::fromStdString(config.Expanded()), QSettings::NativeFormat);
    settings.setValue("state", p_mainWindow->saveState());
  }

}

