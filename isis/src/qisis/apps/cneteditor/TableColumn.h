#ifndef TableColumn_H
#define TableColumn_H

#include <QObject>

class QString;


namespace Isis
{
  namespace CnetViz
  {
    class TableColumn : public QObject
    {
        Q_OBJECT

      public:
        explicit TableColumn(QString, bool, bool);
        TableColumn(const TableColumn &);
        virtual ~TableColumn();
        QString getTitle() const;
        void setTitle(QString text);
        TableColumn & operator=(TableColumn);
        bool isVisible() const;
        void setVisible(bool);
        int getWidth() const;
        void setWidth(int);
        bool isReadOnly() const;
        void setReadOnly(bool);
        bool hasNetworkStructureEffect() const;
        bool sortAscending() const;
        void setSortAscending(bool ascending);


      public:
        static const int EDGE_WIDTH = 4;


      signals:
        void selected(TableColumn *);
        void sortOutDated();
        void widthChanged();
        void visibilityChanged();


      private: // methods
        void nullify();


      private: // data
        QString * title;
        bool visible;
        bool readOnly;
        int width;
        bool affectsNetworkStructure;
        bool ascendingSortOrder;
    };
  }
}

#endif
