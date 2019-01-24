//
// Generated file, do not edit! Created by opp_msgc 4.1 from applications/supercom/SuperComMsg.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "SuperComMsg_m.h"

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




Register_Class(SuperComMsg);

SuperComMsg::SuperComMsg(const char *name, int kind) : GenericAppMsg(name,kind)
{
    this->inputData_var = -1;
    this->outputData_var = -1;
    this->cores_var = -1;
    this->nodes_var = -1;
    this->memory_var = -1;
    this->load_var = -1;
    this->runTime_var = -1;
    this->serverNumber_var = -1;
    this->type_var = -1;
    this->networkLoad_var = 0.0;
    this->loadedTime_var = 1.0;
    this->priority_var = 1;
    this->sleepMode_var = -1;
    this->timeOfStart_var = -1;
    this->clientAddress_var = IPvXAddress("0.0.0.0");
    this->usoAddress_var = IPvXAddress("0.0.0.0");
    this->rmsAddress_var = IPvXAddress("0.0.0.0");
    this->jobID_var = -1;
    this->submitTime_var = -1;
    this->energyAware_var = false;
}

SuperComMsg::SuperComMsg(const SuperComMsg& other) : GenericAppMsg()
{
    setName(other.getName());
    operator=(other);
}

SuperComMsg::~SuperComMsg()
{
}

SuperComMsg& SuperComMsg::operator=(const SuperComMsg& other)
{
    if (this==&other) return *this;
    GenericAppMsg::operator=(other);
    this->inputData_var = other.inputData_var;
    this->outputData_var = other.outputData_var;
    this->cores_var = other.cores_var;
    this->nodes_var = other.nodes_var;
    this->memory_var = other.memory_var;
    this->load_var = other.load_var;
    this->runTime_var = other.runTime_var;
    this->serverNumber_var = other.serverNumber_var;
    this->type_var = other.type_var;
    this->networkLoad_var = other.networkLoad_var;
    this->loadedTime_var = other.loadedTime_var;
    this->priority_var = other.priority_var;
    this->sleepMode_var = other.sleepMode_var;
    this->timeOfStart_var = other.timeOfStart_var;
    this->clientAddress_var = other.clientAddress_var;
    this->usoAddress_var = other.usoAddress_var;
    this->rmsAddress_var = other.rmsAddress_var;
    this->jobID_var = other.jobID_var;
    this->submitTime_var = other.submitTime_var;
    this->energyAware_var = other.energyAware_var;
    return *this;
}

void SuperComMsg::parsimPack(cCommBuffer *b)
{
    GenericAppMsg::parsimPack(b);
    doPacking(b,this->inputData_var);
    doPacking(b,this->outputData_var);
    doPacking(b,this->cores_var);
    doPacking(b,this->nodes_var);
    doPacking(b,this->memory_var);
    doPacking(b,this->load_var);
    doPacking(b,this->runTime_var);
    doPacking(b,this->serverNumber_var);
    doPacking(b,this->type_var);
    doPacking(b,this->networkLoad_var);
    doPacking(b,this->loadedTime_var);
    doPacking(b,this->priority_var);
    doPacking(b,this->sleepMode_var);
    doPacking(b,this->timeOfStart_var);
    doPacking(b,this->clientAddress_var);
    doPacking(b,this->usoAddress_var);
    doPacking(b,this->rmsAddress_var);
    doPacking(b,this->jobID_var);
    doPacking(b,this->submitTime_var);
    doPacking(b,this->energyAware_var);
}

void SuperComMsg::parsimUnpack(cCommBuffer *b)
{
    GenericAppMsg::parsimUnpack(b);
    doUnpacking(b,this->inputData_var);
    doUnpacking(b,this->outputData_var);
    doUnpacking(b,this->cores_var);
    doUnpacking(b,this->nodes_var);
    doUnpacking(b,this->memory_var);
    doUnpacking(b,this->load_var);
    doUnpacking(b,this->runTime_var);
    doUnpacking(b,this->serverNumber_var);
    doUnpacking(b,this->type_var);
    doUnpacking(b,this->networkLoad_var);
    doUnpacking(b,this->loadedTime_var);
    doUnpacking(b,this->priority_var);
    doUnpacking(b,this->sleepMode_var);
    doUnpacking(b,this->timeOfStart_var);
    doUnpacking(b,this->clientAddress_var);
    doUnpacking(b,this->usoAddress_var);
    doUnpacking(b,this->rmsAddress_var);
    doUnpacking(b,this->jobID_var);
    doUnpacking(b,this->submitTime_var);
    doUnpacking(b,this->energyAware_var);
}

long SuperComMsg::getInputData() const
{
    return inputData_var;
}

void SuperComMsg::setInputData(long inputData_var)
{
    this->inputData_var = inputData_var;
}

long SuperComMsg::getOutputData() const
{
    return outputData_var;
}

void SuperComMsg::setOutputData(long outputData_var)
{
    this->outputData_var = outputData_var;
}

int SuperComMsg::getCores() const
{
    return cores_var;
}

void SuperComMsg::setCores(int cores_var)
{
    this->cores_var = cores_var;
}

int SuperComMsg::getNodes() const
{
    return nodes_var;
}

void SuperComMsg::setNodes(int nodes_var)
{
    this->nodes_var = nodes_var;
}

int SuperComMsg::getMemory() const
{
    return memory_var;
}

void SuperComMsg::setMemory(int memory_var)
{
    this->memory_var = memory_var;
}

int SuperComMsg::getLoad() const
{
    return load_var;
}

void SuperComMsg::setLoad(int load_var)
{
    this->load_var = load_var;
}

double SuperComMsg::getRunTime() const
{
    return runTime_var;
}

void SuperComMsg::setRunTime(double runTime_var)
{
    this->runTime_var = runTime_var;
}

int SuperComMsg::getServerNumber() const
{
    return serverNumber_var;
}

void SuperComMsg::setServerNumber(int serverNumber_var)
{
    this->serverNumber_var = serverNumber_var;
}

int SuperComMsg::getType() const
{
    return type_var;
}

void SuperComMsg::setType(int type_var)
{
    this->type_var = type_var;
}

double SuperComMsg::getNetworkLoad() const
{
    return networkLoad_var;
}

void SuperComMsg::setNetworkLoad(double networkLoad_var)
{
    this->networkLoad_var = networkLoad_var;
}

double SuperComMsg::getLoadedTime() const
{
    return loadedTime_var;
}

void SuperComMsg::setLoadedTime(double loadedTime_var)
{
    this->loadedTime_var = loadedTime_var;
}

int SuperComMsg::getPriority() const
{
    return priority_var;
}

void SuperComMsg::setPriority(int priority_var)
{
    this->priority_var = priority_var;
}

int SuperComMsg::getSleepMode() const
{
    return sleepMode_var;
}

void SuperComMsg::setSleepMode(int sleepMode_var)
{
    this->sleepMode_var = sleepMode_var;
}

simtime_t SuperComMsg::getTimeOfStart() const
{
    return timeOfStart_var;
}

void SuperComMsg::setTimeOfStart(simtime_t timeOfStart_var)
{
    this->timeOfStart_var = timeOfStart_var;
}

IPvXAddress& SuperComMsg::getClientAddress()
{
    return clientAddress_var;
}

void SuperComMsg::setClientAddress(const IPvXAddress& clientAddress_var)
{
    this->clientAddress_var = clientAddress_var;
}

IPvXAddress& SuperComMsg::getUsoAddress()
{
    return usoAddress_var;
}

void SuperComMsg::setUsoAddress(const IPvXAddress& usoAddress_var)
{
    this->usoAddress_var = usoAddress_var;
}

IPvXAddress& SuperComMsg::getRmsAddress()
{
    return rmsAddress_var;
}

void SuperComMsg::setRmsAddress(const IPvXAddress& rmsAddress_var)
{
    this->rmsAddress_var = rmsAddress_var;
}

int SuperComMsg::getJobID() const
{
    return jobID_var;
}

void SuperComMsg::setJobID(int jobID_var)
{
    this->jobID_var = jobID_var;
}

int SuperComMsg::getSubmitTime() const
{
    return submitTime_var;
}

void SuperComMsg::setSubmitTime(int submitTime_var)
{
    this->submitTime_var = submitTime_var;
}

bool SuperComMsg::getEnergyAware() const
{
    return energyAware_var;
}

void SuperComMsg::setEnergyAware(bool energyAware_var)
{
    this->energyAware_var = energyAware_var;
}

class SuperComMsgDescriptor : public cClassDescriptor
{
  public:
    SuperComMsgDescriptor();
    virtual ~SuperComMsgDescriptor();

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

Register_ClassDescriptor(SuperComMsgDescriptor);

SuperComMsgDescriptor::SuperComMsgDescriptor() : cClassDescriptor("SuperComMsg", "GenericAppMsg")
{
}

SuperComMsgDescriptor::~SuperComMsgDescriptor()
{
}

bool SuperComMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<SuperComMsg *>(obj)!=NULL;
}

const char *SuperComMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int SuperComMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 20+basedesc->getFieldCount(object) : 20;
}

unsigned int SuperComMsgDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<20) ? fieldTypeFlags[field] : 0;
}

const char *SuperComMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "inputData",
        "outputData",
        "cores",
        "nodes",
        "memory",
        "load",
        "runTime",
        "serverNumber",
        "type",
        "networkLoad",
        "loadedTime",
        "priority",
        "sleepMode",
        "timeOfStart",
        "clientAddress",
        "usoAddress",
        "rmsAddress",
        "jobID",
        "submitTime",
        "energyAware",
    };
    return (field>=0 && field<20) ? fieldNames[field] : NULL;
}

int SuperComMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='i' && strcmp(fieldName, "inputData")==0) return base+0;
    if (fieldName[0]=='o' && strcmp(fieldName, "outputData")==0) return base+1;
    if (fieldName[0]=='c' && strcmp(fieldName, "cores")==0) return base+2;
    if (fieldName[0]=='n' && strcmp(fieldName, "nodes")==0) return base+3;
    if (fieldName[0]=='m' && strcmp(fieldName, "memory")==0) return base+4;
    if (fieldName[0]=='l' && strcmp(fieldName, "load")==0) return base+5;
    if (fieldName[0]=='r' && strcmp(fieldName, "runTime")==0) return base+6;
    if (fieldName[0]=='s' && strcmp(fieldName, "serverNumber")==0) return base+7;
    if (fieldName[0]=='t' && strcmp(fieldName, "type")==0) return base+8;
    if (fieldName[0]=='n' && strcmp(fieldName, "networkLoad")==0) return base+9;
    if (fieldName[0]=='l' && strcmp(fieldName, "loadedTime")==0) return base+10;
    if (fieldName[0]=='p' && strcmp(fieldName, "priority")==0) return base+11;
    if (fieldName[0]=='s' && strcmp(fieldName, "sleepMode")==0) return base+12;
    if (fieldName[0]=='t' && strcmp(fieldName, "timeOfStart")==0) return base+13;
    if (fieldName[0]=='c' && strcmp(fieldName, "clientAddress")==0) return base+14;
    if (fieldName[0]=='u' && strcmp(fieldName, "usoAddress")==0) return base+15;
    if (fieldName[0]=='r' && strcmp(fieldName, "rmsAddress")==0) return base+16;
    if (fieldName[0]=='j' && strcmp(fieldName, "jobID")==0) return base+17;
    if (fieldName[0]=='s' && strcmp(fieldName, "submitTime")==0) return base+18;
    if (fieldName[0]=='e' && strcmp(fieldName, "energyAware")==0) return base+19;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *SuperComMsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "long",
        "long",
        "int",
        "int",
        "int",
        "int",
        "double",
        "int",
        "int",
        "double",
        "double",
        "int",
        "int",
        "simtime_t",
        "IPvXAddress",
        "IPvXAddress",
        "IPvXAddress",
        "int",
        "int",
        "bool",
    };
    return (field>=0 && field<20) ? fieldTypeStrings[field] : NULL;
}

const char *SuperComMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int SuperComMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    SuperComMsg *pp = (SuperComMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string SuperComMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    SuperComMsg *pp = (SuperComMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getInputData());
        case 1: return long2string(pp->getOutputData());
        case 2: return long2string(pp->getCores());
        case 3: return long2string(pp->getNodes());
        case 4: return long2string(pp->getMemory());
        case 5: return long2string(pp->getLoad());
        case 6: return double2string(pp->getRunTime());
        case 7: return long2string(pp->getServerNumber());
        case 8: return long2string(pp->getType());
        case 9: return double2string(pp->getNetworkLoad());
        case 10: return double2string(pp->getLoadedTime());
        case 11: return long2string(pp->getPriority());
        case 12: return long2string(pp->getSleepMode());
        case 13: return double2string(pp->getTimeOfStart());
        case 14: {std::stringstream out; out << pp->getClientAddress(); return out.str();}
        case 15: {std::stringstream out; out << pp->getUsoAddress(); return out.str();}
        case 16: {std::stringstream out; out << pp->getRmsAddress(); return out.str();}
        case 17: return long2string(pp->getJobID());
        case 18: return long2string(pp->getSubmitTime());
        case 19: return bool2string(pp->getEnergyAware());
        default: return "";
    }
}

bool SuperComMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    SuperComMsg *pp = (SuperComMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setInputData(string2long(value)); return true;
        case 1: pp->setOutputData(string2long(value)); return true;
        case 2: pp->setCores(string2long(value)); return true;
        case 3: pp->setNodes(string2long(value)); return true;
        case 4: pp->setMemory(string2long(value)); return true;
        case 5: pp->setLoad(string2long(value)); return true;
        case 6: pp->setRunTime(string2double(value)); return true;
        case 7: pp->setServerNumber(string2long(value)); return true;
        case 8: pp->setType(string2long(value)); return true;
        case 9: pp->setNetworkLoad(string2double(value)); return true;
        case 10: pp->setLoadedTime(string2double(value)); return true;
        case 11: pp->setPriority(string2long(value)); return true;
        case 12: pp->setSleepMode(string2long(value)); return true;
        case 13: pp->setTimeOfStart(string2double(value)); return true;
        case 17: pp->setJobID(string2long(value)); return true;
        case 18: pp->setSubmitTime(string2long(value)); return true;
        case 19: pp->setEnergyAware(string2bool(value)); return true;
        default: return false;
    }
}

const char *SuperComMsgDescriptor::getFieldStructName(void *object, int field) const
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
        NULL,
        NULL,
        "IPvXAddress",
        "IPvXAddress",
        "IPvXAddress",
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<20) ? fieldStructNames[field] : NULL;
}

void *SuperComMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    SuperComMsg *pp = (SuperComMsg *)object; (void)pp;
    switch (field) {
        case 14: return (void *)(&pp->getClientAddress()); break;
        case 15: return (void *)(&pp->getUsoAddress()); break;
        case 16: return (void *)(&pp->getRmsAddress()); break;
        default: return NULL;
    }
}

Register_Class(SuperComInternalMsg);

SuperComInternalMsg::SuperComInternalMsg(const char *name, int kind) : cMessage(name,kind)
{
    this->serverIndex_var = -1;
    this->runTime_var = -1;
    this->connId_var = -1;
    this->clientAddress_var = IPvXAddress("0.0.0.0");
    this->usoAddress_var = IPvXAddress("0.0.0.0");
    this->rmsAddress_var = IPvXAddress("0.0.0.0");
    this->needMemory_var = -1;
    this->nodes_var = -1;
    this->cores_var = -1;
    this->load_var = -1;
    this->subTime_var = -1;
    this->inputData_var = -1;
    this->outputData_var = -1;
    this->networkLoad_var = 0.0;
    this->loadedTime_var = 1.0;
    this->priority_var = 1;
    this->timeOfStart_var = -1;
    this->sleepMode_var = -1;
    this->jobID_var = -1;
    this->submitTime_var = -1;
    this->energyAware_var = false;
}

SuperComInternalMsg::SuperComInternalMsg(const SuperComInternalMsg& other) : cMessage()
{
    setName(other.getName());
    operator=(other);
}

SuperComInternalMsg::~SuperComInternalMsg()
{
}

SuperComInternalMsg& SuperComInternalMsg::operator=(const SuperComInternalMsg& other)
{
    if (this==&other) return *this;
    cMessage::operator=(other);
    this->serverIndex_var = other.serverIndex_var;
    this->runTime_var = other.runTime_var;
    this->connId_var = other.connId_var;
    this->clientAddress_var = other.clientAddress_var;
    this->usoAddress_var = other.usoAddress_var;
    this->rmsAddress_var = other.rmsAddress_var;
    this->needMemory_var = other.needMemory_var;
    this->nodes_var = other.nodes_var;
    this->cores_var = other.cores_var;
    this->load_var = other.load_var;
    this->subTime_var = other.subTime_var;
    this->inputData_var = other.inputData_var;
    this->outputData_var = other.outputData_var;
    this->networkLoad_var = other.networkLoad_var;
    this->loadedTime_var = other.loadedTime_var;
    this->priority_var = other.priority_var;
    this->timeOfStart_var = other.timeOfStart_var;
    this->sleepMode_var = other.sleepMode_var;
    this->jobID_var = other.jobID_var;
    this->submitTime_var = other.submitTime_var;
    this->energyAware_var = other.energyAware_var;
    return *this;
}

void SuperComInternalMsg::parsimPack(cCommBuffer *b)
{
    cMessage::parsimPack(b);
    doPacking(b,this->serverIndex_var);
    doPacking(b,this->runTime_var);
    doPacking(b,this->connId_var);
    doPacking(b,this->clientAddress_var);
    doPacking(b,this->usoAddress_var);
    doPacking(b,this->rmsAddress_var);
    doPacking(b,this->needMemory_var);
    doPacking(b,this->nodes_var);
    doPacking(b,this->cores_var);
    doPacking(b,this->load_var);
    doPacking(b,this->subTime_var);
    doPacking(b,this->inputData_var);
    doPacking(b,this->outputData_var);
    doPacking(b,this->networkLoad_var);
    doPacking(b,this->loadedTime_var);
    doPacking(b,this->priority_var);
    doPacking(b,this->timeOfStart_var);
    doPacking(b,this->sleepMode_var);
    doPacking(b,this->jobID_var);
    doPacking(b,this->submitTime_var);
    doPacking(b,this->energyAware_var);
}

void SuperComInternalMsg::parsimUnpack(cCommBuffer *b)
{
    cMessage::parsimUnpack(b);
    doUnpacking(b,this->serverIndex_var);
    doUnpacking(b,this->runTime_var);
    doUnpacking(b,this->connId_var);
    doUnpacking(b,this->clientAddress_var);
    doUnpacking(b,this->usoAddress_var);
    doUnpacking(b,this->rmsAddress_var);
    doUnpacking(b,this->needMemory_var);
    doUnpacking(b,this->nodes_var);
    doUnpacking(b,this->cores_var);
    doUnpacking(b,this->load_var);
    doUnpacking(b,this->subTime_var);
    doUnpacking(b,this->inputData_var);
    doUnpacking(b,this->outputData_var);
    doUnpacking(b,this->networkLoad_var);
    doUnpacking(b,this->loadedTime_var);
    doUnpacking(b,this->priority_var);
    doUnpacking(b,this->timeOfStart_var);
    doUnpacking(b,this->sleepMode_var);
    doUnpacking(b,this->jobID_var);
    doUnpacking(b,this->submitTime_var);
    doUnpacking(b,this->energyAware_var);
}

int SuperComInternalMsg::getServerIndex() const
{
    return serverIndex_var;
}

void SuperComInternalMsg::setServerIndex(int serverIndex_var)
{
    this->serverIndex_var = serverIndex_var;
}

double SuperComInternalMsg::getRunTime() const
{
    return runTime_var;
}

void SuperComInternalMsg::setRunTime(double runTime_var)
{
    this->runTime_var = runTime_var;
}

int SuperComInternalMsg::getConnId() const
{
    return connId_var;
}

void SuperComInternalMsg::setConnId(int connId_var)
{
    this->connId_var = connId_var;
}

IPvXAddress& SuperComInternalMsg::getClientAddress()
{
    return clientAddress_var;
}

void SuperComInternalMsg::setClientAddress(const IPvXAddress& clientAddress_var)
{
    this->clientAddress_var = clientAddress_var;
}

IPvXAddress& SuperComInternalMsg::getUsoAddress()
{
    return usoAddress_var;
}

void SuperComInternalMsg::setUsoAddress(const IPvXAddress& usoAddress_var)
{
    this->usoAddress_var = usoAddress_var;
}

IPvXAddress& SuperComInternalMsg::getRmsAddress()
{
    return rmsAddress_var;
}

void SuperComInternalMsg::setRmsAddress(const IPvXAddress& rmsAddress_var)
{
    this->rmsAddress_var = rmsAddress_var;
}

int SuperComInternalMsg::getNeedMemory() const
{
    return needMemory_var;
}

void SuperComInternalMsg::setNeedMemory(int needMemory_var)
{
    this->needMemory_var = needMemory_var;
}

int SuperComInternalMsg::getNodes() const
{
    return nodes_var;
}

void SuperComInternalMsg::setNodes(int nodes_var)
{
    this->nodes_var = nodes_var;
}

int SuperComInternalMsg::getCores() const
{
    return cores_var;
}

void SuperComInternalMsg::setCores(int cores_var)
{
    this->cores_var = cores_var;
}

int SuperComInternalMsg::getLoad() const
{
    return load_var;
}

void SuperComInternalMsg::setLoad(int load_var)
{
    this->load_var = load_var;
}

simtime_t SuperComInternalMsg::getSubTime() const
{
    return subTime_var;
}

void SuperComInternalMsg::setSubTime(simtime_t subTime_var)
{
    this->subTime_var = subTime_var;
}

long SuperComInternalMsg::getInputData() const
{
    return inputData_var;
}

void SuperComInternalMsg::setInputData(long inputData_var)
{
    this->inputData_var = inputData_var;
}

long SuperComInternalMsg::getOutputData() const
{
    return outputData_var;
}

void SuperComInternalMsg::setOutputData(long outputData_var)
{
    this->outputData_var = outputData_var;
}

double SuperComInternalMsg::getNetworkLoad() const
{
    return networkLoad_var;
}

void SuperComInternalMsg::setNetworkLoad(double networkLoad_var)
{
    this->networkLoad_var = networkLoad_var;
}

double SuperComInternalMsg::getLoadedTime() const
{
    return loadedTime_var;
}

void SuperComInternalMsg::setLoadedTime(double loadedTime_var)
{
    this->loadedTime_var = loadedTime_var;
}

int SuperComInternalMsg::getPriority() const
{
    return priority_var;
}

void SuperComInternalMsg::setPriority(int priority_var)
{
    this->priority_var = priority_var;
}

simtime_t SuperComInternalMsg::getTimeOfStart() const
{
    return timeOfStart_var;
}

void SuperComInternalMsg::setTimeOfStart(simtime_t timeOfStart_var)
{
    this->timeOfStart_var = timeOfStart_var;
}

int SuperComInternalMsg::getSleepMode() const
{
    return sleepMode_var;
}

void SuperComInternalMsg::setSleepMode(int sleepMode_var)
{
    this->sleepMode_var = sleepMode_var;
}

int SuperComInternalMsg::getJobID() const
{
    return jobID_var;
}

void SuperComInternalMsg::setJobID(int jobID_var)
{
    this->jobID_var = jobID_var;
}

int SuperComInternalMsg::getSubmitTime() const
{
    return submitTime_var;
}

void SuperComInternalMsg::setSubmitTime(int submitTime_var)
{
    this->submitTime_var = submitTime_var;
}

bool SuperComInternalMsg::getEnergyAware() const
{
    return energyAware_var;
}

void SuperComInternalMsg::setEnergyAware(bool energyAware_var)
{
    this->energyAware_var = energyAware_var;
}

class SuperComInternalMsgDescriptor : public cClassDescriptor
{
  public:
    SuperComInternalMsgDescriptor();
    virtual ~SuperComInternalMsgDescriptor();

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

Register_ClassDescriptor(SuperComInternalMsgDescriptor);

SuperComInternalMsgDescriptor::SuperComInternalMsgDescriptor() : cClassDescriptor("SuperComInternalMsg", "cMessage")
{
}

SuperComInternalMsgDescriptor::~SuperComInternalMsgDescriptor()
{
}

bool SuperComInternalMsgDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<SuperComInternalMsg *>(obj)!=NULL;
}

const char *SuperComInternalMsgDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int SuperComInternalMsgDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 21+basedesc->getFieldCount(object) : 21;
}

unsigned int SuperComInternalMsgDescriptor::getFieldTypeFlags(void *object, int field) const
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
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<21) ? fieldTypeFlags[field] : 0;
}

const char *SuperComInternalMsgDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "serverIndex",
        "runTime",
        "connId",
        "clientAddress",
        "usoAddress",
        "rmsAddress",
        "needMemory",
        "nodes",
        "cores",
        "load",
        "subTime",
        "inputData",
        "outputData",
        "networkLoad",
        "loadedTime",
        "priority",
        "timeOfStart",
        "sleepMode",
        "jobID",
        "submitTime",
        "energyAware",
    };
    return (field>=0 && field<21) ? fieldNames[field] : NULL;
}

int SuperComInternalMsgDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "serverIndex")==0) return base+0;
    if (fieldName[0]=='r' && strcmp(fieldName, "runTime")==0) return base+1;
    if (fieldName[0]=='c' && strcmp(fieldName, "connId")==0) return base+2;
    if (fieldName[0]=='c' && strcmp(fieldName, "clientAddress")==0) return base+3;
    if (fieldName[0]=='u' && strcmp(fieldName, "usoAddress")==0) return base+4;
    if (fieldName[0]=='r' && strcmp(fieldName, "rmsAddress")==0) return base+5;
    if (fieldName[0]=='n' && strcmp(fieldName, "needMemory")==0) return base+6;
    if (fieldName[0]=='n' && strcmp(fieldName, "nodes")==0) return base+7;
    if (fieldName[0]=='c' && strcmp(fieldName, "cores")==0) return base+8;
    if (fieldName[0]=='l' && strcmp(fieldName, "load")==0) return base+9;
    if (fieldName[0]=='s' && strcmp(fieldName, "subTime")==0) return base+10;
    if (fieldName[0]=='i' && strcmp(fieldName, "inputData")==0) return base+11;
    if (fieldName[0]=='o' && strcmp(fieldName, "outputData")==0) return base+12;
    if (fieldName[0]=='n' && strcmp(fieldName, "networkLoad")==0) return base+13;
    if (fieldName[0]=='l' && strcmp(fieldName, "loadedTime")==0) return base+14;
    if (fieldName[0]=='p' && strcmp(fieldName, "priority")==0) return base+15;
    if (fieldName[0]=='t' && strcmp(fieldName, "timeOfStart")==0) return base+16;
    if (fieldName[0]=='s' && strcmp(fieldName, "sleepMode")==0) return base+17;
    if (fieldName[0]=='j' && strcmp(fieldName, "jobID")==0) return base+18;
    if (fieldName[0]=='s' && strcmp(fieldName, "submitTime")==0) return base+19;
    if (fieldName[0]=='e' && strcmp(fieldName, "energyAware")==0) return base+20;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *SuperComInternalMsgDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "double",
        "int",
        "IPvXAddress",
        "IPvXAddress",
        "IPvXAddress",
        "int",
        "int",
        "int",
        "int",
        "simtime_t",
        "long",
        "long",
        "double",
        "double",
        "int",
        "simtime_t",
        "int",
        "int",
        "int",
        "bool",
    };
    return (field>=0 && field<21) ? fieldTypeStrings[field] : NULL;
}

const char *SuperComInternalMsgDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int SuperComInternalMsgDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    SuperComInternalMsg *pp = (SuperComInternalMsg *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string SuperComInternalMsgDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    SuperComInternalMsg *pp = (SuperComInternalMsg *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getServerIndex());
        case 1: return double2string(pp->getRunTime());
        case 2: return long2string(pp->getConnId());
        case 3: {std::stringstream out; out << pp->getClientAddress(); return out.str();}
        case 4: {std::stringstream out; out << pp->getUsoAddress(); return out.str();}
        case 5: {std::stringstream out; out << pp->getRmsAddress(); return out.str();}
        case 6: return long2string(pp->getNeedMemory());
        case 7: return long2string(pp->getNodes());
        case 8: return long2string(pp->getCores());
        case 9: return long2string(pp->getLoad());
        case 10: return double2string(pp->getSubTime());
        case 11: return long2string(pp->getInputData());
        case 12: return long2string(pp->getOutputData());
        case 13: return double2string(pp->getNetworkLoad());
        case 14: return double2string(pp->getLoadedTime());
        case 15: return long2string(pp->getPriority());
        case 16: return double2string(pp->getTimeOfStart());
        case 17: return long2string(pp->getSleepMode());
        case 18: return long2string(pp->getJobID());
        case 19: return long2string(pp->getSubmitTime());
        case 20: return bool2string(pp->getEnergyAware());
        default: return "";
    }
}

bool SuperComInternalMsgDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    SuperComInternalMsg *pp = (SuperComInternalMsg *)object; (void)pp;
    switch (field) {
        case 0: pp->setServerIndex(string2long(value)); return true;
        case 1: pp->setRunTime(string2double(value)); return true;
        case 2: pp->setConnId(string2long(value)); return true;
        case 6: pp->setNeedMemory(string2long(value)); return true;
        case 7: pp->setNodes(string2long(value)); return true;
        case 8: pp->setCores(string2long(value)); return true;
        case 9: pp->setLoad(string2long(value)); return true;
        case 10: pp->setSubTime(string2double(value)); return true;
        case 11: pp->setInputData(string2long(value)); return true;
        case 12: pp->setOutputData(string2long(value)); return true;
        case 13: pp->setNetworkLoad(string2double(value)); return true;
        case 14: pp->setLoadedTime(string2double(value)); return true;
        case 15: pp->setPriority(string2long(value)); return true;
        case 16: pp->setTimeOfStart(string2double(value)); return true;
        case 17: pp->setSleepMode(string2long(value)); return true;
        case 18: pp->setJobID(string2long(value)); return true;
        case 19: pp->setSubmitTime(string2long(value)); return true;
        case 20: pp->setEnergyAware(string2bool(value)); return true;
        default: return false;
    }
}

const char *SuperComInternalMsgDescriptor::getFieldStructName(void *object, int field) const
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
        "IPvXAddress",
        "IPvXAddress",
        "IPvXAddress",
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
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<21) ? fieldStructNames[field] : NULL;
}

void *SuperComInternalMsgDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    SuperComInternalMsg *pp = (SuperComInternalMsg *)object; (void)pp;
    switch (field) {
        case 3: return (void *)(&pp->getClientAddress()); break;
        case 4: return (void *)(&pp->getUsoAddress()); break;
        case 5: return (void *)(&pp->getRmsAddress()); break;
        default: return NULL;
    }
}


