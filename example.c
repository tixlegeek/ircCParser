#include "./irc/callbacks.h"
#include "./irc/irc.h"

void publiphoneCbFn(void *ircConnection) {
  (void)ircConnection;
  printf("Command PUBLIPHONE from %s : %s\n",
         ((ircCtx_t *)ircConnection)->senderPtr,
         ((ircCtx_t *)ircConnection)->privmsgPtr);
}

void ringCbFn(void *ircConnection) {
  (void)ircConnection;
  printf("RING CALLBACK\n");
}

void helloCbFn(void *ircConnection) {
  ircPubmsg((ircCtx_t *)ircConnection, ((ircCtx_t *)ircConnection)->channelPtr,
            ((ircCtx_t *)ircConnection)->senderPtr, "Haallo!");
}

int main() {
  // Create callbacks to be plugged onto the bot
  ircCallback_t *publiphoneCb = ircCreateCb("!publiphone", &publiphoneCbFn);
  ircCallback_t *ringCb = ircCreateCb("!ring", &ringCbFn);
  ircCallback_t *helloCb = ircCreateCb("!hello", &helloCbFn);
  // Create a base context for the IRC client
  ircCtx_t ircConnection = {
    socket : 0,
    status : IRC_CONNECTING,
    buffer : NULL,
    bufferLen : 0,
    nick : IRC_NICK,
    channel : IRC_CHANNEL,
    pass : IRC_PASS,
    hostname : IRC_HOST,
    port : IRC_PORT,
    callbacks : NULL,
    command : IRC_CMD_JOIN,
    senderPtr : NULL,
    senderLen : 0,
    channelPtr : NULL,
    channelLen : 0,
    privmsgPtr : NULL,
    privmsgLen : 0,
  };
  // Add callbacks to the list
  ircConnection.callbacks =
      ircCallbackAddList(ircConnection.callbacks, publiphoneCb);
  ircConnection.callbacks = ircCallbackAddList(ircConnection.callbacks, ringCb);
  ircConnection.callbacks =
      ircCallbackAddList(ircConnection.callbacks, helloCb);
  // Networking

  struct hostent *server;
  struct sockaddr_in servaddr;
  // Creates a socket
  ircConnection.socket = socket(AF_INET, SOCK_STREAM, 0);
  if (ircConnection.socket == -1) {
#ifdef IRC_DEBUG
    perror("[x]" RED " socket creation failed...\n" CRST);
#endif
    exit(0);
  } else {
#ifdef IRC_DEBUG
    printf("[s] Socket successfully created..\n");
#endif
  }
  bzero(&servaddr, sizeof(servaddr));
  // Resolves the IRC server hostname
  server = gethostbyname(ircConnection.hostname);
  if (!server) {
#ifdef IRC_DEBUG
    perror("[x]" RED " Lookup failed" CRST);
#endif
    exit(0);
  }

  // Initialize the TCP Client
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr = *((struct in_addr *)server->h_addr);
  servaddr.sin_port = htons(ircConnection.port);

  // Connect to the TCP Socket
  if (connect(ircConnection.socket, (struct sockaddr *)&servaddr,
              sizeof(servaddr)) != 0) {
#ifdef IRC_DEBUG
    perror("[x]" RED " connection with the server failed...\n" CRST);
#endif
    exit(0);
  } else {
#ifdef IRC_DEBUG
    printf("[s] connected to the server..\n");
#endif
  }

  // Plugs the socket to the IRC Client
  ircClient(&ircConnection);
  ircCallbackFreeList(ircConnection.callbacks);
  // Closing the socket.
  close(ircConnection.socket);
  return EXIT_SUCCESS;
}
