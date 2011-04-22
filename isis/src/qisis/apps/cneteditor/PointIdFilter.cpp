#include "IsisDebug.h"

#include "PointIdFilter.h"

#include <QHBoxLayout>
#include <QLineEdit>

#include "ControlPoint.h"


namespace Isis
{
  PointIdFilter::PointIdFilter() : AbstractPointFilter()
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


  bool PointIdFilter::evaluate(ControlPoint const * point) const
  {
    return ((QString) point->GetId()).startsWith(lineEdit->text());
  }
}
