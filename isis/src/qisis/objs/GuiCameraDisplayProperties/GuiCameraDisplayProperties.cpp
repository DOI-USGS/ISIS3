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

namespace Isis {
  /**
   * @brief GuiCameraDisplayProperties constructor. This sets default values and
   * constructs the object pointer.
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
   * @brief Call this with every property you support, otherwise they will not
   * communicate properly between widgets.
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
   * @brief Support may come later, please make sure you are connected to the
   *  supportAdded signal.
   *
   * @return @b bool Returns true if the property has support, false otherwise.
   */
  bool GuiCameraDisplayProperties::supports(Property prop) {
    return (m_propertiesUsed & prop) == prop;
  }


  /**
   * @brief Get a property's associated data.
   * @param prop The property
   * @return @b QVariant Returns the value of the property.
   */
  QVariant GuiCameraDisplayProperties::getValue(Property prop) const {
    return (*m_propertyValues)[prop];
  }


  /**
   * @brief Creates and returns a random color for the intial color of
   * the footprint polygon.
   * @return @b QColor  Returns a random color.
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


  /**
   * @brief Write the Gui Camera Display Properties out to an XML file.
   * @param stream  The output data stream.
   * @param project Not used in this function.
   * @param newProjectRoot Not used in this function.
   */
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
   * @brief Change the color associated with this target.
   * @param newColor is the color to associate with this target.
   */
  void GuiCameraDisplayProperties::setColor(QColor newColor) {
    setValue(Color, QVariant::fromValue(newColor));
  }


  /**
   * @brief Change the selected state associated with this target.
   * @param newValue is the new state associated with this target.
   */
  void GuiCameraDisplayProperties::setSelected(bool newValue) {
    setValue(Selected, newValue);
  }


  /**
   * @brief Change the visibility of the display name associated with this target.
   * @param newValue  Shows/hides the display name of the associated target.
   */
  void GuiCameraDisplayProperties::setShowLabel(bool newValue) {
    setValue(ShowLabel, newValue);
  }


  /**
   * @brief Change the visibility of the display name. This should only be connected to
   *  by an action with a list of displays as its data. This synchronizes all
   *  of the values where at least one is guaranteed to be toggled.
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


  /**
   * @brief Sets the GuiCameraDisplayProperties variable pointer.
   * @param displayProperties  The new pointer.
   */

  GuiCameraDisplayProperties::XmlHandler::XmlHandler(GuiCameraDisplayProperties *displayProperties) {
    m_displayProperties = displayProperties;
  }


  /**
   * @brief The XML reader invokes this method at the start of every element in the
   *        XML document.
   * A quick example using this function:
   *     startElement("xsl","stylesheet","xsl:stylesheet",attributes)
   *
   * @param namespaceURI The Uniform Resource Identifier of the element's namespace
   * @param localName The local name string
   * @param qName The XML qualified string (or empty, if QNames are not available).
   * @param atts The XML attributes attached to each element
   * @return @b bool  Returns True signalling to the reader the start of a valid XML element.  If
   * False is returned, something bad happened.
   *
   */
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


  /**
   * @brief This implementation of a virtual function calls
   * QXmlDefaultHandler::characters(QString &ch)
   * which in turn calls QXmlContentHandler::characters(QString &ch) which
   * is called when the XML processor has parsed a chunk of character data.
   * @see XmlStackedHandler, QXmlDefaultHandler,QXmlContentHandler
   * @param ch The character data.
   * @return @b bool Returns True if there were no problems with the character processing.
   * It returns False if there was a problem, and the XML reader stops.
   */
  bool GuiCameraDisplayProperties::XmlHandler::characters(const QString &ch) {
    m_hexData += ch;

    return XmlStackedHandler::characters(ch);
  }


  /**
   * @brief The XML reader invokes this method at the end of every element in the
   *        XML document.
   * @param namespaceURI  The Uniform Resource Identifier of the namespace (eg. "xmlns")
   * @param localName The local name string (eg. "xhtml")
   * @param qName The XML qualified string (eg.  "xmlns:xhtml"). This can be empty if
   *        QNames are not available.
   * @return @b bool If this function returns True, then a signal is sent to the reader indicating
   * the end of the element.  If this function returns False, something bad
   * happened and processing stops.
   */
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
   * @brief This is the generic mutator for properties. 
   *
   * Given a value, this will change it and emit propertyChanged if its different 
   * and supported.  
   * @param prop The key into the m_propertyValues QMap <int, QVariant>
   * @param value The value we want to change to.
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
   * @brief This is for the slots that have a list of display properties as associated
   * data. This gets that list out of the data.
   * @param senderObj  The caller object containing a list of the display properties.
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
