/*
 *  This is my wrapper-class to create
 *  a MD5 Hash from a string and a file.
 *
 *  This code is completly free, you
 *  can copy it, modify it, or do
 *  what ever you want with it.
 *
 *  Feb. 2005
 *  Benjamin Grüdelbach
 */

//include protection
#ifndef MD5WRAPPER_H
#define MD5WRAPPER_H

//basic includes
#include <QString>

//forwards
class MD5;

/**
 * @author 2005-02-?? Benjamin Grudelbach
 *
 * @internal
 */
class md5wrapper {
  private:
    MD5 *md5;

    /*
     * internal hash function, calling
     * the basic methods from md5.h
     */
    QString hashit(QString text);

    /*
     * converts the numeric giets to
     * a valid QString
     */
    QString convToString(unsigned char *bytes);
  public:
    //constructor
    md5wrapper();

    //destructor
    ~md5wrapper();

    /*
     * creates a MD5 hash from
     * "text" and returns it as
     * string
     */
    QString getHashFromString(QString text);

    /*
     * creates a MD5 hash from
     * a file specified in "filename" and
     * returns it as string
     */
    QString getHashFromFile(QString filename);
};


//include protection
#endif

/*
 * EOF
 */
