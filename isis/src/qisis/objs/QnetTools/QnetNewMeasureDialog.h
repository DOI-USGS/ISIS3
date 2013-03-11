#ifndef QnetNewMeasureDialog_h
#define QnetNewMeasureDialog_h

#include <QDialog>

class QListWidget;
class QPushButton;
class QString;
class QStringList;

namespace Isis {
  class ControlPoint;
  class QnetTool;

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          to null in constructor.  Removed "std::"
   *                          since "using namespace std" in
   *                          header and .cpp files.
   *   @history 2010-10-28 Tracie Sucharski - Fixed some include problems caused
   *                          by changes made to the ControlNet, ControlPoint,
   *                          ControlMeasure header files.  Changed vector of
   *                          std::strings to a QStringList.
   *
   *   @history 2010-12-02 Eric Hyer - Removed "using namespace std" from
   *       header file!!!  Also removed non-parent includes.  Eliminated
   *       the m_pointFiles variable.  SetFiles now puts Selected items on top.
   */
  class QnetNewMeasureDialog : public QDialog {

      Q_OBJECT

    public:
      QnetNewMeasureDialog(QnetTool *qnetTool, QWidget *parent = 0);

      QStringList selectedFiles() const;
      void setFiles(ControlPoint point, QStringList pointFiles);

    private slots:
      void enableOkButton(const QString &text);

    private:
      QListWidget *m_fileList;
      QPushButton *m_okButton;
      QnetTool *m_qnetTool;
  };
};

#endif


