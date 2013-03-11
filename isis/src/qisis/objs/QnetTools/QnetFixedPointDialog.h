#ifndef QnetFixedPointDialog_h
#define QnetFixedPointDialog_h

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
  class QnetTool;

  /**
   * @author ????-??-?? Unknown
   * @internal
   *   @history 2008-11-26 Jeannie Walldren - Added functionality
   *                          to show the last Point ID entered
   *                          into a new point dialog box.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          in constructor.  Removed "std::" in
   *                          header and .cpp files.
   *   @history 2011-06-08 Tracie Sucharski - Point type Ground renamed to
   *                          Fixed, so class renamed from QnetGroundPointDialog
   *                          to QnetFixedPointDialog.
   */
  class QnetFixedPointDialog : public QDialog {
    Q_OBJECT

    public:
      QnetFixedPointDialog(QnetTool *qnetTool,
          QString defaultPointId, QWidget *parent = 0);
      ~QnetFixedPointDialog();

      bool isFixed() const;
      bool isConstrained() const;
      QString pointId() const;
      QStringList selectedFiles() const;
      void setFiles (QStringList pointFiles);


    private slots:
      void enableOkButton(const QString &text);

    private:
      QLineEdit *m_ptIdValue;
      QRadioButton *m_fixed;
      QRadioButton *m_constrained;
      QListWidget *m_fileList;

      QRadioButton *m_avg;
      QRadioButton *m_select;

      QLabel *m_ptIdLabel;
      QPushButton *m_okButton;

      QStringList *m_pointFiles;

      QnetTool *m_qnetTool;
  };
};

#endif

