#ifndef TableColumn_H
#define TableColumn_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QObject>

class QString;


namespace Isis {

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
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

#endif
