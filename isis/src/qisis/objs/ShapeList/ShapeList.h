#ifndef ShapeList_H
#define ShapeList_H

#include <QDebug>
#include <QObject>
#include <QList>
#include <QMetaType>

#include "Shape.h"
#include "ShapeDisplayProperties.h"
#include "SerialNumberList.h"
#include "WorkOrder.h"

class QStringList;
class QXmlStreamWriter;

namespace Isis {
  class FileName;

  /**
   * @brief Internalizes a list of shapes and allows for operations on the entire list
   * 
   * This class reads a list of shapes from an shapes.xml file and internalizes them
   * as aQList of shapes.  It also allows for modifications to the entire list of
   * shapes and storing the shape list as an shapes.xml file.
   * 
   * @author 2016-07-25 Tracie Sucharski
   *
   * @internal 
   */
  class ShapeList : public QObject, public QList<Shape *> {
    Q_OBJECT

    public:
      ShapeList(QString name, QString path, QObject *parent = NULL);
      explicit ShapeList(QObject *parent = NULL);
      explicit ShapeList(QList<Shape *>, QObject *parent = NULL);
      explicit ShapeList(QStringList &);
      ShapeList(const ShapeList &);
      ~ShapeList();

      SerialNumberList serialNumberList();

      // These are overridden (-ish) in order to add notifications to the list changing
      void append(Shape * const & value);
      void append(const QList<Shape *> &value);

      void clear();

      iterator erase(iterator pos);
      iterator erase(iterator begin, iterator end);

      void insert(int i, Shape * const & value);
      iterator insert(iterator before, Shape * const & value);

      void prepend(Shape * const & value);
      void push_back(Shape * const & value);
      void push_front(Shape * const & value);
      int removeAll(Shape * const & value);
      void removeAt(int i);
      void removeFirst();
      void removeLast();
      bool removeOne(Shape * const & value);
      void swap(QList<Shape *> &other);
      Shape *takeAt(int i);
      Shape *takeFirst();
      Shape *takeLast();

      ShapeList &operator+=(const QList<Shape *> &other);
      ShapeList &operator+=(Shape * const &other);
      ShapeList &operator<<(const QList<Shape *> &other);
      ShapeList &operator<<(Shape * const &other);
      ShapeList &operator=(const QList<Shape *> &rhs);

      // This is our own assignment, but it needs to notify just like the operator=(QList)
      ShapeList &operator=(const ShapeList &rhs);

      // Done overriding (-ish)

      void setName(QString newName);
      void setPath(QString newPath);

      QString name() const;
      QString path() const;

      void deleteFromDisk(Project *project);
      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;


    signals:
      void countChanged(int newCount);

    private:

      /**
       * This functor is used for copying the shapes between two projects quickly. This is designed
       *   to work with QtConcurrentMap, though the results are all NULL (QtConcurrentMap is much
       *   faster than many QtConcurrentRun calls).
       *
       * @author 2012-09-?? Steven Lambright
       *
       * @internal
       */
      class CopyShapeDataFunctor : public std::function<void *(Shape * const &)> {
        public:
          CopyShapeDataFunctor(const Project *project, FileName newProjectRoot);
          CopyShapeDataFunctor(const CopyShapeDataFunctor &other);
          ~CopyShapeDataFunctor();

          void *operator()(Shape * const &shapeToCopy);

          CopyShapeDataFunctor &operator=(const CopyShapeDataFunctor &rhs);

        private:
          /**
           * This stores the name of the project that is going to be copied to.
           */
          const Project *m_project;
          /**
           * This stores the path to the root of the project that is going to be copied to.
           */
          FileName m_newProjectRoot;
      };

    private slots:

    private:
      /**
       * This stores the shape list's name
       */
      QString m_name;

      /**
       * This stores the directory name that contains the shapes in this shape list.
       *
       * For example:
       *   import1
       * or
       *   import2
       *
       * This path is relative to Project::shapeDataRoot()
       */
      QString m_path;
  };

}

Q_DECLARE_METATYPE(Isis::ShapeList *);

#endif
