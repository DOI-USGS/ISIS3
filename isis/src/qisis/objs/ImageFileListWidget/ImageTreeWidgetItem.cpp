#include "ImageTreeWidgetItem.h"

#include <QDebug>

#include "Angle.h"
#include "Cube.h"
#include "DisplayProperties.h"
#include "ImageDisplayProperties.h"
#include "FileName.h"
#include "IException.h"
#include "Image.h"
#include "ImageList.h"
#include "ImageTreeWidget.h"
#include "SpecialPixel.h"
#include "Table.h"

namespace Isis {

  /**
   * ImageTreeWidget constructor.
   * ImageTreeWidget is derived from QTreeWidget
   *
   *
   * @param parent
   */
  ImageTreeWidgetItem::ImageTreeWidgetItem(ImageList *imageList, Image *image,
      QTreeWidget *parent) : QTreeWidgetItem(parent, UserType) {
    m_image = image;
    m_imageList = imageList;
    ImageDisplayProperties *displayProps = m_image->displayProperties();
    displayProps->addSupport(ImageDisplayProperties::Selected);
    displayProps->addSupport(ImageDisplayProperties::Color);
    connect(displayProps, SIGNAL(supportAdded(int)), this, SLOT(onDisplayPropertiesChanged()));
    setText(NameColumn, displayProps->displayName());

    setColumnValue(ResolutionColumn, m_image->resolution());
    setColumnValue(EmissionAngleColumn, m_image->emissionAngle().degrees());
    setColumnValue(IncidenceAngleColumn, m_image->incidenceAngle().degrees());
    setColumnValue(PhaseAngleColumn, m_image->phaseAngle().degrees());
    setColumnValue(AspectRatioColumn, m_image->aspectRatio());
    setColumnValue(SampleResolutionColumn, m_image->sampleResolution());
    setColumnValue(LineResolutionColumn, m_image->lineResolution());
    setColumnValue(NorthAzimuthColumn, m_image->northAzimuth().degrees());

    setFlags(Qt::ItemIsEnabled |
             Qt::ItemIsUserCheckable |
             Qt::ItemIsSelectable |
             Qt::ItemIsDragEnabled |
             Qt::ItemIsDropEnabled);

    update(false);
  }


  ImageTreeWidgetItem::~ImageTreeWidgetItem() {
  }


  Image *ImageTreeWidgetItem::image() {
    return m_image;
  }


  QString ImageTreeWidgetItem::imageListName() const {
    return m_imageList->name();
  }


  void ImageTreeWidgetItem::forgetImage() {
    m_image = NULL;
  }


  void ImageTreeWidgetItem::update(bool save) {
    if (m_image) {
      ImageDisplayProperties *displayProps = m_image->displayProperties();

      if (save) {
        if (displayProps->supports(ImageDisplayProperties::ShowFill))
          displayProps->setShowFill(checkState(FootprintColumn));

        if (displayProps->supports(ImageDisplayProperties::ShowOutline))
          displayProps->setShowOutline(checkState(OutlineColumn));

        if (displayProps->supports(ImageDisplayProperties::ShowDNs))
          displayProps->setShowDNs(checkState(ImageColumn));

        if (displayProps->supports(ImageDisplayProperties::ShowLabel))
          displayProps->setShowLabel(checkState(LabelColumn));

        if (parent()) {
          displayProps->setSelected(isSelected() || parent()->isSelected());
        }
      }
      else {

        setBackground(NameColumn,
            QBrush(
            displayProps->getValue(ImageDisplayProperties::Color).value<QColor>()
            ));

        if (displayProps->supports(ImageDisplayProperties::ShowFill))
          setCheckState(FootprintColumn,
              toCheck(displayProps->getValue(ImageDisplayProperties::ShowFill)));

        if (displayProps->supports(ImageDisplayProperties::ShowOutline))
          setCheckState(OutlineColumn,
              toCheck(displayProps->getValue(ImageDisplayProperties::ShowOutline)));

        if (displayProps->supports(ImageDisplayProperties::ShowDNs))
          setCheckState(ImageColumn,
              toCheck(displayProps->getValue(ImageDisplayProperties::ShowDNs)));

        if (displayProps->supports(ImageDisplayProperties::ShowLabel))
          setCheckState(LabelColumn,
              toCheck(displayProps->getValue(ImageDisplayProperties::ShowLabel)));

        bool displaySelected =
            displayProps->getValue(ImageDisplayProperties::Selected).toBool();

        if (parent()) {
          if (displaySelected && !isSelected() && !parent()->isSelected()) {
            setSelected(true);
          }
          else if (!displaySelected && (isSelected() || parent()->isSelected())) {
            setSelected(false);
            parent()->setSelected(false);
          }
        }
      }
    }
  }


  Qt::CheckState ImageTreeWidgetItem::toCheck(QVariant var) {
    if (var.toBool())
      return Qt::Checked;
    else
      return Qt::Unchecked;
  }


  QString ImageTreeWidgetItem::treeColumnToString(TreeColumn column) {
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
      case PhaseAngleColumn:
        return "Phase Angle";
      case AspectRatioColumn:
        return "Aspect Ratio";
      case SampleResolutionColumn:
        return "Sample Resolution";
      case LineResolutionColumn:
        return "Line Resolution";
      case NorthAzimuthColumn:
        return "North Azimuth";

      case BlankColumn:
        return "";
    }

    throw IException(IException::Programmer,
        "Invalid tree column passed to treeColumnToString", _FILEINFO_);
  }


  void ImageTreeWidgetItem::onDisplayPropertiesChanged() {
    ImageTreeWidget *tree = qobject_cast<ImageTreeWidget *>(treeWidget());

    if (tree) {
      tree->enqueueReadDisplayProperties(this);
    }
    else {
      update(false);
    }
  }


  void ImageTreeWidgetItem::setColumnValue(TreeColumn column, double value) {
    if (!IsSpecial(value)) {
      setText(column, QString::number(value));
    }
  }


  bool ImageTreeWidgetItem::operator<(const QTreeWidgetItem &other) const {
    int column = treeWidget()->sortColumn();

    switch((TreeColumn)column) {
      case FootprintColumn:
      case ImageColumn:
      case LabelColumn:
      case NameColumn:
      case OutlineColumn:
        return text(column) < other.text(column);

      case EmissionAngleColumn:
      case IncidenceAngleColumn:
      case PhaseAngleColumn:
      case ResolutionColumn:
      case AspectRatioColumn:
      case SampleResolutionColumn:
      case LineResolutionColumn:
      case NorthAzimuthColumn:
        return text(column).toDouble() <
               other.text(column).toDouble();

      case BlankColumn:
        return false;
    }

    return false;
  }
}
