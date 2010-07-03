#if !defined(Blob_h)
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
#include "Pvl.h"

namespace Isis {
  bool IsBlob(Isis::PvlObject &obj);
  /**
   * @internal
   *   @history Jeff Anderson 04-04-2006 Class was not overwriting
   *          existing blobs correctly.
   *   @history Elizabeth Miller 02-20-2007 Fixed bug with
   *          OriginalLabel naming and modified to be backwards
   *          compatible
   *   @history Steven Lambright Added copy constructor,
   *           assignment operator
   *   @history Steven Lambright Added copy constructor,
   *           assignment operator
   *   @history 2010-05-15 Steven Lambright Changed Read to use an
   *            istream instead of an fstream
   *
   * @todo Write class description, history, etc.
   */
  class Blob {
    public:
      Blob(const std::string &name, const std::string &type);
      Blob(const std::string &name, const std::string &type,
           const std::string &file);
      Blob(const Blob &other);
      virtual ~Blob();

      void Read(const std::string &file);
      virtual void Read(Isis::Pvl &pvl, std::istream &is);

      void Write(const std::string &file);
      void Write(Isis::Pvl &pvl, std::fstream &stm,
                 const std::string &detachedFilename = "");

      std::string Type() const {
        return p_type;
      };
      std::string Name() const {
        return p_blobName;
      };

      PvlObject &Label() {
        return p_blobPvl;
      };

      Blob &operator=(const Blob &other);

    protected:
      void Find(Isis::Pvl &pvl);
      virtual void ReadInit() {};
      virtual void ReadData(std::istream &is);

      virtual void WriteInit() {};
      virtual void WriteData(std::fstream &os);

      Isis::PvlObject p_blobPvl;         //!< Pvl Blob object
      std::string p_blobName;            //!< Name of the Blob object

      char *p_buffer;                    //!< Buffer blob data is stored in
      BigInt p_startByte;                   //!< Byte blob data starts at in buffer
      int p_nbytes;                      //!< Size of blob data (in bytes)
      std::string p_type;                //!< Type of data stored in the buffer
      std::string p_detached;            //!< Used for reading detached blobs
      std::string p_labelFile;           //!< The file containing the labels
  };
};

#endif
