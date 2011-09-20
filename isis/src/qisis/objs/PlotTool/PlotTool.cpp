#include "PlotTool.h"

#include <iostream>

#include "geos/geom/Polygon.h"
#include "geos/geom/CoordinateArraySequence.h"
#include "geos/geom/Point.h"

#include "Brick.h"
#include "Cube.h"
#include "InterestOperator.h"
#include "MdiCubeViewport.h"
#include "PlotTool.h"
#include "PlotToolWindow.h"
#include "PlotWindow.h"
#include "PolygonTools.h"
#include "Pvl.h"
#include "RubberBandComboBox.h"
#include "RubberBandTool.h"
#include "Statistics.h"
#include "ToolPad.h"

using std::cerr;

namespace Isis {

  /**
   * This constructs a plot tool. The plot tool graphs either DN values across a
   * line, or statistics across a spectrum (bands).
   *
   *
   * @param parent
   */
  PlotTool::PlotTool(QWidget *parent) : Tool(parent) {
    p_spectralRubberBand = NULL;
    p_spacialRubberBand = NULL;
    RubberBandTool::allowPoints(1);
    p_parent = parent;
    createWindow();
    setupPlotCurves();
    p_scaled = false;
    p_changingInterp = false;
    p_action = new QAction(p_plotToolWindow);
    p_plotToolWindow->setPlotType("Band");
    p_action->setText("PlotTool");
    p_action->setIcon(QPixmap(toolIconDir() + "/plot.png"));
    QObject::connect(p_action, SIGNAL(activated()), this, SLOT(showPlotWindow()));
    QObject::connect(this, SIGNAL(viewportChanged()), this, SLOT(viewportSelected()));
    p_color = 0;
  }


  /**
   * This protected slot is called when user selects a viewport.
   *
   */
  void PlotTool::viewportSelected() {
    //p_autoScale->setChecked(true);
  }


  /**
   * This method is called when the tool is activated by the
   *   parent, or when the plot mode is changed. It's used to
   *   activate or change the rubber banding mode to be either
   *   rectangle or line, depending on the current plot type.
   */
  void PlotTool::enableRubberBandTool() {
    if(p_spectralRubberBand) {
      if(p_currentPlotType == SpatialPlot) {
        p_spacialRubberBand->reset();

        p_spacialRubberBand->setVisible(true);
        p_spacialRubberBand->setEnabled(true);

        p_spectralRubberBand->setEnabled(false);
        p_spectralRubberBand->setVisible(false);

        plotType->setEnabled(false);
      }
      else {
        p_spectralRubberBand->reset();

        p_spectralRubberBand->setEnabled(true);
        p_spectralRubberBand->setVisible(true);

        p_spacialRubberBand->setVisible(false);
        p_spacialRubberBand->setEnabled(false);

        plotType->setEnabled(true);
      }
    }
  }


  /**
   * Changes the text for the hide/show band lines action in the
   * Options menu.
   *
   */
  void PlotTool::showHideLines() {
    if(p_plotToolWindow->p_markersVisible) {
      p_showHideLines->setText("Hide Band Line(s)");
    }
    else {
      p_showHideLines->setText("Show Band Line(s)");
    }
  }


  /**
   * This method is connected to the plot type combo box. When the
   * user changes it's value, this changes the plot mode. The
   * first thing we do is set the new rubber banding for the new
   * plot type. Then, we update the plot window's menus. Finally,
   * we clear the old plotted lines and reset the plot scale.
   *
   *
   * @param newType
   */
  void PlotTool::changePlotType(int newType) {
    p_currentPlotType = (PlotType) p_plotTypeCombo->itemData(newType).toInt();
    enableRubberBandTool();

    QList<QMenu *> menu;
    QList<QAction *> actionButtons;

    /* menu is the QMenu's at the top of the plot window, while
     actionButtons are the buttons directly below.*/
    p_plotToolWindow->getDefaultMenus(menu, actionButtons);

    for(int i = 0; i < menu.size(); i++) {
      if(menu[i]->title() == "&Options") {
        p_showHideLines = new QAction(p_plotToolWindow);
        p_showHideLines->setText("Hide Band Line(s)");
        p_showHideLines->setIcon(QPixmap("/usgs/cpkgs/isis3/data/base/icons/camera.png"));
        QObject::connect(p_showHideLines, SIGNAL(activated()), p_plotToolWindow,
                         SLOT(showHideLines()));
        QObject::connect(p_showHideLines, SIGNAL(activated()), this,
                         SLOT(showHideLines()));
        menu[i]->addAction(p_showHideLines);

        p_autoScale = new QAction(p_plotToolWindow);
        p_autoScale->setText("AutoScale");
        p_autoScale->setCheckable(true);
        p_autoScale->setChecked(true);
        QString text  =
          "<b>Function:</b>  Turn on/off the auto scale option on the plot.";
        p_autoScale->setWhatsThis(text);
        menu[i]->addAction(p_autoScale);
        actionButtons.push_back(p_autoScale);

      }
    }

    if(p_currentPlotType == SpectralPlot) {
      p_plotToolWindow->setAxisLabel(QwtPlot::xBottom, "Band");
      p_plotToolWindow->setAxisLabel(QwtPlot::yLeft, "Value");

      // QAction *example = new QAction(p_plotToolWindow);
      // example->setText("Example");
      //example->setIcon(QPixmap(""));
      //connect(clear,SIGNAL(activated()),this,SLOT(someSlot()));
      QMenu *spectralOptions = new QMenu("&Spectral Options");
      //spectralOptions->addAction(example);

      // -1 is in order to insert before "Help"
      menu.insert(menu.size() - 1, spectralOptions);
      //actionButtons.push_back(example);
    }
    else if(p_currentPlotType == SpatialPlot) {
      // Spatial plot plots value versus pixel (distance).
      p_plotToolWindow->setAxisLabel(QwtPlot::xBottom, "Pixel");
      p_plotToolWindow->setAxisLabel(QwtPlot::yLeft, "Value");

      p_cubicInterp = new QAction(p_plotToolWindow);
      p_bilinearInterp = new QAction(p_plotToolWindow);
      p_nearestNeighborInterp = new QAction(p_plotToolWindow);

      p_cubicInterp->setText("&Cubic Interpolation");
      p_bilinearInterp->setText("&BiLinear Interpolation");
      p_nearestNeighborInterp->setText("&Nearest Neighbor Interpolation");

      p_cubicInterp->setCheckable(true);
      p_bilinearInterp->setCheckable(true);
      p_bilinearInterp->setChecked(true);
      p_nearestNeighborInterp->setCheckable(true);

      connect(p_cubicInterp, SIGNAL(activated()), this,
              SLOT(cubicInterpolationChanged()));
      connect(p_bilinearInterp, SIGNAL(activated()), this,
              SLOT(bilinearInterpolationChanged()));
      connect(p_nearestNeighborInterp, SIGNAL(activated()), this,
              SLOT(nearestInterpolationChanged()));

      QMenu *interpolation = new QMenu("&Interpolation");
      interpolation->addAction(p_cubicInterp);
      interpolation->addAction(p_bilinearInterp);
      interpolation->addAction(p_nearestNeighborInterp);

      QMenu *spatialOptions = new QMenu("&Spatial Options");
      spatialOptions->addMenu(interpolation);

      // -1 is in order to insert before "Help"
      menu.insert(menu.size() - 1, spatialOptions);
    }

    p_plotToolWindow->setCustomMenu(menu, actionButtons);
    p_plotToolWindow->clearPlotCurves(false);
  }


  /**
   *
   *
   * @param toolpad
   *
   * @return QAction*
   */
  QAction *PlotTool::toolPadAction(ToolPad *toolpad) {
    QAction *action = new QAction(toolpad);
    action->setIcon(QPixmap(toolIconDir() + "/plot.png"));
    action->setToolTip("Plot (L)");
    action->setShortcut(Qt::Key_L);
    QObject::connect(action, SIGNAL(activated()), this, SLOT(showPlotWindow()));

    QString text  =
      "<b>Function:</b>  Plot values in active viewport \
      <p><b>Shortcut:</b> L</p> ";
    action->setWhatsThis(text);
    return action;
  }


  /**
   * Creates the widgets for the tool bar.
   *
   *
   * @param parent
   *
   * @return QWidget*
   */
  QWidget *PlotTool::createToolBarWidget(QStackedWidget *parent) {
    QWidget *hbox = new QWidget(parent);

    p_spectralRubberBand = new RubberBandComboBox(
      RubberBandComboBox::Polygon |
      RubberBandComboBox::Rectangle,
      RubberBandComboBox::Rectangle
    );

    p_spacialRubberBand = new RubberBandComboBox(
      RubberBandComboBox::Line |
      RubberBandComboBox::RotatedRectangle,
      RubberBandComboBox::Line,
      true
    );


    QToolButton *newWindowButton = new QToolButton();
    newWindowButton->setText("New");
    newWindowButton->setToolTip("Opens a new blank plot window");
    QString windowText =
      "<b>Function:</b> This button will bring up a blank plot window that allows \
     the user to copy and paste curves from the active plot window to other windows \
    <p><b>Shortcut:</b>  CTRL+W</p>";
    newWindowButton->setWhatsThis(windowText);
    newWindowButton->setShortcut(Qt::CTRL + Qt::Key_W);
    connect(newWindowButton, SIGNAL(clicked()), this, SLOT(newPlotWindow()));

    QToolButton *plotButton = new QToolButton();
    plotButton->setText("Show");
    plotButton->setToolTip("Shows the active the plot window");
    QString text =
      "<b>Function:</b> This button will bring up the plot window that allows \
     the user to view the min, max, and average values of each band in a  \
     selected range of the image. <p><b>Shortcut:</b>  CTRL+L</p>";
    plotButton->setWhatsThis(text);
    plotButton->setShortcut(Qt::CTRL + Qt::Key_L);
    connect(plotButton, SIGNAL(clicked()), this, SLOT(showPlotWindow()));

    plotType = new QComboBox(hbox);
    plotType->addItem("Band Number");
    //plotType->addItem("Wavelength");
    connect(plotType, SIGNAL(activated(int)), this, SLOT(changePlot()));
    connect(plotType, SIGNAL(activated(int)), this, SLOT(setPlotType()));

    QHBoxLayout *layout = new QHBoxLayout(hbox);
    layout->setMargin(0);
    layout->addWidget(p_spectralRubberBand);
    layout->addWidget(p_spacialRubberBand);
    layout->addWidget(p_plotTypeCombo);
    layout->addWidget(plotType);
    layout->addWidget(newWindowButton);
    layout->addWidget(plotButton);
    layout->addStretch(1);
    hbox->setLayout(layout);
    
    return hbox;
  }


  /**
   * Changes the x axis to band number of wavelength values.
   *
   */
  void PlotTool::setPlotType() {
    if(plotType->currentText() == "Band Number") {
      p_plotToolWindow->setPlotType("Band");
    }
    else {
      p_plotToolWindow->setPlotType("Wavelength");
    }

  }


  /**
   * Adds the plot tool to the menu.
   *
   *
   * @param menu
   */
  void PlotTool::addTo(QMenu *menu) {
    menu->addAction(p_action);
  }


  /**
   * Updates plot tool.
   *
   */
  void PlotTool::updateTool() {

    if(cubeViewport() == NULL) {
      p_action->setEnabled(false);
    }
    else {
      p_action->setEnabled(true);

      MdiCubeViewport *cvp = cubeViewport();
      p_plotToolWindow->setViewport(cvp);
      connect(cvp, SIGNAL(viewportUpdated()), p_plotToolWindow, SLOT(drawBandMarkers()));

      Pvl &pvl = *cvp->cube()->getLabel();
      PvlGroup &dim = pvl.FindObject("IsisCube")
                      .FindObject("Core")
                      .FindGroup("Dimensions");
      int bands = dim["Bands"];

      if(!pvl.FindObject("IsisCube").HasGroup("BandBin")) {
        plotType->removeItem(1);
      }
      else {
        PvlGroup &bandBin = pvl.FindObject("IsisCube")
                            .FindGroup("BandBin");

        if(plotType->findText("Wavelength", Qt::MatchExactly) == -1) {
          for(int i = 0; i < bandBin.Keywords(); i++) {

            if(bandBin[i].Name() == "Center" && bandBin[i].Size() == bands) {

              /*This little chunk of code will be used later when we add the
               option to allow plotting by any keyword in the BandBin group.
               -------------------------------------------------------------*/
              //if (bandBin[i].Size() == bands &&
              //plotType->findText(QString(bandBin[i].Name().c_str())) == -1){
              //plotType->addItem(QString(bandBin[i].Name().c_str()));

              plotType->addItem("Wavelength");

            }
          } // end for bandBin.Keywords()

        }
        else {
          for(int i = 0; i < bandBin.Keywords(); i++) {
            if(bandBin[i].Name() == "Center" && bandBin[i].Size() != bands) {

              plotType->removeItem(1);

            }
          } // end for bandBin.Keywords()

        }// if wavelength is not in drop down already
      }
    }
  }


  /**
   * Creates the active plot window
   *
   */
  void PlotTool::createWindow() {
    p_plotToolWindow = new PlotToolWindow("Active Plot Window", p_parent);
    p_plotToolWindow->setBandMarkersVisible(true);
    p_plotToolWindow->setDestroyOnClose(false);
    p_plotToolWindow->setPlotBackground(Qt::black);
    p_plotToolWindow->setDeletable(false);
    p_plotToolWindow->setPasteable(false);
    connect(p_plotToolWindow, SIGNAL(curveCopied(PlotCurve *)), this,
            SLOT(copyCurve(PlotCurve *)));


    /* Our configuring the plot window depends on this combo box*/
    p_plotTypeCombo = new QComboBox();
    p_plotTypeCombo->addItem("Spectral Plot", QVariant(SpectralPlot));
    p_plotTypeCombo->addItem("Spatial Plot", QVariant(SpatialPlot));
    connect(p_plotTypeCombo, SIGNAL(activated(int)), this,
            SLOT(changePlotType(int)));
    changePlotType(0); /* first element is default*/
    p_plotWindowsCopy.push_back(p_plotToolWindow);
  }


  /**
   * displays the plot window
   *
   */
  void PlotTool::showPlotWindow() {
    p_plotToolWindow->showWindow();

  }


  /**
   * Called when the user has finished drawing with the rubber
   * band.  ChangePlot is called to plot the data within the
   * rubber band.
   *
   */
  void PlotTool::rubberBandComplete() {
    p_plotToolWindow->raise();
    if(RubberBandTool::isValid()) {
      changePlot();
    }
    else {
      QMessageBox::information(p_plotToolWindow, "Error",
                               "The selected Area contains no valid pixels",
                               QMessageBox::Ok);
    }
  }


  /**
   * This method creates and displays a blank plot window in which
   * users can paste curves to and copy curves from.
   */
  void PlotTool::newPlotWindow() {
    PlotToolWindow *blankWindow = new PlotToolWindow("Plot Window", p_parent);
    blankWindow->setDestroyOnClose(true);
    connect(blankWindow, SIGNAL(curvePaste(PlotWindow *)), this,
            SLOT(pasteCurve(PlotWindow *)));
    connect(blankWindow, SIGNAL(curvePasteSpecial(PlotWindow *)), this,
            SLOT(pasteCurveSpecial(PlotWindow *)));
    connect(blankWindow, SIGNAL(curveCopied(PlotCurve *)), this,
            SLOT(copyCurve(PlotCurve *)));
    connect(blankWindow, SIGNAL(destroyed(QObject *)), this,
            SLOT(removeWindow(QObject *)));
    connect(blankWindow, SIGNAL(plotChanged()), this, SLOT(updateViewPort()));
    
    blankWindow->setAxisLabel(QwtPlot::xBottom, p_plotToolWindow->getAxisLabel
                              (QwtPlot::xBottom).text());
    blankWindow->setAxisLabel(QwtPlot::yLeft, p_plotToolWindow->getAxisLabel
                              (QwtPlot::yLeft).text());
    blankWindow->setPlotBackground(p_plotToolWindow->getPlotBackground());
    blankWindow->setScale(QwtPlot::xBottom, p_plotToolWindow->p_xMin,
                          p_plotToolWindow->p_xMax);
    blankWindow->setScale(QwtPlot::yLeft, p_plotToolWindow->p_yMin,
                          p_plotToolWindow->p_yMax);
    blankWindow->setPlotTitle(p_plotToolWindow->getPlotTitle().text());
    blankWindow->setDeletable(true);
    blankWindow->setPasteable(true);
    blankWindow->setCopyEnable(false);
    blankWindow->setupDefaultMenu();
    blankWindow->showWindow();

    p_plotWindows.push_back(blankWindow);

  }


  /**
   * This method replots the data, with current settings and rubber band, in the plot window.
   */
  void PlotTool::changePlot() {
    MdiCubeViewport *cvp = cubeViewport();

    /* Delete any current curves*/
    p_plotToolWindow->clearPlotCurves();

    /* We'll need X-Axis labels and a xMax to scale to.*/
    std::vector<double> labels;
    std::vector<double> stddevLabels;
    double xMax = 10.0;
    Statistics wavelengthStats;

    QString plotTitle = cvp->windowTitle();
    plotTitle.truncate(cvp->windowTitle().lastIndexOf('@'));

    if(p_currentPlotType == SpectralPlot) {
      std::vector<double> avgarray, minarray, maxarray, std1array, std2array,
          wavelengtharray;
      QVector< double > stddevarray;
      std::vector<Statistics> plotStats;

      getSpectralStatistics(labels, plotStats);
      xMax = labels.size();
      Cube *cube = cvp->cube();

      Pvl &pvl = *cube->getLabel();

      Statistics scalingStats;
      for(unsigned int index = 0; index < labels.size(); index++) {
        if (!IsSpecial(plotStats[index].Average()) &&
            !IsSpecial(plotStats[index].Minimum()) &&
            !IsSpecial(plotStats[index].Maximum())) {
          avgarray.push_back(plotStats[index].Average());
          minarray.push_back(plotStats[index].Minimum());
          maxarray.push_back(plotStats[index].Maximum());
          scalingStats.AddData(plotStats[index].Minimum());
          scalingStats.AddData(plotStats[index].Maximum());

          if (!IsSpecial(plotStats[index].StandardDeviation())) {
            stddevLabels.push_back(labels[index]);
            std1array.push_back(plotStats[index].Average() +
                                plotStats[index].StandardDeviation());
            std2array.push_back(plotStats[index].Average() -
                                plotStats[index].StandardDeviation());
            stddevarray.push_back(plotStats[index].StandardDeviation());
          }
        }
        else {
          labels.erase(labels.begin() + index);
          plotStats.erase(plotStats.begin() + index);

          if (index >= 0)
            index--;
        }

        if(pvl.FindObject("IsisCube").HasGroup("BandBin")) {
          PvlGroup &bandBin = pvl.FindObject("IsisCube").FindGroup("BandBin");
          if(bandBin.HasKeyword("Center")) {
            PvlKeyword &wavelength = bandBin.FindKeyword("Center");
            if((unsigned)wavelength.Size() > index) {
              wavelengtharray.push_back(wavelength[index]);
              wavelengthStats.AddData(wavelength[index]);
            }
          }
        }
      } /*end for loop*/

      double border = (scalingStats.Maximum() - scalingStats.Minimum()) * 0.25;
      if(p_autoScale->isChecked())
        p_plotToolWindow->setScale(QwtPlot::yLeft, scalingStats.Minimum() -
                                   border, scalingStats.Maximum() + border);

      if(labels.size() > 0) {
        p_avgCurve->setData(&labels[0], &avgarray[0], labels.size());
        p_minCurve->setData(&labels[0], &minarray[0], labels.size());
        p_maxCurve->setData(&labels[0], &maxarray[0], labels.size());
        p_stdDev1Curve->setData(&stddevLabels[0], &std1array[0],
                                stddevLabels.size());
        p_stdDev2Curve->setData(&stddevLabels[0], &std2array[0],
                                stddevLabels.size());
      }

      p_plotToolWindow->setStdDev(stddevarray);
      p_plotToolWindow->add(p_stdDev1Curve);
      p_plotToolWindow->add(p_stdDev2Curve);
      p_plotToolWindow->add(p_minCurve);
      p_plotToolWindow->add(p_maxCurve);
      p_plotToolWindow->add(p_avgCurve);
      p_plotToolWindow->setViewport(p_minCurve->getViewPort());
      p_plotToolWindow->drawBandMarkers();

      /*copy the average curve each time the user re-plots data*/
      copyCurve(p_avgCurve);
      p_plotToolWindow->p_curveCopied = true;

      if(plotType->currentText() == "Band Number") {
        if(p_autoScale->isChecked())
          p_plotToolWindow->setScale(QwtPlot::xBottom, 1, xMax);
      }
      else {
        if(p_autoScale->isChecked())
          p_plotToolWindow->setScale(QwtPlot::xBottom, wavelengthStats.Minimum(),
                                     wavelengthStats.Maximum());
      }

      if(cvp->isGray()) {
        plotTitle.append(QString("- Band %1").arg(cvp->grayBand()));
      }
      else {
        plotTitle.append(QString("- Bands %1, %2, %3").arg(cvp->redBand()).arg(cvp->greenBand()).arg(cvp->blueBand()));
      }
    }
    else if (p_currentPlotType == SpatialPlot) {
    
      for (int i = 0; i < p_dnCurves.size(); i++)
      {
        if (p_dnCurves[i])
        {
          delete p_dnCurves[i];
          p_dnCurves[i] = NULL;
        }
      }
      p_dnCurves.clear();
      
      MdiCubeViewport * activeViewport = cubeViewport();
      QColor color(Qt::white);
      // get curves for active viewport and also for any linked viewports
      for (int i = 0; i < (int) cubeViewportList()->size(); i++)
      {
        MdiCubeViewport * curViewport = cubeViewportList()->at(i);
        if (curViewport == activeViewport ||
            (activeViewport->isLinked() && curViewport->isLinked()))
        {
          // add new curve to our list of curves (use viewport window title as
          // our label in the legend
          p_dnCurves.append(newDNCurve(
              curViewport->parentWidget()->windowTitle(), color));
          
          // provide a new color for next linked viewport.  Can support up to
          // 11 unique colors (after which colors are re-used)
          color = QColor((Qt::GlobalColor) ((i + 7) % 11));

          // get statistics for this viewport
          std::vector<double> dnValues;
          labels.clear();
          getSpatialStatistics(labels, dnValues, xMax, curViewport);
          
          // do our own autoscaling
          Statistics scaleStats;
          scaleStats.AddData(&dnValues[0], dnValues.size());
          double border = (scaleStats.Maximum() - scaleStats.Minimum()) * 0.25;
          if (p_autoScale->isChecked()) {
            p_plotToolWindow->setScale(QwtPlot::yLeft, scaleStats.Minimum() -
                border, scaleStats.Maximum() + border);
            p_plotToolWindow->setScale(QwtPlot::xBottom, 1, xMax);
          }
          
          // load data into curve
            p_dnCurves[p_dnCurves.size() - 1]->setData(&labels[0],
                &dnValues[0], labels.size());
                
          // add curve to plot
          p_plotToolWindow->add(p_dnCurves[p_dnCurves.size() - 1]);
          
          
          p_plotToolWindow->drawBandMarkers();
          p_plotToolWindow->fillTable();
          
          if(cvp->isGray()) {
            plotTitle.append(QString("- Band %1").arg(cvp->grayBand()));
          }
          else {
            plotTitle.append(QString("- Band %1").arg(cvp->redBand()));
          }
        }
        
      }
    }

    p_plotToolWindow->setPlotTitle(plotTitle);
    p_plotToolWindow->showWindow();
    p_plotToolWindow->replot();
    updateTool();
  }


  /**
   * This method creates a new PlotToolCurve and copies the
   * properties of the curve the user clicked on into the new
   * curve. The plotWindow class emits a signal when a curve has
   * been requested to be copied.
   * @param pc
   */
  void PlotTool::copyCurve(PlotCurve *pc) {
    p_copyCurve = new PlotToolCurve();
    p_copyCurve->copyCurveProperties((PlotToolCurve *)pc);
  }


  /**
   * This method pastes the copied curve into the given plot
   * window.  The plotWindow class emits a signal when a paste
   * command has taken place inside the window.
   * @param pw
   */
  void PlotTool::pasteCurve(PlotWindow *pw) {
     p_cvp = cubeViewport();
    pw->add(p_copyCurve);
    updateViewPort(p_copyCurve);
  }


  /**
   * This method does the same as the above method but gives the
   * curve a differenet color than the copied curve.
   * @param pw
   */
  void PlotTool::pasteCurveSpecial(PlotWindow *pw) {
     p_cvp = cubeViewport();
    if(p_color < p_colors.size()) {
      p_copyCurve->setColor(p_colors[p_color]);
    }
    else {
      QColor c = QColorDialog::getColor(Qt::white, p_plotToolWindow);
      if(c.isValid()) {
        p_copyCurve->setColor(c);
      }
    }
    pw->add(p_copyCurve);
    updateViewPort(p_copyCurve);
    p_color++;

  }


  /**
   * This class keeps a list of how many plot windows it has
   * created (in a QList).  When a user closes a window, we want
   * to remove that window from our QList.  The PlotWindow class
   * emits a signal when the window has been destroyed so we can
   * call this slot when that signal has been sent.
   * @param window
   */
  void PlotTool::removeWindow(QObject *window) {
    for(int i = 0; i < p_plotWindows.size(); i++) {

      if(p_plotWindows[i] == window) {
        p_plotWindows.removeAt(i);

      }
    }
    updateViewPort();
  }

  /**
   * Remove plot window when main app is closed
   * 
   * @author Sharmila Prasad (3/18/2011)
   */
  void PlotTool::removeWindow(void)
  {
    for(int i = 0; i < p_plotWindowsCopy.size(); i++) {
        p_plotWindowsCopy[i]->closeAll();
        p_plotWindowsCopy.removeAt(i);
    }   
  }
  
  /**
   * This method sets up the names, line style, and color  of the
   * all the PlotToolCurves that will be used in this class. This
   * method also fills the p_colors QList with the colors that
   * will be used when the user copies and pastes (special) into
   * another plot window.
   */
  void PlotTool::setupPlotCurves() {
    p_maxCurve = new PlotToolCurve();
    p_maxCurve->setTitle("Maximum");
    p_minCurve = new PlotToolCurve();
    p_minCurve->setTitle("Minimum");
    p_avgCurve = new PlotToolCurve();
    p_avgCurve->setTitle("Average");
    p_stdDev1Curve = new PlotToolCurve();
    p_stdDev1Curve->setTitle("+ Sigma");
    p_stdDev2Curve = new PlotToolCurve();
    p_stdDev2Curve->setTitle("- Sigma");

    QPen *pen = new QPen(Qt::white);
    pen->setWidth(2);
    p_avgCurve->setPen(*pen);

    pen->setColor(Qt::cyan);
    pen->setStyle(Qt::DashLine);
    p_maxCurve->setPen(*pen);
    p_minCurve->setPen(*pen);

    pen->setColor(Qt::red);
    pen->setStyle(Qt::DotLine);
    p_stdDev1Curve->setPen(*pen);
    p_stdDev2Curve->setPen(*pen);

    /*This is for the spatial plot*/
    //p_dnCurves.append(newDNCurve("DN Values"));

    /*Setup colors for paste special*/
    p_colors.push_back(Qt::cyan);
    p_colors.push_back(Qt::magenta);
    p_colors.push_back(Qt::yellow);
    p_colors.push_back(QColor(255, 170, 255));
    p_colors.push_back(Qt::green);
    p_colors.push_back(Qt::white);
    p_colors.push_back(Qt::blue);
    p_colors.push_back(Qt::red);
    p_colors.push_back(QColor(134, 66, 176));
    p_colors.push_back(QColor(255, 152, 0));



  }
  
  
  PlotToolCurve * PlotTool::newDNCurve(QString name, QColor color) {
    PlotToolCurve * newCurve = new PlotToolCurve();
    newCurve->setTitle(name);
    QPen * pen = new QPen(color);
    pen->setWidth(2);
    newCurve->setPen(*pen);
    
    return newCurve;
  }


  /**
   *
   *
   * @param labels
   * @param data
   */
  void PlotTool::getSpectralStatistics(std::vector<double> &labels,
                                       std::vector<Statistics> &data) {
    MdiCubeViewport *cvp = cubeViewport();

    if(plotType->currentText() == "Band Number") {
      p_plotToolWindow->setAxisLabel(QwtPlot::xBottom, "Band");
    }
    else {
      p_plotToolWindow->setAxisLabel(QwtPlot::xBottom, "Wavelength");
    }

    QList<QPoint> vertices = RubberBandTool::getVertices();

    p_avgCurve->setViewPort(cvp);
    p_avgCurve->setVertices(vertices);

    p_minCurve->setViewPort(cvp);
    p_minCurve->setVertices(vertices);

    p_maxCurve->setViewPort(cvp);
    p_maxCurve->setVertices(vertices);

    p_stdDev1Curve->setViewPort(cvp);
    p_stdDev1Curve->setVertices(vertices);

    p_stdDev2Curve->setViewPort(cvp);
    p_stdDev2Curve->setVertices(vertices);

    if(vertices.size() < 1) return;

    double ss, sl, es, el, x, y;
    std::vector <int> x_contained, y_contained;

    // Convert them to line sample values
    cvp->viewportToCube(vertices[0].x(), vertices[0].y(), ss, sl);
    cvp->viewportToCube(vertices[2].x(), vertices[2].y(), es, el);

    ss = ss + 0.5;
    sl = sl + 0.5;
    es = es + 0.5;
    el = el + 0.5;

    int samps = (int)(es - ss + 1);
    if(samps < 1) samps = 1;
    Cube *cube = cvp->cube();
    Brick *brick = new Brick(*cube, samps, 1, 1);
    Pvl &pvl = *cvp->cube()->getLabel();

    if(RubberBandTool::getMode() == RubberBandTool::Polygon) {
      samps = 1;
      geos::geom::CoordinateSequence *pts = new geos::geom::CoordinateArraySequence();
      for(int i = 0; i < vertices.size(); i++) {
        cvp->viewportToCube(vertices[i].x(), vertices[i].y(), x, y);
        pts->add(geos::geom::Coordinate((int)x, (int)y));
      }/*end for*/

      /*Add the first point again in order to make a closed line string*/
      cvp->viewportToCube(vertices[0].x(), vertices[0].y(), x, y);
      pts->add(geos::geom::Coordinate((int)x, (int)y));

      p_poly = globalFactory.createPolygon
               (globalFactory.createLinearRing(pts), NULL);

      p_envelope = p_poly->getEnvelopeInternal();

      for(int y = (int)floor(p_envelope->getMinY()); y <= (int)ceil(p_envelope->getMaxY()); y++) {
        for(int x = (int)floor(p_envelope->getMinX()); x <= (int)ceil(p_envelope->getMaxX()); x++) {
          geos::geom::Coordinate c(x, y);
          geos::geom::Point *p = globalFactory.createPoint(c);
          bool contains = p->within(p_poly);
          delete p;

          if(contains) {
            x_contained.push_back(x);
            y_contained.push_back(y);
          }

        } /*end x*/
      }/*end y*/
    }


    for(int band = 1; band <= cube->getBandCount(); band++) {
      Statistics stats;

      /*Rectangle*/
      if(RubberBandTool::getMode() == RubberBandTool::Rectangle) {
        for(int line = (int)std::min(sl, el); line <= (int)std::max(sl, el); line++) {
          brick->SetBasePosition((int)ss, line, band);
          cube->read(*brick);
          stats.AddData(brick->DoubleBuffer(), samps);
          //if(*brick->DoubleBuffer() == Null) {
          //if(*brick->DoubleBuffer() == NULL) {
          //if(*brick->DoubleBuffer() < 0) {
          // std::cout << "band = " << band << "  here" << std::endl;
          // stats.AddData(0);
          //} else {
          //stats.AddData(brick->DoubleBuffer(),samps);
          // }

        } /*end for*/
      } /*end if Rectangle*/


      /*Polygon*/
      if(RubberBandTool::getMode() == RubberBandTool::Polygon) {

        for(unsigned int j = 0; j < x_contained.size(); j++) {

          brick->SetBasePosition(x_contained[j], y_contained[j], band);
          cube->read(*brick);
          stats.AddData(*brick->DoubleBuffer());

        }

      }/*end if Polygon*/

      if(plotType->currentText() == "Band Number") {
        labels.push_back(band);
      }
      else {
        if(pvl.FindObject("IsisCube").HasGroup("BandBin")) {
          PvlGroup &bandBin = pvl.FindObject("IsisCube").FindGroup("BandBin");
          if(bandBin.HasKeyword("Center")) {
            PvlKeyword &wavelength = bandBin.FindKeyword("Center");
            if(wavelength.Size() > (band - 1)) {
              labels.push_back(wavelength[band-1]);
            }
          }
        }
      }

      if(stats.Average() == Null) {
        //stats.Reset();
        //stats.AddData(0);
        data.push_back(stats);

        /*if (QMessageBox::information(p_plotToolWindow,"Error",
                                     "The selected Area contains no valid pixels",
                                     QMessageBox::Ok) == QMessageBox::Ok) {

          delete brick;
          labels.clear();
          data.clear();
          return;

        }*/
      }
      else {   // end if stats.Average() == null

        data.push_back(stats);
      }
    } /*end for bands*/

    if(RubberBandTool::getMode() == RubberBandTool::Polygon) delete p_poly;
    delete brick;

  }


  /**
   *
   *
   * @param labels
   * @param data
   * @param xmax
   */
  void PlotTool::getSpatialStatistics(std::vector<double> &labels,
                                      std::vector<double> &data, double &xmax,
                                      MdiCubeViewport * cvp) {
    QList<QPoint> vertices = RubberBandTool::getVertices();
    double ss, sl, es, el;

    if(!cvp) return;

    // Convert them to line sample values
    cvp->viewportToCube(vertices[0].x(), vertices[0].y(), ss, sl);
    cvp->viewportToCube(vertices[1].x(), vertices[1].y(), es, el);

    p_dnCurves[p_dnCurves.size() - 1]->setViewPort(cvp);
    p_dnCurves[p_dnCurves.size() - 1]->setVertices(vertices);

    ss = ss + 0.5;
    sl = sl + 0.5;
    es = es + 0.5;
    el = el + 0.5;

    Interpolator interp;

    if(p_cubicInterp->isChecked()) {
      interp.SetType(Interpolator::CubicConvolutionType);
    }
    else if(p_bilinearInterp->isChecked()) {
      interp.SetType(Interpolator::BiLinearType);
    }
    else {
      interp.SetType(Interpolator::NearestNeighborType);
    }

    Portal dataReader(interp.Samples(), interp.Lines(),
                      cvp->cube()->getPixelType());

    int lineLength = (int)(sqrt(pow(ss - es, 2) + pow(sl - el, 2)) + 0.5); //round to the nearest pixel increment
    int band = ((cvp->isGray()) ? cvp->grayBand() : cvp->redBand());
    p_plotToolWindow->setAxisLabel(QwtPlot::xBottom, "Data Point");
    xmax = lineLength;

    if(RubberBandTool::getMode() == RubberBandTool::Line) {
      for(int index = 0; index < lineLength; index++) {
        double x = (index / (double)lineLength) * (es - ss) + ss; // % across * delta x + initial = x position of point
        x -= (interp.Samples() / 2.0); // move back for interpolation
        double y = (index / (double)lineLength) * (el - sl) + sl;
        y -= (interp.Lines() / 2.0); // move back for interpolation

        dataReader.SetPosition(x, y, band);
        cvp->cube()->read(dataReader);
        double result = interp.Interpolate(x, y, dataReader.DoubleBuffer());

        if(!IsSpecial(result)) {
          labels.push_back(index + 1);
          data.push_back(result);
        }
      }
    }
    // If its not a line, it must be a rotated rect...
    else {
      double es2, el2;

      // Convert them to line sample values
      cvp->viewportToCube(vertices[3].x(), vertices[3].y(), es2, el2);

      es2 = es2 + 0.5;
      el2 = el2 + 0.5;

      // these are for walking across the rotated rect
      int numStepsAcross = (int)(sqrt(pow(ss - es2, 2) + pow(sl - el2, 2)) + 0.5); //round to the nearest pixel increment;
      double deltaX = (1.0 / (double)numStepsAcross) * (es2 - ss);
      double deltaY = (1.0 / (double)numStepsAcross) * (el2 - sl);

      // walk the "green" line on the screen
      for(int index = 0; index < lineLength; index++) {
        Statistics lineStats;
        double x = (index / (double)lineLength) * (es - ss) + ss; // % across * delta x + initial = x position of point
        x -= (interp.Samples() / 2.0); // move back for interpolation
        double y = (index / (double)lineLength) * (el - sl) + sl;
        y -= (interp.Lines() / 2.0); // move back for interpolation

        // x/y are now the centered on the appropriate place of the green line, i.e. the start of our walk across the rectangle
        for(int walkIndex = 0; walkIndex < numStepsAcross; walkIndex++) {
          dataReader.SetPosition(x, y, band);
          cvp->cube()->read(dataReader);
          double result = interp.Interpolate(x, y, dataReader.DoubleBuffer());

          if(!IsSpecial(result)) {
            lineStats.AddData(result);
          }

          x += deltaX;
          y += deltaY;
        }

        if(!IsSpecial(lineStats.Average())) {
          labels.push_back(index + 1);
          data.push_back(lineStats.Average());
        }
      }
    }
  }


  /**
   * This triggers when cubic interpolation is enabled or disabled
   *   Since enabling another interpolation will cause cubic to
   *   become unchecked, if p_changingInterp is true then we'll
   *   simply exit. Replot the data if we can.
   */
  void PlotTool::cubicInterpolationChanged() {
    if(p_changingInterp && p_cubicInterp) return;

    p_changingInterp = true;
    p_cubicInterp->setChecked(true);
    p_bilinearInterp->setChecked(false);
    p_nearestNeighborInterp->setChecked(false);
    p_changingInterp = false;

    if(RubberBandTool::isValid()) {
      changePlot();
    }
  }


  /**
   * This triggers when bilinear interpolation is enabled or disabled.
   *   Since enabling another interpolation will cause bilinear to become
   *   unchecked, if p_changingInterp is true then we'll simply exit. Replot the
   *   data if we can.
   */
  void PlotTool::bilinearInterpolationChanged() {
    if(p_changingInterp && p_cubicInterp) return;
    p_changingInterp = true;
    p_cubicInterp->setChecked(false);
    p_bilinearInterp->setChecked(true);
    p_nearestNeighborInterp->setChecked(false);
    p_changingInterp = false;

    if(RubberBandTool::isValid()) {
      changePlot();
    }
  }


  /**
   * This triggers when nearest neighbor interpolation is enabled
   *   or disabled. Since enabling another interpolation will
   *   cause nearest neighbor to become unchecked, if
   *   p_changingInterp is true then we'll simply exit. Replot the
   *   data if we can.
   */
  void PlotTool::nearestInterpolationChanged() {
    if(p_changingInterp && p_cubicInterp) return;

    p_changingInterp = true;
    p_cubicInterp->setChecked(false);
    p_bilinearInterp->setChecked(false);
    p_nearestNeighborInterp->setChecked(true);
    p_changingInterp = false;

    if(RubberBandTool::isValid()) {
      changePlot();
    }
  }


  /**
   * This method paints the polygons of the copied curves
   * onto the cubeviewport
   *
   * @param vp
   * @param painter
   */
  void PlotTool::paintViewport(MdiCubeViewport *vp, QPainter *painter) {
    int sample1, line1, sample2, line2;

    // loop thru the window list
    for(int i = 0; i < p_plotWindows.size(); i++) {

      for(int c = 0; c < p_plotWindows[i]->getNumCurves(); c++) {
        /*get all curves in current window*/
        PlotToolCurve *curve = (PlotToolCurve *)p_plotWindows[i]->getPlotCurve(c);
        if(curve->getViewPort() == vp) {
          QPen pen(curve->pen().color());
          pen.setWidth(curve->pen().width());
          pen.setStyle(curve->pen().style());
          painter->setPen(pen);
          QList <QPointF> points = curve->getVertices();

          for(int p = 1; p < points.size(); p++) {
            vp->cubeToViewport(points[p-1].x(), points[p-1].y(), sample1, line1);
            vp->cubeToViewport(points[p].x(), points[p].y(), sample2, line2);
            painter->drawLine(QPoint(sample1, line1), QPoint(sample2,  line2));
          }

          vp->cubeToViewport(points[points.size()-1].x(),
                             points[points.size()-1].y(), sample1, line1);
          vp->cubeToViewport(points[0].x(), points[0].y(), sample2, line2);
          painter->drawLine(QPoint(sample1, line1), QPoint(sample2,  line2));

        }
      }
    }

  }


  /**
   * This method causes the view port corresponding with the given
   * PlotToolCurve to be repainted with all of the area's of
   * interest associated with the PlotToolCurve's plotwindow.
   * The paintViewport() method is called.
   * @param pc
   */
  void PlotTool::updateViewPort(PlotToolCurve *pc) {
    pc->getViewPort()->repaint();
  }

  /**
   * This overloaded method is called to repaint the current view
   * port.  The paintViewport() method is called.
   */
  void PlotTool::updateViewPort() {
    p_cvp->viewport()->repaint();
  }
  
}

