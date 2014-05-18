//<>< P.K.
#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>
#include "vtime.h"


typedef struct message {
	int sender_id;
	int legion_card;
	vtimer timer;
	int resource_id;
} message;

typedef enum msg_type {
	MASTER,
	SLAVE,
	COMMUNICATE,
	REQUEST,
	FREE,
	ACK
} msg_type;

extern char debug_message[1024];
#define SendComunicate(id, ...) { sprintf(debug_message, __VA_ARGS__); pvm_initsend(PvmDataDefault); pvm_pkstr(debug_message); pvm_send(id, COMUNICATE);}
message* Message(int in_sender, int in_card, vtimer in_timer, int in_res);
int FreeMessage(message* m);
int SendMessage(int in_recievrer, message* in_message, msg_type in_type);
int RecieveMessage(message* m, msg_type in_type);
int NonBlockingRecieveMessage(message* m, msg_type in_type);
void PrintMessage(message* m);
#endif