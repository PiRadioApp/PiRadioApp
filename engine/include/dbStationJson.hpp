#pragma once

#include <fstream>
#include <string>

#include "json.hpp"

#include "Database.hpp"
#include "Station.hpp"
#include "Logs.hpp"


namespace db {

    class StationsJson : public db::Database<std::string, radio::Station> {

        bool loadFlag = false;
        std::string filePath;
        
    public:
        StationsJson(std::string filePath);
        ~StationsJson();

        virtual bool isLoad();
        virtual void load();
        virtual void save();

        virtual std::string toString();
        virtual nlohmann::json toJson();

        virtual radio::Station* getNext(radio::Station* record);
        virtual radio::Station* getPrev(radio::Station* record);

    };

} // namespace db
