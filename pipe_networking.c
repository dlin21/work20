#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client
  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  
  int err = mkfifo(WKP, 0644);
  if(err == -1){
    printf("error: %s\n", strerror(errno));
    return 0;
  }
  printf("receiving private FIFO name...\n");
  int from_client = open(WKP, O_RDONLY);
  if(from_client == -1){
    printf("error: %s\n", strerror(errno));
    return 0;
  }
  printf("reading private FIFO name...\n");
  // client message
  char cm[HANDSHAKE_BUFFER_SIZE];
  err = read(from_client, cm, HANDSHAKE_BUFFER_SIZE);
  if(err == -1){
    printf("error: %s\n", strerror(errno));
    return 0;
  }
  // server receives client message
  remove(WKP);
  
  // server sends client response
  printf("connecting to client FIFO...\n");
  *to_client = open(cm, O_WRONLY);
  if(*to_client == -1){
    printf("error: %s\n", strerror(errno));
    return 0;
  }
  
  err = write(*to_client, ACK, sizeof(ACK));
  if(err == -1){
    printf("error: %s\n", strerror(errno));
    return 0;
  }
  // server receives final response from client
  printf("receiving response from client...\n");
  err = read(from_client, cm, sizeof(cm));
  if(err == -1){
    printf("error: %s\n", strerror(errno));
    return 0;
  }
  
  
  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server
  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.
  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  int from_server = 0;
  char sp[1000];
  
  printf("creating private FIFO...\n");
  int pid = getpid();
  sscanf(sp, "%d", &pid);
  mkfifo(sp, 0644);
  
  printf("opening wkp...\n");
  *to_server = open(WKP, O_WRONLY);
  if(*to_server == -1){
    printf("error: %s\n", strerror(errno));
    return 0;
  }
  printf("sending private FIFO name...\n");
  int err = write(*to_server, pid, HANDSHAKE_BUFFER_SIZE);
  if(err == -1){
    printf("error: %s\n", strerror(errno));
    return 0;
  }
  printf("receiving messeage from server...\n");
  int from_server = open(pid, O_RDONLY);
  if(from_server == -1){
    printf("error: %s\n", strerror(errno));
    return 0;
  }
  printf("reading server message...\n");
  char cm[HANDSHAKE_BUFFER_SIZE];
  err = read(from_server, cm, HANDSHAKE_BUFFER_SIZE);
  if(err == -1){
    printf("error: %s\n", strerror(errno));
    return 0;
  }
  
  remove(pid);
  printf("sending response to server...\n");
  err = write(*to_server, ACK, sizeof(ACK));
  if(err == -1){
    printf("error: %s\n", strerror(errno));
    return 0;
  }
  
  return from_server;
}
