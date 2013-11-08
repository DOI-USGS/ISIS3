#ifndef ControlList_H
#define ControlList_H

#include <QDebug>
#include <QObject>
#include <QList>
#include <QMetaType>

#include "Control.h"
#include "ControlDisplayProperties.h"
#include "WorkOrder.h"
#include "XmlStackedHandler.h"

class QStringList;
class QXmlStreamWriter;

namespace Isis {
  class FileName;
  class XmlStackedHandlerReader;

  /**
   * Adapted from ImageList
   *
   * @author 2012-09-01 Tracie Sucharski 
   *
   * @internal 
   *   @history 2012-09-01 Tracie Sucharski - Original version. 
   */
  class ControlList : public QObject, public QList<Control *> {
    Q_OBJECT

    public:

      ControlList(QString name, QString path, QObject *parent = NULL);
      explicit ControlList(QObject *parent = NULL);
      explicit ControlList(QList<Control *>, QObject *parent = NULL);
      explicit ControlList(Project *project,
                         XmlStackedHandlerReader *xmlReader, QObject *parent = NULL);
      explicit ControlList(QStringList &);
      ControlList(const ControlList &);
      ~ControlList();

      // These are overridden (-ish) in order to add notifications to the list changing
      void append(Control * const & value);
      void append(const QList<Control *> &value);

      void clear();

      iterator erase(iterator pos);
      iterator erase(iterator begin, iterator end);

      void insert(int i, Control * const & value);
      iterator insert(iterator before, Control * const & value);

      void prepend(Control * const & value);
      void push_back(Control * const & value);
      void push_front(Control * const & value);
      int removeAll(Control * const & value);
      void removeAt(int i);
      void removeFirst();
      void removeLast();
      bool removeOne(Control * const & value);
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
       *   designed to work with QtConcurrentMap, though the results are all NULL (QtConcurrentMap
       *   is much faster than many QtConcurrentRun calls).
       *
       * @author 2012-10-11 Tracie Sucharski - Adapted from Copy ImageDataFunctor
       *
       * @internal
       */
      class CopyControlDataFunctor : public std::unary_function<Control * const &, void *> {
        public:
          CopyControlDataFunctor(const Project *project, FileName newProjectRoot);
          CopyControlDataFunctor(const CopyControlDataFunctor &other);
          ~CopyControlDataFunctor();

          void *operator()(Control * const &controlToCopy);

          CopyControlDataFunctor &operator=(const CopyControlDataFunctor &rhs);

        private:
          const Project *m_project;
          FileName m_newProjectRoot;
      };

      /**
       * @author 2012-09-27 Tracie Sucharski
       *
       * @internal 
       *   @history Adapted from ImageList::XmlHandler
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

          ControlList *m_controlList;
          Project *m_project;
      };


    private:
      QString m_name;

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

#endif
