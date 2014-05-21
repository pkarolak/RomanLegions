//<>< P.K.
#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>
#include "vtime.h"

typedef enum msg_type {
	MASTER,
	SLAVE,
	COMMUNICATE,
	ACK,
	REQUEST,
	FREE,
	DBG,
	QUEUE
} msg_type;

typedef struct message {
	int sender_id;
	int legion_card;
	vtimer timer;
	int resource_id;
	msg_type type;
} message;


extern char debug_message[1024];
#define SendComunicate(id, ...) { sprintf(debug_message, __VA_ARGS__); pvm_initsend(PvmDataDefault); pvm_pkstr(debug_message); pvm_send(id, DBG);}
#define SendQueue(id, array,...) { sprintf(debug_message, __VA_ARGS__); pvm_initsend(PvmDataDefault); pvm_pkstr(debug_message); pvm_pkint(array, 1024, 1); pvm_send(id, QUEUE);}
message* Message(int in_sender, int in_card, vtimer in_timer, int in_res, msg_type type);
int FreeMessage(message* m);
int SendMessage(int in_recievrer, message* in_message, msg_type in_type);
int ReceiveMessage(message* m, msg_type in_type);
int NonBlockingReceiveMessage(message* m, msg_type in_type);
void PrintMessage(message* m);
#endif