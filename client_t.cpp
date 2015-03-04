        #include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string>
#include<iostream>
#include<cstdlib>
using namespace std;
using std::string;


#define PORT "3490" // the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once



// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
 if (sa->sa_family == AF_INET) {
 return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
 int sockfd, numbytes;
 char buf[MAXDATASIZE];
 struct addrinfo hints, *servinfo, *p;
 int rv;
 char s[INET6_ADDRSTRLEN];
 string buf1; 

 if (argc != 2)
 {
  fprintf(stderr,"usage: client hostname\n");
  exit(1);
 }


 memset(&hints, 0, sizeof hints);
 hints.ai_family = AF_UNSPEC;
 hints.ai_socktype = SOCK_STREAM;
 if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
 {
  fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
  return 1;
 }



 // loop through all the results and connect to the first we can
 for(p = servinfo; p != NULL; p = p->ai_next)
 {
  if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
  {
   perror("client: socket");
   continue;
  }
  if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
  {
   close(sockfd);
   perror("client: connect");
   continue; 
  }
  break;
 }
 if (p == NULL)
 {
  fprintf(stderr, "client: failed to connect\n");
  return 2;
 }
 inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
 printf("client: connecting to %s\n", s);
 freeaddrinfo(servinfo); // all done with this structure
 if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
 {
  perror("recv");
  exit(1);
 }
 buf[numbytes] = '\0';
 printf("client: received '%s'\n",buf);

 string op1, op2;
 string op;
 printf("Please enter the operation you want to perform (ex: +,-,/,*) : ");
 cin>>op;
 printf("Enter the value or operand-1 : ");
 cin>>op1;
 printf("Enter the value of operand-2 : ");
 cin>>op2;
 cout<<"the operands entered are "<<op1<<" & "<<op2<<endl;

 buf1 = "ASOR " + op + " " + op1 + " " + op2; 
//send the operation to server
 if (send(sockfd, buf1.c_str(), 32, 0) == -1)
 perror("client: send");
 cout<<"client: sent "<<buf1<<endl;

 //receive result and displayed

 char buf2[32];
 if ((numbytes = recv(sockfd, buf2, 32, 0)) == -1) 
 {
  perror("recv"); exit(0);
 }
 cout<<"Port change request received : "<<buf2<<endl;
 
  
 
 string recc=buf2;
 string temp;
 string prt;
 int i=0;

 while(recc[i]!=' ')
  {
     temp=temp+recc[i];
     i++;
  }
  cout<<temp;

  i=i+1;

 while(recc[i]!=' ')
 {
  prt=prt+recc[i];
 }
 
  cout<<prt<<endl;



if (temp=="ACR")
 {
  cout<<buf2<<endl;
 }
 else
 cout<<"Client is changing the port number to ";
 
 

 //connecting to the new port
 if ((rv = getaddrinfo(argv[1], prt.c_str(), &hints, &servinfo)) != 0)
 {
  fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
  return 1;
 }



 // loop through all the results and connect to the first we can
 for(p = servinfo; p != NULL; p = p->ai_next)
 {
  if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
  {
   perror("client: socket");
   continue;
  }
  if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
  {
   close(sockfd);
   perror("client: connect");
   continue; 
  }
  break;
 }
 if (p == NULL)
 {
  fprintf(stderr, "client: failed to connect\n");
  return 2;
 }
 inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
 printf("client: connecting to %s\n", s);
 freeaddrinfo(servinfo); // all done with this structure
 
 //CPN acknowledgment sent
 string buf3="CPN_ACK 1";
 if (send(sockfd, buf3.c_str(), 32, 0) == -1)
 perror("client: send");
 cout<<"client: sent "<<buf3<<endl; 



close(sockfd);
return 0;
}



