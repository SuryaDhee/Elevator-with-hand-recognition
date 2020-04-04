//
//  lift_helper.h
//  opencvtest2
//
//  Created by Surya Dheeshjith on 15/11/18.
//  Copyright © 2018 Surya Dheeshjith. All rights reserved.
//

#ifndef lift_helper
#define lift_helper


#include <iostream>
#include<stdlib.h>
#include <sys/select.h>

#define MAX_K 30

using namespace std;

int cur=0;


struct queue_k
{
    int q[MAX_K];
    int front;
    int rear;
    int flag;
    int empty;
};




int kbhit(void)
{
    struct timeval tv;
    fd_set read_fd;
    
    /* Do not wait at all, not even a microsecond */
    tv.tv_sec=0;
    tv.tv_usec=0;
    
    /* Must be done first to initialize read_fd */
    FD_ZERO(&read_fd);
    
    /* Makes select() ask if input is ready:
     * 0 is the file descriptor for stdin    */
    FD_SET(0,&read_fd);
    
    /* The first parameter is the number of the
     * largest file descriptor to check + 1. */
    if(select(1, &read_fd,NULL, /*No writes*/NULL, /*No exceptions*/&tv) == -1)
        return 0;  /* An error occured */
    
    /*  read_fd now holds a bit map of files that are
     * readable. We test the entry for the standard
     * input (file 0). */
    
    if(FD_ISSET(0,&read_fd))
    /* Character pending on stdin */
        return 1;
    
    /* no characters were pending */
    return 0;
}


void insert(queue_k *qu, int insertF, int up)
{
    if((qu->rear+1)==MAX_K)
    {
        cout<<"queue_k Full";
        return;
    }
    
    if(qu->front == -1)
    {
        qu->front++;
        qu->rear++;
        qu->q[qu->front] = insertF;
        qu->empty=0;
        return;
    }
    
    int temp = qu->front;
    ++qu->rear;
    
    while(temp<=qu->rear)
    {
        if(qu->q[temp] == insertF)
            return;
        if(up ==1)
        {
            if(qu->q[temp]>insertF)
            {
                for(int i = qu->rear;i>=temp;i--)
                {
                    qu->q[i] = qu->q[i-1];
                }
                qu->q[temp] = insertF;
                return;
            }
        }
        else
        {
            if(qu->q[temp]<insertF)
            {
                for(int i = qu->rear;i>=temp;i--)
                {
                    qu->q[i] = qu->q[i-1];
                }
                qu->q[temp] = insertF;
                return;
            }
        }
        temp++;
    }
    
    qu->q[qu->rear] = insertF;
}




void addPass(queue_k *uq, queue_k *dq, queue_k *tempq, int cur,int dest)
{
    
    int from=16;
    int uflag = uq->flag, dflag = dq->flag;
    
    
    while(from>15 || from<0 || dest>15 || dest<0)
    {
        cout<<"\nEnter from floor - ";
        cin>>from;
        
        //cout<<"\nEnter destination floor - ";
        //cin>>dest;
        
        if(from>15 || from<0 || dest>15 || dest<0)
            cout<<"\nInvalid floor entered. Please try again.";
    }
    
    if(uflag==0 && dflag==0)
    {
        if(cur<=from && dest<from)
        {
            insert(uq,from,1);
            insert(dq,dest,0);
            uq->flag=1;
        }
        else if(cur>=from && dest>from)
        {
            insert(dq,from,0);
            insert(uq,dest,1);
            dq->flag=1;
        }
        else if(cur<=from && dest>from)
        {
            insert(uq,from,1);
            insert(uq,dest,1);
            uq->flag=1;
        }
        else if(cur>=from && dest<from)
        {
            insert(dq, from,0);
            insert(dq, dest,0);
        }
        return;
    }
    
    
    if(dest>from)
    {
        if(uflag==1)
        {
            if(cur>=from)
            {
                insert(tempq,from,1);
                insert(tempq,dest,1);
            }
            else
            {
                insert(uq, from,1);
                insert(uq, dest,1);
            }

        }
        else
        {
            insert(uq, from,1);
            insert(uq, dest,1);
        }
    }
    
    else if(dest<from)
    {
        if(dflag==1)
        {
            if(cur<=from)
            {
                insert(tempq,from,0);
                insert(tempq,dest,0);
            }
            else
            {
                insert(dq, from,0);
                insert(dq, dest,0);
            }
        }

        else
        {
            insert(dq, from,0);
            insert(dq, dest,0);
        }
    }
    else
    {
        if(dflag==1 && cur>from)
        {
            insert(dq,from,0);
        }
        else if(uflag == 1 && cur < from)
        {
            insert(uq,from,1);
        }
        else
        {
            insert(tempq,from,uflag);
        }
    }
    
}


void delay()
{
    for(int i=0;i<20000;i++)
        for(int j=0;j<20000;j++)
        {}
    
    
}

void del(queue_k *qu, queue_k *tempq, int up)
{
    if(qu->front==qu->rear)
    {
        qu->front=-1;
        qu->rear=-1;
        qu->flag=0;
        if(tempq->front!=-1)
        {
            for(int i = tempq->front;i<=tempq->rear;i++)
            {
                insert(qu, tempq->q[i],up);
            }
            tempq->front=-1;
            tempq->rear=-1;
            tempq->empty = 1;
        }
        else
        {
            qu->empty=1;
        }
    }
    else
    {
        qu->front++;
    }
}

void lift_me(queue_k *dq,queue_k *uq,queue_k *tempq,int dest) {
    // insert code here...
  
    addPass(uq,dq,tempq,cur,dest);
    char ch='a',ch2='a';
    while(1)
    {
        // while(!kbhit())
        //{
        while(uq->flag==1||dq->flag==1)
        {
            
            while(uq->flag==1 && cur>=0 && cur<=15)
            {
                if(uq->q[uq->front] == cur)
                {
                    delay();
                    cout<<"\nLift has stopped at floor "<<cur<<endl;
                    del(uq,tempq,1);
                    delay();
                    break;
                }
                cur++;
                delay();
                cout<<endl<<"Lift at "<<cur<<endl;
                if(kbhit())
                {
                
                    return;
                    //addPass(uq,dq,tempq,cur,dest);
                }
                
                
            }
            
            if(dq->front!=-1&& uq->flag ==0)
                dq->flag=1;
            
            
            while(dq->flag==1 && cur>=0 && cur<=15)
            {
                if(dq->q[dq->front] == cur)
                {
                    delay();
                    cout<<"\nLift has stopped at floor "<<cur<<endl;
                    del(dq,tempq,0);
                    delay();
                    break;
                }
                cur--;
                cout<<endl<<"Lift at "<<cur<<endl;
                delay();
                if(kbhit())
                {
                
                    return;
                    
                    //addPass(uq,dq,tempq,cur,dest);
                    //ch='a';
                }
            }
            
            
            if(uq->front!=-1&& uq->flag ==0)
                uq->flag=1;
            
            
        }
        cout<<"Lift stationary. (P to call)";
        while(ch2!='p')
            ch2 = getchar();
        ch2 = 'a';
        //}
        return;
        /* ch=getchar();
         if(ch=='p')
         addPass(&uq,&dq,&tempq,cur);
         ch='a'; */
    }
    
    
}




#endif /* lift_helper */
