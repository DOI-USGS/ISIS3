#ifndef TableColumn_H
#define TableColumn_H

#include <QObject>

class QString;


namespace Isis {
  namespace CnetViz {

    /**
     * @author ????-??-?? Unknown
     *
     * @internal
     *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
     */
    class TableColumn : public QObject {
        Q_OBJECT

      public:
        explicit TableColumn(QString, bool, bool);
        TableColumn(const TableColumn &);
        virtual ~TableColumn();
        QString getTitle() const;
        void setTitle(QString text);
        TableColumn &operator=(TableColumn);
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
        QString *m_title;
        bool m_visible;
        bool m_readOnly;
        int m_width;
        bool m_affectsNetworkStructure;
        bool m_ascendingSortOrder;
    };
  }
}

#endif
