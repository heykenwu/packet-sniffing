#ifndef CONFIG_C
#define CONFIG_C

/*encryption*/
#define CMD_LEN  1024
#define ECP_LEN  512
#define BACKDOOR_HEADER_KEY "noob"
#define BACKDOOR_HEADER_LEN 4
#define PASSWORD "password"
#define PASSLEN 8
#define COMMAND_START "start["
#define COMMAND_END "]end"

/* info of the sender or port that needed to open*/
#define BACKDOOR_PORT 383
#define CLIENT_PORT 8000
#define BACKDOOR_IP "192.168.1.71"

/*max data size of the packet received*/
#define DATA_SIZE 65535
#endif
