//
// Generated file, do not edit! Created by opp_msgc 4.1 from applications/ddeclient/ClientMsg.msg.
//

#ifndef _CLIENTMSG_M_H_
#define _CLIENTMSG_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0401
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif

// cplusplus {{
#include "INETDefs.h"
#include "GenericAppMsg_m.h"
#include "IPvXAddress.h"
#include "MACAddress.h"
// }}



/**
 * Class generated from <tt>applications/ddeclient/ClientMsg.msg</tt> by opp_msgc.
 * <pre>
 * message ClientInternalMsg extends cMessage
 * {
 *     int connId = -1;
 *     IPvXAddress serverAddress = IPvXAddress("0.0.0.0");
 *     string connectAddress = "";
 *     int interfaceID = -1;
 *     string ssid = "";
 *     bool connectToAp = true;
 *     bool connectToFileServer = false;
 *     MACAddress macAddress;
 *     MACAddress newMAC;
 * }
 * </pre>
 */
class ClientInternalMsg : public ::cMessage
{
  protected:
    int connId_var;
    ::IPvXAddress serverAddress_var;
    opp_string connectAddress_var;
    int interfaceID_var;
    opp_string ssid_var;
    bool connectToAp_var;
    bool connectToFileServer_var;
    ::MACAddress macAddress_var;
    ::MACAddress newMAC_var;

    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ClientInternalMsg&);

  public:
    ClientInternalMsg(const char *name=NULL, int kind=0);
    ClientInternalMsg(const ClientInternalMsg& other);
    virtual ~ClientInternalMsg();
    ClientInternalMsg& operator=(const ClientInternalMsg& other);
    virtual ClientInternalMsg *dup() const {return new ClientInternalMsg(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getConnId() const;
    virtual void setConnId(int connId_var);
    virtual IPvXAddress& getServerAddress();
    virtual const IPvXAddress& getServerAddress() const {return const_cast<ClientInternalMsg*>(this)->getServerAddress();}
    virtual void setServerAddress(const IPvXAddress& serverAddress_var);
    virtual const char * getConnectAddress() const;
    virtual void setConnectAddress(const char * connectAddress_var);
    virtual int getInterfaceID() const;
    virtual void setInterfaceID(int interfaceID_var);
    virtual const char * getSsid() const;
    virtual void setSsid(const char * ssid_var);
    virtual bool getConnectToAp() const;
    virtual void setConnectToAp(bool connectToAp_var);
    virtual bool getConnectToFileServer() const;
    virtual void setConnectToFileServer(bool connectToFileServer_var);
    virtual MACAddress& getMacAddress();
    virtual const MACAddress& getMacAddress() const {return const_cast<ClientInternalMsg*>(this)->getMacAddress();}
    virtual void setMacAddress(const MACAddress& macAddress_var);
    virtual MACAddress& getNewMAC();
    virtual const MACAddress& getNewMAC() const {return const_cast<ClientInternalMsg*>(this)->getNewMAC();}
    virtual void setNewMAC(const MACAddress& newMAC_var);
};

inline void doPacking(cCommBuffer *b, ClientInternalMsg& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, ClientInternalMsg& obj) {obj.parsimUnpack(b);}


#endif // _CLIENTMSG_M_H_