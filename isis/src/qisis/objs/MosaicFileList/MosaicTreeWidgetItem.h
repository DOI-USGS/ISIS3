#ifndef MosaicTreeWidgetItem_H
#define MosaicTreeWidgetItem_H

#include <QTreeWidgetItem>

namespace Isis {
  class CubeDisplayProperties;

  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2011-05-18 Steven Lambright - Resolution now sorts correctly
   *   @history 2012-12-27 Steven Lambright - Added PhaseAngleColumn to TreeColumn enumeration.
   *                           Also added the necessary code to support this new column.
   *                           References #1093.
   */
  class MosaicTreeWidgetItem : public QTreeWidgetItem {
    public:
      MosaicTreeWidgetItem(CubeDisplayProperties *cube,
                           QTreeWidget *parent = 0);
      virtual ~MosaicTreeWidgetItem();

      CubeDisplayProperties *cubeDisplay() {
        return p_cubeDisplay;
      }

      void forgetDisplay();

      void update(bool save);

      // This is the column number for each column
      enum TreeColumn {
        NameColumn = 0,
        FootprintColumn,
        OutlineColumn,
        ImageColumn,
        LabelColumn,
        ResolutionColumn,
        EmissionAngleColumn,
        PhaseAngleColumn,
        IncidenceAngleColumn,
        BlankColumn
      };

      static QString treeColumnToString(TreeColumn);

    private:
      bool operator<(const QTreeWidgetItem &other) const;

      CubeDisplayProperties *p_cubeDisplay;

      Qt::CheckState toCheck(QVariant);
  };
}

#endif

