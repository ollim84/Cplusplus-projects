//
// Generated file, do not edit! Created by opp_msgc 4.1 from applications/tcpapp/AppContext.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "AppContext_m.h"

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




Register_Class(AppContext);

AppContext::AppContext(const char *name, int kind) : cPacket(name,kind)
{
    this->appIndex_var = 0;
    this->appId_var = 0;
    this->portId_var = 0;
    this->fileSize_var = 0;
    this->remainingFileSize_var = 0;
    this->cxStatus_var = false;
    this->playBufferSize_var = 0;
    this->txDuration_var = 0;
    this->chunkSize_var = 0;
    this->codecRate_var = 0;
    this->initialPlayStarted_var = 0;
    this->userClass_var = 0;
}

AppContext::AppContext(const AppContext& other) : cPacket()
{
    setName(other.getName());
    operator=(other);
}

AppContext::~AppContext()
{
}

AppContext& AppContext::operator=(const AppContext& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    this->appIndex_var = other.appIndex_var;
    this->appId_var = other.appId_var;
    this->portId_var = other.portId_var;
    this->fileSize_var = other.fileSize_var;
    this->remainingFileSize_var = other.remainingFileSize_var;
    this->cxStatus_var = other.cxStatus_var;
    this->playBufferSize_var = other.playBufferSize_var;
    this->txDuration_var = other.txDuration_var;
    this->chunkSize_var = other.chunkSize_var;
    this->codecRate_var = other.codecRate_var;
    this->initialPlayStarted_var = other.initialPlayStarted_var;
    this->userClass_var = other.userClass_var;
    return *this;
}

void AppContext::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->appIndex_var);
    doPacking(b,this->appId_var);
    doPacking(b,this->portId_var);
    doPacking(b,this->fileSize_var);
    doPacking(b,this->remainingFileSize_var);
    doPacking(b,this->cxStatus_var);
    doPacking(b,this->playBufferSize_var);
    doPacking(b,this->txDuration_var);
    doPacking(b,this->chunkSize_var);
    doPacking(b,this->codecRate_var);
    doPacking(b,this->initialPlayStarted_var);
    doPacking(b,this->userClass_var);
}

void AppContext::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->appIndex_var);
    doUnpacking(b,this->appId_var);
    doUnpacking(b,this->portId_var);
    doUnpacking(b,this->fileSize_var);
    doUnpacking(b,this->remainingFileSize_var);
    doUnpacking(b,this->cxStatus_var);
    doUnpacking(b,this->playBufferSize_var);
    doUnpacking(b,this->txDuration_var);
    doUnpacking(b,this->chunkSize_var);
    doUnpacking(b,this->codecRate_var);
    doUnpacking(b,this->initialPlayStarted_var);
    doUnpacking(b,this->userClass_var);
}

int AppContext::getAppIndex() const
{
    return appIndex_var;
}

void AppContext::setAppIndex(int appIndex_var)
{
    this->appIndex_var = appIndex_var;
}

int AppContext::getAppId() const
{
    return appId_var;
}

void AppContext::setAppId(int appId_var)
{
    this->appId_var = appId_var;
}

int AppContext::getPortId() const
{
    return portId_var;
}

void AppContext::setPortId(int portId_var)
{
    this->portId_var = portId_var;
}

int AppContext::getFileSize() const
{
    return fileSize_var;
}

void AppContext::setFileSize(int fileSize_var)
{
    this->fileSize_var = fileSize_var;
}

int AppContext::getRemainingFileSize() const
{
    return remainingFileSize_var;
}

void AppContext::setRemainingFileSize(int remainingFileSize_var)
{
    this->remainingFileSize_var = remainingFileSize_var;
}

bool AppContext::getCxStatus() const
{
    return cxStatus_var;
}

void AppContext::setCxStatus(bool cxStatus_var)
{
    this->cxStatus_var = cxStatus_var;
}

int AppContext::getPlayBufferSize() const
{
    return playBufferSize_var;
}

void AppContext::setPlayBufferSize(int playBufferSize_var)
{
    this->playBufferSize_var = playBufferSize_var;
}

double AppContext::getTxDuration() const
{
    return txDuration_var;
}

void AppContext::setTxDuration(double txDuration_var)
{
    this->txDuration_var = txDuration_var;
}

int AppContext::getChunkSize() const
{
    return chunkSize_var;
}

void AppContext::setChunkSize(int chunkSize_var)
{
    this->chunkSize_var = chunkSize_var;
}

double AppContext::getCodecRate() const
{
    return codecRate_var;
}

void AppContext::setCodecRate(double codecRate_var)
{
    this->codecRate_var = codecRate_var;
}

bool AppContext::getInitialPlayStarted() const
{
    return initialPlayStarted_var;
}

void AppContext::setInitialPlayStarted(bool initialPlayStarted_var)
{
    this->initialPlayStarted_var = initialPlayStarted_var;
}

int AppContext::getUserClass() const
{
    return userClass_var;
}

void AppContext::setUserClass(int userClass_var)
{
    this->userClass_var = userClass_var;
}

class AppContextDescriptor : public cClassDescriptor
{
  public:
    AppContextDescriptor();
    virtual ~AppContextDescriptor();

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

Register_ClassDescriptor(AppContextDescriptor);

AppContextDescriptor::AppContextDescriptor() : cClassDescriptor("AppContext", "cPacket")
{
}

AppContextDescriptor::~AppContextDescriptor()
{
}

bool AppContextDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<AppContext *>(obj)!=NULL;
}

const char *AppContextDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int AppContextDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 12+basedesc->getFieldCount(object) : 12;
}

unsigned int AppContextDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<12) ? fieldTypeFlags[field] : 0;
}

const char *AppContextDescriptor::getFieldName(void *object, int field) const
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
        "fileSize",
        "remainingFileSize",
        "cxStatus",
        "playBufferSize",
        "txDuration",
        "chunkSize",
        "codecRate",
        "initialPlayStarted",
        "userClass",
    };
    return (field>=0 && field<12) ? fieldNames[field] : NULL;
}

int AppContextDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='a' && strcmp(fieldName, "appIndex")==0) return base+0;
    if (fieldName[0]=='a' && strcmp(fieldName, "appId")==0) return base+1;
    if (fieldName[0]=='p' && strcmp(fieldName, "portId")==0) return base+2;
    if (fieldName[0]=='f' && strcmp(fieldName, "fileSize")==0) return base+3;
    if (fieldName[0]=='r' && strcmp(fieldName, "remainingFileSize")==0) return base+4;
    if (fieldName[0]=='c' && strcmp(fieldName, "cxStatus")==0) return base+5;
    if (fieldName[0]=='p' && strcmp(fieldName, "playBufferSize")==0) return base+6;
    if (fieldName[0]=='t' && strcmp(fieldName, "txDuration")==0) return base+7;
    if (fieldName[0]=='c' && strcmp(fieldName, "chunkSize")==0) return base+8;
    if (fieldName[0]=='c' && strcmp(fieldName, "codecRate")==0) return base+9;
    if (fieldName[0]=='i' && strcmp(fieldName, "initialPlayStarted")==0) return base+10;
    if (fieldName[0]=='u' && strcmp(fieldName, "userClass")==0) return base+11;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *AppContextDescriptor::getFieldTypeString(void *object, int field) const
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
        "int",
        "int",
        "bool",
        "int",
        "double",
        "int",
        "double",
        "bool",
        "int",
    };
    return (field>=0 && field<12) ? fieldTypeStrings[field] : NULL;
}

const char *AppContextDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int AppContextDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    AppContext *pp = (AppContext *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string AppContextDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    AppContext *pp = (AppContext *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getAppIndex());
        case 1: return long2string(pp->getAppId());
        case 2: return long2string(pp->getPortId());
        case 3: return long2string(pp->getFileSize());
        case 4: return long2string(pp->getRemainingFileSize());
        case 5: return bool2string(pp->getCxStatus());
        case 6: return long2string(pp->getPlayBufferSize());
        case 7: return double2string(pp->getTxDuration());
        case 8: return long2string(pp->getChunkSize());
        case 9: return double2string(pp->getCodecRate());
        case 10: return bool2string(pp->getInitialPlayStarted());
        case 11: return long2string(pp->getUserClass());
        default: return "";
    }
}

bool AppContextDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    AppContext *pp = (AppContext *)object; (void)pp;
    switch (field) {
        case 0: pp->setAppIndex(string2long(value)); return true;
        case 1: pp->setAppId(string2long(value)); return true;
        case 2: pp->setPortId(string2long(value)); return true;
        case 3: pp->setFileSize(string2long(value)); return true;
        case 4: pp->setRemainingFileSize(string2long(value)); return true;
        case 5: pp->setCxStatus(string2bool(value)); return true;
        case 6: pp->setPlayBufferSize(string2long(value)); return true;
        case 7: pp->setTxDuration(string2double(value)); return true;
        case 8: pp->setChunkSize(string2long(value)); return true;
        case 9: pp->setCodecRate(string2double(value)); return true;
        case 10: pp->setInitialPlayStarted(string2bool(value)); return true;
        case 11: pp->setUserClass(string2long(value)); return true;
        default: return false;
    }
}

const char *AppContextDescriptor::getFieldStructName(void *object, int field) const
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
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<12) ? fieldStructNames[field] : NULL;
}

void *AppContextDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    AppContext *pp = (AppContext *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}


