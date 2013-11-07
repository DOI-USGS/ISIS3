#include "QnetFilter.h"

#include "ControlNet.h"
#include "QnetNavTool.h"
#include "SerialNumberList.h"

namespace Isis {
  // Constructor
  QnetFilter::QnetFilter(QnetNavTool *navTool, QWidget *parent) : QWidget(parent) {
    m_navTool = navTool;
  }
  
  
  QnetFilter::~QnetFilter() {
    m_navTool = NULL;
  }


  void QnetFilter::filter() {
  }
  
  
  QList<int> &QnetFilter::filteredImages() {
    return m_navTool->filteredImages();
  }


  const QList<int> &QnetFilter::filteredImages() const {
    return m_navTool->filteredImages();
  }


  QList<int> &QnetFilter::filteredPoints() {
    return m_navTool->filteredPoints();
  }


  const QList<int> &QnetFilter::filteredPoints() const {
    return m_navTool->filteredPoints();
  }


  ControlNet *QnetFilter::controlNet() {
    return m_navTool->controlNet();
  }


  const ControlNet *QnetFilter::controlNet() const {
    return m_navTool->controlNet();
  }


  SerialNumberList *QnetFilter::serialNumberList() {
    return m_navTool->serialNumberList();
  }


  const SerialNumberList *QnetFilter::serialNumberList() const {
    return m_navTool->serialNumberList();
  }
}
