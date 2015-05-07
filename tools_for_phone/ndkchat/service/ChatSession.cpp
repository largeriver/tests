/////////////////////////////////////////////////////
//
// ChatSession.cpp
//
// Copyright 1997, Don Box/Addison Wesley
// Copyright 2009 Jokul
//
// This code accompanies the book "The Component
// Object Model" from Addison Wesley. Blah blah blah
//
//

#include <binder/IInterface.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>

#include <utils/Log.h>
#include <utils/String8.h>
#include <utils/String16.h>

#include <utils/List.h>
#include <binder/Parcel.h>
#include <utils/Singleton.h>
#include <utils/threads.h>

#include "../comchat.h"
#include "ChatSession.h"





#ifndef _TRACE
#define _TRACE LOGE
//#define _TRACE(...)   ((void)0)
#endif

namespace android{
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



enum{
	E_INVALIDARG = -4,
	E_OUTOFMEMORY = -3,
	E_ACCESSDENIED	= -2,
	S_OK = 0		
};




class ChatWorkThread:public Thread{
	enum{
		WORK_Defer_OnNewStatement,
		WORK_Defer_OnNewUser,
		WORK_Defer_OnUserLeft
		
	};

	class work_item_t:public RefBase{
	public:		
		int32_t cmd;
		sp<ChatSession> cs;
		String16 sparam1;
		String16 sparam2;

		work_item_t(int32_t in_cmd,
					ChatSession* in_cs,
					const String16& in_sparam1, 
					const String16& in_sparam2)
		:cmd(in_cmd),cs(in_cs),sparam1(in_sparam1),sparam2(in_sparam2)
		{
			_TRACE("work_item_t(cmd:%d, sp1:%s, sp2:%s) ",cmd,DISPLAY_STRING16(sparam1),DISPLAY_STRING16(sparam2));
		}
	};
	
	Mutex		workMutex;
	Condition		workCond;
	Vector< sp<work_item_t> >	work;


public:
	ChatWorkThread():Thread(false){}
	
public:
	void Defer_OnNewStatement(ChatSession* cs,const String16& pwszUser, const String16& pwszStatement)
	{
		_TRACE("ChatWorkThread:Defer_OnNewStatement>> (cs %p)",cs);
		sp<work_item_t> data = new work_item_t(WORK_Defer_OnNewStatement,cs,pwszUser,pwszStatement);
		Mutex::Autolock _locker(workMutex);
		work.add(data);
		workCond.signal();

		_TRACE("ChatWorkThread:Defer_OnNewStatement<< (cs %p)",cs);
	}

	void Defer_OnNewUser(ChatSession*  cs,const String16& pwszUser)
	{
		_TRACE("ChatWorkThread:Defer_OnNewUser>>(cs %p) %s",cs,DISPLAY_STRING16(pwszUser));
		sp<work_item_t> data = new work_item_t(WORK_Defer_OnNewUser,cs,pwszUser,String16());
		Mutex::Autolock _locker(workMutex);
		work.add(data);
		workCond.signal();

		_TRACE("ChatWorkThread:Defer_OnNewUser<<(cs %p)",cs);		
	}

	void Defer_OnUserLeft(ChatSession* cs,const String16& pwszUser)
	{
		_TRACE("ChatWorkThread:Defer_OnUserLeft>> (cs %p)",cs);
		sp<work_item_t> data = new work_item_t(WORK_Defer_OnUserLeft,cs,pwszUser,String16());	
		Mutex::Autolock _locker(workMutex);
		work.add(data);
		workCond.signal();

		_TRACE("ChatWorkThread:Defer_OnUserLeft<< (cs %p)",cs);		
	}
	
private:
	virtual bool        threadLoop()
	{
		sp<work_item_t> data;

		while (!exitPending()){
			_TRACE("check work");
			{
				Mutex::Autolock _locker(workMutex);
				if(work.size() == 0){
					workCond.wait(workMutex);
				}
				if(work.size() ==0){
					_TRACE("no work ");
					continue;
				}
			}

			//usleep(500*1000);
			data = work[0];
			work.removeAt(0);

			_TRACE("got work(%p,cmd:%d, sp1:%s, sp2:%s) ",data.get(),data->cmd,DISPLAY_STRING16(data->sparam1),DISPLAY_STRING16(data->sparam2));
			switch(data->cmd){
			case	WORK_Defer_OnNewStatement:
				{
					sp<ChatSession> cs = data->cs;
					cs->Fire_OnNewStatement(data->sparam1,data->sparam2);
				}break;
			case	WORK_Defer_OnNewUser:
				{
					sp<ChatSession> cs = data->cs;
					cs->Fire_OnNewUser(data->sparam1);
				}break;
			case	WORK_Defer_OnUserLeft:
				{
					sp<ChatSession> cs = data->cs;
					cs->Fire_OnUserLeft(data->sparam1);
				}break;
			default:
				break;
			}

			data.clear();
			_TRACE("work down");
		}
		return false;

    }

};

ANDROID_SINGLETON_STATIC_INSTANCE(ChatWorkThread)
typedef  Singleton<ChatWorkThread> ChatWorker;


ChatSession::ChatSession(const String16& name,
                         bool bAllowAnonymousAccess)
:m_wszSessionName(name),
m_bAllowAnonymousAccess(bAllowAnonymousAccess)
{
	_TRACE("ChatSession(%s) cons",DISPLAY_STRING16(m_wszSessionName));
}

ChatSession::~ChatSession(void)
{
	_TRACE("~ChatSession(%s) dest",DISPLAY_STRING16(m_wszSessionName));
	Mutex::Autolock _lock(m_csAdviseLock);
	m_Listeners.clear();

}


// IBinder::DeathRecipient
void        ChatSession::binderDied(const wp<IBinder>& who){
    LOGV("binderDied() %p, tid %d, calling tid %d", who.unsafe_get(), gettid(), IPCThreadState::self()->getCallingPid());
    Mutex::Autolock _l(m_csAdviseLock);
/*
    IBinder *binder = who.unsafe_get();

    if (binder != NULL) {
        int index = mNotificationClients.indexOf(binder);
        if (index >= 0) {
            LOGV("Removing notification client %p", binder);
            mNotificationClients.removeAt(index);
        }
    }
*/    
	pid_t callerPid = IPCThreadState::self()->getCallingPid();
     	LOGV("Removing notification client %d", callerPid);
	 m_Listeners.removeItem(callerPid);

}
// helper methods ///////////

void ChatSession::Disconnect(void)
{

// tear down connected listeners
	_TRACE("ChatSession::Disconnect(%s) ",DISPLAY_STRING16(m_wszSessionName));
	Mutex::Autolock _lock(m_csAdviseLock);
	m_Listeners.clear();
}

// send the OnNewStatement event to all listeners
void ChatSession::Fire_OnNewStatement(const String16& pwszUser,
                                 const String16& pwszStatement)
{
	_TRACE("ChatSession::Fire_OnNewStatement >> this(%p), (%s,%s,%s) ",this,DISPLAY_STRING16(m_wszSessionName),DISPLAY_STRING16(pwszUser),DISPLAY_STRING16(pwszStatement));
	Mutex::Autolock _lock(m_csAdviseLock);
	for (size_t i = 0; i < m_Listeners.size(); i++) 	{
		sp<IBinder> binder = m_Listeners.valueAt(i);
		sp<IChatSessionEvents> cse = interface_cast<IChatSessionEvents> (binder);
		_TRACE("**target(cse:%p binder:%p) ",cse.get(),binder.get());		
		cse ->OnNewStatement(pwszUser, pwszStatement);
	}
	_TRACE("ChatSession::Fire_OnNewStatement <<");
}

// send the OnNewUser event to all listeners
void 
ChatSession::Fire_OnNewUser(const String16& pwszUser)
{
	_TRACE("ChatSession::Fire_OnNewUser >>this(%p), (%s,%s)",this,DISPLAY_STRING16(m_wszSessionName),DISPLAY_STRING16(pwszUser));
	Mutex::Autolock _lock(m_csAdviseLock);
	for (size_t i = 0; i < m_Listeners.size(); i++) 	{
		sp<IBinder> binder = m_Listeners.valueAt(i);
#if 1		
		sp<IChatSessionEvents> cse = interface_cast<IChatSessionEvents> (binder);
		_TRACE("**target(cse:%p binder:%p) ",cse.get(),binder.get());		
#else

		_TRACE("**test");

		_TRACE("**getInterfaceDescriptor:%s",DISPLAY_STRING16(binder->getInterfaceDescriptor()));
	        sp<IChatSessionEvents> intr = static_cast<IChatSessionEvents*>(   binder->queryLocalInterface(IChatSessionEvents::descriptor).get());  
		_TRACE("**intr:%p",intr.get());

		sp<IChatSessionEvents> cse = intr;
	/*	
		sp<IChatSessionEvents> cse = interface_cast<IChatSessionEvents> (binder);
		_TRACE("**target(cse:%p binder:%p) ",cse.get(),binder.get());		

		_TRACE("**curname:%s",DISPLAY_STRING16(cse->GetUserName()));*/
	

#endif				
		cse ->OnNewUser(pwszUser);
	}
	_TRACE("ChatSession::Fire_OnNewUser << ");
}

// send the OnUserLeft event to all listeners
void 
ChatSession::Fire_OnUserLeft(const String16& pwszUser)
{
	_TRACE("ChatSession::Fire_OnUserLeft >>this(%p),(%s,%s) ",this,DISPLAY_STRING16(m_wszSessionName),DISPLAY_STRING16(pwszUser));
	Mutex::Autolock _lock(m_csAdviseLock);
	for (size_t i = 0; i < m_Listeners.size(); i++) 	{
		sp<IBinder> binder = m_Listeners.valueAt(i);
		sp<IChatSessionEvents> cse = interface_cast<IChatSessionEvents> (binder);
		_TRACE("**target(cse:%p binder:%p) ",cse.get(),binder.get());
		cse ->OnUserLeft(pwszUser);
	}	
	_TRACE("ChatSession::Fire_OnUserLeft << ");
}



// helper method to check access to Say method
bool 
ChatSession::CheckAccess(const String16& szUser)
{
	return TRUE;
}


	
// IChatSession methods
String16 ChatSession::GetName()  const
{
	return m_wszSessionName;
}

status_t ChatSession::Say(const String16& statement)
{
	HRESULT hr = S_OK;
	String16 userName;
	int callerPid = GetCallerPID();
	_TRACE("ChatSession::Say(%s) >>",DISPLAY_STRING16(statement));	
	
	{
		Mutex::Autolock _lock(m_csAdviseLock);
		sp<IBinder> binder = m_Listeners.valueFor(callerPid);
		sp<IChatSessionEvents> eventSink = interface_cast<IChatSessionEvents>(binder);
		if(eventSink != NULL){
			_TRACE("**source(cse:%p  binder:%p) ",eventSink.get(),binder.get());
			userName  =  eventSink->GetUserName();
		}
	}

	if (userName.size() && CheckAccess(userName))
	{		
		{
			Mutex::Autolock _lock(m_csStatementLock);
			m_statements.add(userName + String16("\t:\t") +statement);
		}
		//Fire_OnNewStatement(userName, statement);
		ChatWorker::getInstance().Defer_OnNewStatement(this,userName, statement);
	}
	else
		hr = E_ACCESSDENIED;

	_TRACE("ChatSession::Say() <<hr:%x",hr);
	return hr;
}

void ChatSession::AsyncSay(const String16& statement){
	_TRACE("ChatSession::AsyncSay(%s) >>",DISPLAY_STRING16(statement));	
	Mutex::Autolock _lock(m_csStatementLock);
	m_statements.add(String16("\t:\t") +statement);
	ChatWorker::getInstance().Defer_OnNewStatement(this,String16("(Async)"), statement);
	_TRACE("ChatSession::AsyncSay() <<");
}

Vector<String16> 
ChatSession::GetStatements(void) const
{
	return m_statements;
}

void    ChatSession::Advise(const sp<IChatSessionEvents>& eventSink)
{
	int callerPid = GetCallerPID();

	_TRACE("ChatSession::Advise >>(%s,pid:%d)",DISPLAY_STRING16(m_wszSessionName),callerPid);	

	if (eventSink == NULL){
		_TRACE("ChatSession::Advise << E_INVALIDARG");
		return;
	}

	String16 userName = eventSink->GetUserName();

	if(userName.size()&& !m_bAllowAnonymousAccess){
		_TRACE("ChatSession::Advise << E_ACCESSDENIED");
		return;
	}

	 sp<IBinder> binder = eventSink->asBinder();
	 binder->linkToDeath(this);
	_TRACE("**ADD(cse:%p binder:%p) ",eventSink.get(),binder.get());

	_TRACE("add user %s",DISPLAY_STRING16(userName));
	Fire_OnNewUser(userName);
	//ChatWorker::getInstance().Defer_OnNewUser(this,userName);

	Mutex::Autolock _lock(m_csAdviseLock);
	m_Listeners.add(callerPid,binder);

	_TRACE("ChatSession::Advise << S_OK");
	return;
}


status_t ChatSession::Unadvise()
{
	String16 userName;
	int callerPid = GetCallerPID();

	_TRACE("ChatSession::Unadvise(%s,pid:%d)",DISPLAY_STRING16(m_wszSessionName),callerPid);
	
	{
		Mutex::Autolock _lock(m_csAdviseLock);
		sp<IBinder> binder = m_Listeners.valueFor(callerPid);
		sp<IChatSessionEvents> eventSink = interface_cast<IChatSessionEvents>(binder);
		if(eventSink != NULL){
			m_Listeners.removeItem(callerPid);
			userName  =  eventSink->GetUserName();
			_TRACE("**MOV(cse:%p binder:%p) ",eventSink.get(),binder.get());
		}
	}

	if(userName.size()){
		//ChatWorker::getInstance().Defer_OnUserLeft(this,userName);
		Fire_OnUserLeft(userName);
	}
	return NO_ERROR;
}



// class ChatSessionClass /////////////////////


	
ChatSessionClass::ChatSessionClass(void)
{
   // InitializeCriticalSection(&m_csSessionLock);
   ChatWorker::getInstance().run("chat worker");
}

ChatSessionClass::~ChatSessionClass(void)
{
//    DeleteCriticalSection(&m_csSessionLock);
}

// helper method to protect access to DeleteSession
// to only allow COMChat Admins to delete groups
bool 
ChatSessionClass::CheckAccess(const String16& pwszUser)
{
	return TRUE;
}

	
// IChatSessionManager methods
Vector<String16> ChatSessionClass::GetSessionNames(void) 
{
	Vector<String16> names;

	_TRACE("ChatSessionClass::GetSessionNames() >>");
	Mutex::Autolock _lock(m_csSessionLock);
	for(size_t i =0; i< m_sessions.size();i++)
		names.add(m_sessions.valueAt(i)->GetName());
	_TRACE("ChatSessionClass::GetSessionNames() <<");
	return names;

}




sp<IChatSession>  ChatSessionClass::FindSession(const String16& name, 
								BOOL bDontCreate,
								BOOL bAllowAnonymousAccess) 
                              
{
	sp<IChatSession> cs;
	//HRESULT hr = -1;

	_TRACE("ChatSessionClass::FindSession() >>");
	Mutex::Autolock _lock(m_csSessionLock);

	cs = m_sessions.valueFor(name);
	if(cs == NULL && bDontCreate)
	{
		_TRACE("ChatSessionClass::FindSession() << cannot find the session");
		return cs;
	}

	if(cs == NULL && !bDontCreate){
		cs = new ChatSession(name, bAllowAnonymousAccess != FALSE);
		_TRACE("create new ChatSession");
		if (cs  == NULL)
		{  
			_TRACE("ChatSessionClass::FindSession() << memory not enough");			
			return cs;
		}

		m_sessions.add(name,cs);
	}

//	if (!bAllowAnonymousAccess ){
//		LOGE("permission inadequet");
//		return cs;
//	}

	_TRACE("ChatSessionClass::FindSession() << %p",cs.get());			
	return cs;
}

status_t ChatSessionClass::DeleteSession(const String16& name)
{
	Mutex::Autolock _lock(m_csSessionLock);
	m_sessions.removeItem(name);
	return NO_ERROR;
}




};

using namespace android;
 extern "C" sp<IBinder>  load_service(){
 	return new ChatSessionClass();
 }

