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
@@ -18,6 +24,8 @@ class TripAnalyzer {
// Parse Trips.csv, skip dirty rows, never crash
void ingestFile(const std::string& csvPath);

    std::unordered_map<std::string, ZoneStats> zoneData;

// Top K zones: count desc, zone asc
std::vector<ZoneCount> topZones(int k = 10) const;
