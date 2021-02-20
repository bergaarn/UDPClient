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
 
  int errorIndex;
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

  errorIndex = getaddrinfo(serverIP, serverPort, &prep, &server);
  if(errorIndex != 0)
  {
    fprintf(stderr, "Program stopped at getaddrinfo: %s\n", gai_strerror(errorIndex));
    return 1;
  }

  for(pointer = server; pointer != NULL; pointer = pointer->ai_next)
  {
    socketFD = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol);
    if(socketFD == -1)
    {
      continue;
    }
    break;
  }

  int socketFDtemp = socket(pointer->ai_family, pointer->ai_socktype, pointer->ai_protocol);
  errorIndex = connect(socketFDtemp, pointer->ai_addr, pointer->ai_addrlen);
  if(errorIndex == -1)
  {
    fprintf(stderr, "Unable to connect to server.\n");
    return 2;
  }

  /*
   IPV4 Adresser
  */

  char hostIP[INET_ADDRSTRLEN];
  struct sockaddr_in hostAdress;
  socklen_t len = sizeof hostAdress;
  
  memset(&hostAdress, 0, sizeof(hostAdress));
  getsockname(socketFD, (struct sockaddr*)&hostAdress, &len);
  inet_ntop(AF_INET, &hostAdress, hostIP, sizeof(hostIP));
  printf("Host Address: %s:%d\n", hostIP, hostAdress.sin_port);

  char localIP[INET_ADDRSTRLEN];
  struct sockaddr_in localAdress;
  len = sizeof localAdress;
  
  memset(&localAdress, 0, sizeof(localAdress));
  getsockname(socketFDtemp, (struct sockaddr*)&localAdress, &len);
  inet_ntop(AF_INET, &localAdress, localIP, sizeof(localIP));
  int localPort = ntohs(localAdress.sin_port);
  close(socketFDtemp);

  printf("Local IP address: %s:%d\n", localIP, localPort);
  
  //----------------------------------------------------------

  /*
    Hantera IPV6 Adresser
  */

  //----------------------------------------------------------


  // SendTo()
  int32_t sentBytes;
  int32_t recvBytes;
  struct calcMessage mBuffer;
  struct calcProtocol pBuffer;

  mBuffer.type = htons(22);
  mBuffer.message = htonl(0);
  mBuffer.protocol = htons(17);
  mBuffer.major_version = htons(1);
  mBuffer.minor_version = htons(0);

  sentBytes = sendto(socketFD, &mBuffer, sizeof(mBuffer), 0, pointer->ai_addr, pointer->ai_addrlen);
  if(sentBytes == -1)
  {
    fprintf(stderr, "Unable to send.\n");
    exit(1);
  }
  printf("Sent %d bytes.\n", sentBytes);


  // Recvfrom()
  recvBytes = recvfrom(socketFD, &pBuffer, sizeof(pBuffer), 0, pointer->ai_addr, &pointer->ai_addrlen);
  if(recvBytes == -1)
  {
    fprintf(stderr, "Unable to recieve\n");
    exit (1);
  }
  printf("Recieved %d bytes\n", recvBytes);

  uint16_t pType = ntohs(pBuffer.type);
  uint16_t pMajor = ntohs(pBuffer.major_version);
  uint16_t pMinor = ntohs(pBuffer.minor_version);
  uint16_t pID = ntohl(pBuffer.id);
  uint16_t pArith = ntohl(pBuffer.arith);
  uint16_t firstIntValue = ntohl(pBuffer.inValue1);
  uint16_t secondIntValue = ntohl(pBuffer.inValue2);
  uint16_t intResult = ntohl(pBuffer.inResult);
  double firstFloatValue = pBuffer.flValue1;
  double secondFloatValue = pBuffer.flValue2;
  double floatResult = pBuffer.flResult;

  printf("Type: %d, Major: %d, Minor: %d, ID: %d, Arith: %d\n", pType, pMajor, pMinor, pID, pArith);
  printf("First Int: %d, Second Int: %d\n",  firstIntValue, secondIntValue);
  printf("First Float: %8.8g, Second Float: %8.8g\n", firstFloatValue, secondFloatValue);
  freeaddrinfo(server);  
  close(socketFD);

  return 0;
}
