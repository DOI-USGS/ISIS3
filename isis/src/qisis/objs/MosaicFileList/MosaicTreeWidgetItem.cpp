#include "MosaicTreeWidgetItem.h"

#include <QDebug>

#include "Cube.h"
#include "CubeDisplayProperties.h"
#include "FileName.h"
#include "IException.h"
#include "SpecialPixel.h"
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

    if(!IsSpecial(p_cubeDisplay->resolution()))
      setText(ResolutionColumn,
              QString::number(p_cubeDisplay->resolution()));

    if(!IsSpecial(p_cubeDisplay->emissionAngle()))
      setText(EmissionAngleColumn,
              QString::number(p_cubeDisplay->emissionAngle()));

    if(!IsSpecial(p_cubeDisplay->phaseAngle()))
      setText(PhaseAngleColumn,
              QString::number(p_cubeDisplay->phaseAngle()));

    if(!IsSpecial(p_cubeDisplay->incidenceAngle()))
      setText(IncidenceAngleColumn,
              QString::number(p_cubeDisplay->incidenceAngle()));

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
      case PhaseAngleColumn:
        return "Phase Angle";
      case IncidenceAngleColumn:
        return "Incidence Angle";
      case BlankColumn:
        return "";
    }

    throw IException(IException::Programmer,
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
        return text(column) < other.text(column);

      case EmissionAngleColumn:
      case PhaseAngleColumn:
      case IncidenceAngleColumn:
      case ResolutionColumn:
        return text(column).toDouble() <
               other.text(column).toDouble();

      default:
        return false;
    }
  }
}
