#include <iostream>

#include <inja.hpp>

#include "XmlToJson.h"
#include <QDomElement>
#include <QDomAttr>
#include <QDomDocument>
#include <QString>
#include <nlohmann/json.hpp>
#include "topds4.h"

using namespace std;
using namespace inja;
using ordered_json = nlohmann::ordered_json;

namespace Isis {
  
/*ordered_json convertXMLToJson(QDomElement& node, ordered_json& output);
void addElementToJson(QDomElement& e, ordered_json& output);*/

  PvlGroup topds4(UserInterface &ui) {
    Cube *icube = new Cube();
    icube->open(ui.GetFileName("FROM"));  
    return topds4(icube, ui);
  }

  
  PvlGroup topds4(Cube *cube, UserInterface &ui) {
    Process p;
    p.SetInputCube(cube);

    Pvl &label = *cube->label();

    ordered_json data;
    data["name"] = "world";
    render("Hello {{ name }}!", data); 
    render_to(std::cout, "Hello {{ name }}!", data); 

    // Read XML into doc
    QDomDocument doc("xmlInput");
    QFile file("/home/kberry/Downloads/cassisMosaic_with_ck.xml");//templateFileName);

    if (!file.open(QIODevice::ReadOnly))
      std::cout << "failed" << std::endl;
    if (!doc.setContent(&file)) {
      file.close();
      std::cout << "failed" << std::endl;
    }

    file.close();

    // Example for Stuart

//    QDomElement element = doc.documentElement();
//    QDomElement newElement = doc.createElement("XmlOriginalLabel");
//    newElement.insertAfter(newElement, doc.firstChildElement()));
//    element.appendChild(newElement);
//    std::cout << doc.toString() << std::endl; 
//    QDomNode documentRootNode = element.firstChild();
//    element.insertAfter(documentRootNode,)
    
     QDomDocument newDoc("mydoc"); 
     QDomElement newElt = newDoc.createElement("XmlOriginalLabel");
     newDoc.appendChild(newElt);
     newElt.appendChild(doc.documentElement()); 
     std::cout << newDoc.toString() << std::endl; 

//
//// populate json for output
//ordered_json output;
//convertXmlToJson(docElem, output);
//
//std::cout << "Parsed Json" << std::endl;
//std::cout << output.dump(4) << std::endl;
//
    return label.findGroup("Dimensions", Pvl::Traverse);
  }

  /*
  void addElementToJson(QDomElement& element, ordered_json& output){
    if (element.hasAttributes()) {
      // If there are attributes, add them
      ordered_json tempArea;
      QDomNamedNodeMap attrMap = element.attributes();
      for (int i=0; i < attrMap.size(); i++) {
        QDomAttr attr = attrMap.item(i).toAttr(); 
        tempArea["@"+attr.name().toStdString()] = attr.value().toStdString();
      }
      tempArea["#text"] = element.text().toStdString();
      output[element.tagName().toStdString()] = tempArea;
    }
    else {
      // just add element and its value
      output[element.tagName().toStdString()] = element.text().toStdString(); 
    }
  }

  ordered_json convertXMLToJson(QDomElement& element, ordered_json& output) {
    int i = 0;
    while (!element.isNull()) {
      if (element.hasChildNodes()) {
        QDomElement next = element.firstChildElement();
        if (next.isNull()) {
          addElementToJson(element, output);
        }
        else {
          // If there is already an element with this tag name, add it to a list rather than
          // overwriting
          if (output.contains(element.tagName().toStdString())) {
            // if it's a list already, append, else make it a list
            if (output[element.tagName().toStdString()].is_array()) {
              output[element.tagName().toStdString()].push_back(convertXMLToJson(next, output[element.tagName().toStdString()]));
            }
            else{
              output[element.tagName().toStdString()] = { output[element.tagName().toStdString()], convertXMLToJson(next, output[element.tagName().toStdString()]) }; 
             }
          }
          else {
            if (element.hasAttributes()) {
              // If there are attributes, add them
              ordered_json tempArea;
              QDomNamedNodeMap attrMap = element.attributes();
              for (int i=0; i < attrMap.size(); i++) {
                QDomAttr attr = attrMap.item(i).toAttr(); 
                tempArea["@"+attr.name().toStdString()] = attr.value().toStdString();
              }
              tempArea.update(
                  convertXMLToJson(next, output[element.tagName().toStdString()]));
              output[element.tagName().toStdString()] = tempArea;
            }
            else {
              // just add element and its value
              output[element.tagName().toStdString()] = convertXMLToJson(next, output[element.tagName().toStdString()]);
            }
          }
        }
      }
      element = element.nextSiblingElement();
      i++;
    }
    return output;
  }*/
}

