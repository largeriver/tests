#include <binder/Parcel.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <utils/TextOutput.h>
#include "client.h"

#include <getopt.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
 

using namespace android;
  
// get the name of the generic interface we hold a reference to
static String16 get_interface_name(sp<IBinder> service)
{
    if (service != NULL) {
        Parcel data, reply;
        status_t err = service->transact(IBinder::INTERFACE_TRANSACTION, data, &reply);
        if (err == NO_ERROR) {
            return reply.readString16();
        }
    }
    return String16();
}

int main(int argc,char* const* argv){
	sp<IServiceManager> sm = defaultServiceManager();
	if (sm == NULL) {
		aerr << "service: Unable to get default service manager!" << endl;
		return 20;
	}

	bool wantsUsage = false;
	int result = 0;
    
    while (1) {
        int ic = getopt(argc, argv, "h?");
        if (ic < 0)
            break;

        switch (ic) {
        case 'h':
        case '?':
            wantsUsage = true;
            break;
        default:
            aerr << "service: Unknown option -" << ic << endl;
            wantsUsage = true;
            result = 10;
            break;
        }
    }

    if (optind >= argc) {
        wantsUsage = true;
    }

    if (wantsUsage) {
        aout << "Usage: " << argv[0]  << " [-h|-?]\n"
                "       service call CODE [i32 INT | s16 STR] ...\n"
                "Options:\n"
                "   i32: Write the integer INT into the send parcel.\n"
                "   s16: Write the UTF-16 string STR into the send parcel.\n";
        return result;
    }
    
	
	sp<IBinder> service = sm->checkService(String16("guilh.add"));
	if (service == 0) {
		LOGW("AddService not published, waiting...");
		return 21;
	}

	String16 ifName = get_interface_name(service);
	

	Parcel data, reply;
	data.writeInterfaceToken(ifName);
	data.writeInt32(getpid());


	int32_t code = atoi(argv[optind++]);
	status_t ret = NO_ERROR;

	switch(code){
	case 0:
		data.writeInt32(atoi(argv[optind++]));

		aerr << "transact >>\n" << endl;

		ret = service->transact(0, data, &reply);

		if(ret >=0)
			aout << "transact << successed: " << reply.readInt32() << endl;
		else
			aerr << "reanscat << failed: " << ret << endl;
		break;
	case 1:
		{
			const char * path = argv[optind++];
			int fd = open(path,O_RDWR |O_CREAT | O_APPEND);
			if(fd <= 0){
				aerr << "failed to open file:" << path << endl;
				return -10;
			}
			data.writeDupFileDescriptor(fd);
			close(fd);

			aerr << "transact >>\n" << endl;
			ret = service->transact(1, data, &reply);
			aerr << "transact << Result: " << ret << endl;
		}
		break;
	default:
		return -1;
	}
		
	return 0;
}


