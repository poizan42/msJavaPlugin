#pragma once

#include <string>

#include <stdint.h>

#ifdef WIN32
  #define ssize_t int
  //Fix Winsock2 error that occurs when Windows.h is included before it.
  #define _WINSOCKAPI_
  #include <windows.h>
#else
  #include <unistd.h>
  #define FD_T int
  #define READ(fd,buf,count) read(fd,buf,count)
  #define WRITE(fd,buf,count) write(fd,buf,count)
#endif //WIN32

#ifdef USE_SOCKETS
  #ifdef WIN32
    #define FD_T SOCKET
    #include <winsock2.h>
  #endif

inline ssize_t READ(FD_T fd, void *buf, ssize_t count)
{
  char* b = (char*)buf;
  int readCount = 0;
  int c;
  while (readCount < count)
  {
    c = recv(fd, b+readCount, count-readCount, 0);
    if (c < 1)
      return c;
    readCount += c;
  }
  return count;
}

#define WRITE(fd,buf,count) send(fd, (char *)(buf), count, 0)

#else

    #define FD_T HANDLE
inline size_t READ(HANDLE fd, void *buf, size_t count)
{
  DWORD readCount = 0;
  ReadFile(fd, buf, count, &readCount, NULL);
  return readCount;
}

inline size_t WRITE(HANDLE fd, const void *buf, size_t count)
{
  DWORD writtenCount = 0;
  WriteFile(fd, buf, count, &writtenCount, NULL);
  return writtenCount;
}
#endif //USE_SOCKETS

namespace ClientCommand {
  enum ClientCommandType {
    intercom_return = 0,

    user_teleport,
    user_getPosition,
    user_sethealth,

    chat_sendmsgTo,
    chat_sendmsg,
    chat_sendUserlist,

    logger_log,

    map_createPickupSpawn,
    map_setTime,
    map_getSpawn,
    map_getBlock,
    map_setBlock,
    map_saveWholeMap,
    map_getMapData_block,
    map_getMapData_meta,
    map_getMapData_skylight,
    map_getMapData_blocklight,

    config_has,
    config_iData,
    config_lData,
    config_fData,
    config_dData,
    config_sData,
    config_bData,

    plugin_getPluginVersion,
    plugin_setPluginVersion,
    plugin_addCallback,
  };
}

typedef ClientCommand::ClientCommandType ClientCommandType;

namespace ServerCommand
{
  enum {init = -1, shutdown = -2};
}

class InterprocComm
{
public:
  InterprocComm(FD_T in, FD_T out);

  void writeInt32(int32_t i);
  void writeInt16(int16_t i);
  void writeInt8(int8_t i);
  void writeFloat(float f);
  void writeString(std::string s);

  int32_t readInt32(void);
  int16_t readInt16(void);
  int8_t readInt8(void);
  float readFloat(void);
  std::string readString(void);
private:
  FD_T out;  
  FD_T in;
};