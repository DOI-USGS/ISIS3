#ifndef MosaicTreeWidget_H
#define MosaicTreeWidget_H

#include <QTreeWidget>

namespace Qisis {
 
  class MosaicTreeWidget : public QTreeWidget {
    Q_OBJECT

    public:
      MosaicTreeWidget(QWidget *parent = 0);
                                                        
    public slots:

    signals:
      void itemDropped(QPoint point);
      
    protected:
      void dropEvent(QDropEvent *event);
      //QMimeData *mimeData(const QList<QTreeWidgetItem*> items) const;
      //QStringList mimeTypes() const;

    private slots:
      

    private:
     
  };
};

#endif
