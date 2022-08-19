/*
  DateTime.h - Arduino library for date and time functions
  Copyright (c) Michael Margolis.  All right reserved.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef RSAAuth_h
#define RSAAuth_h

#include <Arduino.h>

#define  byte  uint8_t
#define  boolean uint8_t

//typedef unsigned long unsigned long;

struct KEY {
  long n;
  long e;
  long d;
};




class RSAAuth
{
  private:
    struct KEY KEYs[10] = {
      {
        161, 17, 101
      },
      {
        209, 47, 23
      }
      ,
      {
        247, 181, 37
      }
      ,
      {
        133, 37, 73
      },
      {
        209, 29, 149
      }
    };
  private:
    int NoK = 5;
    byte desArr[255];
    byte unpac[255];
    byte en[255];
    byte pac[255];
  public:
    RSAAuth();

    byte GenCrc(byte en[], int len);
    int Package(byte en[], int len, byte pac[]);
    int OpenPackage(byte pac[], int len, byte en[]);
    bool checkArray(byte arr[], int*BEGIN, int*END);
    bool CutString(byte arr[], int*BEGIN, int*END, byte desArr[], int *pDesLen);
    void Send(char cstr[]);
    bool Receive(byte buff[], int*BEGIN, int*END, byte m[], int *pDesLen);
};


extern RSAAuth rsa;  // make an instance for the user


#endif /* RSAAuth */
