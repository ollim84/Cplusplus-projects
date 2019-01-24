//
// Generated file, do not edit! Created by opp_msgc 4.1 from applications/tcpapp/ContextInformation.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "ContextInformation_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




Register_Class(ContextInformation);

ContextInformation::ContextInformation(const char *name, int kind) : cPacket(name,kind)
{
    this->appId_var = 0;
    this->bytesToSend_var = 0;
    this->chunkSize_var = 0;
    this->txRate_var = 0;
    this->iatChunk_var = 0;
}

ContextInformation::ContextInformation(const ContextInformation& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

ContextInformation::~ContextInformation()
{
}

ContextInformation& ContextInformation::operator=(const ContextInformation& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->appId_var = other.appId_var;
    this->bytesToSend_var = other.bytesToSend_var;
    this->chunkSize_var = other.chunkSize_var;
    this->txRate_var = other.txRate_var;
    this->iatChunk_var = other.iatChunk_var;
    return *this;
}

void ContextInformation::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->appId_var);
    doPacking(b,this->bytesToSend_var);
    doPacking(b,this->chunkSize_var);
    doPacking(b,this->txRate_var);
    doPacking(b,this->iatChunk_var);
}

void ContextInformation::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->appId_var);
    doUnpacking(b,this->bytesToSend_var);
    doUnpacking(b,this->chunkSize_var);
    doUnpacking(b,this->txRate_var);
    doUnpacking(b,this->iatChunk_var);
}

int ContextInformation::getAppId() const
{
    return appId_var;
}

void ContextInformation::setAppId(int appId_var)
{
    this->appId_var = appId_var;
}

int ContextInformation::getBytesToSend() const
{
    return bytesToSend_var;
}

void ContextInformation::setBytesToSend(int bytesToSend_var)
{
    this->bytesToSend_var = bytesToSend_var;
}

int ContextInformation::getChunkSize() const
{
    return chunkSize_var;
}

void ContextInformation::setChunkSize(int chunkSize_var)
{
    this->chunkSize_var = chunkSize_var;
}

double ContextInformation::getTxRate() const
{
    return txRate_var;
}

void ContextInformation::setTxRate(double txRate_var)
{
    this->txRate_var = txRate_var;
}

double ContextInformation::getIatChunk() const
{
    return iatChunk_var;
}

void ContextInformation::setIatChunk(double iatChunk_var)
{
    this->iatChunk_var = iatChunk_var;
}

class ContextInformationDescriptor : public cClassDescriptor
{
  public:
    ContextInformationDescriptor();
    virtual ~ContextInformationDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(ContextInformationDescriptor);

ContextInformationDescriptor::ContextInformationDescriptor() : cClassDescriptor("ContextInformation", "cPacket")
{
}

ContextInformationDescriptor::~ContextInformationDescriptor()
{
}

bool ContextInformationDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ContextInformation *>(obj)!=NULL;
}

const char *ContextInformationDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ContextInformationDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 5+basedesc->getFieldCount(object) : 5;
}

unsigned int ContextInformationDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<5) ? fieldTypeFlags[field] : 0;
}

const char *ContextInformationDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "appId",
        "bytesToSend",
        "chunkSize",
        "txRate",
        "iatChunk",
    };
    return (field>=0 && field<5) ? fieldNames[field] : NULL;
}

int ContextInformationDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='a' && strcmp(fieldName, "appId")==0) return base+0;
    if (fieldName[0]=='b' && strcmp(fieldName, "bytesToSend")==0) return base+1;
    if (fieldName[0]=='c' && strcmp(fieldName, "chunkSize")==0) return base+2;
    if (fieldName[0]=='t' && strcmp(fieldName, "txRate")==0) return base+3;
    if (fieldName[0]=='i' && strcmp(fieldName, "iatChunk")==0) return base+4;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ContextInformationDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "double",
        "double",
    };
    return (field>=0 && field<5) ? fieldTypeStrings[field] : NULL;
}

const char *ContextInformationDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int ContextInformationDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ContextInformation *pp = (ContextInformation *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ContextInformationDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ContextInformation *pp = (ContextInformation *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getAppId());
        case 1: return long2string(pp->getBytesToSend());
        case 2: return long2string(pp->getChunkSize());
        case 3: return double2string(pp->getTxRate());
        case 4: return double2string(pp->getIatChunk());
        default: return "";
    }
}

bool ContextInformationDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ContextInformation *pp = (ContextInformation *)object; (void)pp;
    switch (field) {
        case 0: pp->setAppId(string2long(value)); return true;
        case 1: pp->setBytesToSend(string2long(value)); return true;
        case 2: pp->setChunkSize(string2long(value)); return true;
        case 3: pp->setTxRate(string2double(value)); return true;
        case 4: pp->setIatChunk(string2double(value)); return true;
        default: return false;
    }
}

const char *ContextInformationDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<5) ? fieldStructNames[field] : NULL;
}

void *ContextInformationDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ContextInformation *pp = (ContextInformation *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


