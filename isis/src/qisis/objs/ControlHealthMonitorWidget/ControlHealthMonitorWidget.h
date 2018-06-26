#ifndef ControlHealthMonitorWidget_h
#define ControlHealthMonitorWidget_h
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

#include <QLabel>
#include <QTableWidget>
#include <QProgressBar>
#include <QPointer>
#include <ControlNetVitals.h>



namespace Isis {
  /**
   * Interface that allows real-time evaluation of the state of a Control Network.
   *
   * @author 2018-05-28 Adam Goins
   *
   * @internal
   *   @history 2018-05-28 Adam Goins - Initial Creation.
   */
  class ControlHealthMonitorWidget : public QWidget {


    Q_OBJECT

    public:
      ControlHealthMonitorWidget(ControlNetVitals *vitals, QWidget *parent=0);
      ~ControlHealthMonitorWidget();
      void createGui();
      QWidget* createImagesTab();
      QWidget* createPointsTab();
      QWidget* createOverviewTab();
      QWidget* createGraphTab();
      void setVitals(ControlNetVitals *vitals);
      void initializeEverything();

    public slots:
      void emitOpenImageEditor();
      void emitOpenPointEditor();

      void historyEntry(QString, QString, QVariant, QVariant, QString);

      void viewPointAll();
      void viewPointFree();

      void viewPointFixed();
      void viewPointConstrained();

      void viewPointIgnored();
      void viewPointEditLocked();
      void viewPointFewMeasures();

      void viewImageAll();
      void viewImageFewMeasures();
      void viewImageHullTolerance();

      void update();

    signals:
      void openPointEditor(ControlPoint *point);
      void openImageEditor(QList<QString> serials);

    private:
        void updateStatus(int code);
        void updateImageTable(QList<QString> serials);
        void updatePointTable(QList<ControlPoint*> points);

        // QChartView *m_pointChartView;
        ControlNetVitals *m_vitals;
        QProgressBar *m_statusBar;
        QProgressBar *m_pointsFreeProgressbar;
        QProgressBar *m_pointsConstrainedProgressbar;
        QProgressBar *m_pointsFixedProgressbar;


        QTableWidget *m_historyTable;
        QTableWidget *m_imagesTable;
        QTableWidget *m_pointsTable;

        QLabel *m_imagesHullValue;
        QLabel *m_imagesMeasuresValue;
        QLabel *m_imagesShowingLabel;
        QLabel *m_lastModLabel;
        QLabel *m_netLabel;
        QLabel *m_numImagesLabel;
        QLabel *m_numMeasuresLabel;
        QLabel *m_numPointsLabel;
        QLabel *m_pointsConstrainedLabel;
        QLabel *m_pointsEditLockedLabel;
        QLabel *m_pointsFewMeasuresLabel;
        QLabel *m_pointsFixedLabel;
        QLabel *m_pointsFreeLabel;
        QLabel *m_pointsIgnoredLabel;
        QLabel *m_pointsShowingLabel;
        QLabel *m_statusDetails;
        QLabel *m_statusLabel;

  };
}

#endif
