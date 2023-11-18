#pragma once

#include <string>
#include <vector>

#include <hiredis/hiredis.h>

namespace database {

struct ServiceProvider {
    ServiceProvider(
        std::size_t id,
        std::string first_name,
        std::string last_name,
        std::string city,
        std::string house_number,
        double lon,
        double lat,
        unsigned max_driving_distance,
        double profile_picture_score,
        double profile_description_score,
        std::string nearest_plz)
        : id { id }
        , first_name { first_name }
        , last_name { last_name }
        , city { city }
        , house_number { house_number }
        , lon { lon }
        , lat { lat }
        , max_driving_distance { max_driving_distance }
        , profile_picture_score { profile_picture_score }
        , profile_description_score { profile_description_score }
        , nearest_plz { nearest_plz }
    {
    }

    std::size_t id;
    std::string first_name;
    std::string last_name;
    std::string city;
    std::string house_number;
    double lon;
    double lat;
    unsigned max_driving_distance;
    double profile_picture_score;
    double profile_description_score;
    std::string nearest_plz;
};

class Database {
public:
    Database();

    std::vector<ServiceProvider> get_ranking(std::string& plz, std::size_t amount = 20);

    ~Database();
private:
    redisContext* m_c;
    std::vector<std::string> get_neighbours(std::string& plz);
};
}