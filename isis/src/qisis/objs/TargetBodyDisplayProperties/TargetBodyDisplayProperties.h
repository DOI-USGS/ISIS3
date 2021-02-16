#ifndef TargetBodyDisplayProperties_H
#define TargetBodyDisplayProperties_H
/**
 * @file
 * $Revision: 1.9 $
 * $Date: 2012/06/12 06:30:00 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <QColor> // This is required since QColor is in a slot
#include <QMetaType> // required since we're adding to QVariant
#include <QObject>

#include "DisplayProperties.h"
#include "XmlStackedHandler.h"

class QAction;
class QXmlStreamWriter;

namespace Isis {
  class FileName;
  class Project;
  class Pvl;
  class PvlObject;
  class XmlStackedHandlerReader;

  /**
   * @brief This is the GUI communication mechanism for target body objects.
   *
   * This class is the connector between various GUI interfaces for target body objects.
   *   We use this to communicate shared properties that various widgets need
   *   to know/should react to in a generic way.
   *
   * This is how this class is supposed to "connect" widgets:
   *
   *  widgetA         widgetB           widgetC
   *     |               |                 |
   *     ------DisplayProperties -------
   *
   * When a user selects a target in widgetA, widgetB and widgetC now have a
   *   chance to also select the same target. This applies to all shared
   *   properties. Some of the properties are actions - such as ?????. This
   *   also allows a widget with no ??? (such as a list) to have an option
   *   to ???? (if any of the widgets support it*) and have that option work.
   *   There is no state associated with ????? - it's an action connected
   *   to a signal.
   *
   * The proper way to detect a target going away is to connect to the
   *   destroyed signal (from the parent QObject). Once that is emitted you
   *   cannot call any methods on this object.
   *
   * @author 2015-05-27 Ken Edmundson
   *
   * @internal
   *   @history 2015-05-27 Ken Edmundson - Creation.
   *   @hsitory 21016-06-14 Tyler Wilson - Added documentation to member functions/variables
   *                           and corrected some formatting to bring the the code in line
   *                           with ISIS coding standards.  Fixes #3997.
   *
   */
  class TargetBodyDisplayProperties : public DisplayProperties {
      Q_OBJECT
    public:
        /**
         * @brief This is a list of properties and actions that are possible.
         */
        enum Property {
          //! Null display property for bit-flag purposes
          None             = 0,
          //! The color of the control net, default randomized (QColor)
          Color            = 1,
          //! The selection state of this control net (bool)
          Selected         = 2,
          //! True if the control net should show its display name (bool)
          ShowLabel        = 16,
        };


      TargetBodyDisplayProperties(QString displayName, QObject *parent = NULL);
      TargetBodyDisplayProperties(XmlStackedHandlerReader *xmlReader, QObject *parent = NULL);
      virtual ~TargetBodyDisplayProperties();

//      void fromPvl(const PvlObject &pvl);
//      PvlObject toPvl() const;

      void addSupport(Property prop);
      bool supports(Property prop);

      QVariant getValue(Property prop) const;

      static QColor randomColor();

      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;

    signals:
      void propertyChanged(TargetBodyDisplayProperties *);
      void supportAdded(Property);

    public slots:
      void setColor(QColor newColor);
      void setShowLabel(bool);
      void setSelected(bool);

    private slots:
      void toggleShowLabel();

    private:
      /**
       * @brief Process an XML file containing information about a WorkOrder.
       *
       * @author 2012-??-?? Steven Lambright
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(TargetBodyDisplayProperties *displayProperties);

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);

          virtual bool characters(const QString &ch);

          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                  const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          TargetBodyDisplayProperties *m_displayProperties;
          QString m_hexData;
      };

    private:
      TargetBodyDisplayProperties(const TargetBodyDisplayProperties &);
      TargetBodyDisplayProperties &operator=(const TargetBodyDisplayProperties &);

      void setValue(Property prop, QVariant value);
      static QList<TargetBodyDisplayProperties *> senderToData(QObject *sender);

      /**
       * This indicated whether any widgets with this DisplayProperties
       *   is using a particular property. This helps others who can set
       *   but not display know whether they should give the option to set.
       */
      Property m_propertiesUsed;

      /**
       * This is a map from Property to value -- the reason I use an int is
       *   so Qt knows how to serialize this QMap into binary data
       */
      QMap<int, QVariant> *m_propertyValues;
  };
}

Q_DECLARE_METATYPE(QList<Isis::TargetBodyDisplayProperties *>);

#endif
