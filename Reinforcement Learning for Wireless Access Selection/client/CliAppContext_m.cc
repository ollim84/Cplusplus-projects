//
// Generated file, do not edit! Created by opp_msgc 4.1 from applications/ddeclient/CliAppContext.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "CliAppContext_m.h"

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




Register_Class(CliAppContext);

CliAppContext::CliAppContext(const char *name, int kind) : cPacket(name,kind)
{
    this->appIndex_var = 0;
    this->appId_var = 0;
    this->portId_var = 0;
    this->playBufferSize_var = 0;
    this->playBufferState_var = 0;
    this->userClass_var = 0;
}

CliAppContext::CliAppContext(const CliAppContext& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

CliAppContext::~CliAppContext()
{
}

CliAppContext& CliAppContext::operator=(const CliAppContext& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->appIndex_var = other.appIndex_var;
    this->appId_var = other.appId_var;
    this->portId_var = other.portId_var;
    this->playBufferSize_var = other.playBufferSize_var;
    this->playBufferState_var = other.playBufferState_var;
    this->userClass_var = other.userClass_var;
    return *this;
}

void CliAppContext::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->appIndex_var);
    doPacking(b,this->appId_var);
    doPacking(b,this->portId_var);
    doPacking(b,this->playBufferSize_var);
    doPacking(b,this->playBufferState_var);
    doPacking(b,this->userClass_var);
}

void CliAppContext::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->appIndex_var);
    doUnpacking(b,this->appId_var);
    doUnpacking(b,this->portId_var);
    doUnpacking(b,this->playBufferSize_var);
    doUnpacking(b,this->playBufferState_var);
    doUnpacking(b,this->userClass_var);
}

int CliAppContext::getAppIndex() const
{
    return appIndex_var;
}

void CliAppContext::setAppIndex(int appIndex_var)
{
    this->appIndex_var = appIndex_var;
}

int CliAppContext::getAppId() const
{
    return appId_var;
}

void CliAppContext::setAppId(int appId_var)
{
    this->appId_var = appId_var;
}

int CliAppContext::getPortId() const
{
    return portId_var;
}

void CliAppContext::setPortId(int portId_var)
{
    this->portId_var = portId_var;
}

long CliAppContext::getPlayBufferSize() const
{
    return playBufferSize_var;
}

void CliAppContext::setPlayBufferSize(long playBufferSize_var)
{
    this->playBufferSize_var = playBufferSize_var;
}

int CliAppContext::getPlayBufferState() const
{
    return playBufferState_var;
}

void CliAppContext::setPlayBufferState(int playBufferState_var)
{
    this->playBufferState_var = playBufferState_var;
}

int CliAppContext::getUserClass() const
{
    return userClass_var;
}

void CliAppContext::setUserClass(int userClass_var)
{
    this->userClass_var = userClass_var;
}

class CliAppContextDescriptor : public cClassDescriptor
{
  public:
    CliAppContextDescriptor();
    virtual ~CliAppContextDescriptor();

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

Register_ClassDescriptor(CliAppContextDescriptor);

CliAppContextDescriptor::CliAppContextDescriptor() : cClassDescriptor("CliAppContext", "cPacket")
{
}

CliAppContextDescriptor::~CliAppContextDescriptor()
{
}

bool CliAppContextDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<CliAppContext *>(obj)!=NULL;
}

const char *CliAppContextDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int CliAppContextDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 6+basedesc->getFieldCount(object) : 6;
}

unsigned int CliAppContextDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISEDITABLE,
    };
    return (field>=0 && field<6) ? fieldTypeFlags[field] : 0;
}

const char *CliAppContextDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "appIndex",
        "appId",
        "portId",
        "playBufferSize",
        "playBufferState",
        "userClass",
    };
    return (field>=0 && field<6) ? fieldNames[field] : NULL;
}

int CliAppContextDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='a' && strcmp(fieldName, "appIndex")==0) return base+0;
    if (fieldName[0]=='a' && strcmp(fieldName, "appId")==0) return base+1;
    if (fieldName[0]=='p' && strcmp(fieldName, "portId")==0) return base+2;
    if (fieldName[0]=='p' && strcmp(fieldName, "playBufferSize")==0) return base+3;
    if (fieldName[0]=='p' && strcmp(fieldName, "playBufferState")==0) return base+4;
    if (fieldName[0]=='u' && strcmp(fieldName, "userClass")==0) return base+5;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *CliAppContextDescriptor::getFieldTypeString(void *object, int field) const
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
        "long",
        "int",
        "int",
    };
    return (field>=0 && field<6) ? fieldTypeStrings[field] : NULL;
}

const char *CliAppContextDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int CliAppContextDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    CliAppContext *pp = (CliAppContext *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string CliAppContextDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    CliAppContext *pp = (CliAppContext *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getAppIndex());
        case 1: return long2string(pp->getAppId());
        case 2: return long2string(pp->getPortId());
        case 3: return long2string(pp->getPlayBufferSize());
        case 4: return long2string(pp->getPlayBufferState());
        case 5: return long2string(pp->getUserClass());
        default: return "";
    }
}

bool CliAppContextDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    CliAppContext *pp = (CliAppContext *)object; (void)pp;
    switch (field) {
        case 0: pp->setAppIndex(string2long(value)); return true;
        case 1: pp->setAppId(string2long(value)); return true;
        case 2: pp->setPortId(string2long(value)); return true;
        case 3: pp->setPlayBufferSize(string2long(value)); return true;
        case 4: pp->setPlayBufferState(string2long(value)); return true;
        case 5: pp->setUserClass(string2long(value)); return true;
        default: return false;
    }
}

const char *CliAppContextDescriptor::getFieldStructName(void *object, int field) const
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
        NULL,
    };
    return (field>=0 && field<6) ? fieldStructNames[field] : NULL;
}

void *CliAppContextDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    CliAppContext *pp = (CliAppContext *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


