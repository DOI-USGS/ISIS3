#ifndef ChooserNameFilter_H
#define ChooserNameFilter_H

#include "AbstractFilter.h"


class QLineEdit;
class QString;


namespace Isis
{
  class ControlPoint;

  class ChooserNameFilter : public AbstractFilter
  {
      Q_OBJECT

    public:
      ChooserNameFilter(int minimumForImageSuccess = -1);
      virtual ~ChooserNameFilter();

      bool evaluate(const ControlPoint *) const;
      bool evaluate(const ControlMeasure *) const;
      bool evaluate(const ControlCubeGraphNode *) const;
      QString getDescription() const;


    protected:
      void nullify();
      void createWidget();


    private:
      QLineEdit * lineEdit;
  };
}

#endif
