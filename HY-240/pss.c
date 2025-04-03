/***************************************************************
 *
 * file: pss.h
 *
 * @Author  Nikolaos Vasilikopoulos (nvasilik@csd.uoc.gr)
 * @Version 20-10-2020
 *
 * @e-mail       hy240-list@csd.uoc.gr
 *
 * @brief   Implementation of the "pss.h" header file for the Public Subscribe System,
 * function definitions
 *
 *
 ***************************************************************
 */
 /*Michalis Grigorakis csd4335 Project Phase A
 */
#include <stdio.h>
#include <stdlib.h>

#include "pss.h"

/**
 * @brief Optional function to initialize data structures that
 *        need initialization
 *
 * @return 0 on success
 *         1 on failure
 */
 
 /*Global Variable, pointer to the head of the Group list*/
struct Group *group_head[MG];
struct SubInfo *SubInfo_head;

int initialize(){
    int i;
    
    for(i=0;i<MG;i++){
        group_head[i] = (struct Group*)malloc(sizeof(struct Group));
        group_head[i]->gId = -1;
        group_head[i]->gfirst = (struct Info*)malloc(sizeof(struct Info));
        group_head[i]->gfirst->iprev = NULL;
        group_head[i]->glast = (struct Info*)malloc(sizeof(struct Info));
        group_head[i]->glast->inext = NULL;
        group_head[i]->glast->iprev = group_head[i]->gfirst;
        group_head[i]->gfirst->inext = group_head[i]->glast;
        group_head[i]->ggsub = (struct Subscription*)malloc(sizeof(struct Subscription));
        group_head[i]->ggsub->sId = -1;
        group_head[i]->ggsub->snext = NULL;
    }
    SubInfo_head =(struct SubInfo*)malloc(sizeof(struct SubInfo));
    SubInfo_head->snext = NULL;
    return EXIT_SUCCESS;
}

/**
 * @brief Free resources
 *
 * @return 0 on success
 *         1 on failure
 */
int free_all(void){
    return EXIT_SUCCESS;
}

/**
 * @brief Insert info
 *
 * @param iTM Timestamp of arrival
 * @param iId Identifier of information
 * @param gids_arr Pointer to array containing the gids of the Event.
 * @param size_of_gids_arr Size of gids_arr including -1
 * @return 0 on success
 *          1 on failure
 */
int Insert_Info(int iTM,int iId,int* gids_arr,int size_of_gids_arr){
    int gcount = 0;//gId
    int gcounter = 0;
    struct Info *new_info;
    struct Info *temp;
    int count=1;
    int counter = 0;//For selecting the correct group
    if(iId < 1)return EXIT_FAILURE;

    while(gcount < size_of_gids_arr - 1){
        counter = gids_arr[gcount];//The number of Group
        group_head[counter]->gId = gids_arr[gcount];//We set the group Id
        
        /*New node to insert to the correct Group*/
        new_info = (struct Info*)malloc(sizeof(struct Info));
        new_info->iId = iId;
        new_info->itm = iTM;
        group_head[counter]->glast->iprev = new_info;//Setting the prev node
        /*1st Element*/
        if(group_head[counter]->gfirst->inext==group_head[counter]->glast){
            group_head[counter]->gfirst->inext=new_info;
            new_info->iprev=group_head[counter]->gfirst;
            group_head[counter]->glast->iprev = new_info;
        }else{
            struct Info *temp = group_head[counter]->gfirst->inext;
            if(group_head[counter]->gfirst->inext!=group_head[counter]->glast){
                while(temp != new_info && temp->inext != group_head[counter]->glast){
                    temp = temp->inext;
                    if(temp->inext == group_head[counter]->glast)break;
                }
                new_info->iprev = temp;
                (new_info->iprev)->inext = new_info;
            }
        }
        new_info->inext = group_head[counter]->glast;
        /*Sorting the Elements*/
        temp = group_head[counter]->gfirst->inext;
        while(temp->inext != group_head[counter]->glast){
            if(temp->itm > new_info->itm){
                swapper(temp,new_info);
            }
            temp = temp->inext;
        }
        /*Printing*/
        printf("GROUPID=<%d>,INFOLIST=<",group_head[counter]->gId);
        struct Info *current;
        current = group_head[counter]->gfirst->inext;
        count = 1;
        while(current != new_info){
            count = count + 1;
            current = current->inext;
        if(current->inext == group_head[counter]->glast)break;
        }
        current = group_head[counter]->gfirst->inext;
        while(count != 0){
            if(count ==1){
                printf("%d",current->iId);
            }else{
                printf("%d,",current->iId);
            }
            current = current->inext;
            count = count - 1;
        }
        printf(">\n");
        /*Steps*/
        gcount = gcount + 1;
        gcounter = gcounter + 1;
    }
    return EXIT_SUCCESS;
}


/*Sorting the iIds by itm*/
void swapper(struct Info *a,struct Info *b){
    struct Info *temp;
    temp = (struct Info*)malloc(sizeof(struct Info));

    temp->iId = a->iId;
    temp->itm = a->itm;
    temp->igp[MG] = a->igp[MG];

    a->iId = b->iId;
    a->itm = b->itm;
    a->igp[MG] = b->igp[MG];

    b->iId = temp->iId;
    b->itm = temp->itm;
    b->igp[MG] = temp->igp[MG];

}
/**
 * @brief Subsriber Registration
 *
 * @param sTM Timestamp of arrival
 * @param sId Identifier of subscriber
 * @param gids_arr Pointer to array containing the gids of the Event.
 * @param size_of_gids_arr Size of gids_arr including -1
 * @return 0 on success
 *          1 on failure
 */
int Subscriber_Registration(int sTM,int sId,int* gids_arr,int size_of_gids_arr){
    if(sId == -1)EXIT_FAILURE;
    int gcount = 0;//gId
    int gcounter = 0;
    struct Subscription *new_sub;
    struct SubInfo *new_SubInfo;
    struct SubInfo *temp;
    int count=1;
    int counter = 0;//For selecting the correct group
    int same = 0;
    struct SubInfo *temp1;

    while(gcount < size_of_gids_arr - 1){
        counter = gids_arr[gcount];//The number of Group
        group_head[counter]->gId = gids_arr[gcount];//We set the group Id
        
        /* Creating The gsub List:*/
        new_sub = (struct Subscription*)malloc(sizeof(struct Subscription));
        struct Subscription *prev;
        prev = (struct Subscription*)malloc(sizeof(struct Subscription));
        new_sub->sId = sId;
        new_sub->snext = NULL;
        /*First Element*/
         if(group_head[counter]->ggsub->snext==NULL){
            group_head[counter]->ggsub->snext=new_sub;
        }else{
            struct Subscription *temp = group_head[counter]->ggsub->snext;
            if(group_head[counter]->ggsub->snext!=NULL){
                while(temp != new_sub && temp->snext != NULL){
                    prev = temp;
                    temp = temp->snext;
                    if(temp->snext == NULL)break;
                }
                prev->snext = temp;
                temp->snext = new_sub;
            }
        }
       /*Steps*/
        gcount = gcount + 1;
        gcounter = gcounter + 1;
    }
        
    /*Creating the SubInfo List:*/
    new_SubInfo = (struct SubInfo*)malloc(sizeof(struct SubInfo));
    new_SubInfo->sId = sId;
    new_SubInfo->stm = sTM;
    new_SubInfo->snext = NULL;
    /*1st Element*/
    if(SubInfo_head->snext==NULL){
        SubInfo_head->snext = new_SubInfo;
    }else{
        struct SubInfo *temps = SubInfo_head->snext;
        struct SubInfo *prevs;
        prevs= (struct SubInfo*)malloc(sizeof(struct SubInfo));
        if(SubInfo_head->snext!=NULL){
            while(temps != new_SubInfo && temps->snext != NULL){
                prevs = temps;
                if(temps->snext == NULL)break;
                temps = temps->snext;
                if(temps->snext == NULL)break;
            }
            prevs->snext = temps;
            temps->snext = new_SubInfo;
        }
    }
    /*Sorting the Elements*/
    temp1 = SubInfo_head->snext;
    while(temp1->snext != NULL){
        if(temp1->stm > new_SubInfo->stm){
            swapper_s(temp1,new_SubInfo);
        }
        if(temp1->snext == NULL)break;
        temp1 = temp1->snext;
    }
    printf("SUBSCRIBERLIST = <");
    struct SubInfo *current1;
    current1 = SubInfo_head->snext;

    while(current1->snext!=NULL){
        printf("%d,",current1->sId);
        if(current1->snext == NULL)break;
        current1 = current1->snext;
    }
    printf("%d",current1->sId);
    printf(">\n");
    
    gcount = 0;//gId
    gcounter = 0;
    counter = 0;
    while(gcount < size_of_gids_arr - 1){
        counter = gids_arr[gcount];//The number of Group
        /*Printing*/
        printf("GROUPID=<%d>,SUBLIST=<",group_head[counter]->gId);
        struct Subscription *current;
        current = group_head[counter]->ggsub->snext;
     //   printf("%d\n",current->sId);
        count = 1;
        while(current != new_sub){
            count = count + 1;
            if(current->snext == NULL)break;
            current = current->snext;
            if(current->snext == NULL)break;
        }
        current = group_head[counter]->ggsub->snext;
        while(count != 0){
            if(count ==1){
                printf("%d",current->sId);/*Not the comma*/
            }else{
                printf("%d,",current->sId);/*For the comma*/
            }
            if(current->snext == NULL)break;
            current = current->snext;
            count = count - 1;
        }
        
        printf(">\n");
        /*Steps*/
        gcount = gcount + 1;
        gcounter = gcounter + 1;
    }
    return EXIT_SUCCESS;
    
}

void swapper_s(struct SubInfo *a,struct SubInfo *b){
    struct SubInfo *temp;
    temp = (struct SubInfo*)malloc(sizeof(struct SubInfo));
  //  struct Info *temp2;
  //  temp2 = (struct Info*)malloc(sizeof(struct Info));

    temp->sId = a->sId;
    temp->stm = a->stm;
  //  temp->sgp[MG]->igp[MG] = a->sgp[MG]->igp[MG];

    a->sId = b->sId;
    a->stm = b->stm;
//    a->sgp[MG]->igp[MG] = b->sgp[MG]->igp[MG];

    b->sId = temp->sId;
    b->stm = temp->stm;
  //  b->sgp[MG]->igp[MG] = temp->sgp[MG]->igp[MG];
}

/**
 * @brief Consume Information for subscriber
 *
 * @param sId Subscriber identifier
 * @return 0 on success
 *          1 on failure
 */
int Consume(int sId){

    return EXIT_SUCCESS;
}
/**
 * @brief Delete subscriber
 *
 * @param sId Subscriber identifier
 * @return 0 on success
 *          1 on failure
 */
int Delete_Subscriber(int sId){
    
    if(sId == -1)EXIT_FAILURE;
    int count = 0;
    struct Subscription *temp;
    struct Subscription *prev;
    int num = 0;
    while(count < MG-1){
        temp = group_head[count]->ggsub;
        prev = group_head[count]->ggsub;
        if(temp!=NULL){
            while(temp->sId != sId && temp->snext != NULL){
                prev = temp;
                if(temp->snext == NULL)break;
                temp = temp->snext;
            }
            while(temp->sId != -1){
                if(temp->sId == sId){
                    prev->snext = temp->snext;
                    temp->snext = NULL;
                    temp = prev->snext;
                }
                prev = temp;
                if(temp != NULL && temp->snext != NULL){
                    temp = temp->snext;
                }else{
                    break;
                }
            }
        }
        count = count + 1;
    }
    count = 0;
    struct SubInfo *temp1 = SubInfo_head;
    struct SubInfo *prev1 = SubInfo_head; 
    while(count < MG-1){
        temp1 = SubInfo_head;
        prev1 = SubInfo_head;
        if(temp1!=NULL){
            while(temp1->sId != sId && temp1->snext != NULL){
                prev1 = temp1;
                if(temp1->snext == NULL)break;
                temp1 = temp1->snext;
            }
            while(temp1->sId != -1){
                if(temp1->sId == sId){
                    prev1->snext = temp1->snext;
                    temp1->snext = NULL;
                    temp1 = prev1->snext;
                }
                prev1 = temp1;
                if(temp1 != NULL && temp1->snext != NULL){
                    temp1 = temp1->snext;
                }else{
                    break;
                }
            }
        }
        count = count + 1;
    }

    printf("SUBSCRIBERLIST = <");
    struct SubInfo *current1;
    current1 = SubInfo_head->snext;
    while(current1->snext!=NULL && current1 != NULL){
        printf("%d,",current1->sId);
        if(current1->snext == NULL)break;
        current1 = current1->snext;
    }
    printf("%d",current1->sId);
    printf(">\n");
    
    /*Printing*/
    int gcount = 0;//gId-Loop
    int gcounter = 0;
    int counter = 0;//Prints
    int del = 0;
    count = 0;

    while(gcount < MG){
        if(group_head[gcount]->gId != -1){
            printf("GROUPID=<%d>,SUBLIST=<",group_head[gcount]->gId);
            struct Subscription *current;
            if(group_head[gcount]->ggsub->snext != NULL){
                current = group_head[gcount]->ggsub->snext;
            
                count = 1;
                while(current != NULL){
                    count = count + 1;
                    if(current->snext == NULL)break;
                    current = current->snext;
                    if(current->snext == NULL)break;
                }
                current = group_head[gcount]->ggsub->snext;
                if(current == NULL)del = 1;
                while(count != 0){
                    if(count == 1){
                        if(del == 0){
                            printf("%d",current->sId);/*Not the comma*/
                        }else{
                            printf("-1");
                        }
                    }else{
                        if(del == 0){
                            printf("%d,",current->sId);/*For the comma*/
                        }else{
                            printf("-1");
                        }
                    }
                    if(current->snext == NULL)break;
                    if(current == NULL)break;
                    current = current->snext;
                    count = count - 1;
                }
            }
            printf(">\n");
        }
        gcount = gcount + 1;

    }
        



    return EXIT_SUCCESS;
}
/**
 * @brief Print Data Structures of the system
 *
 * @return 0 on success
 *          1 on failure
 */
int Print_all(void){
    int gcount = 0;//gId-Loop
    int gcounter = 0;
    int counter = 0;//Prints
    int count = 0;

    int counter1 = 0;//Prints
    int count1 = 0;
    /*Printing the InfoList*/
    while(gcount < MG){
        if(group_head[gcount]->gId != -1){
            printf("GROUPID=<%d>,INFOLIST=<",group_head[gcount]->gId);
            struct Info *current;
            current = group_head[gcount]->gfirst;
            if(group_head[gcount]->gfirst->inext != NULL && group_head[gcount]->gfirst->inext->iId != 0){
                count = 1;
                while(current->inext != group_head[gcount]->glast){
                    count = count + 1;
                    current = current->inext;
                }
                current = group_head[gcount]->gfirst->inext;
                while(count != 0){
                    if(count == 1){    
                        printf("%d",current->iId);/*Not the comma*/
                    }else{
                        printf("%d,",current->iId);/*For the comma*/
                    }
                    if(current->inext == group_head[gcount]->glast)break;
                    current = current->inext;
                    count = count - 1;
                }
            }
            printf(">,");
            printf("SUBLIST=<");
            /*Printing the SUBLIST*/
            struct Subscription *current1;
            current1 = group_head[gcount]->ggsub->snext;
            if(group_head[gcount]->ggsub->snext != NULL){
                current1 = group_head[gcount]->ggsub->snext;
            
                count1 = 1;
                while(current1 != NULL){
                    count1 = count1 + 1;
                    if(current1->snext == NULL)break;
                    current1 = current1->snext;
                    if(current1->snext == NULL)break;
                }
                current1 = group_head[gcount]->ggsub->snext;
                while(count1 != 0){
                    if(count1 == 1){
                        printf("%d",current1->sId);/*Not the comma*/
                    }else{
                        printf("%d,",current1->sId);/*For the comma*/
                    }
                    if(current1->snext == NULL)break;
                    if(current1 == NULL)break;
                    current1 = current1->snext;
                    count1 = count1 - 1;
                }
            }
            printf(">\n");
        }
        gcount = gcount + 1;
    }
    int countsub = 0;
    /*Printing the SUBSCRIBERLIST*/
    printf("SUBSCRIBERLIST = <");
    struct SubInfo *current2;
    current2 = SubInfo_head->snext;
    while(current2->snext!=NULL && current2 != NULL){
        countsub = countsub + 1;
        printf("%d,",current2->sId);
        if(current2->snext == NULL)break;
        current2 = current2->snext;
    }
    countsub = countsub + 1;
    printf("%d",current2->sId);
    printf(">\n");

    int temp = 0;
    gcount = 0;//gId-Loop
    int gcount1 = 0;
    gcounter = 0;
    counter = 0;//Prints
    count = 0;
    struct Subscription *current1;
    struct Subscription *currtemp;
    /*Printing the SUBSCRIBERS*/
    while(temp < countsub){
        while(gcount < MG){
            if(group_head[gcount]->ggsub->snext != NULL ){
                current1 = group_head[gcount]->ggsub->snext;
                printf("SUBSCRIBERID=<%d>,GROUPLIST=<",current1->sId);
                while(gcount1 < MG){
                    count1 = 0;
                    if(group_head[gcount1]->gId != -1){
                        if(group_head[gcount1]->ggsub->snext != NULL){
                            currtemp = group_head[gcount1]->ggsub->snext;
                
                            count1 = 1;
                            while(currtemp != NULL){
                                count1 = count1 + 1;
                                if(currtemp->snext == NULL)break;
                                currtemp = currtemp->snext;
                                if(currtemp->snext == NULL)break;
            
                            }
                        
                            currtemp = group_head[gcount1]->ggsub->snext;
                            while(count1 != 0){
                                if(count1 == 1){
                                    if(currtemp->sId == current1->sId)
                                    printf("%d",group_head[gcount1]->gId);/*Not the comma*/
                                    }else{
                                        if(currtemp->sId == current1->sId)
                                    printf("%d,",group_head[gcount1]->gId);/*For the comma*/
                                }
                                if(currtemp->snext == NULL)break;
                                if(currtemp == NULL)break;
                                currtemp = current1->snext;
                                count1 = count1 - 1;
                            }
                        }
                    }
                    gcount1 = gcount1 + 1;
                }
                printf(">\n");
                current1 = current1->snext;
            }
            gcount = gcount + 1;
       //     temp = temp + 1;
        }
        temp = temp + 1;
        //gcount = gcount + 1;
    }

    return EXIT_SUCCESS;
}


