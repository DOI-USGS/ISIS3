#ifndef TextFile_h
#define TextFile_h
/**
 * @file
 * $Revision: 1.2 $
 * $Date: 2010/03/30 17:45:42 $
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

#include <fstream>
#include <vector>

namespace Isis {
  /**
   * @brief Provides access to sequential ASCII stream I/O
   *
   * Provides access to sequential ASCII stream I/O. Checks for errors in 
   * opening, reading, and writing ASCII files. Can check for 'comment lines' 
   * while reading a text file. 
   *
   * @ingroup Utility
   *
   * @author 2003-08-29 Glenn Bennett
   *
   * @internal
   *  @history 2005-02-16 Elizabeth Ribelin - Modified file to support Doxygen
   *                          documentation
   *  @history 2010-03-30 Mackenzie Boyd - Modified open method append option to
   *                          create the file if it doesn't exist, changed file
   *                          existence check to use FileName class.
   *  @history 2011-08-08 Jeannie Backer - Modified unitTest to use $temporary
   *                          variable instead of /tmp directory
   *
   *  @todo 2005-02-16 Glenn Bennett - add coded and implementation examples,
   *                       and finish documentation
   */
  class TextFile {

    protected:
      std::fstream p_stream;           //!< File stream handle
      int     p_openmode;              /**< openmode of file: Input, Output,
                                           Overwrite, Append*/
      std::string  p_filename;         //!< FileName of the opened file.
      std::string  p_commentString;    /**< 'comment' string used by GetLine and
                                           PutLineComment*/
      std::string  p_newLineString;    /**< 'newline' string used by PutLine and
                                           PutLineComment*/

      bool p_GetLine(std::string &line, bool chkComment);

    public:
      // Constructors and destructors
      TextFile();

      // implied open file
      TextFile(const std::string &filename, const char *openmode = "input",
               const char *extension = "");

      // implied getFile/putFile
      TextFile(const   char *filename, const char *openmode,
               std::vector<std::string> &lines,
               const int &maxLinesToReadWrite = 0,
               const bool skipComments = true);
      TextFile(const std::string &filename,
               const char *openmode, std::vector<std::string> &lines,
               const int &maxLinesToReadWrite = 0,
               const bool skipComments = true);
      TextFile(const   char *filename,
               const char *openmode, std::string *lines,
               const int &maxLinesToReadWrite, const bool skipComments = true);
      TextFile(const std::string &filename,
               const char *openmode, std::string *lines,
               const int &maxLinesToReadWrite, const bool skipComments = true);

      ~TextFile();

      void Open(const std::string &filename, const char *openmode = "input",
                const char *extension = "");
      void Open(const char *filename, const char *openmode = "input",
                const char *extension = "");

      bool OpenChk(bool bailIfNotOpen = false);

      void Rewind();
      void Close();

      void GetFile(std::vector<std::string> &lines, 
                   const int &maxLinesToRead = 0, 
                   const bool skipComments = true);
      void GetFile(std::string *lines, const int &maxLinesToRead, 
                   const bool skipComments = true);

      void PutFile(std::vector<std::string> &lines, 
                   const int &maxLinesToWrite = 0);
      void PutFile(const std::string *lines, const int &maxLinesToWrite);

      bool GetLine(std::string &line, const bool skipComments = true);
      bool GetLineNoFilter(std::string &line);

      bool GetLine(const bool skipComments = true);
      bool GetLineNoFilter();

      void PutLine(const std::string &line);
      void PutLine(const char *line = "");

      void PutLineComment(const std::string &line);
      void PutLineComment(const char *line = "");

      std::string GetComment();
      std::string GetNewLine();

      void SetComment(const char *commentString = "#");
      void SetNewLine(const char *newLineString = "\n");

      int LineCount(const int &maxLinesToRead = 0);
      std::streamsize Size();
  };
};

#endif
