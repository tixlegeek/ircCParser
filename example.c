#include "./irc/callbacks.h"
#include "./irc/irc.h"
/**
 ██████╗    ██╗██████╗  ██████╗    ██████╗  █████╗ ██████╗ ███████╗███████╗██████╗
██╔════╝    ██║██╔══██╗██╔════╝    ██╔══██╗██╔══██╗██╔══██╗██╔════╝██╔════╝██╔══██╗
██║         ██║██████╔╝██║         ██████╔╝███████║██████╔╝███████╗█████╗  ██████╔╝
██║         ██║██╔══██╗██║         ██╔═══╝ ██╔══██║██╔══██╗╚════██║██╔══╝  ██╔══██╗
╚██████╗    ██║██║  ██║╚██████╗    ██║     ██║  ██║██║  ██║███████║███████╗██║  ██║
 ╚═════╝    ╚═╝╚═╝  ╚═╝ ╚═════╝    ╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚══════╝╚═╝  ╚═╝

                             Example code

            https://github.com/tixlegeek/ircCParser/tree/main/irc

 @tixlegeek <https://tixlegeek.io>
            <https://twitter.com/tixlegeek>
            <https://twitch.tv/tixlegeek>

 * This file is part of the XXX distribution (https://github.com/xxxx or http://xxx.github.io).
 * Copyright (c) 2015 Liviu Ionescu.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
void publiphoneCbFn(void *ircConnection) {
  (void)ircConnection;
  printf("Command PUBLIPHONE from %s : %s\n",
         ((ircCtx_t *)ircConnection)->parsed.sender,
         ((ircCtx_t *)ircConnection)->parsed.privmsg);
}

void ringCbFn(void *ircConnection) {
  (void)ircConnection;
  printf("RING CALLBACK\n");
}

void helloCbFn(void *ircConnection) {
  if(((ircCtx_t *)ircConnection)->command==IRC_CMD_PUBMSG)
  ircPubmsg((ircCtx_t *)ircConnection, ((ircCtx_t *)ircConnection)->parsed.channel,
            ((ircCtx_t *)ircConnection)->parsed.sender, "Haallo!");
  else
  ircPrivmsg((ircCtx_t *)ircConnection, ((ircCtx_t *)ircConnection)->parsed.sender, "Haallo!");
}

void ircClient(ircCtx_t *ircConnection) {
  char tmpc = 0;
  uint8_t reading = 0;

  ircConnection->status = IRC_CONNECTED;

  sleep(1);
  // Sends connection infos to the Server.
  ircUser(ircConnection, IRC_NICK, IRC_REALNAME);
  ircNick(ircConnection, IRC_NICK);
  ircJoin(ircConnection, IRC_CHANNEL);

  ircConnection->status = IRC_JOINED;
  // While the IRC Bot's status is OK
  while ((ircConnection->status == IRC_JOINED) ||
         (ircConnection->status == IRC_BOT_INITIALIZED)) {
    reading = 1;
    // Reads a whole line from the server
    while (reading) {
      if ((read(ircConnection->socket, &tmpc, 1) > 0) && (tmpc != '\n')) {
        // Removes the trailing \r
        if (tmpc != '\r') {
          ircConnection->bufferLen++;
          ircConnection->buffer =
              realloc(ircConnection->buffer, ircConnection->bufferLen);
          if (ircConnection->buffer == NULL) {
            ircConnection->status = IRC_MEMERROR;
            return;
          }
          ircConnection->buffer[ircConnection->bufferLen - 1] = tmpc;
        }
      } else {
        reading = 0;
      }
    }
    // Sends the received line to the IRC parser
    ircParse(ircConnection);
    bzero(ircConnection->buffer, ircConnection->bufferLen);
    ircConnection->bufferLen = 0;
    reading = 1;
  }
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
    hostname : IRC_HOST,
    port : IRC_PORT,
    callbacks : NULL,
    command : IRC_CMD_JOIN,
    parsed: {NULL},
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
