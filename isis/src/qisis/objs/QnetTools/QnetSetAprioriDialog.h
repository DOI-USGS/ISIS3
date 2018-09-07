#ifndef QnetSetAprioriDialog_h
#define QnetSetAprioriDialog_h

#include <QDialog>


//forward declarations
class QDialog;
class QGridLayout;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QStackedWidget;
class QString;
class QStringList;
class QVBoxLayout;


namespace Isis {
  class QnetTool;

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2016-08-09 Makayla Shepherd - Complete redesign of the user interface.
   *                           Fixes #2325, #2383.
   *   @history 2016-10-14 Makayla Shepherd - Fixed an issue that caused the apriori sigmas to be
   *                           set to NULL. You can now set the apriori sigmas. Fixes #4457.
   *   @history 2016-11-18 Makayla Shepherd - Corrected the deletion of this dialog. The incorrect
   *                           deletion caused an error message to pop up when selecting multiple
   *                           Free or Fixed points after closing the Set Apriori dialog. Fixes
   *                           #4490.
   *   @history 2018-06-28 Debbie A. Cook - Removed calls to SurfacePoint::SetRadii
   *                           which is now obsolete. Fixes #5457.
   *   @history 2018-07-06 Jesse Mapel - Removed calls to ControlNet::GetTargetRadii because it is
   *                           both no longer needed and no longer available. Fixes #5457.
   */
  class QnetSetAprioriDialog : public QDialog {
      Q_OBJECT

    public:
      QnetSetAprioriDialog(QnetTool *qnetTool, QWidget *parent = 0);
      void setPoints(QList<QListWidgetItem *> selectedPoints);


    public slots:
      void setVisiblity();
      virtual void reject();

    signals:
      void pointChanged(QString pointId);
      void netChanged();
      void aprioriDialogClosed();

    private slots:
      void fillCurrentAprioriLineEdits();
      void fillReferenceAprioriLineEdits();
      void fillAverageAprioriLineEdits();
//      void fillGroundSourceAprioriLineEdits();
      void fillSigmaLineEdits();
      void clearLineEdits();
      void resetInfoLabels();
      void setApriori();
      void closeEvent();


    private:

      void createSetAprioriDialog(QWidget *parent);
      void setInfoStack(QList<QListWidgetItem *> selectedPoints);
      void checkPointInfoDisable(QList<QListWidgetItem *> selectedPoints);

      QDialog *m_aprioriDialog;
      QGridLayout *m_aprioriGridLayout;
      QPushButton *m_okButton;
      QPushButton *m_cancelButton;
      QPushButton *m_applyButton;
      QStackedWidget *m_pointInfoStack;

      QGroupBox *m_singlePointInfoGroup;
      QLabel *m_pointIDLabel;
      QLabel *m_pointTypeLabel;
      QLabel *m_pointMeasureNumber;
      QLabel *m_editLockedBoolLabel;
      QLabel *m_ignoredBoolLabel;

      QGroupBox *m_multiplePointsInfoGroup;
      QLabel *m_pointsCount;
      QLabel *m_pointsMeasuresCount;
      QLabel *m_constrainedPointsCount;
      QLabel *m_fixedPointsCount;
      QLabel *m_freePointsCount;
      QLabel *m_pointsEditLockedCount;
      QLabel *m_pointsIgnoredCount;

      QGroupBox *m_pointGroup;
      QLabel *m_aprioriLatLabel;
      QLabel *m_aprioriLonLabel;
      QLabel *m_aprioriRadiusLabel;
      QLineEdit *m_latLineEdit;
      QLineEdit *m_lonLineEdit;
      QLineEdit *m_radiusLineEdit;
      QPushButton *m_currentAprioriButton;
      QPushButton *m_referenceAprioriButton;
      QPushButton *m_averageAprioriButton;

      QGroupBox *m_sigmaGroup;
      QLabel *m_sigmaWarningLabel;
      QPushButton *m_currentSigmaButton;
      QLabel *m_latSigmaLabel;
      QLabel *m_lonSigmaLabel;
      QLabel *m_radiusSigmaLabel;
      QLineEdit *m_latSigmaLineEdit;
      QLineEdit *m_lonSigmaLineEdit;
      QLineEdit *m_radiusSigmaLineEdit;

      QList<QListWidgetItem *> m_points;
      QnetTool *m_qnetTool;

      enum Source {
        USER,
        AVERAGE,
        REFERENCE,
      };

      Source m_aprioriSource;

      int m_multiPointsMeasureCount;
      int m_multiPointsConstraintedCount;
      int m_multiPointsFixedCount;
      int m_multiPointsFreeCount;
      int m_multiPointsEditLockedCount;
      int m_multiPointsIgnoredCount;
  };
}

#endif
