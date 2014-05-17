#ifndef _MASTER_H_
#define _MASTER_H_

#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>

#define SLAVENAME "slave"
#define LEGION_NUM	10
#define ROUTE_NUM	6
#define LEGION_MAX_CARD 100

#define NAMESIZE   64

#define MSG_MSTR	1
#define MSG_SLV		2
#define MSG_DEBUG	3
#define MSG_CONF 4


extern char debug_message[1024];
#define d_send(id, ...) { sprintf(debug_message, __VA_ARGS__); pvm_initsend(PvmDataDefault); pvm_pkstr(debug_message); pvm_send((idpvm_initsend(PvmDataDefault);), MSG_DEBUG);}
#define msg_send(id, msg_type, msg) { pvm_initsend(PvmDataDefault); pvm_pkbyte(msg, 1, 1); pvm_send((id), (msg_type));}
#define msg_rcv(msg_type, msg) { pvm_recv(-1, msg_type); pvm_upkbyte(msg, 1, 1);}

typedef struct {
	int sender_id;
	int legion_card;
	int timestamp;
	int resource_id;
} msg;

#endif