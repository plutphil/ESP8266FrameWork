wl_status_t ESP8266WiFiMulti::run(void) {

    int8_t scanResult;
    wl_status_t status = WiFi.status();
    if(status == WL_DISCONNECTED || status == WL_NO_SSID_AVAIL || status == WL_IDLE_STATUS || status == WL_CONNECT_FAILED) {

        scanResult = WiFi.scanComplete();
        if(scanResult == WIFI_SCAN_RUNNING) {
            // scan is running
            return WL_NO_SSID_AVAIL;
        } else if(scanResult > 0) {
            // scan done analyze
            WifiAPlist_t bestNetwork { NULL, NULL };
            int bestNetworkDb = INT_MIN;
            uint8 bestBSSID[6];
            int32_t bestChannel;

            DEBUG_WIFI_MULTI("[WIFI] scan done\n");
            delay(0);

            if(scanResult <= 0) {
                DEBUG_WIFI_MULTI("[WIFI] no networks found\n");
            } else {
                DEBUG_WIFI_MULTI("[WIFI] %d networks found\n", scanResult);
                for(int8_t i = 0; i < scanResult; ++i) {

                    String ssid_scan;
                    int32_t rssi_scan;
                    uint8_t sec_scan;
                    uint8_t* BSSID_scan;
                    int32_t chan_scan;
                    bool hidden_scan;

                    WiFi.getNetworkInfo(i, ssid_scan, sec_scan, rssi_scan, BSSID_scan, chan_scan, hidden_scan);

                    bool known = false;
                    for(uint32_t x = 0; x < APlist.size(); x++) {
                        WifiAPlist_t entry = APlist[x];

                        if(ssid_scan == entry.ssid) { // SSID match
                            known = true;
                            if(rssi_scan > bestNetworkDb) { // best network
                                if(sec_scan == ENC_TYPE_NONE || entry.passphrase) { // check for passphrase if not open wlan
                                    bestNetworkDb = rssi_scan;
                                    bestChannel = chan_scan;
                                    memcpy((void*) &bestNetwork, (void*) &entry, sizeof(bestNetwork));
                                    memcpy((void*) &bestBSSID, (void*) BSSID_scan, sizeof(bestBSSID));
                                }
                            }
                            break;
                        }
                    }

                    if(known) {
                        DEBUG_WIFI_MULTI(" ---> ");
                    } else {
                        DEBUG_WIFI_MULTI("      ");
                    }

                    DEBUG_WIFI_MULTI(" %d: [%d][%02X:%02X:%02X:%02X:%02X:%02X] %s (%d) %c\n", i, chan_scan, BSSID_scan[0], BSSID_scan[1], BSSID_scan[2], BSSID_scan[3], BSSID_scan[4], BSSID_scan[5], ssid_scan.c_str(), rssi_scan, (sec_scan == ENC_TYPE_NONE) ? ' ' : '*');
                    delay(0);
                }
            }

            // clean up ram
            WiFi.scanDelete();

            DEBUG_WIFI_MULTI("\n\n");
            delay(0);

            if(bestNetwork.ssid) {
                DEBUG_WIFI_MULTI("[WIFI] Connecting BSSID: %02X:%02X:%02X:%02X:%02X:%02X SSID: %s Channal: %d (%d)\n", bestBSSID[0], bestBSSID[1], bestBSSID[2], bestBSSID[3], bestBSSID[4], bestBSSID[5], bestNetwork.ssid, bestChannel, bestNetworkDb);

                WiFi.begin(bestNetwork.ssid, bestNetwork.passphrase, bestChannel, bestBSSID);
                status = WiFi.status();

                // wait for connection or fail
                while(status != WL_CONNECTED && status != WL_NO_SSID_AVAIL && status != WL_CONNECT_FAILED) {
                    delay(10);
                    status = WiFi.status();
                }

            } else {
                DEBUG_WIFI_MULTI("[WIFI] no matching wifi found!\n");
            }
        } else {
            // start scan
            DEBUG_WIFI_MULTI("[WIFI] delete old wifi config...\n");
            WiFi.disconnect();

            DEBUG_WIFI_MULTI("[WIFI] start scan\n");
            // scan wifi async mode
            WiFi.scanNetworks(true);
        }
    }
    return status;
}