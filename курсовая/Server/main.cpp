#define WIN32
#ifdef WIN32
#include <windows.h>
#else
#include <stdlib.h>
#endif
#include <stdio.h>
#include "hidapi.h"
#include <stdint.h>
#define MAX_STR 255
#include <locale.h>
#include <WinSock2.h> // подключаем заголовочный файл библиотеки
#include <string.h>
#include <wchar.h>
#include <math.h>
#define Circle 0
#define Square 1
#define Triangle 2
//server
void setLightScreen(unsigned char* buf, hid_device *handle, bool ligthingScreen);
void changeColorBulb(unsigned char* buf, hid_device *handle, char* messageFromClient);
void DrawShape(unsigned char* buf, hid_device *handle, int shape);
int main(int argc, char* argv[])
{
	int res, numberTransaction;
	unsigned char buf[256];
	hid_device *handle;
	setlocale(LC_ALL, "Russian");
    struct sockaddr_in client;
    char messageFromClient[16];
    WSADATA wsa;
    SOCKET s;
    SOCKET new_socket;
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf ( "не получилось: : %d" ,WSAGetLastError());
        return 1;
    }
    else printf ( "получилось.\n" );
    handle = hid_open(0x1234, 0x0001, NULL);
	if (!handle) {
		printf("unable to open device\n");
 		return 1;
	}
    //создаем сокет
    if ((s = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP )) == INVALID_SOCKET)
    {
        printf ( "не получилось создать сокет : %d", WSAGetLastError());
    }
    else printf ( "сокет создан.\n" );

    if ( bind(s , (sockaddr*)&server , sizeof (server)) == SOCKET_ERROR)
    {
        printf ( "не получилось привязать: %d" ,WSAGetLastError());
    }
    else puts ( "сокет привязан" );
    listen(s , 3);
    int c = sizeof (sockaddr_in);
    new_socket = accept(s , (sockaddr*)&client, &c);
    while(new_socket != INVALID_SOCKET )
    {
        if ((recv(new_socket , messageFromClient , 16 , 0)) == SOCKET_ERROR)
        {
            printf("Socket error");
            return -1;
        }
        switch(messageFromClient[7])
        {
            case 5: //зажечь или потушить экран
                switch(messageFromClient[10])
                {
                    case -1: //зажечь
                        setLightScreen(buf, handle, false);
                        break;
                    case 0: //потушить
                        setLightScreen(buf, handle, true);
                        break;
                }
                send (new_socket, messageFromClient, 12, 0);
                break;
            case 16:
                changeColorBulb(buf, handle, messageFromClient);
                send (new_socket, messageFromClient, 16, 0);
                break;
            case 6:
                DrawShape(buf, handle, messageFromClient[11]);
                send (new_socket, messageFromClient, 12, 0);
                break;
        }
    }
    closesocket(s);
    return 0;
}

void setLightScreen(unsigned char* buf, hid_device *handle, bool ligthingScreen)
{
    buf[0] = 0x04;
    !ligthingScreen ? buf[3] = 1 : buf[3] = 0;
    for(int k=0; k<129; k++)
    {
        for(int l=0; l<65; l++)
        {
            buf[1] = k;
            buf[2] = l;
            hid_send_feature_report(handle,buf,4);
        }
    }
}

void changeColorBulb(unsigned char* buf, hid_device *handle, char* messageFromClient)
{
    buf[0] = 0x02; // descriptor number
    buf[1] = messageFromClient[10]; //
    buf[2] = messageFromClient[11]; // 2 byte = uint16_t = power of light color 1
    buf[3] = messageFromClient[12]; //
    buf[4] = messageFromClient[13]; // 2 byte = uint16_t = power of light color 2
    buf[5] = messageFromClient[14]; //
    buf[6] = messageFromClient[15]; // 2 byte = uint16_t = power of light color 3
    hid_send_feature_report(handle,buf,7); // send report, 7 byte
}

void DrawShape(unsigned char* buf, hid_device *handle, int shape)
{
    int x,y;
    buf[0] = 0x04;
    buf[3] = 0;
    for(int k=50; k<79; k++)
    {
        for(int l=18; l<49; l++)
        {
            buf[1] = k;
            buf[2] = l;
            hid_send_feature_report(handle,buf,4);
        }
    }
    buf[3]=1;
    if(shape==Circle)
    {
        for(int k=-10; k<11; k++)
        {
            x = k;
            y = sqrt(100 - x*x);
            //printf("%f : %f;\t",x,y);
            buf[1] = 63 + x;
            buf[2] = 31 + y;
            hid_send_feature_report(handle,buf,4);
            buf[2] = 31 - y;
            hid_send_feature_report(handle,buf,4);
        }
    }
    else if(shape==Square)
    {
        for(int k=-10; k<11; k++)
        {
            x = k;
            y = 10;
            buf[1] = 63 + x;
            buf[2] = 31 + y;
            hid_send_feature_report(handle,buf,4);
            buf[2] = 31 - y;
            hid_send_feature_report(handle,buf,4);
            buf[1] = 63 + y;
            buf[2] = 31 + x;
            hid_send_feature_report(handle,buf,4);
            buf[1] = 63 - y;
            hid_send_feature_report(handle,buf,4);
        }
    }
    else if(shape==Triangle)
    {
        for(int k=-10; k<11; k++)
        {
            x = k;
            y = 10 - fabs(x);
            buf[1] = 63 + x;
            buf[2] = 31 - y;
            hid_send_feature_report(handle,buf,4);
            buf[1] = 63 + x;
            buf[2] = 31;
            hid_send_feature_report(handle,buf,4);
        }
    }
}
