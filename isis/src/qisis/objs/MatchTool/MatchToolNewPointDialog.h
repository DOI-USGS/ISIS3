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
   *   @history 2012-10-03 Tracie Sucharski - Remove static pointId value.  Instead, pass into
   *                          costructor from location of instantiation.
   */
  class MatchToolNewPointDialog : public QDialog {

      Q_OBJECT

    public:
      MatchToolNewPointDialog(const ControlNet &cnet, QString defaultPointId, QWidget *parent = 0);

      QLineEdit *ptIdLineEdit;
      QString pointId() const;

      void setFiles(QStringList pointFiles);
      void highlightFile(QString file);


    signals:
      void measuresFinished();
      void newPointCanceled();

    private slots:
      void enableDoneButton(const QString &text);

    private:
      QListWidget *m_fileList;

      QPushButton *m_doneButton;

      QStringList *m_pointFiles;


  };
};

#endif


