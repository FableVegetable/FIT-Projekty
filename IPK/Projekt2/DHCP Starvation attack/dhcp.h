/*
 *  Author:   Tomas Zubrik, xzubri00
 *  Subject:  IPK, FIT VUT Brno 2018
 *  Project:  DHCP Starvation Attack (2st Project 2nd Variant) 
 */

/* Included standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>

/* Included headers for networking */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h>

/* DHCP Macro Constants*/
#define DHCP_CHADDR_LEN     16
#define DHCP_SNAME_LEN      64
#define DHCP_FILE_LEN       128
#define DHCP_OPT_LEN        312
#define DHCP_DISCOVER       1
#define DHCP_BROADCAST      32768
#define BOOT_REQ            1

#define ETH_IP              0x0800    //IP Protocol
#define ETH_ADD_TYPE        1     /* used in htype field of dhcp packet */
#define MAC_LEN             6 

#define ERROR               -1
#define OK                  0
#define BUFFERSIZE          1024

/* DHCP Discover Message Payload Structure */
typedef struct dhcp_packet_struct{
  uint8_t  op;                   // packet type 
  uint8_t  htype;                // type of hardware address for this machine (Ethernet, etc) 
  uint8_t  hlen;                 // length of hardware address (of this machine) 
  uint8_t  hops;                 // hops 
  uint32_t xid;                  // random transaction id number - chosen by this machine 
  uint16_t secs;                 // seconds used in timing 
  uint16_t flags;                // flags 
  struct in_addr ciaddr;          // IP address of this machine (if we already have one) 
  struct in_addr yiaddr;          //IP address of this machine (offered by the DHCP server) 
  struct in_addr siaddr;          // IP address of DHCP server 
  struct in_addr giaddr;          // IP address of DHCP relay 
  unsigned char chaddr [DHCP_CHADDR_LEN];      // hardware address of this machine 
  char sname [DHCP_SNAME_LEN];    // name of DHCP server 
  char file [DHCP_FILE_LEN];      // boot file name (used for diskless booting?) 
  char options[DHCP_OPT_LEN];  // options 
}dhcp_packet;

/* Declaration and definition of important variables used in program */
int packet_size = 0, dhcp_socket;
char buffer[BUFFERSIZE]={0};
struct ifreq if_mac;
struct ifreq if_idx;
char *interface;
unsigned char client_mac_addr[MAC_LEN];
unsigned char macff[] = {0xff,0xff,0xff,0xff,0xff,0xff};

/* Variables that represent size of used structures */
size_t eth_header_size = sizeof(struct ether_header);
size_t ip_header_size = sizeof(struct iphdr);
size_t sockaddr_ll_size = sizeof(struct sockaddr_ll);
size_t interface_size = sizeof(struct ifreq);
size_t udp_header_size = sizeof(struct udphdr);
size_t discover_msg_payload_size = sizeof(dhcp_packet);

/**
 * @brief Gets index and mac address of interface
 * @param int     dhcp_socket     Integer number that represents socket file descriptor to use.
 * @param char*   interface       Name of set inferface.
 * @note  Important function for communction between Attacker and Server.
 * @return 0 - OK, -1 - ERROR
 */
int get_interface(int dhcp_socket, char *inteface);

/**
 * @brief Checks the attackers's input arguments [-i interface]
 * @param int       argc          Arguments count
 * @param char**    argv          Array of arguments as strings
 * @param char**    interface     Name of set inferface.
 * @note  If input arguments are incorrect exits with error.
 */
void check_args(int argc, char **argv, char **interface);

/**
 * @brief Generates new MAC address for dhcp attacker.
 * @param unsigned char  *    client_mac_addr     New MAC Address to return.
 * @note  Important function to simulate different clients.
 * @return Random Mac Address.
 */
unsigned char * generate_mac_address(unsigned char *client_mac_addr);

/**
 * @brief Creates Ethernet header, fills "ether_header" structure.
 */
void create_ether_header();

/**
 * @brief Creates IP header, fills "iphdr" structure.
 * @return Pointer to "iphdr" (IP Header).
 */
struct iphdr * create_ip_header();

/**
 * @brief Creates UDP header, fills "udphdr" structure.
 * @return Pointer to "udphdr" (UDP Header).
 */
struct udphdr * create_udp_header();

/**
 * @brief Creates DHCP Discover Message payload , fills "dhcp_packet" structure.
 * @param unsigned char  *    client_mac_addr     MAC Address
 * @note  Important function to send DHCP Discover Messages.
 * @return Pointer to "dhcp_packet" (DHCP Discover Message payload).
 */
dhcp_packet * create_dhcp_msg_payload(unsigned char *client_mac_addr);

/**
 * @brief Checks sum of sent packets.
 * @param int     nwords          Number of sent packets.
 * Title:
 * Author:
 * Availability: 
 * @param char*   buf             Buffer to check.
 * @return Sum size
 */
unsigned short check_sum(unsigned short *buf, int nwords);

/**
 * @brief Prints error message on stderr and exits program with error code -1.
 * @param char*     info      Error message to be printed.
 */
void exit_with_error(char *info);

/**
 * @brief Debugging function to print mad adress.
 * @param unsigned char  *    client_mac_addr     MAC Address to print.
 */
void print_mac_address(unsigned char *client_mac_addr);