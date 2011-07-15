#ifndef CnetView_H
#define CnetView_H

#include <QWidget>


namespace Isis
{
  class CnetViewContent;
  class CnetViewHeader;
  class TreeModel;

  class CnetView : public QWidget
  {
      Q_OBJECT

    signals:
      void activated();
      void selectionChanged();


    public:
      CnetView(QWidget * parent = 0);
      virtual ~CnetView();
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


    private: // disable copying and assigning of this class
      CnetView(const CnetView &);
      CnetView & operator=(const CnetView & other);


    private: // methods
      void nullify();


    private: // data
      CnetViewHeader * header;
      CnetViewContent * content;
      bool active;
  };
}

#endif
