#include "callbacks.h"

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
