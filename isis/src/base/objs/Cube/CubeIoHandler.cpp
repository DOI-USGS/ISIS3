/**                                                                       
 * @file                                                                  
 * $Revision: 1.2 $                                                             
 * $Date: 2007/01/30 22:12:22 $                                                                 
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

#include "CubeIoHandler.h"
#include "iString.h"
#include "PixelType.h"
#include "iException.h"
#include "iException.h"
#include "SpecialPixel.h"
#include "Endian.h"

using namespace std;
namespace Isis {
  CubeIoHandler::CubeIoHandler(IsisCubeDef &cube) {
    p_cube = &cube;
    if (p_cube->byteOrder == Isis::Msb) {
      p_native = Isis::IsBigEndian();
    }
    else {
      p_native = Isis::IsLittleEndian();
    }
  }
  
  CubeIoHandler::~CubeIoHandler() {
    if (p_cube->stream.is_open()) {
      p_cube->stream.close();
    }
  }
  
  void CubeIoHandler::Open() {
    // Attempt to open the file
    ios::openmode flags = std::ios::in | std::ios::binary;
    if (p_cube->access == IsisCubeDef::ReadWrite) flags |= std::ios::out;
    p_cube->stream.clear();
    p_cube->stream.open(p_cube->dataFile.c_str(),flags);
    if (!p_cube->stream) {
      string message = "Unable to open cube data file [" +
                       p_cube->dataFile + "]";
      throw Isis::iException::Message(Isis::iException::Io,message,_FILEINFO_);
    }
  }
  
  void CubeIoHandler::Create(bool overwrite) {
    // If we are not allowed to overwrite then complain if the file exists
    if (!overwrite) {
      ios::openmode flags = std::ios::in | std::ios::binary;
      p_cube->stream.open(p_cube->dataFile.c_str(),flags);
      if (p_cube->stream) {
        p_cube->stream.close();
        string msg = "Cube file [" + p_cube->dataFile + "] exists, " +
                     "user preference does not allow overwrite";
        throw Isis::iException::Message(Isis::iException::User,msg,_FILEINFO_);
      }
    }
  
    // Attempt to create the file
    p_cube->stream.clear();
    ios::openmode flags = std::ios::in | std::ios::binary | std::ios::out |
                          std::ios::trunc;
    p_cube->stream.open(p_cube->dataFile.c_str(),flags);
    if (!p_cube->stream) {
      string message = "Unable to open data file [" + p_cube->dataFile + "]";
      throw Isis::iException::Message(Isis::iException::Io,message,_FILEINFO_);
    }
  
    // Zero out label area
    if (p_cube->startByte > 1) {
      char *temp = new char[p_cube->startByte - 1];
      memset(temp,0,p_cube->startByte-1);
      p_cube->stream.seekp(0,std::ios::beg);
      p_cube->stream.write(temp,p_cube->startByte - 1);
      delete [] temp;
    }
  }
  
  void CubeIoHandler::ToDouble(Isis::Buffer &rbuf) {
    if (rbuf.PixelType() == Isis::UnsignedByte) {
      double *dbuf = rbuf.DoubleBuffer();
      unsigned char *cbuf = (unsigned char *) rbuf.RawBuffer();
      for (int i=0; i<rbuf.size(); i++) {
        if (cbuf[i] == Isis::NULL1) {
          dbuf[i] = Isis::NULL8;
        }
        else if (cbuf[i] == Isis::HIGH_REPR_SAT1) {
          dbuf[i] = Isis::HIGH_REPR_SAT8;
        }
        else {
          dbuf[i] = (double) cbuf[i] * p_cube->multiplier + p_cube->base;
        }
      }
    }
    else if (rbuf.PixelType() == Isis::SignedWord) {
      double *dbuf = rbuf.DoubleBuffer();
      short int *sbuf = (short *) rbuf.RawBuffer();
      for (int i=0; i<rbuf.size(); i++) {
        if (sbuf[i] < Isis::VALID_MIN2) {
          if (sbuf[i] == Isis::NULL2) dbuf[i] = Isis::NULL8;
          else if (sbuf[i] == Isis::LOW_INSTR_SAT2) dbuf[i] = Isis::LOW_INSTR_SAT8;
          else if (sbuf[i] == Isis::LOW_REPR_SAT2) dbuf[i] = Isis::LOW_REPR_SAT8;
          else if (sbuf[i] == Isis::HIGH_INSTR_SAT2) dbuf[i] = Isis::HIGH_INSTR_SAT8;
          else if (sbuf[i] == Isis::HIGH_REPR_SAT2) dbuf[i] = Isis::HIGH_REPR_SAT8;
          else dbuf[i] = Isis::LOW_REPR_SAT8;
        }
        else {
          dbuf[i] = (double) sbuf[i] * p_cube->multiplier + p_cube->base;
        }
      }
    }
    else {
      double *dbuf = rbuf.DoubleBuffer();
      float *fbuf = (float *) rbuf.RawBuffer();
      for (int i=0; i<rbuf.size(); i++) {
        if (fbuf[i] < Isis::VALID_MIN4) {
          if (fbuf[i] == Isis::NULL4) dbuf[i] = Isis::NULL8;
          else if (fbuf[i] == Isis::LOW_INSTR_SAT4) dbuf[i] = Isis::LOW_INSTR_SAT8;
          else if (fbuf[i] == Isis::LOW_REPR_SAT4) dbuf[i] = Isis::LOW_REPR_SAT8;
          else if (fbuf[i] == Isis::HIGH_INSTR_SAT4) dbuf[i] = Isis::HIGH_INSTR_SAT8;
          else if (fbuf[i] == Isis::HIGH_REPR_SAT4) dbuf[i] = Isis::HIGH_REPR_SAT8;
          else dbuf[i] = Isis::LOW_REPR_SAT8;
        }
        else {
          dbuf[i] = (double) fbuf[i];
        }
      }
    }
  }
  
  
  
  void CubeIoHandler::ToRaw(Isis::Buffer &rbuf) {
    if (rbuf.PixelType() == Isis::UnsignedByte) {
      double *dbuf = rbuf.DoubleBuffer();
      unsigned char *cbuf = (unsigned char *) rbuf.RawBuffer();
      double temp;
      for (int i=0; i<rbuf.size(); i++) {
        if (dbuf[i] < Isis::VALID_MIN8) {
          if      (dbuf[i] == Isis::NULL8)           cbuf[i] = Isis::NULL1;
          else if (dbuf[i] == Isis::LOW_INSTR_SAT8)  cbuf[i] = Isis::LOW_INSTR_SAT1;
          else if (dbuf[i] == Isis::LOW_REPR_SAT8)   cbuf[i] = Isis::LOW_REPR_SAT1;
          else if (dbuf[i] == Isis::HIGH_INSTR_SAT8) cbuf[i] = Isis::HIGH_INSTR_SAT1;
          else if (dbuf[i] == Isis::HIGH_REPR_SAT8)  cbuf[i] = Isis::HIGH_REPR_SAT1;
          else                                       cbuf[i] = Isis::LOW_REPR_SAT1;
        }
        else {
          temp = (dbuf[i] - p_cube->base) / p_cube->multiplier;
          if (temp < Isis::VALID_MIN1 - 0.5) {
            cbuf[i] = Isis::LOW_REPR_SAT1;
          }
          else if (temp > Isis::VALID_MAX1 + 0.5) {
            cbuf[i] = Isis::HIGH_REPR_SAT1;
          }
          else {
            int itemp = (int) (temp + 0.5);
            if (itemp < Isis::VALID_MIN1) {
              cbuf[i] = Isis::LOW_REPR_SAT1;
            }
            else if (itemp > Isis::VALID_MAX1) {
              cbuf[i] = Isis::HIGH_REPR_SAT1;
            }
            else {
              cbuf[i] = (unsigned char) (temp + 0.5);
            }
          }
        }
      }
    }
    else if (rbuf.PixelType() == Isis::SignedWord) {
      double *dbuf = rbuf.DoubleBuffer();
      short *sbuf = (short *) rbuf.RawBuffer();
      double temp;
      for (int i=0; i<rbuf.size(); i++) {
        if (dbuf[i] < Isis::VALID_MIN8) {
          if      (dbuf[i] == Isis::NULL8)           sbuf[i] = Isis::NULL2;
          else if (dbuf[i] == Isis::LOW_INSTR_SAT8)  sbuf[i] = Isis::LOW_INSTR_SAT2;
          else if (dbuf[i] == Isis::LOW_REPR_SAT8)   sbuf[i] = Isis::LOW_REPR_SAT2;
          else if (dbuf[i] == Isis::HIGH_INSTR_SAT8) sbuf[i] = Isis::HIGH_INSTR_SAT2;
          else if (dbuf[i] == Isis::HIGH_REPR_SAT8)  sbuf[i] = Isis::HIGH_REPR_SAT2;
          else                                       sbuf[i] = Isis::LOW_REPR_SAT2;
        }
        else {
          temp = (dbuf[i] - p_cube->base) / p_cube->multiplier;
          if (temp < Isis::VALID_MIN2 - 0.5) {
            sbuf[i] = Isis::LOW_REPR_SAT2;
          }
          if (temp > Isis::VALID_MAX2 + 0.5) {
            sbuf[i] = Isis::HIGH_REPR_SAT2;
          }
          else {
            int itemp;
            if (temp < 0.0) {
              itemp = (int) (temp - 0.5);
            }
            else {
              itemp = (int) (temp + 0.5);
            }
  
            if (itemp < Isis::VALID_MIN2) {
              sbuf[i] = Isis::LOW_REPR_SAT2;
            }
            else if (itemp > Isis::VALID_MAX2) {
              sbuf[i] = Isis::HIGH_REPR_SAT2;
            }
            else if (temp < 0.0) {
              sbuf[i] = (short) (temp - 0.5);
            }
            else {
              sbuf[i] = (short) (temp + 0.5);
            }
          }
        }
      }
    }
    else {
      double *dbuf = rbuf.DoubleBuffer();
      float *fbuf = (float *) rbuf.RawBuffer();
      double temp;
      for (int i=0; i<rbuf.size(); i++) {
        if (dbuf[i] < Isis::VALID_MIN8) {
          if      (dbuf[i] == Isis::NULL8)           fbuf[i] = Isis::NULL4;
          else if (dbuf[i] == Isis::LOW_INSTR_SAT8)  fbuf[i] = Isis::LOW_INSTR_SAT4;
          else if (dbuf[i] == Isis::LOW_REPR_SAT8)   fbuf[i] = Isis::LOW_REPR_SAT4;
          else if (dbuf[i] == Isis::HIGH_INSTR_SAT8) fbuf[i] = Isis::HIGH_INSTR_SAT4;
          else if (dbuf[i] == Isis::HIGH_REPR_SAT8)  fbuf[i] = Isis::HIGH_REPR_SAT4;
          else                                       fbuf[i] = Isis::LOW_REPR_SAT4;
        }
        else {
          temp = (dbuf[i] - p_cube->base) / p_cube->multiplier;
          if (temp < (double) Isis::VALID_MIN4) {
            fbuf[i] = Isis::LOW_REPR_SAT4;
          }
          else if (temp > (double) Isis::VALID_MAX4) {
            fbuf[i] = Isis::HIGH_REPR_SAT4;
          }
          else {
            fbuf[i] = (float) temp;
          }
        }
      }
    }
  }
}
