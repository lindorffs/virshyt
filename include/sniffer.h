#ifndef SNIFFER_H
#define SNIFFER_H

#include <pcap.h>

#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/icmp.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

char* getHostname(const char* packet) {
	struct ethhdr *eth = (struct ethhdr*)packet;
	struct iphdr *ip = (struct iphdr*)(packet+sizeof(struct ethhdr));
	struct sockaddr_in sin;

	char host[1024];
	char service[20];

	struct in_addr source;
	source.s_addr = ip->saddr;

	sin.sin_family = AF_INET;
	sin.sin_port = 0;
	sin.sin_addr = source;

	memset(sin.sin_zero, 0, sizeof(sin.sin_zero));

	getnameinfo(&sin, sizeof(sin), host, sizeof(host), service, sizeof(service), 0);
	return(host);
}

int isFromHost(const char* packet, char* host) {
	unsigned short iphdrlen;

	struct ethhdr *eth = (struct ethhdr*)packet;
	struct iphdr *ip = (struct iphdr*)(packet+sizeof(struct ethhdr));
	iphdrlen = ip->ihl * 4;

	struct in_addr source, dest;
	source.s_addr = ip->saddr;
	dest.s_addr = ip->daddr;
	char *disp = inet_ntoa(source);

	printf(getHostname(packet));

	return (strcmp(disp,host) != 0 ? 0 : 1);
}

void handlePack(const char* packet, int size) {
	struct ethhdr *eth = (struct ethhdr*)packet;
	struct iphdr *ip = (struct iphdr*)(packet + sizeof(struct ethhdr));
	unsigned short iphdrlen;
	iphdrlen = ip->ihl * 4;

	struct icmphdr *icmp = (struct icmphdr*)(packet + sizeof(struct ethhdr) + iphdrlen);
	printf("Type: %d\n",(unsigned int)(icmp->type));
	int header_size = sizeof(struct ethhdr) + iphdrlen + sizeof(icmp);
}

const char* sniffPack(char* dev) {
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *handle;
	char filter[] = "icmp";
	struct bpf_program fp;
	bpf_u_int32 mask;
	bpf_u_int32 net;
	const char *packet;
	struct pcap_pkthdr header;

	handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
	if (handle == NULL)
		return NULL;
	if (pcap_datalink(handle) != DLT_EN10MB)
		return NULL;
	if (pcap_compile(handle, &fp, filter, 0, net) == -1)
		return NULL;
	if (pcap_setfilter(handle, &fp) == -1)
		return NULL;
	
	printf("waiting for packet on %s\n",dev);
	packet = pcap_next(handle, &header);
	pcap_close(handle);
	return packet;
}

#endif

