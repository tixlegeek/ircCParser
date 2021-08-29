#ifndef IRC_CALLBACK_H
#define IRC_CALLBACK_H
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
