%module(package="isiscore") Blob
%{
    #include "PvlKeyword.h"
    #include "PvlGroup.h"
    #include "PvlObject.h"
    #include "Blob.h"
    #include "TableField.h"
    #include "TableRecord.h"
    #include "Table.h"
%}

%include <QVector>
%include <QString>

%include "PvlKeyword.h"
%include "PvlGroup.h"
%include "PvlObject.h"

%include "Blob.h"

%include "TableField.h"
%extend Isis::TableField {
    double __float__() {
        return (double)*$self;
    }

    int __int__() {
        return (int)*$self;
    }

    std::string __str__() {
        return ((QString)*$self).toStdString();
    }
}

%include "TableRecord.h"
%extend Isis::TableRecord {
    TableField __getitem__(int i) {
        return (*$self)[i];
    }
    void __setitem__(int i, TableField val) {
        (*$self)[i] = val;
    }
}

%include "Table.h"
%extend Isis::Table {
    TableRecord __getitem__(int i) {
        return (*$self)[i];
    }
    void __setitem__(int i, TableRecord val) {
        (*$self)[i] = val;
    }
}