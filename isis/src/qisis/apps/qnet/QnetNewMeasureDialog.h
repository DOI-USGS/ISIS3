#ifndef QnetNewMeasureDialog_h
#define QnetNewMeasureDialog_h

#include <QDialog>

class QLabel;
class QListWidget;
class QPushButton;

#include "ControlPoint.h"

#include <vector>

using namespace std;
namespace Qisis {
  /**
   *
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null in constructor.  Removed "std::"
   *                          since "using namespace std" in
   *                          header and .cpp files.
   *
   */

  class QnetNewMeasureDialog : public QDialog {
      Q_OBJECT

    public:
      QnetNewMeasureDialog(QWidget *parent = 0);
      void SetFiles(const Isis::ControlPoint &point,
                    vector<string> &pointFiles);

      QListWidget *fileList;

    private:

      QPushButton *p_okButton;

      vector<string> *p_pointFiles;

    private slots:
      void enableOkButton(const QString &text);

  };
};

#endif


