/** This is free and unencumbered software released into the public domain.
The authors of ISIS do not claim copyright on the contents of this file.
For more details about the LICENSE terms and the AUTHORS, you will
find files of those names at the top level of this repository. **/

/* SPDX-License-Identifier: CC0-1.0 */
#include <QDebug>
#include <QSharedPointer>
#include <QString>
#include <QStringList>

#include "GisGeometry.h"
#include "IException.h"
#include "Preference.h"
#include "PvlFlatMap.h"
#include "PvlObject.h"
#include "Resource.h"

using namespace Isis;
using namespace std;

void printMap(const PvlFlatMap &);
void printResource(const Resource &);

/**
 * Unit test for Resource class
 *
 * @author 2016-02-29 Ian Humphrey
 *
 * @internal
 *   @history 2016-02-29 Ian Humphrey - Original version. References #2406.
 *
 * NOTE -- not sure how to test keys() with empty resource, since all resource constructors
 *         add an Identity keyword to the resource upon initialization.
 */
int main() {

  Preference::Preferences(true);

  // Testing Constructors
  //   TEST Empty constructor - this sets the name for us automatically
  Resource emptyResource;
  qDebug() << "Creating empty resource:";
  printResource(emptyResource);

  //   TEST Creating a Resource with only a name
  Resource namedResource("Named");
  qDebug() << "Creating " << namedResource.name() << " resource:";
  printResource(namedResource);

  //   TEST Creating a Resource with a PvlFlatMap
  PvlFlatMap animalsMap;
  animalsMap.add("Bee", "Une Abeille");
  animalsMap.add("Cat", "Un Chat");
  animalsMap.add("Dog", "Un Chien");
  animalsMap.add("Frog", "Une Grenouille");
  Resource animals("Animals", animalsMap);
  qDebug() << "Creating " << animals.name() << " resource from PvlFlatMap:";
  printResource(animals);

  //   TEST Creating a Resource with a PvlContainer
  PvlObject colorsObj("Des Couleurs");
  colorsObj += PvlKeyword("Red", "Rouge");
  //   green and blue have multiple values
  PvlKeyword green("Green", "Vert");
  green.addValue("Verte");
  colorsObj += green;
  PvlKeyword blue("Blue", "Bleu");
  blue.addValue("Bleue");
  blue.addValue("NULL");
  colorsObj += blue;
  Resource colors("Colors", colorsObj);
  qDebug() << "Creating " << colors.name() << " resource from PvlObject:";
  printResource(colors);

  //   TEST Creating a Resource from another Resource
  qDebug() << "Copying the Animals resource:";
  Resource animalsCopy(animals);
  printResource(animalsCopy);


  // TEST Resources are equal (i.e. have the same name) 
  qDebug() << "Are the Animals resources equal? " << animals.isEqual(animalsCopy);

  // TEST Resources aren't equal (i.e. don't have the same name)
  qDebug() << "Are the Animals and Colors resources equal? " << animals.isEqual(colors);
  qDebug() << "";


  // TEST count() with a PvlKeyword with 1 value
  qDebug() << "\"Red\" keyword in Colors resource has " << colors.count("red") << " value.";

  // TEST count() with a PvlKeyword with multiple values
  qDebug() << "\"Green\" keyword in Colors resource has " << colors.count("green") << " values.";

  // TEST count() with a PvlKeyword that doesn't exist
  qDebug() << "\"Purple\" keyword does not exist in Colors. It has " 
           << colors.count("purple") << " values.";
  qDebug() << "";


  // TEST isNull() with a non-null PvlKeyword's value
  qDebug() << "\"Blue\" keyword's 2nd value is null? " << colors.isNull("blue", 1);

  // TEST isNull() with a null PvlKeyword's value
  qDebug() << "\"Blue\" keyword's 3rd value is null? " << colors.isNull("blue", 2);

  // TEST isNull() with a non-existent PvlKeyword
  qDebug() << "\"Purple\" keyword is null? " << colors.isNull("purple");
  qDebug() << "";


  // TEST keyword() with existing PvlKeyword (this tests exists() true)
  PvlKeyword greenColor = colors.keyword("green");
  qDebug() << "Grabbing \"Green\" keyword: ";
  cout << "\t" << greenColor << endl;   // can't use qDebug() with PvlKeyword

  // TEST keyword() with non-existing PvlKeyword, returns "" (this tests exists() false)
  PvlKeyword dneColor = colors.keyword("purple");
  qDebug() << "Grabbing non-existent \"Purple\" keyword: ";
  cout << "\t" << dneColor << endl;    // can't use qDebug() with PvlKeyword
  qDebug() << "";


  // TEST Get value of an existing keyword, providing NoColor as default value
  // This value() method calls isNull() as well as the other value() method
  qDebug() << "Grabbing two keywords using \"NoColor\" as default value:";
  qDebug() << "\tGrabbing \"Green\" keyword's 1st value: " << colors.value("Green", "NoColor");

  // TEST Get value of non-existent PvlKeyword, providing NoColor as default value
  qDebug() << "\tGrabbing non-existent \"Purple\" keyword's value: " 
           << colors.value("Purple", "NoColor");
  qDebug() << "";


  // Testing add() methods
  qDebug() << "Adding Rabbit, Octopus, Shark, and Eel to Animals...";

  //    TEST Adding a key,value pair to a resource - add(key, value)
  animals.add("Rabbit", "Un Lapin");
  animals.add(PvlKeyword("Snake", "Un Serpent"));

  //    TEST Adding a PvlKeyword to a resource - add(PvlKeyword)
  PvlKeyword octopus("Octopus", "Une pieuvre");
  octopus.addValue("Un poulpe");
  animals.add(octopus);
  animals.add(octopus);    // Add duplicate PvlKeyword

  //    TEST Adding a PvlFlatMap to a resource - add(PvlFlatMap)
  PvlFlatMap oceanAnimals;
  oceanAnimals.add("Shark", "Un Requin");
  oceanAnimals.add("Eel", "Une Anguille");
  animals.add(oceanAnimals);

  printResource(animals);

  // TEST Appending a value to an existing keyword - append()
  qDebug() << "Adding another value to the \"Bee\" keyword: ";
  animals.append("Bee", "Un Bourdon");
  printResource(animals);

  // TEST Erasing an existing keyword from a resource - erase()
  qDebug() << "Erasing the \"Octopus\" keyword. " 
           << animals.erase("Octopus") << " keywords erased.";
  printResource(animals);


  // TEST the erase() method on non-existent keyword
  qDebug() << "Erasing non-existing \"Squirrel\" keyword. "
           << animals.erase("Squirrel") << " keywords erased.";
  qDebug() << "";


  // Testing the geometry methods
  //   TEST hasValidGeometry() with a resource that has no geometry
  qDebug() << "Does Animals resource have a valid geometry? " << animals.hasValidGeometry();

  //   TEST Adding an empty GisGeometry to a resource, and that geometry should not be valid
  qDebug() << "Adding an empty geometry to Animals resource.";
  animals.add(new GisGeometry);
  qDebug() << "Is the added geometry valid? " << animals.hasValidGeometry(); // redundant

  //   TEST Adding a valid geomeotry to a resource - add(GisGeometry)
  //     This also tests add(GisGeometry *), which calls add(SharedGeometry &)
  qDebug() << "Adding a valid geometry to Animals resource.";
  animals.add(new GisGeometry(100.1, 94.5));
  qDebug() << "Is the added geometry valid? " << animals.hasValidGeometry();

  //   TEST Getting a resource's geometry - geometry()
  qDebug() << "Grabbing the geometry: ";
  qDebug() << "\tHas " << animals.geometry()->points() << " points";
  qDebug() << "";


  // Testing resource activation and discarding
  //    TEST Default active state of a resource
  qDebug() << "Is Colors resource active? " << colors.isActive();
  qDebug() << "";

  //    TEST Discarding an active resource - discard()
  qDebug() << "Discarding Colors resource.";
  colors.discard();
  qDebug() << "Colors is now discarded: " << colors.isDiscarded();
  qDebug() << "";

  //    TEST Activating a discarded resource - activate()
  qDebug() << "Reactivating Colors resource.";
  colors.activate();
  qDebug() << "Colors is now active: " << colors.isActive();
  qDebug() << "";


  //    TEST Adding an asset to a resource - addAsset()
  qDebug() << "Adding \"Int Asset\" and \"Double Asset\" assets to Colors resource:";
  QVariant intAsset(127);
  QVariant dblAsset(3.14);
  colors.addAsset("Int Asset", intAsset);
  colors.addAsset("Double Asset", dblAsset);

  //    TEST Resource does not have specified asset - hasAsset() false
  qDebug() << "Does Colors have an asset called \"DNE Asset\"? " << colors.hasAsset("DNE Asset");
  //    TEST Resource has specified asset - hasAsset() true
  qDebug() << "Does Colors have an asset called \"Int Asset\"? " << colors.hasAsset("Int Asset");

  //    TEST Getting an existing asset - asset()
  qDebug() << "The value of the Int Asset is: " << colors.asset("Int Asset").toInt();

  //    TEST Removing an existing asset - removeAsset()
  qDebug() << "Removing the Double Asset. Removed assets: " << colors.removeAsset("Double Asset");
  qDebug() << "";


  // TEST Copying a resource (shallow) - tests protected copy constructor with deepcopy=false
  qDebug() << "Discarding Colors resource, then copying it:";
  colors.discard();
  Resource *colorsCopy = colors.copy();
  printResource(*colorsCopy);
  // Should be discarded
  qDebug() << "Is Colors copy discarded? " << colorsCopy->isDiscarded();
  // Should have any assets
  qDebug() << "Does Colors copy have the \"Int Asset\"? " << colorsCopy->hasAsset("Int Asset");
  qDebug() << "";
  delete colorsCopy;
  colorsCopy = NULL;


  // Testing cloning a resource (deep) - tests protected copy constructor with deepcopy=true
  //    TEST cloning a resource without copying its assets
  qDebug() << "Cloning Colors resource (without assets):";
  Resource *colorsClone = colors.clone("Cloned Colors");
  printResource(*colorsClone);
  //    Should be active, since cloning activates the resource
  qDebug() << "Is Colors clone active? " << colorsClone->isActive();
  //    Should not have assets
  qDebug() << "Does Colors clone have the \"Int Asset \"? " << colorsClone->hasAsset("Int Asset");
  qDebug() << "";
  delete colorsClone;

  //    TEST cloning a resource and copy its assets 
  qDebug() << "Cloning colors resource with assets:";
  colorsClone = colors.clone("Cloned Animals With Assets", true);
  printResource(*colorsClone);
  //    Should have assets
  qDebug() << "Does Colors clone have the \"Int Asset\"? " << colorsClone->hasAsset("Int Asset");
  qDebug() << "";
  delete colorsClone;
  colorsClone = NULL;


  //    TEST Clearing all assets from a resource - clearAssets()
  qDebug() << "Clearing assets from Colors resource. " << colors.clearAssets() << " assets removed";
  qDebug() << "";


  // TEST toPvl()
  PvlObject animalsPvl = animals.toPvl();
  qDebug() << "Creating Pvl from Animals Resource:";
  cout << animalsPvl << endl;
  qDebug() << "";


  // Testing exceptions
  qDebug() << "";
  qDebug() << "Testing exceptions...";

  //   TEST asset() "Requested asset does not exist."
  try {
    QVariant dneAsset = animals.asset("MysteriousAsset");
  }
  catch (IException &e) {
    qDebug() << e.what();
  }

} // end main()


/**
 * Test function to print a PvlFlatMap (Resource::keys() returns PvlFlatMap)
 * @param map The PvlFlatMap to print
 */
void printMap(const PvlFlatMap &map) {
  PvlFlatMap::ConstPvlFlatMapIterator iter;
  // We don't care about the key, that is just the name of the PvlKeyword
  for (iter = map.constBegin(); iter != map.constEnd(); iter++) {
    cout << "\t" << iter.value() << endl;    // can't use qDebug() with PvlKeyword
  }
}


/**
 * Test function to print a Resource's contents
 * This tests the keys() method
 *
 * @param resource The Resource to print
 */
void printResource(const Resource &resource) {
  printMap(resource.keys());
  cout << endl;
}
