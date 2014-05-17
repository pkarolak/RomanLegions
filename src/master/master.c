//<>< P.K.
#include "master.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

char debug_message[1024];

int main(int argc, char* argv[]) {

	int c;
    char *lopt = NULL, *ropt = NULL, *copt = NULL;
    while ( (c = getopt(argc, argv, "l:r:c:")) != -1) {
        switch (c) {
	        case 'l':
	            lopt = optarg;
	            break;
	        case 'r':
	            ropt = optarg;
	            break;
	        case 'c':
	        	copt = optarg;
	            break;
	        case '?':
	            break;
	        default:
	            printf ("?? getopt returned character code 0%o ??\n", c);
        }
    }
    if (optind < argc) {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        printf ("\n");
    }

    int legion_num = lopt ? atoi(lopt) : LEGION_NUM;
    int legion_max_card = copt ? atoi(copt) : LEGION_MAX_CARD;
    int route_num = ropt ? atoi(ropt) : ROUTE_NUM;

    printf("legion_num = %d\n", legion_num);
    printf("route_num = %d\n", route_num);

	int mytid;
	int tids[legion_num];	
	int nproc, i, who;

	mytid = pvm_mytid();

	nproc = pvm_spawn(SLAVENAME, NULL, PvmTaskDefault, "", legion_num, tids);

	for( i = 0 ; i < nproc ; ++i ) {
		printf("%d\n", tids[i]);
	}

	srand(time(NULL));

	printf("\nLegions card:\n");	
	int legion_card[legion_num];
	for( i = 0 ; i < legion_num ; ++i) {
		legion_card[i] = rand()%legion_max_card + 1;
		printf("%d, ", legion_card[i]);
	}

	printf("\nRoute capacities:\n");
	int route_capacity[route_num];
	for( i = 0 ; i < route_num; ++i) {
		route_capacity[i] = 2*(rand()%legion_max_card) + legion_max_card;
		printf("%d, ", route_capacity[i]);
	}

	for( i = 0 ; i < nproc ; ++i ) {
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&mytid, 1, 1); 		// master id
		pvm_pkint(&i, 1, 1);			// index
		pvm_pkint(&nproc, 1, 1);		// nproc
		pvm_pkint(&legion_num, 1, 1);	// number of legions
		pvm_pkint(&legion_card[i], 1, 1);// card of current legion
		pvm_pkint(&route_num, 1, 1);	// number of routes
		pvm_pkint(route_capacity, route_num, 1);// routes capacity
		pvm_pkint(tids, nproc, 1);		// array of tids
		pvm_send(tids[i], MSG_MSTR);	// receiver tid
	}

	int num;
	for( i = 0 ; i < nproc ; ++i ) {
		printf("\nrcv: %d\n", i);
		pvm_recv( -1, MSG_SLV );
		pvm_upkint(&who, 1, 1 );
		pvm_upkint(&num, 1, 1 );
		printf("%d: %d\n",who, num);
	}

	printf("\ncore rvc\n");
	
	char str[1024];
	for( i = 0 ; i < nproc ; ++i ) {
		pvm_recv( -1, MSG_DEBUG );
		pvm_upkstr(str);
		printf("%s\n", str);
	}


	msg* core = calloc(1, sizeof(msg));
	for( i = 0 ; i < nproc ; ++i ) {
		pvm_recv( -1, MSG_CONF );
		pvm_upkbyte((char*)core, sizeof(msg), 1 );
		printf("sender: %d, card: %d, time: %d, res: %d\n", core->sender_id, core->legion_card, core->timestamp, core->resource_id);
	}
	free(core);
	
	pvm_exit();
	return 0;
}

