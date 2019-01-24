//
// Generated file, do not edit! Created by opp_msgc 4.1 from applications/ddeclient/ClientMsg.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "ClientMsg_m.h"

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




Register_Class(ClientInternalMsg);

ClientInternalMsg::ClientInternalMsg(const char *name, int kind) : cMessage(name,kind)
{
    this->connId_var = -1;
    this->serverAddress_var = IPvXAddress("0.0.0.0");
    this->connectAddress_var = "";
    this->interfaceID_var = -1;
    this->ssid_var = "";
    this->connectToAp_var = true;
    this->connectToFileServer_var = false;
}

ClientInternalMsg::ClientInternalMsg(const ClientInternalMsg& other) : cMessage()
{
    setName(other.getName());
    operator=(other);
}

ClientInternalMsg::~ClientInternalMsg()
{
}

ClientInternalMsg& ClientInternalMsg::operator=(const ClientInternalMsg& other)
{
    if (this==&other) return *this;
    cMessage::operator=(other);
    this->connId_var = other.connId_var;
    this->serverAddress_var = other.serverAddress_var;
    this->connectAddress_var = other.connectAddress_var;
    this->interfaceID_var = other.interfaceID_var;
    this->ssid_var = other.ssid_var;
    this->connectToAp_var = other.connectToAp_var;
    this->connectToFileServer_var = other.connectToFileServer_var;
    this->macAddress_var = other.macAddress_var;
    this->newMAC_var = other.newMAC_var;
    return *this;
}

void ClientInternalMsg::parsimPack(cCommBuffer *b)
{
    cMessage::parsimPack(b);
    doPacking(b,this->connId_var);
    doPacking(b,this->serverAddress_var);
    doPacking(b,this->connectAddress_var);
    doPacking(b,this->interfaceID_var);
    doPacking(b,this->ssid_var);
    doPacking(b,this->connectToAp_var);
    doPacking(b,this->connectToFileServer_var);
    doPacking(b,this->macAddress_var);
    doPacking(b,this->newMAC_var);
}

void ClientInternalMsg::parsimUnpack(cCommBuffer *b)
{
    cMessage::parsimUnpack(b);
    doUnpacking(b,this->connId_var);
    doUnpacking(b,this->serverAddress_var);
    doUnpacking(b,this->connectAddress_var);
    doUnpacking(b,this->interfaceID_var);
    doUnpacking(b,this->ssid_var);
    doUnpacking(b,this->connectToAp_var);
    doUnpacking(b,this->connectToFileServer_var);
    doUnpacking(b,this->macAddress_var);
    doUnpacking(b,this->newMAC_var);
}

int ClientInternalMsg::getConnId() const
{
    return connId_var;
}

void ClientInternalMsg::setConnId(int connId_var)
{
    this->connId_var = connId_var;
}

IPvXAddress& ClientInternalMsg::getServerAddress()
{
    return serverAddress_var;
}

void ClientInternalMsg::setServerAddress(const IPvXAddress& serverAddress_var)
{
    this->serverAddress_var = serverAddress_var;
}

const char * ClientInternalMsg::getConnectAddress() const
{
    return connectAddress_var.c_str();
}

void ClientInternalMsg::setConnectAddress(const char * connectAddress_var)
{
    this->connectAddress_var = connectAddress_var;
}

int ClientInternalMsg::getInterfaceID() const
{
    return interfaceID_var;
}

void ClientInternalMsg::setInterfaceID(int interfaceID_var)
{
    this->interfaceID_var = interfaceID_var;
}

const char * ClientInternalMsg::getSsid() const
{
    return ssid_var.c_str();
}

void ClientInternalMsg::setSsid(const char * ssid_var)
{
    this->ssid_var = ssid_var;
}

bool ClientInternalMsg::getConnectToAp() const
{
    return connectToAp_var;
}

void ClientInternalMsg::setConnectToAp(bool connectToAp_var)
{
    this->connectToAp_var = connectToAp_var;
}

bool ClientInternalMsg::getConnectToFileServer() const
{
    return connectToFileServer_var;
}

void ClientInternalMsg::setConnectToFileServer(bool connectToFileServer_var)
{
    this->connectToFileServer_var = connectToFileServer_var;
}

MACAddress& ClientInternalMsg::getMacAddress()
{
    return macAddress_var;
}

void ClientInternalMsg::setMacAddress(const MACAddress& macAddress_var)
{
    this->macAddress_var = macAddress_var;
}

MACAddress& ClientInternalMsg::getNewMAC()
{
    return newMAC_var;
}

void ClientInternalMsg::setNewMAC(const MACAddress& newMAC_var)
{
    this->newMAC_var = newMAC_var;
}

class ClientInternalMsgDescriptor : public cClassDescriptor
{
  public:
    ClientInternalMsgDescriptor();
    virtual ~ClientInternalMsgDescriptor();

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

Register_ClassDescriptor(ClientInternalMsgDescriptor);

ClientInternalMsgDescriptor::ClientInternalMsgDescriptor() : cClassDescriptor("ClientInternalMsg", "cMessage")
{
}

ClientInternalMsgDescriptor::~ClientInternalMsgDescriptor()
{
}

bool ClientInternalMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ClientInternalMsg *>(obj)!=NULL;
}

const char *ClientInternalMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ClientInternalMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 9+basedesc->getFieldCount(object) : 9;
}

unsigned int ClientInternalMsgDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<9) ? fieldTypeFlags[field] : 0;
}

const char *ClientInternalMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "connId",
        "serverAddress",
        "connectAddress",
        "interfaceID",
        "ssid",
        "connectToAp",
        "connectToFileServer",
        "macAddress",
        "newMAC",
    };
    return (field>=0 && field<9) ? fieldNames[field] : NULL;
}

int ClientInternalMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='c' && strcmp(fieldName, "connId")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "serverAddress")==0) return base+1;
    if (fieldName[0]=='c' && strcmp(fieldName, "connectAddress")==0) return base+2;
    if (fieldName[0]=='i' && strcmp(fieldName, "interfaceID")==0) return base+3;
    if (fieldName[0]=='s' && strcmp(fieldName, "ssid")==0) return base+4;
    if (fieldName[0]=='c' && strcmp(fieldName, "connectToAp")==0) return base+5;
    if (fieldName[0]=='c' && strcmp(fieldName, "connectToFileServer")==0) return base+6;
    if (fieldName[0]=='m' && strcmp(fieldName, "macAddress")==0) return base+7;
    if (fieldName[0]=='n' && strcmp(fieldName, "newMAC")==0) return base+8;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ClientInternalMsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "IPvXAddress",
        "string",
        "int",
        "string",
        "bool",
        "bool",
        "MACAddress",
        "MACAddress",
    };
    return (field>=0 && field<9) ? fieldTypeStrings[field] : NULL;
}

const char *ClientInternalMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int ClientInternalMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ClientInternalMsg *pp = (ClientInternalMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ClientInternalMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ClientInternalMsg *pp = (ClientInternalMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getConnId());
        case 1: {std::stringstream out; out << pp->getServerAddress(); return out.str();}
        case 2: return oppstring2string(pp->getConnectAddress());
        case 3: return long2string(pp->getInterfaceID());
        case 4: return oppstring2string(pp->getSsid());
        case 5: return bool2string(pp->getConnectToAp());
        case 6: return bool2string(pp->getConnectToFileServer());
        case 7: {std::stringstream out; out << pp->getMacAddress(); return out.str();}
        case 8: {std::stringstream out; out << pp->getNewMAC(); return out.str();}
        default: return "";
    }
}

bool ClientInternalMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ClientInternalMsg *pp = (ClientInternalMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setConnId(string2long(value)); return true;
        case 2: pp->setConnectAddress((value)); return true;
        case 3: pp->setInterfaceID(string2long(value)); return true;
        case 4: pp->setSsid((value)); return true;
        case 5: pp->setConnectToAp(string2bool(value)); return true;
        case 6: pp->setConnectToFileServer(string2bool(value)); return true;
        default: return false;
    }
}

const char *ClientInternalMsgDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        "IPvXAddress",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        "MACAddress",
        "MACAddress",
    };
    return (field>=0 && field<9) ? fieldStructNames[field] : NULL;
}

void *ClientInternalMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ClientInternalMsg *pp = (ClientInternalMsg *)object; (void)pp;
    switch (field) {
        case 1: return (void *)(&pp->getServerAddress()); break;
        case 7: return (void *)(&pp->getMacAddress()); break;
        case 8: return (void *)(&pp->getNewMAC()); break;
        default: return NULL;
    }
}


