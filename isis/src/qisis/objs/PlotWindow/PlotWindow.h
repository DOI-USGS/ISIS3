#ifndef PlotWindow_h
#define PlotWindow_h

/**                                                                       
 * @file                                                                  
 * $Revision: 1.17 $                                                             
 * $Date: 2009/04/07 16:19:40 $
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

#include <QtGui>
#include <Qt>
#include "Workspace.h"
#include "Tool.h"
#include "PlotCurve.h"
#include "PlotToolCurve.h"
#include "MainWindow.h"
#include "TableMainWindow.h"
#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_plot_zoomer.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qmainwindow.h>

class QDockWidget;
class QListWidgetItem;

namespace Qisis {
  class PlotWindow : public Qisis::MainWindow {
    Q_OBJECT

    public:
      PlotWindow(QString title,QWidget *parent);
      void add(PlotCurve *pc);
      void clearPlotCurves(bool keepScale = true);
      QwtText getAxisLabel(int axisId);
      int getNumCurves();
      QColor getPlotBackground();
      QwtText getPlotTitle();
      void setAxisLabel(int axisId, QString title);
      void setAutoScaleAxis(int axisId);
      void setPlotBackground(QColor c);
      void setPlotTitle(QString pt);
      void setScale(int axisId, double minimum, double maximum, double stepSize = 0); 
      void setScaleDiv(int axisId, QwtScaleDiv scaleDiv); 
      void showWindow();
      void setupDefaultMenu();
      void setCustomMenu(QList<QMenu *> &menu, QList<QAction *> &actions);
      void getDefaultMenus(QList<QMenu *> &menu, QList<QAction *> &actions); 

      /**
       * 
       * 
       * 
       * @return int 
       */
      int getPlotCurveCount() { return p_plotCurves.size(); }
      /**
       *  Reuturns the plot curve at the given index.
       * 
       * @param index 
       * 
       * @return QwtPlotCurve* 
       */
      QwtPlotCurve *getPlotCurve(int index) { return p_plotCurves[index]; }

      double p_xMin;//!< X minimum
      double p_xMax;//!< X maximum
      double p_yMin;//!< Y minimum
      double p_yMax;//!< Y maximum
      static bool p_curveCopied;//!< Has a curve been copied?

    protected:
      QwtPlot *p_plot;//!< The plot in this window
      //! Returns the name of the menu.
      QString menuName() const { return "&Options"; };
      bool eventFilter(QObject *o, QEvent *e);
      Qisis::TableMainWindow *p_tableWindow;//!< Table window
      QList <PlotCurve*> p_plotCurves; //!< List of the plot curves in this window
      QToolBar *p_toolBar;//!< Tool bar on the plot window
      std::string p_header;//!< Header name
      std::string p_axisTitle;//!< Header name and axis title
      Qisis::MainWindow  *p_mainWindow;
         
    protected slots:
      void cancelConfig();
      void copyCurve();
      void pasteCurve();
      void pasteCurveSpecial();
      void createWindow();   
      void createConfigDialog();
      void createLegendMenu();
      void deleteFromTable();
      void fillInValues(int title);
      void saveProperties();
      void colorSelect();
      void deleteCurve();
      void writeSettings();
      void readSettings();
  
    signals:
      /**
       *  Emitted when a curve has been copied.
       * 
       * @param pc 
       */
      void curveCopied(Qisis::PlotCurve *pc);
      /**
       *  Emitted when a curve has been requested to be pasted.
       * 
       * @param pw 
       */
      void curvePaste(Qisis::PlotWindow *pw);
      /**
       * Emitted when a curve needs to be pasted with a new random 
       * color. 
       * 
       * @param pw 
       */
      void curvePasteSpecial(Qisis::PlotWindow *pw);
      //! Emitted every time there is a change to the plot window.
      void plotChanged();

    public slots:
      void clearPlot();
      void configPlot();
      void setDeletable(bool d=true);
      void setPasteable(bool paste=true);
      void setCopyEnable(bool c = true);
      void setDestroyOnClose(bool destroy = false);
      virtual void fillTable();
      void printPlot();
      void reLabel();
      void savePlot();
      void setDefaultRange();
      void setLabels();
      void setUserValues();
      void changeTitle(QString s);
      void changeCurveSize(int size);
      void changeCurveStyle(int style);
      void changeMarkerStyle(int style);
      void hideAllSymbols();
      void showCurve();
      void hideAllCurves();
      void resetScale();
      void showGrid();
      void showHelp();
      void showSymbols();
      void showTable();
      void switchBackground();   
      void trackerEnabled();
           
  private:
      QWidget       *p_parent;//!< Parent widget
      QDialog       *configDialog;//!<Dialog box to configure the entire plot window and curves.
      QPen          *p_pen;//!<Plot window's pen.
      QwtPlotZoomer *p_zoomer;//!< Plot Zoomer
      QwtLegend     *p_legend;//!< Plot legend
      QwtPlotGrid   *p_grid;//!< Plot grid lines
      QLineEdit     *p_plotTitleText;//!< Plot title line edit for the config dialog.
      QLineEdit     *p_xAxisText;//!< X-axis line edit for the config dialog.
      QLineEdit     *p_yAxisText;//!< Y-axis line edit for the config dialog.
      QLineEdit     *p_xMinEdit;//!< X min. line edit for the config dialog.
      QLineEdit     *p_xMaxEdit;//!< X max. line edit for the config dialog.
      QLineEdit     *p_yMinEdit;//!< Y min. line edit for the config dialog.
      QLineEdit     *p_yMaxEdit;//!< Y max. line edit for the config dialog.
      QLineEdit     *p_titleLineEdit;//!< Table title edit for the config dialog.
      QPushButton   *p_colorButton;//!< Color button for the config dialog.   
      QPushButton   *p_hideButton;//!< Hide/show button for the config dialog. 
      QPushButton   *p_symbolsButton;//!< Hide/show symbols button for the config dialog.
      QPushButton   *p_deleteButton;//!< Delete curve button for the config dialog.
      QComboBox     *p_styleBox;//!< Line style combo box for the config dialog.
      QComboBox     *p_sizeBox;//!< Line size combo box for the config dialog.
      QComboBox     *p_symbolBox;//!< Symbol style combo box for the config dialog.
      QComboBox     *p_titleBox;//!< Curve selection combo box for the config dialog.
      QCheckBox     *p_xLogCheckBox;
      QCheckBox     *p_yLogCheckBox;
      
      QMenuBar      *p_menubar;//!< Plot window's menu bar.
        
  
      QAction *p_action;//!< Plot window's action. 
      QAction *p_hideAllCurves;//!< Hide all curves action.
      QAction *p_hideAllMarkers;//!< Hide all markers action.
      QAction *p_gridShow;//!< Show plot grid lines action.
      QAction *p_pasteCurve;//!< Paste curve action.
      QAction *p_pasteSpecial;//!< Paste spectial action.
      QAction *p_deleteCurve;//!< Delete curve action.
      QAction *p_copyCurveAction;//!< Copy curve action.
      QAction *p_hideShowCurve;//!< Hide/show curve action.

      QStringList p_curveTitleList;//!< String list of all curves in the window.
  
      int p_selected;//!< The index of the selected curve.

      bool p_deletable;//!< Is the curve delete-able?
      bool p_pasteable;//!< Is the window paste-able?
      bool p_symbols;//!< Is the window showing the curve markers?
      bool p_curvePropsSaved;//!< Have the curve properties been saved?
      bool p_copyable;//!< Is this curve copy-able?
      bool p_destroyOnClose;//!< Should the curve be destroyed on close?
      bool p_scaled;//!< Has this window been rescaled?
      bool p_xLogScale;
      bool p_yLogScale;

      static PlotCurve *p_dragCurve;//!< The curve to drag to a new window.
  
      QObject *p_eventObject;//!< The object that just had an event happen.
      QMenu *p_legendMenu;//!< Context menu for the plot legend.

  };
};

#endif

