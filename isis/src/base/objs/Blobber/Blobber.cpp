/**                                                                       
 * @file                                                                  
 * $Revision: 1.3 $                                                             
 * $Date: 2008/05/14 21:07:10 $                                                                 
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
#include <vector>
#include "Blobber.h"
#include "Table.h"
#include "Cube.h"
#include "Progress.h"
#include "SpecialPixel.h"
#include "iException.h"
 
namespace Isis {

using std::string;
using std::vector;

/** 
 * @brief Default basic constructor that is mostly not useful
 * 
 * This basic constructor may only be required so that Blobbers can be
 * used in STL constructs (as they require a default constructor)
 */
Blobber::Blobber() : _blobname("_undefined_"), _fieldname("_undefined_"),
                     _name("Blob") { }
/** 
 * @brief Name-only based constructor
 * 
 * This constructor does not require an accompanying cube and allows the
 * user to simply define the Table object and field names to establish these
 * for multiple reads from different cubes.
 * 
 * @param [in] blobname (const std::string&) specifies the name of the ISIS
 *                                          BLOB that contains the field
 *                                          to read the data from
 * @param [in] fieldname (const std::string&) specifies the name of the field
 *                                           in blobname to read and convert
 *                                           to double precision floating point
 *                                           data
 * @param [in] name (const std::string&) Associates a name of the implementors
 *                                      choosing that identifies an 
 *                                      instantiation of this class
 */
Blobber::Blobber(const std::string &blobname, const std::string &fieldname, 
                 const std::string &name) : _blobname(blobname), 
                                      _fieldname(fieldname), _name(name) {
}

/** 
 * @brief Constructor using an ISIS cube class
 * 
 * Reads the contents of the specified field (fieldname) from an ISIS table
 * BLOB (blobname).  Upon instatiation of this class, the BLOB data is read 
 * in and converted to double precision floating point data.  Upon successful
 * return from this construtor, the data is accessble through various methods.
 * 
 * @param [in] cube (Cube&) Reference to an ISIS cube file that has been
 *                         opened or created in the Cube object.  This file
 *                         is expected to contain a Table object that is
 *                         named blobname and must contain a field called
 *                         fieldname.
 * @param [in] blobname (const std::string&) specifies the name of the ISIS
 *                                          BLOB that contains the field
 *                                          to read the data from
 * @param [in] fieldname (const std::string&) specifies the name of the field
 *                                           in blobname to read and convert
 *                                           to double precision floating point
 *                                           data
 * @param [in] name (const std::string&) Associates a name of the implementors
 *                                      choosing that identifies an 
 *                                      instantiation of this class
 */
Blobber::Blobber(Cube &cube, const std::string &blobname,
                 const std::string &fieldname, 
                 const std::string &name) :
                 _blobname(blobname), 
                 _fieldname(fieldname), 
                 _name(name) {
  load(cube);
}

/** 
 * @brief Create a unique copy of this blob 
 * 
 * This method creates a fully new copy of this object.  The default copy
 * constructors/methods create a reference to the data read from the Table
 * object.  For example, the following code fragment will result in two 
 * Blobbers that refer to the same memory location that stores the BLOB data:
 * 
 * @code
 *   Blobber myblob = yourblob;
 * @endcode
 * 
 * To ensure you have two unique storage areas of the BLOB data so they
 * can change independantly, use:
 * 
 * @code
 *   Blobber myblob = yourblob.deepcopy();
 * @endcode
 * 
 * @return (Blobber) Returns a completely new copy, including data, to
 *                       caller.
 */
Blobber Blobber::deepcopy() const {
  Blobber myblob = *this;
  myblob._buf = _buf.copy();
  return (myblob);
}

/** 
 * @brief Loads the contents of a BLOB from a cube file
 * 
 * Provides the I/O interface for ISIS cube files.
 * 
 * @param [in] filename (string&) Name of ISIS cube file to read
 */
void Blobber::load(const std::string &filename) {
  Cube cube;
  cube.Open(filename);
  load(cube);
  return;
}
/** 
 * @brief Loads the contents of a BLOB from a Cube object
 * 
 * Provides the I/O interface for the Cube object.  One thing to note here
 * is that it creates a CubeInfo object from the Cube object and then calls
 * the CubeInfo load method.  Hence, this method is required as an 
 * intermediary method that cascades to the actual method that does the real
 * work.
 * 
 * @param [in] cube (Cube&) Reference to an ISIS cube file that has been
 *                         opened or created in the Cube object.
 */
void Blobber::load(Cube &cube) {
  Table tbl(getBlobName());
  cube.Read(tbl);
  TableField data = tbl[0][getFieldName()];
  if (data.IsDouble()) {
    loadDouble(tbl);
  }
  else if (data.IsInteger()) {
    loadInteger(tbl);
  }
  else {
    string msg = "Field type for " + getFieldName() + 
                 " is not double or integer";
    throw iException::Message(iException::Programmer,msg,_FILEINFO_);
  }
}


/** 
 * @brief Provides direct reading in of the field data from the BLOB
 * 
 * This method is called when the data stored in the BLOB is double precision.
 * It determines the number of rows (lines) and columns (samples) in the BLOB
 * and allocates the internal buffer required to store it
 * 
 * @param [in] tbl (Table&) Reference to an ISIS Table object that contains the
 *                         field from which to extract the data.
 */
void Blobber::loadDouble(Table &tbl) {
  int nlines = tbl.Records();
  int nsamps = tbl[0][getFieldName()].Size();
  BlobBuf pixels(nlines, nsamps);
  for (int i = 0 ; i < nlines ; i++) {
    vector<double> d = tbl[i][getFieldName()];
    for (unsigned int j = 0 ; j < d.size() ; j++) {
      pixels[i][j] = d[j];
    }
  }
  _buf = pixels;
}

/** 
 * @brief Provides direct reading in of the field data from the BLOB
 * 
 * This method is called when the data stored in the BLOB is integer data.
 * It determines the number of rows (lines) and columns (samples) in the BLOB
 * and allocates the internal buffer required to store it.  This differs from
 * the double precision version only in the care taken when casting the data
 * to double precision.  We must properly convert special pixels from integer
 * to double precision.
 * 
 * @param [in] tbl (Table&) Reference to an ISIS Table object that contains the
 *                         field from which to extract the data.
 */
void Blobber::loadInteger(Table &tbl) {
  int nlines = tbl.Records();
  int nsamps = tbl[0][getFieldName()].Size();
  BlobBuf pixels(nlines, nsamps);
  for (int i = 0 ; i < nlines ; i++) {
    vector<int> d = tbl[i][getFieldName()];
    for (unsigned int j = 0 ; j < d.size(); j++) {
      pixels[i][j] = int2ToDouble(d[j]);
    }
  }
  _buf = pixels;
}

/** 
 * @brief Converts integer data to double precision
 * 
 * This method lives to properly handle the conversion of integer BLOB data
 * to double precision.  We must properly convert integer special pixel data
 * that may exist in the BLOB to its appropriate double precision value.
 * 
 * @param [in] value (int) Integer value to convert
 */
double Blobber::int2ToDouble (int value) const {
  if (value == NULL2) return NULL8;
  else if (value == LOW_REPR_SAT2) return LOW_REPR_SAT8;
  else if (value == LOW_INSTR_SAT2) return LOW_INSTR_SAT8;
  else if (value == HIGH_INSTR_SAT2) return HIGH_INSTR_SAT8;
  else if (value == HIGH_REPR_SAT2) return HIGH_REPR_SAT8;
  else return value;

}

}  //  end namespace Isis
