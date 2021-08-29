#include "irc.h"
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
 
#ifndef HAVE_ASPRINTF
#include <stdarg.h>
int vasprintf(char **ret, const char *format, va_list ap) {
  va_list ap2;
  int len = 100; /* First guess at the size */
  if ((*ret = (char *)malloc(len)) == NULL)
    return -1;
  while (1) {
    int nchar;
    va_copy(ap2, ap);
    nchar = vsnprintf(*ret, len, format, ap2);
    if (nchar > -1 && nchar < len)
      return nchar;
    if (nchar > len)
      len = nchar + 1;
    else
      len *= 2;
    if ((*ret = (char *)realloc(*ret, len)) == NULL) {
      free(*ret);
      return -1;
    }
  }
}

int asprintf(char **ret, const char *format, ...) {
  va_list ap;
  int nc;
  va_start(ap, format);
  nc = vasprintf(ret, format, ap);
  va_end(ap);
  return nc;
}
#endif /*HAVE_ASPRINTF*/

/**
  Parses informations stored in ircConnection->buffer
  and fires callbacks;
*/
ircCommand_t ircParse(ircCtx_t *ircConnection) {
  ircCallbackList_t *callbacks = (ircCallbackList_t *)ircConnection->callbacks;
  char *ptr = strdup(ircConnection->buffer);
  if (ptr == NULL) {
    ircConnection->status = IRC_MEMERROR;
    return IRC_GLOBAL_ERROR;
  }
  char *ptrdup = ptr;

  char *cmd = NULL;
  char *sender = NULL;
  char *hostname = NULL;
  char *args = NULL;
  char *channel = NULL;
  char *privmsg = NULL;

  char *tokptr_msg = NULL;
  char *tokptr_hostname = NULL;
  char *tokptr_args = NULL;

  char *buffer = NULL;
  uint8_t i = 0;
  // reinit ptrs
  // Splits the message depending on its construction.
#ifdef IRC_DEBUG
  printf("[>]" CYN " %s" CRST "\n", ircConnection->buffer);
#endif
  // From now, ptr will hold the string containing the command.
  if (ptr[0] == ':') {
    // Trims ':' at the end of the command
    ptr++;
    // hostname should contain the entire sender+hostname string, that will be
    // parsed
    hostname = strtok_r(ptr, " ", &tokptr_msg);
    if (hostname != NULL) {
      // Splits the hostname. If no '!' is found, then sender = hostname
      sender = strtok_r(hostname, " !", &tokptr_hostname);
      if (sender != NULL) {
        hostname = strtok_r(NULL, " ", &tokptr_hostname);
        if (hostname == NULL) {
          hostname = sender;
        }
      }
    }
    // Extracts the IRC CMD parameter
    cmd = strtok_r(NULL, " ", &tokptr_msg);
#ifdef IRC_DEBUG
    printf(".....[?]" YEL " SENDER: \"%s\"" CRST "\n", sender);
    printf(".....[?]" YEL " HOSTNAME: \"%s\"" CRST "\n", hostname);
#endif
  } else {
    cmd = strtok_r(ptr, " ", &tokptr_msg);
  }
  // Extracts arguments
  args = strtok_r(NULL, "", &tokptr_msg);
#ifdef IRC_DEBUG
  printf(".....[?]" YEL " CMD: \"%s\"" CRST "\n", cmd);
  printf(".....[?]" YEL " ARGS: \"%s\"" CRST "\n", args);
#endif

  // Message parsing
  // PING requests
  if ((strncmp(cmd, "PING", 4)) == 0) {
    buffer = NULL;
    int bufferLen = asprintf(&buffer, "PONG %s", args);
    if (bufferLen == -1) {
      ircConnection->status = IRC_MEMERROR;
      return IRC_GLOBAL_ERROR;
    }
    ircSend(ircConnection, buffer);
    free(buffer);
    ircConnection->command = IRC_CMD_PING;
  }
  // PRIVMSG requests
  else if ((strncmp(cmd, "PRIVMSG", 7)) == 0) {
    channel = strtok_r(args, ":", &tokptr_args);
    privmsg = strtok_r(NULL, "", &tokptr_args);
#ifdef IRC_DEBUG
    printf(".....[?]" YEL " CHANNEL: \"%s\"" CRST "\n", channel);
    printf(".....[?]" YEL " PRIVMSG: \"%s\"" CRST "\n", privmsg);
#endif
    // compute the place of the pointer on the buffer

    // trims PRIVMSG
    if (strncmp(channel, ircConnection->nick, strlen(ircConnection->nick)) == 0)
      ircConnection->command = IRC_CMD_PRIVMSG;
    else
      ircConnection->command = IRC_CMD_PUBMSG;
    while (*privmsg == ' ')
      privmsg++;

    if (privmsg[0] == '!') {
      ircConnection->parsed.sender = strdup(sender);
      ircConnection->parsed.channel = strdup(channel);
      ircConnection->parsed.privmsg = strdup(privmsg);

      for (i = 0; i < callbacks->len; i++) {
        printf("Compairing with cb %s\n", callbacks->list[i]->keyword);
        if (strncmp(privmsg, callbacks->list[i]->keyword,
                    callbacks->list[i]->kwlen) == 0) {
          printf("Launching cb %s\n", callbacks->list[i]->keyword);
          callbacks->list[i]->cb(ircConnection);
          break;
        }
      }
    }
  }
  // ERROR Messages
  else if ((strncmp(cmd, "ERROR", 5)) == 0) {
    ircConnection->status = IRC_CONNECTION_LOST;
    ircConnection->command = IRC_CMD_ERROR;
  }
  // Waits for the 353 Message to complete the login process
  else if ((strncmp(cmd, "353", 3)) == 0) {
#ifdef IRC_DEBUG
    printf("[!]" GRN " *** Bot initialized ! ***\n" CRST);
#endif
    ircConnection->status = IRC_BOT_INITIALIZED;
  }
  // When a user joins the channel
  else if ((strncmp(cmd, "JOIN", 4)) == 0) {
    ircConnection->command = IRC_CMD_JOIN;
#ifdef IRC_DEBUG
    printf("[!]" WHT " *** User %s joined ***\n" CRST, sender);
#endif
  }
  // PART and QUIT are quite the same stuff.
  else if ((strncmp(cmd, "PART", 4)) == 0) {
    ircConnection->command = IRC_CMD_PART;
#ifdef IRC_DEBUG
    printf("[!]" WHT " *** User %s exited: PART ***\n" CRST, sender);
#endif
  } else if ((strncmp(cmd, "QUIT", 4)) == 0) {
    ircConnection->command = IRC_CMD_QUIT;
#ifdef IRC_DEBUG
    printf("[!]" WHT " *** User %s exited: QUIT ***\n" CRST, sender);
#endif
  }
  free(ptrdup);

  return ircConnection->command;
}
/**
  Sends a public message onthe channel
*/
void ircPubmsg(ircCtx_t *ircConnection, char *channel, char *dst,
               char *privmsg) {
  char *buffer=NULL;
  int bufferLen = asprintf(&buffer, "%s :%s", channel, dst);
  if (bufferLen == -1) {
    ircConnection->status = IRC_MEMERROR;
    return;
  }
  ircPrivmsg(ircConnection, buffer, privmsg);
  free(buffer);
}

void ircPrivmsg(ircCtx_t *ircConnection, char *dst, char *privmsg) {
  char *buffer = NULL;
  int bufferLen = asprintf(&buffer, "PRIVMSG %s :%s", dst, privmsg);
  if (bufferLen == -1) {
    ircConnection->status = IRC_MEMERROR;
    return;
  }
  ircSend(ircConnection, buffer);
  free(buffer);
}

void ircSend(ircCtx_t *ircConnection, char *cmd) {
  char *buffer = NULL;
  // Creates the message to be sent (basically adds \r\n )
  int bufferLen = asprintf(&buffer, "%s\r\n", cmd);
  if (bufferLen == -1) {
    ircConnection->status = IRC_MEMERROR;
    return;
  }
// Hide the PASS requests
#ifdef IRC_DEBUG
  if (strstr(cmd, "PASS") != NULL) {
    printf("[<]" GRN " PASS ****** " CRST "\n");
  } else {
    printf("[<]" GRN " %s" CRST "\n", cmd);
  }
#endif
  // Send buffer to the socket
  write(ircConnection->socket, buffer, bufferLen);
  free(buffer);
}

void ircJoin(ircCtx_t *ircConnection, char *channel){
  char *buffer = NULL;
  int bufferLen = asprintf(&buffer, "JOIN %s", channel);
  if (bufferLen == -1) {
    ircConnection->status = IRC_MEMERROR;
    return;
  }
  ircSend(ircConnection, buffer);
  free(buffer);
  ircConnection->status = IRC_JOINED;
}

void ircUser(ircCtx_t *ircConnection, char *nick, char *realname){
  char *buffer = NULL;
  int  bufferLen = asprintf(&buffer, "USER %s 0 * :%s", nick, realname);
  if (bufferLen == -1) {
    ircConnection->status = IRC_MEMERROR;
    return;
  }
  ircSend(ircConnection, buffer);
  free(buffer);
}

void ircNick(ircCtx_t *ircConnection, char *nick){
  char *buffer = NULL;
  int bufferLen = asprintf(&buffer, "NICK %s", nick);
  if (bufferLen == -1) {
    ircConnection->status = IRC_MEMERROR;
    return;
  }
  ircSend(ircConnection, buffer);
  free(buffer);
  ircConnection->nick = strdup(nick);
}
