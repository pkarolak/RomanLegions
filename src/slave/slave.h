//<>< P.K.
#ifndef _SLAVE_H_
#define _SLAVE_H_

#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>
#include "../common/message.h"
#include "../common/vtime.h"

int CriticalSection();
int ResourceRequest(int ResourceId);
int FreeResource(int ResourceId);
int ReceiveMessageRoutines();


#endif