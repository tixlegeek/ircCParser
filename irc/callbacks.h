#ifndef IRC_CALLBACK_H
#define IRC_CALLBACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
typedef struct {
  char *keyword;
  uint8_t kwlen;
  void (*cb)(void *ircConnection);
} ircCallback_t;

typedef struct {
  ircCallback_t **list;
  uint16_t len;
} ircCallbackList_t;

ircCallbackList_t *ircCallbackAddList(ircCallbackList_t *ircCallbackList, ircCallback_t *ircCallback);
void ircCallbackFreeList(ircCallbackList_t *ircCallbackList);
ircCallback_t *ircCreateCb(char *keyword, void (*cb)());
void ircFreeCb(ircCallback_t *ircCallback);

#endif
