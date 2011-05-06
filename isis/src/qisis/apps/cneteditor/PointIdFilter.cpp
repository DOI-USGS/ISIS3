#include "IsisDebug.h"

#include "PointIdFilter.h"

#include <QHBoxLayout>
#include <QLineEdit>

#include "ControlPoint.h"


namespace Isis
{
  PointIdFilter::PointIdFilter() : AbstractFilter()
  {
    nullify();
    createWidget();
  }


  PointIdFilter::~PointIdFilter()
  {
  }


  void PointIdFilter::nullify()
  {
    AbstractFilter::nullify();

    lineEdit = NULL;
  }


  void PointIdFilter::createWidget()
  {
    AbstractFilter::createWidget();

    lineEdit = new QLineEdit;
    connect(lineEdit, SIGNAL(textChanged(QString)),
        this, SIGNAL(filterChanged()));
    mainLayout->insertWidget(0, lineEdit);
  }


  /**
   * Given a point to evaluate, return true if it makes it through the filter,
   * and false otherwise.  Criteria defining the filter is defined in this
   * method.  Note that whether the filter is inclusive or exclusive is handled
   * in this method.
   *
   * @param point The point to evaluate
   *
   * @returns True if the point makes it through the filter, false otherwise
   */
  bool PointIdFilter::evaluate(const ControlPoint * point) const
  {
    bool match = ((QString) point->GetId()).startsWith(lineEdit->text());
    
    //                     match
    //                    T     F
    //                  ___________
    //                 |     |     |
    //              T  |  T  |  F  |
    // inclusive()     |_____|_____|
    //                 |     |     |
    //              F  |  F  |  T  |
    //                 |_____|_____|
    
    return !(inclusive() ^ match);
  }


  bool PointIdFilter::evaluate(const ControlMeasure * measure) const
  {
    return true;
  }


  bool PointIdFilter::evaluate(const ControlCubeGraphNode * node) const
  {
    return true;
  }
}
