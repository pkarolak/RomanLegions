#include "master.h"

int main()
{
	int mytid;
	int tids[SLAVENUM];		/* slave task ids */
	char slave_name[NAMESIZE];
	int nproc, i, who;

	mytid = pvm_mytid();

	nproc=pvm_spawn(SLAVENAME, NULL, PvmTaskDefault, "", SLAVENUM, tids);

	for( i=0 ; i<nproc ; i++ )
	{
		pvm_initsend(PvmDataDefault);
		pvm_pkint(&mytid, 1, 1);
		pvm_pkint(&i, 1, 1);
		pvm_send(tids[i], MSG_MSTR);
	}

	for( i=0 ; i<nproc ; i++ )
	{
		pvm_recv( -1, MSG_SLV );
		pvm_upkint(&who, 1, 1 );
		pvm_upkstr(slave_name );
		printf("%d: %s\n",who, slave_name);
	}

	pvm_exit();
	return 0;
}

