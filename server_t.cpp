/*
** server.c -- a stream socket server demo
*/
#include<stdio.h>
#include<sstream>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<sys/wait.h>
#include<signal.h>
#include<string>
#include<iostream>
using namespace std;

#define PORT "3490" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold
#define MAXDATASIZE 100

void sigchld_handler(int s)
{
 while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
 if (sa->sa_family == AF_INET)
 {
  return &(((struct sockaddr_in*)sa)->sin_addr);
 }
 return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(void)
{
 int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
 struct addrinfo hints, *servinfo, *p;
 struct sockaddr_storage their_addr; // connector's address information
 socklen_t sin_size;
 struct sigaction sa;
 int yes=1;
 char s[INET6_ADDRSTRLEN]; 

 string portreq;
 string port_number_str;
 string newport;

 int rv;
 int numbytes;
 memset(&hints, 0, sizeof hints);
 hints.ai_family = AF_UNSPEC;
 hints.ai_socktype = SOCK_STREAM;
 hints.ai_flags = AI_PASSIVE; // use my IP

 if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
 {
  fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
  return 1;
 }

 // loop through all the results and bind to the first we can
 for(p = servinfo; p != NULL; p = p->ai_next) 
 {
  if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
  {
   perror("server: socket");
   continue;
  }
 
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1)
  {
   perror("setsockopt");
   exit(1);
  }
 
  if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
  {
   close(sockfd);
   perror("server: bind");
   continue;
  }
  break;
  }

  if (p == NULL)
  {
   fprintf(stderr, "server: failed to bind\n");
   return 2;
  }
  
  freeaddrinfo(servinfo); // all done with this structure
  if (listen(sockfd, BACKLOG) == -1)
  {
   perror("listen");
   exit(1);
  }

  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  if (sigaction(SIGCHLD, &sa, NULL) == -1)
  {
   perror("sigaction");
   exit(1);
  }

  printf("server: waiting for connections...\n");
  cout<<endl;
  while(1)
  { // main accept() loop
   sin_size = sizeof their_addr;
   new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
   if (new_fd == -1)
   {
    perror("accept");
    continue; 
   }
   inet_ntop(their_addr.ss_family,
   get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);

   printf("server: got connection from %s\n", s);
   cout<<endl<<endl;
   if (!fork()) 
   { // this is the child process
     //close(sockfd); // child doesn't need the listener
    
     /*if (send(new_fd, "Hello, message format: AOSR operation operand1 operand2", 55, 0) == -1)
     perror("send");*/

     char buf[32];
     if ((numbytes = recv(new_fd, buf, 32, 0)) == -1) 
     {
      perror("recv"); exit(0);
     }

     //buf[numbytes] = '\0';
     cout<<"Server received: "<<buf<<endl<<endl; 

     string temp,op,op1,op2;
     int i=0;
     string recvd=buf;

     //store the d=buffer data in indivisual strings
     while(recvd[i]!=' ')
     {
      temp=temp+recvd[i];
      i++;
     }
     //cout<<temp<<endl;

     //error message
     if (temp!="AOSR")
     {
      if (send(new_fd, "ERROR UNRECOGMESSADE", 20, 0) == -1)
      perror("send");
      cout<<"error code sent successfully"<<endl;
      break;
     }

     i = i+1;

     while(recvd[i]!=' ')
     {
      op=op+recvd[i];
      i++ ;
     }
     //cout<<op<<endl;
     //error message
     if (op!="+")
      { if(op!="-")
        { if(op!="*")
          {if(op!="/"){
            if (send(new_fd, "ERROR INCVOPERATION", 20, 0) == -1)
             perror("send");
             cout<<"error code sent successfully"<<endl;
             break;
        }}}
      }

     i = i+1;

     while(recvd[i]!=' ')
     {
       op1=op1+recvd[i];
       i++;
     }
     //cout<<op1<<endl;

    i=i+1;

    op2=op2+recvd[i];
    //cout<<op2<<endl;

    //convert string to integer and compute result
    int OP1;
    int OP2;
    float result;
    stringstream(op1)>>OP1;
    stringstream(op2)>>OP2;

    if(op=="+")
    {result=OP1+OP2;
    cout<<"result = "<<result<<endl;
    }
    else if(op=="-")
    {
    result=OP1-OP2; 
    cout<<"result = "<<result<<endl;}
    else if(op=="*")
    {result=OP1*OP2;
    cout<<"result = "<<result<<endl;}
    else if(op=="/")
    {
     result=OP1/OP2;
     cout<<"result = "<<result<<endl;
    }

    // convert result to string
    string Result;
    ostringstream convert;
    convert<<result;
    Result=convert.str();


    //change port request
    cout<<"Do you want to change the port? (y/n):   ";
    cin>>portreq;
    cout<<endl;
    if(portreq=="n")
    {
      //send the result to client
      string buf1="ACR " + Result + "\r";
      if (send(new_fd, buf1.c_str(), 10, 0) == -1)
       perror("send");
       cout<<"result sent successfully"<<endl;
    }

    else 
    cout<<"enter the port number you want to switch to :";
    cin>>port_number_str;
    string newport="CPN " + port_number_str + "\r";
    if (send(new_fd,newport.c_str(), 10, 0) == -1)
    perror("send");
    cout<<"port change request sent:"<<newport<<endl;
    
    char buf4[32];
     if ((numbytes = recv(new_fd, buf4, 32, 0)) == -1) 
     {
      perror("recv"); exit(0);
     }
    cout<<"server received CPN acknowledgement : "<<buf4;

}
 
//close(new_fd); // parent doesn't need this

 


}







close(sockfd);
return 0;
}


//This is on the server side. 

void change_the_port(string& port_number_str)
{ 
      int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
      struct addrinfo hints, *servinfo, *p;
      struct sockaddr_storage their_addr; // connector's address information
      socklen_t sin_size;
      struct sigaction sa;
      int yes=1;
     char s[INET6_ADDRSTRLEN]; 

     string portreq;
     
     string newport;

      int rv;
      int numbytes1;

      //change of port number to notified port number??
      //1.create structs using get_addrinfo() system call??
      cout<<"Server: changing to the new port number "<<port_number_str<<endl;
      if ((rv = getaddrinfo(NULL, port_number_str.c_str(), &hints, &servinfo)) != 0) 
      {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      //return 1;
      }
      //2.create new socket with new port num using those structs??
      for(p = servinfo; p != NULL; p = p->ai_next) 
       {
       if ((sockfd = socket(p->ai_family, p->ai_socktype,  //same sock_fd as before but it got new fd
       p->ai_protocol)) == -1) {
       perror("server: socket");
       continue;
       }
       if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
       sizeof(int)) == -1) {
       perror("setsockopt");
       exit(1);
       }
      //3.bind the socket to new port number
       if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
       close(sockfd);
       perror("server: bind");
       continue;
       }
       break;
       }
      //listen for new conns
       freeaddrinfo(servinfo);
       if (p == NULL) 
       {
       fprintf(stderr, "server: failed to bind\n");
       //return 2;
       }
       else
       cout<<"Server: succesfully binded to the port on:"<<port_number_str<<endl; 

       if (listen(sockfd, BACKLOG) == -1) 
       {
       perror("listen");
       exit(1);
       }

       sin_size = sizeof their_addr;
       new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size); //new_fd got over written

       if (new_fd == -1) 
       {
       perror("accept");
       }
       else
       {
       inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), 
       s, sizeof s);

     printf("server: client at %s is now connected", s);
     cout<<"on the new port number:"<<port_number_str<<endl;
////////////////////////////
     if (!fork()) 
       { // this is the child process
       //close(sockfd); // child doesn't need the listener
 string result_string;
       if ((numbytes1 = send(new_fd, result_string.c_str(), result_string.size(), 0)) == -1) 
      perror("server: Error while sending the result");
      else{
      //cout<<"Server: sent the result "<<result_string; 
      printf("Server: sent the result to the client at %s\n\n",  inet_ntop(their_addr.ss_family,
      get_in_addr((struct sockaddr *)&their_addr),
      s, sizeof s) );}
      }
      cout<<"Server: The server is now listening on port number:"<<port_number_str<<endl;
    }
      //return 1;
}
