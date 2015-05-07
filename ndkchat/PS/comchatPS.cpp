#ifndef LOG_TAG
#define LOG_TAG "comchatPS"
#endif

#include <utils/Debug.h>
#include <utils/Log.h>
#include <binder/IPCThreadState.h>
#include <binder/Parcel.h>
#include <utils/String8.h>
#include <utils/SystemClock.h>

#include <private/binder/Static.h>

#include <unistd.h>

#include <../comchat.h>

#ifndef _TRACE
#define _TRACE LOGE
//#define _TRACE(...)   ((void)0)
#endif

namespace android {


//#define CHECKOUT_CALLER(data) comchat_caller_autosetter _autocaller(data)

//const CONTEXT_CALLER_T * Chat_GetContextCaller(void){
//	return comchat_get_caller_thread_slot();
//}
static pid_t GetCallerPID(void){
	return IPCThreadState::self()->getCallingPid();
}
static String8 good_old_string(const String16& src)
{
    String8 name8;
    char ch8[2];
    ch8[1] = 0;
    for (unsigned j = 0; j < src.size(); j++) {
        char16_t ch = src[j];
        if (ch < 128) ch8[0] = (char)ch;
        name8.append(ch8);
    }
    return name8;
}
#define DISPLAY_STRING16(XXX) good_old_string(XXX).string()


//-------------------------------------------------------------------------------------------

enum{
	ICSE_OnNewUser = IBinder::FIRST_CALL_TRANSACTION,
	ICSE_OnUserLeft,
	ICSE_OnNewStatement,

	ICSE_Sync_OnNewStatement,
		
	ICSE_GetUserName,

	ICSE_MAX
};

class BpChatSessionEvents : public BpInterface<IChatSessionEvents>
{
public:
	BpChatSessionEvents(const sp<IBinder>& impl)
        : BpInterface<IChatSessionEvents>(impl)
	{
	}

	virtual void    OnNewUser(const String16& name)
	{
		Parcel data, reply;
		_TRACE("BpChatSessionEvents::OnNewUser>>"); 
		data.writeInterfaceToken(IChatSessionEvents::getInterfaceDescriptor());
		data.writeString16(name);
		status_t err = remote()->transact(ICSE_OnNewUser, data, &reply,IBinder::FLAG_ONEWAY);
		_TRACE("BpChatSessionEvents::OnNewUser<< err(%d)",err); 
	}
	
	virtual void    OnUserLeft(const String16& name) 
	{
		Parcel data, reply;
		_TRACE("BpChatSessionEvents::OnUserLeft>>"); 
		data.writeInterfaceToken(IChatSessionEvents::getInterfaceDescriptor());
		data.writeString16(name);
		status_t err = remote()->transact(ICSE_OnUserLeft, data, &reply,IBinder::FLAG_ONEWAY);
		_TRACE("BpChatSessionEvents::OnUserLeft<< err(%d)",err);
	}
	
	virtual void    OnNewStatement(const String16& name,
										const String16& statement)
	{
		Parcel data, reply;
		_TRACE("BpChatSessionEvents::OnNewStatement>>"); 
		data.writeInterfaceToken(IChatSessionEvents::getInterfaceDescriptor());
		data.writeString16(name);
		data.writeString16(statement);		
		status_t err = remote()->transact(ICSE_OnNewStatement, data, &reply,IBinder::FLAG_ONEWAY);
		_TRACE("BpChatSessionEvents::OnNewStatement<< err(%d)",err);		
	}	

	virtual status_t Sync_OnNewStatement(const String16& name,
										const String16& statement)
	{
		Parcel data, reply;
		_TRACE("BpChatSessionEvents::Sync_OnNewStatement>>"); 
		data.writeInterfaceToken(IChatSessionEvents::getInterfaceDescriptor());
		data.writeString16(name);
		data.writeString16(statement);		
		status_t err = remote()->transact(ICSE_Sync_OnNewStatement, data, &reply);
		_TRACE("BpChatSessionEvents::Sync_OnNewStatement<< err(%d)",err);	
		return err == NO_ERROR ? reply.readInt32() : 0;
	}	


	

	virtual String16    GetUserName(void) const 
	{
		Parcel data, reply;
		String16 userName;
		_TRACE("BpChatSessionEvents::GetUserName this(%p)>>",this); 

		data.writeInterfaceToken(IChatSessionEvents::getInterfaceDescriptor());
		status_t err = remote()->transact(ICSE_GetUserName, data, &reply);

		if(err == NO_ERROR)
			userName =  reply.readString16();
		_TRACE("BpChatSessionEvents::GetUserName this(%p)<< err:%d,%s",this,err,DISPLAY_STRING16(userName)); 
		return userName;
	}
	

	
	
};

IMPLEMENT_META_INTERFACE(ChatSessionEvents, "android.os.IChatSessionEvents");

status_t BnChatSessionEvents::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    	status_t result = 0x80000000;
	_TRACE("ChatSessionEvents::onTransact[pid:%d] >>:code 0x%x",GetCallerPID(),code); //data.print();
	
	switch(code) {
		case ICSE_OnNewUser: {
			CHECK_INTERFACE(IChatSessionEvents, data, reply);
			String16 name = data.readString16();
			OnNewUser(name);
			result = NO_ERROR;
		} break;		
		case ICSE_OnUserLeft: {
			CHECK_INTERFACE(IChatSessionEvents, data, reply);
			String16 name = data.readString16();
			OnUserLeft(name);
			result = NO_ERROR;
		} break;
		case ICSE_OnNewStatement: {
			CHECK_INTERFACE(IChatSessionEvents, data, reply);
			String16 name = data.readString16();
			String16 statement = data.readString16();		
			OnNewStatement(name,statement);
			result = NO_ERROR;
		} break;
		case ICSE_Sync_OnNewStatement: {
			CHECK_INTERFACE(IChatSessionEvents, data, reply);
			String16 name = data.readString16();
			String16 statement = data.readString16();		
			status_t err = Sync_OnNewStatement(name,statement);
			reply->writeInt32(err);
			result = NO_ERROR;
		} break;		
		case ICSE_GetUserName: {
			CHECK_INTERFACE(IChatSessionEvents, data, reply);
			String16 name = GetUserName();
			reply->writeString16(name);
			result = NO_ERROR;
		} break;		
		default:
			result = BBinder::onTransact(code, data, reply, flags);
	}
	
	_TRACE("ChatSessionEvents::onTransact[pid:%d] <<:result %d",GetCallerPID(),result); //reply.print();
	return result;	
}

//----------------------------------------------------------------------------------

enum{
	ICS_getName = IBinder::FIRST_CALL_TRANSACTION,
	ICS_GetStatements,
	ICS_Advise,
	ICS_Unadvise,
	ICS_Say,
	ICS_AsyncSay,

	ICS_MAX
};

class BpChatSession : public BpInterface<IChatSession>
{
public:
	BpChatSession(const sp<IBinder>& impl)
        : BpInterface<IChatSession>(impl)
	{
	}

	virtual String16    GetName(void) const 
	{
		Parcel data, reply;
		data.writeInterfaceToken(IChatSession::getInterfaceDescriptor());
//		data.writeInt32((int32_t)getpid());

		remote()->transact(ICS_getName, data, &reply);
		return reply.readString16();
	}
	

	
	virtual Vector<String16>    GetStatements(void) const
	{
	
		Vector<String16> res;

#ifdef RDVEC_ONE
		int n = 0;
		for (;;) {
			Parcel data, reply;
			data.writeInterfaceToken(IChatSession::getInterfaceDescriptor());
//			data.writeInt32((int32_t)getpid());
			data.writeInt32(n++);
			status_t err = remote()->transact(ICS_GetStatements, data, &reply);
			if (err != NO_ERROR){
				_TRACE("GetStatements failed: %d",n);
				break;
			}
			if(res.add(reply.readString16()) != 1){
				_TRACE("GetStatements: invalid data:%d",n);
				break;
			}
		}
#else
		Parcel data, reply;
		data.writeInterfaceToken(IChatSession::getInterfaceDescriptor());
//		data.writeInt32((int32_t)getpid());
		status_t err = remote()->transact(ICS_GetStatements, data, &reply);
		if (err != NO_ERROR){
			_TRACE("GetStatements failed");
			return res;
		}

		int n = reply.readInt32();
		while(n>0){
			res.add(reply.readString16());
			n--;
		}
		_TRACE("GetStatements: get %d",res.size());
#endif
		return res;
	}

	virtual void    Advise(const sp<IChatSessionEvents>& eventSink)
	{
		Parcel data, reply;
		data.writeInterfaceToken(IChatSession::getInterfaceDescriptor());
//		data.writeInt32((int32_t)getpid());
		data.writeStrongBinder(eventSink->asBinder());
		remote()->transact(ICS_Advise, data, &reply);
	}
	
	virtual status_t    Unadvise() 
	{
		Parcel data, reply;
		data.writeInterfaceToken(IChatSession::getInterfaceDescriptor());
//		data.writeInt32((int32_t)getpid());
		status_t err = remote()->transact(ICS_Unadvise, data, &reply);
		return err == NO_ERROR ? reply.readInt32() : 0;
	}

	virtual status_t    Say(const String16& statement)
	{
		Parcel data, reply;
		data.writeInterfaceToken(IChatSession::getInterfaceDescriptor());
//		data.writeInt32((int32_t)getpid());

		data.writeString16(statement);
		status_t err = remote()->transact(ICS_Say, data, &reply);
		return err == NO_ERROR ? reply.readInt32() : 0;
	}

	virtual void    AsyncSay(const String16& statement)
	{
		Parcel data, reply;
		_TRACE("BpChatSession::AsyncSay>>");	
		data.writeInterfaceToken(IChatSession::getInterfaceDescriptor());
//		data.writeInt32((int32_t)getpid());

		data.writeString16(statement);
		status_t err = remote()->transact(ICS_AsyncSay, data, &reply,IBinder::FLAG_ONEWAY);
		_TRACE("BpChatSession::AsyncSay<< err(%d)",err);	
	}
	
	
};

IMPLEMENT_META_INTERFACE(ChatSession, "android.os.IChatSession");


status_t BnChatSession::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
	status_t result = 0x80000000;
	_TRACE("ChatSession::onTransact[pid:%d] >>:code 0x%x",GetCallerPID(),code); //data.print();
	
	switch(code) {
		case ICS_getName: {
			CHECK_INTERFACE(IChatSession, data, reply);
			//CHECKOUT_CALLER(data);

			String16 name = const_cast<BnChatSession*>(this)->GetName();			
			reply->writeString16(name);
			result =  NO_ERROR;
		} break;		
		case ICS_GetStatements: {

			CHECK_INTERFACE(IChatSession, data, reply);
			//CHECKOUT_CALLER(data);
			Vector<String16> list = GetStatements();
			const size_t N = list.size();
			reply->writeInt32(N);
			_TRACE("GetStatements num %u",(unsigned int)N);
			for (size_t i=0; i<N; i++) {
				reply->writeString16(list[i]);
			}
			result = NO_ERROR;
			
		} break;
		case ICS_Say: {
			CHECK_INTERFACE(IChatSession, data, reply);
			//CHECKOUT_CALLER(data);

			String16 statement = data.readString16();
			status_t err = Say(statement);
			reply->writeInt32(err);
			result = NO_ERROR;
		} break;
		case ICS_AsyncSay: {
			CHECK_INTERFACE(IChatSession, data, reply);
			//CHECKOUT_CALLER(data);

			String16 statement = data.readString16();
			AsyncSay(statement);
			result = NO_ERROR;
		} break;
		case ICS_Advise: {
			CHECK_INTERFACE(IChatSession, data, reply);
			sp<IChatSessionEvents> eventSink = interface_cast<IChatSessionEvents>(data.readStrongBinder());
			Advise(eventSink);
			result = NO_ERROR;
		} break;
		case ICS_Unadvise: {
			CHECK_INTERFACE(IChatSession, data, reply);
			//CHECKOUT_CALLER(data);

			status_t err = Unadvise();
			reply->writeInt32(err);
			result = NO_ERROR;
		} break;		
		default:
			result = BBinder::onTransact(code, data, reply, flags);
	}
	
	_TRACE("ChatSession::onTransact[pid:%d] <<:result %d",GetCallerPID(),result); //reply.print();
	return result;
}


//----------------------------------------------------------------------------------
enum{
	ICSM_GetSessionNames = IBinder::FIRST_CALL_TRANSACTION,
	ICSM_FindSession,
	ICSM_DeleteSession,

	ICSM_MAX
};

class BpChatSessionManager : public BpInterface<IChatSessionManager>
{
public:
	BpChatSessionManager(const sp<IBinder>& impl)
        : BpInterface<IChatSessionManager>(impl)
	{
	}
	
	virtual Vector<String16> GetSessionNames(void) 
	{
		//??? 为什么返回时需要多次调用? 而服务器端只需要一次返回?
		Vector<String16> res;
#ifdef RDVEC_ONE					
		int n = 0;

		_TRACE("BpChatSessionManager::GetSessionNames() >>");
		
		for (;;) {
			Parcel data, reply;
			data.writeInterfaceToken(IChatSessionManager::getInterfaceDescriptor());
			data.writeInt32(n++);
			status_t err = remote()->transact(ICSM_GetSessionNames, data, &reply);
			_TRACE("transact(err %d)", err);
			if (err != NO_ERROR)
				break;
			res.add(reply.readString16());
		}
		
		_TRACE("BpChatSessionManager::GetSessionNames() <<" );
#else
		Parcel data, reply;
		data.writeInterfaceToken(IChatSessionManager::getInterfaceDescriptor());
		status_t err = remote()->transact(ICSM_GetSessionNames, data, &reply);
		if (err != NO_ERROR){
			_TRACE("BpChatSessionManager::GetSessionNames failed");
			return res;
		}

		int n = reply.readInt32();
		while(n>0){
			res.add(reply.readString16());
			n--;
		}
		_TRACE("BpChatSessionManager::GetSessionNames: get %d",res.size());
#endif
		return res;
	}


	virtual sp<IChatSession>  FindSession(const String16& name, 
								BOOL bDontCreate,
								BOOL bAllowAnonymousAccess) 
	{
		Parcel data, reply;
		sp<IChatSession> cs;
		_TRACE("BpChatSessionManager::FindSession() >>" );

		data.writeInterfaceToken(IChatSessionManager::getInterfaceDescriptor());
		data.writeString16(name);
		data.writeInt32(bDontCreate);
		data.writeInt32(bAllowAnonymousAccess);		
		status_t err = remote()->transact(ICSM_FindSession, data, &reply);
		if (err != NO_ERROR) {
	            LOGE("FindSession error: %s", strerror(-err));
        	} else {
       	     cs  = interface_cast<IChatSession>(reply.readStrongBinder());
        	}

		if(cs == NULL){
			_TRACE("failed to find Session");
		}
			
		_TRACE("BpChatSessionManager::FindSession() <<" );				
		return cs;
	}

	
	virtual status_t DeleteSession(const String16& name) 
	{
		Parcel data, reply;
		_TRACE("BpChatSessionManager::DeleteSession() >>" );
		data.writeInterfaceToken(IChatSessionManager::getInterfaceDescriptor());
		data.writeString16(name);
		status_t err = remote()->transact(ICSM_DeleteSession, data, &reply);
		_TRACE("BpChatSessionManager::DeleteSession() <<" );
		return err == NO_ERROR ? reply.readInt32() : 0;
	}
};

IMPLEMENT_META_INTERFACE(ChatSessionManager, "android.os.IChatSessionManager");

status_t BnChatSessionManager::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
	status_t result = 0x80000000;
	_TRACE("ChatSessionManager::onTransact[pid:%d] >>:code 0x%x",GetCallerPID(),code); //data.print();
	switch(code) {
	case ICSM_GetSessionNames: {
		CHECK_INTERFACE(IChatSessionManager, data, reply);
		Vector<String16> list = GetSessionNames();
		const size_t N = list.size();
		reply->writeInt32(N);
		for (size_t i=0; i<N; i++) {
		    reply->writeString16(list[i]);
		}
		result = NO_ERROR;
	} break;
	case ICSM_FindSession: {
		CHECK_INTERFACE(IChatSessionManager, data, reply);
		String16 name = data.readString16();
		BOOL bDontCreate = data.readInt32();
		BOOL bAllowAnonymousAccess = data.readInt32();	
		sp<IChatSession> b = FindSession(name,bDontCreate,bAllowAnonymousAccess);			
		reply->writeStrongBinder(b->asBinder());
		result =  NO_ERROR;
	} break;
	case ICSM_DeleteSession: {
		CHECK_INTERFACE(IChatSessionManager, data, reply);
		String16 name = data.readString16();
		status_t err = DeleteSession(name);
		reply->writeInt32(err);
		result =  NO_ERROR;
	} break;		
	default:
		result =  BBinder::onTransact(code, data, reply, flags);
	}
	_TRACE("ChatSessionManager::onTransact[pid:%d] <<:result %d",GetCallerPID(),result); //reply.print();
	return result;
}

}

