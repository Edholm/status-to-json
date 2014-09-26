/* Get essid and signal strength from wifi
 *
 * Copyright 2014 Emil Edholm <emil@edholm.it>
*/
#include <string>
#include <iostream>
#include <iwlib.h>
#include "wifi.hh"

using namespace std;

Wifi::winfo Wifi::collect() {
    struct winfo wi;
    int skfd;
    struct wireless_info *winfo;
    winfo = (struct wireless_info*)malloc(sizeof(struct wireless_info));
    memset(winfo, 0, sizeof(struct wireless_info));

    skfd = iw_sockets_open();
    if(iw_get_basic_config(skfd, INTERFACE, &(winfo->b)) > -1) {
        // Get statistics, such as quality
        if (iw_get_stats(skfd, INTERFACE, &(winfo->stats),
                &winfo->range, winfo->has_range) >= 0) {
            winfo->has_stats = 1;
        }
        if (iw_get_range_info(skfd, INTERFACE, &(winfo->range)) >= 0) {
            winfo->has_range = 1;
        }
        wi.quality = (winfo->stats.qual.qual*100) /
                        winfo->range.max_qual.qual;

        if (winfo->b.has_essid) {
            if (winfo->b.essid_on) {
                wi.essid = winfo->b.essid;
                wi.is_connected = true;
            }
            else {
                wi.essid = "N/A";
                wi.is_connected = false;
            }
        }
    }
    iw_sockets_close(skfd);
    free(winfo);
    return wi;
}

string Wifi::generate_json() {
    struct winfo wi = collect();
    if(!wi.is_connected) {
        return "Not connected!";
    }
    return wi.essid + " (" + to_string(wi.quality) + "%)";
};