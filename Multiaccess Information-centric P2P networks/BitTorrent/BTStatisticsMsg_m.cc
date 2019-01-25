//
// Generated file, do not edit! Created by opp_msgc 3.3 from BTStatisticsMsg.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "BTStatisticsMsg_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw new cException("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}
template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw new cException("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

// Automatically supply array (un)packing functions
template<typename T>
void doPacking(cCommBuffer *b, T *t, int n) {
    for (int i=0; i<n; i++)
        doPacking(b,t[i]);
}
template<typename T>
void doUnpacking(cCommBuffer *b, T *t, int n) {
    for (int i=0; i<n; i++)
        doUnpacking(b,t[i]);
}
inline void doPacking(cCommBuffer *, cPolymorphic&) {}
inline void doUnpacking(cCommBuffer *, cPolymorphic&) {}

#define DOPACKING(T,R) \
    inline void doPacking(cCommBuffer *b, T R a) {b->pack(a);}  \
    inline void doPacking(cCommBuffer *b, T *a, int n) {b->pack(a,n);}  \
    inline void doUnpacking(cCommBuffer *b, T& a) {b->unpack(a);}  \
    inline void doUnpacking(cCommBuffer *b, T *a, int n) {b->unpack(a,n);}
#define _
DOPACKING(char,_)
DOPACKING(unsigned char,_)
DOPACKING(bool,_)
DOPACKING(short,_)
DOPACKING(unsigned short,_)
DOPACKING(int,_)
DOPACKING(unsigned int,_)
DOPACKING(long,_)
DOPACKING(unsigned long,_)
DOPACKING(float,_)
DOPACKING(double,_)
DOPACKING(long double,_)
DOPACKING(char *,_)
DOPACKING(const char *,_)
DOPACKING(opp_string,&)
//DOPACKING(std::string,&)
#undef _
#undef DOPACKING


Register_Class(BTStatisticsDWLMsg);

BTStatisticsDWLMsg::BTStatisticsDWLMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    this->downloadTime_var = 0;
    this->remainingBlocks_var = 0;
}

BTStatisticsDWLMsg::BTStatisticsDWLMsg(const BTStatisticsDWLMsg& other) : GenericAppMsg()
{
    unsigned int i;
    setName(other.name());
    operator=(other);
}

BTStatisticsDWLMsg::~BTStatisticsDWLMsg()
{
    unsigned int i;
}

BTStatisticsDWLMsg& BTStatisticsDWLMsg::operator=(const BTStatisticsDWLMsg& other)
{
    if (this==&other) return *this;
    unsigned int i;
    GenericAppMsg::operator=(other);
    this->downloadTime_var = other.downloadTime_var;
    this->remainingBlocks_var = other.remainingBlocks_var;
    return *this;
}

void BTStatisticsDWLMsg::netPack(cCommBuffer *b)
{
    GenericAppMsg::netPack(b);
    doPacking(b,this->downloadTime_var);
    doPacking(b,this->remainingBlocks_var);
}

void BTStatisticsDWLMsg::netUnpack(cCommBuffer *b)
{
    GenericAppMsg::netUnpack(b);
    doUnpacking(b,this->downloadTime_var);
    doUnpacking(b,this->remainingBlocks_var);
}

double BTStatisticsDWLMsg::downloadTime() const
{
    return downloadTime_var;
}

void BTStatisticsDWLMsg::setDownloadTime(double downloadTime_var)
{
    this->downloadTime_var = downloadTime_var;
}

double BTStatisticsDWLMsg::remainingBlocks() const
{
    return remainingBlocks_var;
}

void BTStatisticsDWLMsg::setRemainingBlocks(double remainingBlocks_var)
{
    this->remainingBlocks_var = remainingBlocks_var;
}

class BTStatisticsDWLMsgDescriptor : public cStructDescriptor
{
  public:
    BTStatisticsDWLMsgDescriptor();
    virtual ~BTStatisticsDWLMsgDescriptor();
    BTStatisticsDWLMsgDescriptor& operator=(const BTStatisticsDWLMsgDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTStatisticsDWLMsgDescriptor(*this);}

    virtual int getFieldCount();
    virtual const char *getFieldName(int field);
    virtual int getFieldType(int field);
    virtual const char *getFieldTypeString(int field);
    virtual const char *getFieldEnumName(int field);
    virtual int getArraySize(int field);

    virtual bool getFieldAsString(int field, int i, char *resultbuf, int bufsize);
    virtual bool setFieldAsString(int field, int i, const char *value);

    virtual const char *getFieldStructName(int field);
    virtual void *getFieldStructPointer(int field, int i);
    virtual sFieldWrapper *getFieldWrapper(int field, int i);
};

Register_Class(BTStatisticsDWLMsgDescriptor);

BTStatisticsDWLMsgDescriptor::BTStatisticsDWLMsgDescriptor() : cStructDescriptor("GenericAppMsg")
{
}

BTStatisticsDWLMsgDescriptor::~BTStatisticsDWLMsgDescriptor()
{
}

int BTStatisticsDWLMsgDescriptor::getFieldCount()
{
    return baseclassdesc ? 2+baseclassdesc->getFieldCount() : 2;
}

int BTStatisticsDWLMsgDescriptor::getFieldType(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldType(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return FT_BASIC;
        case 1: return FT_BASIC;
        default: return FT_INVALID;
    }
}

const char *BTStatisticsDWLMsgDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "downloadTime";
        case 1: return "remainingBlocks";
        default: return NULL;
    }
}

const char *BTStatisticsDWLMsgDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "double";
        case 1: return "double";
        default: return NULL;
    }
}

const char *BTStatisticsDWLMsgDescriptor::getFieldEnumName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldEnumName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        default: return NULL;
    }
}

int BTStatisticsDWLMsgDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsDWLMsg *pp = (BTStatisticsDWLMsg *)p;
    switch (field) {
        default: return 0;
    }
}

bool BTStatisticsDWLMsgDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsDWLMsg *pp = (BTStatisticsDWLMsg *)p;
    switch (field) {
        case 0: double2string(pp->downloadTime(),resultbuf,bufsize); return true;
        case 1: double2string(pp->remainingBlocks(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTStatisticsDWLMsgDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsDWLMsg *pp = (BTStatisticsDWLMsg *)p;
    switch (field) {
        case 0: pp->setDownloadTime(string2double(value)); return true;
        case 1: pp->setRemainingBlocks(string2double(value)); return true;
        default: return false;
    }
}

const char *BTStatisticsDWLMsgDescriptor::getFieldStructName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        default: return NULL;
    }
}

void *BTStatisticsDWLMsgDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsDWLMsg *pp = (BTStatisticsDWLMsg *)p;
    switch (field) {
        default: return NULL;
    }
}

sFieldWrapper *BTStatisticsDWLMsgDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

Register_Class(BTStatisticsStartupTimeMsg);

BTStatisticsStartupTimeMsg::BTStatisticsStartupTimeMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    this->startupTime_var = 0;
}

BTStatisticsStartupTimeMsg::BTStatisticsStartupTimeMsg(const BTStatisticsStartupTimeMsg& other) : GenericAppMsg()
{
    unsigned int i;
    setName(other.name());
    operator=(other);
}

BTStatisticsStartupTimeMsg::~BTStatisticsStartupTimeMsg()
{
    unsigned int i;
}

BTStatisticsStartupTimeMsg& BTStatisticsStartupTimeMsg::operator=(const BTStatisticsStartupTimeMsg& other)
{
    if (this==&other) return *this;
    unsigned int i;
    GenericAppMsg::operator=(other);
    this->startupTime_var = other.startupTime_var;
    return *this;
}

void BTStatisticsStartupTimeMsg::netPack(cCommBuffer *b)
{
    GenericAppMsg::netPack(b);
    doPacking(b,this->startupTime_var);
}

void BTStatisticsStartupTimeMsg::netUnpack(cCommBuffer *b)
{
    GenericAppMsg::netUnpack(b);
    doUnpacking(b,this->startupTime_var);
}

double BTStatisticsStartupTimeMsg::startupTime() const
{
    return startupTime_var;
}

void BTStatisticsStartupTimeMsg::setStartupTime(double startupTime_var)
{
    this->startupTime_var = startupTime_var;
}

class BTStatisticsStartupTimeMsgDescriptor : public cStructDescriptor
{
  public:
    BTStatisticsStartupTimeMsgDescriptor();
    virtual ~BTStatisticsStartupTimeMsgDescriptor();
    BTStatisticsStartupTimeMsgDescriptor& operator=(const BTStatisticsStartupTimeMsgDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTStatisticsStartupTimeMsgDescriptor(*this);}

    virtual int getFieldCount();
    virtual const char *getFieldName(int field);
    virtual int getFieldType(int field);
    virtual const char *getFieldTypeString(int field);
    virtual const char *getFieldEnumName(int field);
    virtual int getArraySize(int field);

    virtual bool getFieldAsString(int field, int i, char *resultbuf, int bufsize);
    virtual bool setFieldAsString(int field, int i, const char *value);

    virtual const char *getFieldStructName(int field);
    virtual void *getFieldStructPointer(int field, int i);
    virtual sFieldWrapper *getFieldWrapper(int field, int i);
};

Register_Class(BTStatisticsStartupTimeMsgDescriptor);

BTStatisticsStartupTimeMsgDescriptor::BTStatisticsStartupTimeMsgDescriptor() : cStructDescriptor("GenericAppMsg")
{
}

BTStatisticsStartupTimeMsgDescriptor::~BTStatisticsStartupTimeMsgDescriptor()
{
}

int BTStatisticsStartupTimeMsgDescriptor::getFieldCount()
{
    return baseclassdesc ? 1+baseclassdesc->getFieldCount() : 1;
}

int BTStatisticsStartupTimeMsgDescriptor::getFieldType(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldType(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return FT_BASIC;
        default: return FT_INVALID;
    }
}

const char *BTStatisticsStartupTimeMsgDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "startupTime";
        default: return NULL;
    }
}

const char *BTStatisticsStartupTimeMsgDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "double";
        default: return NULL;
    }
}

const char *BTStatisticsStartupTimeMsgDescriptor::getFieldEnumName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldEnumName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        default: return NULL;
    }
}

int BTStatisticsStartupTimeMsgDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsStartupTimeMsg *pp = (BTStatisticsStartupTimeMsg *)p;
    switch (field) {
        default: return 0;
    }
}

bool BTStatisticsStartupTimeMsgDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsStartupTimeMsg *pp = (BTStatisticsStartupTimeMsg *)p;
    switch (field) {
        case 0: double2string(pp->startupTime(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTStatisticsStartupTimeMsgDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsStartupTimeMsg *pp = (BTStatisticsStartupTimeMsg *)p;
    switch (field) {
        case 0: pp->setStartupTime(string2double(value)); return true;
        default: return false;
    }
}

const char *BTStatisticsStartupTimeMsgDescriptor::getFieldStructName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        default: return NULL;
    }
}

void *BTStatisticsStartupTimeMsgDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsStartupTimeMsg *pp = (BTStatisticsStartupTimeMsg *)p;
    switch (field) {
        default: return NULL;
    }
}

sFieldWrapper *BTStatisticsStartupTimeMsgDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

Register_Class(BTStatisticsNumProvidersMsg);

BTStatisticsNumProvidersMsg::BTStatisticsNumProvidersMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    this->numPeers_var = 0;
}

BTStatisticsNumProvidersMsg::BTStatisticsNumProvidersMsg(const BTStatisticsNumProvidersMsg& other) : GenericAppMsg()
{
    unsigned int i;
    setName(other.name());
    operator=(other);
}

BTStatisticsNumProvidersMsg::~BTStatisticsNumProvidersMsg()
{
    unsigned int i;
}

BTStatisticsNumProvidersMsg& BTStatisticsNumProvidersMsg::operator=(const BTStatisticsNumProvidersMsg& other)
{
    if (this==&other) return *this;
    unsigned int i;
    GenericAppMsg::operator=(other);
    this->numPeers_var = other.numPeers_var;
    return *this;
}

void BTStatisticsNumProvidersMsg::netPack(cCommBuffer *b)
{
    GenericAppMsg::netPack(b);
    doPacking(b,this->numPeers_var);
}

void BTStatisticsNumProvidersMsg::netUnpack(cCommBuffer *b)
{
    GenericAppMsg::netUnpack(b);
    doUnpacking(b,this->numPeers_var);
}

int BTStatisticsNumProvidersMsg::numPeers() const
{
    return numPeers_var;
}

void BTStatisticsNumProvidersMsg::setNumPeers(int numPeers_var)
{
    this->numPeers_var = numPeers_var;
}

class BTStatisticsNumProvidersMsgDescriptor : public cStructDescriptor
{
  public:
    BTStatisticsNumProvidersMsgDescriptor();
    virtual ~BTStatisticsNumProvidersMsgDescriptor();
    BTStatisticsNumProvidersMsgDescriptor& operator=(const BTStatisticsNumProvidersMsgDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTStatisticsNumProvidersMsgDescriptor(*this);}

    virtual int getFieldCount();
    virtual const char *getFieldName(int field);
    virtual int getFieldType(int field);
    virtual const char *getFieldTypeString(int field);
    virtual const char *getFieldEnumName(int field);
    virtual int getArraySize(int field);

    virtual bool getFieldAsString(int field, int i, char *resultbuf, int bufsize);
    virtual bool setFieldAsString(int field, int i, const char *value);

    virtual const char *getFieldStructName(int field);
    virtual void *getFieldStructPointer(int field, int i);
    virtual sFieldWrapper *getFieldWrapper(int field, int i);
};

Register_Class(BTStatisticsNumProvidersMsgDescriptor);

BTStatisticsNumProvidersMsgDescriptor::BTStatisticsNumProvidersMsgDescriptor() : cStructDescriptor("GenericAppMsg")
{
}

BTStatisticsNumProvidersMsgDescriptor::~BTStatisticsNumProvidersMsgDescriptor()
{
}

int BTStatisticsNumProvidersMsgDescriptor::getFieldCount()
{
    return baseclassdesc ? 1+baseclassdesc->getFieldCount() : 1;
}

int BTStatisticsNumProvidersMsgDescriptor::getFieldType(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldType(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return FT_BASIC;
        default: return FT_INVALID;
    }
}

const char *BTStatisticsNumProvidersMsgDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "numPeers";
        default: return NULL;
    }
}

const char *BTStatisticsNumProvidersMsgDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "int";
        default: return NULL;
    }
}

const char *BTStatisticsNumProvidersMsgDescriptor::getFieldEnumName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldEnumName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        default: return NULL;
    }
}

int BTStatisticsNumProvidersMsgDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsNumProvidersMsg *pp = (BTStatisticsNumProvidersMsg *)p;
    switch (field) {
        default: return 0;
    }
}

bool BTStatisticsNumProvidersMsgDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsNumProvidersMsg *pp = (BTStatisticsNumProvidersMsg *)p;
    switch (field) {
        case 0: long2string(pp->numPeers(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTStatisticsNumProvidersMsgDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsNumProvidersMsg *pp = (BTStatisticsNumProvidersMsg *)p;
    switch (field) {
        case 0: pp->setNumPeers(string2long(value)); return true;
        default: return false;
    }
}

const char *BTStatisticsNumProvidersMsgDescriptor::getFieldStructName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        default: return NULL;
    }
}

void *BTStatisticsNumProvidersMsgDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsNumProvidersMsg *pp = (BTStatisticsNumProvidersMsg *)p;
    switch (field) {
        default: return NULL;
    }
}

sFieldWrapper *BTStatisticsNumProvidersMsgDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

Register_Class(BTStatisticsNumSeederBlocksMsg);

BTStatisticsNumSeederBlocksMsg::BTStatisticsNumSeederBlocksMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    this->numSeederBlocks_var = 0;
}

BTStatisticsNumSeederBlocksMsg::BTStatisticsNumSeederBlocksMsg(const BTStatisticsNumSeederBlocksMsg& other) : GenericAppMsg()
{
    unsigned int i;
    setName(other.name());
    operator=(other);
}

BTStatisticsNumSeederBlocksMsg::~BTStatisticsNumSeederBlocksMsg()
{
    unsigned int i;
}

BTStatisticsNumSeederBlocksMsg& BTStatisticsNumSeederBlocksMsg::operator=(const BTStatisticsNumSeederBlocksMsg& other)
{
    if (this==&other) return *this;
    unsigned int i;
    GenericAppMsg::operator=(other);
    this->numSeederBlocks_var = other.numSeederBlocks_var;
    return *this;
}

void BTStatisticsNumSeederBlocksMsg::netPack(cCommBuffer *b)
{
    GenericAppMsg::netPack(b);
    doPacking(b,this->numSeederBlocks_var);
}

void BTStatisticsNumSeederBlocksMsg::netUnpack(cCommBuffer *b)
{
    GenericAppMsg::netUnpack(b);
    doUnpacking(b,this->numSeederBlocks_var);
}

double BTStatisticsNumSeederBlocksMsg::numSeederBlocks() const
{
    return numSeederBlocks_var;
}

void BTStatisticsNumSeederBlocksMsg::setNumSeederBlocks(double numSeederBlocks_var)
{
    this->numSeederBlocks_var = numSeederBlocks_var;
}

class BTStatisticsNumSeederBlocksMsgDescriptor : public cStructDescriptor
{
  public:
    BTStatisticsNumSeederBlocksMsgDescriptor();
    virtual ~BTStatisticsNumSeederBlocksMsgDescriptor();
    BTStatisticsNumSeederBlocksMsgDescriptor& operator=(const BTStatisticsNumSeederBlocksMsgDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTStatisticsNumSeederBlocksMsgDescriptor(*this);}

    virtual int getFieldCount();
    virtual const char *getFieldName(int field);
    virtual int getFieldType(int field);
    virtual const char *getFieldTypeString(int field);
    virtual const char *getFieldEnumName(int field);
    virtual int getArraySize(int field);

    virtual bool getFieldAsString(int field, int i, char *resultbuf, int bufsize);
    virtual bool setFieldAsString(int field, int i, const char *value);

    virtual const char *getFieldStructName(int field);
    virtual void *getFieldStructPointer(int field, int i);
    virtual sFieldWrapper *getFieldWrapper(int field, int i);
};

Register_Class(BTStatisticsNumSeederBlocksMsgDescriptor);

BTStatisticsNumSeederBlocksMsgDescriptor::BTStatisticsNumSeederBlocksMsgDescriptor() : cStructDescriptor("GenericAppMsg")
{
}

BTStatisticsNumSeederBlocksMsgDescriptor::~BTStatisticsNumSeederBlocksMsgDescriptor()
{
}

int BTStatisticsNumSeederBlocksMsgDescriptor::getFieldCount()
{
    return baseclassdesc ? 1+baseclassdesc->getFieldCount() : 1;
}

int BTStatisticsNumSeederBlocksMsgDescriptor::getFieldType(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldType(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return FT_BASIC;
        default: return FT_INVALID;
    }
}

const char *BTStatisticsNumSeederBlocksMsgDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "numSeederBlocks";
        default: return NULL;
    }
}

const char *BTStatisticsNumSeederBlocksMsgDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "double";
        default: return NULL;
    }
}

const char *BTStatisticsNumSeederBlocksMsgDescriptor::getFieldEnumName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldEnumName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        default: return NULL;
    }
}

int BTStatisticsNumSeederBlocksMsgDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsNumSeederBlocksMsg *pp = (BTStatisticsNumSeederBlocksMsg *)p;
    switch (field) {
        default: return 0;
    }
}

bool BTStatisticsNumSeederBlocksMsgDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsNumSeederBlocksMsg *pp = (BTStatisticsNumSeederBlocksMsg *)p;
    switch (field) {
        case 0: double2string(pp->numSeederBlocks(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTStatisticsNumSeederBlocksMsgDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsNumSeederBlocksMsg *pp = (BTStatisticsNumSeederBlocksMsg *)p;
    switch (field) {
        case 0: pp->setNumSeederBlocks(string2double(value)); return true;
        default: return false;
    }
}

const char *BTStatisticsNumSeederBlocksMsgDescriptor::getFieldStructName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        default: return NULL;
    }
}

void *BTStatisticsNumSeederBlocksMsgDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsNumSeederBlocksMsg *pp = (BTStatisticsNumSeederBlocksMsg *)p;
    switch (field) {
        default: return NULL;
    }
}

sFieldWrapper *BTStatisticsNumSeederBlocksMsgDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

Register_Class(BTStatisticsDownloadRateMsg);

BTStatisticsDownloadRateMsg::BTStatisticsDownloadRateMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    this->downloadRate_var = 0;
}

BTStatisticsDownloadRateMsg::BTStatisticsDownloadRateMsg(const BTStatisticsDownloadRateMsg& other) : GenericAppMsg()
{
    unsigned int i;
    setName(other.name());
    operator=(other);
}

BTStatisticsDownloadRateMsg::~BTStatisticsDownloadRateMsg()
{
    unsigned int i;
}

BTStatisticsDownloadRateMsg& BTStatisticsDownloadRateMsg::operator=(const BTStatisticsDownloadRateMsg& other)
{
    if (this==&other) return *this;
    unsigned int i;
    GenericAppMsg::operator=(other);
    this->downloadRate_var = other.downloadRate_var;
    return *this;
}

void BTStatisticsDownloadRateMsg::netPack(cCommBuffer *b)
{
    GenericAppMsg::netPack(b);
    doPacking(b,this->downloadRate_var);
}

void BTStatisticsDownloadRateMsg::netUnpack(cCommBuffer *b)
{
    GenericAppMsg::netUnpack(b);
    doUnpacking(b,this->downloadRate_var);
}

double BTStatisticsDownloadRateMsg::downloadRate() const
{
    return downloadRate_var;
}

void BTStatisticsDownloadRateMsg::setDownloadRate(double downloadRate_var)
{
    this->downloadRate_var = downloadRate_var;
}

class BTStatisticsDownloadRateMsgDescriptor : public cStructDescriptor
{
  public:
    BTStatisticsDownloadRateMsgDescriptor();
    virtual ~BTStatisticsDownloadRateMsgDescriptor();
    BTStatisticsDownloadRateMsgDescriptor& operator=(const BTStatisticsDownloadRateMsgDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTStatisticsDownloadRateMsgDescriptor(*this);}

    virtual int getFieldCount();
    virtual const char *getFieldName(int field);
    virtual int getFieldType(int field);
    virtual const char *getFieldTypeString(int field);
    virtual const char *getFieldEnumName(int field);
    virtual int getArraySize(int field);

    virtual bool getFieldAsString(int field, int i, char *resultbuf, int bufsize);
    virtual bool setFieldAsString(int field, int i, const char *value);

    virtual const char *getFieldStructName(int field);
    virtual void *getFieldStructPointer(int field, int i);
    virtual sFieldWrapper *getFieldWrapper(int field, int i);
};

Register_Class(BTStatisticsDownloadRateMsgDescriptor);

BTStatisticsDownloadRateMsgDescriptor::BTStatisticsDownloadRateMsgDescriptor() : cStructDescriptor("GenericAppMsg")
{
}

BTStatisticsDownloadRateMsgDescriptor::~BTStatisticsDownloadRateMsgDescriptor()
{
}

int BTStatisticsDownloadRateMsgDescriptor::getFieldCount()
{
    return baseclassdesc ? 1+baseclassdesc->getFieldCount() : 1;
}

int BTStatisticsDownloadRateMsgDescriptor::getFieldType(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldType(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return FT_BASIC;
        default: return FT_INVALID;
    }
}

const char *BTStatisticsDownloadRateMsgDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "downloadRate";
        default: return NULL;
    }
}

const char *BTStatisticsDownloadRateMsgDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "double";
        default: return NULL;
    }
}

const char *BTStatisticsDownloadRateMsgDescriptor::getFieldEnumName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldEnumName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        default: return NULL;
    }
}

int BTStatisticsDownloadRateMsgDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsDownloadRateMsg *pp = (BTStatisticsDownloadRateMsg *)p;
    switch (field) {
        default: return 0;
    }
}

bool BTStatisticsDownloadRateMsgDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsDownloadRateMsg *pp = (BTStatisticsDownloadRateMsg *)p;
    switch (field) {
        case 0: double2string(pp->downloadRate(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTStatisticsDownloadRateMsgDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsDownloadRateMsg *pp = (BTStatisticsDownloadRateMsg *)p;
    switch (field) {
        case 0: pp->setDownloadRate(string2double(value)); return true;
        default: return false;
    }
}

const char *BTStatisticsDownloadRateMsgDescriptor::getFieldStructName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        default: return NULL;
    }
}

void *BTStatisticsDownloadRateMsgDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsDownloadRateMsg *pp = (BTStatisticsDownloadRateMsg *)p;
    switch (field) {
        default: return NULL;
    }
}

sFieldWrapper *BTStatisticsDownloadRateMsgDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

Register_Class(BTStatisticsUploadRateMsg);

BTStatisticsUploadRateMsg::BTStatisticsUploadRateMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    this->uploadRate_var = 0;
}

BTStatisticsUploadRateMsg::BTStatisticsUploadRateMsg(const BTStatisticsUploadRateMsg& other) : GenericAppMsg()
{
    unsigned int i;
    setName(other.name());
    operator=(other);
}

BTStatisticsUploadRateMsg::~BTStatisticsUploadRateMsg()
{
    unsigned int i;
}

BTStatisticsUploadRateMsg& BTStatisticsUploadRateMsg::operator=(const BTStatisticsUploadRateMsg& other)
{
    if (this==&other) return *this;
    unsigned int i;
    GenericAppMsg::operator=(other);
    this->uploadRate_var = other.uploadRate_var;
    return *this;
}

void BTStatisticsUploadRateMsg::netPack(cCommBuffer *b)
{
    GenericAppMsg::netPack(b);
    doPacking(b,this->uploadRate_var);
}

void BTStatisticsUploadRateMsg::netUnpack(cCommBuffer *b)
{
    GenericAppMsg::netUnpack(b);
    doUnpacking(b,this->uploadRate_var);
}

double BTStatisticsUploadRateMsg::uploadRate() const
{
    return uploadRate_var;
}

void BTStatisticsUploadRateMsg::setUploadRate(double uploadRate_var)
{
    this->uploadRate_var = uploadRate_var;
}

class BTStatisticsUploadRateMsgDescriptor : public cStructDescriptor
{
  public:
    BTStatisticsUploadRateMsgDescriptor();
    virtual ~BTStatisticsUploadRateMsgDescriptor();
    BTStatisticsUploadRateMsgDescriptor& operator=(const BTStatisticsUploadRateMsgDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTStatisticsUploadRateMsgDescriptor(*this);}

    virtual int getFieldCount();
    virtual const char *getFieldName(int field);
    virtual int getFieldType(int field);
    virtual const char *getFieldTypeString(int field);
    virtual const char *getFieldEnumName(int field);
    virtual int getArraySize(int field);

    virtual bool getFieldAsString(int field, int i, char *resultbuf, int bufsize);
    virtual bool setFieldAsString(int field, int i, const char *value);

    virtual const char *getFieldStructName(int field);
    virtual void *getFieldStructPointer(int field, int i);
    virtual sFieldWrapper *getFieldWrapper(int field, int i);
};

Register_Class(BTStatisticsUploadRateMsgDescriptor);

BTStatisticsUploadRateMsgDescriptor::BTStatisticsUploadRateMsgDescriptor() : cStructDescriptor("GenericAppMsg")
{
}

BTStatisticsUploadRateMsgDescriptor::~BTStatisticsUploadRateMsgDescriptor()
{
}

int BTStatisticsUploadRateMsgDescriptor::getFieldCount()
{
    return baseclassdesc ? 1+baseclassdesc->getFieldCount() : 1;
}

int BTStatisticsUploadRateMsgDescriptor::getFieldType(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldType(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return FT_BASIC;
        default: return FT_INVALID;
    }
}

const char *BTStatisticsUploadRateMsgDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "uploadRate";
        default: return NULL;
    }
}

const char *BTStatisticsUploadRateMsgDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "double";
        default: return NULL;
    }
}

const char *BTStatisticsUploadRateMsgDescriptor::getFieldEnumName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldEnumName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        default: return NULL;
    }
}

int BTStatisticsUploadRateMsgDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsUploadRateMsg *pp = (BTStatisticsUploadRateMsg *)p;
    switch (field) {
        default: return 0;
    }
}

bool BTStatisticsUploadRateMsgDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsUploadRateMsg *pp = (BTStatisticsUploadRateMsg *)p;
    switch (field) {
        case 0: double2string(pp->uploadRate(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTStatisticsUploadRateMsgDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsUploadRateMsg *pp = (BTStatisticsUploadRateMsg *)p;
    switch (field) {
        case 0: pp->setUploadRate(string2double(value)); return true;
        default: return false;
    }
}

const char *BTStatisticsUploadRateMsgDescriptor::getFieldStructName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        default: return NULL;
    }
}

void *BTStatisticsUploadRateMsgDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTStatisticsUploadRateMsg *pp = (BTStatisticsUploadRateMsg *)p;
    switch (field) {
        default: return NULL;
    }
}

sFieldWrapper *BTStatisticsUploadRateMsgDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

