#if !defined(Component_h)
#define Component_h
/**                                                                       
 * @file                                                                  
 * $Revision: 1.1 $
 * $Date: 2009/09/16 03:37:22 $
 * $Id: Component.h,v 1.1 2009/09/16 03:37:22 kbecker Exp $
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

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include "HiCalTypes.h"
#include "HiCalUtil.h"
#include "PvlKeyword.h"
#include "PvlGroup.h"
#include "iException.h"

namespace Isis {

  class PvlGroup;

  /**
   * @brief Component manages HiRISE calibration vectors from various sources
   * 
   * @ingroup Utility
   * @author 2007-10-05 Kris Becker
   */
  class Component {
    public:
      friend std::ostream &operator<<(std::ostream &o, const Component &c) {
        c.printOn(o);
        return (o);
      }

    public: 
      //  Constructors and Destructor
      Component() : _name("Component"), _data(), _history(), 
                  _fmtWidth(DefaultWidth),  _fmtPrecision(DefaultPrecision) { }
      Component(const std::string &name) : _name(name), _data(), _history(), 
                  _fmtWidth(DefaultWidth),  _fmtPrecision(DefaultPrecision) { }
      Component(const std::string &name, const HiHistory &history) : 
                _name(name), _data(), _history(history),  
                _fmtWidth(DefaultWidth),  _fmtPrecision(DefaultPrecision) { }
      Component(const std::string &name, const Component &c) : _name(name),
                _data(c._data), _history(c._history), _fmtWidth(c._fmtWidth),
                _fmtPrecision(c._fmtPrecision) { }
      Component(const Component &c) : _name(c._name), _data(c._data),
                                      _history(c._history),
                                      _fmtWidth(c._fmtWidth),
                                      _fmtPrecision(c._fmtPrecision) { }


      /** Destructor */
      virtual ~Component() { }

      inline std::string name() const { return (_name); }
      inline int size() const { return (_data.dim()); }

      virtual void Process(const Component &c) {
        Process(c.ref());
      }

      virtual void Process(const HiVector &v) {
        _data = v;
        return;
      }

      const HiVector &ref() const { return (_data); }
      inline double operator()(int index) const { return (_data[index]);}

      const HiHistory &History() const { return (_history); }

      virtual void record(PvlGroup &pvl, 
                          const std::string keyname = "ComponentHistory") 
                          const {
        pvl += _history.makekey(keyname);
        return;
      }

      /**
       * @brief Dumps the component to a specified file
       * 
       * @param fname  Name of file to dump contents to
       * 
       */
      void Dump(const std::string &fname) const {
        Filename dumpc(fname);
        std::string dumpcFile = dumpc.Expanded();
        std::ofstream ofile(dumpcFile.c_str(), std::ios::out);
        if (!ofile) {
          std::string mess = "Unable to open/create module dump file " + 
                             dumpc.Expanded();
          throw iException::Message(iException::User, mess, _FILEINFO_);
        }
        ofile << *this;
        ofile.close();
        return;
      }


    protected:
      enum { DefaultWidth = 10, DefaultPrecision = 6};

      std::string   _name;         //!< Name of component
      HiVector      _data;         //!< Data vector 
      HiHistory     _history;      //!< Hierarchial component history
      int           _fmtWidth;     //!< Default field with of double
      int           _fmtPrecision; //!< Default field with of double

      /**
       * @brief Properly format values that could be special pixels
       * 
       * This method applies ISIS special pixel value conventions to properly 
       * print pixel values.
       * 
       * @param[in] (double) value Input value to test for specialness and print
       *                           as requested by caller
       * @param[in] (int) width Width of field in which to print the value
       * @param[in] (int) prec  Precision used to format the value
       * @return (string) Formatted double value
       */
      inline std::string formatDbl(const double &value) const {
        std::ostringstream ostr;
        if (IsSpecial(value)) {
          ostr << std::setw(_fmtWidth) << PixelToString(value);
          return (std::string(ostr.str()));
        }
        else {
        // Its not special so format to callers specs
          ostr << std::setw(_fmtWidth) << std::setprecision(_fmtPrecision) << value;
          return (std::string(ostr.str()));
        }
      }

      virtual void printOn(std::ostream &o) const {
        o << "#  History = " << _history << std::endl;
        o << "#  Count =   " << _data.dim() << std::endl;
        for (int i = 0 ; i < _data.dim() ; i++) {
          o << formatDbl(_data[i]) << std::endl;
        }
        return;
      }

  };

}     // namespace Isis
#endif
