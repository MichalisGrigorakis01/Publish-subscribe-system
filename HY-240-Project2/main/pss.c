/***************************************************************
 *
 * file: pss.h
 *
 * @Author  Nikolaos Vasilikopoulos (nvasilik@csd.uoc.gr), John Petropoulos (johnpetr@csd.uoc.gr)
 * @Version 30-11-2022
 *
 * @e-mail       hy240-list@csd.uoc.gr
 *
 * @brief   Implementation of the "pss.h" header file for the Public Subscribe System,
 * function definitions
 *
 *
 ***************************************************************
 */
 /*Michalis Grigorakis csd4335 Project Phase B
 */

#include <stdio.h>
#include <stdlib.h>

#include "pss.h"

struct Group* group_tree[MG];//Pointer to the head of the players tree
struct Group* group_sentinel[MG];//Pointer to the sentinel of the players tree

struct SubInfo* sub_tree[MG];
int hashTableSize;//m
int universalHashingNumber,a,b; /*p, Variables for the universal hashing*/ 
//struct SubInfo *SubInfo_head;
/**
 * @brief Optional function to initialize data structures that
 *        need initialization
 *
 * @param m Size of the hash table.
 * @param p Prime number for the universal hash functions.
 *
 * @return 0 on success
 *         1 on failure
 */
int initialize(int m, int p){
    int i;
    for(i=0;i<MG;i++){
        group_sentinel[i] = (struct Group*)malloc(sizeof(struct Group));
        group_sentinel[i]->gId = -1;
        group_sentinel[i]->gr = (struct Info*)malloc(sizeof(struct Info));
        group_sentinel[i]->gr->iId = -1;
        group_sentinel[i]->gr->itm = -1;
        group_sentinel[i]->gr->ip = NULL;
        group_sentinel[i]->gr->irc = NULL;
        group_sentinel[i]->gr->ilc = NULL;
        group_sentinel[i]->gsub = (struct Subscription*)malloc(sizeof(struct Subscription));

        group_tree[i] = (struct Group*)malloc(sizeof(struct Group));
        group_tree[i]->gId = -1;//For checking if it is the first node
        group_tree[i]->gr = (struct Info*)malloc(sizeof(struct Info));
        group_tree[i]->gr->iId = -1;
        group_tree[i]->gsub = (struct Subscription*)malloc(sizeof(struct Subscription));
    }
    hashTableSize = m;
    universalHashingNumber = p;
    a = (rand()%(p))+1;
    b = (rand()%(p));
    
    for(i=0;i<m;i++){
        sub_tree[i] = (struct SubInfo*)malloc(sizeof(struct SubInfo));
        sub_tree[i]->snext = NULL;
    }
    
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

int print_groups(int i,struct Info* info){
    if(info == group_sentinel[i]->gr){
        return;
    }
    print_groups(i,info->ilc);
    printf("%d,",info->iId);
    print_groups(i,info->irc);  
    return 1;
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
    struct Info* new_info;
    struct Info* g;
    struct Info* pg = NULL;
    int gcount = 0;//gId
    int counter=0;//For selecting the correct Group
    if(iId < 1)EXIT_FAILURE;
    while(gcount < size_of_gids_arr - 1){
        /*Setting the Group*/
        counter = gids_arr[gcount];//The number of Group
       // group_tree[counter]->gId = gids_arr[gcount];//We set the group Id
        /*Setting the Tree for the first node*/
        if(group_tree[counter]->gId == -1){
            group_tree[counter]->gId = gids_arr[gcount];//We set the group Id
            group_tree[counter]->gr->iId = iId;
            group_tree[counter]->gr->itm = iTM;
            group_tree[counter]->gr->ilc = group_sentinel[counter]->gr;
            group_tree[counter]->gr->irc = group_sentinel[counter]->gr;
            group_tree[counter]->gr->ip = NULL;
        }else{
            g = group_tree[counter]->gr;
            while(g != group_sentinel[counter]->gr){
                if(g->iId == iId){
                    printf("Fail\n");
                    return 0;
                }
                pg = g;
                if(iId < g->iId){//mporei na allajei se iTM < g->itm
                    g = g->ilc;
                }else{
                    g = g->irc;
                }
            }
            /*New node to insert to the correct Group*/
            new_info = (struct Info*)malloc(sizeof(struct Info));
            new_info->iId = iId;
            new_info->itm = iTM;
            new_info->ip = pg;
            new_info->ilc = group_sentinel[counter]->gr;
            new_info->irc = group_sentinel[counter]->gr;
            if(iId < pg->iId){//mporei na allajei se iTM < pg->itm
                pg->ilc = new_info;
            }else{
                pg->irc = new_info;
            }
        }
        /*Printing*/
        printf("GROUPID=<%d>,INFOLIST=<",group_tree[counter]->gId);
        print_groups(counter,group_tree[counter]->gr);
        printf(">\n");
        /*Steps*/
        gcount = gcount + 1;

    }

    return EXIT_SUCCESS;
}

int hash_function(int sId){
 //   printf("%d %d %d %d\n",a, b, hashTableSize, universalHashingNumber);
    return ((a*sId + b) % universalHashingNumber) % hashTableSize; //((ax + b) mod p) mod m
}

int print_sub(int i,struct Subscription* sub){
    struct Subscription *current;
    current = group_tree[i]->gsub->snext;
    int count = 1;
    int j;
    while(current != sub){
        count = count + 1;
        if(current->snext == NULL)break;
        current = current->snext;
        if(current->snext == NULL)break;
    }
    current = group_tree[i]->gsub->snext;
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
   // printf(",%d",current->sId);    
    printf(">\n");
    
    return 1;
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
    struct Subscription *new_sub = (struct Subscription*)malloc(sizeof(struct Subscription));
    
    struct SubInfo *new_SubInfo = (struct SubInfo*)malloc(sizeof(struct SubInfo));
    struct SubInfo *temp_subInfo;

    new_sub->sId = sId;
    new_sub->snext = NULL;

    new_SubInfo->sId = sId;
    new_SubInfo->stm = sTM;
    new_SubInfo->snext = NULL;

    int index;
    int gcount = 0;//gId
    int counter=0;//For selecting the correct Group

    index = hash_function(sId);
    while(gcount < size_of_gids_arr - 1){
        /*Setting the Group*/
        counter = gids_arr[gcount];//The number of Group
        if(group_tree[counter]->gId == -1)
        group_tree[counter]->gId = gids_arr[gcount];//We set the group Id
        
        /* Creating The gsub List:*/
        struct Subscription *new_sub = (struct Subscription*)malloc(sizeof(struct Subscription));
        struct Subscription *prev;
        new_sub->sId = sId;
        new_sub->snext = NULL;
        prev = (struct Subscription*)malloc(sizeof(struct Subscription));
        new_sub->sId = sId;
        new_sub->snext = NULL;
        /*First Element*/
         if(group_tree[counter]->gsub->snext==NULL){
            group_tree[counter]->gsub->snext=new_sub;
        }else{
            struct Subscription *temp = group_tree[counter]->gsub->snext;
            if(group_tree[counter]->gsub->snext!=NULL){
                while(temp != new_sub && temp->snext != NULL){
                    prev = temp;
                    temp = temp->snext;
                    if(temp->snext == NULL)break;
                }
                prev->snext = temp;
                temp->snext = new_sub;
            }
        }
        /*Creating the SubInfo List*/
        if(sub_tree[index]->snext == NULL){
            sub_tree[index] = new_SubInfo;//SubInfo
        }else{
            /*SubInfo*/
            temp_subInfo = sub_tree[index];
            while(temp_subInfo->snext != NULL){
                temp_subInfo = temp_subInfo->snext;
            }
            sub_tree[index]->snext = new_SubInfo;
        }
        /*Steps*/
         gcount = gcount + 1;
    }
    /*In case we want to test the positioning in the hashtable remove the comments above*/
    //printf("tester: % d %d\n",index,sub_tree[index]->sId);

    /*Printing*/
    /*Printing SubscriberList*/
    printf("SUBSCRIBERLIST = <");
        struct SubInfo *current1;
        current1 = sub_tree[index];
        int j;
        for(j=0;j<MG;j++){
            if(sub_tree[j]->sId != NULL){
                printf("%d,",sub_tree[j]->sId);
            }
        }
        printf(">\n");

    gcount = 0;
    while(gcount < size_of_gids_arr - 1){
        counter = gids_arr[gcount];//The number of Group
        group_tree[counter]->gId = gids_arr[gcount];//We set the group Id
        
        /*Printing*/
        printf("GROUPID=<%d>,SUBLIST=<",group_tree[counter]->gId);
        print_sub(counter,new_sub);
         /*Steps*/
         gcount = gcount + 1;
    }
    printf(">\n");

    return EXIT_SUCCESS;
}

struct Info* inorder_successor(struct Info* head,int count){
    struct Info* q = group_tree[count]->gr;
    struct Info* successor;
    while(q->iId != -1){
        if(head->itm < q->itm){
            successor = q;
            q = q->ilc;
        }else{
            q = q->irc;
        }
    }
    return successor;
}

int Tree_delete(struct Info*g, int i,int count){
    struct Info *temp, *x;
    if(g->itm <= i && g->iId != -1){
        if(g->ilc->iId == -1 || g->irc->iId == -1){
            temp = g;
        }else{
            printf("%d\n",g->itm);
            temp = inorder_successor(g,count);
        }
        if(temp->ilc->iId != -1){
            x = temp->ilc;
        }else{
            x = temp->irc;
        }

        if(x->iId != -1){
            x->ip = temp->ip;
        }
        if(temp->ip == NULL){
            group_tree[count]->gr = x;
        }else if(temp == temp->ip->ilc){
            temp->ip->ilc = x;
        }else{
            temp->ip->irc = x;
        }

        if(temp != g){
            g->iId = temp->iId;
            g->itm = temp->itm;
        //    g->igp = temp->igp;
        }
    }
    return 1;
}

/**
 * @brief Prune Information from server and forward it to client
 *
 * @param tm Information timestamp of arrival
 * @return 0 on success
 *          1 on failure
 */
int Prune(int tm){
    int count = 0;
    int gcount = 0;
    int del = 0;
    /*
    gcount = 0;
    while(gcount < MG - 1){
    //    printf("%d eleos\n",group_tree[gcount]->gId);
        if(group_tree[gcount]->gId != -1 && group_tree[gcount]->gr->iId != -1){
            printf("GROUPID=<%d>,INFOLIST=<",group_tree[gcount]->gId);
            print_groups(gcount,group_tree[gcount]->gr);
            printf(">\n");
        }
        gcount = gcount + 1;
    }
    */

    while(count < MG - 1){
        if(group_tree[count]->gId != -1 && group_tree[count]->gr->iId != -1){
            Tree_delete(group_tree[count]->gr,tm,count);
        }
        count = count + 1;
    }
    /*Printing*/
    gcount = 0;
    count = 0;
    while(gcount < MG - 1){
        if(group_tree[gcount]->gId != -1){
            printf("GROUPID=<%d>,INFOLIST=<",group_tree[gcount]->gId);
            if(group_tree[gcount]->gr->iId != -1)
            print_groups(gcount,group_tree[gcount]->gr);
            printf(">,");
            
            printf("SUBLIST=<");
            
            struct Subscription *current;
            if(group_tree[gcount]->gsub->snext != NULL){
                current = group_tree[gcount]->gsub->snext;
            
                count = 1;
                while(current != NULL){
                    count = count + 1;
                    if(current->snext == NULL)break;
                    current = current->snext;
                    if(current->snext == NULL)break;
                }
                current = group_tree[gcount]->gsub->snext;
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
        /*Steps*/
        gcount = gcount + 1;
    }
    return EXIT_SUCCESS;
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
        temp = group_tree[count]->gsub;
        prev = group_tree[count]->gsub;
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
    struct SubInfo *temp1;
    struct SubInfo *prev1;
     /*Delete in the SubInfo List*/
     count = 0;
     while(count < MG - 1){
        if(sub_tree[count]->sId != NULL){
            temp1 = sub_tree[count];
            //printf("%d\n",temp1->stm);
            prev = sub_tree[count];
            while(temp1->sId != sId){
                if(temp1->snext == NULL)break;
                prev1 = temp1;
                temp1 = temp1->snext;
            }
            if(temp1->sId == sId){
                if(temp1->snext == NULL){
                    prev1->snext = NULL;
                    temp1->sId = NULL;
                    temp1->stm = NULL;
                    temp1 = NULL;
                }else{
                    prev1->snext = temp1->snext;
                    temp1 = NULL;
                    temp1->snext = NULL;
                }
            }
        }
        count = count + 1;
     }

    printf("SUBSCRIBERLIST = <");
    int j;
    for(j=0;j<MG;j++){
        if(sub_tree[j]->sId != NULL){
            printf("%d,",sub_tree[j]->sId);
        }
    }
    printf(">\n");
    
    /*Printing*/
    int gcount = 0;//gId-Loop
    int gcounter = 0;
    int counter = 0;//Prints
    int del = 0;
    count = 0;

    while(gcount < MG){
        if(group_tree[gcount]->gId != -1){
            printf("GROUPID=<%d>,SUBLIST=<",group_tree[gcount]->gId);
            struct Subscription *current;
            if(group_tree[gcount]->gsub->snext != NULL){
                current = group_tree[gcount]->gsub->snext;
            
                count = 1;
                while(current != NULL){
                    count = count + 1;
                    if(current->snext == NULL)break;
                    current = current->snext;
                    if(current->snext == NULL)break;
                }
                current = group_tree[gcount]->gsub->snext;
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
    int gcount;
    int count;
    int no_groups = 0;
    int no_subscribers = 0;
    /*Printing*/
    gcount = 0;
    count = 0;
    while(gcount < MG - 1){
        if(group_tree[gcount]->gId != -1){
            no_groups = no_groups + 1;
            printf("GROUPID=<%d>,INFOLIST=<",group_tree[gcount]->gId);
            if(group_tree[gcount]->gr->iId != -1)
            print_groups(gcount,group_tree[gcount]->gr);
            printf(">,");
            
            printf("SUBLIST=<");
            
            struct Subscription *current;
            if(group_tree[gcount]->gsub->snext != NULL){
                current = group_tree[gcount]->gsub->snext;
            
                count = 1;
                while(current != NULL){
                    count = count + 1;
                    if(current->snext == NULL)break;
                    current = current->snext;
                    if(current->snext == NULL)break;
                }
                current = group_tree[gcount]->gsub->snext;
                while(count != 0){
                    no_subscribers = no_subscribers + 1;
                    if(count == 1){
                        printf("%d",current->sId);/*Not the comma*/
                    }else{
                        printf("%d,",current->sId);/*For the comma*/
                    }
                    if(current->snext == NULL)break;
                    if(current == NULL)break;
                    current = current->snext;
                    count = count - 1;
                }
            }
            printf(">\n");
        }
        /*Steps*/
        gcount = gcount + 1;
    }

    printf("SUBSCRIBERLIST = <");
    int j;
    for(j=0;j<MG;j++){
        if(sub_tree[j]->sId != NULL){
            printf("%d,",sub_tree[j]->sId);
        }
    }
    printf(">\n");

    printf("NO_GROUPS=<%d>,NO_SUBSCRIBERS=<%d>\n",no_groups,no_subscribers);

    return EXIT_SUCCESS;
}
