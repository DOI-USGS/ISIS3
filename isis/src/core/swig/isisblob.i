%module isisblob
%{
    #include "Blob.h"
%}

%include "Blob.h"
%nodefaultdtor Isis::Blob;

%extend Isis::Blob {

  Blob(const char* name, const char* type) {
    QString qname(name);
    QString qtype(type);
    Isis::Blob *blob = new Isis::Blob(qname, qtype);
    return blob;
  }

  Blob(const char* name, const char* type, const char* file) {
    QString qname(name);
    QString qtype(type);
    QString qfile(file);
    Isis::Blob *blob = new Isis::Blob(qname, qtype, file);
    return blob;
  }

  setData(const char *buffer, int nbytes) {
    $self->setData(buffer, nbytes);
  }

}
