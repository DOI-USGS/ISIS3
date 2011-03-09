#ifndef QnetGroundPointDialog_h
#define QnetGroundPointDialog_h

#include <QDialog>

class QLabel;
class QLineEdit;
class QListWidget;
class QRadioButton;
class QPushButton;
class QString;
class QStringList;

namespace Isis {
  class ControlPoint;
}


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
  class QnetGroundPointDialog : public QDialog {
    Q_OBJECT

    public:
      static QString lastPtIdValue;

      QnetGroundPointDialog (QWidget *parent=0);

      QLineEdit *ptIdValue;
      void SetFiles (QStringList &pointFiles);

      QListWidget *fileList;

    signals:
      void groundPoint (Isis::ControlPoint &point);
      
    private:

      QRadioButton *p_avg;
      QRadioButton *p_select;

      QLabel *p_ptIdLabel;
      QPushButton *p_okButton;

      QStringList *p_pointFiles;

    private slots:
      void enableOkButton(const QString &text);

  };
};

#endif

