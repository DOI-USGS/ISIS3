#include "MosaicTreeWidgetItem.h"

#include "Cube.h"
#include "CubeDisplayProperties.h"
#include "Filename.h"
#include "iException.h"
#include "Table.h"

namespace Isis {

  /**
   * MosaicTreeWidget constructor.
   * MosaicTreeWidget is derived from QTreeWidget
   *
   *
   * @param parent
   */
  MosaicTreeWidgetItem::MosaicTreeWidgetItem(CubeDisplayProperties *cubeDisplay,
      QTreeWidget *parent) : QTreeWidgetItem(parent, UserType) {
    p_cubeDisplay = cubeDisplay;

    p_cubeDisplay->addSupport(CubeDisplayProperties::Selected);
    p_cubeDisplay->addSupport(CubeDisplayProperties::Color);

    setText(NameColumn, p_cubeDisplay->displayName());
    Cube *cube = p_cubeDisplay->cube();

    try {
      Table table("CameraStatistics", cube->Filename());
      for (int i = 0; i < table.Records(); i++) {
        for (int j = 0; j < table[i].Fields(); j++) {
          QString label;

          if (table[i][j].IsText()) {
            label = QString::fromStdString((std::string)table[i][j]);
            label.truncate(10);
          }

          // Get the average resolution for this mosaic item.
          if (table[i][j].IsText() && label.compare("Resolution") == 0) {
            if (j + 3 < table[i].Fields()) {
              if (table[i][j+3].IsInteger()) {
              }
              else if (table[i][j+3].IsDouble()) {
                setText(ResolutionColumn,
                        QString::number((double)table[i][j+3]));
              }
              else if (table[i][j+3].IsText()) {
              }
            }
          }

          // Get the average emission angle for this mosaic item.
          if (table[i][j].IsText() && label.compare("EmissionAn") == 0) {
            if (j + 3 < table[i].Fields()) {
              if (table[i][j+3].IsInteger()) {
              }
              else if (table[i][j+3].IsDouble()) {
                setText(EmissionAngleColumn,
                        QString::number((double)table[i][j+3]));
              }
              else if (table[i][j+3].IsText()) {
              }
            }
          }

          // Get the average incidence angle for this mosaic item.
          if (table[i][j].IsText() && label.compare("IncidenceA") == 0) {
            if (j + 3 < table[i].Fields()) {
              if (table[i][j+3].IsInteger()) {
              }
              else if (table[i][j+3].IsDouble()) {
                setText(IncidenceAngleColumn,
                        QString::number((double)table[i][j+3]));
              }
              else if (table[i][j+3].IsText()) {
              }
            }
          }
        }
      }
    }
    catch(iException &e) {
      p_cubeDisplay->deleteLater();
      e.Report();
      e.Clear();
    }

    setFlags(Qt::ItemIsEnabled |
             Qt::ItemIsUserCheckable |
             Qt::ItemIsSelectable |
             Qt::ItemIsDragEnabled |
             Qt::ItemIsDropEnabled);

    update(false);
  }


  MosaicTreeWidgetItem::~MosaicTreeWidgetItem() {
  }


  void MosaicTreeWidgetItem::forgetDisplay() {
    p_cubeDisplay = NULL;
  }


  void MosaicTreeWidgetItem::update(bool save) {
    if(save && p_cubeDisplay) {
      p_cubeDisplay->setShowFill(checkState(FootprintColumn));
      p_cubeDisplay->setShowOutline(checkState(OutlineColumn));
      p_cubeDisplay->setShowDNs(checkState(ImageColumn));
      p_cubeDisplay->setShowLabel(checkState(LabelColumn));

      if(parent()) {
        p_cubeDisplay->setSelected(isSelected() || parent()->isSelected());
      }
    }
    else if(p_cubeDisplay) {
      setBackground(NameColumn,
          QBrush(
          p_cubeDisplay->getValue(CubeDisplayProperties::Color).value<QColor>()
          ));

      setCheckState(FootprintColumn,
          toCheck(p_cubeDisplay->getValue(CubeDisplayProperties::ShowFill)));

      setCheckState(OutlineColumn,
          toCheck(p_cubeDisplay->getValue(CubeDisplayProperties::ShowOutline)));

      setCheckState(ImageColumn,
          toCheck(p_cubeDisplay->getValue(CubeDisplayProperties::ShowDNs)));

      setCheckState(LabelColumn,
          toCheck(p_cubeDisplay->getValue(CubeDisplayProperties::ShowLabel)));

      bool displaySelected =
          p_cubeDisplay->getValue(CubeDisplayProperties::Selected).toBool();

      if(parent()) {
        if(displaySelected && !isSelected() && !parent()->isSelected()) {
          setSelected(true);
        }
        else if(!displaySelected && (isSelected() || parent()->isSelected())) {
          setSelected(false);
          parent()->setSelected(false);
        }
      }
    }
  }


  Qt::CheckState MosaicTreeWidgetItem::toCheck(QVariant var) {
    if(var.toBool())
      return Qt::Checked;
    else
      return Qt::Unchecked;
  }


  QString MosaicTreeWidgetItem::treeColumnToString(TreeColumn column) {
    switch(column) {
      case NameColumn:
        return "Name";
      case FootprintColumn:
        return "Footprint";
      case OutlineColumn:
        return "Outline";
      case ImageColumn:
        return "Image";
      case LabelColumn:
        return "Label";
      case ResolutionColumn:
        return "Resolution";
      case EmissionAngleColumn:
        return "Emission Angle";
      case IncidenceAngleColumn:
        return "Incidence Angle";
      case IslandColumn:
        return "Island";
      case BlankColumn:
        return "";
    }

    throw iException::Message(iException::Programmer,
        "Invalid tree column passed to treeColumnToString", _FILEINFO_);
  }


  bool MosaicTreeWidgetItem::operator<(const QTreeWidgetItem &other) const {
    int column = treeWidget()->sortColumn();

    switch((TreeColumn)column) {
      case FootprintColumn:
      case ImageColumn:
      case LabelColumn:
      case NameColumn:
      case OutlineColumn:
      case ResolutionColumn:
        return text(column) < other.text(column);

      case EmissionAngleColumn:
      case IncidenceAngleColumn:
      case IslandColumn:
        return text(column).toDouble() <
               other.text(column).toDouble();

      default:
        return false;
    }
  }
}

