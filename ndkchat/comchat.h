#ifndef ANDROID_COMCHAT_H
#define ANDROID_COMCHAT_H

#include <binder/IInterface.h>
#include <utils/Vector.h>
#include <utils/String16.h>

namespace android {

typedef int32_t BOOL;
#define TRUE 1
#define FALSE 0

typedef int32_t DWORD;
typedef status_t HRESULT;
#define SUCCEEDED(XXX) ((XXX) >=0)
#define FAILED(XXX) (XXX <0)


typedef struct{
	int32_t pid;
}CONTEXT_CALLER_T;


//extern "C" const CONTEXT_CALLER_T * Chat_GetContextCaller(void);



// ----------------------------------------------------------------------
class IChatSessionEvents : public IInterface
{
public:
	DECLARE_META_INTERFACE(ChatSessionEvents);
	virtual void    OnNewUser(const String16& userName) = 0;
	virtual void    OnUserLeft(const String16& userName) = 0;
	virtual void    OnNewStatement(const String16& userName,
										const String16& statement) = 0;
	virtual status_t Sync_OnNewStatement(const String16& userName,
										const String16& statement) = 0;

	virtual String16	GetUserName(void) const = 0;

	
};

class BnChatSessionEvents : public BnInterface<IChatSessionEvents>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};

// ----------------------------------------------------------------------

class IChatSession :public IInterface
{
public:
	DECLARE_META_INTERFACE(ChatSession);
	
	virtual String16    GetName(void) const = 0;
	virtual Vector<String16>    GetStatements(void) const= 0;


	virtual void    Advise(const sp<IChatSessionEvents>& eventSink) = 0;
	virtual status_t    Unadvise() = 0;
	virtual status_t    Say(const String16& statement) = 0;
	virtual void    AsyncSay(const String16& statement) = 0;
	
};

class BnChatSession : public BnInterface<IChatSession>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};


// ----------------------------------------------------------------------
class IChatSessionManager :public IInterface
{
public:
	DECLARE_META_INTERFACE(ChatSessionManager);
	
	virtual Vector<String16> GetSessionNames(void) = 0;
	virtual sp<IChatSession>  FindSession(const String16& name, 
								BOOL bDontCreate,
								BOOL bAllowAnonymousAccess)  = 0;
	virtual status_t DeleteSession(const String16& name) = 0;
};

class BnChatSessionManager : public BnInterface<IChatSessionManager>
{
public:
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};

}; // namespace android

#endif // ANDROID_ISERVICE_MANAGER_H
