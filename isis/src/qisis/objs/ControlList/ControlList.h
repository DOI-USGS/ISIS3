#ifndef ControlList_H
#define ControlList_H

/** This is free and unencumbered software released into the public domain.

The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include <QDebug>
#include <QList>
#include <QMetaType>
#include <QObject>

#include "Control.h"
#include "ControlDisplayProperties.h"
#include "XmlStackedHandler.h"

class QStringList;
class QXmlStreamWriter;

namespace Isis {
  class FileName;

  /**
   * Maintains a list of Controls so that control nets can easily be copied from one Project to
   * another, saved to disk, or deleted from disk. Overrides several common QList methods for
   * managing a list of Controls as well. Adapted from ImageList
   *
   * @author 2012-09-01 Tracie Sucharski
   *
   * @internal
   *   @history 2012-09-01 Tracie Sucharski - Original version.
   *   @history 2015-10-14 Jeffrey Covington - Declared ControlList * as a Qt
   *                           metatype for use with QVariant.
   *   @history 2016-06-06 Ian Humphrey - Updated documentation and coding standards. Fixes #3959.
   *   @history 2017-05-05 Tracie Sucharski - Removed Workorder.h, never used.
   *   @history 2017-12-08 Tracie Sucharski - When saving project only copy the control if project
   *                           is being saved to a new location.
   */
  class ControlList : public QObject, public QList<Control *> {
    Q_OBJECT

    public:

      ControlList(QString name, QString path, QObject *parent = NULL);
      explicit ControlList(QObject *parent = NULL);
      explicit ControlList(QList<Control *>, QObject *parent = NULL);
      explicit ControlList(QStringList &);
      ControlList(const ControlList &);
      ~ControlList();

      // These are overridden (-ish) in order to add notifications to the list changing
      void append(Control * const &value);
      void append(const QList<Control *> &value);

      void clear();

      iterator erase(iterator pos);
      iterator erase(iterator begin, iterator end);

      void insert(int i, Control * const &value);
      iterator insert(iterator before, Control * const &value);

      void prepend(Control * const &value);
      void push_back(Control * const &value);
      void push_front(Control * const &value);
      int removeAll(Control * const &value);
      void removeAt(int i);
      void removeFirst();
      void removeLast();
      bool removeOne(Control * const &value);
      void swap(QList<Control *> &other);
      Control *takeAt(int i);
      Control *takeFirst();
      Control *takeLast();

      ControlList &operator+=(const QList<Control *> &other);
      ControlList &operator+=(Control * const &other);
      ControlList &operator<<(const QList<Control *> &other);
      ControlList &operator<<(Control * const &other);
      ControlList &operator=(const QList<Control *> &rhs);

      // This is our own assignment, but it needs to notify just like the operator=(QList)
      ControlList &operator=(const ControlList &rhs);

      // Done overriding (-ish)


      QList<QAction *> supportedActions(Project *project = NULL);
      bool allSupport(ControlDisplayProperties::Property prop);

      void setName(QString newName);
      void setPath(QString newPath);

      QString name() const;
      QString path() const;

      void deleteFromDisk(Project *project);
      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;


    signals:
      void countChanged(int newCount);
      void deletingList(ControlList *list);

    private:


      /**
       * This functor is used for copying the control nets between two projects quickly. This is
       * designed to work with QtConcurrentMap, though the results are all NULL (QtConcurrentMap
       * is much faster than many QtConcurrentRun calls).
       *
       * @author 2012-10-11 Tracie Sucharski - Adapted from Copy ImageDataFunctor
       *
       * @internal
       *   @history 2012-10-11 Tracie Sucharski - Original version.
       *
       */
      class CopyControlDataFunctor : public std::function<void *(Control * const &)> {
        public:
          CopyControlDataFunctor(const Project *project, FileName newProjectRoot);
          CopyControlDataFunctor(const CopyControlDataFunctor &other);
          ~CopyControlDataFunctor();

          void *operator()(Control * const &controlToCopy);

          CopyControlDataFunctor &operator=(const CopyControlDataFunctor &rhs);

        private:
          const Project *m_project;  //!< Project to copy the control list to
          FileName m_newProjectRoot; //!< The filename of the destination project's root
      };

      /**
       * Nested class used to write the ControlList object information to an XML file for the
       * purposes of saving an restoring the state of the object.
       *
       * @see ControlList::save for the expected format
       *
       * @author 2012-09-27 Tracie Sucharski - Adapted from ImageList::XmlHandler
       *
       * @internal
       *   @history 2012-09-27 Tracie Sucharski - Original version.
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(ControlList *controlList, Project *project);

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                  const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          ControlList *m_controlList; //!< Control list to be read or written
          Project *m_project; //!< Project that contains the control list
      };


    private:
      QString m_name; //!< Name of the ControlList

      /**
       * This stores the directory name that contains the controls in this control list.
       *
       * For example:
       *   import1
       * or
       *   import2
       *
       * This path is relative to Project::controlNetRoot()
       */
      QString m_path;
  };
}

Q_DECLARE_METATYPE(Isis::ControlList *);

#endif
