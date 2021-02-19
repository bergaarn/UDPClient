// Support library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <calcLib.h>

// Header files
#include "protocol.h"

// Defines
#define DEBUG
#define BUFFERSIZE 100

int main(int argc, char *argv[]){
 
  int errorStatus;
  int socketFD;
  char* serverIP;
  char* serverPort;
  char delim[] = ":";
  struct addrinfo prep;
  struct addrinfo* server;
  struct addrinfo* pointer;

  if(argc != 2)
  {
    fprintf(stderr, "Program call was incorrect.\n");
    exit(1);
  }

  if(strlen(argv[1]) < 13)
  {
    fprintf(stderr, "No port found.\n");
    exit(1);
  }

  serverIP = strtok(argv[1], delim);
  serverPort = strtok(NULL, delim);

  if(serverIP == NULL || serverPort == NULL)
  {
    fprintf(stderr, "Invalid server input.\n");
    exit(1); 
  }

  memset(&prep, 0, sizeof prep);
  prep.ai_family = AF_UNSPEC;
  prep.ai_socktype = SOCK_DGRAM;

  errorStatus = getaddrinfo(serverIP, serverPort, &prep, &server);
  if(errorStatus != 0)
  {
    fprintf(stderr, "Program stopped at getaddrinfo: %s\n", gai_strerror(errorStatus));
    return 1;
  }

  for(pointer = server; pointer != NULL; pointer->ai_next)
  {
    socketFD = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol);
    if(socketFD == -1)
    {
      continue;
    }
    break;
  }
/*
  errorStatus = connect(socketFD, pointer->ai_addr, pointer->ai_addrlen);
  if(errorStatus == -1)
  {
    fprintf(stderr, "Unable to connect to server.\n");
    return 2;
  }
*/
  char localIP[INET_ADDRSTRLEN];
  struct sockaddr_in localAdress;
  socklen_t len = sizeof localAdress;
  
  memset(&localAdress, 0, sizeof(localAdress));
  getsockname(socketFD, (struct sockaddr*)&localAdress, &len);
  inet_ntop(AF_INET, &localAdress, localIP, sizeof(localIP));
  int localPort = ntohs(localAdress.sin_port);

  printf("Local IP address: %s:%d\n", localIP, localPort);
/*
  Hantera IPV6 Adresser
*/

  



  close(socketFD);

  return 0;
}
