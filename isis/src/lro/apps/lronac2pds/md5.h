/*
 *  This is the C++ implementation of the MD5 Message-Digest
 *  Algorithm desrcipted in RFC 1321.
 *  I translated the C code from this RFC to C++.
 *  There is no warranty.
 *
 *  Feb. 12. 2005
 *  Benjamin Gr�delbach
 */

/*
 * Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 *
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 *
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 *
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 *
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 */

//----------------------------------------------------------------------
//include protection
#ifndef MD5_H
#define MD5_H

//----------------------------------------------------------------------
//STL includes
#include "stdint.h"
#include <string>

//----------------------------------------------------------------------
//typedefs
typedef uint8_t *POINTER;

/*
 * MD5 context.
 */
typedef struct {
  uint32_t state[4];           /* state (ABCD) */
  uint32_t count[2];         /* number of bits, modulo 2^64 (lsb first) */
  uint8_t buffer[64];        /* input buffer */
} MD5_CTX;

/**
 * MD5 class
 *
 * This is the RSA Data Security, Inc. MD5 Message-Digest Algorithm
 *
 * @author ????-??-?? Unknown
 *
 * @internal
 */
class MD5 {

  private:

    void MD5Transform(uint32_t state[4], uint8_t block[64]);
    void Encode(uint8_t *, uint32_t *, uint32_t);
    void Decode(uint32_t *, uint8_t *, uint32_t);
    void MD5_memcpy(POINTER, POINTER, uint32_t);
    void MD5_memset(POINTER, int32_t, uint32_t);

  public:

    void MD5Init(MD5_CTX *);
    void MD5Update(MD5_CTX *, uint8_t *, uint32_t);
    void MD5Final(uint8_t [16], MD5_CTX *);

    MD5() {};
};

//----------------------------------------------------------------------
//End of include protection
#endif

/*
 * EOF
 */
