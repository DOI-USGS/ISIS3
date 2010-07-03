#ifndef QHistogram_h
#define QHistogram_h
/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2008/06/19 18:43:47 $
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch 
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website, 
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */                       

 #include "Histogram.h"
 #include <QWidget>
 #include <QMenu>
 #include <qwt_plot.h>
 #include <qwt_plot_zoomer.h>
 #include <qwt_plot_curve.h>

namespace Qisis {
/**                                                                       
 * @brief Plot Histograms
 *                                                                        
 * This class is used to plot histograms.  It is a utility class for the hist
 * application.
 * 
 * @ingroup Utility                                                  
 *                                                                        
 * @author 2006-12-21 Elizabeth Miller
 *                                                                        
 * @internal
 */                                                                       
  class QHistogram: public QwtPlot {
    Q_OBJECT

      public:
        QHistogram (QWidget *parent=NULL);

        //! Destroys the QHistogram object
        ~QHistogram () {};

        void Load(Isis::Histogram &hist);

      public slots:
        void trackerEnabled();
        void cdfCurveVisible();
        void printPlot();
        void savePlot();
        void switchBackground();
        void showHelp();

      private:
        QwtPlotZoomer *p_zoomer; //!< Plot Zoomer
        QwtPlotCurve *p_histCurve; //!< Historgram plot curve
        QwtPlotCurve *p_cdfCurve; //!< CDF plot curve
  };
};
  
#endif
