#ifndef DisplayProperties_H
#define DisplayProperties_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QObject>

// This is required since QColor is in a slot
#include <QColor>

#include "XmlStackedHandler.h"

class QAction;
class QBitArray;
class QXmlStreamWriter;

namespace Isis {
  class FileName;
  class Project;
  class Pvl;
  class PvlObject;

  /**
   * @author 2012-??-?? ???
   *
   * @internal
   */
  class DisplayProperties : public QObject {
      Q_OBJECT
    public:
      DisplayProperties(QString displayName, QObject *parent = NULL);
      DisplayProperties(XmlStackedHandlerReader *xmlReader, QObject *parent = NULL);
      virtual ~DisplayProperties();

      void fromPvl(const PvlObject &pvl);
      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;
      PvlObject toPvl() const;

      void addSupport(int property);
      bool supports(int property);

      QVariant getValue(int property) const;

      void setDisplayName(QString displayName);
      QString displayName() const;

      static QColor randomColor();

    signals:
      void propertyChanged(DisplayProperties *);
      void supportAdded(int);

    protected:
      void setValue(int prop, QVariant value);

    private:
      Q_DISABLE_COPY(DisplayProperties);

      /**
       * @author 2012-??-?? Steven Lambright
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(DisplayProperties *displayProperties);

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);

          virtual bool characters(const QString &ch);

          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                  const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          DisplayProperties *m_displayProperties;
          QString m_hexData;
      };


    private:

      /**
       * This is the display name
       */
      QString m_displayName;

      /**
       * This indicated whether any widgets with this DisplayProperties
       *   is using a particulay property. This helps others who can set
       *   but not display know whether they should give the option to set.
       */
      int m_propertiesUsed;

      /**
       * This is a map from Property to value -- the reason I use an int is
       *   so Qt knows how to serialize this QMap into binary data
       */
      QMap<int, QVariant> *m_propertyValues;
  };
}

#endif
