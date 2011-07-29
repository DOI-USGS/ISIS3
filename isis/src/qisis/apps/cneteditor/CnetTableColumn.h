#ifndef CnetTableColumn_H
#define CnetTableColumn_H

#include <QObject>

class QString;


namespace Isis
{
  class CnetTableColumn : public QObject
  {

      Q_OBJECT

    public:
      explicit CnetTableColumn(QString, bool);
      CnetTableColumn(const CnetTableColumn &);
      virtual ~CnetTableColumn();
      QString getTitle() const;
      void setTitle(QString text);
      CnetTableColumn & operator=(CnetTableColumn);
      bool isVisible() const;
      void setVisible(bool);
      int getWidth() const;
      void setWidth(int);
      bool isReadOnly() const;
      void setReadOnly(bool);

    public:
      static const int EDGE_WIDTH = 4;


    signals:
      void selected(CnetTableColumn *);
      void widthChanged();
      void visibilityChanged();


    private: // methods
      void nullify();


    private: // data
      QString * title;
      bool visible;
      bool readOnly;
      int width;
  };
}

#endif

