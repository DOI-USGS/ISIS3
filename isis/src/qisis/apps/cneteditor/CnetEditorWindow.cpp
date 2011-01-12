#include "CnetEditorWindow.h"

#include "CnetEditorWidget.h"
#include "ControlMeasure.h"
#include "ControlNet.h"
#include "ControlPoint.h"

using namespace Isis;

namespace Qisis
{

  CnetEditorWindow::CnetEditorWindow() 
  {
    nullify();

    // control point 0
    ControlMeasure cp0cm1;
    cp0cm1.SetCubeSerialNumber("A");
    ControlMeasure cp0cm2;
    cp0cm2.SetCubeSerialNumber("B");
  
    ControlPoint cp0("0");
    cp0.Add(cp0cm1);
//    cp0.Add(cp0cm2);
  
  
    // control point 1
    ControlMeasure cp1cm1;
    cp1cm1.SetCubeSerialNumber("A");
    ControlMeasure cp1cm2;
    cp1cm2.SetCubeSerialNumber("B");
    ControlMeasure cp1cm3;
    cp1cm3.SetCubeSerialNumber("C");
  
    ControlPoint cp1("1");
    cp1.Add(cp1cm1);
//    cp1.Add(cp1cm2);
//    cp1.Add(cp1cm3);
  
  
    // control point 2
    ControlMeasure cp2cm1;
    cp2cm1.SetCubeSerialNumber("A");
    ControlMeasure cp2cm2;
    cp2cm2.SetCubeSerialNumber("B");
    ControlMeasure cp2cm3;
    cp2cm3.SetCubeSerialNumber("C");
  
    ControlPoint cp2("2");
    cp2.Add(cp2cm1);
    cp2.Add(cp2cm2);
    cp2.Add(cp2cm3);
  
  
    // control point 3
    ControlMeasure cp3cm1;
    cp3cm1.SetCubeSerialNumber("B");
    ControlMeasure cp3cm2;
    cp3cm2.SetCubeSerialNumber("C");
  
    ControlPoint cp3("3");
    cp3.Add(cp3cm1);
    cp3.Add(cp3cm2);
  
  
    // control point 4
    ControlMeasure cp4cm1;
    cp4cm1.SetCubeSerialNumber("B");
    ControlMeasure cp4cm2;
    cp4cm2.SetCubeSerialNumber("C");
  
    ControlPoint cp4("4");
    cp4.Add(cp4cm1);
    cp4.Add(cp4cm2);
  
  
    // control point 5
    ControlMeasure cp5cm1;
    cp5cm1.SetCubeSerialNumber("D");
    ControlMeasure cp5cm2;
    cp5cm2.SetCubeSerialNumber("E");
  
    ControlPoint cp5("5");
    cp5.Add(cp5cm1);
    cp5.Add(cp5cm2);
  //  cp5.SetIgnore(true);
  
  
    // now build controlnet
    ControlNet * cnet = new ControlNet;
    cnet->Add(cp0);
    cnet->Add(cp1);
//    cnet->Add(cp2);
//    cnet->Add(cp3);
//    cnet->Add(cp4);
//    cnet->Add(cp5);

    editorWidget = new CnetEditorWidget(cnet);
    setCentralWidget(editorWidget);
  }

  CnetEditorWindow::~CnetEditorWindow() 
  {

  }

  void CnetEditorWindow::nullify()
  {
    editorWidget = NULL;
  }
}
