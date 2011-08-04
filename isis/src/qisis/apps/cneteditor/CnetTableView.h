#ifndef CnetTableView_H
#define CnetTableView_H


#include <QWidget>


template<typename T> class QList;
class QString;


namespace Isis
{
  class AbstractCnetTableModel;
  class AbstractTreeItem;
  class CnetTableViewContent;
  class CnetTableViewHeader;
  class CnetTableColumnList;

  class CnetTableView : public QWidget
  {
      Q_OBJECT

    public:
      CnetTableView();
      virtual ~CnetTableView();
      QSize sizeHint();
      QFont getContentFont() const;
      CnetTableViewHeader * getHorizontalHeader();

      QStringList getTitles() const;
      void setTitles(QStringList someTitle);

      void setColumnVisible(QString, bool);

      AbstractCnetTableModel * getModel();
      void setModel(AbstractCnetTableModel * newModel);


    public slots:
      void onModelSelectionChanged();
      void onModelSelectionChanged(QList< AbstractTreeItem * >);


    signals:
      void activated();
      void rebuildModels(QList<AbstractTreeItem *>);
      void selectionChanged();
      void modelDataChanged();


    private: // disable copying and assigning of this class
      CnetTableView(const CnetTableView &);
      CnetTableView & operator=(const CnetTableView & other);


    private: // methods
      void nullify();


    private: // data
      CnetTableViewHeader * header;
      CnetTableViewContent * content;
      CnetTableColumnList * columns;
      bool active;
  };
}

#endif
