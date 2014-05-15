#include <stdio.h>
#include <stdlib.h>
#include <pvm3.h>

#define MSG_MSTR 1
#define MSG_SLV  2
#define MSG_DEBUG  3

extern char debug_message[1024];
#define d_send(id, ...) { sprintf(debug_message, __VA_ARGS__); pvm_initsend(PvmDataDefault); pvm_pkstr(debug_message); pvm_send(id, MSG_DEBUG);}

