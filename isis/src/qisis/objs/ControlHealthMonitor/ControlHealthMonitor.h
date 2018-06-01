#ifndef ControlHealthMonitor_h
#define ControlHealthMonitor_h
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

 #include "AbstractProjectItemView.h"
#include <QLabel>
#include <QTableWidget>
#include <QProgressBar>
#include <QPointer>
#include <NetworkVitals.h>



namespace Isis {

  /**
   * Interface that allows real-time evaluation of the state of a Control Network.
   *
   * @author 2018-05-28 Adam Goins
   *
   * @internal
   *   @history 2018-05-28 Adam Goins - Initial Creation.
   */
  class ControlHealthMonitor : public QWidget {


    Q_OBJECT

    public:
      ControlHealthMonitor(NetworkVitals *vitals, QWidget *parent=0);
      ~ControlHealthMonitor();
      void createGui();
      QWidget* createImagesTab();
      QWidget* createPointsTab();
      QWidget* createOverviewTab();
      void initializeEverything();

    public slots:
      void viewPointAll();
      void viewPointIgnored();
      void viewPointEditLocked();
      void viewPointFewMeasures();

      void viewImageAll();
      void viewImageFewMeasures();
      void viewImageHullTolerance();

      void breakNet();
      void weak();
      void healthy();
      void update();


    private:
        void updateStatus(int code);
        void updateImageTable(QList<QString> serials);
        void updatePointTable(QList<ControlPoint*> points);

        NetworkVitals *m_vitals;

        QProgressBar *m_statusBar;

        QLabel *m_sizeLabel;
        QLabel *m_numImagesLabel;
        QLabel *m_numPointsLabel;
        QLabel *m_numMeasuresLabel;
        QLabel *m_lastModLabel;

        QLabel *m_imagesMeasuresValue;
        QLabel *m_imagesHullValue;
        QLabel *m_imagesShowingLabel;
        QLabel *m_statusLabel;
        QLabel *m_statusDetails;

        QLabel *m_pointsIgnoredLabel;
        QLabel *m_pointsEditLockedLabel;
        QLabel *m_pointsFewMeasuresLabel;
        QLabel *m_pointsShowingLabel;

        QTableWidget *m_historyTable;
        QTableWidget *m_imagesTable;
        QTableWidget *m_pointsTable;

        QStringList *activeImageList;
        QStringList *activePointsList;
  };
}

#endif
