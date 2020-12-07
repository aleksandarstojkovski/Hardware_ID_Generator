#include <stdio.h>
#include <ifaddrs.h>
#include <netpacket/packet.h>
#include <string.h>
#include "md5.h"
#include <sys/sysinfo.h>

void get_mac_info(char * mac_info){
    struct ifaddrs *ifaddr=NULL;
    struct ifaddrs *ifa = NULL;
    int i = 0;
    char concat[1000] = {0};
    char buff[500] = {0};

    if (getifaddrs(&ifaddr) == -1) {
         printf("unknown");
    }
    else {
         for ( ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
             // exclude wlan and wifi interfaces (because they are not loaded into the initramfs)
             if ( (ifa->ifa_addr) && (ifa->ifa_addr->sa_family == AF_PACKET)  && strstr(ifa->ifa_name,"wifi") == NULL && strstr(ifa->ifa_name,"wlan") == NULL ) {
                 struct sockaddr_ll *s = (struct sockaddr_ll*)ifa->ifa_addr;
                 snprintf(buff,sizeof(buff),"%-8s ", ifa->ifa_name);
                 strcat(concat, buff);
                  for (i=0; i <s->sll_halen; i++){
                      //printf("%02x%c", (s->sll_addr[i]), (i+1!=s->sll_halen)?':':'\n');
                      snprintf(buff, sizeof(buff),  "%02x%c", (s->sll_addr[i]), (i+1!=s->sll_halen)?':':'\n');
                      strcat(concat, buff);
                  }
             }
         }
         freeifaddrs(ifaddr);
         strcpy(mac_info, concat);
    }
}

void get_proc_info(char *buff){
    snprintf(buff,sizeof(buff),"%d%d",get_nprocs_conf(), get_nprocs());
}

void get_mem_info(char *buff){
    struct sysinfo si;
    sysinfo(&si);
    snprintf(buff,sizeof(buff),"%lu",si.totalram);
}

void compute_md5(char *str, unsigned char digest[16]) {
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, str, strlen(str));
    MD5_Final(digest, &ctx);
}

int main (int argc, const char * argv[]) {

    char mac_info[1000] = {0};
    char proc_info[1000] = {0};
    char mem_info[1000] = {0};
    char concat_info[1000] = {0};
    char proc_desc[] = "proc_info=";
    char mem_desc[] = "mem_info=";
    unsigned char md5_digest[16] = {0};
    
    get_mac_info(mac_info);
    get_proc_info(proc_info);
    get_mem_info(mem_info);

    strcat(concat_info, mem_desc);
    strcat(concat_info, mem_info);
    strcat(concat_info, "\n");
    strcat(concat_info, proc_desc);
    strcat(concat_info, proc_info);
    strcat(concat_info, "\n");
    strcat(concat_info, mac_info);

    // create md5sum
    compute_md5(concat_info, md5_digest);

    // print the hashed value
    for (int i = 0; i < 16; i++)
        printf("%02x", md5_digest[i]);
    putchar ('\n');

}
