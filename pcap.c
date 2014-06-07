#include<stdio.h>
#include<pcap.h>
#include<stdlib.h>
#include<net/ethernet.h>   /* Ethernet */
#include<netinet/ip.h>	   /* Internet Protocol */
#include<netinet/tcp.h>	   /* Transmission Control Protocol */
#include<arpa/inet.h>


#define ARP_REQUEST 1
#define ARP_REPLY 2

typedef struct arphdr
{
	u_int16_t htype;
	u_int16_t ptype;
	u_char hlen;
	u_char plen;
	u_int16_t oprcode;
	u_char sha[6];
	u_char sipa[4];
	u_char tha[6];
	u_char tipa[4];
}arphdr_t;



int main(int argc,char **argv)
{
	pcap_t *handle;
	int pktCount=0,i=0;
	long int current_ts=0;
	const u_char *packet;
	struct pcap_pkthdr *header=NULL;
	struct ether_header *eptr=NULL;
	struct iphdr *ip=NULL;
	struct tcphdr *tcp=NULL;
	arphdr_t *arpheader=NULL;
	char errbuf[PCAP_ERRBUF_SIZE];

	if(argc < 2)
	{
		fprintf(stderr,"Input File name not specified");
		exit(1);
	}
    handle=pcap_open_offline(argv[1],errbuf);			/* Open file for reading */
    if(handle==NULL)
    {
    	fprintf(stderr,"Unable to open the pcap file %s:%s",argv[1],errbuf);
    	exit(1);
    }
    else
	    printf("\nPcap Opened");
    
    while((pcap_next_ex(handle,&header,&packet))>0)
    {
    	pktCount++; 	 	
    	
    	eptr = (struct ether_header *) packet;
		if (ntohs (eptr->ether_type) == ETHERTYPE_IP)  /*Check if it is an IP packet*/
		{
			printf("\n\n\nEthernet type hex:%x dec:%d is an IP packet",ntohs(eptr->ether_type),ntohs(eptr->ether_type));
	    	printf("\nEpoch Time : %ld:%ld",header->ts.tv_sec,header->ts.tv_usec);
	    	printf("\nLenght : %d",header->caplen);
	    	
	     	ip = (struct iphdr*)(packet + ETHER_HDR_LEN);     /* Point to IP header */
			if (ip->protocol == 6 ) 
			{
				tcp=(struct tcphdr *)(packet + ETHER_HDR_LEN + ip->ihl*4);       /* Point to TCP Header */
				printf("\nSource IP : %s",(inet_ntoa(*(struct in_addr *)&ip->saddr)));
				printf("\nDestination IP : %s",(inet_ntoa(*(struct in_addr *)&ip->daddr)));
				printf("\nSource Port : %d",ntohs(tcp->th_sport));
				printf("\nDestination Port : %d",ntohs(tcp->th_dport));
				printf("\nACK : %u",ntohl(tcp->th_ack));
				printf("\nSEQ : %u",ntohl(tcp->th_seq));
    		}      
		}
		else if (ntohs (eptr->ether_type) == ETHERTYPE_ARP)   /* Check if it an ARP packet */
		{
			printf("\n\n\nEthernet type hex:%x dec:%d is an ARP packet",ntohs(eptr->ether_type),ntohs(eptr->ether_type));
	    	printf("\nEpoch Time : %ld:%ld",header->ts.tv_sec,header->ts.tv_usec);
	    	printf("\nLength : %d",header->caplen);
			arpheader=(arphdr_t *)(packet + ETHER_HDR_LEN);  /* Point to ARP Header */
			printf("\nHardware Type : %s ",(ntohs(arpheader->htype)==1)? "Ethernet":"Unknown");
			printf("\nProtocol Type : %s ",(ntohs(arpheader->ptype)==0X0800)? "IPv4":"Unknown");
			printf("\nOperation : %s ",(ntohs(arpheader->oprcode)==ARP_REQUEST)? "ARP Request":"ARP Reply");
			
			/*If it is Ethernet and IPv4 then print packet contents*/
			
			if((ntohs(arpheader->htype)==1) && (ntohs(arpheader->ptype)==0X0800))
			{
				printf("\nSender MAC : ");
				for(i=0;i<6;i++)
					printf("%02X:",arpheader->sha[i]);
				printf("\nSender IP Address : ");
				for(i=0;i<4;i++)
					printf("%d.",arpheader->sipa[i]);
				printf("\nTarget MAC : ");
				for(i=0;i<6;i++)
					printf("%02X:",arpheader->tha[i]);
				printf("\nTarget IP Address : ");				
				for(i=0;i<4;i++)
					printf("%d.",arpheader->tipa[i]);
			}
			
		}
		else 
		{
			printf("Ethernet type %x not IP", ntohs(eptr->ether_type));
			exit(1);
		}
    }
    
    printf("\nDone Processing %d packets",pktCount);
    pcap_close(handle);
    return 0;
}

