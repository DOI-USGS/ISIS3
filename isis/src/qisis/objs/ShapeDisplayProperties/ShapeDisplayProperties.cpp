#include "ShapeDisplayProperties.h"

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
   * @brief ShapeDisplayProperties constructor. 
   *
   * This sets default values and constructs the object *.
   * @param displayName The filename (fully expanded) of the object.
   * @param parent Qt parent object (this is destroyed when parent is destroyed)
   */
  ShapeDisplayProperties::ShapeDisplayProperties(QString displayName, QObject *parent) :
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
   * @brief The destructor.
   */
  ShapeDisplayProperties::~ShapeDisplayProperties() {
  }





  /**
   * @brief Call this with every property you support, otherwise they will not
   *  communicate properly between widgets.
   * @param prop The property you are adding support for
   */
  void ShapeDisplayProperties::addSupport(Property prop) {
    if (!supports(prop)) {
      m_propertiesUsed = (Property)(m_propertiesUsed | prop);
      emit supportAdded(prop);
    }
  }


  /**
   * @brief Support for this may come later. Please make sure you are connected to the
   *  supportAdded signal.
   * @return @b bool True if the property has support, false otherwise.
   */
  bool ShapeDisplayProperties::supports(Property prop) {
    return (m_propertiesUsed & prop) == prop;
  }


  /**
   * @brief Get a property's associated data.
   *
   * @param prop The property
   * @return @b QVariant The data associated with the property.
   */
  QVariant ShapeDisplayProperties::getValue(Property prop) const {
    return (*m_propertyValues)[prop];
  }


  /**
   * @brief Creates and returns a random color for the initial color of
   * the footprint polygon.
   * @return @b QColor The color for the initial footprint polygon.
   */
  QColor ShapeDisplayProperties::randomColor() {
    // Gives a random number between 0 and 255
    int red = 0;
    int green = 0;
    int blue = 0;

    // Generate dark
    while (red + green + blue < 300) {
      red   = rand() % 256;
      green = rand() % 256;
      blue  = rand() % 256;
    }

    return QColor(red, green, blue, 60);
  }

  /**
   * @brief Saves this object to an XML file.
   * @param stream  The XML stream writer write to.
   * @param project The project this object is attached to (not used).
   * @param newProjectRoot FileName of the project?  (not used).
   */
  void ShapeDisplayProperties::save(QXmlStreamWriter &stream, const Project *project,
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
   * @brief Change the color associated with this shape.
   * @param newColor  The new color associated with this shape.
   */
  void ShapeDisplayProperties::setColor(QColor newColor) {
    setValue(Color, QVariant::fromValue(newColor));
  }


  /**
   * @brief Change the selected state associated with this shape.
   * @param The new state associated with this shape.
   */
  void ShapeDisplayProperties::setSelected(bool newValue) {
    setValue(Selected, newValue);
  }


  /**
   * @brief Change the visibility of the display name associated with this shape.
   * @param newValue The visibiliy of the display name for this shape.
   */
  void ShapeDisplayProperties::setShowLabel(bool newValue) {
    setValue(ShowLabel, newValue);
  }


  /**
   * @brief Change the visibility of the display name. 
   *
   * This should only be connected to
   * by an action with a list of displays as its data. This synchronizes all
   * of the values where at least one is guaranteed to be toggled.
   */
  void ShapeDisplayProperties::toggleShowLabel() {
    QList<ShapeDisplayProperties *> displays = senderToData(sender());

    bool value = getValue(ShowLabel).toBool();
    value = !value;

    ShapeDisplayProperties *display;
    foreach (display, displays) {
      display->setShowLabel(value);
    }
  }



  /**
   * @brief This is the generic mutator for properties. 
   *
   * Given a value, this will change it and emit propertyChanged if its 
   * different and supported.
   * @param prop The key into the m_propertyValues QMap <int, QVariant>
   * @param value The value we want to change to.
   */
  void ShapeDisplayProperties::setValue(Property prop, QVariant value) {
    if ((*m_propertyValues)[prop] != value) {
      (*m_propertyValues)[prop] = value;

      if (supports(prop)) {
        emit propertyChanged(this);
      }
    }
  }


  /**
   * @brief  Get the display properties from a slot
   *
   * This is for the slots that have a list of display properties as associated
   * data. This gets that list out of the data.
   * @param The object requesting the data.
   * @return @b QList<ShapeDisplayProperties *> A list of pointers to
   * ShapeDisplayProperties objects.
   */
  QList<ShapeDisplayProperties *> ShapeDisplayProperties::senderToData(
      QObject *senderObj) {
    QList<ShapeDisplayProperties *> data;

    if (senderObj) {
      QAction *caller = (QAction *)senderObj;
      QVariant callerData = caller->data();

      if (callerData.canConvert< QList<ShapeDisplayProperties *> >() ) {
        data = callerData.value< QList<ShapeDisplayProperties *> >();
      }
    }

    return data;
  }


}
