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

using json = nlohmann::json;

namespace Isis {
   json convertLastChildNodeToJson(QDomElement& element);
   json convertXmlToJson(QDomElement& element, json& output);

  /**
   * Converts an XML file to a json object. 
   *  
   * Please see other functions for details about how XML elements are 
   * converted to corresponding json elements.
   * 
   * @param xmlFile Path to an XML file.
   * 
   * @return json The xml file converted to a json object.
   */
  json xmlToJson(QString xmlFile) {
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
   * @return json The XMl file converted to a json object.
   */
  json xmlToJson(QDomDocument& doc) {
    QDomElement docElem = doc.documentElement();
    json output;
    return convertXmlToJson(docElem, output);
  }
    

  /**
   * Not intended to be used directly. Converts a QDomElement to JSON 
   * and returns. Only called when a QDomElement 
   * has no further child nodes 
   *  
   * Used for the following situations: 
   *  
   * XML: <tag>value</tag>
   * JSON: {tag: value}
   * 
   *  XML: <tag attributeName="attributeValue">textValue</tag>
   *  JSON: {tag: {@attributeName: "attributeValue, "#text":textValue } }
   *
   *  XML: <tag attributeName="attributeValue" />
   *  JSON: {tag: {@attributeName: "attributeValue"} }
   * 
   *  XML: <tag />
   *  JSON: tag: null
   * 
   * @param element A QDomElement to be converted to JSON and added to the JSON object.
   */
  json convertLastChildNodeToJson(QDomElement& element){
    json newJson;
    if (element.hasAttributes()) {
      // If there are attributes, add them
      // <tag attributeName="attributeValue">textValue</tag>
      json attributeSection;
      QDomNamedNodeMap attrMap = element.attributes();
      for (int i=0; i < attrMap.size(); i++) {
        QDomAttr attr = attrMap.item(i).toAttr(); 
        attributeSection["@"+attr.name().toStdString()] = attr.value().toStdString();
      }
      // If there is no textValue, don't include it
      // <tag attributeName="attributeValue" />
      if (!element.text().isEmpty()) {
        attributeSection["#text"] = element.text().toStdString(); 
      }
      newJson[element.tagName().toStdString()] = attributeSection;
    }
    else {
      // Just add element and its value
      // <tag>value</tag>
      if (!element.text().isEmpty()) {
        newJson[element.tagName().toStdString()] = element.text().toStdString();
      }
      else {
        // <tag /> no value case
        newJson[element.tagName().toStdString()];
      }
    }
    return newJson;
  }


  /**
   * Not intended to be used directly. Intended to be used by xmlToJson to convert 
   * an input XML document to JSON. 
   *  
   * This function does the following conversions: 
   *  
   * XML: <a><b>val1</b><c>val2</c></a> 
   * JSON: a : {b: val1, c: val2}
   *  
   * XML: <a> <first>value1</first> </a> <a> <second>value2</second></a>
   * JSON: a: [ {first:value1}, {second:value2} ]
   *  
   * XML: <a>val1</a><a>val2</a> 
   * JSON: a:[val1, val2]
   *  
   * @param element A QDomElement representing the whole or some subset of a QDomDocument
   * @param output A JSON object constructed from XML input.
   * 
   * @return json 
   */
  json convertXmlToJson(QDomElement& element, json& output) {
    while (!element.isNull()) {
      QDomElement next = element.firstChildElement();
      if (next.isNull()){
        json converted = convertLastChildNodeToJson(element);
        // Simple case with no repeated tags at the same level
        if (!output.contains(element.tagName().toStdString())){
          output.update(converted);
        }
        else {
          // There is a repeated tag at the same level in the XML, i.e: <a>val1</a><a>val2</a>
          // Translated json goal: a:[val1, val2]
          // If the converted json has an array already, append, else make it an array
          if (!output[element.tagName().toStdString()].is_array()) {
            json repeatedArray;
            repeatedArray.push_back(output[element.tagName().toStdString()]);
            output[element.tagName().toStdString()] = repeatedArray;
          }
          output[element.tagName().toStdString()].push_back(converted[element.tagName().toStdString()]);
        }
      }
      else {
        // If there is already an element with this tag name at any level besides the same one,
        // add it to a list rather than 
        // overwriting. This is the following situation:
        // XML: <a> <first>value1</first> </a> <a> <second>value2</second></a>
        // JSON: a: [ {first:value1, second:value2} ]
        if (output.contains(element.tagName().toStdString())) {
          // If it's an array already, append, else make it an array
          json temporaryJson;
          convertXmlToJson(next, temporaryJson);
          if (!output[element.tagName().toStdString()].is_array()) {
            json repeatedArray;
            repeatedArray.push_back(output[element.tagName().toStdString()]);
            output[element.tagName().toStdString()] = repeatedArray;
          }
          output[element.tagName().toStdString()].push_back(temporaryJson);
        }
        else {
          if (element.hasAttributes()) {
            json tempArea;
            QDomNamedNodeMap attrMap = element.attributes();
            for (int j=0; j < attrMap.size(); j++) {
              QDomAttr attr = attrMap.item(j).toAttr(); 
              tempArea["@"+attr.name().toStdString()] = attr.value().toStdString();
            }
            tempArea.update(
                convertXmlToJson(next, output[element.tagName().toStdString()]));
            output[element.tagName().toStdString()] = tempArea;
          }
          else {
            output[element.tagName().toStdString()] = 
              convertXmlToJson(next, output[element.tagName().toStdString()]);
          }
        }
      }
      element = element.nextSiblingElement();
    }
    return output;
  }
}

