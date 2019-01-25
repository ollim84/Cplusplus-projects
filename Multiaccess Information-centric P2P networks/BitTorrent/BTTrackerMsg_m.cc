//
// Generated file, do not edit! Created by opp_msgc 3.3 from BTTrackerMsg.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "BTTrackerMsg_m.h"

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


EXECUTE_ON_STARTUP(ANNOUNCE_TYPE,
    cEnum *e = findEnum("ANNOUNCE_TYPE");
    if (!e) enums.instance()->add(e = new cEnum("ANNOUNCE_TYPE"));
    e->insert(A_STARTED, "A_STARTED");
    e->insert(A_STOPPED, "A_STOPPED");
    e->insert(A_COMPLETED, "A_COMPLETED");
    e->insert(A_NORMAL, "A_NORMAL");
);

EXECUTE_ON_STARTUP(REPLY_TYPE,
    cEnum *e = findEnum("REPLY_TYPE");
    if (!e) enums.instance()->add(e = new cEnum("REPLY_TYPE"));
    e->insert(R_FAIL, "R_FAIL");
    e->insert(R_VALID, "R_VALID");
    e->insert(R_WARN, "R_WARN");
);

PEER::PEER()
{
    peerId = "";
    peerPort = 0;
}

void doPacking(cCommBuffer *b, PEER& a)
{
    doPacking(b,a.peerId);
    doPacking(b,a.peerPort);
    doPacking(b,a.ipAddress);
}

void doUnpacking(cCommBuffer *b, PEER& a)
{
    doUnpacking(b,a.peerId);
    doUnpacking(b,a.peerPort);
    doUnpacking(b,a.ipAddress);
}

class PEERDescriptor : public cStructDescriptor
{
  public:
    PEERDescriptor();
    virtual ~PEERDescriptor();
    PEERDescriptor& operator=(const PEERDescriptor& other);
    virtual cPolymorphic *dup() const {return new PEERDescriptor(*this);}

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

Register_Class(PEERDescriptor);

PEERDescriptor::PEERDescriptor() : cStructDescriptor("")
{
}

PEERDescriptor::~PEERDescriptor()
{
}

int PEERDescriptor::getFieldCount()
{
    return baseclassdesc ? 3+baseclassdesc->getFieldCount() : 3;
}

int PEERDescriptor::getFieldType(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldType(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return FT_BASIC;
        case 1: return FT_BASIC;
        case 2: return FT_STRUCT;
        default: return FT_INVALID;
    }
}

const char *PEERDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "peerId";
        case 1: return "peerPort";
        case 2: return "ipAddress";
        default: return NULL;
    }
}

const char *PEERDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "string";
        case 1: return "unsigned int";
        case 2: return "IPvXAddress";
        default: return NULL;
    }
}

const char *PEERDescriptor::getFieldEnumName(int field)
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

int PEERDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    PEER *pp = (PEER *)p;
    switch (field) {
        default: return 0;
    }
}

bool PEERDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    PEER *pp = (PEER *)p;
    switch (field) {
        case 0: oppstring2string(pp->peerId,resultbuf,bufsize); return true;
        case 1: long2string(pp->peerPort,resultbuf,bufsize); return true;
        case 2: {std::stringstream out; out << pp->ipAddress; opp_strprettytrunc(resultbuf,out.str().c_str(),bufsize-1); return true;}
        default: return false;
    }
}

bool PEERDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    PEER *pp = (PEER *)p;
    switch (field) {
        case 0: pp->peerId = (value); return true;
        case 1: pp->peerPort = string2long(value); return true;
        case 2: return false;
        default: return false;
    }
}

const char *PEERDescriptor::getFieldStructName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 2: return "IPvXAddress"; break;
        default: return NULL;
    }
}

void *PEERDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    PEER *pp = (PEER *)p;
    switch (field) {
        case 2: return (void *)&pp->ipAddress; break;
        default: return NULL;
    }
}

sFieldWrapper *PEERDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

Register_Class(BTTrackerMsgAnnounce);

BTTrackerMsgAnnounce::BTTrackerMsgAnnounce(const char *name, int kind) : cMessage(name,kind)
{
    this->infoHash_var = "";
    this->peerId_var = "";
    this->peerPort_var = 0;
    this->event_var = 0;
    this->compact_var = false;
    this->noPeerId_var = false;
    this->numWant_var = 0;
    this->key_var = "";
    this->trackerId_var = "";
}

BTTrackerMsgAnnounce::BTTrackerMsgAnnounce(const BTTrackerMsgAnnounce& other) : cMessage()
{
    unsigned int i;
    setName(other.name());
    operator=(other);
}

BTTrackerMsgAnnounce::~BTTrackerMsgAnnounce()
{
    unsigned int i;
}

BTTrackerMsgAnnounce& BTTrackerMsgAnnounce::operator=(const BTTrackerMsgAnnounce& other)
{
    if (this==&other) return *this;
    unsigned int i;
    cMessage::operator=(other);
    this->infoHash_var = other.infoHash_var;
    this->peerId_var = other.peerId_var;
    this->peerPort_var = other.peerPort_var;
    this->event_var = other.event_var;
    this->compact_var = other.compact_var;
    this->noPeerId_var = other.noPeerId_var;
    this->numWant_var = other.numWant_var;
    this->key_var = other.key_var;
    this->trackerId_var = other.trackerId_var;
    return *this;
}

void BTTrackerMsgAnnounce::netPack(cCommBuffer *b)
{
    cMessage::netPack(b);
    doPacking(b,this->infoHash_var);
    doPacking(b,this->peerId_var);
    doPacking(b,this->peerPort_var);
    doPacking(b,this->event_var);
    doPacking(b,this->compact_var);
    doPacking(b,this->noPeerId_var);
    doPacking(b,this->numWant_var);
    doPacking(b,this->key_var);
    doPacking(b,this->trackerId_var);
}

void BTTrackerMsgAnnounce::netUnpack(cCommBuffer *b)
{
    cMessage::netUnpack(b);
    doUnpacking(b,this->infoHash_var);
    doUnpacking(b,this->peerId_var);
    doUnpacking(b,this->peerPort_var);
    doUnpacking(b,this->event_var);
    doUnpacking(b,this->compact_var);
    doUnpacking(b,this->noPeerId_var);
    doUnpacking(b,this->numWant_var);
    doUnpacking(b,this->key_var);
    doUnpacking(b,this->trackerId_var);
}

const char * BTTrackerMsgAnnounce::infoHash() const
{
    return infoHash_var.c_str();
}

void BTTrackerMsgAnnounce::setInfoHash(const char * infoHash_var)
{
    this->infoHash_var = infoHash_var;
}

const char * BTTrackerMsgAnnounce::peerId() const
{
    return peerId_var.c_str();
}

void BTTrackerMsgAnnounce::setPeerId(const char * peerId_var)
{
    this->peerId_var = peerId_var;
}

unsigned int BTTrackerMsgAnnounce::peerPort() const
{
    return peerPort_var;
}

void BTTrackerMsgAnnounce::setPeerPort(unsigned int peerPort_var)
{
    this->peerPort_var = peerPort_var;
}

unsigned int BTTrackerMsgAnnounce::event() const
{
    return event_var;
}

void BTTrackerMsgAnnounce::setEvent(unsigned int event_var)
{
    this->event_var = event_var;
}

bool BTTrackerMsgAnnounce::compact() const
{
    return compact_var;
}

void BTTrackerMsgAnnounce::setCompact(bool compact_var)
{
    this->compact_var = compact_var;
}

bool BTTrackerMsgAnnounce::noPeerId() const
{
    return noPeerId_var;
}

void BTTrackerMsgAnnounce::setNoPeerId(bool noPeerId_var)
{
    this->noPeerId_var = noPeerId_var;
}

unsigned int BTTrackerMsgAnnounce::numWant() const
{
    return numWant_var;
}

void BTTrackerMsgAnnounce::setNumWant(unsigned int numWant_var)
{
    this->numWant_var = numWant_var;
}

const char * BTTrackerMsgAnnounce::key() const
{
    return key_var.c_str();
}

void BTTrackerMsgAnnounce::setKey(const char * key_var)
{
    this->key_var = key_var;
}

const char * BTTrackerMsgAnnounce::trackerId() const
{
    return trackerId_var.c_str();
}

void BTTrackerMsgAnnounce::setTrackerId(const char * trackerId_var)
{
    this->trackerId_var = trackerId_var;
}

class BTTrackerMsgAnnounceDescriptor : public cStructDescriptor
{
  public:
    BTTrackerMsgAnnounceDescriptor();
    virtual ~BTTrackerMsgAnnounceDescriptor();
    BTTrackerMsgAnnounceDescriptor& operator=(const BTTrackerMsgAnnounceDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTTrackerMsgAnnounceDescriptor(*this);}

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

Register_Class(BTTrackerMsgAnnounceDescriptor);

BTTrackerMsgAnnounceDescriptor::BTTrackerMsgAnnounceDescriptor() : cStructDescriptor("cMessage")
{
}

BTTrackerMsgAnnounceDescriptor::~BTTrackerMsgAnnounceDescriptor()
{
}

int BTTrackerMsgAnnounceDescriptor::getFieldCount()
{
    return baseclassdesc ? 9+baseclassdesc->getFieldCount() : 9;
}

int BTTrackerMsgAnnounceDescriptor::getFieldType(int field)
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
        case 6: return FT_BASIC;
        case 7: return FT_BASIC;
        case 8: return FT_BASIC;
        default: return FT_INVALID;
    }
}

const char *BTTrackerMsgAnnounceDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "infoHash";
        case 1: return "peerId";
        case 2: return "peerPort";
        case 3: return "event";
        case 4: return "compact";
        case 5: return "noPeerId";
        case 6: return "numWant";
        case 7: return "key";
        case 8: return "trackerId";
        default: return NULL;
    }
}

const char *BTTrackerMsgAnnounceDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "string";
        case 1: return "string";
        case 2: return "unsigned int";
        case 3: return "unsigned int";
        case 4: return "bool";
        case 5: return "bool";
        case 6: return "unsigned int";
        case 7: return "string";
        case 8: return "string";
        default: return NULL;
    }
}

const char *BTTrackerMsgAnnounceDescriptor::getFieldEnumName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldEnumName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 3: return "ANNOUNCE_TYPE";
        default: return NULL;
    }
}

int BTTrackerMsgAnnounceDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTTrackerMsgAnnounce *pp = (BTTrackerMsgAnnounce *)p;
    switch (field) {
        default: return 0;
    }
}

bool BTTrackerMsgAnnounceDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTTrackerMsgAnnounce *pp = (BTTrackerMsgAnnounce *)p;
    switch (field) {
        case 0: oppstring2string(pp->infoHash(),resultbuf,bufsize); return true;
        case 1: oppstring2string(pp->peerId(),resultbuf,bufsize); return true;
        case 2: long2string(pp->peerPort(),resultbuf,bufsize); return true;
        case 3: long2string(pp->event(),resultbuf,bufsize); return true;
        case 4: bool2string(pp->compact(),resultbuf,bufsize); return true;
        case 5: bool2string(pp->noPeerId(),resultbuf,bufsize); return true;
        case 6: long2string(pp->numWant(),resultbuf,bufsize); return true;
        case 7: oppstring2string(pp->key(),resultbuf,bufsize); return true;
        case 8: oppstring2string(pp->trackerId(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool BTTrackerMsgAnnounceDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTTrackerMsgAnnounce *pp = (BTTrackerMsgAnnounce *)p;
    switch (field) {
        case 0: pp->setInfoHash((value)); return true;
        case 1: pp->setPeerId((value)); return true;
        case 2: pp->setPeerPort(string2long(value)); return true;
        case 3: pp->setEvent(string2long(value)); return true;
        case 4: pp->setCompact(string2bool(value)); return true;
        case 5: pp->setNoPeerId(string2bool(value)); return true;
        case 6: pp->setNumWant(string2long(value)); return true;
        case 7: pp->setKey((value)); return true;
        case 8: pp->setTrackerId((value)); return true;
        default: return false;
    }
}

const char *BTTrackerMsgAnnounceDescriptor::getFieldStructName(int field)
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

void *BTTrackerMsgAnnounceDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTTrackerMsgAnnounce *pp = (BTTrackerMsgAnnounce *)p;
    switch (field) {
        default: return NULL;
    }
}

sFieldWrapper *BTTrackerMsgAnnounceDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

Register_Class(BTTrackerMsgResponse);

BTTrackerMsgResponse::BTTrackerMsgResponse(const char *name, int kind) : cMessage(name,kind)
{
    unsigned int i;
    this->failure_var = "";
    this->warning_var = "";
    this->announceInterval_var = 0;
    this->trackerId_var = "";
    this->complete_var = 0;
    this->incomplete_var = 0;
    peers_arraysize = 0;
    this->peers_var = 0;
}

BTTrackerMsgResponse::BTTrackerMsgResponse(const BTTrackerMsgResponse& other) : cMessage()
{
    unsigned int i;
    setName(other.name());
    peers_arraysize = 0;
    this->peers_var = 0;
    operator=(other);
}

BTTrackerMsgResponse::~BTTrackerMsgResponse()
{
    unsigned int i;
    delete [] peers_var;
}

BTTrackerMsgResponse& BTTrackerMsgResponse::operator=(const BTTrackerMsgResponse& other)
{
    if (this==&other) return *this;
    unsigned int i;
    cMessage::operator=(other);
    this->failure_var = other.failure_var;
    this->warning_var = other.warning_var;
    this->announceInterval_var = other.announceInterval_var;
    this->trackerId_var = other.trackerId_var;
    this->complete_var = other.complete_var;
    this->incomplete_var = other.incomplete_var;
    delete [] this->peers_var;
    this->peers_var = (other.peers_arraysize==0) ? NULL : new PEER[other.peers_arraysize];
    peers_arraysize = other.peers_arraysize;
    for (i=0; i<peers_arraysize; i++)
        this->peers_var[i] = other.peers_var[i];
    return *this;
}

void BTTrackerMsgResponse::netPack(cCommBuffer *b)
{
    cMessage::netPack(b);
    doPacking(b,this->failure_var);
    doPacking(b,this->warning_var);
    doPacking(b,this->announceInterval_var);
    doPacking(b,this->trackerId_var);
    doPacking(b,this->complete_var);
    doPacking(b,this->incomplete_var);
    b->pack(peers_arraysize);
    doPacking(b,this->peers_var,peers_arraysize);
}

void BTTrackerMsgResponse::netUnpack(cCommBuffer *b)
{
    cMessage::netUnpack(b);
    doUnpacking(b,this->failure_var);
    doUnpacking(b,this->warning_var);
    doUnpacking(b,this->announceInterval_var);
    doUnpacking(b,this->trackerId_var);
    doUnpacking(b,this->complete_var);
    doUnpacking(b,this->incomplete_var);
    delete [] this->peers_var;
    b->unpack(peers_arraysize);
    if (peers_arraysize==0) {
        this->peers_var = 0;
    } else {
        this->peers_var = new PEER[peers_arraysize];
        doUnpacking(b,this->peers_var,peers_arraysize);
    }
}

const char * BTTrackerMsgResponse::failure() const
{
    return failure_var.c_str();
}

void BTTrackerMsgResponse::setFailure(const char * failure_var)
{
    this->failure_var = failure_var;
}

const char * BTTrackerMsgResponse::warning() const
{
    return warning_var.c_str();
}

void BTTrackerMsgResponse::setWarning(const char * warning_var)
{
    this->warning_var = warning_var;
}

unsigned int BTTrackerMsgResponse::announceInterval() const
{
    return announceInterval_var;
}

void BTTrackerMsgResponse::setAnnounceInterval(unsigned int announceInterval_var)
{
    this->announceInterval_var = announceInterval_var;
}

const char * BTTrackerMsgResponse::trackerId() const
{
    return trackerId_var.c_str();
}

void BTTrackerMsgResponse::setTrackerId(const char * trackerId_var)
{
    this->trackerId_var = trackerId_var;
}

unsigned int BTTrackerMsgResponse::complete() const
{
    return complete_var;
}

void BTTrackerMsgResponse::setComplete(unsigned int complete_var)
{
    this->complete_var = complete_var;
}

unsigned int BTTrackerMsgResponse::incomplete() const
{
    return incomplete_var;
}

void BTTrackerMsgResponse::setIncomplete(unsigned int incomplete_var)
{
    this->incomplete_var = incomplete_var;
}

void BTTrackerMsgResponse::setPeersArraySize(unsigned int size)
{
    PEER *peers_var2 = (size==0) ? NULL : new PEER[size];
    unsigned int sz = peers_arraysize < size ? peers_arraysize : size;
    unsigned int i;
    for (i=0; i<sz; i++)
        peers_var2[i] = this->peers_var[i];
    peers_arraysize = size;
    delete [] this->peers_var;
    this->peers_var = peers_var2;
}

unsigned int BTTrackerMsgResponse::peersArraySize() const
{
    return peers_arraysize;
}

PEER& BTTrackerMsgResponse::peers(unsigned int k)
{
    if (k>=peers_arraysize) throw new cException("Array of size %d indexed by %d", peers_arraysize, k);
    return peers_var[k];
}

void BTTrackerMsgResponse::setPeers(unsigned int k, const PEER& peers_var)
{
    if (k>=peers_arraysize) throw new cException("Array of size %d indexed by %d", peers_arraysize, k);
    this->peers_var[k]=peers_var;
}

class BTTrackerMsgResponseDescriptor : public cStructDescriptor
{
  public:
    BTTrackerMsgResponseDescriptor();
    virtual ~BTTrackerMsgResponseDescriptor();
    BTTrackerMsgResponseDescriptor& operator=(const BTTrackerMsgResponseDescriptor& other);
    virtual cPolymorphic *dup() const {return new BTTrackerMsgResponseDescriptor(*this);}

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

Register_Class(BTTrackerMsgResponseDescriptor);

BTTrackerMsgResponseDescriptor::BTTrackerMsgResponseDescriptor() : cStructDescriptor("cMessage")
{
}

BTTrackerMsgResponseDescriptor::~BTTrackerMsgResponseDescriptor()
{
}

int BTTrackerMsgResponseDescriptor::getFieldCount()
{
    return baseclassdesc ? 7+baseclassdesc->getFieldCount() : 7;
}

int BTTrackerMsgResponseDescriptor::getFieldType(int field)
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
        case 6: return FT_STRUCT_ARRAY;
        default: return FT_INVALID;
    }
}

const char *BTTrackerMsgResponseDescriptor::getFieldName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "failure";
        case 1: return "warning";
        case 2: return "announceInterval";
        case 3: return "trackerId";
        case 4: return "complete";
        case 5: return "incomplete";
        case 6: return "peers";
        default: return NULL;
    }
}

const char *BTTrackerMsgResponseDescriptor::getFieldTypeString(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldTypeString(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 0: return "string";
        case 1: return "string";
        case 2: return "unsigned int";
        case 3: return "string";
        case 4: return "unsigned int";
        case 5: return "unsigned int";
        case 6: return "PEER";
        default: return NULL;
    }
}

const char *BTTrackerMsgResponseDescriptor::getFieldEnumName(int field)
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

int BTTrackerMsgResponseDescriptor::getArraySize(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getArraySize(field);
        field -= baseclassdesc->getFieldCount();
    }
    BTTrackerMsgResponse *pp = (BTTrackerMsgResponse *)p;
    switch (field) {
        case 6: return pp->peersArraySize();
        default: return 0;
    }
}

bool BTTrackerMsgResponseDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldAsString(field,i,resultbuf,bufsize);
        field -= baseclassdesc->getFieldCount();
    }
    BTTrackerMsgResponse *pp = (BTTrackerMsgResponse *)p;
    switch (field) {
        case 0: oppstring2string(pp->failure(),resultbuf,bufsize); return true;
        case 1: oppstring2string(pp->warning(),resultbuf,bufsize); return true;
        case 2: long2string(pp->announceInterval(),resultbuf,bufsize); return true;
        case 3: oppstring2string(pp->trackerId(),resultbuf,bufsize); return true;
        case 4: long2string(pp->complete(),resultbuf,bufsize); return true;
        case 5: long2string(pp->incomplete(),resultbuf,bufsize); return true;
        case 6: {std::stringstream out; out << pp->peers(i); opp_strprettytrunc(resultbuf,out.str().c_str(),bufsize-1); return true;}
        default: return false;
    }
}

bool BTTrackerMsgResponseDescriptor::setFieldAsString(int field, int i, const char *value)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->setFieldAsString(field,i,value);
        field -= baseclassdesc->getFieldCount();
    }
    BTTrackerMsgResponse *pp = (BTTrackerMsgResponse *)p;
    switch (field) {
        case 0: pp->setFailure((value)); return true;
        case 1: pp->setWarning((value)); return true;
        case 2: pp->setAnnounceInterval(string2long(value)); return true;
        case 3: pp->setTrackerId((value)); return true;
        case 4: pp->setComplete(string2long(value)); return true;
        case 5: pp->setIncomplete(string2long(value)); return true;
        case 6: return false;
        default: return false;
    }
}

const char *BTTrackerMsgResponseDescriptor::getFieldStructName(int field)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructName(field);
        field -= baseclassdesc->getFieldCount();
    }
    switch (field) {
        case 6: return "PEER"; break;
        default: return NULL;
    }
}

void *BTTrackerMsgResponseDescriptor::getFieldStructPointer(int field, int i)
{
    if (baseclassdesc) {
        if (field < baseclassdesc->getFieldCount())
            return baseclassdesc->getFieldStructPointer(field, i);
        field -= baseclassdesc->getFieldCount();
    }
    BTTrackerMsgResponse *pp = (BTTrackerMsgResponse *)p;
    switch (field) {
        case 6: return (void *)&pp->peers(i); break;
        default: return NULL;
    }
}

sFieldWrapper *BTTrackerMsgResponseDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

