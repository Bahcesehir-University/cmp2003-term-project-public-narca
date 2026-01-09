#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct ZoneStats {
    long long count = 0;
    long long hours[24] = {0};
};

struct ZoneCount {
    std::string zone;
    long long count;
};

struct SlotCount {
    std::string zone;
    int hour;              // 0–23
    long long count;
};

class TripAnalyzer {
public:
    // Parse Trips.csv, skip dirty rows, never crash
    void ingestFile(const std::string& csvPath);

    std::unordered_map<std::string, ZoneStats> zoneData;

    // Top K zones: count desc, zone asc
    std::vector<ZoneCount> topZones(int k = 10) const;

    // Top K slots: count desc, zone asc, hour asc
    std::vector<SlotCount> topBusySlots(int k = 10) const;
};
