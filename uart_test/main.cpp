#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <termios.h>                   // B115200, CS8 등 상수 정의
#include <fcntl.h>                     // O_RDWR , O_NOCTTY 등의 상수 정의

void print_buf(char * input,int count)
{
   for(int i=0;i<count;i++)
      printf("0x%02x, ",input[i]);
   printf("\r\n");
}

static char buf[4096];
static char cmd[4096];

int cmd_mode=0;
int data_cnt=0;
int buf_cnt=0;
#define header_1 0
#define header_2 1
#define header_3 2
#define length_1 3
#define length_2 4
#define read_data 5

void uart_reader(char * input,int count)
{
   for(int i=0; i< count; i++)
   {
      //printf("0x%02x",input[i]);
      char data = input[i];

      switch(cmd_mode)
      {
         case header_1:
            if(data == 'L')
               cmd_mode++;
            else
               cmd_mode = header_1;
         break;
         case header_2:
            if(data == 'i')
               cmd_mode++;
            else
               cmd_mode = header_1;
         break;
         case header_3:
            if(data == 'f')
               cmd_mode++;
            else
               cmd_mode = header_1;
         break;
         case length_1:
               cmd_mode++;
               data_cnt=(int)data*256;
         break;
         case length_2:
               data_cnt=data_cnt+data;
               cmd_mode=read_data;
         break;
         case read_data:
 
            cmd[buf_cnt]=data;
            buf_cnt++;
            data_cnt--;

            if(data_cnt==0)
            {
               print_buf(cmd,buf_cnt);
               cmd_mode=header_1;
               buf_cnt=0;
            }

         break;
      }
   }

}


int main( void)
{        
   int    fd;
   int    ndx;
   int    cnt;
   struct termios    newtio;
   struct pollfd     poll_events;      // 체크할 event 정보를 갖는 struct
   int    poll_state;

   // 시리얼 포트를 open

   fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY );        // 디바이스를 open 한다.
   if ( 0 > fd)
   {        
      printf("open error\n");
      return -1;
   }

   // 시리얼 포트 통신 환경 설정

   memset(&newtio, 0, sizeof(newtio) );
   newtio.c_cflag       = B9600 | CS8 | CLOCAL | CREAD;
   newtio.c_oflag       = 0;
   newtio.c_lflag       = 0;
   newtio.c_cc[VTIME]   = 0;
   newtio.c_cc[VMIN]    = 1;
   
   tcflush(fd, TCIFLUSH);
   tcsetattr(fd, TCSANOW, &newtio);
   fcntl(fd, F_SETFL, FNDELAY); 

   // poll 사용을 위한 준비   
   poll_events.fd        = fd;
   poll_events.events    = POLLIN | POLLERR;          // 수신된 자료가 있는지, 에러가 있는지
   poll_events.revents   = 0;

   // 자료 송수신
   while (1)
   {
      poll_state = poll(                                // poll()을 호출하여 event 발생 여부 확인     
                         (struct pollfd*)&poll_events,  // event 등록 변수
                                                    1,  // 체크할 pollfd 개수
                                                  1000  // time out 시간
                       );
      sleep(1);
      if (poll_state > 0)                             // 발생한 event 가 있음
      {     
         if ( poll_events.revents & POLLIN)            // event 가 자료 수신?
         {
            cnt = read( fd, buf, 4096);
            uart_reader(buf,cnt);
            //write( fd, buf, cnt);                       //serial write test
            //printf( "data received - %d %s\n", cnt, buf);
         }
         if ( poll_events.revents & POLLERR)      // event 가 에러?
         {
            printf( "통신 라인에 에러가 발생, 프로그램 종료");
            break;
         }
      }
      else if(poll_state < 0)
      {
          printf("Critial Error!\n");
          break;
      }
      else if(poll_state == 0)
      {
          printf("wait...\n");
      }
   }

   close( fd);
   return 0;
}