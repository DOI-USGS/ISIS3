#ifndef SerialNumberList_h
#define SerialNumberList_h
/**
 * @file
 * $Revision: 1.6 $
 * $Date: 2009/11/05 18:42:56 $
 *
 *   Unless noted otherwise, the portions of Isis written by the USGS are
 *   public domain. See individual third-party library and package descriptions
 *   for intellectual property information, user agreements, and related
 *   information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or
 *   implied, is made by the USGS as to the accuracy and functioning of such
 *   software and related material nor shall the fact of distribution
 *   constitute any such warranty, and no responsibility is assumed by the
 *   USGS in connection therewith.
 *
 *   For additional information, launch
 *   $ISISROOT/doc/documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <map>
#include <vector>

#include <QString>

namespace Isis {

  class Progress;

  /**
   * @brief Serial Number list generator
   *
   * Create a list of serial numbers from a list of files
   *
   * @ingroup ControlNetworks
   *
   * @author  2005-08-03 Jeff Anderson
   *
   * @internal
   *
   *  @history 2005-08-03 Jeff Anderson - Original Version
   *  @history 2006-02-10 Jacob Danton - Added SerialNumber function
   *  @history 2006-02-13 Stuart Sides - Added checks to make sure all the serial number items have
   *                          the same target.
   *  @history 2006-05-31 Tracie Sucharski - Added filename function that uses index instead of
   *                          serial number.
   *  @history 2006-06-15 Jeff Anderson - Added GetIndex method
   *  @history 2006-06-22 Brendan George - Added functions to get index, modified/added functions
   *                          to get filename and serial number, and modified so that index 
   *                          corresponds to order files are input.
   *  @history 2006-08-16 Brendan George - Added/fixed error checking in FileNameIndex() and
   *                          SerialNumber(string filename).
   *  @history 2006-08-18 Brendan George - Modified to use Expanded FileName on input, allowing for
   *                          filenames that use environment variables
   *  @history 2006-09-13 Steven Koechle - Added method to get the ObservationNumber when you give 
   *                          it an index
   *  @history 2008-01-10 Christopher Austin - Adapted for the new ObservationNumber class.
   *  @history 2008-10-30 Steven Lambright - Fixed problem with definition of struct Pair, pointed 
   *                          out by "novus0x2a" (Support Board Member)
   *  @history 2009-10-20 Jeannie Walldren - Added Progress flag to Constructor
   *  @history 2009-11-05 Jeannie Walldren - Modified number of maximum steps for Progress flag
   *                          in Constructor
   *  @history 2010-09-09 Sharmila Prasad - Added API to delete serial# off of the list given 
   *                          Serial #
   *  @history 2010-11-24 Tracie Sucharski - Added bool def2filename parameter to the Add method. 
   *                          This will allow level 2 images to be added to a serial number list.
   *  @history 2012-07-12 Tracie Sucharski - Added new method Add, which takes a pre-composed
   *                          serial number and a filename.
   *  @history 2016-06-02 Ian Humphrey - Removed SpacecraftInstrumentId method prototypes
   *                          Fixes #3967.
   */

  class SerialNumberList {
    public:
      SerialNumberList(bool checkTarget = true);
      SerialNumberList(const QString &list, bool checkTarget = true, Progress *progress = NULL);
      virtual ~SerialNumberList();

      void add(const QString &filename, bool def2filename = false);
      void add(const QString &serialNumber, const QString &filename);
      void add(const char *serialNumber, const char *filename);
      bool hasSerialNumber(QString sn);
      
      void Delete(const QString &sn);

      int size() const;
      QString fileName(const QString &sn);
      QString fileName(int index);
      QString serialNumber(const QString &filename);
      QString serialNumber(int index);
      QString observationNumber(int index);
      QString spacecraftInstrumentId(int index);
      QString spacecraftInstrumentId(const QString &sn);

      int serialNumberIndex(const QString &sn);
      int fileNameIndex(const QString &filename);

      std::vector<QString> possibleSerialNumbers(const QString &on);

    protected:
      /**
       * A serial number list entity that contains the filename serial number pair. May also 
       * contain an observation number, spacecraft name, and instrument id. 
       */
      struct Pair {
        QString filename;
        QString serialNumber;
        QString observationNumber;
        QString spacecraftName;
        QString instrumentId;
      };

      std::vector<Pair> m_pairs;            //!< List of serial number Pair entities
      std::map<QString, int> m_serialMap;   //!< Maps serial numbers to their positions in the list 
      std::map<QString, int> m_fileMap;     //!< Maps filenames to their positions in the list

      /**
       * Specifies whether or not to check to make sure the target names match between files added
       * to the serialnumber list
       */
      bool m_checkTarget;
      QString m_target; //!< Target name that the files must have if m_checkTarget is true  

  };
};

#endif
