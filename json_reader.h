#pragma once
#include "json.h"
#include "map_renderer.h"

namespace transport_system {

    class JsonReader {
        public:
            explicit JsonReader(TransportCatalogue& tc, map_entities::MapRenderer& mr) : TC_(tc), MR_(mr) {}
            void LoadData(std::istream& in);
		    void LoadBaseRequests(const json::Array& base_requests);
			void LoadStatRequests(const json::Array& stat_requests);
			void LoadRenderSettings(const json::Dict& render_settings);
        
        private:
            TransportCatalogue& TC_;
            map_entities::MapRenderer& MR_;
    };
    
}