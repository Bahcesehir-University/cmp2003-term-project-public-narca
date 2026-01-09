#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

struct ZoneStats {
    int count = 0;                 
    vector<int> hours;            

    ZoneStats() : count(0), hours(24, 0) {}
};

struct PickupZone {
    string zone;
    int count;
};

struct BusiestSlot {
    string zone;
    int hour;
    int count;
};


class TripAnalyzer {
public:
    void ingestStdin();

    unordered_map<string, ZoneStats> zoneData;

    
    vector<PickupZone> topZones() {

        auto comparing_function = [](const PickupZone &zone1, const PickupZone &zone2) {
            if (zone1.count != zone2.count)
                return zone1.count > zone2.count;        
            return zone1.zone < zone2.zone;             
        };

        priority_queue<PickupZone, vector<PickupZone>, decltype(comparing_function)> pq(comparing_function);
       
        for (const auto &zd : zoneData) {
            
            PickupZone cur{zd.first, zd.second.count};

            if ((int)pq.size() < 10) {
                pq.push(cur);
            } 
            else if (comparing_function(cur, pq.top())) {
                pq.pop();
                pq.push(cur);
            }
        }

        vector<PickupZone> result;
        result.reserve(pq.size());

        while (!pq.empty()) {
            result.push_back(pq.top());
            pq.pop();
        }

        reverse(result.begin(), result.end());
        return result;
    }

vector<BusiestSlot> topBusySlots() {

    auto comparing_function = [](const BusiestSlot &slot1, const BusiestSlot &slot2) {
        if (slot1.count != slot2.count)
            return slot1.count > slot2.count;           
        if (slot1.zone != slot2.zone)
            return slot1.zone < slot2.zone;             
        return slot1.hour > slot2.hour;                
    };

    priority_queue<BusiestSlot, vector<BusiestSlot>, decltype(comparing_function)> pq(comparing_function);

    for (const auto &zd : zoneData) {
        const string &zone = zd.first;
        const auto &hours = zd.second.hours;

        for (int h = 0; h < 24; ++h) {
            int c = hours[h];
            if (c == 0) continue;

            BusiestSlot cur{zone, h, c};

            if ((int)pq.size() < 10) {
                pq.push(cur);
            } 
            else if (comparing_function(cur, pq.top())) {
                pq.pop();
                pq.push(cur);
            }
        }
    }

    vector<BusiestSlot> result;
    result.reserve(pq.size());

    while (!pq.empty()) {
        result.push_back(pq.top());
        pq.pop();
    }

    reverse(result.begin(), result.end());
    return result;
}


};

void TripAnalyzer::ingestStdin() {
    string line;
    
    while (getline(cin, line)) {
        size_t pos1 = line.find(',');           
        if (pos1 == string::npos) continue;

        size_t pos2 = line.find(',', pos1 + 1); 
        if (pos2 == string::npos) continue;

        string pickup_zone = line.substr(pos1 + 1, pos2 - pos1 - 1);
        if (pickup_zone.empty()) continue;
        
        size_t pos3 = line.find(',', pos2 + 1); 
        if (pos3 == string::npos) continue;

        size_t pos4 = line.find(',', pos3 + 1); 
        if (pos4 == string::npos) continue;

        string pickup_date_time = line.substr(pos3 + 1, pos4 - pos3 - 1);

        if (pickup_date_time.size() < 16) 
        {
            continue;
        }

        int hour;
        try{
            hour = stoi(pickup_date_time.substr(11, 2));
        }
        catch(...){
            continue;
        }
        if (hour < 0 || hour > 23) continue;

        ZoneStats &stats = zoneData[pickup_zone];
        stats.count++;
        stats.hours[hour]++;
    }
}

int main() {
    TripAnalyzer analyzer;

    analyzer.ingestStdin();
    
    cout << "TOP_ZONES\n";
    for (auto& z : analyzer.topZones())
        cout << z.zone << "," << z.count << "\n";
    
    cout << "TOP_SLOTS\n";
    for (auto& s : analyzer.topBusySlots())
        cout << s.zone << "," << s.hour << "," << s.count << "\n";

    return 0;
}
