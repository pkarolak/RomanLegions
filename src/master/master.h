//<>< P.K.
#ifndef _MASTER_H_
#define _MASTER_H_

#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>
#include "../common/message.h"

#define SLAVENAME "slave"
#define LEGION_NUM	8
#define ROUTE_NUM	5
#define LEGION_MAX_CARD 10

extern char debug_message[1024];
#define d_send(id, ...) { sprintf(debug_message, __VA_ARGS__); pvm_initsend(PvmDataDefault); pvm_pkstr(debug_message); pvm_send((idpvm_initsend(PvmDataDefault);), MSG_DEBUG);}

#endif