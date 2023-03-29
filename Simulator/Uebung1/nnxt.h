#ifndef NNXT_H_INCLUDED
#define NNXT_H_INCLUDED

#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <thread>

#include <vector>
#include <string>



std::vector<std::string> receivedCommands;


int startWinsock(void);
long getAddrFromString(const char* hostnameOrIp, SOCKADDR_IN* addr);

SOCKET s;
volatile unsigned int TimerVal = 0;


static void connectToServer() __attribute__((constructor (65534)));


class Mutex
{
    private:
        volatile int interlock;
    public:
        Mutex() {interlock=0;};
        ~Mutex(){};
        void lock() {while(this->interlock == 1);}
        void unlock(){this->interlock = 0;}

        friend class Locker;
};

class Locker
{
     Mutex&    m;
     public:
         Locker(Mutex& m): m(m) {m.lock();}
         ~Locker()              {m.unlock();}
};


Mutex socket_Mutex;
Mutex senden_Mutex;
Mutex task_Mutex;

void generateCommand(char *cmd){
    if (cmd[0]=='T' && cmd[1]=='V') { // Timestamps nicht mit in die Kommandos aufnehmen
        unsigned int recvTV;
        recvTV = strtol(&(cmd[2]),NULL,16);
        TimerVal = recvTV;
        return;
    }
    std::string command = cmd;
    Locker lock(socket_Mutex);
    receivedCommands.push_back(command);
}

void recvMsg(const char *expectedMsg, char *buf, int len){
  while(1) {
    while(receivedCommands.size()==0){}
    {
        char *msg;
        for(size_t f=0;f<receivedCommands.size();f++){
            msg = strdup(receivedCommands[f].c_str());
            if (!strstr(msg,expectedMsg)){
                delete msg;
                continue;
            }
            strcpy(buf,msg);
            delete msg;
            {
                 Locker l(socket_Mutex);
                 receivedCommands.erase(receivedCommands.begin()+f);
            }
            return;
        }
    }
  }
}

void socketCheck(){
    char buf[1000];
    char command[100];
    int commandIdx = 0;
    long rc;
    while(1) {
        rc=recv(s,buf,1000,0);
        // prüfen ob die verbindung geschlossen wurde oder ein fehler auftrat
        if(rc==0 || rc==SOCKET_ERROR)
        {
            printf("Simulator hat die Verbindung getrennt\n");
            exit(1);
        }
        //else
        //    printf("Empfangen: %s[%ld]\n",buf,rc);
        for(int f=0;f<rc;f++){
           if (buf[f]==';') {
                command[commandIdx] = '\0';
                generateCommand(command);
                commandIdx = 0;
           }
           else {
                if (!(buf[f]==0x0a || buf[f]==0x0d))
                    command[commandIdx++] = buf[f];
           }
        }
    }
}


void connectToServer(){
  long rc;
  SOCKADDR_IN addr;
  char buf[256];

  // Winsock starten
  rc=startWinsock();
  if(rc!=0)
  {
    printf("Fehler: startWinsock, fehler code: %ld\n",rc);
    exit(1);
  }
  // Socket erstellen
  s=socket(AF_INET,SOCK_STREAM,0);
  if(s==INVALID_SOCKET)
  {
    printf("Fehler: Der Socket konnte nicht erstellt werden, fehler code: %d\n",WSAGetLastError());
    exit(1);
  }
  // Verbinden
  memset(&addr,0,sizeof(SOCKADDR_IN)); // zuerst alles auf 0 setzten
  addr.sin_family=AF_INET;
  addr.sin_port=htons(12345); // wir verwenden mal port 12345
  rc=getAddrFromString("127.0.0.1",&addr);
  if(rc==SOCKET_ERROR)
  {
    printf("IP für konnte nicht aufgeloest werden\n");
    exit(1);
  }
  rc=connect(s,(SOCKADDR*)&addr,sizeof(SOCKADDR));
  if(rc==SOCKET_ERROR)
  {
    printf("Fehler: connect gescheitert, fehler code: %d\n",WSAGetLastError());
    exit(1);
  }
  // jetzt auf TimerVal vom Server warten und dann raus in die main-Funktion
  rc=recv(s,buf,256,0);
    if(rc==0)
    {
      printf("Simulator hat die Verbindung getrennt\n");
      exit(1);
    }
    if(rc==SOCKET_ERROR)
    {
      printf("Simulator hat die Verbindung getrennt\n");
      exit(1);
    }
    unsigned int recvTV;
    recvTV = strtol(&(buf[2]),NULL,16);
    TimerVal = recvTV;
    std::thread t1(socketCheck);
    t1.detach();
}




uint32_t GetSysTime(){
    return TimerVal*10;
}


void StartScheduler() {while(1);}

typedef void (*nnxt_task)( void * );
typedef void * NNXT_TaskHandle;

#define CreateTask(task) {}

#define StartTask(task) {std::thread t(task);t.detach();}

#define CreateAndStartTask(task) {std::thread t(task);t.detach();}

#define taskENTER_CRITICAL() task_Mutex.lock()
#define taskEXIT_CRITICAL() task_Mutex.unlock()


enum motor_stop_e {
    Motor_stop_break = 0,   /// Breaking the motor very hard!
    Motor_stop_float        /// Breaking the motor by just sending no new values to him. The motor will go after a little.
};
typedef enum motor_stop_e		motor_stop_t;

enum motor_ports_e {
    Port_A = 0,
    Port_B = 1,
    Port_C = 2
};
typedef enum motor_ports_e      motorport_t;


enum motor_dir_e {
    Motor_dir_forward = 0,
    Motor_dir_backward
};
typedef enum motor_dir_e motor_dir_t;


void MotorPortInit(motorport_t port){}

void Motor_Drive(motorport_t port, motor_dir_t dir, uint8_t dutycycle){
    char PortChar;
    char PortDir;
    char msg[20];
    switch (port) {
        case Port_A: PortChar = 'A';break;
        case Port_B: PortChar = 'B';break;
        default:     PortChar = 'C';
    }
    if (dir==Motor_dir_forward)
        PortDir = 'F';
    else
        PortDir = 'R';
    sprintf(msg,"M%c%c%.2x;",PortChar,PortDir,dutycycle);
    senden_Mutex.lock();
    long rc=send(s,msg,strlen(msg),0);
    senden_Mutex.unlock();
    if(rc==0 || rc==SOCKET_ERROR)
    {
        printf("Simulator hat die Verbindung getrennt\n");
        exit(1);
    }
}

void Motor_Stop(motorport_t port, motor_stop_t stop){
    char PortChar;
    char PortDir;
    char msg[20];
    switch (port) {
        case Port_A: PortChar = 'A';break;
        case Port_B: PortChar = 'B';break;
        default:     PortChar = 'C';
    }
    PortDir = 'F';
    sprintf(msg,"M%c%c%.2x;",PortChar,PortDir,0);
    senden_Mutex.lock();
    long rc=send(s,msg,strlen(msg),0);
    senden_Mutex.unlock();
    if(rc==0 || rc==SOCKET_ERROR)
    {
        printf("Simulator hat die Verbindung getrennt\n");
        exit(1);
    }
}



void Delay(unsigned int d){
    unsigned int startTime = TimerVal;
    if (d<=10)
        return;
    while(1) {
        if (TimerVal > startTime + d/10)
            return;
        Sleep(5);
    }
}


void NNXT_LCD_DisplayStringAtLine(uint16_t Line, const char *ptr){
    char msg[200];

    sprintf(msg,"PL%.1d\"%s\";",Line,ptr);
    senden_Mutex.lock();
    long rc=send(s,msg,strlen(msg),0);
    senden_Mutex.unlock();
    if(rc==0 || rc==SOCKET_ERROR)
    {
        printf("Simulator hat die Verbindung getrennt\n");
        exit(1);
    }
}

#define LCD_COLOR_BLACK         0x0000
#define LCD_COLOR_GREY          0xF7DE
#define LCD_COLOR_BLUE          0x001F
#define LCD_COLOR_RED           0xF800
#define LCD_COLOR_GREEN         0x07E0
#define LCD_COLOR_CYAN          0x07FF
#define LCD_COLOR_MAGENTA       0xF81F
#define LCD_COLOR_YELLOW        0xFFE0
#define LCD_COLOR_WHITE         0xFFFF


void NNXT_LCD_Clear(int Color) {
    char msg[200];

    sprintf(msg,"CD;");
    senden_Mutex.lock();
    long rc=send(s,msg,strlen(msg),0);
    senden_Mutex.unlock();
    if(rc==0 || rc==SOCKET_ERROR)
    {
        printf("Simulator hat die Verbindung getrennt\n");
        exit(1);
    }
}

enum touch_clicked_e {
    SensorTouch_clicked = 0,
    SensorTouch_released = 1
};
typedef enum touch_clicked_e	sensor_touch_clicked_t;

enum sensor_types_e {
	SensorUnderConfiguration = -2,
	SensorUnConfigured = -1,
    SensorUS = 0,
    SensorTouch,
    SensorColor,
    SensorExt,
};
typedef enum sensor_types_e     sensortypes_t;


enum sensor_error_e {
    sensor_error_NoError = 0,
    sensor_error_SensorNotConfigured,
    sensor_error_FalsePort,
    sensor_error_TooFastToRead,
};
typedef enum sensor_error_e     sensor_error_t;

enum sensor_ports_e {
    Port_0 = 0,
    Port_1 = 1,
    Port_2 = 2,
    Port_3 = 3,
    Port_extern = 4,
};
typedef enum sensor_ports_e     sensorport_t;


sensor_error_t SensorConfig(sensorport_t port, sensortypes_t type){return sensor_error_NoError;}


sensor_error_t Touch_Clicked(sensorport_t port, sensor_touch_clicked_t *clicked){
    char msg[200];
    char portChar;

    switch (port) {
        case Port_0 : portChar = '0';break;
        case Port_1 : portChar = '1';break;
        default:      portChar = '2';break;
    }
    sprintf(msg,"PT%c;",portChar);
    senden_Mutex.lock();
    long rc=send(s,msg,strlen(msg),0);
    senden_Mutex.unlock();
    if(rc==0 || rc==SOCKET_ERROR)
    {
        printf("Simulator hat die Verbindung getrennt\n");
        exit(1);
    }
    recvMsg("TS",msg,100);
    if (msg[0]=='T' && msg[1]=='S'){
        if (msg[2]=='0')
            *clicked = SensorTouch_released;
        else
            *clicked = SensorTouch_clicked;
        return sensor_error_NoError;
    }
    else {
        printf("Fehler beim Sensor auslesen!\n");
        exit(1);
    }
}



sensor_error_t US_GetDistance(sensorport_t port, uint8_t *distance){
    char msg[100];

    sprintf(msg,"USV;");
    senden_Mutex.lock();
    long rc=send(s,msg,strlen(msg),0);
    senden_Mutex.unlock();
    if(rc==0 || rc==SOCKET_ERROR)
    {
        printf("Simulator hat die Verbindung getrennt\n");
        exit(1);
    }
    recvMsg("USV",msg,100);
    if (msg[0]=='U' && msg[1]=='S' && msg[2]=='V'){
        int dist = strtol(&(msg[3]),NULL,16);
        *distance = dist;
        return sensor_error_NoError;
    }
    else {
       printf("Fehler: USV: keine Antwort vom Server\n");
       exit(1);
    }

}


int startWinsock(void)
{
  WSADATA wsa;
  return WSAStartup(MAKEWORD(2,0),&wsa);
}


long getAddrFromString(const char* hostnameOrIp, SOCKADDR_IN* addr)
{
  unsigned long ip;
  HOSTENT* he;


  /* Parameter prüfen */
  if(hostnameOrIp==NULL || addr==NULL)
    return SOCKET_ERROR;


  /* eine IP in hostnameOrIp ? */
  ip=inet_addr(hostnameOrIp);

  /* bei einem fehler liefert inet_addr den Rückgabewert INADDR_NONE */
  if(ip!=INADDR_NONE)
  {
    addr->sin_addr.s_addr=ip;
    return 0;
  }
  else
  {
    /* Hostname in hostnameOrIp auflösen */
    he=gethostbyname(hostnameOrIp);
    if(he==NULL)
    {
      return SOCKET_ERROR;
    }
    else
    {
      /*die 4 Bytes der IP von he nach addr kopieren */
      memcpy(&(addr->sin_addr),he->h_addr_list[0],4);
    }
    return 0;

  }
}

#endif // NNXT_H_INCLUDED
