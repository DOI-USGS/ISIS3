#ifndef TargetBodyList_H
#define TargetBodyList_H

#include <QDebug>
#include <QObject>
#include <QList>
#include <QMetaType>

#include "TargetBody.h"
#include "TargetBodyDisplayProperties.h"
//#include "TargetBodyListActionWorkOrder.h"  TODO - will we need this?
#include "WorkOrder.h"
#include "XmlStackedHandler.h"

class QStringList;
class QXmlStreamWriter;

namespace Isis {
  class FileName;

  /**
   * List for holding TargetBodies.  Overrides several QList methods in order to emit
   * signals about the list changing.  TargetBodies are stored as QSharedPointer<TargetBody>.
   * 
   * @author 2015-06-11 Ken Edmundson
   *
   * @internal 
   *   @history 2015-06-11 Ken Edmundson - original version
   *   @history 2016-06-17 Jesse Mapel - Updated documentation before merging from IPCE into ISIS.
   *                           Fixes #4007.
   */
  class TargetBodyList : public QObject, public QList<TargetBodyQsp> {
    Q_OBJECT

    public:
//      friend class TargetBodyListActionWorkOrder;

      TargetBodyList(QString name, QString path, QObject *parent = NULL);
      explicit TargetBodyList(QObject *parent = NULL);
      explicit TargetBodyList(QList<TargetBodyQsp>, QObject *parent = NULL);
//    explicit TargetBodyList(QStringList &);
      TargetBodyList(const TargetBodyList &);
      ~TargetBodyList();

      // These are overridden (-ish) in order to add notifications to the list changing
      void append(TargetBodyQsp const &value);
      void append(const QList<TargetBodyQsp> &value);

      void clear();

      iterator erase(iterator pos);
      iterator erase(iterator begin, iterator end);

      void insert(int i, TargetBodyQsp const &value);
      iterator insert(iterator before, TargetBodyQsp const &value);

      void prepend(TargetBodyQsp const &value);
      void push_back(TargetBodyQsp const &value);
      void push_front(TargetBodyQsp const &value);
      int removeAll(TargetBodyQsp const &value);
      void removeAt(int i);
      void removeFirst();
      void removeLast();
      bool removeOne(TargetBodyQsp const &value);
      void swap(QList<TargetBodyQsp> &other);
      TargetBodyQsp takeAt(int i);
      TargetBodyQsp takeFirst();
      TargetBodyQsp takeLast();

      TargetBodyList &operator+=(const QList<TargetBodyQsp> &other);
      TargetBodyList &operator+=(TargetBodyQsp const &other);
      TargetBodyList &operator<<(const QList<TargetBodyQsp> &other);
      TargetBodyList &operator<<(TargetBodyQsp const &other);
      TargetBodyList &operator=(const QList<TargetBodyQsp> &rhs);

      // This is our own assignment, but it needs to notify just like the operator=(QList)
      TargetBodyList &operator=(const TargetBodyList &rhs);

      // Done overriding (-ish)


      QList<QAction *> supportedActions(Project *project = NULL);
      bool allSupport(TargetBodyDisplayProperties::Property prop);

      void setName(QString newName);
      void setPath(QString newPath);

      QString name() const;
      QString path() const;

//      void deleteFromDisk(Project *project);
      void save(QXmlStreamWriter &stream, const Project *project, FileName newProjectRoot) const;


    signals:
      void countChanged(int newCount);

    private:
      /**
       * XmlReader for working with TargetBody XML files
       * 
       * @author 2012-07-01 Steven Lambright
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
        public:
          XmlHandler(TargetBodyList *TargetBodyList, Project *project);

          virtual bool startElement(const QString &namespaceURI, const QString &localName,
                                    const QString &qName, const QXmlAttributes &atts);
          virtual bool endElement(const QString &namespaceURI, const QString &localName,
                                  const QString &qName);

        private:
          Q_DISABLE_COPY(XmlHandler);

          TargetBodyList *m_TargetBodyList; //!< The TargetBodyList to read into/save from
          Project *m_project; //!< The project that contains the TargetBodies
      };


      /**
       * This functor is used for copying the TargetBody objects between two projects quickly. This
       *   is designed to work with QtConcurrentMap, though the results are all NULL 
       *   (QtConcurrentMap is much faster than many QtConcurrentRun calls).
       *
       * @author 2015-06-11 Ken Edmundson (after Steven Lambright)
       *
       * @internal
       */
//      class CopyTargetBodyDataFunctor : public std::unary_function<TargetBodyQsp const &, void *> {
//        public:
//          CopyTargetBodyDataFunctor(const Project *project, FileName newProjectRoot);
//          CopyTargetBodyDataFunctor(const CopyTargetBodyDataFunctor &other);
//          ~CopyTargetBodyDataFunctor();

//          void *operator()(TargetBodyQsp const &imageToCopy);

//          CopyTargetBodyDataFunctor &operator=(const CopyTargetBodyDataFunctor &rhs);

//        private:
//          const Project *m_project;
//          FileName m_newProjectRoot;
//      };

    private:
//      QAction *createWorkOrder(Project *project, TargetBodyListActionWorkOrder::Action action) {
//        QAction *result = NULL;

//        if (project) {
//          result = new TargetBodyListActionWorkOrder(action, project);
//          ((TargetBodyListActionWorkOrder *)result)->setData(this);
//        }
//        else {
//          result = new QAction(
//              TargetBodyListActionWorkOrder::qualifyString(TargetBodyListActionWorkOrder::toString(action),
//                                                      this),
//              this);
//        }

//        return result;
//      }

//      void applyAlphas(QStringList alphaValues);
//      void applyColors(QStringList colorValues, int column = 0);
//      void applyShowLabel(QStringList showLabelValues);
//      void applyShowFill(QStringList showFillValues);
//      void applyShowDNs(QStringList showDNsValues);
//      void applyShowOutline(QStringList showOutlineValues);
//      bool askAlpha(int *alphaResult) const;
//      bool askNewColor(QColor *colorResult) const;
//      QStringList saveAndApplyAlpha(int newAlpha);
//      QStringList saveAndApplyColor(QColor newColor);
//      QStringList saveAndApplyRandomColor();

    private slots:
//      void askAndUpdateAlpha();
//      void askAndUpdateColor();
//      void showRandomColor();
//      QStringList saveAndToggleShowDNs();
//      QStringList saveAndToggleShowFill();
//      QStringList saveAndToggleShowLabel();
//      QStringList saveAndToggleShowOutline();

    private:
      QString m_name; //!< The display name of the TaregetBodyList

      /**
       * This stores the directory name that contains the TargetBody objects in this list.
       *
       * For example:
       *   import1
       * or
       *   import2
       *
       * This path is relative to Project::imageDataRoot()
       */
      QString m_path;
  };

}

Q_DECLARE_METATYPE(Isis::TargetBodyList *);

#endif
