#ifndef IRC_VARS_H
#define IRC_VARS_H

  #include "pass.h"
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <netdb.h>
  #include <sys/socket.h>
  #include <unistd.h>
  #include "callbacks.h"

  #define BLK "\e[30m"
  #define RED "\e[31m"
  #define GRN "\e[32m"
  #define YEL "\e[33m"
  #define BLU "\e[34m"
  #define MAG "\e[35m"
  #define CYN "\e[36m"
  #define WHT "\e[37m"
  #define CRST "\e[0m"

  #define IRC_HOST "ircnet.clue.be"
  #define IRC_PORT 6667
  #define IRC_NICK "tixcbot2"
  #define IRC_REALNAME "tixCBot2"
  #define IRC_CHANNEL "#poneysapin"
  #define IRC_BUFFER_LEN 64
  #define IRC_DEBUG

  typedef enum {
    IRC_CONNECTED,
    IRC_CONNECTING,
    IRC_JOINED,
    IRC_MEMERROR,
    IRC_CONNECTION_LOST,
    IRC_BOT_INITIALIZED
  } ircConnectionStatus_t;

  typedef enum {
    IRC_GLOBAL_ERROR,
    IRC_CMD_JOIN,
    IRC_CMD_PART,
    IRC_CMD_QUIT,
    IRC_CMD_PRIVMSG,
    IRC_CMD_ERROR,
    IRC_CMD_PING,
  } ircCommand_t;

    typedef struct ircCtx_t{
      int socket;
      ircConnectionStatus_t status;
      char *buffer;
      uint16_t bufferLen;
      char hostname[IRC_BUFFER_LEN];
      char nick[IRC_BUFFER_LEN];
      char channel[IRC_BUFFER_LEN];
      char pass[IRC_BUFFER_LEN];
      uint16_t port;
      ircCallbackList_t *callbacks;
      ircCommand_t command;
      char *senderPtr;
      size_t senderLen;
      char *channelPtr;
      size_t channelLen;
      char *privmsgPtr;
      size_t privmsgLen;

    } ircCtx_t;

  ircCommand_t ircParse(ircCtx_t *ircConnection);
  void ircPubmsg(ircCtx_t *ircConnection, char *channel, char *dst, char* privmsg);
  void ircPrivmsg(ircCtx_t *ircConnection, char *dst, char* privmsg);
  void ircSend(ircCtx_t *ircConnection, char *cmd);
  void ircIdentify(ircCtx_t *ircConnection);
  void ircClient(ircCtx_t *ircConnection);

#endif
