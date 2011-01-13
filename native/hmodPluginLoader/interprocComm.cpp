#include "interprocComm.h"

#ifdef WIN32
  #include <winsock2.h>
#else
  #include <arpa/inet.h>
#endif

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