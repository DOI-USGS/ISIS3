#ifndef PointIdFilter_H
#define PointIdFilter_H

#include "AbstractFilter.h"


class QLineEdit;
class QString;


namespace Isis
{
  class ControlPoint;

  class PointIdFilter : public AbstractFilter
  {
      Q_OBJECT

    public:
      PointIdFilter(AbstractFilter::FilterEffectivenessFlag flag,
                    int minimumForImageSuccess = -1);
      virtual ~PointIdFilter();
      
      bool evaluate(const ControlCubeGraphNode *) const;
      bool evaluate(const ControlPoint *) const;

      QString getImageDescription() const;
      QString getPointDescription() const;


    protected:
      void nullify();
      void createWidget();
      
      
    private slots:
      void updateLineEditText(QString);


    private:
      QLineEdit * lineEdit;
      QString * lineEditText;
  };
}

#endif
