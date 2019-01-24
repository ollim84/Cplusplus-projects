//
// Generated file, do not edit! Created by opp_msgc 4.1 from applications/ddeclient/AppContext.msg.
//

#ifndef _APPCONTEXT_M_H_
#define _APPCONTEXT_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0401
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif

// cplusplus {{
#include "INETDefs.h"
#include "GenericAppMsg_m.h"
// }}



/**
 * Class generated from <tt>applications/ddeclient/AppContext.msg</tt> by opp_msgc.
 * <pre>
 * packet AppContext extends GenericAppMsg
 * {
 * 
 * 
 * 
 *     int fileSize; 
 *     int remainingFileSize; 
 * 
 * 
 *     double txDuration; 
 *     int chunkSize; 
 *     double codecRate; 
 *     int userClass; 
 *     
 * 
 * 
 * 
 * }
 * </pre>
 */
class AppContext : public ::GenericAppMsg
{
  protected:
    int fileSize_var;
    int remainingFileSize_var;
    double txDuration_var;
    int chunkSize_var;
    double codecRate_var;
    int userClass_var;

    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const AppContext&);

  public:
    AppContext(const char *name=NULL, int kind=0);
    AppContext(const AppContext& other);
    virtual ~AppContext();
    AppContext& operator=(const AppContext& other);
    virtual AppContext *dup() const {return new AppContext(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getFileSize() const;
    virtual void setFileSize(int fileSize_var);
    virtual int getRemainingFileSize() const;
    virtual void setRemainingFileSize(int remainingFileSize_var);
    virtual double getTxDuration() const;
    virtual void setTxDuration(double txDuration_var);
    virtual int getChunkSize() const;
    virtual void setChunkSize(int chunkSize_var);
    virtual double getCodecRate() const;
    virtual void setCodecRate(double codecRate_var);
    virtual int getUserClass() const;
    virtual void setUserClass(int userClass_var);
};

inline void doPacking(cCommBuffer *b, AppContext& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, AppContext& obj) {obj.parsimUnpack(b);}


#endif // _APPCONTEXT_M_H_