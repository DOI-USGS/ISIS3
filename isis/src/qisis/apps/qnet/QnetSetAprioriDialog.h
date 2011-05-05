#ifndef QnetSetAprioriDialog_h
#define QnetSetAprioriDialog_h

#include <QDialog>

#include "ui_QnetSetAprioriDialog.h"

#include "SurfacePoint.h"


class QnetSetAprioriDialog : public QDialog, public Ui::QnetSetAprioriDialog {
    Q_OBJECT

  public:
    QnetSetAprioriDialog(QWidget *parent = 0);
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


    QList<QListWidgetItem *> p_points;


};

#endif
