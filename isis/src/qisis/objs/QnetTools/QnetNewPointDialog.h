#ifndef QnetNewPointDialog_h
#define QnetNewPointDialog_h

#include <QDialog>

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QString;
class QStringList;

namespace Isis {
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
   *   @history 2010-12-03 Eric Hyer - Selected points now go to the top!
   */
  class QnetNewPointDialog : public QDialog {

      Q_OBJECT

    public:
      QnetNewPointDialog(QnetTool *qnetTool, QString defaultPointId, QWidget *parent = 0);

      QString pointId() const;
      QStringList selectedFiles() const;
      void setFiles(QStringList pointFiles);
    
    private slots:
      void enableOkButton(const QString &text);

    private:
      QLabel *m_ptIdLabel;
      QPushButton *m_okButton;
      QLineEdit *m_ptIdEdit;
      QListWidget *m_fileList;
      QStringList *m_pointFiles;
      QnetTool *m_qnetTool;

  };
};

#endif


