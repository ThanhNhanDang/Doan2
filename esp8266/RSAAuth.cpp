/*
  DateTime.cpp - Arduino Date and Time library
  Copyright (c) Michael Margolis.  All right reserved.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

extern "C" {
  // AVR LibC Includes
}

#include "RSAAuth.h"


SoftwareSerial swSer(D2, D4, false, 256); //D2, D1 = SRX, STX
RSAAuth::RSAAuth()
{

}


byte RSAAuth::GenCrc(byte en[], int n)
{
  long tong = 0;
  for (int i = 0; i < n; i++)
  {
    tong += en[i];
    tong %= 253;
  }
  return (byte)tong;
}
int RSAAuth::Package(byte en[], int n, byte pac[])
{
  int i;
  byte crc = GenCrc(en, n);
  pac[0] = 255;
  for (i = 0; i < n; i++)
  {
    pac[i + 1] = (byte)en[i];
  }
  pac[i + 1] = crc;
  pac[i + 2] = 254;
  return n + 3;
}
int RSAAuth::OpenPackage(byte pac[], int m, byte en[])
{
  if (pac[0] != 255 || pac[m - 1] != 254) return -1;
  byte crc = GenCrc(pac + 1, m - 3);
  if (crc != pac[m - 2]) return -1;
  for (int i = 1; i < m - 2; i++)
    en[i - 1] = pac[i];
  return m - 3;
}
bool RSAAuth::checkArray(byte arr[], int*BEGIN, int*END)
{
  bool dau = false, cuoi = false;
  for (int i = *BEGIN; i != *END; i++, i %= 255)
  {

    if (arr[i] == 255) dau = true;
    if (arr[i] == 254 && dau == true) return true;
  }
  return false;
}
bool RSAAuth::CutString(byte arr[], int*BEGIN, int*END, byte desArr[], int *pDesLen)
{

  if (checkArray(arr, BEGIN, END) == true)
  {
    //Serial.println("Nhan:");
    //Serial.println(*BEGIN);
    //Serial.println(*END);
    while (arr[*BEGIN] != 255) //start
    {
      (*BEGIN)++;
      (*BEGIN) %= 255;
    }

    *pDesLen = 0;
    while (arr[*BEGIN] != 254) //end
    {
      desArr[(*pDesLen)++] = arr[(*BEGIN)++];
      (*BEGIN) %= 255;
    }

    desArr[(*pDesLen)++] = arr[(*BEGIN)++];
    (*BEGIN) %= 255;
    return true;
  }
  return false;
}
void RSAAuth::Send(char cstr[])
{


  int len;//=encrypt(cstr,en);
  for (len = 0; cstr[len] != 0; len++)
    en[len] = cstr[len];
  en[len] = 0;
  len = Package(en, len, pac);
  int i = 0;
  while (i < len) {
    swSer.write(pac[i++]);
    yield();
  }

}

bool RSAAuth::Receive(byte buff[], int*BEGIN, int*END, byte m[], int *pDesLen)
{
  int i;
  while (swSer.available() > 0 && (*END + 1) % 255 != *BEGIN) {
    buff[(*END)++] = swSer.read();
    (*END) %= 255;
    yield();
  }

  if (CutString(buff, BEGIN, END, desArr, pDesLen))
  {
    Serial.print("Nhan :");
    //    Serial.println((char*)buff);
    *pDesLen = OpenPackage(desArr, *pDesLen, unpac);
    if (*pDesLen >= 0)
    {
      desArr[*pDesLen] = 0;
      //rsa.decrypt(unpac,m,*pDesLen);
      for (i = 0; i <= *pDesLen; i++)
        m[i] = unpac[i];
      m[*pDesLen] = 0;
      //Serial.println("Nhan:");
      //Serial.println((char*)m);
      return true;
    }
    else return false;//Serial.println("Nhan error:");
  }
  return true;
}
RSAAuth rsa = RSAAuth() ;
