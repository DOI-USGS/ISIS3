#include <iostream>
#include "Blob.h"
#include "IException.h"
#include "Preference.h"

using namespace std;

class MyBlob : public Isis::Blob {
  public:
    MyBlob(const string &name) : Isis::Blob(name, "Blob") {};
    MyBlob(const string &name, const string &file) : Isis::Blob(name, "Blob", file) {};
    void MyBuf(char *buf, int size) {
      p_buffer = new char[size];
      p_nbytes = size;
      memcpy(p_buffer, buf, size);
    };
    int Bytes() {
      return p_nbytes;
    };
    int StartByte() {
      return p_startByte;
    };
    void GetBuf(char *buf) {
      memcpy(buf, p_buffer, p_nbytes);
    }
};

int main(int argc, char *argv[]) {
  Isis::Preference::Preferences(true);
  try {
    MyBlob b("UnitTest");
    char buf[] = {"ABCD"};
    b.MyBuf(buf, 4);
    b.Write("junk");

    MyBlob c("UNITtest", "junk");
    cout << c.Bytes() << endl;
    cout << c.StartByte() << endl;
    char buf2[5];
    c.GetBuf(buf2);
    buf2[4] = 0;
    cout << buf2 << endl;

    // Test writing into existing space
    Isis::Pvl pvl("junk");
    fstream strm;
    strm.open("junk", std::ios::binary | std::ios::out);
    c.MyBuf(buf, 3);
    c.Write(pvl, strm);
    strm.seekp(0, std::ios::beg);
    strm << pvl;
    cout << c.Bytes() << endl;
    cout << c.StartByte() << endl;
    strm.close();

    // Test writing over existing space at the end of file
    Isis::Pvl pvl2("junk");
    fstream strm2;
    strm2.open("junk", std::ios::binary | std::ios::out);
    c.MyBuf(buf, 4);
    c.Write(pvl2, strm2);
    strm2.seekp(0, std::ios::beg);
    strm2 << pvl2;
    cout << c.Bytes() << endl;
    cout << c.StartByte() << endl;
    strm2.close();

    remove("junk");

  }
  catch(Isis::IException &e) {
    e.print();
  }
}

