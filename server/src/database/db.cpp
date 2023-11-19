#include "db.h"
#include <cwchar>
#include <optional>
#include <sstream>
#include <string>
#include <sw/redis++/redis.h>
#include <utility>

// helper function
double convert_string_2_double(const std::string& input)
{
    std::istringstream iss(input);
    // Set precision to maximum to preserve the exact representation
    iss >> std::setprecision(std::numeric_limits<double>::max_digits10);

    double result;
    iss >> result;

    if (iss.fail()) {
        // Handle conversion failure
        throw std::invalid_argument("Invalid input for conversion to double.");
    }

    return result;
}

namespace database {

Database::Database()
    : m_redis { "tcp://127.0.0.1" }
{
}

std::vector<std::pair<std::string, double>> Database::get_precomputed_ranking(std::string plz)
{
    // TODO
    return {};
}

std::vector<std::string> Database::get_neighbours(std::string& plz)
{
    std::vector<std::string> neighbours {};
    m_redis.smembers(plz, std::inserter(neighbours, neighbours.begin()));
    return neighbours;
}

crow::json::wvalue Database::service_provider_ret_val(std::string& id, double rankval)
{
    // TODO
    return {};
}

std::optional<std::string> Database::get_plz_density(std::string& plz)
{
    return m_redis.get(plz + "_group");
}

std::optional<std::pair<double, double>> Database::get_lat_lon_provider(std::string& wid)
{
    std::vector<std::optional<std::string>> vals;
    m_redis.hmget(provider_prefix + wid, { "lat", "lon" }, std::back_inserter(vals));
    if (!vals[0].has_value() || !vals[1].has_value()) {
        return {};
    }
    return std::make_pair(
        convert_string_2_double(vals[0].value()), convert_string_2_double(vals[1].value()));
}

std::optional<std::pair<double, double>> Database::get_lat_lon_plz(std::string& plz)
{
    std::vector<std::optional<std::string>> vals;
    m_redis.hmget(plz + "_coord", { "lat", "lon" }, std::back_inserter(vals));
    if (!vals[0].has_value() || !vals[1].has_value()) {
        return {};
    }
    return std::make_pair(
        convert_string_2_double(vals[0].value()), convert_string_2_double(vals[1].value()));
}

std::optional<double> Database::get_pfp_score(std::string& wid)
{
    auto result = m_redis.hget(provider_prefix + wid, "profile_picture_score");
    if (!result.has_value()) {
        return {};
    }
    return convert_string_2_double(result.value());
}

std::optional<double> Database::get_pfd_score(std::string& wid)
{
    auto result = m_redis.hget(provider_prefix + wid, "profile_description_score");
    if (!result.has_value()) {
        return {};
    }
    return convert_string_2_double(result.value());
}

std::optional<double> Database::get_max_distance(std::string& wid)
{
    auto result = m_redis.hget(provider_prefix + wid, "max_driving_distance");
    if (!result.has_value()) {
        return {};
    }
    return convert_string_2_double(result.value());
}

std::optional<std::string> Database::get_nearest_plz(std::string& wid)
{
    return m_redis.hget(provider_prefix + wid, "nearest_plz");
}

void Database::set_pfp_score(std::string& wid, double score)
{
    std::ostringstream strs;
    strs << score;
    m_redis.hset(provider_prefix + wid, "profile_picture_score", strs.str());
}

void Database::set_pfd_score(std::string& wid, double score)
{
    std::ostringstream strs;
    strs << score;
    m_redis.hset(provider_prefix + wid, "profile_description_score", strs.str());
}

void Database::set_max_distance(std::string& wid, size_t max_distance)
{
    std::ostringstream strs;
    strs << max_distance;
    m_redis.hset(provider_prefix + wid, "max_driving_distance", strs.str());
}

void Database::update_wid_reachable(std::string& wid, std::string& plz, double dist)
{
    std::string prefix = "reachable_";
    m_redis.zadd(prefix+wid, plz, dist);
}

void Database::update_wid_reachable_mass(std::string& wid, std::unordered_map<std::string, double> um)
{
    std::string prefix = "reachable_";
    m_redis.zadd(prefix+wid, um.begin(), um.end());
}


void Database::remove_wid_reachable(std::string& wid, std::string& plz){
    return;
}

void Database::update_plz_rank(std::string& wid, std::string& plz, double score)
{
    std::string prefix = "rank_";
    m_redis.zadd(prefix+plz, wid, score);
}


void Database::update_plz_rank_mass(const std::string& plz, std::unordered_map<std::string, double> um){
    std::string prefix = "rank_";
    m_redis.zadd(prefix+plz, um.begin(), um.end() );

}

}
