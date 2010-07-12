#ifndef QnetHoldPointDialog_h
#define QnetHoldPointDialog_h

#include <QDialog>

class QListWidget;
class QRadioButton;
class QPushButton;

#include "ControlPoint.h"

#include <vector>

namespace Qisis {
  /**
   * Dialog box to help user choose how to determine lat/lon/rad
   * for selected hold point.
   *
   * @internal
   *   @history 2008-12-29 Jeannie Walldren - Changed name from
   *                          QnetGroundPointDialog
   *   @history 2010-06-02 Jeannie Walldren - Added overridden reject() method
   *                          to emit the holdCancelled() signal to indicate
   *                          that the "Cancel" button was clicked. This signal
   *                          is connected in QnetTool's setHoldPoint() method.
   *   @history 2010-06-03 Jeannie Walldren - Initialized pointers to null in
   *                          constructor
   *   @history 2010-06-03 Jeannie Walldren - Moved cancel button to private
   *                          variables for consistency
   *
   */
  class QnetHoldPointDialog : public QDialog {
      Q_OBJECT

    public:
      QnetHoldPointDialog(QWidget *parent = 0);
      void setPoint(Isis::ControlPoint &point);


    signals:
      void holdPoint(Isis::ControlPoint &point);
      void holdCancelled();

    private:

      QRadioButton *p_avg;
      QRadioButton *p_select;
      QListWidget *p_fileList;

      QPushButton *p_okButton;
      QPushButton *p_cancelButton;

      Isis::ControlPoint *p_point;

    private slots:
      void selectMeasures();
      void accept();
      void reject();
  };
};

#endif




