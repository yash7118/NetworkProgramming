/*

Subject to code: TO develop a blocking proxy program, considering the requirements and constraints, as posted on NJIT moodle by prof. Kumar Mani. 
Course: Internet and Higher Level Protocols, CS 656.
Code submitted by: Group M4.
Group Member: Hirdesh, Ritesh, Swapnil, Neil, Aashish, Yash. 
Code Version: 1.6

Working and testing of the code:
The code has to be  compiles and runs on NJIT AFS. 

SOP:
Navigate to the directory in which the code is saved. 
Open the Terminal and compile using:
gcc proxy_by_M4.c -o proxy_by_M4

If the integrity of the code is intact, it should ideally compile without any errors or warnings. 
Proceed to run the code: ./proxy_by_M4 10500
Note: "10500" is the port number on which you want the program to execute. 

After running the programs:

Test case 1:
Try opening multiple instances of following website:
http://gaia.cs.umass.edu/wireshark-labs/HTTP-wireshark-file1.html
Note: Above mentioned website contains only the text, and opens succesfully. 

Test case 2:\
Try opening single instance of following website:
http://gaia.cs.umass.edu/wireshark-labs/HTTP-wireshark-file1.html
Note: The core shall be dumped as in, the images are still not retrived. 

Test case 3:
Try Opening the following website:
http://www.cricinfo.com/
This particular website contains redirection, and we are succesfully able to redirect it to:
http://www.espncricinfo.com
Note: It will display "transferring the data from espsncricinfo.com", but since images are still not retrived so goes into infinite loop. 

Test case 4: 
Play test case 1,2 and 3 together.

*/
#include <stdio.h> 
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> 
#include <arpa/inet.h>   
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#define TRUE   1 
#define FALSE  0 
#define size32 32
#define size256 256
#define size1024 1024
#define size2048 2048
#define size4096 4096
#define size10800 10800


struct Data{
	char req[size1024];
	char domain[size1024];
	char path[size1024];
}d;	

struct Data getHost(char req_in[size4096]) {
	strcpy(d.req, req_in);
	int i, j=0,m=0,k; 
	char cmp1[size32]="Host:";
	char cmp2[size32]=".jpg";
	char stop1[size32] = {"\r"};
	char stop2[size32] = {" "};
	char stop3[size32] = {"/"};
	char *p,*q, temp[size256], temp2[size256];
  	for(i=0; i<=size1024; i++){
    		d.domain[i] = 0;
    		d.path[i] =0;
  	}
   	for(i=0; i<=size256; i++){
    		temp[i] = 0;
    		temp2[i] =0;
  	}
    	

    	if(strstr(d.req,cmp1)){
	  	p=strstr(d.req ,cmp1);
		
		  for(i=6; p[i]!=strlen(p); i++) {
        		temp[j] = p[i];
			      j++;
        		if(p[i]==stop1[0]){
            			break;
        		}
    		}
		
      for (i=0; i<(strlen(temp)-1); i++){
    			d.domain[i] = temp[i];
    		}
		printf("\n");	
		puts(d.domain);
		printf("%lu", strlen(d.domain));
	}


	else if(strstr(req_in,"http")){
				p=strstr(req_in,"http");
				for(i=7;i<strlen(p);i++){
					temp[j]=p[i];
					j++;
					if(p[i]==stop3[0]){
						break;
					}
				}
			for(k=0; k<(strlen(temp)-1); k++){
    			d.domain[k] = temp[k];
    			}

			for(;;i++){
				temp2[m]=p[i];
				m++;
				if(p[i]==stop2[0]){
					break;
					}
			}
			
			for(k=0; k<(strlen(temp2)-1); k++){
    				d.path[k] = temp2[k];
				}
			
			printf("\n");	
			puts(d.domain);
			puts(d.path);
			printf("%lu", strlen(d.domain));
			printf("%lu", strlen(d.path));				
			}

	
			if(strstr(req_in,"Referer:")){
				q=strstr(req_in,"GET /");
				j=0;				
				for(i=4;i<strlen(q);i++){
					temp2[j]=q[i];
					j++;
					if(q[i]==stop2[0]){
						break;
					}
				}
				for(i=0; i<(strlen(temp2)-1); i++){
    					d.path[i] = temp2[i];
    				}							
				printf("\n");	
				puts(d.path);
				printf("%lu", strlen(d.path));
				printf("\n");					
		}
  
	return d;
}


//Function to induce DNS Lookup
char * dnsLOOKup(char *hostname){
	
	//Uses addrinfo structure. 
	int sockfd;  
	struct addrinfo hints, *results, *p;
	struct sockaddr_in *ip_access;	
	int rv;
	char *ipv4;	

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; 
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname, "domain", &hints, &results)) != 0) {
    		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    		exit(1);
	}
	//Taking the first IP address
	p = results;
	ip_access = (struct sockaddr_in*) p -> ai_addr;
	ipv4 = inet_ntoa(ip_access -> sin_addr);
	printf("IP address for domain %s is: %s\n",hostname, ipv4);
	printf("DNS LOOKUP COMPLETED\n");
	printf("*********************************************\n");
	freeaddrinfo(results); // all done with this structure
	return ipv4;
	
}

char * prxClient(char addr_in[]){
	char *address, *host_addr;
	char response[10800];
 	char * resp_out = response;
	int prx_cli;
	
	//Parsing the request and get the host name.
	//Basically this parses the code, to get the domain name	
	getHost(addr_in);
	//host_addr contains the host name. 

	//Beginning the DNS-LookUP code for hostname. 
	address =  dnsLOOKup(d.domain);
	// DNS lookup Completed. 

	//Starting with the in_proxy_CLIENT code:
	if((prx_cli = socket(AF_INET, SOCK_STREAM, 0)) == -1){
	perror("Client Socket: ");
	exit(0);	
	}
	
	printf(" %lu ", sizeof(address));

	struct sockaddr_in remote_address;
	remote_address.sin_family = AF_INET;
	remote_address.sin_port = htons(80);
	inet_aton(address, (struct in_addr *)&(remote_address.sin_addr.s_addr));
	
	//Connecting the server. TCP connection establishes. 
	connect(prx_cli, (struct sockaddr *)&remote_address, sizeof(remote_address));
	
	//Sending the HTTP Request to the Web-Server. 
	send(prx_cli, d.req, sizeof(d.req), 0);
	printf("Request sent to server: %s\n", d.req);
	
	//Recieving the Response from the Web-Server. 
	recv(prx_cli, &response, sizeof(response), 0);

	printf("response froms server: %s\n", response);
	close(prx_cli);		
	return resp_out;
}


    
int main(int argc , char *argv[])  
{  	
	int on = TRUE;
    	int port = atoi(argv[1]);	  
    	int master_socket , addrlen , new_socket , client_socket[size32],max_cli= size32 , activity, i , valread , sd;  
    	int max_sd, fimg; 
    	char act_resp[10800];
    	struct sockaddr_in address;
	
	//This buffer contains the request from the client. 
     	char buffer[1025];  //data buffer of 1K 
	
	//This is temporary code, for image retrival. Yet to be achieved. 			  
   	char path1[1024] = "GET /images"; 
    	char image_header[2048] = " HTTP/1.1 200 OK\r\n\n" "Content-Type:image/jpg\r\n\r\n";\
    	
        printf("Fall-2017/CS-656 Project\n Prof: Kumar Mani\n Project 1: HTTP_Blocking_Proxy \n  Group: M4.\n Mail: yss22@njit.edu \n\n\n");

    	//set of socket descriptors 
    	fd_set readfds;  
        
    	//just a ping.
    	char *message = "Connection to Proxy_by_GroupM4 Iniatiated.\r\n";  
    
	// There are no clients connected in the beginning, so. 
    	//initialise all client_socket[] to 0. 
    	for (i = 0; i < max_cli; i++){  
        	client_socket[i] = 0;  
    	}  
        
    	//create a master socket 
    	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  {  
        perror("socket failed");  
        exit(0);  
    	}  
    
    	//set master socket to allow multiple connections.It shall act as single point to monitor the change in status of client. 
   	 if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0 )  
    	{  
        perror("setsockopt");  
        exit(0);  
   	 }  
    
    	//inisitializing the socket.
    	address.sin_family = AF_INET;  
    	address.sin_addr.s_addr = INADDR_ANY;  
    	address.sin_port = htons(port);  
        
    	//binding the socket, with the parameters mentioned above.  
    	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  {  
        perror("bind failed");  
        exit(0);  
    	}  
    	
	//Verifying the port server is running on. 
	printf("Listener on port %d \n", port);  
        
    	//To ensure maximum of 5 backLOG connections for the master socket 
    	if (listen(master_socket, 5) < 0){  
        perror("listen");  
        exit(0);  
    	}  
        
    	//accept the incoming connection 
    	addrlen = sizeof(address);  
    	puts("Waiting for connections ...");  
        
    	while(TRUE){
	  
        //Clear the socket set 
        FD_ZERO(&readfds);  
    
        //add master socket to set 
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket;  
            
        //add child sockets to set 
        for ( i = 0 ; i < max_cli ; i++){  
            //socket descriptor 
            sd = client_socket[i];  
                
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
    
        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
        if ((activity < 0) && (errno!=EINTR)){  
            printf("select error");  
        }  
            
        //If any activity happends on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds)){  
            if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0){  
                perror("accept");  
            }  
            
         //Log socket number of proxy - used in send and receive commands 
         printf("New connection is established, socket_fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
                
         //add new socket to array of sockets 
          for (i = 0; i < max_cli; i++)  {  
               //if position is empty 
                if( client_socket[i] == 0 ){  
                    client_socket[i] = new_socket;  
                    printf("Adding to list of sockets as %d\n" , i);    
                    break;  
                }  
            }  
        }  
            
        //else its some IO operation on some other socket
        for (i = 0; i < max_cli; i++){  
            sd = client_socket[i];  		
	    if (FD_ISSET( sd , &readfds)){  

                //Check if it was for closing , and also read the 
                //incoming message 
                if ((valread = read( sd , buffer, 1024)) == 0){  

                    //Somebody disconnected , get his details and print 
                    getpeername(sd , (struct sockaddr*)&address ,(socklen_t*)&addrlen);  
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                        
                    //Close the socket and mark as 0 in list for reuse 
                   close( sd );  
                   client_socket[i] = 0;  
                   
                }else{
  		     //Serve the request. 
                    //set the string terminating NULL byte on the end 
                    //of the data read 
                    buffer[valread] = '\0';  
                     
		    //Processing the request, parsing the hostname, Looking up DNS and getting the response to act_resp. 		
                    strcpy(act_resp,(prxClient(buffer)));          
                    if(strcmp(buffer, "GET / HTTP/1.1\r\n")){
			//Sending the HTML file back. 
                 	write(sd, act_resp, strlen(act_resp));
		     }
			//Following code has been written for image retirval, but does not give any effective results as of now. 
			else if(strcmp(d.req,path1)){
				fimg = open(d.path,O_RDONLY);
                  		write(sd, act_resp, strlen(image_header));
                        	sendfile(sd, fimg, NULL,sizeof fimg);
                       		close(fimg);
                      	}else{
			 	fimg = open(d.path,O_RDONLY);
	                 	write(sd, image_header, strlen(image_header));
			 	sendfile(sd, fimg, NULL ,sizeof fimg);
			 	close(fimg);
		        }
			//memset(d.req, 0, sizeof(d.req));	 
                        continue;
                }  
            }  
        }  
    }  
        
    return 0;  
}  
