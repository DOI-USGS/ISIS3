#ifndef CkKernelWriter_h
#define CkKernelWriter_h
/**                                                                       
 * @file                                                                  
 * $Revision$ 
 * $Date$
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
 *  
 *   $Id$
 */                                                                       
#include <QString>

namespace Isis {

class SpiceSegment;
/**
 * @brief Support writting of CK kernels to NAIF SPICE files
 * 
 * This class is designed to support the fundamentals of opening/creating a NAIF
 * CK kernel file for writing if CK segments.  There are currently up to 5
 * different types of CK kernels that NAIF supports in its toolkit.  We are
 * mainly concerned with creation of type 3 CK kernels.
 * 
 * As such, this object provides the ability to create general CK files, specify
 * comment section size, get the I/O handle associated with the output file,
 * write comments to the kernel and then close the file when you are done.
 * 
 * The thing you may see as missing is the write method.  Due to the structure
 * of the NAIF CK writing procedure, each CK type has its own routine that has
 * different arguments.  This makes it "messy" to try and support writing of all
 * CK types directly in this class.  It makes more sense, to me anyway, that the
 * environment that contains the data to write can better provide the data
 * required to properly write the CK segment.  See the implementation of
 * SpiceSegment which provides this procedure.  It will have a method that
 * looks like SpiceSegment::write(CkKernelWriter &writer).  This class exposes
 * the handle need to write the segment and the addComment() method to add
 * details in the kernel file regarding the segment being written.
 *
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class CkKernelWriter {
  public:
    CkKernelWriter();
    CkKernelWriter(const QString &kfile, const int &csize = 0, 
                   const int &cktype = 3);
    virtual ~CkKernelWriter() { close(); }

    void setType(const int cktype);

    void setCommentSize(const int &comsize);
    int getCommentSize() const { return (_comSize); }
    int getCommentWrittenCount() const { return (_comCharsWritten); }

    bool addComment(const QString &comment);
    bool addCommentFile(const QString &comfile);

    void open(const QString &kfile, 
              const QString &intCkName = "USGS_CK_File");
    void write(const SpiceSegment &segment) const;
    void close();

  private:
    enum { DefaultCommentSize = 7000 };
    enum { DefaultCkType = 3 };
    int          _ckType;
    int          _handle;
    int          _comSize;
    mutable unsigned int _comCharsWritten;

    void init();
    bool writeComment(const QString &comment) const;
    void writeCk1(const SpiceSegment &segment) const;
    void writeCk2(const SpiceSegment &segment) const;
    void writeCk3(const SpiceSegment &segment) const;

};

};     // namespace Isis
#endif


