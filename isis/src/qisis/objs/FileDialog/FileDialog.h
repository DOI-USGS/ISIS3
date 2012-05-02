#ifndef FileDialog_h
#define FileDialog_h
#include <QFileDialog>
#include <QtGui>
#include <QSettings>
#include "FileName.h"
#include "FileTool.h"

namespace Isis {
  /**
   * @brief Class for browsing cubes.
   *
   * @ingroup Visualization Tools
   *
   * @author ????-??-?? Stacy Alley
   *
   * @internal
   *   @history 2008-01-18 Stacy Alley - Changed the constructor to
   *                           accept a QStringList which serves as the default
   *                           filters for the file dialog boxes.
   *   @history 2008-01-28 Stacy Alley - Changed the constructor
   *                           again to accept a QDir which is the default
   *                           directory the file dialog box should point to.
   *   @history 2011-07-28 Steven Lambright - Now connects to the buttons'
   *                           clicked signals instead of pressed, clicked is
   *                           the appropriate signal. Fixes #270
   */
  class FileDialog : public QFileDialog {
      Q_OBJECT
    public:
      FileDialog(QString title, QStringList &filterList, QDir &directory, QWidget *parent = 0);
      void closeEvent(QCloseEvent *event);
      void readSettings();
      void writeSettings();

    protected:
      bool eventFilter(QObject *o, QEvent *e);

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
