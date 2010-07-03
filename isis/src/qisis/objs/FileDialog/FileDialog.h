#ifndef FileDialog_h
#define FileDialog_h
#include <QFileDialog>
#include <QtGui>
#include <QSettings>
#include "Filename.h"
#include "FileTool.h"

namespace Qisis {
   /**
  * @brief Class for browsing cubes.
  *
  * @ingroup Visualization Tools
  *
  * @author Stacy Alley
  *
  * @internal
  *  
   * @history 2008-01-18 Stacy Alley - Changed the constructor to
   * accept a QStringList which serves as the default filters for
   * the file dialog boxes.
   * 
   * @history 2008-01-28 Stacy Alley - Changed the constructor
   *          again to accept a QDir which is the default directory
   *          the file dialog box should point to.
  */
 

  class FileDialog : public QFileDialog {
    Q_OBJECT
    public:
      FileDialog (QString title, QStringList &filterList, QDir &directory, QWidget *parent=0);
      void closeEvent(QCloseEvent *event);
      void readSettings();
      void writeSettings();
      
    protected:
      bool eventFilter(QObject *o,QEvent *e);

    protected slots:
      void sendSignal();
      void done();
      void cancel();
      void saveFilter();
      
    signals:
      void fileSelected(QString);
      void filterSelected(QString);

    private:
      QList<QComboBox *> p_comboBoxes;
      QList<QPushButton *> p_allPButtons;
      std::string p_appName;
      QDialog *p_mainDialog;
      QWidget *p_parent;
      QPushButton *p_filterButton;
      QLineEdit *p_filterLine;
      QStringList &p_filterList;
      QDir &p_dir;
      //QStringList &p_fileList;

  };
};

#endif
