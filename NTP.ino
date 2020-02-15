#include <TZ.h>
#include <coredecls.h>

int timeDiff = 0;

void ntpInit() {
  settimeofday_cb(ntpCallback);

  // Configure NTP to be able to get real-time from time()
  configTime(TZ_Etc_UTC, "pool.ntp.org");
}

void ntpCallback() {
  // Find difference between real-time and millis(), rounded up to the nearest increment
  timeDiff = (time(nullptr) - millis() / 1000) / HISTORY_INCREMENT_SEC * HISTORY_INCREMENT_SEC;
}
