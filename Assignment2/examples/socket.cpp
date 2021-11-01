#include <iostream>
#include <sys/socket.h> /*basic socket definitions*/
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h> /* for variadic functions */
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <netdb.h>


#define SERVER_PORT 80  //for http
#define MAX_LINE 4096    //buffer where the data goes
#define SA struct sockaddr 
using namespace std;

void err_n_die(const char*fmt,...);
int main(int argc,char **argv)
{

    int sockfd,n;
    int sendbytes;
    struct sockaddr_in servaddr;
    char sendline[MAX_LINE];
    char recvline[MAX_LINE];
    if(argc!=2)
        err_n_die("usage: %s <serveraddress>",argv[0]);
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        err_n_die("Error while creating the socket!");

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);

    // string representation of ip to binary (1.2.3.4 to [1,2,3,4])
    if(inet_pton(AF_INET,argv[1],&servaddr.sin_addr)<= 0)
        err_n_die("iner_pton error for %s",argv[1]);
    

    if(connect(sockfd,(SA *)&servaddr ,sizeof(servaddr)) < 0)
        err_n_die("connect failed");

    // we are connected prepare the message
    sprintf(sendline,"GET / HTTP/1.1\r\n\r\n");
    sendbytes = strlen(sendline);


    // send the request -- making sure you send it all
    // This code is a bit fragile since bails if only some of the bytes are sent
    // normally you want to retry unless the return value is -1

    if(write(sockfd,sendline,sendbytes)!= sendbytes)
        err_n_die("write error");
    
    while((n = read(sockfd,recvline,MAX_LINE-1) )>0 )
    {
        memset(recvline,0,MAX_LINE);
        printf("%s",recvline);
    }

    if(n < 0 )
        err_n_die("read error");


    exit(0);  //end successfully
}
void err_n_die(const char*fmt,...)
{
    int errno_save;
    va_list ap;


    // any system or library call can set errno,so we need to save it now
    errno_save = errno;

    // print out the fmt+args to standard output
    va_start(ap,fmt);
    vfprintf(stdout,fmt,ap);
    fprintf(stdout,"\n");
    fflush(stdout);

    // printout error message is errno was set
    if(errno_save !=0)
    {
        fprintf(stdout,"(errnp = %d) : %s\n",errno_save,strerror(errno_save));
        fprintf(stdout,"\n");
        fflush(stdout);
    }

    va_end(ap);

    // this is the ...and_die part,Terminate with an error
    exit(1);
}


