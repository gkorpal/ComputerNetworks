/*-----------------------------------------------------------------------------
 * file:  sr_pwospf.h
 * date:  Tue Nov 23 23:21:22 PST 2004 
 * Author: Martin Casado
 *
 * Description:
 *
 *---------------------------------------------------------------------------*/

#ifndef SR_PWOSPF_H
#define SR_PWOSPF_H

#include <pthread.h>
#include <stdint.h> // uint32_t
#include "sr_if.h" // SR_IFACE_NAMELEN
#include "pwospf_protocol.h" // struct ospfv2_lsu

/* forward declare */
struct sr_instance;

struct neighbor 
{
    uint32_t rid;
    uint32_t ip;
    unsigned char mac[6];
    time_t last_hello_received;
    int islost;
    struct neighbor* next;
};

struct pwospf_if // modified sr_if
{
    char name[SR_IFACE_NAMELEN];
    unsigned char mac[6];
    uint32_t ip;
    volatile uint32_t mask;
    uint16_t helloint;
    struct neighbor* nlist;
    struct pwospf_if* next;
};

struct pwospf_link 
{
    char interface[SR_IFACE_NAMELEN];
    uint32_t subnet;
    uint32_t mask;
    uint32_t nbor_rid;
    int isdown;
    struct pwospf_link* next;
};

struct pwospf_rt // modified sr_rt
{
    uint32_t dest; // same as struct in_addr for Linux
    uint32_t gw;
    uint32_t mask;
    char interface[SR_IFACE_NAMELEN];
    struct pwospf_rt* next;
};

struct pwospf_ftable 
{
    uint32_t dest;
    uint32_t gw;
    uint32_t mask;
    char interface[SR_IFACE_NAMELEN];
    struct pwospf_ftable* next;
};

struct pwospf_topo_entry 
{
    uint32_t sending_host;
    uint32_t src_rid;
    uint16_t last_seqnum_received;
    time_t last_received_time;
    char interface[SR_IFACE_NAMELEN];
    int num_adv; // number of lsu this entry contains
    struct ospfv2_lsu lsu_array[4]; // array of lsu received
};

struct pwospf_subsys // part of struct sr_instance in sr_router.h
{
    /* -- pwospf subsystem state variables here -- */
    struct pwospf_if* iflist;
    uint32_t rid; // router id
    uint32_t aid; // area id
    struct pwospf_link* links; // a list of links known to the router
    struct pwospf_rt* rt;
    struct pwospf_ftable ft[20]; // 20 rows should be enough
    int ftable_size; // number of entries; ft[ftable_size] non-zero entries
    uint16_t last_lsu_seq_sent;
    int topo_changed; // whether or not topo changed
    time_t last_lsu_sent;
    struct pwospf_topo_entry topo_entries[4];    

    /* -- thread and single lock for pwospf subsystem -- */
    pthread_t thread;
    pthread_mutex_t lock;
};

int pwospf_init(struct sr_instance*);
void pwospf_create_iflist(struct sr_instance*);
void pwospf_init_links(struct sr_instance*);
void pwospf_init_rt(struct sr_instance*);
void pwospf_init_ftable(struct sr_instance*);
void pwospf_get_router_id(struct sr_instance*);

// static void* pwospf_run_thread(void*); -- static functions are private
void pwospf_lock(struct pwospf_subsys*);
void pwospf_unlock(struct pwospf_subsys*);
void pwospf_check_nbor_timeout(struct sr_instance*);
void topo_delete_entry(struct sr_instance*, uint32_t, uint32_t);
void pwospf_check_topo_entry_timeout(struct sr_instance*);

void pwospf_broadcast_hello(struct sr_instance*);

void pwospf_flood_lsu(struct sr_instance*);
int pwospf_get_num_uplinks(struct sr_instance*);
void pwospf_forward_lsu(struct sr_instance*, uint8_t*, unsigned int, char*);

void pwospf_compute_shortest_path(struct sr_instance*);
void pwospf_clear_ftable(struct sr_instance*);
int is_direct_subnet(struct sr_instance*, uint32_t);
void pwospf_rt_delete_route(struct sr_instance*, uint32_t, struct pwospf_if*);
void pwospf_rt_delete_gw_route(struct sr_instance*, uint32_t, uint32_t);

void pwospf_handle_packet(struct sr_instance*, uint8_t*, unsigned int, char*);
void pwospf_handle_hello(struct sr_instance*, uint8_t*, unsigned int, char*);
struct pwospf_if* get_pwospf_if_by_name(struct sr_instance*, char*);
struct pwospf_link* pwospf_get_link_by_name(struct sr_instance*, char*);
void pwospf_handle_lsu(struct sr_instance*, uint8_t*, unsigned int, char*);
int topo_get_entry_index(struct sr_instance*, uint32_t, uint32_t);
int topo_get_free_entry_index(struct sr_instance*);
void topo_add_entry(struct sr_instance*, uint32_t, uint32_t, struct ospfv2_lsu_hdr*,
        int, char*);
void pwospf_add_rt(struct sr_instance*, int, uint32_t, struct pwospf_if*);
int topo_check_entry_content(struct sr_instance*, struct ospfv2_lsu_hdr*, int, 
        uint32_t, struct pwospf_if*);

#endif /* SR_PWOSPF_H */
