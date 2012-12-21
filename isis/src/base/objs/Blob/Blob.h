#ifndef Blob_h
#define Blob_h
/**
 * @file
 * $Revision: 1.4 $
 * $Date: 2010/05/14 19:16:39 $
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
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html
 *   in a browser or see the Privacy &amp; Disclaimers page on the Isis website,
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */

#include <string>
#include "PvlObject.h"

namespace Isis {
  bool IsBlob(PvlObject &obj);
  class Pvl;
  /**
   * @author ????-??-?? Unknown
   *
   * @internal
   *   @history 2006-04-04 Jeff Anderson Class was not overwriting existing
   *                           blobs correctly.
   *   @history 2007-02-20 Elizabeth Miller Fixed bug with
   *                           OriginalLabel naming and modified to be backwards
   *                           compatible
   *   @history ????-??-?? Steven Lambright Added copy constructor,
   *                           assignment operator
   *   @history 2010-05-15 Steven Lambright Changed Read to use an
   *                           istream instead of an fstream
   *   @history 2011-05-25 Janet Barrett and Steven Lambright Added a Read
   *                           method that takes the pvl labels so they do not
   *                           have to be re-read, which is a very expensive
   *                           operation.
   *   @history 2012-10-04 Jeannie Backer Added include due to forward
   *                           declaration in TableField. Ordered includes and
   *                           added forward declaration. Fixed header
   *                           definition statement. Moved method implementation
   *                           to cpp and reordered methods in cpp. Added
   *                           documentation. Improved test coverage in all
   *                           categories. Added padding to control statements.
   *                           References #1169.
   *
   * @todo Write class description, history, etc.
   */
  class Blob {
    public:
      Blob(const QString &name, const QString &type);
      Blob(const QString &name, const QString &type,
           const QString &file);
      Blob(const Blob &other);
      Blob &operator=(const Blob &other);

      virtual ~Blob();

      QString Type() const;
      QString Name() const;
      int Size() const;
      PvlObject &Label();

      void Read(const QString &file);
      void Read(const QString &file, const Pvl &pvlLabels);
      virtual void Read(const Pvl &pvl, std::istream &is);

      void Write(const QString &file);
      void Write(Pvl &pvl, std::fstream &stm,
                 const QString &detachedFileName = "");

    protected:
      void Find(const Pvl &pvl);
      virtual void ReadInit();
      virtual void ReadData(std::istream &is);
      virtual void WriteInit();
      virtual void WriteData(std::fstream &os);

      PvlObject p_blobPvl;     //!< Pvl Blob object
      QString p_blobName;  //!< Name of the Blob object

      char *p_buffer;          //!< Buffer blob data is stored in
      BigInt p_startByte;      //!< Byte blob data starts at in buffer
      int p_nbytes;            //!< Size of blob data (in bytes)
      QString p_type;      //!< Type of data stored in the buffer
      QString p_detached;  //!< Used for reading detached blobs
      QString p_labelFile; //!< The file containing the labels
  };
};

#endif
