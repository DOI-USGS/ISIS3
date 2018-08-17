A new ISIS3 class needs to have the following Doxygen tags filled out just above the class declaration, as in this example below:

```/**
    * @brief Add map templates to a project
    * Asks the user for a map template and copies it into the project.
    *
    * @author 2018-07-05 Summer Stapleton
    * @internal
    *    @history 2018-07-05 Summer Stapleton - Created ImportMapTemplateWorkOrder
    *                        class
    *
    */
    class ImportMapTemplateWorkOrder : public WorkOrder {....
```
Sometimes, classes are declared inside the header files for other classes.  This happens a lot in the $ISISROOT/src/qisis/objs directory where internal XmlHandler classes are defined to handle object serialization.
These classes need to be documented as well (as in this example from the ImageList header file):

```/**
       * This class is used to read an images.xml file into an image list
       *
       * @author 2012-07-01 Steven Lambright
       *
       * @internal
       */
      class XmlHandler : public XmlStackedHandler {
```
 
All dates need to be filled out the way they are in the examples above (YYYY-MM-DD).  Sometimes you will see dates in this format:  2012-??-???? or  ????-??-??.  Do not do this.  Also, do not put any dashes or
other symbols between the date and the name of the programmer.  If these documentation standards are not followed, PHP warnings get output when the nightly builds are run.