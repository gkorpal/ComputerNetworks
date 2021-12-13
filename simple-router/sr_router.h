/*-----------------------------------------------------------------------------
 * File: sr_router.h
 * Date: ?
 * Authors: Guido Apenzeller, Martin Casado, Virkam V.
 * Contact: casado@stanford.edu
 * 90904102 
 *---------------------------------------------------------------------------*/

#ifndef SR_ROUTER_H
#define SR_ROUTER_H

#include <netinet/in.h>
#include <sys/time.h>
#include <stdio.h>

#include "sr_if.h"
#include "sr_protocol.h"
#ifdef VNL
#include "vnlconn.h"
#endif

/* we dont like this debug , but what to do for varargs ? */
#ifdef _DEBUG_
#define Debug(x, args...) printf(x, ## args)
#define DebugMAC(x) \
  do { int ivyl; for(ivyl=0; ivyl<5; ivyl++) printf("%02x:", \
  (unsigned char)(x[ivyl])); printf("%02x",(unsigned char)(x[5])); } while (0)
#else
#define Debug(x, args...) do{}while(0)
#define DebugMAC(x) do{}while(0)
#endif

#define INIT_TTL 255
#define PACKET_DUMP_SIZE 1024

/* forward declare */
struct sr_if;
struct sr_rt;

/* ----------------------------------------------------------------------------
 * struct sr_instance
 *
 * Encapsulation of the state for a single virtual router.
 *
 * -------------------------------------------------------------------------- */

struct sr_instance
{
    int  sockfd;   /* socket to server */
#ifdef VNL
    struct VnlConn* vc;
#endif
    char user[32]; /* user name */
    char host[32]; /* host name */
    char template[30]; /* template name if any */
    char auth_key_fn[64]; /* auth key filename */
    unsigned short topo_id;
    struct sockaddr_in sr_addr; /* address to server */
    struct sr_if* if_list; /* list of interfaces */
    struct sr_rt* routing_table; /* routing table */
    FILE* logfile;
};

/* -- sr_main.c -- */
int sr_verify_routing_table(struct sr_instance* sr);

/* -- sr_vns_comm.c -- */
int sr_send_packet(struct sr_instance* , uint8_t* , unsigned int , const char*);
int sr_connect_to_server(struct sr_instance* ,unsigned short , char* );
int sr_read_from_server(struct sr_instance* );

/* -- sr_router.c -- */
void sr_init(struct sr_instance* );
void sr_handlepacket(struct sr_instance* , uint8_t * , unsigned int , char* );
int dstIsBroadcast(struct sr_ethernet_hdr*);
int ipSanityCheck(uint8_t*);
int isIcmp(uint8_t*);
int sameIFList(struct sr_instance*, uint8_t*, const char*);

/* -- sr_if.c -- */
void sr_add_interface(struct sr_instance* , const char* );
void sr_set_ether_ip(struct sr_instance* , uint32_t );
void sr_set_ether_addr(struct sr_instance* , const unsigned char* );
void sr_print_if_list(struct sr_instance* );


/* -- new stuff from sr_router.c -- */
// Ethernet 
void makeEthernet(struct sr_ethernet_hdr*, uint16_t, uint8_t*, uint8_t*);

// ARP
#define ARP_CACHE_SIZE 100
#define ARP_STALE_TIME 15.0 // in seconds

struct arp_cache_entry {
    uint32_t    ar_sip;                 // sender ip address
    uint8_t     ar_sha[ETHER_ADDR_LEN]; // sender hardware address
    time_t      timeCached;             // time stamp
    int         valid;                  // timeout
};


void makeArp(struct sr_arphdr* arpHdr, uint16_t arp_hrd, uint16_t arp_pro, 
        uint8_t arp_hln, uint8_t arp_pln, uint16_t arp_op, uint8_t* arp_sha, 
        uint32_t arp_sip, uint8_t* arp_tha, uint32_t arp_tip );
void handleArp(struct sr_instance*, uint8_t*, unsigned int, char* );
void arpSendReply(struct sr_instance*, uint8_t*, unsigned int, char*, 
        struct sr_if* );
void arpSendRequest(struct sr_instance*, struct sr_if*, uint32_t );

void arpInitCache();
int arpSearchCache(uint32_t );
void arpCacheEntry(struct sr_arphdr* );
void arpUpdateCache();
uint8_t* arpReturnEntryMac(int );

// IP
#define PACKET_CACHE_SIZE 255 // maximum value of uint8_t

struct packet_cache_entry {
    uint8_t         packet[1500];       // Ethernet frame max size without padding
    struct sr_rt*   nexthop;            // pointer ot the next hop entry
    uint32_t        tip;                // target ip of the packet
    unsigned int    len;                // actual length of packet
    int             arps;               // number of times MAC was requested
    time_t          timeCached;
};

void makeIp(struct ip*, unsigned int, uint16_t, unsigned char, 
        unsigned char, uint32_t, uint32_t );
void handleForward(struct sr_instance*, uint8_t* , unsigned int , char* );
void forwardPacket(struct sr_instance*, uint8_t*, unsigned int, char*, uint8_t*);

void cachePacket(struct sr_instance* , uint8_t*, unsigned int, struct sr_rt*);
void checkCachedPackets(struct sr_instance*);
void initPacketCache();

// ICMP
#define ICMP_ECHO_REPLY 0
#define ICMP_ECHO_REQUEST 8

struct icmp_hdr {
    uint8_t     icmp_type;
    uint8_t     icmp_code;
    uint16_t    icmp_sum;
} __attribute__ ((packed)); // UCLA -- will have to add more for traceroute
// http://www.networksorcery.com/enp/protocol/icmp/msg0.htm

void makeIcmp(struct icmp_hdr*, uint8_t, uint8_t, unsigned int);
void handleIcmp(struct sr_instance*, uint8_t* , unsigned int , char* );
void icmpSendEchoReply(struct sr_instance*, uint8_t*, unsigned int, char*);


// checksum
uint16_t checksum(uint16_t*, int);

#endif /* SR_ROUTER_H */