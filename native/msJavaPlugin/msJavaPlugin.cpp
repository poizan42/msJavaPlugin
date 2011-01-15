#include "msJavaPlugin.h"

#ifdef WIN32
  #include <windows.h>
  #include <winsock2.h>
#else
  #include <stdio.h>
  #include <unistd.h>
  #include <arpa/inet.h>
#endif

#include "plugin_api.h"
#include "logtype.h"

#define PLUGIN_VERSION 0.1
#define _QUOTE_STR__(x) #x
#define _QUOTE_STR_(x) _QUOTE_STR__(x)
#define PLUGIN_VERSION_STR _QUOTE_STR_(PLUGIN_VERSION) 
#define PLUGIN_NAME "hModPluginLoader"
#define JAVA_CLASS_NAME "hModPluginLoader.hModPluginLoader"
#define SOCKET_PORT 38830

PLUGIN_API_EXPORT void CALLCONVERSION hModPluginLoader_init(mineserver_pointer_struct* mineserver)
{
	if (mineserver->plugin.getPluginVersion(PLUGIN_NAME) > 0)
	{
		std::string msg = PLUGIN_NAME " is already loaded v." +
			dtos(mineserver->plugin.getPluginVersion(PLUGIN_NAME));
		mineserver->logger.log(LogType::LOG_ERROR, PLUGIN_NAME, msg.c_str());
		return;
	}
    mineserver->plugin.setPluginVersion(PLUGIN_NAME, PLUGIN_VERSION);
    new MSJavaPlugin(mineserver);
}

PLUGIN_API_EXPORT void CALLCONVERSION hModPluginLoader_shutdown(void)
{
	delete MSJavaPlugin::get();
}

#ifdef WIN32
inline int getSockErr()
{
  return WSAGetLastError();
}

inline std::string getErrorMessage(int err)
{
  char* msg;
  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, err, 0, (LPSTR)&msg, 1, NULL);
  std::string s(msg);
  LocalFree(msg);
  return s;
}

inline std::string getLastErrorMessageSock()
{
  return getErrorMessage(WSAGetLastError());
}
#endif

MSJavaPlugin::MSJavaPlugin(mineserver_pointer_struct* mineserver)
{
	instance = this;
	this->mineserver = mineserver;
	mineserver->logger.log(LogType::LOG_INFO, PLUGIN_NAME,
		"Loaded "PLUGIN_NAME" v."PLUGIN_VERSION_STR);

#ifdef USE_SOCKETS
#ifdef WIN32
    WSADATA wsaData;
    // Initialize Winsock
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(res != 0)
    {
      mineserver->logger.log(LogType::LOG_ERROR, JAVA_CLASS_NAME,
        (std::string("WSAStartup failed: ")+getErrorMessage(res)).c_str() );
      return;
    }
#endif

    FD_T s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(SOCKET_PORT);
    if (connect(s, (const sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
      mineserver->logger.log(LogType::LOG_ERROR, JAVA_CLASS_NAME,
        (std::string("Connection to java server failed: ")+
         getLastErrorMessageSock()).c_str() );
      return;
    }
    ic = new InterprocComm(s, s);
#else
    //0=read,1=write
    FD_T stdinP[2], stdoutP[2];
#ifdef WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = true;
    sa.lpSecurityDescriptor = NULL;

    CreatePipe(&stdinP[0], &stdinP[1], &sa, 0);
    CreatePipe(&stdoutP[0], &stdoutP[1], &sa, 0);

    //si.dwFlags = 0;
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = stdinP[0];
    si.hStdOutput = stdoutP[1];
    si.hStdError = GetStdHandle(STD_OUTPUT_HANDLE);

    if (!CreateProcess(
      NULL, (LPSTR)(std::string("java ")+JAVA_CLASS_NAME+" -pipe").c_str(),
      NULL, NULL, true, 0, NULL,NULL, &si, &pi))
    {
      DWORD err = GetLastError();
      std::string msg = getErrorMessage(err);
      mineserver->logger.log(LogType::LOG_ERROR, JAVA_CLASS_NAME,
        (std::string("CreateProcess failed: ")+msg).c_str());
      return;
    }
#else
    pid_t cpid;
    pipe(pfd); //!= -1
    cpid = fork(); //!= -1
    if (cpid == 0) {    /* Child reads from pipe */
        close(stdinP[1]); // Close unused write end of stdin
        close(stoutP[0]); // and read end of stdout
        dup2(stdinP[0], STDIN_FILENO);
        dup2(stdoutP[1], STDOUT_FILENO);
        execlp("java", PLUGIN_NAME, "-pipe");
    }
    close(stdinP[0]);
    close(stdoutP[1]);
#endif
    ic = new InterprocComm(stdoutP[0], stdinP[1]);
#endif
    //call init
    ic->writeInt32(ServerCommand::init);
    ic->handleCommands();
}

MSJavaPlugin::~MSJavaPlugin(void)
{
  delete ic;
  mineserver->logger.log(LogType::LOG_INFO, PLUGIN_NAME,
		"Unloaded "PLUGIN_NAME" v."PLUGIN_VERSION_STR);
}

MSJavaPlugin* MSJavaPlugin::instance;