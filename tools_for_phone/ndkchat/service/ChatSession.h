/////////////////////////////////////////////////////
//
// ChatSession.h
//
// Copyright 1997, Don Box/Addison Wesley
// Copyright 2009 Jokul
//
// This code accompanies the book "The Component
// Object Model" from Addison Wesley. Blah blah blah
//
//

#ifndef _CHATSESSION_H
#define _CHATSESSION_H

#include "../comchat.h"
#include <utils/threads.h>
#include <utils/KeyedVector.h>

namespace android{
class ChatWorkThread;
class ChatSession : public IBinder::DeathRecipient,public BnChatSession
{
	Mutex    m_csStatementLock;
	Mutex    m_csAdviseLock;

	String16		m_wszSessionName;
	bool	            m_bIsDeleted;
	bool             m_bAllowAnonymousAccess;

	Vector< String16>    m_statements;	
	
//	DefaultKeyedVector<int, sp<IChatSessionEvents> >    m_Listeners;	
	DefaultKeyedVector<int, sp<IBinder> >    m_Listeners;	


	bool CheckAccess(const String16& pwszUser);

protected:
	virtual ~ChatSession(void);
	void Fire_OnNewStatement(const String16& pwszUser,
	const String16& pwszStatement);
	void Fire_OnNewUser(const String16& pwszUser);
	void Fire_OnUserLeft(const String16& pwszUser);

public:
	ChatSession(const String16& pwszSessionName,
						bool bAllowAnonymousAccess);

	// IBinder::DeathRecipient
	virtual     void        binderDied(const wp<IBinder>& who);
		
	void Disconnect(void);


	virtual String16    GetName(void) const;
	virtual status_t    Say(const String16& statement);
	virtual void    AsyncSay(const String16& statement);

	virtual Vector<String16>    GetStatements(void) const;

	virtual void    Advise(const sp<IChatSessionEvents>& eventSink) ;
	virtual status_t    Unadvise();	

	friend class ChatWorkThread;
};



// this class models the management of chat sessions
// and acts as the class object for CLSID_ChatSession
class ChatSessionClass : public BnChatSessionManager
{
public:
	/*
	//本宏实现类对象的注册和反注册
	DEFAULT_CLASS_REGISTRY_TABLE(ChatSessionClass, 
		"{5223A053-2441-11d1-AF4F-0060976AA886}",
//		"CLSID\\{5223A053-2441-11d1-AF4F-0060976AA886}",
		"ChatServer", 
		"Chat.Server.1", 
		"Chat.Server", 
		"both")
		
		////实现IUnknown
		IMPLEMENT_UNKNOWN(ChatSessionClass)

		//定义支持的接口表
		BEGIN_INTERFACE_TABLE(ChatSessionClass)
			IMPLEMENTS_INTERFACE(IChatSessionManager)
			IMPLEMENTS_INTERFACE(IExternalConnection)
		END_INTERFACE_TABLE()

		//
		// implement static CreateInstance(IUnknown *, REFIID, void**)
		static HRESULT STDAPICALLTYPE CreateInstance(IUnknown *pUnkOuter,REFIID riid, void**ppv);

	// implement static GetClassObject(REFIID, void **) that uses a GenericClassFactory
	IMPLEMENT_GENERIC_CLASS_FACTORY(ChatSessionClass)*/
private:
	DefaultKeyedVector<String16 , sp<IChatSession> >    m_sessions;
	//    LONG                m_cStrongLocks;	
	Mutex    	m_csSessionLock;
	bool CheckAccess(const String16& pwszUser);
public:
	virtual ~ChatSessionClass(void);
	ChatSessionClass(void);

	virtual Vector<String16> GetSessionNames(void) ;
	virtual sp<IChatSession>  FindSession(const String16& name, 
								BOOL bDontCreate,
								BOOL bAllowAnonymousAccess) ;
	virtual status_t DeleteSession(const String16& name);
	
};



};
#endif
