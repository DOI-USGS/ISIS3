#ifndef NewControlPointDialog_h
#define NewControlPointDialog_h

#include <QDialog>

class QComboBox;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QRadioButton;
class QString;
class QStringList;

namespace Isis {
  class ControlNet;
  class SerialNumberList;

  /**
   * @author ????-??-?? Tracie Sucharski
   * @internal
   *   @history 2008-11-26 Jeannie Walldren - Added functionality
   *                          to show the last Point ID entered
   *                          into a new point dialog box.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers
   *                          in constructor.  Removed "std::" in
   *                          header and .cpp files.
   *   @history 2010-12-03 Eric Hyer - Selected points now go to the top!
   *   @history 2016-09-16 Tracie Sucharski - Renamed to NewControlPointDialog in anticipation of
   *                          creating a parent class that QnetNewPointDialog and
   *                          MatchToolNewPointDialog can inherit.
   *   @history 2016-10-18 Tracie Sucharski - Added method to return value of the
   *                          subpixelRegister radio button.  If set, all measures in the control
   *                          point created will be subpixel registered.
   *   @history 2017-07-04 Christopher Combs - Combined functionality of dialogs from qnet,
   *                          qview and ipce applications by adding bools to toggle on specific
   *                          elements of the dialog to remove similar classes like
   *                          QnetNewPointDialog. Fixes #4383.
   *   @history 2018-05-02 Tracie Sucharski - If no shapes available remove option to change point
   *                          type to "Constrained" or "Fixed". Because pointType combo dependent on
   *                          having all 3 types to use ControlPoint's enum, change comparisons to
   *                          check text instead of int values. Add tool tip explaining why point
   *                          type cannot be changed.  Set tool tip on Ok button explaining why it
   *                          is not enabled. Fixes #5087.
   */
  class NewControlPointDialog : public QDialog {

      Q_OBJECT

    public:
      NewControlPointDialog(ControlNet *controlNet,
                            SerialNumberList *serialNumberList,
                            QString defaultPointId,
                            QWidget *parent = 0,
                            bool pointType = false,
                            bool groundSource = false,
                            bool subpixelRegisterMeasures = false);

      QString pointId() const;
      int pointType() const;
      void setGroundSource(QStringList groundFiles, int numberShapesWithPoint);
      QString groundSource() const;
      QStringList selectedFiles() const;
      void setFiles(QStringList pointFiles);
      bool subpixelRegisterPoint();

    private slots:
      void pointTypeChanged(QString pointType);
      void enableOkButton(const QString &text);

    private:
      ControlNet *m_controlNet;
      SerialNumberList *m_serialNumberList;

      QLabel *m_ptIdLabel;
      QComboBox *m_pointTypeCombo;
      QComboBox *m_groundSourceCombo;
      QHBoxLayout *m_groundSourceLayout;
      QRadioButton *m_subpixelRegisterButton;
      QPushButton *m_okButton;
      QLineEdit *m_ptIdEdit;
      QListWidget *m_fileList;
      QStringList *m_pointFiles;

  };
};

#endif
