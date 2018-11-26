/*
Class CS-656 - Internet and higher language protocol
Group - M4.
Project 2: UDP radio transmitter.

File name: udp.c
Compile: gcc udp.c -o udp
Running: ./udp <VLC_Host_ID> <Port> <File Name> <Buffer Size> <Packet Size> <Transmission Delay>
<VLC_Host_ID> : IP of the host on which the VLC is running.
<PORT>: The port on which the UDP server is running.
<File Name>: The audio file that you want to stream.
<Buffer Size>: The buffer size = maximum size packet that you want to read from the audio file.
<Packet Size>: The packet Size = packet size to be read from the file. Must be lesser than the buffer size.
<Transmission Delay>: Delay in transmission. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

void error(char *err_msg){
  perror(err_msg);
  exit(0);
}

int val_pack(int pack_size){
  int resp;
  printf("PacketSize larger than the UDP payload... \n");
  printf("\n\nDo you wish to continue??\nPress '1' to continue and '0' to terminate...\n");
  printf("If you wish to continue, packet size will be explicitly set to 1500\n");
  scanf("%d",&resp);
  printf("User decides to continue!!\n");
  if(resp == 1){
      pack_size = 1500;
      return pack_size;
  }else{
    error("Packet Size exceeds UDP payload Error!");
  }
}

int val_buff(){
  int resp =0;
  printf("\n\nALERT!! Buffer Size smaller compared to the Packet Size.\n");
  printf("Expected running method:  ./M4_udp_radiostream <VLC_Host_ID> <Port> <File Name> <Buffer Size> <Packet Size> <Transmission Delay>\n\n");
  printf("\nDo you still want to continue?!!\nEnter '1' to continue, Press '0' to terminate the transmitter!\n");
  printf("If you still wish to continue, buffer size and the packet size will be explicitly set to 1500\n");
  scanf("%d",&resp);
  if(resp == 1){
     return 1500;
  }else{
    error("Buffer & Packet Size MisMatch Error!");
  }
}

void partition(char *part_msg){
  printf("\n==================================%s==================================\n",part_msg);
}
int main(int argc, char *argv[]){
  printf("\nIHLP CS 656: Project 2: UDP Radio Streaming\nProf Kumar Mani\nGroup: M4\nMail: yss22@gmail.com\n\n");
  int server_sock, fp, port,  data_sent, dtrans,count=0, data_read;
  char *filename, *vlcID;
  struct sockaddr_in server;
  int pkt_size, buff_size;
  if(argc != 7){
    printf("\nExpected running method:  ./udp <VLC_Host_ID> <Port> <File Name> <Buffer Size> <Packet Size> <Transmission Delay>");
    printf("\nVLC Host ID = IP address of the host running VLC\nPort = Port on which Host system is listening!\nFile Name= Name of theaudio file to be streamed");
    printf("\nBuffer Size = Size of buffer holding the data from file\nPacket Size= Size of packetizing the audio file.\nTransmission Delay= dtrans");
    error("\nInvalid Argument Number");
  }

  vlcID = argv[1];
  port = atoi(argv[2]);
  filename = argv[3];
  buff_size = atoi(argv[4]);
  pkt_size = atoi(argv[5]);
  partition("Validating packet size!");
  if(pkt_size <= 0){
    printf("Seems like the packet size is not valid. TO get a valid output, please enter a significant positive value for packet size in bytes\n");
    error("Invalid packet size Error: ");
  }
  if((pkt_size > 65507)){
    pkt_size = val_pack(pkt_size);
  }

  partition("Validating buffer size!");
  if(buff_size <= 0){
    printf("Seems like the buffer size is not valid, To get a valid output, please enter a significant positive value for buffer size in bytes\n");
    error("Invalid Buffer Size Error!");
  }
  if((buff_size < pkt_size)) {
    buff_size = pkt_size = val_buff();
  }
  dtrans = atoi(argv[6]);
  partition("Validating Delay");
  if(dtrans < 0){
    printf("Seems like the delay value is not valid, To get a valud output, please enter significant positive value for delay in milliseconds.\n");
    error("Negative delay error!");
  }
  char buff[buff_size];

  printf("\nPacket Size: %d \nTransmission Delay: %d \nFile Name: %s", pkt_size, dtrans, argv[1]);
  if((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) <0){
    error("\nSocket Error");
  }

  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = inet_addr(vlcID);

  if((fp = open(filename, O_RDONLY)) < 0){
    error("\nFile did not open");
    }

    while(data_read = read(fp, buff, pkt_size)){
    printf("\nData read is: %d bytes", data_read);
    if((data_sent = sendto(server_sock, buff,data_read,0, (struct sockaddr*)&server, sizeof(server)))== -1){
      error("Data sending error");
    }
    printf("\nData sent is: %d bytes\n", data_sent);
    usleep(1000 * dtrans);
    bzero(buff, sizeof(buff));
  }
  close(server_sock);
  return 0;
}
