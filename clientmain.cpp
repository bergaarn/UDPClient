#include <stdio.h>
#include <stdlib.h>
/* You will to add includes here */
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

// Included to get the support library
#include <calcLib.h>


#include "protocol.h"

int main(int argc, char *argv[]){
  
  char delim[] = ":";
  char* serverIP;
  char* serverPort;

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

  printf("|%s|:|%s|\n", serverIP, serverPort);
  int socketFD;
  int errorNo;
  struct addrinfo prepServer;
  struct addrinfo* server;
  struct addrinfo* pointer;
  
  memset(&prepServer, 0, sizeof prepServer);
  prepServer.ai_family = AF_UNSPEC;
  prepServer.ai_protocol = SOCK_DGRAM;

  
  errorNo = getaddrinfo(serverIP, serverPort, &prepServer, &server);
  if(errorNo != 0)
  {
    fprintf(stderr, "Program stopped at getaddrinfo: %s\n", gai_strerror(errorNo));
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

  errorNo = connect(socketFD, pointer->ai_addr, pointer->ai_addrlen);
  if(errorNo == -1)
  {
    fprintf(stderr, "Unable to connect to server.\n");
    return 2;
  }
  else
  {
    char localIP[INET_ADDRSTRLEN];
    struct sockaddr_in localAdress;
    socklen_t len = sizeof localAdress;
    
    memset(&localAdress, 0, sizeof(localAdress));
    getsockname(socketFD, (struct sockaddr*)&localAdress, &len);
    inet_ntop(AF_INET, &localAdress.sin_addr.s_addr, localIP, sizeof(localIP));

    printf("Local IP address: %s\n", localIP);
  }
  return 0;
}
