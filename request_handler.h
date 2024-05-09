#pragma once
#include "json_reader.h"

namespace transport_system {
    
    class RequestHandler {
        public:
            explicit RequestHandler(const TransportCatalogue& tc, map_entities::MapRenderer& mr) : TC_(tc), MR_(mr) {}
            void FormAnswer(std::ostream& out) const;
            json::Dict ProcessBusRequest(const std::string& request, int id) const;
            json::Dict ProcessStopRequest(const std::string& request, int id) const;
            json::Dict ProcessMapRequest(int id) const;
            std::pair<std::vector<geo::Coordinates>, std::vector<std::string>> ProcessDrawRequest(const std::string& request) const;
        
        private:
            const TransportCatalogue& TC_;
            map_entities::MapRenderer& MR_;
            const std::map<std::string, std::vector<std::string>>& paths_=TC_.GivePathDataReadAccess();
            const std::map<std::string, detail::StopData>& stops_=TC_.GiveStopDataReadAccess();
            const std::vector<std::pair<int, std::string>>& requests_=TC_.GiveRequestDataReadAccess();
            const std::map<std::string, std::set<std::string>>& BTS_=TC_.GiveBusThroughStopDataReadAccess();
    };
    
}