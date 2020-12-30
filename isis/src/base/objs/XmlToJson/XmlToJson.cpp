/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */

#include "XmlToJson.h"
#include "IException.h"

#include <nlohmann/json.hpp>
#include <iostream>

#include <QDomDocument>
#include <QDomElement>
#include <QFile>

using ordered_json = nlohmann::ordered_json;

namespace Isis {

/**
 * Converts an XML file to a json object. 
 *  
 * Here are some details: 
 *  
 * 
 * @param xmlFile Path to an XML file.
 * 
 * @return ordered_json The xml file converted to a json object.
 */
ordered_json xmlToJson(QString xmlFile) {
  QDomDocument doc("xmlInput");
  QFile file(xmlFile);

  if (!file.open(QIODevice::ReadOnly)) {
    QString message = QString("Failed to open file for XML Input: [%1]").arg(xmlFile);
    throw IException(IException::Io, message, _FILEINFO_);
  }

  if (!doc.setContent(&file)) {
    file.close();
    QString message = QString("Failed to use file for XML Input: [%1]").arg(xmlFile);    
    throw IException(IException::Io, message, _FILEINFO_);
  }

  file.close();

  return xmlToJson(doc);
}

  
/**
 * Converts an XML document stored in a QDomDocument into a JSON 
 * object. 
 *  
 * @param doc A QDomDocument with an XML file loaded into it.
 * 
 * @return ordered_json The XMl file converted to a json object.
 */
ordered_json xmlToJson(QDomDocument& doc) {
  QDomElement docElem = doc.documentElement();
  ordered_json output;
  return convertXmlToJson(docElem, output);
}
  

/**
 * Not intended to be used directly. Converts a QDomElement to JSON 
 * and adds it to the passed-in JSON object. Only called when a QDomElement 
 * has no further child nodes 
 * 
 * @param element A QDomElement to be converted to JSON and added to the JSON object.
 * @param output The JSON object to add the converted QDomElement to.
 */
void addLastChildNodeToJson(QDomElement& element, ordered_json& output){
//  std::cout << "passed in:" << output.dump(4) << std::endl;
  if (!output.is_array()) {
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
//      std::cout << "returning: " << output.dump(4) << std::endl;
    }
  }
  else {
//    std::cout << "we're here" << std::endl;
//    std::cout << "output size: " << output.size() << std::endl;
    ordered_json objout;
    objout[element.tagName().toStdString()] = element.text().toStdString(); 
    output += objout;
//    std::cout << "here's the one: " << output.dump() << std::endl;
  }
}

//ordered_json convertXmlListToJson(QDomElement& element, ordered_json& output) {
//
//}


/**
 * Not intended to be used directly. Intended to be used by xmlToJson to convert 
 * an input XML document to JSON. 
 * 
 * @param element A QDomElement representing the whole or some subset of a QDomDocument
 * @param output A JSON object constructed from XML input.
 * 
 * @return ordered_json 
 */
ordered_json convertXmlToJson(QDomElement& element, ordered_json& output) {
  int i = 0;
  while (!element.isNull()) {
    if (element.hasChildNodes() ) {//&& !output.contains(element.tagName().toStdString())) {
      QDomElement next = element.firstChildElement();
      if (next.isNull()) {
        addLastChildNodeToJson(element, output);
      }
      else {
//        std::cout << element.tagName().toStdString() << std::endl;
        // If there is already an element with this tag name, add it to a list rather than 
        // overwriting
        if (output.contains(element.tagName().toStdString())) {
          std::cout << "being identified: " << element.tagName().toStdString() << std::endl;
          // if it's a list already, append, else make it a list
          if (output[element.tagName().toStdString()].is_array()) {
//            std::cout << "it's already a list!" << std::endl;
//            std::cout << output[element.tagName().toStdString()] << std::endl;
            //convertXmlToJson(next, output[element.tagName().toStdString()]);
            ordered_json temporaryJson;
            convertXmlToJson(next, temporaryJson);
            output[element.tagName().toStdString()].push_back(temporaryJson);
          }
          else {
/*            std::cout << "it's not a list!" << std::endl;
            std::cout <<  element.tagName().toStdString() << std::endl;
            std::cout << output[element.tagName().toStdString()] << std::endl;
            std::cout << "DONE";*/
            //output[element.tagName().toStdString()] = { output[element.tagName().toStdString()], convertXmlToJson(next, output[element.tagName().toStdString()]) }; 
            ordered_json temporaryJson;
            convertXmlToJson(next, temporaryJson);
            output[element.tagName().toStdString()] = {output[element.tagName().toStdString()], temporaryJson};

//            output[element.tagName().toStdString()].push_back(convertXmlToJson(next, output[element.tagName().toStdString()]));
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
                convertXmlToJson(next, output[element.tagName().toStdString()]));
            output[element.tagName().toStdString()] = tempArea;
          }
          else {
            // Otherwise, just add element and its value
            output[element.tagName().toStdString()] = convertXmlToJson(next, output[element.tagName().toStdString()]);
          }
        }
      }
    }
    element = element.nextSiblingElement();
    i++;
  }
  return output;
}

}

