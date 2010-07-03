#ifndef QnetNewPointDialog_h
#define QnetNewPointDialog_h

#include <QDialog>
#include <QString>

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;

#include <vector>
using namespace std;

namespace Qisis {
  /**
   * @internal
   *   @history 2008-11-26 Jeannie Walldren - Added functionality
   *                          to show the last Point ID entered
   *                          into a new point dialog box.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          in constructor.  Removed "std::" in
   *                          header and .cpp files.
   */
  class QnetNewPointDialog : public QDialog {
      Q_OBJECT


    public:
      static QString lastPtIdValue;

      QnetNewPointDialog(QWidget *parent = 0);

      QLineEdit *ptIdValue;
      void SetFiles(vector<string> &pointFiles);

      QListWidget *fileList;

    private:

      QLabel *p_ptIdLabel;
      QPushButton *p_okButton;

      vector<string> *p_pointFiles;

    private slots:
      void enableOkButton(const QString &text);

  };
};

#endif


