#include <binder/IInterface.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>

#include <utils/Log.h>
#include <utils/String8.h>
#include <utils/String16.h>

#include <string.h>
#include <ctype.h>

#include "../comchat.h"

using namespace android;


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

static void Error(HRESULT hr, const char *psz)
{
    printf("%s failed and returned 0x%x\n", psz, hr);
}

// utility function to print command line syntax
static int Usage(void)
{
   const char *psz = 
    "usage: client.exe <action> <user>\n"
    "   where:\n"
    "          action = /sessions|/chat:session|/delete:session\n"
    "          user = /user:name\n"
                "/anonymous | <nothing>\n";
   aout << psz << endl;
   return -1;
}

static const char*  _trim_string(char* str)
{
    char* p = str;
    while (*p && isspace(*p)) {
        p++;
    }
    char* q = str + strlen(str) - 1;
    while (q > p && isspace(*q)) {
        q--;
    }
    *(++q) = 0;
    return p;
}

// utility function to print initial state of 
// a chat session
static void PrintToDate(const sp<IChatSession> &pcs)
{
    Vector<String16> statements = pcs->GetStatements();
    aout << "Found " << statements.size() << " statements:" << endl;
    for (unsigned i = 0; i < statements.size(); i++) {
        aout << good_old_string(statements[i])  << endl;
    }
}

// this class implements the callback interface
// that receives chat notifications. It simply 
// prints the event to the console
class EventSink : public IBinder::DeathRecipient,public BnChatSessionEvents
{
public:
	// DeathRecipient
	virtual void binderDied(const wp<IBinder>& who){
	    LOGW("Chat server died!");
	}
			
    status_t Sync_OnNewStatement(const String16& pwszUser,
                                const String16& pwszStmt)
    {
	aout << pwszUser << "	::	" << pwszStmt << endl;
	return NO_ERROR;
    }
    void OnNewStatement(const String16& pwszUser,
                                const String16& pwszStmt)
    {
	aout << pwszUser << "	:	" << pwszStmt << endl;
    }
	
    void OnNewUser(const String16& pwszUser)
    {
	aout << "\n\n>>> Say Hello to" << pwszUser << endl;
    }
    void OnUserLeft(const String16& pwszUser)
    {
		aout << "\n\n>>> Say Bye to" << pwszUser << endl;
    }

    String16   GetUserName(void) const{
		return mUserName;
	}

	EventSink(const String16& userName):mUserName(userName)
	{
	}
private:
	~EventSink(){
		LOGE("~EventSink");
	}
	String16 mUserName;
    
};

// type of operations this client can perform
enum ACTION
{
    ACTION_NONE,
    ACTION_CHAT,
    ACTION_DELETE_SESSION,
    ACTION_LIST_SESSION_NAMES,
};

static sp<EventSink> gEventSink;

// run chat command
static void Chat(const String16& pwszSession,const String16& userName,const sp<IChatSessionManager> &pcsm) 
{
	HRESULT hr = 0;
	sp<IChatSession> pcs = pcsm->FindSession(pwszSession, FALSE, TRUE);
	if (pcs == NULL){
		Error(hr, "FindSession");
		return;
	}
	
	// catch up on past messages
	PrintToDate(pcs);
	// hook up event sink to receive new messages
	gEventSink = new EventSink(userName);//hold 一个全局的指针? 防止被客户被析构?
	
	sp<IChatSessionEvents> es = gEventSink;

	sp<IBinder> binder = pcs->asBinder();
	binder->linkToDeath(gEventSink);
	pcs->Advise(es);

	
	// run UI loop to get statements from console and send them
	char cszStmt[4096];
	printf("Please input message,say \"byebye\" to exit room\n");
	while (fgets(cszStmt,4096,stdin))
	{
		if(cszStmt[0] == 0)
			continue;
		if(strcmp(cszStmt,"byebye\n") == 0)
			break;

#if 0
		pcs->AsyncSay(String16(cszStmt));
#else
		hr = pcs->Say(String16(cszStmt));
		if (FAILED(hr))
			Error(hr, "Say");
#endif			
	}
	// tear down connection for event sink
	pcs->Unadvise();
		
}

// run delete command
static void Delete(const String16& pwszSession,
           const  sp<IChatSessionManager>& pcsm)
{
    HRESULT hr = pcsm->DeleteSession(pwszSession);
    if (FAILED(hr))
        Error(hr, "DeleteSession");
}

// run list command
static void List(const sp<IChatSessionManager>& pcsm)
{
    Vector<String16> sessions = pcsm->GetSessionNames();
    aout << "Found " << sessions.size() << " services:" << endl;
    for (unsigned i = 0; i < sessions.size(); i++) {
        String16 name = sessions[i];
        aout << i 
             << "\t" << good_old_string(name) 
             << endl;
    }
}

int main(int argc, char **argv)
{
	bool bAnonymous = false;
	static String16 wszSessionName;
	static String16 wszUserName;

	static ACTION action = ACTION_NONE;

	android::ProcessState::self()->startThreadPool();

	for (int i = 1; i < argc; i++)	{
		if (strcmp(argv[i], "/anonymous") == 0)
			bAnonymous = true;
		else if (strstr(argv[i], "/delete:") == argv[i])
		{
			if (action != ACTION_NONE)
				return Usage();
			action = ACTION_DELETE_SESSION;
			wszSessionName = String16(_trim_string(argv[i] + 8));
		}
		else if (strstr(argv[i], "/chat:") == argv[i])
		{
			if (action != ACTION_NONE)
				return Usage();
			action = ACTION_CHAT;
			wszSessionName = String16(_trim_string(argv[i] + 6));
		}
		else if (strcmp(argv[i], "/sessions") == 0)
		{
			if (action != ACTION_NONE)
				return Usage();
			action = ACTION_LIST_SESSION_NAMES;
		}
		else if (strstr(argv[i], "/user:") == argv[i])
		{
			if (bAnonymous)
				return Usage();
			wszUserName = String16(_trim_string(argv[i] + 6));			
		}
	}

	if (action == ACTION_NONE)
		return Usage();


	sp<IServiceManager> sm = defaultServiceManager();
	if (sm == NULL) {
		fprintf(stderr, "error: can't get default service manager\n");
		return 1;
	}	
	sp<IChatSessionManager> pcsm = interface_cast<IChatSessionManager>(sm->checkService(String16("android.os.IChatSessionManager")));

	switch (action)
	{
		case ACTION_CHAT:
			Chat(wszSessionName, wszUserName, pcsm);
			break;
		case ACTION_DELETE_SESSION:
			Delete(wszSessionName, pcsm);
			break;
		case ACTION_LIST_SESSION_NAMES:
			List(pcsm);
			break;
		default:
			Usage();
	}


	return 0;
}
