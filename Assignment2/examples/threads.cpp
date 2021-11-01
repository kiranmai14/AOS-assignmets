#include <pthread.h>
#include <iostream>
#include <unistd.h>
#define SOCKETERROR (-1)
using namespace std;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int check(int exp,const char *msg)
{
    if(exp == SOCKETERROR)
    {
        perror(msg);
        exit(1);
    }
    return exp;
}
void * myTurn(void *args)
{
    int *ptr = (int *)malloc(sizeof(int));
    *ptr = 5;
    for(int i=0;i<7;)
    {
        sleep(1);
        cout<<"My turn "<<i<<endl;
        pthread_mutex_lock(&lock);
        i++;
        pthread_mutex_unlock(&lock);
        (*ptr)++;
        
    }
    return ptr;
}
void  yourTurn()
{
    for(int i=0;i<4;i++)
    {
        sleep(2);
        cout<<"your turn "<<i<<endl;
    }
}
int main(){
    pthread_t newTh;
    int *res = 0; 
    pthread_create(&newTh,NULL,myTurn,NULL);
     yourTurn();
    pthread_join(newTh,(void **)&res);
    cout<<(*res)<<endl;
}
