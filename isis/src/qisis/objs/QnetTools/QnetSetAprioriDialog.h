#ifndef QnetSetAprioriDialog_h
#define QnetSetAprioriDialog_h

#include <QDialog>

#include "ui_QnetSetAprioriDialog.h"

namespace Isis {
  class QnetTool;

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   */
  class QnetSetAprioriDialog : public QDialog, public Ui::QnetSetAprioriDialog {
      Q_OBJECT

    public:
      QnetSetAprioriDialog(QnetTool *qnetTool, QWidget *parent = 0);
      void setPoints(QList<QListWidgetItem *> selectedPoints);

    signals:
     // void aprioriDialogClosed();
      void pointChanged(QString pointId);
      void netChanged();

    private slots:
      void unlockPoint(QListWidgetItem *ptId);
      void setApriori();

    private:
      void clearLineEdits();
      void fillLineEdits();


      QList<QListWidgetItem *> m_points;
      QnetTool *m_qnetTool;
  };
}

#endif
