#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <queue>
#include <sstream>
#include <iomanip>

using namespace std;

struct ZoneStats {
    long long count = 0;
    long long hours[24] = {0};
};

struct ZoneCount {
    string zone;
    long long count;
};

struct SlotCount {
    string zone;
    int hour;
    long long count;
};


class TripAnalyzer {
private:
    unordered_map<string, ZoneStats> zoneData;
    
    void processLine(const string& line) {
        if (line.empty()) return;
        
        size_t p1 = line.find(',');
        if (p1 == string::npos) return;
        
        size_t p2 = line.find(',', p1 + 1);
        if (p2 == string::npos) return;
        
        size_t p3 = line.find(',', p2 + 1);
        if (p3 == string::npos) return;
        
        size_t p4 = line.find(',', p3 + 1);
        if (p4 == string::npos) return;
        
        size_t p5 = line.find(',', p4 + 1);
        if (p5 == string::npos) return;
        
        string pickup_zone = line.substr(p1 + 1, p2 - p1 - 1);
        if (pickup_zone.empty()) return;
        
        string pickup_time = line.substr(p3 + 1, p4 - p3 - 1);
        if (pickup_time.empty()) return;
        
        tm t{};
        istringstream ss(pickup_time);
        ss >> get_time(&t, "%Y-%m-%d %H:%M");
        
        if (ss.fail()) return;
        if (t.tm_hour < 0 || t.tm_hour > 23) return;
        
        ZoneStats &stats = zoneData[pickup_zone];
        stats.count++;
        stats.hours[t.tm_hour]++;
    }

public:
    void ingestFile(const string& path) {
        ifstream file(path);
        if (!file.is_open()) return;
        
        string line;
        while (getline(file, line)) {
            processLine(line);
        }
    }
    
    vector<ZoneCount> topZones(int k=10) {
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

    vector<SlotCount> topBusySlots(int k=10) {
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
};
