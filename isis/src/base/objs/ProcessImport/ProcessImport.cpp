/**
 * @file
 * $Revision: 1.13 $
 * $Date: 2010/02/22 02:24:59 $
 * 
 *   Unless noted otherwise, the portions of Isis written by the USGS are public
 *   domain. See individual third-party library and package descriptions for 
 *   intellectual property information,user agreements, and related information.
 *
 *   Although Isis has been used by the USGS, no warranty, expressed or implied,
 *   is made by the USGS as to the accuracy and functioning of such software 
 *   and related material nor shall the fact of distribution constitute any such 
 *   warranty, and no responsibility is assumed by the USGS in connection 
 *   therewith.
 *
 *   For additional information, launch 
 *   $ISISROOT/doc//documents/Disclaimers/Disclaimers.html in a browser or see 
 *   the Privacy &amp; Disclaimers page on the Isis website, 
 *   http://isis.astrogeology.usgs.gov, and the USGS privacy and disclaimers on
 *   http://www.usgs.gov/privacy.html.
 */                                                                    

#include <float.h>
#include <iostream>
#include <string>
#include <sstream>

#include "Preference.h"
#include "Brick.h"
#include "ProcessImport.h"
#include "iException.h"
#include "LineManager.h"
#include "Pvl.h"
#include "PvlTokenizer.h"
#include "PixelType.h"
#include "SpecialPixel.h"
#include "iString.h"
#include "PixelType.h"
#include "Application.h"
#include "JP2Decoder.h"

using namespace std;
namespace Isis {

  //! Constructs an Import object.
  ProcessImport::ProcessImport () : Isis::Process() {
  
    p_progress->SetText ("Importing");
           
    //  Initialize input file information
    p_inFile = "";
    p_pixelType = Isis::None;
    p_ns = p_nl = p_nb = 0;
    p_byteOrder = Isis::NoByteOrder;
    p_fileHeaderBytes = 0;
    p_dataHeaderBytes = 0;
    p_dataTrailerBytes = 0;
    p_dataPreBytes = 0;
    p_dataPostBytes = 0;
    p_organization = ProcessImport::BSQ;
    p_base.push_back(0.0);
    p_mult.push_back(1.0);
    // Make all special pixels invalid
    SetNull(DBL_MAX,-DBL_MAX);
    SetHRS(DBL_MAX,-DBL_MAX);
    SetLRS(DBL_MAX,-DBL_MAX);
    SetHIS(DBL_MAX,-DBL_MAX);
    SetLIS(DBL_MAX,-DBL_MAX);

    p_saveFileHeader = false;
    p_saveDataHeader = false;
    p_saveDataTrailer = false;
    p_saveDataPre = false;
    p_saveDataPost = false;
    p_saveFileTrailer = false;

    p_fileHeader = NULL;
    p_fileTrailer = NULL;
  }

  //!  Destroys the Import object
  ProcessImport::~ProcessImport () {
    if (p_fileHeader) {
      delete [] p_fileHeader;
      p_fileHeader = NULL;
    }

    for (unsigned int i=0; i<p_dataHeader.size(); i++) {
      delete [] p_dataHeader[i];
    }

    for (unsigned int i=0; i<p_dataTrailer.size(); i++) {
      delete [] p_dataTrailer[i];
    }

    vector<char *> temp;
    for (unsigned int j=0; j<p_dataPre.size(); j++) {
      temp = p_dataPre[j];
      for (unsigned int i=0; i<temp.size(); i++) delete [] temp[i];
    }

    for (unsigned int j=0; j<p_dataPost.size(); j++) {
      temp = p_dataPost[j];
      for (unsigned int i=0; i<temp.size(); i++) delete [] temp[i];
    }

    if (p_fileTrailer) {
      delete p_fileTrailer;
    }
  }


 /** 
  * Sets the pixel type of the input file.
  * 
  * @param type The enumerated pixel type Isis::Real, Isis::SignedWord, or
  *             Isis::UnsignedByte. This must be invoked prior to setOutputCube
  * 
  * @throws Isis::iException::Message "Unsupported pixel type."
  */
  void ProcessImport::SetPixelType (const Isis::PixelType type) {
  
    if ((type == Isis::Real) || (type == Isis::SignedWord) ||
        (type == Isis::UnsignedWord) || (type == Isis::UnsignedByte)) {  
      p_pixelType = type;
    }
    else {
      string msg = "Unsupported pixel type [" +
                   Isis::PixelTypeName(type) + "]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
    }
  };


 /** 
  * Sets the physical size of the input cube. This must be invoked prior to 
  * setOutputCube.
  * 
  * @param ns Number of samples
  * 
  * @param nl Number of lines
  * 
  * @param nb Number of bands
  * 
  * @throws Isis::iException::Message "Illegal dimension."
  */
  void ProcessImport::SetDimensions (const int ns, const int nl, const int nb) {
    if (ns > 0 && nl >0 && nb>0) {
      p_ns = ns;
      p_nl = nl;
      p_nb = nb;
    }
    else {
      string msg = "Illegal dimension [" + Isis::iString(ns) + ", " + 
                   Isis::iString(nl) + ", " + Isis::iString(nb) + "]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
    }
  };
  
 /** 
  * Sets the byte order of the input file. This must be invoked prior to 
  * SetOutputCube.
  * 
  * @param order The enumerated byte order Isis::Lsb or Isis::Msb
  */
  void ProcessImport::SetByteOrder (const Isis::ByteOrder order) {
    p_byteOrder = order;
  };

 /** 
  * This method sets the number of bytes in the header of a file.
  * The file header is a block of non-image data at the beginning
  * of the entire file.  This does not include any section 
  * headers, such as band headers or line prefixes. Typically it 
  * is used to delineate the boundary of the binary or ascii 
  * labels. It must be called before StartProcess(). This data 
  * will only be available if saved via SaveFileHeader() and may 
  * be accessed via FileHeader() after StartProcess() has 
  * returned and before EndProcess() is called. If not invoked 
  * the default is 0. 
  * 
  * @param bytes Number of bytes
  * 
  * @throws Isis::iException::Message "Illegal file header size." 
  *             (when bytes < 0)
  * @see SaveFileHeader() 
  * @see FileHeader() 
  *  
  */
  void ProcessImport::SetFileHeaderBytes (const int bytes) {
    if (bytes >= 0) {
      p_fileHeaderBytes = bytes;
    }
    else {
      string msg = "Illegal file header size [" + Isis::iString(bytes) + "]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
    }
  };

  /** 
  * This method sets the number of bytes in the trailer of a file.
  * The trailer is a block of non-image data at the end of the 
  * entire file.   This does not include any section trailers, 
  * such as band trailers or line suffixes.  It must be called 
  * before StartProcess(). This data will only be available if 
  * saved via SaveFileTrailer() and may be accessed via 
  * FileTrailer() after StartProcess() has returned and before 
  * EndProcess() is called. If not invoked the default is 0. 
  * 
  * @param bytes Number of bytes
  * 
  * @throws Isis::iException::Message "Illegal file trailer size." 
  *             (when bytes < 0)
  * @see SaveFileTrailer() 
  * @see FileTrailer() 
  * @history 2008-08-12 Jeannie Walldren, Method created  
  *  
  */
  void ProcessImport::SetFileTrailerBytes (const int bytes) {
    if (bytes >= 0) {
      p_fileTrailerBytes = bytes;
    }
    else {
      string msg = "Illegal file trailer size [" + Isis::iString(bytes) + "]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
    }
  };

 /** 
  * This method sets the number of bytes in the header of each 
  * datablock of a file. A data header is a block of non-image 
  * data at the beginning of each data block (for BSQ data blocks 
  * are bands).  This is not included in the file header and does 
  * not include any record headers, such as line prefixes.  It 
  * must be called before StartProcess(). This data will only be 
  * available if saved via SaveDataHeader() and may be accessed 
  * via DataHeader() after StartProcess() has returned and before
  * EndProcess() is called. If not invoked the default is 0. 
  * 
  * @param bytes Number of bytes
  * 
  * @throws Isis::iException::Message "Illegal data header size." 
  *             (when bytes < 0)
  * @see SaveDataHeader() 
  * @see DataHeader() 
  */
  void ProcessImport::SetDataHeaderBytes (const int bytes) {
    if (bytes >= 0) {
      p_dataHeaderBytes = bytes;
    }
    else {
      string msg = "Illegal data header size [" + Isis::iString(bytes) + "]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
    }
  };

 /** 
  * This method sets the number of bytes in the trailer of each 
  * datablock of a file. A data trailer is a block of non-image 
  * data at the end of each data block (for BSQ data blocks 
  * are bands).  This is not included in the file trailer and 
  * does not include any record trailers, such as line suffixes. 
  * It must be called before StartProcess(). This data will only 
  * be available if saved via SaveDataTrailer() and may be 
  * accessed via DataTrailer() after StartProcess() has returned 
  * and before EndProcess() is called. If not invoked the default 
  * is 0. 
  * 
  * @param bytes Number of bytes
  * 
  * @throws Isis::iException::Message "Illegal data trailer 
  *             size." (when bytes < 0) 
  * @see SaveDataTrailer() 
  * @see DataTrailer() 
  */
  void ProcessImport::SetDataTrailerBytes (const int bytes) {
  
    if (bytes >= 0) {
      p_dataTrailerBytes = bytes;
    }
    else {
      string msg = "Illegal data trailer size [" + Isis::iString(bytes) + "]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
    }
  };

 /** 
  * This method sets the number of bytes at the beginning of each 
  * data record of a file. A data prefix is a block of non-image 
  * data at the beginning of each data record (for BSQ data 
  * records are lines).  This is not included in the file header 
  * or the data header. It must be called before StartProcess(). 
  * This data will only be available if saved via 
  * SaveDataPrefix() and may be accessed via DataPrefix() after 
  * StartProcess() has returned and before EndProcess() is 
  * called. If not invoked the default is 0. 
  * 
  * @param bytes Number of bytes
  * 
  * @throws Isis::iException::Message "Illegal data prefix size." 
  *             (when bytes < 0)
  * @see SaveDataPrefix() 
  * @see DataPrefix() 
  */
  void ProcessImport::SetDataPrefixBytes (const int bytes) {
  
    if (bytes >= 0) {
      p_dataPreBytes = bytes;
    }
    else {
      string msg = "Illegal data prefix size [" + Isis::iString(bytes) + "]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
    }
  };

 /** 
  * This method sets the number of bytes at the end of each 
  * data record of a file. A data suffix is a block of non-image 
  * data at the end of each data record (for BSQ data records are 
  * lines).  This is not included in the file trailer or the data
  * trailer. It must be called before StartProcess(). This data 
  * will only be available if saved via SaveDataSuffix() and may be 
  * accessed via DataSuffix() after StartProcess() has returned 
  * and before EndProcess() is called. If not invoked the default 
  * is 0. 
  * 
  * @param bytes Number of bytes
  * 
  * @throws Isis::iException::Message "Illegal data suffix size." 
  *             (when bytes < 0)
  * @see SaveDataSuffix() 
  * @see DataSuffix() 
  */
  void ProcessImport::SetDataSuffixBytes (const int bytes) {
  
    if (bytes >= 0) {
      p_dataPostBytes = bytes;
    }
    else {
      string msg = "Illegal data suffix size [" + Isis::iString(bytes) + "]";
      throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
    }
  };

  /** 
  * This method marks the file header to be saved.  A file 
  * header is a block of non-image data at the beginning of the 
  * entire file.   This does not include any section headers, such 
  * as band headers or line prefixes.  The file header will be 
  * saved inside StartProcess(). It must be called after setting 
  * the size with SetFileHeaderBytes() and before StartProcess() 
  * is called. This data will only be available via FileHeader() 
  * after StartProcess() has returned and before EndProcess() is 
  * called. 
  *  
  * @throws Isis::iException::Message "File header bytes equals
  *              0.  There is nothing to save.  Use
  *              SetFileHeaderBytes() first."
  * @see SetFileHeaderBytes() 
  * @see FileHeader() 
  */
  void ProcessImport::SaveFileHeader () { 
    if (p_fileHeaderBytes == 0){
      iString msg = "File header bytes equals 0.  There is nothing to save.  Use SetFileHeaderBytes() first.";
      throw iException::Message(iException::Programmer,msg.c_str(),_FILEINFO_);
    }
    p_saveFileHeader = true;
  };

 /** 
  * This method marks the file trailer to be saved.  A file 
  * trailer is a block of non-image data at the end of the entire 
  * file.   This does not include any section trailers, such as 
  * band trailers or line suffixes.  The file trailer will be 
  * saved inside StartProcess(). It must be called after setting 
  * the size with SetFileTrailerBytes() and before StartProcess() 
  * is called. This data will only be available via FileTrailer() 
  * after StartProcess() has returned and before EndProcess() is 
  * called. 
  *  
  * @throws Isis::iException::Message "File trailer bytes equals
  *              0.  There is nothing to save.  Use
  *              SetFileTrailerBytes() first." 
  * @see SetFileTrailerBytes() 
  * @see FileTrailer() 
  */
  void ProcessImport::SaveFileTrailer () {
    if (p_fileTrailerBytes == 0){
      iString msg = "File trailer bytes equals 0.  There is nothing to save.  Use SetFileTrailerBytes() first.";
      throw iException::Message(iException::Programmer,msg.c_str(),_FILEINFO_);
    }
    p_saveFileTrailer = true;
  };

 /** 
  * This method marks the data block headers to be saved. A data 
  * header is a block of non-image data at the beginning of each 
  * data block (for BSQ data blocks are bands).  This is not 
  * included in the file header and does not include any record 
  * headers, such as line prefixes. Each of the header blocks 
  * will be saved inside StartProcess().  It must be called after 
  * setting the size with SetDataHeaderBytes() and before 
  * StartProcess() is called. This data will only be available 
  * via DataHeader() after StartProcess() has returned and before 
  * EndProcess() is called. 
  *  
  * @throws Isis::iException::Message "Data header bytes equals
  *              0.  There is nothing to save.  Use
  *              SetDataHeaderBytes() first." 
  * @see SetDataHeaderBytes() 
  * @see DataHeader() 
  */
  void ProcessImport::SaveDataHeader () { 
    if (p_dataHeaderBytes == 0){
      iString msg = "Data header bytes equals 0.  There is nothing to save.  Use SetDataHeaderBytes() first.";
      throw iException::Message(iException::Programmer,msg.c_str(),_FILEINFO_);
    }
    p_saveDataHeader = true;
  };

 /** 
  * This method marks the data block trailers to be saved. A data 
  * trailer is a block of non-image data at the end of each data 
  * block (for BSQ data blocks are bands).  This is not included 
  * in the file trailer and does not include any record trailers, 
  * such as line suffixes. Each of the trailer blocks will be 
  * saved inside StartProcess(). It must be called after setting 
  * the size with SetDataTrailerBytes() and before StartProcess() 
  * is called. This data will only be available via DataTrailer() 
  * after StartProcess() has returned and before EndProcess() is 
  * called. 
  * 
  * @throws Isis::iException::Message "Data trailer bytes equals
  *             0.  There is nothing to save.  Use
  *             SetDataTrailerBytes() first."
  * @see SetDataTrailerBytes() 
  * @see DataTrailer() 
  */
  void ProcessImport::SaveDataTrailer () {
    if (p_dataTrailerBytes == 0){
      iString msg = "Data trailer bytes equals 0.  There is nothing to save.  Use SetDataTrailerBytes() first.";
      throw iException::Message(iException::Programmer,msg.c_str(),_FILEINFO_);
    }
    p_saveDataTrailer = true;
  };

 /** 
  * This method marks the data prefix to be saved.  A data prefix
  * is a block of non-image data at the beginning of each data 
  * record (for BSQ data records are lines).  This is not 
  * included in the file header or the data header.  Each of the 
  * data prefixes will be saved inside StartProcess().  It must 
  * be called after setting the size with SetDataPrefixBytes() 
  * and before StartProcess() is called. This data will only be 
  * available via DataPrefix() after StartProcess() has returned 
  * and before EndProcess() is called. 
  *  
  *  @throws Isis::iException::Message "Data prefix bytes equals
  *              0.  There is nothing to save.  Use
  *              SetDataPrefixBytes() first."
  * @see SetDataPrefixBytes() 
  * @see DataPrefix() 
  */    
  void ProcessImport::SaveDataPrefix () {
    if (p_dataPreBytes == 0){
      iString msg = "Data prefix bytes equals 0.  There is nothing to save.  Use SetDataPrefixBytes() first.";
      throw iException::Message(iException::Programmer,msg.c_str(),_FILEINFO_);
    }
    p_saveDataPre = true;
  };

 /** 
  * This method marks the data suffix to be saved.  A data
  * suffix is a block of non-image data at the end of each data
  * record (for BSQ data records are lines).  This is not 
  * included in the file trailer or the data trailer. Each of 
  * the data suffixes will be saved inside StartProcess().  It 
  * must be called after setting the size with 
  * SetDataPrefixBytes() and before StartProcess() is called. 
  * This data will only be available via DataPrefix() after 
  * StartProcess() has returned and before EndProcess() is 
  * called. 
  * 
  * @throws Isis::iException::Message "Data suffix bytes equals
  *             0.  There is nothing to save.  Use
  *             SetDataSuffixBytes() first."
  * @see SetDataSuffixBytes() 
  * @see DataSuffix() 
  */
  void ProcessImport::SaveDataSuffix () {
    if (p_dataPostBytes == 0){
      iString msg = "Data suffix bytes equals 0.  There is nothing to save.  Use SetDataSuffixBytes() first.";
      throw iException::Message(iException::Programmer,msg.c_str(),_FILEINFO_);
    }
    p_saveDataPost = true;
  };

 /**  
  * This method returns a pointer to the file header.  A file
  * header is a block of non-image data at the beginning of the entire 
  * file.  This does not include any section headers, such 
  * as band headers or line prefixes.  In order to retrieve this 
  * data, the file header bytes must be set and the file header 
  * must be saved via SetFileHeaderBytes() and SaveFileHeader(), 
  * respectively. This method must be called only after 
  * StartProcess() has returned and before EndProcess() is 
  * called. 
  *  
  * @return <B>char*</B> Pointer to file header, if saved.
  * @throws Isis::iException::Message "File header was not saved. 
  *              Use SaveFileHeader()."
  *  
  * @see SetFileHeaderBytes() 
  * @see SaveFileHeader() 
  */
  char* ProcessImport::FileHeader () {
    if (p_saveFileHeader) {
      return p_fileHeader;
    }
    std::string msg = "File header was not saved.  Use SaveFileHeader().";
    throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
  };

 /**
  * This method returns a pointer to the file trailer.  A file
  * trailer is a block of non-image data at the end of the entire 
  * file.  This does not include any section trailers, such as 
  * band trailers or line suffixes.  In order to retrieve this 
  * data, the file trailer bytes must be set and the file trailer 
  * must be saved via SetFileTrailerBytes() and 
  * SaveFileTrailer(), respectively. This method must be called 
  * only after StartProcess() has returned and before 
  * EndProcess() is called. 
  *  
  * @return <B>char*</B> Pointer to file trailer, if saved. 
  * @throws Isis::iException::Message "File trailer was not 
  *              saved. Use SaveFileTrailer()."
  * @see SetFileTrailerBytes() 
  * @see SaveFileTrailer() 
  */
  char* ProcessImport::FileTrailer () {
    if (p_saveFileTrailer) {
      return p_fileTrailer;
    }
    std::string msg = "File trailer was not saved.  Use SaveFileTrailer()";
    throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
  };


 /**
  * This method returns a pointer to the data header.  A data 
  * header is a block of non-image data at the beginning of each 
  * data block (for BSQ data blocks are bands).  This is not 
  * included in the file header and does not include any record 
  * headers, such as line prefixes.  In order to retrieve this 
  * data, the data header bytes must be set and the data header 
  * must be saved via SetDataHeaderBytes() and SaveDataHeader(), 
  * respectively. This method must be called only after 
  * StartProcess() has returned and before EndProcess() is 
  * called. 
  *  
  * @return <B>vector <char*> </B> Vector of pointers to data 
  *         headers for each block, if saved.
  * @throws Isis::iException::Message "Data header was not saved.
  *              Use SaveDataHeader()." 
  * @see SetDataHeaderBytes() 
  * @see SaveDataHeader() 
  */
  std::vector<char *> ProcessImport::DataHeader () { 
    if (p_saveDataHeader) {
      return p_dataHeader;
    }
    std::string msg = "Data header was not saved.  Use SaveDataHeader()";
    throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
  };

 /**
  * This method returns a pointer to the data trailer.  A data 
  * trailer is a block of non-image data at the end each data 
  * block (for BSQ data blocks are bands).  This is not included 
  * in the file trailer and does not include any record trailers, 
  * such as line suffixes.  In order to retrieve this data, the 
  * data trailer bytes must be set and the data trailer must be 
  * saved via SetDataTrailerBytes() and SaveDataTrailer(), 
  * respectively. This method must be called only after 
  * StartProcess() has returned and before EndProcess() is 
  * called. 
  *  
  * @return <B>vector <char*> </B> Vector of pointers to data 
  *         trailers for each block, if saved.
  * @throws Isis::iException::Message "Data trailer was not 
  *              saved. Use SaveDataTrailer()."
  * @see SetDataTrailerBytes() 
  * @see SaveDataTrailer() 
  */
  std::vector<char *> ProcessImport::DataTrailer () {
    if (p_saveDataTrailer) {
      return p_dataTrailer;
    }
    std::string msg = "Data trailer was not saved.  Use SaveDataTrailer()";
    throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
  };

 /**
  *  This method returns a pointer to the data prefix.  A data
  *  prefix is a block of non-image data at the beginning of each
  * data record (for BSQ data records are lines).  This is not 
  *  included in the file header or the data header.  In order to
  *  retrieve this data, the data prefix bytes must be set and
  *  the data prefix must be saved via SetDataPrefixBytes() and
  *  SaveDataPrefix(), respectively. This method must be called
  *  only after StartProcess() has returned and before
  *  EndProcess() is called.
  *  
  * @return <B>vector <vector <char*> > </B> Two-dimensional 
  *         vector of pointers to data prefixes for each record,
  *         if saved.
  * @throws Isis::iException::Message "Data prefix was not saved.
  *              Use SaveDataPrefix()." 
  * @see SetDataPrefixBytes() 
  * @see SaveDataPrefix() 
  */
  std::vector<std::vector<char *> > ProcessImport::DataPrefix () {
    if (p_saveDataPre) {
      return p_dataPre;
    }
    std::string msg = "Data prefix was not saved.  Use SaveDataPrefix()";
    throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
  };

 /**
  * This method returns a pointer to the data suffix.  A data 
  * suffix is a block of non-image data at the end of each data 
  * record (for BSQ data records are lines).  This is not 
  * included in the file trailer or the data trailer. In order to 
  * retrieve this data, the data suffix bytes must be set and the 
  * data suffix must be saved via SetDataSuffixBytes() and 
  * SaveDataSuffix(), respectively. This method must be called 
  * only after StartProcess() has returned and before 
  * EndProcess() is called. 
  *  
  * @return <B>vector <vector <char*> > </B> Two-dimensional 
  *         vector of pointers to data suffixes for each record,
  *         if saved.
  * @throws Isis::iException::Message "Data suffix was not saved.
  *              Use SaveDataSuffix()." 
  * @see SetDataSuffixBytes() 
  * @see SaveDataSuffix() 
  */
  std::vector<std::vector<char *> > ProcessImport::DataSuffix () {
    if (p_saveDataPost) {
      return p_dataPost;
    }
    std::string msg = "Data suffix was not saved.  Use SaveDataSuffix()";
    throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
  };
  
 /** 
  * Sets the organization of the input cube. If not invoked it is assumed
  * Isis::Import::Bsq.
  * 
  * @param org The enumerated organizations either Isis::Import::Bsq  
  *            Isis::Import::Bil or Isis::Import::Bip
  */
  void ProcessImport::SetOrganization (const ProcessImport::Interleave org) { 
    p_organization = org;
  };

 /** 
  * Sets the core base of the input cube. Defaults to zero if not invoked.
  * 
  * @param base Core base
  */
  void ProcessImport::SetBase (const double base) { 
    p_base.clear();
    p_base.push_back(base);
  };

 /** 
  * Sets band dependent core base
  * 
  * @param base A vector containing the base for each band.
  */
  void ProcessImport::SetBase (const std::vector<double> base) {
    p_base = base;
  };
  
 /** 
  * Sets the core multiplier of the input cube. Defaults to one if not invoked.
  * 
  * @param mult Core base
  */
  void ProcessImport::SetMultiplier (const double mult) { 
    p_mult.clear();
    p_mult.push_back(mult);
  };

 /** 
  * Sets band dependent core multiplier
  * 
  * @param mult A vector containing the multiplier for each band.
  */
  void ProcessImport::SetMultiplier (const std::vector<double> mult) {
    p_mult = mult;
  };

 /** 
  * Sets a mapping of input pixel values to output special pixel values.
  * 
  * @param null The double precision value used to identify null pixels in the 
  *             image being imported.
  * 
  * @param lrs The double precisiion value used to identify low representation 
  *            saturation LRS pixels in the image being imported.
  * 
  * @param lis The double precision value used to identify low instrument 
  *            saturation LIS pixels in the image being imported.
  * 
  * @param hrs The double precision value used to identify high representation 
  *            saturation HRS pixels in the image being imported.
  * 
  * @param his The double precision value used to identify high instrument 
  *            saturation HIS pixels in the image being imported.
  */
  void ProcessImport::SetSpecialValues (const double null, const double lrs,
                                        const double lis, const double hrs,
                                        const double his) {
    SetNull(null,null);
    SetLRS(lrs,lrs);
    SetLIS(lis,lis);
    SetHRS(hrs,hrs);
    SetHIS(his,his);
  };

  /** 
  * Sets the range that will be considered Isis::Null
  * 
  * @param null_min The double precision value used to identify
  *             the min range of null pixels in the image being
  *             imported.
  * @param null_max The double precision value used to identify
  *             the max range of null pixels in the image being
  *             imported.
  */
  void ProcessImport::SetNull(const double null_min, const double null_max){
    CheckPixelRange( "Null", null_min, null_max );
    p_null_min = null_min;
    p_null_max = null_max;
  }

  /** 
  * Sets the range that will be considered Isis::Null
  * 
  * @param lrs_min The double precision value used to identify
  *             the min range of lrs pixels in the image being
  *             imported.
  * @param lrs_max The double precision value used to identify
  *             the max range of lrs pixels in the image being
  *             imported.
  */
  void ProcessImport::SetLRS(const double lrs_min, const double lrs_max){
    CheckPixelRange( "LRS", lrs_min, lrs_max );
    p_lrs_min = lrs_min;
    p_lrs_max = lrs_max;
  }

  /** 
  * Sets the range that will be considered Isis::Null
  * 
  * @param lis_min The double precision value used to identify
  *             the min range of lis pixels in the image being
  *             imported.
  * @param lis_max The double precision value used to identify
  *             the max range of lis pixels in the image being
  *             imported.
  */
  void ProcessImport::SetLIS(const double lis_min, const double lis_max){
    CheckPixelRange( "LIS", lis_min, lis_max );
    p_lis_min = lis_min;
    p_lis_max = lis_max;
  }

  /** 
  * Sets the range that will be considered Isis::Null
  * 
  * @param hrs_min The double precision value used to identify
  *             the min range of hrs pixels in the image being
  *             imported.
  * @param hrs_max The double precision value used to identify
  *             the max range of hrs pixels in the image being
  *             imported.
  */
  void ProcessImport::SetHRS(const double hrs_min, const double hrs_max){
    CheckPixelRange( "HRS", hrs_min, hrs_max );
    p_hrs_min = hrs_min;
    p_hrs_max = hrs_max;
  }

  /** 
  * Sets the range that will be considered Isis::Null
  * 
  * @param his_min The double precision value used to identify
  *             the min range of his pixels in the image being
  *             imported.
  * @param his_max The double precision value used to identify
  *             the max range of his pixels in the image being
  *             imported.
  */
  void ProcessImport::SetHIS(const double his_min, const double his_max){
    CheckPixelRange( "HIS", his_min, his_max );
    p_his_min = his_min;
    p_his_max = his_max;
  }


  /**
   * Checks the special pixel range of the given against all other special pixel 
   * value ranges, making sure none overlap. 
   * 
   * @param pixelName Name of the special pixel type to be displayed in the error 
   *                  message.
   * @param pixelMin The minimum value of the special pixel range
   * @param pixelMax The maximum value of the special pixel range
   */
  void ProcessImport::CheckPixelRange( string pixelName, double pixelMin, double pixelMax ) {
    if( pixelMin == DBL_MAX || pixelMax == -DBL_MAX ) return;

    if(  p_null_min != DBL_MAX && p_null_max != -DBL_MAX && ( // Checks if null has been set
        (pixelMin > p_null_min && pixelMin < p_null_max) ||      // Checks for min crossing
        (pixelMax > p_null_min && pixelMax < p_null_max) ||      // Checks for max crossing
        (pixelMin < p_null_min && pixelMax > p_null_max) )) {    // Checks for straddling values
      string msg = "The " + pixelName + " range [" + iString(pixelMin) +
        "," + iString(pixelMax) + "] overlaps the NULL range [" +
        iString(p_null_min) + "," + iString(p_null_max) + "]";
      throw Isis::iException::Message(Isis::iException::User,msg, _FILEINFO_);
    }

    if(  p_lrs_min != DBL_MAX && p_lrs_max != -DBL_MAX && (
        (pixelMin > p_lrs_min && pixelMin < p_lrs_max) ||
        (pixelMax > p_lrs_min && pixelMax < p_lrs_max) ||
        (pixelMin < p_lrs_min && pixelMax > p_lrs_max) )) {
      string msg = "The " + pixelName + " range [" + iString(pixelMin) +
        "," + iString(pixelMax) + "] overlaps the LRS range [" +
        iString(p_lrs_min) + "," + iString(p_lrs_max) + "]";
      throw Isis::iException::Message(Isis::iException::User,msg, _FILEINFO_);
    }

    if(  p_lis_min != DBL_MAX && p_lis_max != -DBL_MAX && (
        (pixelMin > p_lis_min && pixelMin < p_lis_max) ||
        (pixelMax > p_lis_min && pixelMax < p_lis_max) ||
        (pixelMin < p_lis_min && pixelMax > p_lis_max) )) {
      string msg = "The " + pixelName + " range [" + iString(pixelMin) +
        "," + iString(pixelMax) + "] overlaps the LIS range [" +
        iString(p_lis_min) + "," + iString(p_lis_max) + "]";
      throw Isis::iException::Message(Isis::iException::User,msg, _FILEINFO_);
    }

    if(  p_hrs_min != DBL_MAX && p_hrs_max != -DBL_MAX && (
        (pixelMin > p_hrs_min && pixelMin < p_hrs_max) ||
        (pixelMax > p_hrs_min && pixelMax < p_hrs_max) ||
        (pixelMin < p_hrs_min && pixelMax > p_hrs_max) )) {
      string msg = "The " + pixelName + " range [" + iString(pixelMin) +
        "," + iString(pixelMax) + "] overlaps the HRS range [" +
        iString(p_hrs_min) + "," + iString(p_hrs_max) + "]";
      throw Isis::iException::Message(Isis::iException::User,msg, _FILEINFO_);
    }

    if(  p_his_min != DBL_MAX && p_his_max != -DBL_MAX && (
        (pixelMin > p_his_min && pixelMin < p_his_max) ||
        (pixelMax > p_his_min && pixelMax < p_his_max) ||
        (pixelMin < p_his_min && pixelMax > p_his_max) )) {
      string msg = "The " + pixelName + " range [" + iString(pixelMin) +
        "," + iString(pixelMax) + "] overlaps the HIS range [" +
        iString(p_his_min) + "," + iString(p_his_max) + "]";
      throw Isis::iException::Message(Isis::iException::User,msg, _FILEINFO_);
    }

  }

  /** 
  * Tests the pixel. If it is valid it will return the dn value,
  * otherwise it will return the Isis special pixel value that
  * corresponds to it
  * 
  * @param pixel The double precision value that represents a
  *              pixel.
  * @return double  The double precision value representing the
  *         pixel will return as a valid dn or changed to an isis
  *         special pixel.
  */
  double ProcessImport::TestPixel(const double pixel){
    if (pixel <= p_null_max && pixel >= p_null_min){
      return Isis::NULL8;
    } else if (pixel <= p_hrs_max && pixel >= p_hrs_min){
      return Isis::HIGH_REPR_SAT8;
    } else if (pixel <= p_lrs_max && pixel >= p_lrs_min){
      return Isis::LOW_REPR_SAT8;
    } else if (pixel <= p_his_max && pixel >= p_his_min){
      return Isis::HIGH_INSTR_SAT8;
    } else if (pixel <= p_lis_max && pixel >= p_lis_min){
      return Isis::LOW_INSTR_SAT8;
    } else {
      return pixel;
    }
  }

 /** 
  * Create the output file. Note that all the appropiate calls to at least
  * SetDimensions and SetPixelType should be made prior to calling this method.
  * 
  * @param parameter The parameter name that holds the output file name.
  * 
  * @throws Isis::iException::Message "Unsupported pixel type."
  */
  Isis::Cube* ProcessImport::SetOutputCube (const std::string &parameter) {
    Isis::CubeAttributeOutput &att = Application::GetUserInterface().GetOutputAttribute(parameter);
  
    if (att.PropagateMinimumMaximum()) {
      double min,max;
      if ((p_pixelType == Isis::Real) || 
          (p_base.size() > 1) || (p_mult.size() > 1)) {
        min = Isis::VALID_MIN4;
        max = Isis::VALID_MAX4;
      }
      else if (p_pixelType == Isis::SignedWord) {
        min = Isis::VALID_MIN2 * p_mult[0] + p_base[0];
        max = Isis::VALID_MAX2 * p_mult[0] + p_base[0];
      }
      else if (p_pixelType == Isis::UnsignedWord) {
        min = Isis::VALID_MINU2 * p_mult[0] + p_base[0];
        max = Isis::VALID_MAXU2 * p_mult[0] + p_base[0];
      }
      else if (p_pixelType == Isis::UnsignedByte){
        min = Isis::VALID_MIN1 * p_mult[0] + p_base[0];
        max = Isis::VALID_MAX1 * p_mult[0] + p_base[0];
      }
      else {
        string msg = "Unsupported pixel type [" +
                     Isis::PixelTypeName(p_pixelType) + "]";
        throw Isis::iException::Message(Isis::iException::Programmer,msg, _FILEINFO_);
      }
      att.Minimum(min);
      att.Maximum(max);
    }
  
    if (att.PropagatePixelType()) {
      if ((p_base.size() > 1) || (p_mult.size() > 1)) { 
        att.PixelType(Isis::Real);
      }
      else if (p_pixelType == Isis::UnsignedWord) {
        att.PixelType(Isis::Real);
      }
      else {
        att.PixelType(p_pixelType);
      }
    }
  
    return Isis::Process::SetOutputCube (Application::GetUserInterface().GetFilename(parameter),
                                       att, p_ns, p_nl, p_nb);
  }

 /** 
  * Create the output file. Note that all the appropiate calls to at least 
  * SetDimensions should be invoked prior to calling this method.
  * 
  * @param parameter The parameter name that holds the output file name.
  * 
  * @param att An output cube attribute to define the characteristics of the
  *            output cube.
  * 
  * @throws Isis::iException::Message "File is not in a supported
  *             organization."
  */
  Isis::Cube* ProcessImport::SetOutputCube (const std::string &parameter,
                                                Isis::CubeAttributeOutput &att) {
    return Isis::Process::SetOutputCube (parameter, att, p_ns, p_nl, p_nb);
  }


  //! Process the input file and write it to the output.
  void ProcessImport::StartProcess() {
    if (p_organization == ProcessImport::JP2) {
      ProcessJp2 ();
    }
    else if (p_organization == ProcessImport::BSQ) {
      ProcessBsq ();
    }
    else if (p_organization == ProcessImport::BIL) {
      ProcessBil ();
    }
    else if (p_organization == ProcessImport::BIP) {
      ProcessBip ();
    }
    else {
      string msg = "File [" + p_inFile + "] is not in a supported organization.";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
  }


  /**
   * Process the input file and send data to method
   * 
   * @param funct Method that accepts Isis::Buffer as an input
  *              parameter, processes the image, and has no
  *              return value.
   * @throws Isis::iException::Message "File is not a supported
   *             organization."
   */
  void ProcessImport::StartProcess(void funct(Isis::Buffer &out)) {
    if (p_organization == ProcessImport::JP2) {
      ProcessJp2 (funct);
    }
    else if (p_organization == ProcessImport::BSQ) {
      ProcessBsq (funct);
    }
    else if (p_organization == ProcessImport::BIL) {
      ProcessBil (funct);
    }
    else if (p_organization == ProcessImport::BIP) {
      ProcessBip (funct);
    }
    else {
      string msg = "File [" + p_inFile + "] is not in a supported organization.";
      throw Isis::iException::Message(Isis::iException::Programmer,msg,_FILEINFO_);
    }
  }
  
 /**
  * Process the import data as a band sequential file. 
  *  
  * @param funct Method that accepts Isis::Buffer as an input
  *              parameter, processes the image, and has no
  *              return value.
  * 
  * @throws Isis::iException::Message "Cannot open input file."
  * @throws Isis::iException::Message "Cannot read file. 
  *             Position[]. Byte count[]"
  */
  void ProcessImport::ProcessBsq (void funct(Isis::Buffer &out)) {
    // Figure out the number of bytes to read for a single line
    int readBytes = Isis::SizeOf (p_pixelType);
    readBytes = readBytes * p_ns;
    char *in = new char [readBytes];
  
    // Set up an Isis::EndianSwapper object
    Isis::iString tok(Isis::ByteOrderName(p_byteOrder));
    tok.UpCase();
    Isis::EndianSwapper swapper (tok);
  
    ifstream fin;
    // Open input file
    Isis::Filename inFile(p_inFile);
    string inFilename(inFile.Expanded());
    fin.open (inFilename.c_str(), ios::in|ios::binary);
    if (!fin.is_open()) {
      string msg = "Cannot open input file [" + p_inFile + "]";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }
  
    // Handle the file header
    streampos pos = fin.tellg();
    if (p_saveFileHeader) {
      p_fileHeader = new char[p_fileHeaderBytes];
      fin.read (p_fileHeader, p_fileHeaderBytes);
    }
    else {
      fin.seekg (p_fileHeaderBytes, ios_base::beg);
    }
  
    // Check the last io
    if (!fin.good ()) {
     string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                  Isis::iString((int)pos) + "]. Byte count [" +
                  Isis::iString(p_fileHeaderBytes) + "]" ;
     throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
    }
  
    // Construct a line buffer manager
    Isis::Buffer *out = NULL;

    if(funct != NULL) {
      out = new Isis::Brick (p_ns, 1, 1, p_pixelType);
    }
    else {
      out = new Isis::LineManager (*OutputCubes[0]);
    }
  
    // Loop once for each band in the image
    p_progress->SetMaximumSteps(p_nl*p_nb);
    p_progress->CheckStatus();
  
    // Loop for each band
    for (int band=0; band<p_nb; band++) {
      // Set the base multiplier
      double base,mult;
      if (p_base.size() > 1) {
        base = p_base[band];
        mult = p_mult[band];
      }
      else {
        base = p_base[0];
        mult = p_mult[0];
      }
  
      // Handle any data headers (e.g., the data at the beginning of each band)
      pos = fin.tellg();
      if (p_saveDataHeader) {
        p_dataHeader.push_back(new char[p_dataHeaderBytes]);
        fin.read (p_dataHeader.back(), p_dataHeaderBytes);
      }
      else {
        fin.seekg (p_dataHeaderBytes, ios_base::cur);
      }
  
      // Check the last io
      if (!fin.good ()) {
       string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                    Isis::iString((int)pos) + "]. Byte count [" +
                    Isis::iString(p_dataHeaderBytes) + "]" ;
       throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
      }
  
      // Space for storing prefix and suffix data pointers
      vector<char *> tempPre, tempPost;
  
      // Loop for each line in a band
      for (int line=0; line<p_nl; line++) {
  
        // Handle any line prefix bytes
        pos = fin.tellg();
        if (p_saveDataPre) {
          tempPre.push_back(new char[p_dataPreBytes]);
          fin.read (tempPre.back(), p_dataPreBytes);
        }
        else {
          fin.seekg (p_dataPreBytes, ios_base::cur);
        }
  
        // Check the last io
        if (!fin.good ()) {
         string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                      Isis::iString((int)pos) + "]. Byte count [" +
                      Isis::iString(p_dataPreBytes) + "]" ;
         throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
        }
  
  
        // Get a line of data from the input file
        pos = fin.tellg();
        fin.read (in, readBytes);
        if (!fin.good ()) {
         string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                      Isis::iString((int)pos) + "]. Byte count [" + 
                      Isis::iString(readBytes) + "]" ;
         throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
        }
  
        // Swap the bytes if necessary and convert any out of bounds pixels
        // to special pixels
        for (int samp=0; samp<p_ns; samp++) {
          switch (p_pixelType) {
            case Isis::UnsignedByte:
              (*out)[samp] = (double) ((unsigned char *)in)[samp];
              break;
            case Isis::UnsignedWord:
              (*out)[samp] = (double)swapper.UnsignedShortInt ((unsigned short int *)in+samp);
              break;
            case Isis::SignedWord:
              (*out)[samp] = (double)swapper.ShortInt ((short int *)in+samp);
              break;
            case Isis::Real:
              (*out)[samp] = (double)swapper.Float ((float *)in+samp);
              break;
            default:
              break;
          }
 
          // Sets out to isis special pixel or leaves it if valid
          (*out)[samp] = TestPixel((*out)[samp]);
  
          if (Isis::IsValidPixel((*out)[samp])) {
            (*out)[samp] = mult * ((*out)[samp]) + base;
          }
        } // End sample loop

        if(funct == NULL) {
          // Set the buffer position and write the line to the output file
          ((Isis::LineManager*)out)->SetLine ((band * p_nl) + line + 1);
          OutputCubes[0]->Write (*out);
        }
        else {
          ((Isis::Brick*)out)->SetBaseSample (1);
          ((Isis::Brick*)out)->SetBaseLine (line + 1);
          ((Isis::Brick*)out)->SetBaseBand (band + 1);
          funct(*out);
        }

        p_progress->CheckStatus();
  
        // Handle any line suffix bytes
        pos = fin.tellg();
        if (p_saveDataPost) {
          tempPost.push_back(new char[p_dataPostBytes]);
          fin.read (tempPost.back(), p_dataPostBytes);
        }
        else {
          fin.seekg (p_dataPostBytes, ios_base::cur);
        }
  
        // Check the last io
        if (!fin.good ()) {
         string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                      Isis::iString((int)pos) + "]. Byte count [" +
                      Isis::iString(p_dataPreBytes) + "]" ;
         throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
        }
      } // End line loop
  
      // Save off the prefix bytes vector
      if (p_saveDataPre) {
        p_dataPre.push_back(tempPre);
        tempPre.clear();
      }
  
      // Save off the suffix bytes vector
      if (p_saveDataPost) {
        p_dataPost.push_back(tempPost);
        tempPost.clear();
      }
  
      // Handle the band trailer
      pos = fin.tellg();
      if (p_saveDataTrailer) {
        p_dataTrailer.push_back(new char[p_dataTrailerBytes]);
        fin.read (p_dataTrailer.back(), p_dataTrailerBytes);
      }
      else {
        fin.seekg (p_dataTrailerBytes, ios_base::cur);
      }
  
      // Check the last io
      if (!fin.good ()) {
       string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                    Isis::iString((int)pos) + "]. Byte count [" +
                    Isis::iString(p_fileHeaderBytes) + "]" ;
       throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
      }
  
    } // End band loop
  
    // Handle the file trailer
    pos = fin.tellg();
    if (p_saveFileTrailer) {
      fin.seekg (0, ios_base::end);
      streampos e = fin.tellg();
      p_fileTrailerBytes = (int)(e - pos + (streampos)1);
      p_fileTrailer = new char[p_fileTrailerBytes];
      fin.seekg (pos);
      fin.read (p_fileTrailer, p_fileTrailerBytes);
  
      // Check the io
      if (!fin.good ()) {
       string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                    Isis::iString((int)pos) + "]. Byte count [" +
                    Isis::iString(p_fileTrailerBytes) + "]" ;
       throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
      }
  
    }
  
    // Close the file and clean up
    fin.close ();
    delete [] in;
  }


 /** 
  * Function to process files stored as Band Interleaved by Line
  *  
  * @param funct Method that accepts Isis::Buffer as an input
  *              parameter, processes the image, and has no
  *              return value.
  * @throws Isis::iException::Message "Cannot open input file."
  * @throws Isis::iException::Message "Cannot read file. 
  *             Position[]. Byte count[]"
  */
  void ProcessImport::ProcessBil (void funct(Isis::Buffer &out)) {
  
    // Figure out the number of bytes to read for a single line
    int readBytes = Isis::SizeOf (p_pixelType);
    readBytes = readBytes * p_ns;
    char *in = new char [readBytes];
  
    // Set up an Isis::EndianSwapper object
    Isis::iString tok(Isis::ByteOrderName(p_byteOrder));
    tok.UpCase();
    Isis::EndianSwapper swapper (tok);
  
    ifstream fin;
    // Open input file
    Isis::Filename inFile(p_inFile);
    string inFilename(inFile.Expanded());
    fin.open (inFilename.c_str(), ios::in|ios::binary);
    if (!fin.is_open()) {
      string msg = "Cannot open input file [" + p_inFile + "]";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }
  
    // Handle the file header
    streampos pos = fin.tellg();
    if (p_saveFileHeader) {
      p_fileHeader = new char[p_fileHeaderBytes];
      fin.read (p_fileHeader, p_fileHeaderBytes);
    }
    else {
      fin.seekg (p_fileHeaderBytes, ios_base::beg);
    }
  
    // Check the last io
    if (!fin.good ()) {
     string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                  Isis::iString((int)pos) + "]. Byte count [" +
                  Isis::iString(p_fileHeaderBytes) + "]" ;
     throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
    }
  
    // Construct a line buffer manager
    Isis::Buffer *out = NULL;

    if(funct != NULL) {
      out = new Isis::Buffer (p_ns, p_nl, p_nb, p_pixelType);
    }
    else {
      out = new Isis::LineManager (*OutputCubes[0]);
    }

    // Loop once for each line in the image
    p_progress->SetMaximumSteps(p_nb*p_nl);
    p_progress->CheckStatus();
    
    // Loop for each line
    for (int line=0; line<p_nl; line++) {
      // Loop for each band 
      for (int band=0; band<p_nb; band++) {
        // Set the base multiplier
        double base,mult;
        if (p_base.size() > 1) {
          base = p_base[band];
          mult = p_mult[band];
        }
        else {
          base = p_base[0];
          mult = p_mult[0];
        }

        // Check the last io
        if (!fin.good ()) {
         string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                      Isis::iString((int)pos) + "]. Byte count [" +
                      Isis::iString(p_dataHeaderBytes) + "]" ;
         throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
        }

        // Space for storing prefix and suffix data pointers
        vector<char *> tempPre, tempPost;


        // Handle any line prefix bytes
        pos = fin.tellg();
        if (p_saveDataPre) {
          tempPre.push_back(new char[p_dataPreBytes]);
          fin.read (tempPre.back(), p_dataPreBytes);
        }
        else {
          fin.seekg (p_dataPreBytes, ios_base::cur);
        }

        // Check the last io
        if (!fin.good ()) {
         string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                      Isis::iString((int)pos) + "]. Byte count [" +
                      Isis::iString(p_dataPreBytes) + "]" ;
         throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
        }


        // Get a line of data from the input file
        pos = fin.tellg();
        fin.read (in, readBytes);
        if (!fin.good ()) {
        string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                      Isis::iString((int)pos) + "]. Byte count [" + 
                      Isis::iString(readBytes) + "]" ;
         throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
        }

        // Swap the bytes if necessary and convert any out of bounds pixels
        // to special pixels
        for (int samp=0; samp<p_ns; samp++) {
          switch (p_pixelType) {
            case Isis::UnsignedByte:
              (*out)[samp] = (double) ((unsigned char *)in)[samp];
              break;
            case Isis::UnsignedWord:
              (*out)[samp] = (double)swapper.UnsignedShortInt ((unsigned short int *)in+samp);
              break;
            case Isis::SignedWord:
              (*out)[samp] = (double)swapper.ShortInt ((short int *)in+samp);
              break;
            case Isis::Real:
              (*out)[samp] = (double)swapper.Float ((float *)in+samp);
              break;
            default:
              break;
          }

          // Sets out to isis special pixel or leaves it if valid
          (*out)[samp] = TestPixel((*out)[samp]);

          if (Isis::IsValidPixel((*out)[samp])) {
            (*out)[samp] = mult * ((*out)[samp]) + base;
          }
        } // End sample loop

        if(funct == NULL) {
          //Set the buffer position and write the line to the output file
          ((Isis::LineManager*)out)->SetLine ((band * p_nl) + line + 1);
          OutputCubes[0]->Write (*out);
        }
        else {
          funct(*out);
        }

        p_progress->CheckStatus();

        // Handle any line suffix bytes
        pos = fin.tellg();
        if (p_saveDataPost) {
          tempPost.push_back(new char[p_dataPostBytes]);
          fin.read (tempPost.back(), p_dataPostBytes);
        }
        else {
          fin.seekg (p_dataPostBytes, ios_base::cur);
        }

        // Check the last io
        if (!fin.good ()) {
         string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                      Isis::iString((int)pos) + "]. Byte count [" +
                      Isis::iString(p_dataPreBytes) + "]" ;
         throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
        }


        // Save off the prefix bytes vector
        if (p_saveDataPre) {
          p_dataPre.push_back(tempPre);
          tempPre.clear();
        }
  
        // Save off the suffix bytes vector
        if (p_saveDataPost) {
          p_dataPost.push_back(tempPost);
          tempPost.clear();
        }
   
        // Check the last io
        if (!fin.good ()) {
         string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                    Isis::iString((int)pos) + "]. Byte count [" +
                      Isis::iString(p_fileHeaderBytes) + "]" ;
         throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
        }
  
      } // End band loop

    } // End line loop

    // Handle the file trailer
    pos = fin.tellg();
    if (p_saveFileTrailer) {
      fin.seekg (0, ios_base::end);
      streampos e = fin.tellg();
      p_fileTrailerBytes = (int)(e - pos + (streampos)1);
      p_fileTrailer = new char[p_fileTrailerBytes];
      fin.seekg (pos);
      fin.read (p_fileTrailer, p_fileTrailerBytes);

      // Check the io
      if (!fin.good ()) {
       string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                    Isis::iString((int)pos) + "]. Byte count [" +
                    Isis::iString(p_fileTrailerBytes) + "]" ;
       throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
      }

    }

    // Close the file and clean up
    fin.close ();
    delete [] in;

  }


 /**
  * Function to process files stored as Band Interleaved by Pixel 
  *  
  * @param funct Method that accepts Isis::Buffer as an input
  *              parameter, processes the image, and has no
  *              return value.
  * @throws Isis::iException::Message "Cannot open input file."
  * @throws Isis::iException::Message "Cannot read file. 
  *             Position[]. Byte count[]" 
  */
  void ProcessImport::ProcessBip (void funct(Isis::Buffer &out)) {
 
    // Figure out the number of bytes to read for a single line
    int readBytes = Isis::SizeOf (p_pixelType);
    readBytes = readBytes * p_ns * p_nb;
    char *in = new char [readBytes];
  
    // Set up an Isis::EndianSwapper object
    Isis::iString tok(Isis::ByteOrderName(p_byteOrder));
    tok.UpCase();
    Isis::EndianSwapper swapper (tok);
  
    ifstream fin;
    // Open input file
    Isis::Filename inFile(p_inFile);
    string inFilename(inFile.Expanded());
    fin.open (inFilename.c_str(), ios::in|ios::binary);
    if (!fin.is_open()) {
      string msg = "Cannot open input file [" + p_inFile + "]";
      throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
    }
  
    // Handle the file header
    streampos pos = fin.tellg();
    if (p_saveFileHeader) {
      p_fileHeader = new char[p_fileHeaderBytes];
      fin.read (p_fileHeader, p_fileHeaderBytes);
    }
    else {
      fin.seekg (p_fileHeaderBytes, ios_base::beg);
    }
  
    // Check the last io
    if (!fin.good ()) {
     string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                  Isis::iString((int)pos) + "]. Byte count [" +
                  Isis::iString(p_fileHeaderBytes) + "]" ;
     throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
    }
  
    // Construct a line buffer manager
    Isis::Buffer *out = NULL;

    if(funct != NULL) {
      out = new Isis::Buffer (p_ns, p_nl, p_nb, p_pixelType);
    }
    else {
      out = new Isis::LineManager (*OutputCubes[0]);
    }

    // Loop once for each line in the image
    p_progress->SetMaximumSteps(p_nl);
    p_progress->CheckStatus();
    
    // Loop for each line
    for (int line=0; line<p_nl; line++) {
      // Check the last io
      if (!fin.good ()) {
       string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                    Isis::iString((int)pos) + "]. Byte count [" +
		    Isis::iString(p_dataHeaderBytes) + "]" ;
       throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
      }

      // Space for storing prefix and suffix data pointers
      vector<char *> tempPre, tempPost;


      // Handle any line prefix bytes
      pos = fin.tellg();
      if (p_saveDataPre) {
        tempPre.push_back(new char[p_dataPreBytes]);
        fin.read (tempPre.back(), p_dataPreBytes);
      }
      else {
        fin.seekg (p_dataPreBytes, ios_base::cur);
      }

      // Check the last io
      if (!fin.good ()) {
       string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                    Isis::iString((int)pos) + "]. Byte count [" +
                    Isis::iString(p_dataPreBytes) + "]" ;
       throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
      }

      // Get a line of data from the input file
      pos = fin.tellg();
      fin.read (in, readBytes);
      if (!fin.good ()) {
       string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                    Isis::iString((int)pos) + "]. Byte count [" + 
                    Isis::iString(readBytes) + "]" ;
       throw Isis::iException::Message(Isis::iException::Io,msg,_FILEINFO_);
      }

      // Loop for each band 
      for (int band=0; band<p_nb; band++) {
        // Set the base multiplier
        double base,mult;
        if (p_base.size() > 1) {
          base = p_base[band];
          mult = p_mult[band];
        }
        else {
          base = p_base[0];
          mult = p_mult[0];
        }

        // Swap the bytes if necessary and convert any out of bounds pixels
        // to special pixels
        int osamp=0; 
        for (int samp=band; samp<p_ns*p_nb; samp+=p_nb) {
          switch (p_pixelType) {
            case Isis::UnsignedByte:
              (*out)[osamp] = (double) ((unsigned char *)in)[samp];
              break;
            case Isis::UnsignedWord:
              (*out)[osamp] = (double)swapper.UnsignedShortInt ((unsigned short int *)in+samp);
              break;
            case Isis::SignedWord:
              (*out)[osamp] = (double)swapper.ShortInt ((short int *)in+samp);
              break;
            case Isis::Real:
              (*out)[osamp] = (double)swapper.Float ((float *)in+samp);
              break;
            default:
              break;
          }

          // Sets out to isis special pixel or leaves it if valid
          (*out)[osamp] = TestPixel((*out)[osamp]);

          if (Isis::IsValidPixel((*out)[osamp])) {
            (*out)[osamp] = mult * ((*out)[osamp]) + base;
          }
	  osamp++;
        } // End sample loop

        if(funct == NULL) {
          //Set the buffer position and write the line to the output file
          ((Isis::LineManager*)out)->SetLine ((band * p_nl) + line + 1);
          OutputCubes[0]->Write (*out);
        }
        else {
          funct(*out);
        }

        // Handle any line suffix bytes
        pos = fin.tellg();
        if (p_saveDataPost) {
          tempPost.push_back(new char[p_dataPostBytes]);
          fin.read (tempPost.back(), p_dataPostBytes);
        }
        else {
          fin.seekg (p_dataPostBytes, ios_base::cur);
        }

        // Check the last io
        if (!fin.good ()) {
         string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                      Isis::iString((int)pos) + "]. Byte count [" +
                      Isis::iString(p_dataPreBytes) + "]" ;
         throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
        }


        // Save off the prefix bytes vector
        if (p_saveDataPre) {
          p_dataPre.push_back(tempPre);
          tempPre.clear();
        }
  
        // Save off the suffix bytes vector
        if (p_saveDataPost) {
          p_dataPost.push_back(tempPost);
          tempPost.clear();
        }
   
        // Check the last io
        if (!fin.good ()) {
         string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                    Isis::iString((int)pos) + "]. Byte count [" +
                      Isis::iString(p_fileHeaderBytes) + "]" ;
         throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
        }
  
      } // End band loop

      p_progress->CheckStatus();

    } // End line loop

    // Handle the file trailer
    pos = fin.tellg();
    if (p_saveFileTrailer) {
      fin.seekg (0, ios_base::end);
      streampos e = fin.tellg();
      p_fileTrailerBytes = (int)(e - pos + (streampos)1);
      p_fileTrailer = new char[p_fileTrailerBytes];
      fin.seekg (pos);
      fin.read (p_fileTrailer, p_fileTrailerBytes);

      // Check the io
      if (!fin.good ()) {
       string msg = "Cannot read file [" + p_inFile + "]. Position [" +
                    Isis::iString((int)pos) + "]. Byte count [" +
                    Isis::iString(p_fileTrailerBytes) + "]" ;
       throw Isis::iException::Message(Isis::iException::Io,msg, _FILEINFO_);
      }

    }

    // Close the file and clean up
    fin.close ();
    delete [] in;

  }


 /**
  * Function to process files containing compressed JPEG2000 data
  * (which is always BSQ but is processed as BIL with one or more
  * lines stored sequentially for each band). There is no need to
  * set up an Isis::EndianSwapper because JP2 data is always
  * assumed to be in MSB format. The Kakadu library does an
  * automatic byte swap for the current architecture.
  *
  * @param funct Method that accepts Isis::Buffer as an input
  *              parameter, processes the image, and has no
  *              return value.
  * @throws Isis::iException::Message "Cannot open input file."
  * @throws Isis::iException::Message "Cannot read file.
  *             Position[]. Byte count[]"
  */
  void ProcessImport::ProcessJp2 (void funct(Isis::Buffer &out)) {

    // Set up an Isis::Jp2Decoder object
    JP2Decoder *JP2_decoder;
    JP2_decoder = new JP2Decoder(p_inFile);

    // Open JP2 file
    JP2_decoder->OpenFile();

    // Make sure JP2 file dimensions match PDS labels
    p_ns = JP2_decoder->GetSampleDimension();
    p_nl = JP2_decoder->GetLineDimension();
    p_nb = JP2_decoder->GetBandDimension();

    // Figure out the number of bytes to read for a single line
    // from all bands
    int sizeofpixel = Isis::SizeOf (p_pixelType);
    int startsamp = p_dataPreBytes / sizeofpixel;
    int endsamp = startsamp + p_ns;
    int readBytes = sizeofpixel * p_ns * p_nb + p_dataPreBytes + p_dataPostBytes;
    char **in = new char* [p_nb];
    for (int i=0; i<p_nb; i++) {
      in[i] = new char [readBytes];
    }

    // Construct a line buffer manager
    Isis::Buffer *out = NULL;

    if(funct != NULL) {
      out = new Isis::Buffer (p_ns, p_nl, p_nb, p_pixelType);
    }
    else {
      out = new Isis::LineManager (*OutputCubes[0]);
    }

    // Loop once for each line in the image
    p_progress->SetMaximumSteps(p_nb*p_nl);
    p_progress->CheckStatus();

    // Loop for each line
    for (int line=0; line<p_nl; line++) {
      if (p_pixelType == Isis::UnsignedByte) {
        JP2_decoder->Read((unsigned char**)in);
      } else {
        JP2_decoder->Read((short int**)in);
      }
      // Loop for each band
      for (int band=0; band<p_nb; band++) {
        // Set the base multiplier
        double base,mult;
        if (p_base.size() > 1) {
          base = p_base[band];
          mult = p_mult[band];
        }
        else {
          base = p_base[0];
          mult = p_mult[0];
        }

        // Space for storing prefix and suffix data pointers
        vector<char *> tempPre, tempPost;


        // Handle any line prefix bytes
        if (p_saveDataPre) {
          tempPre.push_back(new char[p_dataPreBytes]);
          memcpy(&tempPre[0],in[band],p_dataPreBytes);
        }

        // Swap the bytes if necessary and convert any out of bounds pixels
        // to special pixels
        for (int samp=startsamp; samp<endsamp; samp++) {
          switch (p_pixelType) {
            case Isis::UnsignedByte:
              (*out)[samp] = (double) ((unsigned char *)in[band])[samp];
              break;
            case Isis::UnsignedWord:
              (*out)[samp] = (double) ((unsigned short int *)in[band])[samp];
              break;
            case Isis::SignedWord:
              (*out)[samp] = (double) ((short int *)in[band])[samp];
              break;
            default:
              break;
          }

          // Sets out to isis special pixel or leaves it if valid
          (*out)[samp] = TestPixel((*out)[samp]);

          if (Isis::IsValidPixel((*out)[samp])) {
            (*out)[samp] = mult * ((*out)[samp]) + base;
          }
        } // End sample loop

        if(funct == NULL) {
          //Set the buffer position and write the line to the output file
          ((Isis::LineManager*)out)->SetLine ((band * p_nl) + line + 1);
          OutputCubes[0]->Write (*out);
        }
        else {
          funct(*out);
        }

        p_progress->CheckStatus();

        // Handle any line suffix bytes
        if (p_saveDataPost) {
          tempPost.push_back(new char[p_dataPostBytes]);
          memcpy(&tempPost[0],&in[band][p_dataPreBytes+p_ns*sizeofpixel],p_dataPostBytes);
        }

        // Save off the prefix bytes vector
        if (p_saveDataPre) {
          p_dataPre.push_back(tempPre);
          tempPre.clear();
        }

        // Save off the suffix bytes vector
        if (p_saveDataPost) {
          p_dataPost.push_back(tempPost);
          tempPost.clear();
        }

      } // End band loop

    } // End line loop

    // Close the file and clean up
    delete JP2_decoder;
    delete [] in;
  }


  #if 0
  
 /** 
  * Adds a label to the Cube section of the existing cube label using a supplied 
  * Pvl 
  * 
  * @param label The Pvl containing the label to be added.
  */
  void ProcessImport::AddLabel (Isis::Pvl &label) {
  
    label.Root();
  
    Isis::Pvl *output = OutputCubes[0]->Label();
    output->Find ("IsisCube");
    output->Insert(label);
  }

 /** 
  * Adds an ImportLabel to the output cube using a supplied Pvl
  * 
  * @param importlab The Pvl containing the original labels from the import file.
  */
  void ProcessImport::AddImportLabel (Isis::Pvl &importLab) {
  
    importLab.Root();
  
    Isis::Pvl *output = OutputCubes[0]->Label();
    output->Find ("IsisCube");
    output->AddObject ("ImportLabel");
    output->Insert(importLab);
  }
  #endif


 /** 
  * Sets the name of the input file to be read in the import StartProcess method
  * and verifies its existance.
  * 
  * @param file The name of the input file to import.
  * 
  * @throws Isis::iException::Message "File does not exist."
  */
  void ProcessImport::SetInputFile (const std::string &file) {
    p_inFile = file;
    if (!Isis::Filename(file).Exists()) {
      string msg = "File [" + file + "] does not exist";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
  }
}

