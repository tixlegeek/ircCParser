#include "irc.h"

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
  uint16_t argLen;
  uint8_t i = 0;
  // reinit ptrs
  ircConnection->channelPtr = NULL;
  ircConnection->channelLen = 0;
  ircConnection->privmsgPtr = NULL;
  ircConnection->privmsgLen = 0;
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
  argLen = strlen(args);

  // Message parsing
  // PING requests
  if ((strncmp(cmd, "PING", 4)) == 0) {
    buffer = malloc((argLen + 5) * sizeof(char));
    if (buffer == NULL) {
      ircConnection->status = IRC_MEMERROR;
      ircConnection->command = IRC_GLOBAL_ERROR;
    }
    sprintf(buffer, "PONG %s", args);
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
    // struct ircCallbackList_t *callbacks =
    // (struct ircCallbackList_t*)ircConnection->callbacks;
    // compute the place of the pointer on the buffer

    // trims PRIVMSG
    if (strncmp(channel, ircConnection->nick, strlen(ircConnection->nick)) ==
        0) {
      printf("%s %s ************* PRIVATE MESSAGE ******************\n",
             ircConnection->nick, channel);
    }
    while (*privmsg == ' ')
      privmsg++;

    if (privmsg[0] == '!') {
      ircConnection->senderPtr =
          (ircConnection->buffer + (uint16_t)(sender - ptr + 1));
      ircConnection->senderLen = strlen(sender);
      *(ircConnection->senderPtr + ircConnection->senderLen) = '\0';

      ircConnection->channelPtr =
          (ircConnection->buffer + (uint16_t)(channel - ptr + 1));
      ircConnection->channelLen = strlen(channel);
      *(ircConnection->channelPtr + ircConnection->channelLen) = '\0';

      ircConnection->privmsgPtr =
          (ircConnection->buffer + (uint16_t)(privmsg - ptr + 1));
      ircConnection->privmsgLen = strlen(privmsg);
      *(ircConnection->privmsgPtr + ircConnection->privmsgLen) = '\0';

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
    ircConnection->command = IRC_CMD_PRIVMSG;
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
#ifdef IRC_DEBUG
    printf("[!]" WHT " *** User %s joined ***\n" CRST, sender);
#endif
    ircConnection->command = IRC_CMD_JOIN;
  }
  // PART and QUIT are quite the same stuff.
  else if ((strncmp(cmd, "PART", 4)) == 0) {
#ifdef IRC_DEBUG
    printf("[!]" WHT " *** User %s exited: PART ***\n" CRST, sender);
#endif
    ircConnection->command = IRC_CMD_PART;
  } else if ((strncmp(cmd, "QUIT", 4)) == 0) {
#ifdef IRC_DEBUG
    printf("[!]" WHT " *** User %s exited: QUIT ***\n" CRST, sender);
#endif
    ircConnection->command = IRC_CMD_QUIT;
  }
  free(ptrdup);

  return ircConnection->command;
}

void ircPubmsg(ircCtx_t *ircConnection, char *channel, char *dst,
               char *privmsg) {
  uint16_t bufferLen = strlen(dst) + strlen(channel) + 2;
  char *buffer = malloc(sizeof(char) * bufferLen);
  if (buffer == NULL)
    return;

  sprintf(buffer, "%s :%s", channel, dst);
  ircPrivmsg(ircConnection, buffer, privmsg);
  free(buffer);
}

void ircPrivmsg(ircCtx_t *ircConnection, char *dst, char *privmsg) {
  char template[] = "PRIVMSG %s :%s";
  uint16_t privmsgLen = strlen(template) - 4 + strlen(dst) + strlen(privmsg);
  char *buffer = malloc(sizeof(char) * privmsgLen);
  if (buffer == NULL)
    return;

  sprintf(buffer, template, dst, privmsg);
  ircSend(ircConnection, buffer);
  free(buffer);
}

void ircSend(ircCtx_t *ircConnection, char *cmd) {
  char *buffer;
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

void ircIdentify(ircCtx_t *ircConnection) {
  sleep(1);
  char *buffer = NULL;
  // Sends strings to the IRC Server to complete the login process
  asprintf(&buffer, "USER %s 0 * :%s", ircConnection->nick, IRC_REALNAME);
  ircSend(ircConnection, buffer);
  free(buffer);

  sleep(1);
  asprintf(&buffer, "NICK %s", ircConnection->nick);
  ircSend(ircConnection, buffer);
  free(buffer);

  sleep(1);
  asprintf(&buffer, "JOIN %s", ircConnection->channel);
  ircSend(ircConnection, buffer);
  free(buffer);
  ircConnection->status = IRC_JOINED;
}

void ircClient(ircCtx_t *ircConnection) {
  char tmpc = 0;
  uint8_t reading = 0;

  ircConnection->status = IRC_CONNECTED;
  ircIdentify(ircConnection);
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
