#ifndef ShapeDisplayProperties_H
#define ShapeDisplayProperties_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QColor> // This is required since QColor is in a slot
#include <QMetaType> // required since we're adding to QVariant
#include <QObject>

#include "DisplayProperties.h"

class QAction;
class QXmlStreamWriter;

namespace Isis {
  class FileName;
  class Project;
  class Pvl;
  class PvlObject;

  /**
   * @brief This is the GUI communication mechanism for shape objects.
   *
   * This class is the connector between various GUI interfaces for shape objects.
   *   We use this to communicate shared properties that various widgets need
   *   to know/should react to in a generic way.
   *
   * This is how this class is supposed to "connect" widgets:
   *
   *  widgetA         widgetB           widgetC
   *     |               |                 |
   *     ------DisplayProperties -------
   *
   * When a user selects a shape in widgetA, widgetB and widgetC now have a
   *   chance to also select the same shape. This applies to all shared
   *   properties. Some of the properties are actions - such as ?????. This
   *   also allows a widget with no ??? (such as a list) to have an option
   *   to ???? (if any of the widgets support it*) and have that option work.
   *   There is no state associated with ????? - it's an action connected
   *   to a signal.
   *
   * The proper way to detect a shape going away is to connect to the
   *   destroyed signal (from the parent QObject). Once that is emitted you
   *   cannot call any methods on this object.
   *
   * @author 2016-07-25 Tracie Sucharski
   *
   * @internal
   *   @history 2016-07-25 Tracie Sucharski - Original Version
   *
   */
  class ShapeDisplayProperties : public DisplayProperties {
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


      ShapeDisplayProperties(QString displayName, QObject *parent = NULL);
      virtual ~ShapeDisplayProperties();

//      void fromPvl(const PvlObject &pvl);
//      PvlObject toPvl() const;

      void addSupport(Property prop);
      bool supports(Property prop);

      QVariant getValue(Property prop) const;

      static QColor randomColor();

      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;

    signals:
      void propertyChanged(ShapeDisplayProperties *);
      void supportAdded(Property);

    public slots:
      void setColor(QColor newColor);
      void setShowLabel(bool);
      void setSelected(bool);

    private slots:
      void toggleShowLabel();

    private:
      ShapeDisplayProperties(const ShapeDisplayProperties &);
      ShapeDisplayProperties &operator=(const ShapeDisplayProperties &);

      void setValue(Property prop, QVariant value);
      static QList<ShapeDisplayProperties *> senderToData(QObject *sender);

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

Q_DECLARE_METATYPE(QList<Isis::ShapeDisplayProperties *>);

#endif
