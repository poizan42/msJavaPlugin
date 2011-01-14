#include "interprocComm.h"

#ifdef WIN32
  #include <winsock2.h>
#else
  #include <arpa/inet.h>
#endif

#include "hModPluginLoader.h"

InterprocComm::InterprocComm(FD_T inArg, FD_T outArg)
{
  in = inArg;
  out = outArg;
}

void InterprocComm::writeInt32(int32_t i)
{
  int32_t ib = htonl(i);
  WRITE(out, &ib, 4);
}

void InterprocComm::writeInt16(int16_t i)
{
  int16_t ib = htons(i);
  WRITE(out, &ib, 2);
}

void InterprocComm::writeInt8(int8_t i)
{
  WRITE(out, &i, 1);
}

void InterprocComm::writeFloat(float f)
{
  WRITE(out, &f, sizeof(float));
}

void InterprocComm::writeDouble(double f)
{
  WRITE(out, &f, sizeof(double));
}

void InterprocComm::writeBool(bool b)
{
  writeInt8(b);
}

void InterprocComm::writeString(std::string s)
{
  writeInt16(s.length());
  WRITE(out, s.c_str(), s.length());
}

int32_t InterprocComm::readInt32()
{
  int32_t ib;
  READ(in, &ib, 4);
  return ntohl(ib);
}

int16_t InterprocComm::readInt16()
{
  int16_t ib;
  READ(in, &ib, 2);
  return ntohs(ib);
}

int8_t InterprocComm::readInt8()
{
  char oc;
  READ(in, &oc, 1);
  return oc;
}

float InterprocComm::readFloat()
{
  float f;
  READ(in, &f, sizeof(float));
  return f;
}

double InterprocComm::readDouble()
{
  double f;
  READ(in, &f, sizeof(double));
  return f;
}

bool InterprocComm::readBool()
{
  return readInt8() != 0; //stupid MSVC giving a warning (C4800)
}

std::string InterprocComm::readString()
{
  int16_t len = readInt16();
  char* sc = new char[len+1];
  READ(in, sc, len);
  sc[len] = 0;
  std::string s(sc);
  delete sc;
  return s;
}

void InterprocComm::handleCommands(void)
{
  mineserver_pointer_struct* ms = hModPluginLoader::get()->getMineserver();
  while (true)
  {
    ClientCommandType cmd = (ClientCommandType)readInt32();
    switch (cmd)
    {
      case ClientCommand::intercom_return:
        return; //return value should be read by caller if expected
      case ClientCommand::plugin_hasPluginVersion:
      {
        std::string name = readString();
        bool retval = ms->plugin.hasPluginVersion(name.c_str());
        writeBool(retval);
        break;
      }
      case ClientCommand::plugin_getPluginVersion:
      {
        std::string name = readString();
        float retval = ms->plugin.getPluginVersion(name.c_str());
        writeFloat(retval);
        break;
      }
      case ClientCommand::plugin_setPluginVersion:
      {
        std::string name = readString();
        float version = readFloat();
        ms->plugin.setPluginVersion(name.c_str(), version);
        break;
      }
      case ClientCommand::plugin_remPluginVersion:
      {
        std::string name = readString();
        ms->plugin.remPluginVersion(name.c_str());
      }

      case ClientCommand::logger_log:
      {
        int32_t type = readInt32();
        std::string source = readString();
        std::string message = readString();
        ms->logger.log(
          type, source.c_str(), message.c_str()
        );
        break;
      }
    }
  }
}