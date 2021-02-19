#ifndef TreeViewHeader_H
#define TreeViewHeader_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QWidget>

class QMouseEvent;
class QPainter;
class QPaintEvent;
class QSize;
class QString;

namespace Isis {
  class TreeViewContent;

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2012-09-28 Kimberly Oyama - Changed member variables to be prefixed with "m_".
   *   @history 2017-07-25 Summer Stapleton - Removed the CnetViz namespace. Fixes #5054.
   */
  class TreeViewHeader : public QWidget {

      Q_OBJECT

    public:
      explicit TreeViewHeader(TreeViewContent *someContent,
          QWidget *parent);
      TreeViewHeader(const TreeViewHeader &other);
      virtual ~TreeViewHeader();
      QSize minimumSizeHint() const;
      QString getText();
      void setText(QString text);


      QSize sizeHint() const {
        return minimumSizeHint();
      }

      TreeViewHeader &operator=(const TreeViewHeader &other);


    public slots:
      void setActive(bool);
      void handleFilterCountsChanged(int visibleTopLevelItemCount,
          int topLevelItemCount);


    signals:
      void activated();


    protected:
      void mouseReleaseEvent(QMouseEvent *event);
      void paintEvent(QPaintEvent *event);


    private: // methods
      void nullify();
      void paintHeader(QPainter *painter, int rowheight);
      void paintProgress(QPainter *painter, const QRect &rect, int min,
          int max, int value);


    private slots:
      void updateFilterProgress(int newProgress);
      void updateFilterProgressRange(int min, int max);
      void updateRebuildProgress(int newProgress);
      void updateRebuildProgressRange(int min, int max);


    private: // data
      TreeViewContent *m_content;
      QString *m_headerText;
      int m_filterProgress;
      int m_filterProgressMin;
      int m_filterProgressMax;
      int m_rebuildProgress;
      int m_rebuildProgressMin;
      int m_rebuildProgressMax;
      bool m_active;
      int m_visibleCount;
      int m_totalCount;
  };
}


#endif
