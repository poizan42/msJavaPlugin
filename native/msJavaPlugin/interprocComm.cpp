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

InterprocComm::~InterprocComm()
{
  std::list<CallbackData*>::iterator it;
  for (it = callbackStructs.begin(); it != callbackStructs.end(); it++)
  {
    delete *it;
  }
}

void InterprocComm::writeInt64(int64_t i)
{
  int32_t ih = (int32_t)(i >> 32);
  int32_t il = (int32_t)(i & 0xFFFFFFFF);
  WRITE(out, &ih, 4);
  WRITE(out, &il, 4);
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

int64_t InterprocComm::readInt64()
{
  int32_t ih;
  int32_t il;
  READ(in, &ih, 4);
  READ(in, &il, 4);
  return (int64_t)ih << 32 | (int64_t)il;
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

bool CALLCONVERSION InterprocComm::_callbackFun(CallbackData* d, ...)
{
  va_list args;
  va_start(args, d);
  bool retval = d->ic->doCallback(d->javaFun, d->types, args);
  va_end(args);
  return retval;
}

bool InterprocComm::doCallback(int32_t javaFun, std::vector<InterprocType> types, va_list args)
{
  //Tell java part to call javaFun
  writeInt32(javaFun);
  for (unsigned int i = 0; i < types.size(); i++)
  {
    switch (types[i])
    {
      case InterprocTypes::int8:
        writeInt8(va_arg(args, int8_t));
        break;
      case InterprocTypes::int16:
        writeInt16(va_arg(args, int16_t));
        break;
      case InterprocTypes::int32:
        writeInt32(va_arg(args, int32_t));
        break;
      case InterprocTypes::int64:
        writeInt64(va_arg(args, int64_t));
        break;
      case InterprocTypes::floatType:
        writeFloat(va_arg(args, float));
        break;
      case InterprocTypes::doubleType:
        writeDouble(va_arg(args, double));
        break;
      case InterprocTypes::boolType:
        writeBool(va_arg(args, bool));
        break;
      case InterprocTypes::string:
        writeString(std::string(va_arg(args, char*)));
        break;
    } 
  }
  handleCommands();
  return readBool();
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
      /*getPointer/setPointer probably not very useful
        for java plugins. But here they are:
      */
      case ClientCommand::plugin_getPointer: 
      {
        std::string name = readString();
        void* retval = ms->plugin.getPointer(name.c_str());
        writeInt64((int64_t)retval);
        break;
      }
      case ClientCommand::plugin_setPointer: 
      {
        std::string name = readString();
        void* pointer = (void*)readInt64();
        ms->plugin.setPointer(name.c_str(), pointer);
        break;
      }
      /*The hard stuff: callbacks!
        The format we receives this from the java part is as follows:
        string HookID
        int32 function id
        int32 argument count
        int8 type of arg1
        ...
        int8 type of argn
      */
      case ClientCommand::plugin_addCallback:
      {
        std::string hookID = readString();
        CallbackData* cb = new CallbackData;
        cb->ic = this;
        cb->javaFun = readInt32();
        int argCount = readInt32();
        cb->types.reserve(argCount);
        for (int i = 0; i < argCount; i++)
        {
          cb->types.push_back((InterprocType)readInt8());
        }
        callbackStructs.push_back(cb);
        ms->plugin.addIdentifiedCallback(hookID.c_str(), cb, _callbackFun);
        break;
      }
      case ClientCommand::plugin_doUntilTrue:
      {
        std::string hookID = readString();

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
//  Hook* (*getHook)(const char* hookID);
//struct="plugin", ret="Hook*", name="getHook", args=[('const char*', 'hookID')]
//Unknown return type: "Hook*". Not generating code!

//  void  (*setHook)(const char* hookID, Hook* hook);
//struct="plugin", ret="void", name="setHook", args=[('const char*', 'hookID'), ('Hook*', 'hook')]
//Unknown argument type: "Hook*" for argument "hook". Not generating code!

      case ClientCommand::plugin_remHook:
      {
        std::string hookID = readString();
        ms->plugin.remHook(hookID.c_str());
        break;
      }
//  bool (*hasCallback)          (const char* hookID, void* function);
//struct="plugin", ret="bool", name="hasCallback", args=[('const char*', 'hookID'), ('void*', 'function')]
//Unknown argument type: "void*" for argument "function". Not generating code!

//  void (*addCallback)          (const char* hookID, void* function);
//struct="plugin", ret="void", name="addCallback", args=[('const char*', 'hookID'), ('void*', 'function')]
//Unknown argument type: "void*" for argument "function". Not generating code!

//  void (*addIdentifiedCallback)(const char* hookID, void* identifier, void* function);
//struct="plugin", ret="void", name="addIdentifiedCallback", args=[('const char*', 'hookID'), ('void*', 'identifier'), ('void*', 'function')]
//Unknown argument type: "void*" for argument "identifier". Not generating code!

//  void (*remCallback)          (const char* hookID, void* function);
//struct="plugin", ret="void", name="remCallback", args=[('const char*', 'hookID'), ('void*', 'function')]
//Unknown argument type: "void*" for argument "function". Not generating code!

//  bool (*doUntilTrue)          (const char* hookID, ...);
//struct="plugin", ret="bool", name="doUntilTrue", args=[('const char*', 'hookID'), ('', '...')]
//Unknown argument type: "" for argument "...". Not generating code!

//  bool (*doUntilFalse)         (const char* hookID, ...);
//struct="plugin", ret="bool", name="doUntilFalse", args=[('const char*', 'hookID'), ('', '...')]
//Unknown argument type: "" for argument "...". Not generating code!

//  void (*doAll)                (const char* hookID, ...);
//struct="plugin", ret="void", name="doAll", args=[('const char*', 'hookID'), ('', '...')]
//Unknown argument type: "" for argument "...". Not generating code!

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
//  unsigned char* (*getMapData_block)(int x, int z);
//struct="map", ret="unsigned char*", name="getMapData_block", args=[('int', 'x'), ('int', 'z')]
//Unknown return type: "unsigned char*". Not generating code!

//  unsigned char* (*getMapData_meta) (int x, int z);
//struct="map", ret="unsigned char*", name="getMapData_meta", args=[('int', 'x'), ('int', 'z')]
//Unknown return type: "unsigned char*". Not generating code!

//  unsigned char* (*getMapData_skylight)  (int x, int z);
//struct="map", ret="unsigned char*", name="getMapData_skylight", args=[('int', 'x'), ('int', 'z')]
//Unknown return type: "unsigned char*". Not generating code!

//  unsigned char* (*getMapData_blocklight)(int x, int z);
//struct="map", ret="unsigned char*", name="getMapData_blocklight", args=[('int', 'x'), ('int', 'z')]
//Unknown return type: "unsigned char*". Not generating code!

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
      case ClientCommand::config_lData:
      {
        std::string name = readString();
        int64_t retval = ms->config.lData(name.c_str());
        writeInt64(retval);
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