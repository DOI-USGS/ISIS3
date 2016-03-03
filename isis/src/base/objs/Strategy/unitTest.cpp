#include "Isis.h"

#include <QFile>
#include <QList>
#include <QPair>
#include <QDebug>

#include "PvlFlatMap.h"
#include "Resource.h"
#include "Strategy.h"
#include "Cube.h"
#include "GisGeometry.h"
#include "Application.h"
#include "Preference.h"
#include "Pvl.h"
#include "PvlGroup.h"
#include "PvlObject.h"


#include "Statistics.h"

using namespace Isis;
using namespace std;


/**
 * @internal
 *   @history 2016-02-29 Tyler Wilson
 *
 *   @TODO:  Need tests for applyToIntersectedGeometry, queryCallback
 *      although the probability is high that the code is being
 *      called in other places in isisminer
 *
 *
 *
 */



void printResourceList(const ResourceList &list);
void printMap(const PvlFlatMap &map);



//Derived class to access protected member functions in Strategy

class derivedStrategy: public Strategy{



  public:




    derivedStrategy():Strategy(){


    }


    derivedStrategy(const PvlObject &definition,const ResourceList &globals)
        :Strategy (definition,globals)
    {



    }

    derivedStrategy(const QString &name, const QString &type):Strategy(name,type) {



    }



  void setNameType(const QString & name, const QString &type){

        setName(name);
        setType(type);
}

  const PvlObject &getDefinitionA(){

      return getDefinition();


  }


  ResourceList getGlobalDefaultsA(){

      return getGlobalDefaults();

  }

  ResourceList getGlobalsA(SharedResource &myGlobals,const ResourceList &globals) {

      return getGlobals(myGlobals,globals);

  }

  PvlFlatMap getDefinitionMapA(){

      return getDefinitionMap();


  }





  void setApplyToDiscardedA(){
      setApplyToDiscarded();
  }

  bool isApplyToDiscardedA() {

      return isApplyToDiscarded();

  }

  void setDoNotApplyToDiscardedA() {

      setDoNotApplyToDiscarded();


  }

  int applyToResourcesA(ResourceList &resources, const ResourceList &globals) {

      return applyToResources(resources,globals);


  }

  unsigned int processedA() {

      return processed();

  }

  void resetProcessedA() {


      return resetProcessed();


  }

  int countActiveA(const ResourceList &resources) {

      return countActive(resources);

  }

  int countDiscardedA(const ResourceList &resources) {

      return countDiscarded(resources);


  }

  ResourceList assetResourceListA(const SharedResource &resource,
                                    const QString &name) {

      return assetResourceList(resource,name);

  }

  QString findReplacementA(const QString &target, const ResourceList &globals,
                          const int &index = 0,
                          const QString &defValue = "") {


      return findReplacement(target,globals,index,defValue);

  }

  QStringList qualifiersA(const QString &keyspec,
                            const QString &delimiter = "::") {

      return qualifiers(keyspec,delimiter);


  }

  QString scanAndReplaceA(const QString &input, const QString &target,
                         const QString &replacement) {

      return scanAndReplace(input,target,replacement);

  }

  QString translateKeywordArgsA(const QString &value,
                               const ResourceList &globals,
                               const QString &defValue = "") {

      return translateKeywordArgs(value,globals,defValue);


  }


  QString processArgsA(const QString &value, const QStringList &argKeys,
                      const ResourceList &globals,
                      const QString &defValue = "") {

       return processArgs(value,argKeys,globals,defValue);


  }

  void propagateKeysA(SharedResource &source, SharedResource &target) {

      return propagateKeys(source,target);


  }

  SharedResource compositeA(SharedResource &resourceA, SharedResource &resourceB,
         const QPair<QString, QString> &keySuffix = qMakePair(QString("A"),QString("B"))) {


      //QPair<QString,QString> keySuffix = qMakePair(QString("A"),QString("B"));

      return composite(resourceA, resourceB,keySuffix);


  }

  bool importGeometryA(SharedResource &resource,
                      const ResourceList &globals) {

      return importGeometry(resource,globals);

  }

  ResourceList activeListA(ResourceList &resource) {

      return activeList(resource);

  }

  void activateListA(ResourceList &resources) {

      activateList(resources);


  }

  void deactivateListA(ResourceList &resources) {

      deactivateList(resources);

  }

  ResourceList copyListA(const ResourceList &resources) {

      return copyList(resources);

  }

  ResourceList cloneListA(const ResourceList &resources,
                         const bool &withAssets = false) {

      return cloneList(resources,withAssets);

  }

  int applyToIntersectedGeometryA(ResourceList &resources, GisGeometry &geom,
                                 const ResourceList &globals) {

      return applyToIntersectedGeometry(resources,geom,globals);


  }

  bool isDebugA() {

      return isDebug();

  }

  bool doShowProgressA() {

      return doShowProgress();

  }

  bool initProgressA(const int &nsteps = 0, const QString &text = "") {

      return initProgress(nsteps,text);

  }

  static void queryCallbackA(void *item, void *userdata) {

      return queryCallback(item, userdata);

  }

  QStringList getObjectListA(const PvlObject &object) {

      return getObjectList(object);

  }




};



void IsisMain() {



  Preference::Preferences(true);


  //Test creating an empty Strategy
  qDebug() << "Testing default constructor:  "  << endl;
  Strategy strat1;
  ResourceList lst;
  ResourceList lstA;
  qDebug() << "Creating empty strategy:";

  qDebug() << "Name:         " << strat1.name();
  qDebug() << "Type:         " << strat1.type();
  qDebug() << "Description:  " << strat1.description();

  PvlFlatMap elven1Map, elven2Map,elven3Map;

  elven1Map.add("demon","balrog"); 
  elven1Map.add("dark","dur");
  elven1Map.add("shield-wall","thangail");

  elven2Map.add("wolf","draug");
  elven2Map.add("Middle-Earth","Endora");
  elven2Map.add("Sun","Anor1");

  elven3Map.add("Sun","Anor");
  elven3Map.add("daisy","eirien");
  elven3Map.add("pipe-weed","galena");

  SharedResource elven1 = SharedResource(new Resource("Elven Word List 1", elven1Map));
  SharedResource elven2 = SharedResource(new Resource("Elven Word List 2", elven2Map));
  SharedResource elven3 = SharedResource(new Resource("Elven Word List 3", elven3Map));


  lst.append(elven1);
  lst.append(elven2);

  lstA.append(elven1);
  lstA.append(elven3);


 qDebug() << "Testing constructors:"  << endl;

 PvlObject elfDictionary("Elven Dictionary");



  qDebug() << "Testing Strategy(const PvlObject &def,const ResourceList &globals) constructor:";

  //This should throw an error because we haven't added keywords to elfDictionary
  try{
        Strategy strat2(elfDictionary,lst);

     }
  catch(IException &e){
          qDebug() <<  e.toString();

          }


  //Add the keywords the constructor is looking for
   elfDictionary += PvlKeyword("Name", "Elven Dictionary");
   elfDictionary += PvlKeyword("Type", "Dictionary");
   elfDictionary += PvlKeyword("ApplyToDiscarded", "false");
   elfDictionary += PvlKeyword("Debug","true");




  //Testing Strategy(PvlObject &o, ResourceList &l) constructor
   Strategy strat3(elfDictionary,lst);
   qDebug() << strat3.name() << endl;

   //Testing Strategy(QString &name, QString &type)
    derivedStrategy strat4("strat4","strat4");
    qDebug() << strat4.name() << endl;



   derivedStrategy dstrat1;

   //Testing setName(const QString &name), setType(const QString &type)
   dstrat1.setNameType("Derived Strategy Name","Derived Strategy Type");

   qDebug() << "Name:         " << dstrat1.name();
   qDebug() << "Type:         " << dstrat1.type();



   derivedStrategy dstrat4(elfDictionary,lst);


  qDebug() << "Testing getGlobalDefaults:  " << endl;
  ResourceList globalDefaults = dstrat4.getGlobalDefaultsA();

    printResourceList(globalDefaults);


  qDebug() << "************************************************";
  qDebug() << endl;

  qDebug() << "Testing getGlobals:  " << endl;
  ResourceList globals = dstrat4.getGlobalsA(elven3,globalDefaults);

    printResourceList(globals);



  qDebug() << "************************************************";
  qDebug() << endl;



  qDebug() << "Testing getDefinition:  " << endl;
  PvlObject o = dstrat4.getDefinitionA();
  qDebug() << o.name() << endl;
  qDebug() << "************************************************";
  qDebug() << endl;

  qDebug() << "Testing getDefinitionMap:  " << endl;
  PvlFlatMap map = dstrat4.getDefinitionMapA();
  printMap(map);




   qDebug() << "************************************************";
   qDebug() << endl;
   qDebug() << "Testing setApplyToDiscarded(), isApplytToDiscarded(), setDoNotApplyToDiscarded()"
        << endl;
   qDebug() << "isApplyToDiscarded = ";

   if( dstrat4.isApplyToDiscardedA() )
       qDebug() << "true" << endl;
   else
       qDebug() << "false" << endl;

   qDebug() << "Testing setApplyToDiscarded:  " << endl;
   dstrat4.setApplyToDiscardedA();
   qDebug() << "isApplyToDiscarded = ";
   if( dstrat4.isApplyToDiscardedA() )
       qDebug() << "true" << endl;
   else
       qDebug() << "false" << endl;


   qDebug() << "Testing setDoNotApplyToDiscarded:  " << endl;
   dstrat4.setDoNotApplyToDiscardedA();
   qDebug() << "isApplyToDiscarded = ";
   if( dstrat4.isApplyToDiscardedA() )
       qDebug() << "true" << endl;
   else
       qDebug() << "false" << endl;

   qDebug() << "************************************************";
   qDebug() << endl;

   qDebug() << "Testing applyToResources(....)" << endl;


  int resourcesProcessed = dstrat4.applyToResourcesA(lstA,globals);

  qDebug() << "Number of resources processed = " << resourcesProcessed << endl;


  qDebug() << "************************************************";
  qDebug() << endl;



   PvlObject elvenPlantsObj("Botany");
   elvenPlantsObj += PvlKeyword("herb","salab");
   elvenPlantsObj += PvlKeyword("snowdrop","niphredil");
   elvenPlantsObj += PvlKeyword("poplar-tree","tulus");
   elvenPlantsObj += PvlKeyword("poplar-tree1","tulus");
   elvenPlantsObj += PvlKeyword("pipe-weed","galenas");



   //Resource elvenPlants("Sindarin Plant Names",elvenPlantsObj);


   SharedResource elven4 = SharedResource(new Resource("Sindarin Plant Names",elvenPlantsObj));

   QVariant elfPlants(elven4);


   qDebug() << "************************************************";
   qDebug() << endl;
   qDebug() << "Testing processed/resetProcessed:  " << endl;

   qDebug() << "Processed = " << dstrat4.processedA() << endl;

   qDebug() << "Resetting Processed:  " << endl;

   dstrat4.resetProcessedA();

   qDebug() << "Processed = " << dstrat4.processedA() << endl;




   qDebug() << "************************************************";
   qDebug() << endl;
   qDebug() << "Testing countActive/countDiscard:  " << endl;

   qDebug() << "Active Resources in lstA:"  << dstrat4.countActiveA(lstA) << endl;

   qDebug() << "Discarded Resources in lstA:"  << dstrat4.countDiscardedA(lstA) << endl;



    qDebug() << "************************************************" << endl;
    qDebug() << "Testing findReplacement:  " << endl;

    printResourceList(globals);
    cout << "Searching for elvish word for demon (with default args):  ";
    cout << dstrat4.findReplacementA("demon",lst)  << endl;

    cout <<"Searching for value not in the ResourceList:  "<< endl;

    QString searchKey = "fluffy bunny";
    QString failMsg = "Could not find "+searchKey;
    cout << dstrat4.findReplacementA(searchKey,lst,0,failMsg)  << endl;


    qDebug() << "************************************************" << endl;
    qDebug() << "Testing qualifiers (more elvish words):  " << endl;


    QString elvishWordsStartingWithA  = "Aaye,Aelin,Adan,Adanedhel,Aduial,Aglarond";
    QString elvishWordsStartingWithA1  = "Aaye::Aelin::Adan::Adanedhel::Aduial::Aglarond";


    QStringList aWords = dstrat4.qualifiersA(elvishWordsStartingWithA,",");

    for (int i = 0; i < aWords.count(); i++ )
        cout << aWords[i] << endl;


    cout << endl;

    qDebug() << "Testing qualifiers with default delimiter (::):  " << endl;


    QStringList aWords1 = dstrat4.qualifiersA(elvishWordsStartingWithA1);

    for (int i = 0; i < aWords1.count(); i++ )
        cout << aWords1[i] << endl;


    qDebug() << "************************************************" << endl;
    qDebug() << "Testing scanAndReplace:  " << endl;

    QString originalSentence("Balrogs require much fnord love and fnord attention.");
    QString fixedSentence =
            dstrat4.scanAndReplaceA(originalSentence,"fnord","");

    cout << "Original sentence:  "  << originalSentence << endl;
    cout << "Fixed sentence:  "  << fixedSentence << endl;



    qDebug() << "************************************************" << endl;
    qDebug() << "Testing translateKeywordArgs:  " << endl;


    QString modifiedKeyword = dstrat4.translateKeywordArgsA("shield-wall",lst,"blah");

    cout << modifiedKeyword << endl;


    qDebug() << "************************************************" << endl;
    qDebug() << "Testing processArgs:  " << endl;


    /**
     * Processes the given string value using the given argument list, resource
     * and default resource.
     *
     * For each argument in the given list, the target string "%i" (where i is the
     * argument number) is replaced with the resource's keyword value
     * corresponding to the argument.  If this value doesn't exist, then the
     * default resource's keyword value is used.  If both fail, then the target
     * string is replaced with "NULL"
     *
     * @param value A keyword value to modified using the given argument list and
     *              resources.
     * @param argKeys A list of string arguments representing the resource values
     *                to be found in the resource's PVL flat map.
     * @param resource A pointer to the resource whose PVL flat map will be
     *                 searched for the arguments.
     * @param defaults A pointer to the default resource whose PVL flat map will
     *                 be searched for the arguments if they are not found in the
     *                 main resource PVL flat map.
     *
     * @return QString The modified string value.
     */

        QStringList argKeys;

        argKeys << "demon" << "dark"  << "shield-wall";

        cout << dstrat4.processArgsA("balrog",argKeys,lst,"default resource") << endl;



        //cout << dstrat4.processArgsA("bunny",argKeys,lst,"default resource") << endl;

        qDebug() << "************************************************" << endl;
        qDebug() << "Testing propagateKeys:  "  << endl;


        printMap(elven3->keys());

        dstrat4.propagateKeysA(elven2,elven3);

        printMap(elven3->keys());




          qDebug() << "************************************************" << endl;
          qDebug() << "Testing activeList" << endl;

           ResourceList l1 = dstrat4.activeListA(globals);
           printResourceList(l1);
           qDebug() <<"Number of active resources = " << dstrat4.countActiveA(globals) << endl;


           qDebug() << "************************************************" << endl;
           qDebug() << "Testing deactivateList" << endl;

           dstrat4.deactivateListA(globals);
           qDebug() <<"Number of active resources = " << dstrat4.countActiveA(globals) << endl;


           qDebug() << "************************************************" << endl;
           qDebug() << "Testing activateList" << endl;
           dstrat4.activateListA(globals);
           qDebug() <<"Number of active resources = " << dstrat4.countActiveA(globals) << endl;


           qDebug() << "************************************************" << endl;
           qDebug() << "Testing copyList (globals -> l5)" << endl;
           ResourceList l5 = dstrat4.copyListA(globals);
           printResourceList(l5);
           qDebug() << "Decativating l5 Resources (global resources are active)" << endl;
           dstrat4.deactivateListA(l5);

           qDebug() <<"Number of active resources in l5 = " << dstrat4.countActiveA(l5) << endl;
           qDebug() <<"Number of active resources in global = "
                   << dstrat4.countActiveA(globals) << endl;




           qDebug() << "************************************************" << endl;
           qDebug() << "Testing cloneList (globals -> l6)" << endl;
           ResourceList l6 = dstrat4.cloneListA(globals);
           printResourceList(l6);


           qDebug() << "************************************************" << endl;
           qDebug() << "Testing applyToIntersectedGeometry" << endl;



           qDebug() << "************************************************" << endl;
           qDebug() << "Testing isDebug():" << endl;
           qDebug() << "isDebug() = " << dstrat4.isDebugA() << endl;



           qDebug() << "************************************************" << endl;
           qDebug() << "Testing doShowProgress():" << endl;
           qDebug() << "doShowProgress() = "<< dstrat4.doShowProgressA() << endl;





           qDebug() << "************************************************" << endl;
           qDebug() << "Testing initProgress():" << endl;
           qDebug() << "Calling with no args, initProgress =  " << dstrat4.initProgressA() << endl;
           qDebug() << "Calling:  initProgress(2,\"some text\") = "
                    << dstrat4.initProgressA(2,"some text")<< endl ;


           qDebug() << "************************************************" << endl;
           qDebug() << "Testing composite(...):" << endl;
           SharedResource shared = dstrat4.compositeA(elven1,elven3,qMakePair(QString("A"),QString("B")));
           qDebug() << shared->name() << endl;

            qDebug() << "************************************************" << endl;
            qDebug() << "Testing importGeometry(...):" << endl;

  //This call will fail
  dstrat4.importGeometryA(elven1,lst);


         PvlFlatMap line1Map,line2Map,line3Map;

line1Map.add("GisGeometry","LINESTRING(1.0 22.5, 90.0 65.0)");
line2Map.add("GisGeometry","LINESTRING(1.0 22.5, 50.0 65.0)");
line3Map.add("GisGeometry","LINESTRING(1.0 22.5, 20.0 65.0)");

SharedResource line1 = SharedResource(new Resource("Line 1", line1Map));
SharedResource line2 = SharedResource(new Resource("Line 2", line2Map));
SharedResource line3 = SharedResource(new Resource("Line 3", line3Map));

ResourceList lines;
lines.append(line1);
lines.append(line2);
lines.append(line3);

PvlObject Geometry("Geom Object");

Geometry += PvlKeyword("Type","Intersect");
Geometry += PvlKeyword("Name","H5");
Geometry += PvlKeyword("GisType","WKT");
Geometry += PvlKeyword("GisGeometry","LINESTRING(0.0 22.5, 90.0 65.0)");
Geometry += PvlKeyword("BoundingBox","True");
derivedStrategy geoms(Geometry,lines);

//This call succeeds
qDebug() << "importGeometry = ";
qDebug() << geoms.importGeometryA(line2,lines)  << endl;


  qDebug() << "************************************************" << endl;
  qDebug() << "getObjectList:  "  << endl;




qDebug() << "************************************************" << endl;
  qDebug() << "getObjectList:  "  << endl;

  PvlObject obj("O");
  PvlObject obj1("O1");
  PvlObject obj2("O2");
  PvlObject obj3("O3");
  obj.addObject(obj1);
  obj.addObject(obj2);
  obj.addObject(obj3);

  QStringList objList = dstrat4.getObjectListA(obj);

  for (int i = 0; i < objList.count(); i++ )
      qDebug() << objList[i] << endl;




  qDebug() << "Testing applyToIntersected Geometry ";

  GisGeometry emptyGeom;

  //This call will fail
  try{
  qDebug() << dstrat4.applyToIntersectedGeometryA(lst,emptyGeom,globals);
  }
  catch(IException &e){

qDebug() << e.toString() << endl;

  }

  GisGeometry g1;
  //g1.setGeometry();

//  qDebug() << geoms.applyToIntersectedGeometryA(lines,);



 }


void printMap(const PvlFlatMap &map){

    PvlFlatMap::ConstPvlFlatMapIterator iter;

    for (iter=map.constBegin(); iter != map.constEnd(); iter++) {

        cout << "\t" << iter.value() << endl;

    }



}


void printResourceList(const ResourceList &list){

    for (int i = 0; i < list.count(); i++)
       cout << list[i] ->name() << endl;


}
