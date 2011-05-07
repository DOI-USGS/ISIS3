#ifndef MosaicTreeWidgetItem_H
#define MosaicTreeWidgetItem_H

#include <QTreeWidgetItem>

namespace Isis {
  class CubeDisplayProperties;

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
        IncidenceAngleColumn,
        IslandColumn,
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

