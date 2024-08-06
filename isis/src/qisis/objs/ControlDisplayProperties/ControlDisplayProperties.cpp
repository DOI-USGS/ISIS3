/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "ControlDisplayProperties.h"

#include <QAction>
#include <QBitArray>
#include <QBuffer>
#include <QColorDialog>
#include <QDebug>
#include <QInputDialog>
#include <QMap>
#include <QVariant>
#include <QXmlStreamWriter>

#include "FileName.h"
#include "Pvl.h"

namespace Isis {
  /**
   * ControlDisplayProperties constructor. This sets default values and
   *   constructs the object *.
   *
   *
   * @param displayName The filename (fully expanded) of the object.
   * @param parent Qt parent object (this is destroyed when parent is destroyed)
   */
  ControlDisplayProperties::ControlDisplayProperties(QString displayName, QObject *parent) :
      DisplayProperties(displayName, parent) {

    m_propertiesUsed = None;
    m_propertyValues = new QMap<int, QVariant>;

    // set all of the defaults to prevent unwanted change signals from
    //   being emitted later.
    setShowLabel(false);
    setSelected(false);

    setValue(Color, QVariant::fromValue(randomColor()));
  }


  /**
   * destructor
   */
  ControlDisplayProperties::~ControlDisplayProperties() {
  }


  /**
   * Call this with every property you support, otherwise they will not
   *   communicate properly between widgets.
   *
   * @param prop The property you are adding support for
   */
  void ControlDisplayProperties::addSupport(Property prop) {
    if (!supports(prop)) {
      m_propertiesUsed = (Property)(m_propertiesUsed | prop);
      emit supportAdded(prop);
    }
  }


  /**
   * Support may come later, please make sure you are connected to the
   *   supportAdded signal.
   *
   * @returns True if the property has support, false otherwise
   */
  bool ControlDisplayProperties::supports(Property prop) {
    return (m_propertiesUsed & prop) == prop;
  }


  /**
   * Get a property's associated data.
   *
   * @param prop The property
   */
  QVariant ControlDisplayProperties::getValue(Property prop) const {
    return (*m_propertyValues)[prop];
  }


  /**
   * Creates and returns  a random color for the intial color of
   * the footprint polygon.
   */
  QColor ControlDisplayProperties::randomColor() {
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


  void ControlDisplayProperties::save(QXmlStreamWriter &stream, const Project *project,
                                      FileName newProjectRoot) const {
    stream.writeStartElement("displayProperties");

    stream.writeAttribute("displayName", displayName());

    // Get hex-encoded data
    QBuffer dataBuffer;
    dataBuffer.open(QIODevice::ReadWrite);
    QDataStream propsStream(&dataBuffer);
    propsStream << *m_propertyValues;
    dataBuffer.seek(0);

    stream.writeCharacters(dataBuffer.data().toHex());

    stream.writeEndElement();
  }


  /**
   * Change the color associated with this cube.
   */
  void ControlDisplayProperties::setColor(QColor newColor) {
    setValue(Color, QVariant::fromValue(newColor));
  }


  /**
   * Change the selected state associated with this cube.
   */
  void ControlDisplayProperties::setSelected(bool newValue) {
    setValue(Selected, newValue);
  }


  /**
   * Change the visibility of the display name associated with this cube.
   */
  void ControlDisplayProperties::setShowLabel(bool newValue) {
    setValue(ShowLabel, newValue);
  }


  /**
   * Change the visibility of the display name. This should only be connected to
   *   by an action with a list of displays as its data. This synchronizes all
   *   of the values where at least one is guaranteed to be toggled.
   */
  void ControlDisplayProperties::toggleShowLabel() {
    QList<ControlDisplayProperties *> displays = senderToData(sender());

    bool value = getValue(ShowLabel).toBool();
    value = !value;

    ControlDisplayProperties *display;
    foreach(display, displays) {
      display->setShowLabel(value);
    }
  }

  /**
   * This is the generic mutator for properties. Given a value, this will
   *   change it and emit propertyChanged if its different and supported.
   */
  void ControlDisplayProperties::setValue(Property prop, QVariant value) {
    if ((*m_propertyValues)[prop] != value) {
      (*m_propertyValues)[prop] = value;

      if (supports(prop)) {
        emit propertyChanged(this);
      }
    }
  }


  /**
   * This is for the slots that have a list of display properties as associated
   *   data. This gets that list out of the data.
   */
  QList<ControlDisplayProperties *> ControlDisplayProperties::senderToData(
      QObject *senderObj) {
    QList<ControlDisplayProperties *> data;

    if (senderObj) {
      QAction *caller = (QAction *)senderObj;
      QVariant callerData = caller->data();

      if (callerData.canConvert< QList<ControlDisplayProperties *> >() ) {
        data = callerData.value< QList<ControlDisplayProperties *> >();
      }
    }

    return data;
  }


}
