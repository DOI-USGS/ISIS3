%module(package="isiscore") Pvl

%{
    #include <sstream>
    #include "PvlKeyword.h"
    #include "PvlContainer.h"
    #include "PvlGroup.h"
    #include "PvlObject.h"
    #include "Pvl.h"
%}

%include <QVector>
%include <QString>

%include "PvlKeyword.h"
%include "PvlContainer.h"
%include "PvlGroup.h"

%extend Isis::PvlGroup {

    // This allows Python's print() to display your object
    const char* __str__() {
        static std::string temp;
        std::ostringstream out;
        out << *($self);
        temp = out.str();
        return temp.c_str();
    }
}

%typemap(in) Isis::PvlObject::FindOptions {
  $1 = (Isis::PvlObject::FindOptions) PyInt_AsLong($input);;
}

%ignore findGroup(const QString &name);
%ignore findGroup(const QString &name, FindOptions opts = None) const;
%ignore findGroup(QString const &name, PvlGroupIterator beg, PvlGroupIterator end);
%ignore findGroup(QString const &name, ConstPvlGroupIterator beg, ConstPvlGroupIterator end) const;

%include "PvlObject.h"

%extend Isis::PvlObject {

    // This allows Python's print() to display your object
    const char* __str__() {
        static std::string temp;
        std::ostringstream out;
        out << *($self);
        temp = out.str();
        return temp.c_str();
    }
}

%rename(PvlFromFile) Isis::Pvl::Pvl(const QString &file);

%include "Pvl.h"

%extend Isis::Pvl {

    // This allows Python's print() to display your object
    const char* __str__() {
        static std::string temp;
        std::ostringstream out;
        out << *($self);
        temp = out.str();
        return temp.c_str();
    }
}
