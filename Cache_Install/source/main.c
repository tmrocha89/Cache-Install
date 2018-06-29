#include "ps4.h"
#include "patch.h"
#include "base64.h"

void systemMessage(char* msg) {
 	char buffer[512]; 
 	sprintf(buffer, "%s\n\n\n\n\n\n\n", msg);
 	sceSysUtilSendSystemNotificationWithText(0x81, buffer);
}


int file_exists(char *fname)
{
    int file = open(fname, O_RDONLY, 0);
    if (file != -1)
    {
        close(file);
        return 1;
    }
    return 0;
}


int dir_exists(char *dname)
{
    DIR *dir = opendir(dname);

    if (dir)
    {
        closedir(dir);
        return 1;
    }
    return 0;
}


void copyFile(char *sourcefile, char* destfile)
{
    int src = open(sourcefile, O_RDONLY, 0);
    if (src != -1)
    {
        int out = open(destfile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
        if (out != -1)
        {
             size_t bytes;
             char *buffer = malloc(65536);
             if (buffer != NULL)
             {
                 while (0 < (bytes = read(src, buffer, 65536)))
                     write(out, buffer, bytes);
                     free(buffer);
             }
             close(out);
         }
         else {
         }
         close(src);
     }
     else {
     }
}


void writeCacheDB(char *dbDir)
{
	size_t len;
	unsigned char* fbuf = base64_decode(istr, sizeof(istr), &len);
    int fid = open(dbDir, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    write(fid, fbuf, len);
    close(fid);
}


int _main(struct thread *td) {
	initKernel();
	initLibc();
	syscall(11,patcher,td);
    initSysUtil();
	if (!dir_exists("/user/system/webkit/webbrowser/appcache"))
	{
	    mkdir ("/user/system/webkit/webbrowser/appcache", 0777);
	}
	int usbdir = open("/mnt/usb0/.dirtest", O_WRONLY | O_CREAT | O_TRUNC, 0777);
         if (usbdir == -1)
            {
                usbdir = open("/mnt/usb1/.dirtest", O_WRONLY | O_CREAT | O_TRUNC, 0777);
                if (usbdir == -1)
                {
                    systemMessage("No usb found\nUsing Internal Cache");
					writeCacheDB("/user/system/webkit/webbrowser/appcache/ApplicationCache.db");
				    systemMessage("Cache install Complete.\n\nBookmark:\nhttp://cache/index.html");
                    return 0;
                }
                else
                {
                    close(usbdir);    
                    if (file_exists("/mnt/usb1/ApplicationCache.db")) 
                    { 
					systemMessage("Cache file found on USB1");
                    copyFile("/mnt/usb1/ApplicationCache.db","/user/system/webkit/webbrowser/appcache/ApplicationCache.db");
					systemMessage("Cache restore Complete.");
                    }
					else
					{
						systemMessage("No cache file found on USB1\nUsing Internal Cache");
						writeCacheDB("/user/system/webkit/webbrowser/appcache/ApplicationCache.db");
						systemMessage("Cache install Complete.\n\nBookmark:\nhttp://cache/index.html");
					}
                }
            }
            else
            {
                close(usbdir);
                if (file_exists("/mnt/usb0/ApplicationCache.db")) 
                {
					systemMessage("Cache file found on USB0");
                    copyFile("/mnt/usb0/ApplicationCache.db","/user/system/webkit/webbrowser/appcache/ApplicationCache.db");
					systemMessage("Cache restore Complete.");
                }
				else
				{
					systemMessage("No cache file found on USB0\nUsing Internal Cache");
					writeCacheDB("/user/system/webkit/webbrowser/appcache/ApplicationCache.db");
					systemMessage("Cache install Complete.\n\nBookmark:\nhttp://cache/index.html");
				}
            }
	return 0;
}