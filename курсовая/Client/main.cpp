#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h> // подключаем заголовочный файл библиотеки
#include <locale.h>
#define Red 1
#define Blue 0
#define Green -1
#define Circle 1
#define Square 0
#define Triangle -1
int setLightScreen(SOCKET Socket,int numberTransaction, bool lightScreenIsLighting);
int setColorBumb(SOCKET Socket, int numberTransaction, int color);
int DrawShape(SOCKET Socket,int numberTransaction, int shape);
//client
int main(){
 // Инициализируем DLL
 setlocale(LC_ALL, "Russian");
 SOCKET Socket;
 struct sockaddr_in server;
 char server_reply[2000],message[16];
 bool lightScreenIsLighting = false;
 server.sin_addr.s_addr = inet_addr( "127.0.0.1" );
 server.sin_family = AF_INET;
 server.sin_port = htons( 8888 );
 int recv_size,menu,numberTransaction=0x00; // создаем переменную для хранения размера принимаемой посылки
 WSADATA wsa; // объявляем переменную для инициализации библиотеки
 WSAStartup(MAKEWORD(2, 2), &wsa); // инициализируем библиотеку, передаем в
 if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
 {
    printf ( "Не получилось. Код ошибки : %d" ,WSAGetLastError()); // WSAGetLastError()
    //возвращает код ошибки инициализации библиотеки
    return 1;
 }
 else printf ( "Инициализировали." );
 if ((Socket = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
 {
        printf ( "Не можем создать сокет : %d" , WSAGetLastError());
 }
 else printf ( "Сокет создан.\n" );
 //соединение с сервером
 if (connect(Socket , (sockaddr*)&server , sizeof (server)) < 0)
 {
    printf  ( "ошибка соединения\n" );
    return 1;
 }
 else printf  ( "присоединились\n" );
 do {
        printf("1. Окрасить лампочку\n");
		lightScreenIsLighting ?  printf("2. Сделать экран тёмным\n")  : printf("2. Зажечь весь экран\n");
		printf("3. Нарисовать фигуру\n");
		printf("4. Выход\n");
		scanf("%d",&menu);
		switch (menu) {
            case 1:
                system("cls");
                printf("1. Красный\n");
                printf("2. Синий\n");
                printf("3. Зелёный\n");
                scanf("%d",&menu);
                switch(menu)
                {
                    case 1:
                        setColorBumb(Socket, numberTransaction, Red);
                        break;
                    case 2:
                        setColorBumb(Socket, numberTransaction, Green);
                        break;
                    case 3:
                        setColorBumb(Socket, numberTransaction, Blue);
                        break;
                }
                break;
            case 2:
                if( !lightScreenIsLighting && setLightScreen(Socket, numberTransaction, lightScreenIsLighting)) lightScreenIsLighting = true;
                else { setLightScreen(Socket,numberTransaction,lightScreenIsLighting); lightScreenIsLighting = false; }
                break;
            case 3:
                system("cls");
                printf("1. Круг\n");
                printf("2. Квадрат\n");
                printf("3. Треугольник\n");
                scanf("%d",&menu);
                switch(menu)
                {
                    case 1:
                        DrawShape(Socket, numberTransaction, Circle);
                        break;
                    case 2:
                        DrawShape(Socket, numberTransaction, Square);
                        break;
                    case 3:
                        DrawShape(Socket, numberTransaction, Triangle);
                        break;
                }
                break;
         }
         if ((recv_size = recv(Socket , server_reply , 2000 , 0)) == SOCKET_ERROR)
         {
            puts ( "ошибка приема" );
         }
         else
         {
            puts ( "ответ получен\n" );
            server_reply[recv_size] = '\0';
            puts(server_reply);
         }
         getchar();
         numberTransaction++;
         system("cls");
 } while (menu != '3');
//принимаем данные от сервера
 return 0;
}

int setLightScreen(SOCKET Socket,int numberTransaction, bool lightScreenIsLighting)
{
     char message[16];
     //tr id
     message[0]=0x00; message[1]=numberTransaction;
     //prot id
     message[2]=0x00; message[3]=0x00;
     //length
     message[4]=0x00; message[5]=0x06;
     //addr
     message[6]=0x00;
     //fc
     message[7]=0x05;
     //addr
     message[8]=0x00; message[9]=0x00;
     //val
     !lightScreenIsLighting ?  message[10]=0xff : message[10] = 0x00;
     message[11]=0x00;
     if ( send(Socket , message , 16 , 0) < 0)
     {
        puts ( "Отправить не получилось\n" );
        return -1;
     }
     else
     {
        puts ( "Данные отправлены \n" );
        return 1;
     }
}

int setColorBumb(SOCKET Socket, int numberTransaction, int color)
{
    char message[16];
     //tr id
     message[0]=0x00; message[1]=numberTransaction;
     //prot id
     message[2]=0x00; message[3]=0x00;
     //length
     message[4]=0x00; message[5]=0x0D;
     //addr server
     message[6]=0x00;
     //fc
     message[7]=0x10;
     //addr first index
     message[8]=0x00; message[9]=0x01;
     //val
     if(color == Red) {message[10] = 0xff; message[11] = 0xff; for(int i=12; i<16; i++) message[i] = 0x00; }
     else if(color == Green) {message[12] = 0xff; message[13] = 0xff; for(int i=10; i<16; i=i+4) { message[i] = 0x00; message[i+1] = 0x00; } }
     else if(color == Blue) {for(int i=10; i<14; i++) message[i] = 0x00; message[14] = 0xff; message[15] = 0xff;}
     if ( send(Socket , message , 16 , 0) < 0)
     {
        puts ( "Отправить не получилось\n" );
        return -1;
     }
     else
     {
        puts ( "Данные отправлены \n" );
        return 1;
     }
}

int DrawShape(SOCKET Socket,int numberTransaction, int shape)
{
     char message[16];
     //tr id
     message[0]=0x00; message[1]=numberTransaction;
     //prot id
     message[2]=0x00; message[3]=0x00;
     //length
     message[4]=0x00; message[5]=0x06;
     //addr
     message[6]=0x00;
     //fc
     message[7]=0x06;
     //addr
     message[8]=0x00; message[9]=0x00;
     //val
     message[10] = 0x00;
     if(shape == Circle) message[11] = 0x00;
     else if (shape == Square) message[11] = 0x01;
     else message[11] = 0x02;
     if ( send(Socket , message , 16 , 0) < 0)
     {
        puts ( "Отправить не получилось\n" );
        return -1;
     }
     else
     {
        puts ( "Данные отправлены \n" );
        return 1;
     }
}
