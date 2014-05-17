#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>

#define MSG_MSTR 1
#define MSG_SLV  2
#define MSG_DEBUG  3
#define MSG_CONF 4

extern char debug_message[1024];
#define d_send(id, ...) { sprintf(debug_message, __VA_ARGS__); pvm_initsend(PvmDataDefault); pvm_pkstr(debug_message); pvm_send(id, MSG_CONF);}
#define msg_send(id, msg_type, core) { pvm_initsend(PvmDataDefault); pvm_pkbyte((char*)&(core), sizeof(core), 1); pvm_send((id), (msg_type));}
#define msg_rcv(msg_type, core) { pvm_recv(-1, (msg_type)); pvm_upkbyte(&(core), sizeof(core), 1);}

typedef struct {
	int sender_id;
	int legion_card;
	int timestamp;
	int resource_id;
} msg;