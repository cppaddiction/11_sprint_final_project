#include "request_handler.h"

void transport_system::RequestHandler::FormAnswer(std::ostream& out) const
{
    bool is_first=true;
    out<<'[';
    for(const auto& request: requests_)
    {
        if(!is_first)
        {
            out<<',';
        }
        if(request.second[0]=='B')
        {
            out<<Print(ProcessBusRequest(request.second, request.first));
        }
        else if(request.second[0]=='S')
        {           
            out<<Print(ProcessStopRequest(request.second.substr(5, request.second.size()-5), request.first));
        }
        else
        {
            out<<Print(ProcessMapRequest(request.first));
        }
        is_first=false;
    }
    out<<']';
}

json::Dict transport_system::RequestHandler::ProcessBusRequest(const std::string& request, int id) const
{
    json::Dict answer;
    if(paths_.find(request)!=paths_.end())
    {
        const auto& v=paths_.at(request);
        std::set<std::string> s(v.begin(), v.end());

        auto stops_on_route=v.size()-1;
        auto unique_stops=s.size()-1;

        double distance=0;
        double real_distance=0;
        bool is_round_path=">"==v[v.size()-1]?true:false;

        for(int i=1; i<static_cast<int>(v.size())-1; i++)
        {
            const auto& stop1=stops_.at(v[i]);
            const auto& stop2=stops_.at(v[i-1]);
            distance+=geo::ComputeDistance(geo::Coordinates{stop1.coords.lat, stop1.coords.lng}, geo::Coordinates{stop2.coords.lat, stop2.coords.lng});
            if(is_round_path)
            {
                try
                {
                    real_distance+=stop2.connections.at(v[i]);
                }
                catch(const std::out_of_range& e)
                {
                    real_distance+=stop1.connections.at(v[i-1]);
                }
            }
            else
            {
                try
                {
                    real_distance+=stop2.connections.at(v[i]);
                }
                catch(const std::out_of_range& e)
                {
                    real_distance+=stop1.connections.at(v[i-1]);
                }
                try
                {
                    real_distance+=stop1.connections.at(v[i-1]);
                }
                catch(const std::out_of_range& e)
                {
                    real_distance+=stop2.connections.at(v[i]);
                }
            }
        }

        if(!is_round_path)
        {
            distance*=2;
            stops_on_route=stops_on_route*2-1;
        }

        double curve=real_distance/distance;

        answer["curvature"]=json::Node(curve);
        answer["request_id"]=json::Node(id);
        answer["route_length"]=json::Node(real_distance);
        answer["stop_count"]=json::Node(int(stops_on_route));
        answer["unique_stop_count"]=json::Node(int(unique_stops));
    }
    else
    {
        answer["request_id"]=json::Node(id);
        std::string msg="not found";
        answer["error_message"]=json::Node(msg);
    }
    return answer;
}

json::Dict transport_system::RequestHandler::ProcessStopRequest(const std::string& request, int id) const
{    
    json::Dict answer;
    if(BTS_.find(request)!=BTS_.end())
    {
        const auto& item=BTS_.at(request);
        json::Array tempv;
        if(item.size()!=0)
        {
            std::vector<std::string> temp(item.begin(), item.end());
            std::sort(temp.begin(), temp.end());
            for(const auto& t: temp)
            {
                tempv.push_back(json::Node(t.substr(4, t.size()-4)));
            }
        }
        answer["buses"]=tempv;
        answer["request_id"]=json::Node(id);
    }
    else
    {
        answer["request_id"]=json::Node(id);
        std::string msg="not found";
        answer["error_message"]=json::Node(msg);
    }
    return answer;
}

json::Dict transport_system::RequestHandler::ProcessMapRequest(int id) const
{  
    std::ostringstream answer;
    MR_.Draw(answer);    
    json::Dict one;
    one["map"]=json::Node(answer.str());
    one["request_id"] = json::Node(id);    
    return one;
}