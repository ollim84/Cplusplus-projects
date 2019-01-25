//
// Generated file, do not edit! Created by opp_msgc 3.3 from BTPeerWireMsg.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "BTPeerWireMsg_m.h"

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


EXECUTE_ON_STARTUP(PeerWireMsgIDs,
    cEnum *e = findEnum("PeerWireMsgIDs");
    if (!e) enums.instance()->add(e = new cEnum("PeerWireMsgIDs"));
    e->insert(CHOKE, "CHOKE");
    e->insert(UNCHOKE, "UNCHOKE");
    e->insert(INTERESTED, "INTERESTED");
    e->insert(NOT_INTERESTED, "NOT_INTERESTED");
    e->insert(HAVE, "HAVE");
    e->insert(BITFIELD, "BITFIELD");
    e->insert(REQUEST, "REQUEST");
    e->insert(PIECE, "PIECE");
    e->insert(CANCEL, "CANCEL");
);

Register_Class(BTMsgHandshake);

BTMsgHandshake::BTMsgHandshake(const char *name, int kind) : GenericAppMsg(name,kind)
{
    unsigned int i;
    this->pstrlen_var = 0;
    this->pstr_var = "";
    for (i=0; i<8; i++)
        this->reserved_var[i] = false;
    this->infoHash_var = "";
    this->peerId_var = "";
}

BTMsgHandshake::BTMsgHandshake(const BTMsgHandshake& other) : GenericAppMsg()
{
    unsigned int i;
    setName(other.name());
    operator=(other);
}

BTMsgHandshake::~BTMsgHandshake()
{
    unsigned int i;
}

BTMsgHandshake& BTMsgHandshake::operator=(const BTMsgHandshake& other)
{
    if (this==&other) return *this;
    unsigned int i;
    GenericAppMsg::operator=(other);
    this->pstrlen_var = other.pstrlen_var;
    this->pstr_var = other.pstr_var;
    for (i=0; i<8; i++)
        this->reserved_var[i] = other.reserved_var[i];
    this->infoHash_var = other.infoHash_var;
    this->peerId_var = other.peerId_var;
    return *this;
}

void BTMsgHandshake::netPack(cCommBuffer *b)
{
    GenericAppMsg::netPack(b);
    doPacking(b,this->pstrlen_var);
    doPacking(b,this->pstr_var);
    doPacking(b,this->reserved_var,8);
    doPacking(b,this->infoHash_var);
    doPacking(b,this->peerId_var);
}

void BTMsgHandshake::netUnpack(cCommBuffer *b)
{
    GenericAppMsg::netUnpack(b);
    doUnpacking(b,this->pstrlen_var);
    doUnpacking(b,this->pstr_var);
    doUnpacking(b,this->reserved_var,8);
    doUnpacking(b,this->infoHash_var);
    doUnpacking(b,this->peerId_var);
}

char BTMsgHandshake::pstrlen() const
{
    return pstrlen_var;
}

void BTMsgHandshake::setPstrlen(char pstrlen_var)
{
    this->pstrlen_var = pstrlen_var;
}

const char * BTMsgHandshake::pstr() const
{
    return pstr_var.c_str();
}

void BTMsgHandshake::setPstr(const char * pstr_var)
{
    this->pstr_var = pstr_var;
}

unsigned int BTMsgHandshake::reservedArraySize() const
{
    return 8;
}

bool BTMsgHandshake::reserved(unsigned int k) const
{
    if (k>=8) throw new cException("Array of size 8 indexed by %d", k);
    return reserved_var[k];
}

void BTMsgHandshake::setReserved(unsigned int k, bool reserved_var)
{
    if (k>=8) throw new cException("Array of size 8 indexed by %d", k);
    this->reserved_var[k] = reserved_var;
}

const char * BTMsgHandshake::infoHash() const
{
    return infoHash_var.c_str();
}

void BTMsgHandshake::setInfoHash(const char * infoHash_var)
{
    this->infoHash_var = infoHash_var;
}

const char * BTMsgHandshake::peerId() const
{
    return peerId_var.c_str();
}

void BTMsgHandshake::setPeerId(const char * peerId_var)
{
    this->peerId_var = peerId_var;
}

class BTMsgHandshakeDescriptor : public cStructDescriptor
{
  public:
    BTMsgHandshakeDescriptor();
    virtual ~BTMsgHandshakeDescriptor();
    BTMsgHandshakeDescriptor& operator=(const BTMsgHandshakeDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTMsgHandshakeDescriptor(*this);}

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

Register_Class(BTMsgHandshakeDescriptor);

BTMsgHandshakeDescriptor::BTMsgHandshakeDescriptor() : cStructDescriptor("GenericAppMsg")
{
}

BTMsgHandshakeDescriptor::~BTMsgHandshakeDescriptor()
{
}

int BTMsgHandshakeDescriptor::getFieldCount()
{
    return baseclassdesc ? 5+baseclassdesc->getFieldCount() : 5;
}

int BTMsgHandshakeDescriptor::getFieldType(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldType(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return FT_BASIC;
        case 1: return FT_BASIC;
        case 2: return FT_BASIC_ARRAY;
        case 3: return FT_BASIC;
        case 4: return FT_BASIC;
        default: return FT_INVALID;
    }
}

const char *BTMsgHandshakeDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "pstrlen";
        case 1: return "pstr";
        case 2: return "reserved";
        case 3: return "infoHash";
        case 4: return "peerId";
        default: return NULL;
    }
}

const char *BTMsgHandshakeDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "char";
        case 1: return "string";
        case 2: return "bool";
        case 3: return "string";
        case 4: return "string";
        default: return NULL;
    }
}

const char *BTMsgHandshakeDescriptor::getFieldEnumName(int field)
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

int BTMsgHandshakeDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTMsgHandshake *pp = (BTMsgHandshake *)p;
    switch (field) {
        case 2: return 8;
        default: return 0;
    }
}

bool BTMsgHandshakeDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTMsgHandshake *pp = (BTMsgHandshake *)p;
    switch (field) {
        case 0: long2string(pp->pstrlen(),resultbuf,bufsize); return true;
        case 1: oppstring2string(pp->pstr(),resultbuf,bufsize); return true;
        case 2: bool2string(pp->reserved(i),resultbuf,bufsize); return true;
        case 3: oppstring2string(pp->infoHash(),resultbuf,bufsize); return true;
        case 4: oppstring2string(pp->peerId(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTMsgHandshakeDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTMsgHandshake *pp = (BTMsgHandshake *)p;
    switch (field) {
        case 0: pp->setPstrlen(string2long(value)); return true;
        case 1: pp->setPstr((value)); return true;
        case 2: pp->setReserved(i,string2bool(value)); return true;
        case 3: pp->setInfoHash((value)); return true;
        case 4: pp->setPeerId((value)); return true;
        default: return false;
    }
}

const char *BTMsgHandshakeDescriptor::getFieldStructName(int field)
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

void *BTMsgHandshakeDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTMsgHandshake *pp = (BTMsgHandshake *)p;
    switch (field) {
        default: return NULL;
    }
}

sFieldWrapper *BTMsgHandshakeDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

Register_Class(BTKeepAliveMsg);

BTKeepAliveMsg::BTKeepAliveMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    this->length_prefix_var = 0;
}

BTKeepAliveMsg::BTKeepAliveMsg(const BTKeepAliveMsg& other) : GenericAppMsg()
{
    unsigned int i;
    setName(other.name());
    operator=(other);
}

BTKeepAliveMsg::~BTKeepAliveMsg()
{
    unsigned int i;
}

BTKeepAliveMsg& BTKeepAliveMsg::operator=(const BTKeepAliveMsg& other)
{
    if (this==&other) return *this;
    unsigned int i;
    GenericAppMsg::operator=(other);
    this->length_prefix_var = other.length_prefix_var;
    return *this;
}

void BTKeepAliveMsg::netPack(cCommBuffer *b)
{
    GenericAppMsg::netPack(b);
    doPacking(b,this->length_prefix_var);
}

void BTKeepAliveMsg::netUnpack(cCommBuffer *b)
{
    GenericAppMsg::netUnpack(b);
    doUnpacking(b,this->length_prefix_var);
}

int BTKeepAliveMsg::length_prefix() const
{
    return length_prefix_var;
}

void BTKeepAliveMsg::setLength_prefix(int length_prefix_var)
{
    this->length_prefix_var = length_prefix_var;
}

class BTKeepAliveMsgDescriptor : public cStructDescriptor
{
  public:
    BTKeepAliveMsgDescriptor();
    virtual ~BTKeepAliveMsgDescriptor();
    BTKeepAliveMsgDescriptor& operator=(const BTKeepAliveMsgDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTKeepAliveMsgDescriptor(*this);}

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

Register_Class(BTKeepAliveMsgDescriptor);

BTKeepAliveMsgDescriptor::BTKeepAliveMsgDescriptor() : cStructDescriptor("GenericAppMsg")
{
}

BTKeepAliveMsgDescriptor::~BTKeepAliveMsgDescriptor()
{
}

int BTKeepAliveMsgDescriptor::getFieldCount()
{
    return baseclassdesc ? 1+baseclassdesc->getFieldCount() : 1;
}

int BTKeepAliveMsgDescriptor::getFieldType(int field)
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

const char *BTKeepAliveMsgDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "length_prefix";
        default: return NULL;
    }
}

const char *BTKeepAliveMsgDescriptor::getFieldTypeString(int field)
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

const char *BTKeepAliveMsgDescriptor::getFieldEnumName(int field)
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

int BTKeepAliveMsgDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTKeepAliveMsg *pp = (BTKeepAliveMsg *)p;
    switch (field) {
        default: return 0;
    }
}

bool BTKeepAliveMsgDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTKeepAliveMsg *pp = (BTKeepAliveMsg *)p;
    switch (field) {
        case 0: long2string(pp->length_prefix(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTKeepAliveMsgDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTKeepAliveMsg *pp = (BTKeepAliveMsg *)p;
    switch (field) {
        case 0: pp->setLength_prefix(string2long(value)); return true;
        default: return false;
    }
}

const char *BTKeepAliveMsgDescriptor::getFieldStructName(int field)
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

void *BTKeepAliveMsgDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTKeepAliveMsg *pp = (BTKeepAliveMsg *)p;
    switch (field) {
        default: return NULL;
    }
}

sFieldWrapper *BTKeepAliveMsgDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

Register_Class(BTPeerStateMsg);

BTPeerStateMsg::BTPeerStateMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    this->length_prefix_var = 0;
    this->ID_var = 0;
}

BTPeerStateMsg::BTPeerStateMsg(const BTPeerStateMsg& other) : GenericAppMsg()
{
    unsigned int i;
    setName(other.name());
    operator=(other);
}

BTPeerStateMsg::~BTPeerStateMsg()
{
    unsigned int i;
}

BTPeerStateMsg& BTPeerStateMsg::operator=(const BTPeerStateMsg& other)
{
    if (this==&other) return *this;
    unsigned int i;
    GenericAppMsg::operator=(other);
    this->length_prefix_var = other.length_prefix_var;
    this->ID_var = other.ID_var;
    return *this;
}

void BTPeerStateMsg::netPack(cCommBuffer *b)
{
    GenericAppMsg::netPack(b);
    doPacking(b,this->length_prefix_var);
    doPacking(b,this->ID_var);
}

void BTPeerStateMsg::netUnpack(cCommBuffer *b)
{
    GenericAppMsg::netUnpack(b);
    doUnpacking(b,this->length_prefix_var);
    doUnpacking(b,this->ID_var);
}

int BTPeerStateMsg::length_prefix() const
{
    return length_prefix_var;
}

void BTPeerStateMsg::setLength_prefix(int length_prefix_var)
{
    this->length_prefix_var = length_prefix_var;
}

unsigned short BTPeerStateMsg::ID() const
{
    return ID_var;
}

void BTPeerStateMsg::setID(unsigned short ID_var)
{
    this->ID_var = ID_var;
}

class BTPeerStateMsgDescriptor : public cStructDescriptor
{
  public:
    BTPeerStateMsgDescriptor();
    virtual ~BTPeerStateMsgDescriptor();
    BTPeerStateMsgDescriptor& operator=(const BTPeerStateMsgDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTPeerStateMsgDescriptor(*this);}

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

Register_Class(BTPeerStateMsgDescriptor);

BTPeerStateMsgDescriptor::BTPeerStateMsgDescriptor() : cStructDescriptor("GenericAppMsg")
{
}

BTPeerStateMsgDescriptor::~BTPeerStateMsgDescriptor()
{
}

int BTPeerStateMsgDescriptor::getFieldCount()
{
    return baseclassdesc ? 2+baseclassdesc->getFieldCount() : 2;
}

int BTPeerStateMsgDescriptor::getFieldType(int field)
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

const char *BTPeerStateMsgDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "length_prefix";
        case 1: return "ID";
        default: return NULL;
    }
}

const char *BTPeerStateMsgDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "int";
        case 1: return "unsigned short";
        default: return NULL;
    }
}

const char *BTPeerStateMsgDescriptor::getFieldEnumName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldEnumName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 1: return "PeerWireMsgIDs";
        default: return NULL;
    }
}

int BTPeerStateMsgDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTPeerStateMsg *pp = (BTPeerStateMsg *)p;
    switch (field) {
        default: return 0;
    }
}

bool BTPeerStateMsgDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTPeerStateMsg *pp = (BTPeerStateMsg *)p;
    switch (field) {
        case 0: long2string(pp->length_prefix(),resultbuf,bufsize); return true;
        case 1: long2string(pp->ID(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTPeerStateMsgDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTPeerStateMsg *pp = (BTPeerStateMsg *)p;
    switch (field) {
        case 0: pp->setLength_prefix(string2long(value)); return true;
        case 1: pp->setID(string2long(value)); return true;
        default: return false;
    }
}

const char *BTPeerStateMsgDescriptor::getFieldStructName(int field)
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

void *BTPeerStateMsgDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTPeerStateMsg *pp = (BTPeerStateMsg *)p;
    switch (field) {
        default: return NULL;
    }
}

sFieldWrapper *BTPeerStateMsgDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

Register_Class(BTHaveMsg);

BTHaveMsg::BTHaveMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    this->length_prefix_var = 0;
    this->ID_var = 0;
    this->index_var = 0;
}

BTHaveMsg::BTHaveMsg(const BTHaveMsg& other) : GenericAppMsg()
{
    unsigned int i;
    setName(other.name());
    operator=(other);
}

BTHaveMsg::~BTHaveMsg()
{
    unsigned int i;
}

BTHaveMsg& BTHaveMsg::operator=(const BTHaveMsg& other)
{
    if (this==&other) return *this;
    unsigned int i;
    GenericAppMsg::operator=(other);
    this->length_prefix_var = other.length_prefix_var;
    this->ID_var = other.ID_var;
    this->index_var = other.index_var;
    return *this;
}

void BTHaveMsg::netPack(cCommBuffer *b)
{
    GenericAppMsg::netPack(b);
    doPacking(b,this->length_prefix_var);
    doPacking(b,this->ID_var);
    doPacking(b,this->index_var);
}

void BTHaveMsg::netUnpack(cCommBuffer *b)
{
    GenericAppMsg::netUnpack(b);
    doUnpacking(b,this->length_prefix_var);
    doUnpacking(b,this->ID_var);
    doUnpacking(b,this->index_var);
}

int BTHaveMsg::length_prefix() const
{
    return length_prefix_var;
}

void BTHaveMsg::setLength_prefix(int length_prefix_var)
{
    this->length_prefix_var = length_prefix_var;
}

unsigned short BTHaveMsg::ID() const
{
    return ID_var;
}

void BTHaveMsg::setID(unsigned short ID_var)
{
    this->ID_var = ID_var;
}

int BTHaveMsg::index() const
{
    return index_var;
}

void BTHaveMsg::setIndex(int index_var)
{
    this->index_var = index_var;
}

class BTHaveMsgDescriptor : public cStructDescriptor
{
  public:
    BTHaveMsgDescriptor();
    virtual ~BTHaveMsgDescriptor();
    BTHaveMsgDescriptor& operator=(const BTHaveMsgDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTHaveMsgDescriptor(*this);}

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

Register_Class(BTHaveMsgDescriptor);

BTHaveMsgDescriptor::BTHaveMsgDescriptor() : cStructDescriptor("GenericAppMsg")
{
}

BTHaveMsgDescriptor::~BTHaveMsgDescriptor()
{
}

int BTHaveMsgDescriptor::getFieldCount()
{
    return baseclassdesc ? 3+baseclassdesc->getFieldCount() : 3;
}

int BTHaveMsgDescriptor::getFieldType(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldType(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return FT_BASIC;
        case 1: return FT_BASIC;
        case 2: return FT_BASIC;
        default: return FT_INVALID;
    }
}

const char *BTHaveMsgDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "length_prefix";
        case 1: return "ID";
        case 2: return "index";
        default: return NULL;
    }
}

const char *BTHaveMsgDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "int";
        case 1: return "unsigned short";
        case 2: return "int";
        default: return NULL;
    }
}

const char *BTHaveMsgDescriptor::getFieldEnumName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldEnumName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 1: return "PeerWireMsgIDs";
        default: return NULL;
    }
}

int BTHaveMsgDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTHaveMsg *pp = (BTHaveMsg *)p;
    switch (field) {
        default: return 0;
    }
}

bool BTHaveMsgDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTHaveMsg *pp = (BTHaveMsg *)p;
    switch (field) {
        case 0: long2string(pp->length_prefix(),resultbuf,bufsize); return true;
        case 1: long2string(pp->ID(),resultbuf,bufsize); return true;
        case 2: long2string(pp->index(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTHaveMsgDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTHaveMsg *pp = (BTHaveMsg *)p;
    switch (field) {
        case 0: pp->setLength_prefix(string2long(value)); return true;
        case 1: pp->setID(string2long(value)); return true;
        case 2: pp->setIndex(string2long(value)); return true;
        default: return false;
    }
}

const char *BTHaveMsgDescriptor::getFieldStructName(int field)
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

void *BTHaveMsgDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTHaveMsg *pp = (BTHaveMsg *)p;
    switch (field) {
        default: return NULL;
    }
}

sFieldWrapper *BTHaveMsgDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

Register_Class(BTBitfieldMsg);

BTBitfieldMsg::BTBitfieldMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    unsigned int i;
    this->length_prefix_var = 0;
    this->ID_var = 0;
    bitfield_arraysize = 0;
    this->bitfield_var = 0;
}

BTBitfieldMsg::BTBitfieldMsg(const BTBitfieldMsg& other) : GenericAppMsg()
{
    unsigned int i;
    setName(other.name());
    bitfield_arraysize = 0;
    this->bitfield_var = 0;
    operator=(other);
}

BTBitfieldMsg::~BTBitfieldMsg()
{
    unsigned int i;
    delete [] bitfield_var;
}

BTBitfieldMsg& BTBitfieldMsg::operator=(const BTBitfieldMsg& other)
{
    if (this==&other) return *this;
    unsigned int i;
    GenericAppMsg::operator=(other);
    this->length_prefix_var = other.length_prefix_var;
    this->ID_var = other.ID_var;
    delete [] this->bitfield_var;
    this->bitfield_var = (other.bitfield_arraysize==0) ? NULL : new bool[other.bitfield_arraysize];
    bitfield_arraysize = other.bitfield_arraysize;
    for (i=0; i<bitfield_arraysize; i++)
        this->bitfield_var[i] = other.bitfield_var[i];
    return *this;
}

void BTBitfieldMsg::netPack(cCommBuffer *b)
{
    GenericAppMsg::netPack(b);
    doPacking(b,this->length_prefix_var);
    doPacking(b,this->ID_var);
    b->pack(bitfield_arraysize);
    doPacking(b,this->bitfield_var,bitfield_arraysize);
}

void BTBitfieldMsg::netUnpack(cCommBuffer *b)
{
    GenericAppMsg::netUnpack(b);
    doUnpacking(b,this->length_prefix_var);
    doUnpacking(b,this->ID_var);
    delete [] this->bitfield_var;
    b->unpack(bitfield_arraysize);
    if (bitfield_arraysize==0) {
        this->bitfield_var = 0;
    } else {
        this->bitfield_var = new bool[bitfield_arraysize];
        doUnpacking(b,this->bitfield_var,bitfield_arraysize);
    }
}

int BTBitfieldMsg::length_prefix() const
{
    return length_prefix_var;
}

void BTBitfieldMsg::setLength_prefix(int length_prefix_var)
{
    this->length_prefix_var = length_prefix_var;
}

unsigned short BTBitfieldMsg::ID() const
{
    return ID_var;
}

void BTBitfieldMsg::setID(unsigned short ID_var)
{
    this->ID_var = ID_var;
}

void BTBitfieldMsg::setBitfieldArraySize(unsigned int size)
{
    bool *bitfield_var2 = (size==0) ? NULL : new bool[size];
    unsigned int sz = bitfield_arraysize < size ? bitfield_arraysize : size;
    unsigned int i;
    for (i=0; i<sz; i++)
        bitfield_var2[i] = this->bitfield_var[i];
    for (i=sz; i<size; i++)
        bitfield_var2[i] = 0;
    bitfield_arraysize = size;
    delete [] this->bitfield_var;
    this->bitfield_var = bitfield_var2;
}

unsigned int BTBitfieldMsg::bitfieldArraySize() const
{
    return bitfield_arraysize;
}

bool BTBitfieldMsg::bitfield(unsigned int k) const
{
    if (k>=bitfield_arraysize) throw new cException("Array of size %d indexed by %d", bitfield_arraysize, k);
    return bitfield_var[k];
}

void BTBitfieldMsg::setBitfield(unsigned int k, bool bitfield_var)
{
    if (k>=bitfield_arraysize) throw new cException("Array of size %d indexed by %d", bitfield_arraysize, k);
    this->bitfield_var[k]=bitfield_var;
}

class BTBitfieldMsgDescriptor : public cStructDescriptor
{
  public:
    BTBitfieldMsgDescriptor();
    virtual ~BTBitfieldMsgDescriptor();
    BTBitfieldMsgDescriptor& operator=(const BTBitfieldMsgDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTBitfieldMsgDescriptor(*this);}

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

Register_Class(BTBitfieldMsgDescriptor);

BTBitfieldMsgDescriptor::BTBitfieldMsgDescriptor() : cStructDescriptor("GenericAppMsg")
{
}

BTBitfieldMsgDescriptor::~BTBitfieldMsgDescriptor()
{
}

int BTBitfieldMsgDescriptor::getFieldCount()
{
    return baseclassdesc ? 3+baseclassdesc->getFieldCount() : 3;
}

int BTBitfieldMsgDescriptor::getFieldType(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldType(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return FT_BASIC;
        case 1: return FT_BASIC;
        case 2: return FT_BASIC_ARRAY;
        default: return FT_INVALID;
    }
}

const char *BTBitfieldMsgDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "length_prefix";
        case 1: return "ID";
        case 2: return "bitfield";
        default: return NULL;
    }
}

const char *BTBitfieldMsgDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "int";
        case 1: return "unsigned short";
        case 2: return "bool";
        default: return NULL;
    }
}

const char *BTBitfieldMsgDescriptor::getFieldEnumName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldEnumName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 1: return "PeerWireMsgIDs";
        default: return NULL;
    }
}

int BTBitfieldMsgDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTBitfieldMsg *pp = (BTBitfieldMsg *)p;
    switch (field) {
        case 2: return pp->bitfieldArraySize();
        default: return 0;
    }
}

bool BTBitfieldMsgDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTBitfieldMsg *pp = (BTBitfieldMsg *)p;
    switch (field) {
        case 0: long2string(pp->length_prefix(),resultbuf,bufsize); return true;
        case 1: long2string(pp->ID(),resultbuf,bufsize); return true;
        case 2: bool2string(pp->bitfield(i),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTBitfieldMsgDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTBitfieldMsg *pp = (BTBitfieldMsg *)p;
    switch (field) {
        case 0: pp->setLength_prefix(string2long(value)); return true;
        case 1: pp->setID(string2long(value)); return true;
        case 2: pp->setBitfield(i,string2bool(value)); return true;
        default: return false;
    }
}

const char *BTBitfieldMsgDescriptor::getFieldStructName(int field)
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

void *BTBitfieldMsgDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTBitfieldMsg *pp = (BTBitfieldMsg *)p;
    switch (field) {
        default: return NULL;
    }
}

sFieldWrapper *BTBitfieldMsgDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

Register_Class(BTRequestCancelMsg);

BTRequestCancelMsg::BTRequestCancelMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    this->length_prefix_var = 0;
    this->ID_var = 0;
    this->index_var = 0;
    this->begin_var = 0;
    this->dataLength_var = 0;
    this->endGame_var = false;
}

BTRequestCancelMsg::BTRequestCancelMsg(const BTRequestCancelMsg& other) : GenericAppMsg()
{
    unsigned int i;
    setName(other.name());
    operator=(other);
}

BTRequestCancelMsg::~BTRequestCancelMsg()
{
    unsigned int i;
}

BTRequestCancelMsg& BTRequestCancelMsg::operator=(const BTRequestCancelMsg& other)
{
    if (this==&other) return *this;
    unsigned int i;
    GenericAppMsg::operator=(other);
    this->length_prefix_var = other.length_prefix_var;
    this->ID_var = other.ID_var;
    this->index_var = other.index_var;
    this->begin_var = other.begin_var;
    this->dataLength_var = other.dataLength_var;
    this->endGame_var = other.endGame_var;
    return *this;
}

void BTRequestCancelMsg::netPack(cCommBuffer *b)
{
    GenericAppMsg::netPack(b);
    doPacking(b,this->length_prefix_var);
    doPacking(b,this->ID_var);
    doPacking(b,this->index_var);
    doPacking(b,this->begin_var);
    doPacking(b,this->dataLength_var);
    doPacking(b,this->endGame_var);
}

void BTRequestCancelMsg::netUnpack(cCommBuffer *b)
{
    GenericAppMsg::netUnpack(b);
    doUnpacking(b,this->length_prefix_var);
    doUnpacking(b,this->ID_var);
    doUnpacking(b,this->index_var);
    doUnpacking(b,this->begin_var);
    doUnpacking(b,this->dataLength_var);
    doUnpacking(b,this->endGame_var);
}

int BTRequestCancelMsg::length_prefix() const
{
    return length_prefix_var;
}

void BTRequestCancelMsg::setLength_prefix(int length_prefix_var)
{
    this->length_prefix_var = length_prefix_var;
}

unsigned short BTRequestCancelMsg::ID() const
{
    return ID_var;
}

void BTRequestCancelMsg::setID(unsigned short ID_var)
{
    this->ID_var = ID_var;
}

int BTRequestCancelMsg::index() const
{
    return index_var;
}

void BTRequestCancelMsg::setIndex(int index_var)
{
    this->index_var = index_var;
}

int BTRequestCancelMsg::begin() const
{
    return begin_var;
}

void BTRequestCancelMsg::setBegin(int begin_var)
{
    this->begin_var = begin_var;
}

int BTRequestCancelMsg::dataLength() const
{
    return dataLength_var;
}

void BTRequestCancelMsg::setDataLength(int dataLength_var)
{
    this->dataLength_var = dataLength_var;
}

bool BTRequestCancelMsg::endGame() const
{
    return endGame_var;
}

void BTRequestCancelMsg::setEndGame(bool endGame_var)
{
    this->endGame_var = endGame_var;
}

class BTRequestCancelMsgDescriptor : public cStructDescriptor
{
  public:
    BTRequestCancelMsgDescriptor();
    virtual ~BTRequestCancelMsgDescriptor();
    BTRequestCancelMsgDescriptor& operator=(const BTRequestCancelMsgDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTRequestCancelMsgDescriptor(*this);}

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

Register_Class(BTRequestCancelMsgDescriptor);

BTRequestCancelMsgDescriptor::BTRequestCancelMsgDescriptor() : cStructDescriptor("GenericAppMsg")
{
}

BTRequestCancelMsgDescriptor::~BTRequestCancelMsgDescriptor()
{
}

int BTRequestCancelMsgDescriptor::getFieldCount()
{
    return baseclassdesc ? 6+baseclassdesc->getFieldCount() : 6;
}

int BTRequestCancelMsgDescriptor::getFieldType(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldType(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return FT_BASIC;
        case 1: return FT_BASIC;
        case 2: return FT_BASIC;
        case 3: return FT_BASIC;
        case 4: return FT_BASIC;
        case 5: return FT_BASIC;
        default: return FT_INVALID;
    }
}

const char *BTRequestCancelMsgDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "length_prefix";
        case 1: return "ID";
        case 2: return "index";
        case 3: return "begin";
        case 4: return "dataLength";
        case 5: return "endGame";
        default: return NULL;
    }
}

const char *BTRequestCancelMsgDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "int";
        case 1: return "unsigned short";
        case 2: return "int";
        case 3: return "int";
        case 4: return "int";
        case 5: return "bool";
        default: return NULL;
    }
}

const char *BTRequestCancelMsgDescriptor::getFieldEnumName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldEnumName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 1: return "PeerWireMsgIDs";
        default: return NULL;
    }
}

int BTRequestCancelMsgDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTRequestCancelMsg *pp = (BTRequestCancelMsg *)p;
    switch (field) {
        default: return 0;
    }
}

bool BTRequestCancelMsgDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTRequestCancelMsg *pp = (BTRequestCancelMsg *)p;
    switch (field) {
        case 0: long2string(pp->length_prefix(),resultbuf,bufsize); return true;
        case 1: long2string(pp->ID(),resultbuf,bufsize); return true;
        case 2: long2string(pp->index(),resultbuf,bufsize); return true;
        case 3: long2string(pp->begin(),resultbuf,bufsize); return true;
        case 4: long2string(pp->dataLength(),resultbuf,bufsize); return true;
        case 5: bool2string(pp->endGame(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTRequestCancelMsgDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTRequestCancelMsg *pp = (BTRequestCancelMsg *)p;
    switch (field) {
        case 0: pp->setLength_prefix(string2long(value)); return true;
        case 1: pp->setID(string2long(value)); return true;
        case 2: pp->setIndex(string2long(value)); return true;
        case 3: pp->setBegin(string2long(value)); return true;
        case 4: pp->setDataLength(string2long(value)); return true;
        case 5: pp->setEndGame(string2bool(value)); return true;
        default: return false;
    }
}

const char *BTRequestCancelMsgDescriptor::getFieldStructName(int field)
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

void *BTRequestCancelMsgDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTRequestCancelMsg *pp = (BTRequestCancelMsg *)p;
    switch (field) {
        default: return NULL;
    }
}

sFieldWrapper *BTRequestCancelMsgDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

Register_Class(BTPieceMsg);

BTPieceMsg::BTPieceMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    this->length_prefix_var = 0;
    this->ID_var = 0;
    this->index_var = 0;
    this->begin_var = 0;
    this->data_var = "";
}

BTPieceMsg::BTPieceMsg(const BTPieceMsg& other) : GenericAppMsg()
{
    unsigned int i;
    setName(other.name());
    operator=(other);
}

BTPieceMsg::~BTPieceMsg()
{
    unsigned int i;
}

BTPieceMsg& BTPieceMsg::operator=(const BTPieceMsg& other)
{
    if (this==&other) return *this;
    unsigned int i;
    GenericAppMsg::operator=(other);
    this->length_prefix_var = other.length_prefix_var;
    this->ID_var = other.ID_var;
    this->index_var = other.index_var;
    this->begin_var = other.begin_var;
    this->data_var = other.data_var;
    return *this;
}

void BTPieceMsg::netPack(cCommBuffer *b)
{
    GenericAppMsg::netPack(b);
    doPacking(b,this->length_prefix_var);
    doPacking(b,this->ID_var);
    doPacking(b,this->index_var);
    doPacking(b,this->begin_var);
    doPacking(b,this->data_var);
}

void BTPieceMsg::netUnpack(cCommBuffer *b)
{
    GenericAppMsg::netUnpack(b);
    doUnpacking(b,this->length_prefix_var);
    doUnpacking(b,this->ID_var);
    doUnpacking(b,this->index_var);
    doUnpacking(b,this->begin_var);
    doUnpacking(b,this->data_var);
}

int BTPieceMsg::length_prefix() const
{
    return length_prefix_var;
}

void BTPieceMsg::setLength_prefix(int length_prefix_var)
{
    this->length_prefix_var = length_prefix_var;
}

unsigned short BTPieceMsg::ID() const
{
    return ID_var;
}

void BTPieceMsg::setID(unsigned short ID_var)
{
    this->ID_var = ID_var;
}

int BTPieceMsg::index() const
{
    return index_var;
}

void BTPieceMsg::setIndex(int index_var)
{
    this->index_var = index_var;
}

int BTPieceMsg::begin() const
{
    return begin_var;
}

void BTPieceMsg::setBegin(int begin_var)
{
    this->begin_var = begin_var;
}

const char * BTPieceMsg::data() const
{
    return data_var.c_str();
}

void BTPieceMsg::setData(const char * data_var)
{
    this->data_var = data_var;
}

class BTPieceMsgDescriptor : public cStructDescriptor
{
  public:
    BTPieceMsgDescriptor();
    virtual ~BTPieceMsgDescriptor();
    BTPieceMsgDescriptor& operator=(const BTPieceMsgDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTPieceMsgDescriptor(*this);}

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

Register_Class(BTPieceMsgDescriptor);

BTPieceMsgDescriptor::BTPieceMsgDescriptor() : cStructDescriptor("GenericAppMsg")
{
}

BTPieceMsgDescriptor::~BTPieceMsgDescriptor()
{
}

int BTPieceMsgDescriptor::getFieldCount()
{
    return baseclassdesc ? 5+baseclassdesc->getFieldCount() : 5;
}

int BTPieceMsgDescriptor::getFieldType(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldType(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return FT_BASIC;
        case 1: return FT_BASIC;
        case 2: return FT_BASIC;
        case 3: return FT_BASIC;
        case 4: return FT_BASIC;
        default: return FT_INVALID;
    }
}

const char *BTPieceMsgDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "length_prefix";
        case 1: return "ID";
        case 2: return "index";
        case 3: return "begin";
        case 4: return "data";
        default: return NULL;
    }
}

const char *BTPieceMsgDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "int";
        case 1: return "unsigned short";
        case 2: return "int";
        case 3: return "int";
        case 4: return "string";
        default: return NULL;
    }
}

const char *BTPieceMsgDescriptor::getFieldEnumName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldEnumName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 1: return "PeerWireMsgIDs";
        default: return NULL;
    }
}

int BTPieceMsgDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTPieceMsg *pp = (BTPieceMsg *)p;
    switch (field) {
        default: return 0;
    }
}

bool BTPieceMsgDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTPieceMsg *pp = (BTPieceMsg *)p;
    switch (field) {
        case 0: long2string(pp->length_prefix(),resultbuf,bufsize); return true;
        case 1: long2string(pp->ID(),resultbuf,bufsize); return true;
        case 2: long2string(pp->index(),resultbuf,bufsize); return true;
        case 3: long2string(pp->begin(),resultbuf,bufsize); return true;
        case 4: oppstring2string(pp->data(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTPieceMsgDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTPieceMsg *pp = (BTPieceMsg *)p;
    switch (field) {
        case 0: pp->setLength_prefix(string2long(value)); return true;
        case 1: pp->setID(string2long(value)); return true;
        case 2: pp->setIndex(string2long(value)); return true;
        case 3: pp->setBegin(string2long(value)); return true;
        case 4: pp->setData((value)); return true;
        default: return false;
    }
}

const char *BTPieceMsgDescriptor::getFieldStructName(int field)
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

void *BTPieceMsgDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTPieceMsg *pp = (BTPieceMsg *)p;
    switch (field) {
        default: return NULL;
    }
}

sFieldWrapper *BTPieceMsgDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

Register_Class(BTInternalMsg);

BTInternalMsg::BTInternalMsg(const char *name, int kind) : cMessage(name,kind)
{
    this->text_var = "";
    this->pieceIndex_var = 0;
    this->blockIndex_var = 0;
    this->chokedPiece_var = false;
}

BTInternalMsg::BTInternalMsg(const BTInternalMsg& other) : cMessage()
{
    unsigned int i;
    setName(other.name());
    operator=(other);
}

BTInternalMsg::~BTInternalMsg()
{
    unsigned int i;
}

BTInternalMsg& BTInternalMsg::operator=(const BTInternalMsg& other)
{
    if (this==&other) return *this;
    unsigned int i;
    cMessage::operator=(other);
    this->text_var = other.text_var;
    this->peer_var = other.peer_var;
    this->pieceIndex_var = other.pieceIndex_var;
    this->blockIndex_var = other.blockIndex_var;
    this->chokedPiece_var = other.chokedPiece_var;
    return *this;
}

void BTInternalMsg::netPack(cCommBuffer *b)
{
    cMessage::netPack(b);
    doPacking(b,this->text_var);
    doPacking(b,this->peer_var);
    doPacking(b,this->pieceIndex_var);
    doPacking(b,this->blockIndex_var);
    doPacking(b,this->chokedPiece_var);
}

void BTInternalMsg::netUnpack(cCommBuffer *b)
{
    cMessage::netUnpack(b);
    doUnpacking(b,this->text_var);
    doUnpacking(b,this->peer_var);
    doUnpacking(b,this->pieceIndex_var);
    doUnpacking(b,this->blockIndex_var);
    doUnpacking(b,this->chokedPiece_var);
}

const char * BTInternalMsg::text() const
{
    return text_var.c_str();
}

void BTInternalMsg::setText(const char * text_var)
{
    this->text_var = text_var;
}

PEER& BTInternalMsg::peer()
{
    return peer_var;
}

void BTInternalMsg::setPeer(const PEER& peer_var)
{
    this->peer_var = peer_var;
}

int BTInternalMsg::pieceIndex() const
{
    return pieceIndex_var;
}

void BTInternalMsg::setPieceIndex(int pieceIndex_var)
{
    this->pieceIndex_var = pieceIndex_var;
}

int BTInternalMsg::blockIndex() const
{
    return blockIndex_var;
}

void BTInternalMsg::setBlockIndex(int blockIndex_var)
{
    this->blockIndex_var = blockIndex_var;
}

bool BTInternalMsg::chokedPiece() const
{
    return chokedPiece_var;
}

void BTInternalMsg::setChokedPiece(bool chokedPiece_var)
{
    this->chokedPiece_var = chokedPiece_var;
}

class BTInternalMsgDescriptor : public cStructDescriptor
{
  public:
    BTInternalMsgDescriptor();
    virtual ~BTInternalMsgDescriptor();
    BTInternalMsgDescriptor& operator=(const BTInternalMsgDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTInternalMsgDescriptor(*this);}

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

Register_Class(BTInternalMsgDescriptor);

BTInternalMsgDescriptor::BTInternalMsgDescriptor() : cStructDescriptor("cMessage")
{
}

BTInternalMsgDescriptor::~BTInternalMsgDescriptor()
{
}

int BTInternalMsgDescriptor::getFieldCount()
{
    return baseclassdesc ? 5+baseclassdesc->getFieldCount() : 5;
}

int BTInternalMsgDescriptor::getFieldType(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldType(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return FT_BASIC;
        case 1: return FT_STRUCT;
        case 2: return FT_BASIC;
        case 3: return FT_BASIC;
        case 4: return FT_BASIC;
        default: return FT_INVALID;
    }
}

const char *BTInternalMsgDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "text";
        case 1: return "peer";
        case 2: return "pieceIndex";
        case 3: return "blockIndex";
        case 4: return "chokedPiece";
        default: return NULL;
    }
}

const char *BTInternalMsgDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "string";
        case 1: return "PEER";
        case 2: return "int";
        case 3: return "int";
        case 4: return "bool";
        default: return NULL;
    }
}

const char *BTInternalMsgDescriptor::getFieldEnumName(int field)
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

int BTInternalMsgDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTInternalMsg *pp = (BTInternalMsg *)p;
    switch (field) {
        default: return 0;
    }
}

bool BTInternalMsgDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTInternalMsg *pp = (BTInternalMsg *)p;
    switch (field) {
        case 0: oppstring2string(pp->text(),resultbuf,bufsize); return true;
        case 1: {std::stringstream out; out << pp->peer(); opp_strprettytrunc(resultbuf,out.str().c_str(),bufsize-1); return true;}
        case 2: long2string(pp->pieceIndex(),resultbuf,bufsize); return true;
        case 3: long2string(pp->blockIndex(),resultbuf,bufsize); return true;
        case 4: bool2string(pp->chokedPiece(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTInternalMsgDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTInternalMsg *pp = (BTInternalMsg *)p;
    switch (field) {
        case 0: pp->setText((value)); return true;
        case 1: return false;
        case 2: pp->setPieceIndex(string2long(value)); return true;
        case 3: pp->setBlockIndex(string2long(value)); return true;
        case 4: pp->setChokedPiece(string2bool(value)); return true;
        default: return false;
    }
}

const char *BTInternalMsgDescriptor::getFieldStructName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 1: return "PEER"; break;
        default: return NULL;
    }
}

void *BTInternalMsgDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTInternalMsg *pp = (BTInternalMsg *)p;
    switch (field) {
        case 1: return (void *)&pp->peer(); break;
        default: return NULL;
    }
}

sFieldWrapper *BTInternalMsgDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

Register_Class(BTAcceptConnectionMsg);

BTAcceptConnectionMsg::BTAcceptConnectionMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    this->accept_var = true;
}

BTAcceptConnectionMsg::BTAcceptConnectionMsg(const BTAcceptConnectionMsg& other) : GenericAppMsg()
{
    unsigned int i;
    setName(other.name());
    operator=(other);
}

BTAcceptConnectionMsg::~BTAcceptConnectionMsg()
{
    unsigned int i;
}

BTAcceptConnectionMsg& BTAcceptConnectionMsg::operator=(const BTAcceptConnectionMsg& other)
{
    if (this==&other) return *this;
    unsigned int i;
    GenericAppMsg::operator=(other);
    this->accept_var = other.accept_var;
    return *this;
}

void BTAcceptConnectionMsg::netPack(cCommBuffer *b)
{
    GenericAppMsg::netPack(b);
    doPacking(b,this->accept_var);
}

void BTAcceptConnectionMsg::netUnpack(cCommBuffer *b)
{
    GenericAppMsg::netUnpack(b);
    doUnpacking(b,this->accept_var);
}

bool BTAcceptConnectionMsg::accept() const
{
    return accept_var;
}

void BTAcceptConnectionMsg::setAccept(bool accept_var)
{
    this->accept_var = accept_var;
}

class BTAcceptConnectionMsgDescriptor : public cStructDescriptor
{
  public:
    BTAcceptConnectionMsgDescriptor();
    virtual ~BTAcceptConnectionMsgDescriptor();
    BTAcceptConnectionMsgDescriptor& operator=(const BTAcceptConnectionMsgDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTAcceptConnectionMsgDescriptor(*this);}

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

Register_Class(BTAcceptConnectionMsgDescriptor);

BTAcceptConnectionMsgDescriptor::BTAcceptConnectionMsgDescriptor() : cStructDescriptor("GenericAppMsg")
{
}

BTAcceptConnectionMsgDescriptor::~BTAcceptConnectionMsgDescriptor()
{
}

int BTAcceptConnectionMsgDescriptor::getFieldCount()
{
    return baseclassdesc ? 1+baseclassdesc->getFieldCount() : 1;
}

int BTAcceptConnectionMsgDescriptor::getFieldType(int field)
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

const char *BTAcceptConnectionMsgDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "accept";
        default: return NULL;
    }
}

const char *BTAcceptConnectionMsgDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "bool";
        default: return NULL;
    }
}

const char *BTAcceptConnectionMsgDescriptor::getFieldEnumName(int field)
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

int BTAcceptConnectionMsgDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTAcceptConnectionMsg *pp = (BTAcceptConnectionMsg *)p;
    switch (field) {
        default: return 0;
    }
}

bool BTAcceptConnectionMsgDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTAcceptConnectionMsg *pp = (BTAcceptConnectionMsg *)p;
    switch (field) {
        case 0: bool2string(pp->accept(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTAcceptConnectionMsgDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTAcceptConnectionMsg *pp = (BTAcceptConnectionMsg *)p;
    switch (field) {
        case 0: pp->setAccept(string2bool(value)); return true;
        default: return false;
    }
}

const char *BTAcceptConnectionMsgDescriptor::getFieldStructName(int field)
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

void *BTAcceptConnectionMsgDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTAcceptConnectionMsg *pp = (BTAcceptConnectionMsg *)p;
    switch (field) {
        default: return NULL;
    }
}

sFieldWrapper *BTAcceptConnectionMsgDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

