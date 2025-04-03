#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

/*#include "pss.h"*/

int main(void){

    
    /*I*/
	int itm;
	int iId;
    unsigned int num_of_gids;
    int gids_arr;
    itm=12;
    iId=24;
    gids_arr= 10;
    num_of_gids=2;
	if (Insert_Info(itm,iId,gids_arr,num_of_gids)==0)
	{
	//	printf("<%d> <%d> DONE\n", itm,iId);
	}
	else
	{
		printf("failed\n");
	}
		
    return 0;
}