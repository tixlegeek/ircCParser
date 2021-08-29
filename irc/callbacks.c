#include "callbacks.h"
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
// Create a new list and/or add an element to it, and then returns the new list.
ircCallbackList_t *ircCallbackAddList(ircCallbackList_t *ircCallbackList, ircCallback_t *ircCallback){
    if(ircCallbackList == NULL)
    {
      ircCallbackList = malloc(sizeof(ircCallbackList_t));
      ircCallbackList->len=0;
    }
    ircCallbackList->len++;
    ircCallbackList->list = (ircCallback_t**)realloc(ircCallbackList->list, sizeof(ircCallback_t*)*ircCallbackList->len);
    if(ircCallbackList->list==NULL)
      return NULL;
    ircCallbackList->list[ircCallbackList->len-1]=ircCallback;
    return ircCallbackList;
}

void ircCallbackFreeList(ircCallbackList_t *ircCallbackList){
  uint8_t i=0;
    for(i=0; i< ircCallbackList->len; i++)
      ircFreeCb(ircCallbackList->list[i]);
    free(ircCallbackList->list);
    free(ircCallbackList);
}

// Create a ircCallback_t structure to be used as callback element
ircCallback_t *ircCreateCb(char *keyword, void (*cb)()){
  ircCallback_t *ircCallback = malloc(sizeof(ircCallback_t));
  if(ircCallback==NULL){
    return NULL;
  }
  ircCallback->kwlen = strlen(keyword);
  ircCallback->keyword = strdup(keyword);
  ircCallback->cb = cb;
  return ircCallback;
}
// Freeing a ircCallback_t struct
void ircFreeCb(ircCallback_t *ircCallback){
  free(ircCallback->keyword);
  free(ircCallback);
}
