#ifndef IRC_VARS_H
#define IRC_VARS_H
/**
 ██████╗    ██╗██████╗  ██████╗    ██████╗  █████╗ ██████╗ ███████╗███████╗██████╗
██╔════╝    ██║██╔══██╗██╔════╝    ██╔══██╗██╔══██╗██╔══██╗██╔════╝██╔════╝██╔══██╗
██║         ██║██████╔╝██║         ██████╔╝███████║██████╔╝███████╗█████╗  ██████╔╝
██║         ██║██╔══██╗██║         ██╔═══╝ ██╔══██║██╔══██╗╚════██║██╔══╝  ██╔══██╗
╚██████╗    ██║██║  ██║╚██████╗    ██║     ██║  ██║██║  ██║███████║███████╗██║  ██║
 ╚═════╝    ╚═╝╚═╝  ╚═╝ ╚═════╝    ╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚══════╝╚═╝  ╚═╝
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
#include "callbacks.h"
#include "pass.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

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
  IRC_CMD_PUBMSG,
  IRC_CMD_ERROR,
  IRC_CMD_PING,
} ircCommand_t;

typedef struct {
  char *channel;
  char *sender;
  char *privmsg;
} ircParsed_t;

typedef struct ircCtx_t {
  int socket;
  char *buffer;
  uint16_t bufferLen;

  char *nick;
  char hostname[IRC_BUFFER_LEN];
  uint16_t port;

  ircConnectionStatus_t status;
  ircCommand_t command;
  ircParsed_t parsed;
  ircCallbackList_t *callbacks;

} ircCtx_t;

ircCommand_t ircParse(ircCtx_t *ircConnection);
void ircPubmsg(ircCtx_t *ircConnection, char *channel, char *dst,
               char *privmsg);
void ircPrivmsg(ircCtx_t *ircConnection, char *dst, char *privmsg);
void ircSend(ircCtx_t *ircConnection, char *cmd);
void ircJoin(ircCtx_t *ircConnection, char *channel);
void ircUser(ircCtx_t *ircConnection, char *nick, char *realname);
void ircNick(ircCtx_t *ircConnection, char *nick);

#endif
