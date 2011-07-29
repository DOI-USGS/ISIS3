#ifndef CnetTreeView_H
#define CnetTreeView_H

#include <QWidget>


namespace Isis
{
  class CnetTreeViewContent;
  class CnetTreeViewHeader;
  class TreeModel;

  class CnetTreeView : public QWidget
  {
      Q_OBJECT

    signals:
      void activated();
      void selectionChanged();


    public:
      CnetTreeView(QWidget * parent = 0);
      virtual ~CnetTreeView();
      QSize sizeHint();
      QFont getContentFont() const;
      void setModel(TreeModel * someModel);
      TreeModel * getModel() const;
      bool isActive() const;
      QString getTitle() const;
      void setTitle(QString someTitle);


    public slots:
      void deactivate();
      void activate();
      void onModelSelectionChanged();


    private: // disable copying and assigning of this class
      CnetTreeView(const CnetTreeView &);
      CnetTreeView & operator=(const CnetTreeView & other);


    private: // methods
      void nullify();


    private: // data
      CnetTreeViewHeader * header;
      CnetTreeViewContent * content;
      bool active;
  };
}

#endif
