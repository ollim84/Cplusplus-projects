//
// Generated file, do not edit! Created by opp_msgc 4.1 from applications/dde/ddeMsg.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "ddeMsg_m.h"

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




Register_Class(ddeMsg);

ddeMsg::ddeMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    this->classifyOperation_var = -1;
    this->apClassifyValue_var = -1;
    this->nsClassifyValue_var = -1;
    this->mnClassifyValue_var = -1;
    this->seedsPerLeechers_var = -1;
    this->BW_requirement_var = -1;
    this->numUsers_var = -1;
    this->load_var = -1;
    this->packetLoss_var = -1;
    this->cachedContent_var = 0;
    this->hostID_var = 0;
    this->assocAP_var = 0;
    this->consumedBW_var = -1;
    this->signalStrength_var = -1;
    this->client_x_var = -1;
    this->client_y_var = -1;
    this->userClass_var = -1;
    this->HO_to_AP_SSID_var = 0;
    this->maxBW_var = -1;
    this->usedBW_var = -1;
    this->avBW_var = -1;
    this->rssiDBM_var = -1;
    this->sensitivity_var = -1;
    this->noiseLevelDBM_var = -1;
    this->numGold_var = -1;
    this->numSilver_var = -1;
    this->numBronze_var = -1;
    this->backgroundTraffic_var = 0;
}

ddeMsg::ddeMsg(const ddeMsg& other) : GenericAppMsg()
{
    setName(other.getName());
    operator=(other);
}

ddeMsg::~ddeMsg()
{
}

ddeMsg& ddeMsg::operator=(const ddeMsg& other)
{
    if (this==&other) return *this;
    GenericAppMsg::operator=(other);
    this->classifyOperation_var = other.classifyOperation_var;
    this->apClassifyValue_var = other.apClassifyValue_var;
    this->nsClassifyValue_var = other.nsClassifyValue_var;
    this->mnClassifyValue_var = other.mnClassifyValue_var;
    this->seedsPerLeechers_var = other.seedsPerLeechers_var;
    this->BW_requirement_var = other.BW_requirement_var;
    this->numUsers_var = other.numUsers_var;
    this->load_var = other.load_var;
    this->packetLoss_var = other.packetLoss_var;
    this->apList_var = other.apList_var;
    this->cachedContent_var = other.cachedContent_var;
    this->hostID_var = other.hostID_var;
    this->assocAP_var = other.assocAP_var;
    this->consumedBW_var = other.consumedBW_var;
    this->macAddress_var = other.macAddress_var;
    this->signalStrength_var = other.signalStrength_var;
    this->client_x_var = other.client_x_var;
    this->client_y_var = other.client_y_var;
    this->userClass_var = other.userClass_var;
    this->HO_to_AP_MAC_var = other.HO_to_AP_MAC_var;
    this->HO_to_AP_SSID_var = other.HO_to_AP_SSID_var;
    this->maxBW_var = other.maxBW_var;
    this->usedBW_var = other.usedBW_var;
    this->avBW_var = other.avBW_var;
    this->rssiDBM_var = other.rssiDBM_var;
    this->sensitivity_var = other.sensitivity_var;
    this->noiseLevelDBM_var = other.noiseLevelDBM_var;
    this->numGold_var = other.numGold_var;
    this->numSilver_var = other.numSilver_var;
    this->numBronze_var = other.numBronze_var;
    this->backgroundTraffic_var = other.backgroundTraffic_var;
    return *this;
}

void ddeMsg::parsimPack(cCommBuffer *b)
{
    GenericAppMsg::parsimPack(b);
    doPacking(b,this->classifyOperation_var);
    doPacking(b,this->apClassifyValue_var);
    doPacking(b,this->nsClassifyValue_var);
    doPacking(b,this->mnClassifyValue_var);
    doPacking(b,this->seedsPerLeechers_var);
    doPacking(b,this->BW_requirement_var);
    doPacking(b,this->numUsers_var);
    doPacking(b,this->load_var);
    doPacking(b,this->packetLoss_var);
    doPacking(b,this->apList_var);
    doPacking(b,this->cachedContent_var);
    doPacking(b,this->hostID_var);
    doPacking(b,this->assocAP_var);
    doPacking(b,this->consumedBW_var);
    doPacking(b,this->macAddress_var);
    doPacking(b,this->signalStrength_var);
    doPacking(b,this->client_x_var);
    doPacking(b,this->client_y_var);
    doPacking(b,this->userClass_var);
    doPacking(b,this->HO_to_AP_MAC_var);
    doPacking(b,this->HO_to_AP_SSID_var);
    doPacking(b,this->maxBW_var);
    doPacking(b,this->usedBW_var);
    doPacking(b,this->avBW_var);
    doPacking(b,this->rssiDBM_var);
    doPacking(b,this->sensitivity_var);
    doPacking(b,this->noiseLevelDBM_var);
    doPacking(b,this->numGold_var);
    doPacking(b,this->numSilver_var);
    doPacking(b,this->numBronze_var);
    doPacking(b,this->backgroundTraffic_var);
}

void ddeMsg::parsimUnpack(cCommBuffer *b)
{
    GenericAppMsg::parsimUnpack(b);
    doUnpacking(b,this->classifyOperation_var);
    doUnpacking(b,this->apClassifyValue_var);
    doUnpacking(b,this->nsClassifyValue_var);
    doUnpacking(b,this->mnClassifyValue_var);
    doUnpacking(b,this->seedsPerLeechers_var);
    doUnpacking(b,this->BW_requirement_var);
    doUnpacking(b,this->numUsers_var);
    doUnpacking(b,this->load_var);
    doUnpacking(b,this->packetLoss_var);
    doUnpacking(b,this->apList_var);
    doUnpacking(b,this->cachedContent_var);
    doUnpacking(b,this->hostID_var);
    doUnpacking(b,this->assocAP_var);
    doUnpacking(b,this->consumedBW_var);
    doUnpacking(b,this->macAddress_var);
    doUnpacking(b,this->signalStrength_var);
    doUnpacking(b,this->client_x_var);
    doUnpacking(b,this->client_y_var);
    doUnpacking(b,this->userClass_var);
    doUnpacking(b,this->HO_to_AP_MAC_var);
    doUnpacking(b,this->HO_to_AP_SSID_var);
    doUnpacking(b,this->maxBW_var);
    doUnpacking(b,this->usedBW_var);
    doUnpacking(b,this->avBW_var);
    doUnpacking(b,this->rssiDBM_var);
    doUnpacking(b,this->sensitivity_var);
    doUnpacking(b,this->noiseLevelDBM_var);
    doUnpacking(b,this->numGold_var);
    doUnpacking(b,this->numSilver_var);
    doUnpacking(b,this->numBronze_var);
    doUnpacking(b,this->backgroundTraffic_var);
}

int ddeMsg::getClassifyOperation() const
{
    return classifyOperation_var;
}

void ddeMsg::setClassifyOperation(int classifyOperation_var)
{
    this->classifyOperation_var = classifyOperation_var;
}

float ddeMsg::getApClassifyValue() const
{
    return apClassifyValue_var;
}

void ddeMsg::setApClassifyValue(float apClassifyValue_var)
{
    this->apClassifyValue_var = apClassifyValue_var;
}

float ddeMsg::getNsClassifyValue() const
{
    return nsClassifyValue_var;
}

void ddeMsg::setNsClassifyValue(float nsClassifyValue_var)
{
    this->nsClassifyValue_var = nsClassifyValue_var;
}

float ddeMsg::getMnClassifyValue() const
{
    return mnClassifyValue_var;
}

void ddeMsg::setMnClassifyValue(float mnClassifyValue_var)
{
    this->mnClassifyValue_var = mnClassifyValue_var;
}

float ddeMsg::getSeedsPerLeechers() const
{
    return seedsPerLeechers_var;
}

void ddeMsg::setSeedsPerLeechers(float seedsPerLeechers_var)
{
    this->seedsPerLeechers_var = seedsPerLeechers_var;
}

double ddeMsg::getBW_requirement() const
{
    return BW_requirement_var;
}

void ddeMsg::setBW_requirement(double BW_requirement_var)
{
    this->BW_requirement_var = BW_requirement_var;
}

int ddeMsg::getNumUsers() const
{
    return numUsers_var;
}

void ddeMsg::setNumUsers(int numUsers_var)
{
    this->numUsers_var = numUsers_var;
}

float ddeMsg::getLoad() const
{
    return load_var;
}

void ddeMsg::setLoad(float load_var)
{
    this->load_var = load_var;
}

float ddeMsg::getPacketLoss() const
{
    return packetLoss_var;
}

void ddeMsg::setPacketLoss(float packetLoss_var)
{
    this->packetLoss_var = packetLoss_var;
}

APList& ddeMsg::getApList()
{
    return apList_var;
}

void ddeMsg::setApList(const APList& apList_var)
{
    this->apList_var = apList_var;
}

bool ddeMsg::getCachedContent() const
{
    return cachedContent_var;
}

void ddeMsg::setCachedContent(bool cachedContent_var)
{
    this->cachedContent_var = cachedContent_var;
}

const char * ddeMsg::getHostID() const
{
    return hostID_var.c_str();
}

void ddeMsg::setHostID(const char * hostID_var)
{
    this->hostID_var = hostID_var;
}

const char * ddeMsg::getAssocAP() const
{
    return assocAP_var.c_str();
}

void ddeMsg::setAssocAP(const char * assocAP_var)
{
    this->assocAP_var = assocAP_var;
}

double ddeMsg::getConsumedBW() const
{
    return consumedBW_var;
}

void ddeMsg::setConsumedBW(double consumedBW_var)
{
    this->consumedBW_var = consumedBW_var;
}

MACAddress& ddeMsg::getMacAddress()
{
    return macAddress_var;
}

void ddeMsg::setMacAddress(const MACAddress& macAddress_var)
{
    this->macAddress_var = macAddress_var;
}

float ddeMsg::getSignalStrength() const
{
    return signalStrength_var;
}

void ddeMsg::setSignalStrength(float signalStrength_var)
{
    this->signalStrength_var = signalStrength_var;
}

double ddeMsg::getClient_x() const
{
    return client_x_var;
}

void ddeMsg::setClient_x(double client_x_var)
{
    this->client_x_var = client_x_var;
}

double ddeMsg::getClient_y() const
{
    return client_y_var;
}

void ddeMsg::setClient_y(double client_y_var)
{
    this->client_y_var = client_y_var;
}

int ddeMsg::getUserClass() const
{
    return userClass_var;
}

void ddeMsg::setUserClass(int userClass_var)
{
    this->userClass_var = userClass_var;
}

MACAddress& ddeMsg::getHO_to_AP_MAC()
{
    return HO_to_AP_MAC_var;
}

void ddeMsg::setHO_to_AP_MAC(const MACAddress& HO_to_AP_MAC_var)
{
    this->HO_to_AP_MAC_var = HO_to_AP_MAC_var;
}

const char * ddeMsg::getHO_to_AP_SSID() const
{
    return HO_to_AP_SSID_var.c_str();
}

void ddeMsg::setHO_to_AP_SSID(const char * HO_to_AP_SSID_var)
{
    this->HO_to_AP_SSID_var = HO_to_AP_SSID_var;
}

double ddeMsg::getMaxBW() const
{
    return maxBW_var;
}

void ddeMsg::setMaxBW(double maxBW_var)
{
    this->maxBW_var = maxBW_var;
}

double ddeMsg::getUsedBW() const
{
    return usedBW_var;
}

void ddeMsg::setUsedBW(double usedBW_var)
{
    this->usedBW_var = usedBW_var;
}

double ddeMsg::getAvBW() const
{
    return avBW_var;
}

void ddeMsg::setAvBW(double avBW_var)
{
    this->avBW_var = avBW_var;
}

float ddeMsg::getRssiDBM() const
{
    return rssiDBM_var;
}

void ddeMsg::setRssiDBM(float rssiDBM_var)
{
    this->rssiDBM_var = rssiDBM_var;
}

float ddeMsg::getSensitivity() const
{
    return sensitivity_var;
}

void ddeMsg::setSensitivity(float sensitivity_var)
{
    this->sensitivity_var = sensitivity_var;
}

float ddeMsg::getNoiseLevelDBM() const
{
    return noiseLevelDBM_var;
}

void ddeMsg::setNoiseLevelDBM(float noiseLevelDBM_var)
{
    this->noiseLevelDBM_var = noiseLevelDBM_var;
}

int ddeMsg::getNumGold() const
{
    return numGold_var;
}

void ddeMsg::setNumGold(int numGold_var)
{
    this->numGold_var = numGold_var;
}

int ddeMsg::getNumSilver() const
{
    return numSilver_var;
}

void ddeMsg::setNumSilver(int numSilver_var)
{
    this->numSilver_var = numSilver_var;
}

int ddeMsg::getNumBronze() const
{
    return numBronze_var;
}

void ddeMsg::setNumBronze(int numBronze_var)
{
    this->numBronze_var = numBronze_var;
}

bool ddeMsg::getBackgroundTraffic() const
{
    return backgroundTraffic_var;
}

void ddeMsg::setBackgroundTraffic(bool backgroundTraffic_var)
{
    this->backgroundTraffic_var = backgroundTraffic_var;
}

class ddeMsgDescriptor : public cClassDescriptor
{
  public:
    ddeMsgDescriptor();
    virtual ~ddeMsgDescriptor();

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

Register_ClassDescriptor(ddeMsgDescriptor);

ddeMsgDescriptor::ddeMsgDescriptor() : cClassDescriptor("ddeMsg", "GenericAppMsg")
{
}

ddeMsgDescriptor::~ddeMsgDescriptor()
{
}

bool ddeMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ddeMsg *>(obj)!=NULL;
}

const char *ddeMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ddeMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 31+basedesc->getFieldCount(object) : 31;
}

unsigned int ddeMsgDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
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
    return (field>=0 && field<31) ? fieldTypeFlags[field] : 0;
}

const char *ddeMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "classifyOperation",
        "apClassifyValue",
        "nsClassifyValue",
        "mnClassifyValue",
        "seedsPerLeechers",
        "BW_requirement",
        "numUsers",
        "load",
        "packetLoss",
        "apList",
        "cachedContent",
        "hostID",
        "assocAP",
        "consumedBW",
        "macAddress",
        "signalStrength",
        "client_x",
        "client_y",
        "userClass",
        "HO_to_AP_MAC",
        "HO_to_AP_SSID",
        "maxBW",
        "usedBW",
        "avBW",
        "rssiDBM",
        "sensitivity",
        "noiseLevelDBM",
        "numGold",
        "numSilver",
        "numBronze",
        "backgroundTraffic",
    };
    return (field>=0 && field<31) ? fieldNames[field] : NULL;
}

int ddeMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='c' && strcmp(fieldName, "classifyOperation")==0) return base+0;
    if (fieldName[0]=='a' && strcmp(fieldName, "apClassifyValue")==0) return base+1;
    if (fieldName[0]=='n' && strcmp(fieldName, "nsClassifyValue")==0) return base+2;
    if (fieldName[0]=='m' && strcmp(fieldName, "mnClassifyValue")==0) return base+3;
    if (fieldName[0]=='s' && strcmp(fieldName, "seedsPerLeechers")==0) return base+4;
    if (fieldName[0]=='B' && strcmp(fieldName, "BW_requirement")==0) return base+5;
    if (fieldName[0]=='n' && strcmp(fieldName, "numUsers")==0) return base+6;
    if (fieldName[0]=='l' && strcmp(fieldName, "load")==0) return base+7;
    if (fieldName[0]=='p' && strcmp(fieldName, "packetLoss")==0) return base+8;
    if (fieldName[0]=='a' && strcmp(fieldName, "apList")==0) return base+9;
    if (fieldName[0]=='c' && strcmp(fieldName, "cachedContent")==0) return base+10;
    if (fieldName[0]=='h' && strcmp(fieldName, "hostID")==0) return base+11;
    if (fieldName[0]=='a' && strcmp(fieldName, "assocAP")==0) return base+12;
    if (fieldName[0]=='c' && strcmp(fieldName, "consumedBW")==0) return base+13;
    if (fieldName[0]=='m' && strcmp(fieldName, "macAddress")==0) return base+14;
    if (fieldName[0]=='s' && strcmp(fieldName, "signalStrength")==0) return base+15;
    if (fieldName[0]=='c' && strcmp(fieldName, "client_x")==0) return base+16;
    if (fieldName[0]=='c' && strcmp(fieldName, "client_y")==0) return base+17;
    if (fieldName[0]=='u' && strcmp(fieldName, "userClass")==0) return base+18;
    if (fieldName[0]=='H' && strcmp(fieldName, "HO_to_AP_MAC")==0) return base+19;
    if (fieldName[0]=='H' && strcmp(fieldName, "HO_to_AP_SSID")==0) return base+20;
    if (fieldName[0]=='m' && strcmp(fieldName, "maxBW")==0) return base+21;
    if (fieldName[0]=='u' && strcmp(fieldName, "usedBW")==0) return base+22;
    if (fieldName[0]=='a' && strcmp(fieldName, "avBW")==0) return base+23;
    if (fieldName[0]=='r' && strcmp(fieldName, "rssiDBM")==0) return base+24;
    if (fieldName[0]=='s' && strcmp(fieldName, "sensitivity")==0) return base+25;
    if (fieldName[0]=='n' && strcmp(fieldName, "noiseLevelDBM")==0) return base+26;
    if (fieldName[0]=='n' && strcmp(fieldName, "numGold")==0) return base+27;
    if (fieldName[0]=='n' && strcmp(fieldName, "numSilver")==0) return base+28;
    if (fieldName[0]=='n' && strcmp(fieldName, "numBronze")==0) return base+29;
    if (fieldName[0]=='b' && strcmp(fieldName, "backgroundTraffic")==0) return base+30;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ddeMsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "float",
        "float",
        "float",
        "float",
        "double",
        "int",
        "float",
        "float",
        "APList",
        "bool",
        "string",
        "string",
        "double",
        "MACAddress",
        "float",
        "double",
        "double",
        "int",
        "MACAddress",
        "string",
        "double",
        "double",
        "double",
        "float",
        "float",
        "float",
        "int",
        "int",
        "int",
        "bool",
    };
    return (field>=0 && field<31) ? fieldTypeStrings[field] : NULL;
}

const char *ddeMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int ddeMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ddeMsg *pp = (ddeMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ddeMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ddeMsg *pp = (ddeMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getClassifyOperation());
        case 1: return double2string(pp->getApClassifyValue());
        case 2: return double2string(pp->getNsClassifyValue());
        case 3: return double2string(pp->getMnClassifyValue());
        case 4: return double2string(pp->getSeedsPerLeechers());
        case 5: return double2string(pp->getBW_requirement());
        case 6: return long2string(pp->getNumUsers());
        case 7: return double2string(pp->getLoad());
        case 8: return double2string(pp->getPacketLoss());
        case 9: {std::stringstream out; out << pp->getApList(); return out.str();}
        case 10: return bool2string(pp->getCachedContent());
        case 11: return oppstring2string(pp->getHostID());
        case 12: return oppstring2string(pp->getAssocAP());
        case 13: return double2string(pp->getConsumedBW());
        case 14: {std::stringstream out; out << pp->getMacAddress(); return out.str();}
        case 15: return double2string(pp->getSignalStrength());
        case 16: return double2string(pp->getClient_x());
        case 17: return double2string(pp->getClient_y());
        case 18: return long2string(pp->getUserClass());
        case 19: {std::stringstream out; out << pp->getHO_to_AP_MAC(); return out.str();}
        case 20: return oppstring2string(pp->getHO_to_AP_SSID());
        case 21: return double2string(pp->getMaxBW());
        case 22: return double2string(pp->getUsedBW());
        case 23: return double2string(pp->getAvBW());
        case 24: return double2string(pp->getRssiDBM());
        case 25: return double2string(pp->getSensitivity());
        case 26: return double2string(pp->getNoiseLevelDBM());
        case 27: return long2string(pp->getNumGold());
        case 28: return long2string(pp->getNumSilver());
        case 29: return long2string(pp->getNumBronze());
        case 30: return bool2string(pp->getBackgroundTraffic());
        default: return "";
    }
}

bool ddeMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ddeMsg *pp = (ddeMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setClassifyOperation(string2long(value)); return true;
        case 1: pp->setApClassifyValue(string2double(value)); return true;
        case 2: pp->setNsClassifyValue(string2double(value)); return true;
        case 3: pp->setMnClassifyValue(string2double(value)); return true;
        case 4: pp->setSeedsPerLeechers(string2double(value)); return true;
        case 5: pp->setBW_requirement(string2double(value)); return true;
        case 6: pp->setNumUsers(string2long(value)); return true;
        case 7: pp->setLoad(string2double(value)); return true;
        case 8: pp->setPacketLoss(string2double(value)); return true;
        case 10: pp->setCachedContent(string2bool(value)); return true;
        case 11: pp->setHostID((value)); return true;
        case 12: pp->setAssocAP((value)); return true;
        case 13: pp->setConsumedBW(string2double(value)); return true;
        case 15: pp->setSignalStrength(string2double(value)); return true;
        case 16: pp->setClient_x(string2double(value)); return true;
        case 17: pp->setClient_y(string2double(value)); return true;
        case 18: pp->setUserClass(string2long(value)); return true;
        case 20: pp->setHO_to_AP_SSID((value)); return true;
        case 21: pp->setMaxBW(string2double(value)); return true;
        case 22: pp->setUsedBW(string2double(value)); return true;
        case 23: pp->setAvBW(string2double(value)); return true;
        case 24: pp->setRssiDBM(string2double(value)); return true;
        case 25: pp->setSensitivity(string2double(value)); return true;
        case 26: pp->setNoiseLevelDBM(string2double(value)); return true;
        case 27: pp->setNumGold(string2long(value)); return true;
        case 28: pp->setNumSilver(string2long(value)); return true;
        case 29: pp->setNumBronze(string2long(value)); return true;
        case 30: pp->setBackgroundTraffic(string2bool(value)); return true;
        default: return false;
    }
}

const char *ddeMsgDescriptor::getFieldStructName(void *object, int field) const
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
        "APList",
        NULL,
        NULL,
        NULL,
        NULL,
        "MACAddress",
        NULL,
        NULL,
        NULL,
        NULL,
        "MACAddress",
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
    return (field>=0 && field<31) ? fieldStructNames[field] : NULL;
}

void *ddeMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ddeMsg *pp = (ddeMsg *)object; (void)pp;
    switch (field) {
        case 9: return (void *)(&pp->getApList()); break;
        case 14: return (void *)(&pp->getMacAddress()); break;
        case 19: return (void *)(&pp->getHO_to_AP_MAC()); break;
        default: return NULL;
    }
}

Register_Class(ddeInternalMsg);

ddeInternalMsg::ddeInternalMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    this->apIndex_var = -1;
    this->clientAddr_var = IPvXAddress("0.0.0.0");
    this->cachedContent_var = 0;
}

ddeInternalMsg::ddeInternalMsg(const ddeInternalMsg& other) : GenericAppMsg()
{
    setName(other.getName());
    operator=(other);
}

ddeInternalMsg::~ddeInternalMsg()
{
}

ddeInternalMsg& ddeInternalMsg::operator=(const ddeInternalMsg& other)
{
    if (this==&other) return *this;
    GenericAppMsg::operator=(other);
    this->apIndex_var = other.apIndex_var;
    this->apList_var = other.apList_var;
    this->clientAddr_var = other.clientAddr_var;
    this->cachedContent_var = other.cachedContent_var;
    return *this;
}

void ddeInternalMsg::parsimPack(cCommBuffer *b)
{
    GenericAppMsg::parsimPack(b);
    doPacking(b,this->apIndex_var);
    doPacking(b,this->apList_var);
    doPacking(b,this->clientAddr_var);
    doPacking(b,this->cachedContent_var);
}

void ddeInternalMsg::parsimUnpack(cCommBuffer *b)
{
    GenericAppMsg::parsimUnpack(b);
    doUnpacking(b,this->apIndex_var);
    doUnpacking(b,this->apList_var);
    doUnpacking(b,this->clientAddr_var);
    doUnpacking(b,this->cachedContent_var);
}

int ddeInternalMsg::getApIndex() const
{
    return apIndex_var;
}

void ddeInternalMsg::setApIndex(int apIndex_var)
{
    this->apIndex_var = apIndex_var;
}

APList& ddeInternalMsg::getApList()
{
    return apList_var;
}

void ddeInternalMsg::setApList(const APList& apList_var)
{
    this->apList_var = apList_var;
}

IPvXAddress& ddeInternalMsg::getClientAddr()
{
    return clientAddr_var;
}

void ddeInternalMsg::setClientAddr(const IPvXAddress& clientAddr_var)
{
    this->clientAddr_var = clientAddr_var;
}

bool ddeInternalMsg::getCachedContent() const
{
    return cachedContent_var;
}

void ddeInternalMsg::setCachedContent(bool cachedContent_var)
{
    this->cachedContent_var = cachedContent_var;
}

class ddeInternalMsgDescriptor : public cClassDescriptor
{
  public:
    ddeInternalMsgDescriptor();
    virtual ~ddeInternalMsgDescriptor();

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

Register_ClassDescriptor(ddeInternalMsgDescriptor);

ddeInternalMsgDescriptor::ddeInternalMsgDescriptor() : cClassDescriptor("ddeInternalMsg", "GenericAppMsg")
{
}

ddeInternalMsgDescriptor::~ddeInternalMsgDescriptor()
{
}

bool ddeInternalMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ddeInternalMsg *>(obj)!=NULL;
}

const char *ddeInternalMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ddeInternalMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int ddeInternalMsgDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *ddeInternalMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "apIndex",
        "apList",
        "clientAddr",
        "cachedContent",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int ddeInternalMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='a' && strcmp(fieldName, "apIndex")==0) return base+0;
    if (fieldName[0]=='a' && strcmp(fieldName, "apList")==0) return base+1;
    if (fieldName[0]=='c' && strcmp(fieldName, "clientAddr")==0) return base+2;
    if (fieldName[0]=='c' && strcmp(fieldName, "cachedContent")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ddeInternalMsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "APList",
        "IPvXAddress",
        "bool",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *ddeInternalMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int ddeInternalMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ddeInternalMsg *pp = (ddeInternalMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ddeInternalMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ddeInternalMsg *pp = (ddeInternalMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getApIndex());
        case 1: {std::stringstream out; out << pp->getApList(); return out.str();}
        case 2: {std::stringstream out; out << pp->getClientAddr(); return out.str();}
        case 3: return bool2string(pp->getCachedContent());
        default: return "";
    }
}

bool ddeInternalMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ddeInternalMsg *pp = (ddeInternalMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setApIndex(string2long(value)); return true;
        case 3: pp->setCachedContent(string2bool(value)); return true;
        default: return false;
    }
}

const char *ddeInternalMsgDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        NULL,
        "APList",
        "IPvXAddress",
        NULL,
    };
    return (field>=0 && field<4) ? fieldStructNames[field] : NULL;
}

void *ddeInternalMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ddeInternalMsg *pp = (ddeInternalMsg *)object; (void)pp;
    switch (field) {
        case 1: return (void *)(&pp->getApList()); break;
        case 2: return (void *)(&pp->getClientAddr()); break;
        default: return NULL;
    }
}


