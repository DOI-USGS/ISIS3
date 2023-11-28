#include "ImageDisplayProperties.h"

#include <QAction>
#include <QBitArray>
#include <QBuffer>
#include <QColorDialog>
#include <QDebug>
#include <QInputDialog>
#include <QMap>
#include <QVariant>

#include "FileName.h"
#include "Pvl.h"

namespace Isis {
  /**
   * ImageDisplayProperties constructor. This sets default values and constructs
   *   the Cube *. You cannot have much more than 1K of these without calling
   *   closeCube().
   *
   *
   * @param displayName The filename (fully expanded) of the cube file.
   * @param parent Qt parent object (this is destroyed when parent is destroyed)
   */
  ImageDisplayProperties::ImageDisplayProperties(QString displayName, QObject *parent) :
      DisplayProperties(displayName, parent) {

    // set all of the defaults to prevent unwanted change signals from
    //   being emitted later.
    setShowFill(true);
    setShowOutline(true);
    setShowDNs(false);

    setValue(Color, QVariant::fromValue(randomColor()));
  }


  /**
   */
  ImageDisplayProperties::~ImageDisplayProperties() {
  }


  /**
   * Creates and returns  a random color for the intial color of
   * the footprint polygon.
   */
  QColor ImageDisplayProperties::randomColor() {
    // Gives a random number between 0 and 255
    int red = 0;
    int green = 0;
    int blue = 0;

    // Generate dark
    while(red + green + blue < 300) {
      red   = rand() % 256;
      green = rand() % 256;
      blue  = rand() % 256;
    }

    return QColor(red, green, blue, 60);
  }


  /**
   * Change the color associated with this cube.
   */
  void ImageDisplayProperties::setColor(QColor newColor) {
    setValue(Color, QVariant::fromValue(newColor));
  }


  /**
   * Change the selected state associated with this cube.
   */
  void ImageDisplayProperties::setSelected(bool newValue) {
    setValue(Selected, newValue);
  }


  /**
   * Change the visibility of DNs associated with this cube.
   */
  void ImageDisplayProperties::setShowDNs(bool newValue) {
    setValue(ShowDNs, newValue);
  }


  /**
   * Change the visibility of the fill area associated with this cube.
   */
  void ImageDisplayProperties::setShowFill(bool newValue) {
    setValue(ShowFill, newValue);
  }


  /**
   * Change the visibility of the display name associated with this cube.
   */
  void ImageDisplayProperties::setShowLabel(bool newValue) {
    setValue(ShowLabel, newValue);
  }


  /**
   * Change the visibility of the outline associated with this cube.
   */
  void ImageDisplayProperties::setShowOutline(bool newValue) {
    setValue(ShowOutline, newValue);
  }
}

