#ifndef MatchToolNewPointDialog_h
#define MatchToolNewPointDialog_h

#include <QDialog>

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QString;
class QStringList;

namespace Isis {
  class ControlNet;
  /**
   * @author 2012-05-04 Tracie Sucharski - Adapted from QnetMatchToolNewPointDialog 
   *  
   * @internal
   */
  class MatchToolNewPointDialog : public QDialog {

      Q_OBJECT

    public:
      static QString lastPtIdValue;

      MatchToolNewPointDialog(const ControlNet &cnet, QWidget *parent = 0);

      QLineEdit *ptIdValue;

      void setFiles(QStringList pointFiles);
      void highlightFile(QString file);

      QListWidget *fileList;

    signals:
      void measuresFinished();
      void newPointCanceled();

    private slots:
      void enableDoneButton(const QString &text);

    private:
      QLabel *m_ptIdLabel;
      QPushButton *m_doneButton;

      QStringList *m_pointFiles;


  };
};

#endif


