#include "analyzer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <queue>
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;


void TripAnalyzer::ingestFile(const string& path) {
    ifstream file(path);
    if (!file.is_open()) return;
    
    string line;
    getline(file, line);

    while (getline(file, line)) {
        if (line.empty()) return;
        
        size_t p1 = line.find(',');
        if (p1 == string::npos) continue;
        
        size_t p2 = line.find(',', p1 + 1);
        if (p2 == string::npos) continue;
      
        string id = line.substr(0, p1);
        
        try{
            stoi(id);
        } catch (...) {
            continue;
        }
        
        string pickup_zone = line.substr(p1 + 1, p2 - p1 - 1);
        if (pickup_zone.empty()) continue;
        
        string pickup_time = line.substr(p2 + 1);
        if (pickup_time.empty()) continue;
        
        tm t{};
        istringstream ss(pickup_time);
        ss >> get_time(&t, "%Y-%m-%d %H:%M");
        
        if (ss.fail()) continue;
        if (t.tm_hour < 0 || t.tm_hour > 23) continue;
        
        ZoneStats &stats = zoneData[pickup_zone];
        stats.count++;
        stats.hours[t.tm_hour]++;
    }
}

vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    auto comparing_function = [](const ZoneCount &zone1, const ZoneCount &zone2) {
        if (zone1.count != zone2.count)
            return zone1.count > zone2.count;        
        return zone1.zone < zone2.zone;             
    };

    priority_queue<ZoneCount, vector<ZoneCount>, decltype(comparing_function)> pq(comparing_function);
   
    for (const auto &zd : zoneData) {
        ZoneCount cur{zd.first, zd.second.count};

        if ((int)pq.size() < k) {
            pq.push(cur);
        } 
        else if (comparing_function(cur, pq.top())) {
            pq.pop();
            pq.push(cur);
        }
    }

    vector<ZoneCount> result;
    result.reserve(pq.size());

    while (!pq.empty()) {
        result.push_back(pq.top());
        pq.pop();
    }

    reverse(result.begin(), result.end());
    return result;
}

vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    auto comparing_function = [](const SlotCount &slot1, const SlotCount &slot2) {
        if (slot1.count != slot2.count)
            return slot1.count > slot2.count;           
        if (slot1.zone != slot2.zone)
            return slot1.zone < slot2.zone;             
        return slot1.hour < slot2.hour;                
    };

    priority_queue<SlotCount, vector<SlotCount>, decltype(comparing_function)> pq(comparing_function);

    for (const auto &zd : zoneData) {
        const string &zone = zd.first;
        const long long* hours = zd.second.hours;

        for (int h = 0; h < 24; h++) {
            long long c = hours[h];
            if (c == 0) continue;

            SlotCount cur{zone, h, c};

            if ((int)pq.size() < k) {
                pq.push(cur);
            } 
            else if (comparing_function(cur, pq.top())) {
                pq.pop();
                pq.push(cur);
            }
        }
    }

    vector<SlotCount> result;
    result.reserve(pq.size());

    while (!pq.empty()) {
        result.push_back(pq.top());
        pq.pop();
    }

    reverse(result.begin(), result.end());
    return result;
}
