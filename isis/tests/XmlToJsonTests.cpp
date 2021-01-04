#include <nlohmann/json.hpp>

#include "XmlToJson.h"

#include "gmock/gmock.h"

using ordered_json = nlohmann::ordered_json;
using namespace Isis;

TEST(XmlToJson, TestXMLParsingEverything) {
  QString xmlInput = R"(<TagLevel0>
  <TagLevel1A>
    <TagLevel2A>TagLevel2AValue</TagLevel2A>
    <TagLevel2B>TagLevel2BValue</TagLevel2B>
    <TagLevel2Extra attr="justAnAttribute" />
    <TagLevel2ExtraExtra />
    <TagLevel2C>
      <TagLevel3>
        <TagLevel4A>TagLevel4AValue</TagLevel4A>
        <TagLevel4B>TagLevel4BValue</TagLevel4B>
        <TagLevel4C> <TagLevel4D> <TagLevel4E>DeepValue</TagLevel4E></TagLevel4D></TagLevel4C>
     </TagLevel3>
  </TagLevel2C>
  <TagLevel2D attributeTag2D="Attribute value">TagLevel2DValue</TagLevel2D>
  </TagLevel1A>
  <TagLevel1B>
      <First>
       <A>A1</A>
       <A>A2</A>
       <A attribute="value"/>
       <A attr="val">zoom</A>
       <A>
         <B>b1</B>
         <B>b2</B>
         <C>notlist</C>
       </A>
       <A>A3</A>
       <A />
       <ten>10</ten>
       <ten>TEN</ten>
       <oddball>notrepeated</oddball>
      </First>
      <First>
        <tweleve>12</tweleve>
        <thirteen>13</thirteen>
      </First>
      <First>
        <fourteen>14</fourteen>
        <fifteen>15</fifteen>
      </First>
      <Second>
          <A attributeA="A" attributeB="B" attributeC="C">ElementValue</A>
      </Second>
      <Third>
        <Greek attr="alphabet" otherattr="firstLetter" >
          <Alpha attr1="attr1" attr2="attr2">AlphaValue</Alpha>
          <Beta attrbeta1="beta1" attrbeta2="beta2">BetaValue</Beta>
          <AnotherLevel>
            <Gamma attrgamma="gamma">GammaValue</Gamma>
          </AnotherLevel>
        </Greek>
      </Third>
  </TagLevel1B>
</TagLevel0>)";

  QDomDocument xmlDocument("TestDocument");
  xmlDocument.setContent(xmlInput);
  ordered_json result = xmlToJson(xmlDocument);
  std::cout << result.dump(4) << std::endl;

  // Test deeply nested value retrieval (uncomplicated)
  EXPECT_EQ(result["TagLevel0"]["TagLevel1A"]["TagLevel2B"], "TagLevel2BValue");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1A"]["TagLevel2C"]["TagLevel3"]["TagLevel4A"], "TagLevel4AValue");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1A"]["TagLevel2C"]["TagLevel3"]["TagLevel4C"]["TagLevel4D"]["TagLevel4E"], "DeepValue");

  // Test attributes (uncomplicated)
  EXPECT_EQ(result["TagLevel0"]["TagLevel1A"]["TagLevel2D"]["@attributeTag2D"], "Attribute value");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1A"]["TagLevel2D"]["#text"], "TagLevel2DValue");

  // Test no-text value cases <tag /> and <tag attributeName="attributeValue" /> 
  EXPECT_EQ(result["TagLevel0"]["TagLevel1A"]["TagLevel2ExtraExtra"], nullptr);
  EXPECT_EQ(result["TagLevel0"]["TagLevel1A"]["TagLevel2Extra"]["@attr"], "justAnAttribute");

  // Test list creation for repeated tags at the same level
  
  // Case A: <a><b>bcontents</b></a> <a><c>cContents</c></a> JSON: a: [ {b: bcontents}, {c: cContents} ]
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][1]["tweleve"], "12");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][1]["thirteen"], "13");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][2]["fourteen"], "14");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][2]["fifteen"], "15");

  // Case B: <z><a>aContents1</a><a>aContents2</a></a> JSON: z: {a: [aContents1, aContents2] }
  // including lots of possible combinations
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][0]["A"][0], "A1");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][0]["A"][1], "A2");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][0]["A"][2]["@attribute"], "value");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][0]["A"][3]["@attr"], "val");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][0]["A"][3]["#text"], "zoom");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][0]["A"][5], "A3");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][0]["A"][6], nullptr);

  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][0]["oddball"], "notrepeated");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][0]["A"][4]["B"][0], "b1");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][0]["A"][4]["B"][1], "b2");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][0]["A"][4]["C"], "notlist");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][0]["ten"][0], "10");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["First"][0]["ten"][1], "TEN");

  // Test many attributes at one level
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["Second"]["A"]["@attributeA"], "A");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["Second"]["A"]["@attributeB"], "B");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["Second"]["A"]["@attributeC"], "C");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["Second"]["A"]["#text"], "ElementValue");


  // Test multiple attributes at multiple levels
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["Third"]["Greek"]["@otherattr"], "firstLetter");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["Third"]["Greek"]["@attr"], "alphabet");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["Third"]["Greek"]["Alpha"]["@attr2"], "attr2");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["Third"]["Greek"]["Alpha"]["@attr1"], "attr1");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["Third"]["Greek"]["Alpha"]["#text"], "AlphaValue");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["Third"]["Greek"]["Beta"]["#text"], "BetaValue");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["Third"]["Greek"]["Beta"]["@attrbeta2"], "beta2");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["Third"]["Greek"]["AnotherLevel"]["Gamma"]["@attrgamma"], "gamma");
  EXPECT_EQ(result["TagLevel0"]["TagLevel1B"]["Third"]["Greek"]["AnotherLevel"]["Gamma"]["#text"], "GammaValue");
}


