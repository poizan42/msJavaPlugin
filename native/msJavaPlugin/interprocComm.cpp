#include "interprocComm.h"

#ifdef WIN32
  #include <winsock2.h>
#else
  #include <arpa/inet.h>
#endif

#include "msJavaPlugin.h"

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
  mineserver_pointer_struct* ms = MSJavaPlugin::get()->getMineserver();
  while (true)
  {
    ClientCommandType cmd = (ClientCommandType)readInt32();
    switch (cmd)
    {
      case ClientCommand::intercom_return:
        return; //return value should be read by caller if expected
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

      //*************** codegen.py output below ***************
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
        break;
      }
      case ClientCommand::plugin_hasPointer:
      {
        std::string name = readString();
        bool retval = ms->plugin.hasPointer(name.c_str());
        writeBool(retval);
        break;
      }
      case ClientCommand::plugin_remPointer:
      {
        std::string name = readString();
        ms->plugin.remPointer(name.c_str());
        break;
      }
      case ClientCommand::plugin_hasHook:
      {
        std::string hookID = readString();
        bool retval = ms->plugin.hasHook(hookID.c_str());
        writeBool(retval);
        break;
      }
      case ClientCommand::plugin_remHook:
      {
        std::string hookID = readString();
        ms->plugin.remHook(hookID.c_str());
        break;
      }
      case ClientCommand::user_teleport:
      {
        std::string user = readString();
        double x = readDouble();
        double y = readDouble();
        double z = readDouble();
        bool retval = ms->user.teleport(user.c_str(), x, y, z);
        writeBool(retval);
        break;
      }
      case ClientCommand::user_getPosition:
      {
        std::string user = readString();
        double x;
        double y;
        double z;
        float yaw;
        float pitch;
        double stance;
        bool retval = ms->user.getPosition(user.c_str(), &x, &y, &z, &yaw, &pitch, &stance);
        writeDouble(x);
        writeDouble(y);
        writeDouble(z);
        writeFloat(yaw);
        writeFloat(pitch);
        writeDouble(stance);
        writeBool(retval);
        break;
      }
      case ClientCommand::user_sethealth:
      {
        std::string user = readString();
        int32_t userHealth = readInt32();
        bool retval = ms->user.sethealth(user.c_str(), userHealth);
        writeBool(retval);
        break;
      }
      case ClientCommand::chat_sendmsgTo:
      {
        std::string user = readString();
        std::string msg = readString();
        bool retval = ms->chat.sendmsgTo(user.c_str(), msg.c_str());
        writeBool(retval);
        break;
      }
      case ClientCommand::chat_sendmsg:
      {
        std::string msg = readString();
        bool retval = ms->chat.sendmsg(msg.c_str());
        writeBool(retval);
        break;
      }
      case ClientCommand::chat_sendUserlist:
      {
        std::string user = readString();
        bool retval = ms->chat.sendUserlist(user.c_str());
        writeBool(retval);
        break;
      }
      case ClientCommand::map_createPickupSpawn:
      {
        int32_t x = readInt32();
        int32_t y = readInt32();
        int32_t z = readInt32();
        int32_t type = readInt32();
        int32_t count = readInt32();
        int32_t health = readInt32();
        std::string user = readString();
        ms->map.createPickupSpawn(x, y, z, type, count, health, user.c_str());
        break;
      }
      case ClientCommand::map_setTime:
      {
        int32_t timeValue = readInt32();
        bool retval = ms->map.setTime(timeValue);
        writeBool(retval);
        break;
      }
      case ClientCommand::map_getSpawn:
      {
        int32_t x;
        int32_t y;
        int32_t z;
        ms->map.getSpawn(&x, &y, &z);
        writeInt32(x);
        writeInt32(y);
        writeInt32(z);
        break;
      }
      case ClientCommand::map_getBlock:
      {
        int32_t x = readInt32();
        int32_t y = readInt32();
        int32_t z = readInt32();
        uint8_t type;
        uint8_t meta;
        bool retval = ms->map.getBlock(x, y, z, &type, &meta);
        writeInt8(type);
        writeInt8(meta);
        writeBool(retval);
        break;
      }
      case ClientCommand::map_setBlock:
      {
        int32_t x = readInt32();
        int32_t y = readInt32();
        int32_t z = readInt32();
        uint8_t type = readInt8();
        uint8_t meta = readInt8();
        bool retval = ms->map.setBlock(x, y, z, type, meta);
        writeBool(retval);
        break;
      }
      case ClientCommand::map_saveWholeMap:
      {
        ms->map.saveWholeMap();
        break;
      }
      case ClientCommand::config_has:
      {
        std::string name = readString();
        bool retval = ms->config.has(name.c_str());
        writeBool(retval);
        break;
      }
      case ClientCommand::config_iData:
      {
        std::string name = readString();
        int32_t retval = ms->config.iData(name.c_str());
        writeInt32(retval);
        break;
      }
      case ClientCommand::config_fData:
      {
        std::string name = readString();
        float retval = ms->config.fData(name.c_str());
        writeFloat(retval);
        break;
      }
      case ClientCommand::config_dData:
      {
        std::string name = readString();
        double retval = ms->config.dData(name.c_str());
        writeDouble(retval);
        break;
      }
      case ClientCommand::config_sData:
      {
        std::string name = readString();
        std::string retval = ms->config.sData(name.c_str());
        writeString(retval);
        break;
      }
      case ClientCommand::config_bData:
      {
        std::string name = readString();
        bool retval = ms->config.bData(name.c_str());
        writeBool(retval);
        break;
      }
    }

  }
}