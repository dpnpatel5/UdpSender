#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int makeMsg(char* data, char* strMessage);
#define MAX_BUF_SIZE 20000
FILE * fp;
char opcode_buf[MAX_BUF_SIZE];


int main(int argc , char *argv[])
{ 
  int retval;
  int len = 0;
  struct sockaddr_in serv_addr,send_addr;

  char strMessage[20000] = {0};

  if(argc < 4)
  {
     cout<<"usage "<<argv[0] <<"<dest IP> <dest port> <src IP> <own port>"<<endl; 
     return 0 ;
  }
  memset(&serv_addr, 0, sizeof(serv_addr));
  int fd =  socket(PF_INET, SOCK_DGRAM, 0);
  int optval = 1;
  socklen_t optlen = sizeof(optval);
  getsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, &optlen);

  int iPort = atoi(argv[4]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(iPort);
  serv_addr.sin_addr.s_addr = inet_addr(argv[3]);

  int lncount=0;
  if((fp = fopen(argv[5],"r")) != NULL)
  {
    while(fgets(opcode_buf,MAX_BUF_SIZE,fp)!=NULL)
    {
      printf("\n file copied %d \n",fp);
      for (lncount = strlen(opcode_buf)-1 ; lncount >= 0 && (opcode_buf[lncount] == '\n' || opcode_buf[lncount] == '\r') ; lncount--)
      {
        opcode_buf[lncount] = 0 ;
      }
      if (!strlen(opcode_buf))
        continue ;
    }
  }
  printf("\n file pointer %d , len[%d], opcode_buf[%s]\n",fp, sizeof(opcode_buf), opcode_buf);

  if( 0 > bind(fd,(const  struct sockaddr *)&serv_addr,sizeof(serv_addr)))
  {
    printf("retval of bind recv: %d\n", retval);
    perror("bind");
    return 0;
  }

  int fd_send =  socket(PF_INET, SOCK_DGRAM, 0);

  send_addr.sin_family = AF_INET;
  int iPortSend = atoi(argv[2]);
  send_addr.sin_port = htons(iPortSend);

  send_addr.sin_addr.s_addr = inet_addr(argv[1]);

  struct sockaddr addr;
  int newFd = 0;
  socklen_t addrlen = sizeof(addr);

  len = makeMsg(opcode_buf, strMessage);
  printf("makeMsg Len:%d , strMessage[%s]", len, strMessage);

  if( 0 > (retval = sendto(fd, strMessage, (len), 0,(struct sockaddr*)&send_addr,sizeof(send_addr))))
  {
    printf("failed to send the message\n");
    perror("send");
    return 0;
  }
  else
    printf("sent message of len:%d to %s\n", retval,argv[1]);
}

int makeMsg(char* data, char* strMessage)
{
  int length = 0;
  int j;
  printf("\nmakeMsg Enter data[%s]\n",data);

  if(data == NULL) return 0;

  for (j = 0 ; j < strlen(data) ; j += 2)
  {
    char     ch1 = data[j] ;
    char     ch2 = data[j+1] ;

    ch1 = (ch1 >= '0' && ch1 <= '9') ? ch1 - '0' : (ch1 >= 'A' && ch1 <= 'F') ? ch1 - 'A' + 10 :
      (ch1 >= 'a' && ch1 <= 'f') ? ch1 - 'a' + 10 : -1 ;
    ch2 = (ch2 >= '0' && ch2 <= '9') ? ch2 - '0' : (ch2 >= 'A' && ch2 <= 'F') ? ch2 - 'A' + 10 :
      (ch2 >= 'a' && ch2 <= 'f') ? ch2 - 'a' + 10 : -1 ;

    if (ch1 == -1 || ch2 == -1)
      return 0 ;

    strMessage[length++] = (ch1 << 4) | ch2 ;
  }

  printf("\nmakeMag Exit\n");
  return length;
}
