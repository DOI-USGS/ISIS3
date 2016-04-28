#include "GuiCameraDisplayProperties.h"

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
#include "XmlStackedHandlerReader.h"

namespace Isis {
  /**
   * GuiCameraDisplayProperties constructor. This sets default values and
   *   constructs the object *.
   *
   *
   * @param displayName The filename (fully expanded) of the object.
   * @param parent Qt parent object (this is destroyed when parent is destroyed)
   */
  GuiCameraDisplayProperties::GuiCameraDisplayProperties(QString displayName, QObject *parent) :
      DisplayProperties(displayName, parent) {

    m_propertiesUsed = None;
    m_propertyValues = new QMap<int, QVariant>;

    // set all of the defaults to prevent unwanted change signals from
    //   being emitted later.
    setShowLabel(false);
    setSelected(false);

    setValue(Color, QVariant::fromValue(randomColor()));
  }


  GuiCameraDisplayProperties::GuiCameraDisplayProperties(XmlStackedHandlerReader *xmlReader,
      QObject *parent) : DisplayProperties("", parent) {
    m_propertiesUsed = None;
    m_propertyValues = new QMap<int, QVariant>;

    xmlReader->pushContentHandler(new XmlHandler(this));
  }


  /**
   * destructor
   */
  GuiCameraDisplayProperties::~GuiCameraDisplayProperties() {
  }


//  void GuiCameraDisplayProperties::fromPvl(const PvlObject &pvl) {
//    m_displayName = ((IString)pvl["DisplayName"][0]).ToQt();

//    QByteArray hexValues(pvl["Values"][0].c_str());
//    QDataStream valuesStream(QByteArray::fromHex(hexValues));
//    valuesStream >> *m_propertyValues;
//  }


//  /**
//   * Convert to Pvl for project files. This stores all of the data associated
//   *   with all of the properties (but not what is supported). This also s  tores
//   *   the target filename.
//   */
//  PvlObject GuiCameraDisplayProperties::toPvl() const {
//    PvlObject output("DisplayProperties");
//    output += PvlKeyword("DisplayName", m_displayName);

//    QBuffer dataBuffer;
//    dataBuffer.open(QIODevice::ReadWrite);

//    QDataStream propsStream(&dataBuffer);
//    propsStream << *m_propertyValues;
//    dataBuffer.seek(0);

//    output += PvlKeyword("Values", QString(dataBuffer.data().toHex()));

//    return output;
//  }


  /**
   * Call this with every property you support, otherwise they will not
   *   communicate properly between widgets.
   *
   * @param prop The property you are adding support for
   */
  void GuiCameraDisplayProperties::addSupport(Property prop) {
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
  bool GuiCameraDisplayProperties::supports(Property prop) {
    return (m_propertiesUsed & prop) == prop;
  }


  /**
   * Get a property's associated data.
   *
   * @param prop The property
   */
  QVariant GuiCameraDisplayProperties::getValue(Property prop) const {
    return (*m_propertyValues)[prop];
  }


  /**
   * Creates and returns  a random color for the intial color of
   * the footprint polygon.
   */
  QColor GuiCameraDisplayProperties::randomColor() {
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


  void GuiCameraDisplayProperties::save(QXmlStreamWriter &stream, const Project *project,
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
   * Change the color associated with this target.
   */
  void GuiCameraDisplayProperties::setColor(QColor newColor) {
    setValue(Color, QVariant::fromValue(newColor));
  }


  /**
   * Change the selected state associated with this target.
   */
  void GuiCameraDisplayProperties::setSelected(bool newValue) {
    setValue(Selected, newValue);
  }


  /**
   * Change the visibility of the display name associated with this target.
   */
  void GuiCameraDisplayProperties::setShowLabel(bool newValue) {
    setValue(ShowLabel, newValue);
  }


  /**
   * Change the visibility of the display name. This should only be connected to
   *   by an action with a list of displays as its data. This synchronizes all
   *   of the values where at least one is guaranteed to be toggled.
   */
  void GuiCameraDisplayProperties::toggleShowLabel() {
    QList<GuiCameraDisplayProperties *> displays = senderToData(sender());

    bool value = getValue(ShowLabel).toBool();
    value = !value;

    GuiCameraDisplayProperties *display;
    foreach(display, displays) {
      display->setShowLabel(value);
    }
  }


  GuiCameraDisplayProperties::XmlHandler::XmlHandler(GuiCameraDisplayProperties *displayProperties) {
    m_displayProperties = displayProperties;
  }


  bool GuiCameraDisplayProperties::XmlHandler::startElement(const QString &namespaceURI,
      const QString &localName, const QString &qName, const QXmlAttributes &atts) {
    if (XmlStackedHandler::startElement(namespaceURI, localName, qName, atts)) {
      if (localName == "displayProperties") {
        QString displayName = atts.value("displayName");

        if (!displayName.isEmpty()) {
          m_displayProperties->setDisplayName(displayName);
        }
      }
    }

    return true;
  }


  bool GuiCameraDisplayProperties::XmlHandler::characters(const QString &ch) {
    m_hexData += ch;

    return XmlStackedHandler::characters(ch);
  }


  bool GuiCameraDisplayProperties::XmlHandler::endElement(const QString &namespaceURI,
      const QString &localName, const QString &qName) {
    if (localName == "displayProperties") {
      QByteArray hexValues(m_hexData.toLatin1());
      QDataStream valuesStream(QByteArray::fromHex(hexValues));
      valuesStream >> *m_displayProperties->m_propertyValues;
    }

    return XmlStackedHandler::endElement(namespaceURI, localName, qName);
  }


  /**
   * This is the generic mutator for properties. Given a value, this will
   *   change it and emit propertyChanged if its different and supported.
   */
  void GuiCameraDisplayProperties::setValue(Property prop, QVariant value) {
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
  QList<GuiCameraDisplayProperties *> GuiCameraDisplayProperties::senderToData(
      QObject *senderObj) {
    QList<GuiCameraDisplayProperties *> data;

    if (senderObj) {
      QAction *caller = (QAction *)senderObj;
      QVariant callerData = caller->data();

      if (callerData.canConvert< QList<GuiCameraDisplayProperties *> >() ) {
        data = callerData.value< QList<GuiCameraDisplayProperties *> >();
      }
    }

    return data;
  }


}
