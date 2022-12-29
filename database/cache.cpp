#include "cache.h"
#include "../config/config.h"

#include <exception>
#include <mutex>

#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

namespace database
{
    std::mutex _mtx;
    Cache::Cache()
    {
        std::string host = Config::get().get_cache_servers().substr(0,
                                                                    Config::get().get_cache_servers().find(':'));
        std::string port = Config::get().get_cache_servers().substr(Config::get().get_cache_servers().find(':') + 1);

        _stream = rediscpp::make_stream(host, port);
        try
        {
            auto response = rediscpp::execute(*_stream, "ping");
            std::cout << response.as<std::string>() << std::endl;
        }
        catch (std::exception const &e)
        {
            std::cerr << "--- redis error: " << e.what() << std::endl;
            return;
        }
    }

    Cache Cache::get()
    {
        static Cache instance;
        return instance;
    }

    void Cache::put([[maybe_unused]] const std::string &id, [[maybe_unused]] const std::string &val)
    {
        std::lock_guard<std::mutex> lck(_mtx);
        try
        {
            rediscpp::value response = rediscpp::execute(*_stream, "set",
                                                         id,
                                                         val, "ex", "60");
        }
        catch (std::exception &ex)
        {
            std::cout << "+++++ redis exception: can't set " << ex.what() << std::endl;
        }
    }

    bool Cache::get([[maybe_unused]] const std::string &id, [[maybe_unused]] std::string &val)
    {
        std::lock_guard<std::mutex> lck(_mtx);
        rediscpp::value response = rediscpp::execute(*_stream, "get", id);

        if (response.is_error_message())
            return false;
        if (response.empty())
            return false;

        try
        {
            if(response.is_string()){
                val = response.as<std::string>();
            } else return false;
        }
        catch (std::exception &ex)
        {
            //std::cout << "--- redis exception: " << ex.what() << std::endl;
            return false;
        }

        return true;
    }
}