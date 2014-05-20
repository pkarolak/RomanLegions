//<>< P.K.
#include "message.h"


message* Message(int in_sender, int in_card, vtimer in_timer, int in_res) {
	message* m = calloc(1, sizeof(message));
	m->sender_id = in_sender;
	m->legion_card = in_card;
	m->timer = in_timer;
	m->resource_id = in_res;
	return m;
}

int FreeMessage(message* m) {
	free(m);
	return 0;
}

int SendMessage(int in_reciever, message* in_message, msg_type in_type) {
	pvm_initsend(PvmDataDefault);
	pvm_pkbyte((char*)in_message, sizeof(message), 1);
	return pvm_send(in_reciever, in_type);
}

int ReceiveMessage(message* m, msg_type in_type) {
	int stat = pvm_recv(-1, in_type);
	pvm_upkbyte((char*)m, sizeof(message), 1);
	return stat;
}

int NonBlockingReceiveMessage(message* m, msg_type in_type) {
	int stat = pvm_nrecv(-1, in_type);
	if( stat ) {
		pvm_upkbyte((char*)m, sizeof(message), 1);
	}
	return stat;
}

void PrintMessage(message* m) {
//	printf("Sender: %d, Card: %d, Resource: %d\n", m->sender_id, m->legion_card, m->resource_id);
//	PrintVtimer(&(m->timer));
}