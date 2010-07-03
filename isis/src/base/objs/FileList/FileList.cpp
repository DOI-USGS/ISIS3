/**                                                                       
 * @file                                                                  
 * $Revision: 1.4 $                                                             
 * $Date: 2008/02/20 16:28:21 $                                                                 
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
#include "FileList.h"
#include "iException.h"
#include "Message.h"
#include "Filename.h"
#include "iString.h"

using namespace std;
namespace Isis {

/**                                                                       
 * Constructs an empty FileList.
 * 
 */                                                                         
  FileList::FileList () {
  }

/**                                                                       
 * Constructs a FileList from a file.
 * 
 * @param list Name of the file containing a list of files.                                                          
 */                                                                       
  FileList::FileList (const std::string &list) {
    Read(list);
  }

/**                                                                       
 * Constructs a FileList from a stream.                                                        
 *                                                                        
 * @param in An input stream containing a list of files.                   
 */                                                                         
  FileList::FileList (std::istream &in) {
    Read(in);
  }

/**                                                                       
 * Opens and loads the list of files from a file.                                                         
 *                                                                        
 * @param list Name of the file to open that contains the list of files.
 * 
 * @throws Isis::iException::Io - Cannot open file
 */                                                                         
  void FileList::Read(const std::string &list) {
    // Set up for opening
    Isis::Filename temp(list);
    string file = temp.Expanded();
    ifstream istm;
  
    // Open the file
    istm.open(file.c_str(),std::ios::in);
    if (!istm) {
      string message = Isis::Message::FileOpen(file);
      throw Isis::iException::Message(Isis::iException::Io,message,_FILEINFO_);
    }
  
    // Internalize
    try {
      Read(istm);

      // Close the file
      istm.close();
    } 
    catch (Isis::iException &e) {
      istm.close();
      string msg = "File [" + file + "] contains no data";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
  
  }
  
/**                                                                       
 * Loads list of files from a stream.
 * This takes in a stream and loads a file list from it. The lines in the stream
 * are considered separate entries, and comments are ignored. comments are
 * considered to be any line starting with a '#' or '//', and anything after any
 * whitespace following the first text on the line.
 * 
 * @param in An input stream containing a list of files.
 * 
 */                                                                         
  void FileList::Read(std::istream &in) {
    // Read each file and put it in the vector
    char buf[65536];
    Isis::iString s;
    bool bHasQuotes = false;
    
    in.getline(buf,65536);
    bool isComment = false;
    while (!in.eof()) {
      s = buf;
      string::size_type loc = s.find("\"", 0);

      if(loc != string::npos) {
        bHasQuotes = true;
      }

      if(bHasQuotes) {
        s = s.TrimHead("\"");
        s = s.TrimTail("\"");
      }

      s = s.TrimHead(" \n\r\t\v");

      isComment = false;
      if (strlen(buf) == 0) {
        in.getline(buf,65536);
        continue;
      }
      for (int index = 0; index < (int)strlen(buf); index++) {
       if (buf[index] == '#' || (buf[index] == '/' && buf[index+1] == '/')) {
         isComment = true;
         break;
       }
        else if (buf[index] == ' ') {
          continue;
        }
        else {
          isComment = false;
          break;
        }
      }
      if (isComment) {
        in.getline(buf,65536);
        continue;
      }
      else{

        if(bHasQuotes) {
          s = s.Token(" \n\r\t\v");
        }else {
          s = s.Token(" \n\r\t\v,");
        }

        this->push_back(s);
        in.getline(buf,65536);
      }
    }
    if (this->size() == 0) {
      string msg = "Input Stream Empty";
      throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
    }
  }
  
/**                                                                       
 * Writes a list of files to a file.                                                          
 *                                                                        
 * @param list The name of the file to create. The method will overwrite any
 * existing files.
 * 
 * @throws Isis::iException::Io File could not be created.                                      
 */                                                                         
  void FileList::Write(const std::string &list) {
    // Set up for opening
    Isis::Filename temp(list);
    string file = temp.Expanded();
    ofstream ostm;
  
    // Open the file
    ostm.open(file.c_str(),std::ios::out);
    if (!ostm) {
      string message = Isis::Message::FileOpen(file);
      throw Isis::iException::Message(Isis::iException::Io,message,_FILEINFO_);
    }
  
    // Internalize
    Write(ostm);
  
    // Close the file
    ostm.close();
  }

/**                                                                       
 * Writes a list of files to a stream.                                                          
 *                                                                        
 * @param out The list will be written to this output stream.                                                           
 */                                                                         
  void FileList::Write(std::ostream &out) {
    for (unsigned int i=0; i<this->size(); i++) {
      out << (*this)[i] << endl;
    }
  }
} // end isis namespace
