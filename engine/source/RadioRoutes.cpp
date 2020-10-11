#include "RadioRoutes.hpp"

namespace radio
{

    void Routes::database_get_all(ipc::message::IPCRecived &recived, ipc::message::IPCReply &reply)
    {
        try
        {
            if (!database->isLoad())
                database->load();
            reply.setState(200);
            reply.setMessage("Stations");
            reply.setValue(database->toJson());
            Log::debug("Returned all stations!");
        }
        catch (std::string str)
        {
            std::string homedir = getenv("HOME");
            Log::info("HOMEDIR " + homedir);
            delete database;
            homedir += "/radio-database.json";
            database = new db::StationsJson(homedir);
            database->save();
            reply.setState(206);
            reply.setMessage("Cannot load database: " + str + ". Created new database: " + homedir);
            Log::warn("Cannot load database: " + str + ". CREATED NEW DATABASE: " + homedir);
        }
    }
 
    void Routes::database_put(ipc::message::IPCRecived &recived, ipc::message::IPCReply &reply)
    {
        radio::Station station (recived.getValue());
        std::unique_ptr<radio::Station> StationPtr = std::make_unique<radio::Station>(recived.getValue());
        try
        {
            database->put(std::move(StationPtr));
            reply.setState(200);
            reply.setMessage("Station added to database: " + station.getName());
            Log::debug("Station added to database!");
        }
        catch (...)
        {
            reply.setStatus(304);
            reply.setMessage("Cannot add/change station: " + station.getName());
            Log::debug("Cannot add/change station: " + station.getName());
        }
    }

    void Routes::database_delete(ipc::message::IPCRecived &recived, ipc::message::IPCReply &reply)
    {
        radio::Station station (recived.getValue());
        std::string id = station.getName();
        if (database->getByID(id) != nullptr)
        {
            database->remove(database->getByID(id));
            reply.setState(200);
            reply.setMessage("Station deleted!");
            Log::debug("Station deleted: " + station.getName());
        }
        else
        {
            reply.setState(400);
            reply.setMessage("Station does not exist: " + station.getName());
            Log::debug("Station does not exist: " + station.getName());
        }
    }

    void Routes::audio_switch_prev(ipc::message::IPCRecived &recive, ipc::message::IPCReply &reply)
    {
        if (database->getDatabase()->size() > 0){
            audio->setStation(
                database->getPrev(audio->getStation())
                );
            reply.setState(200);
            reply.setMessage("Station switch: previous");
            Log::debug("Station switch: previous");
        }
        else
        {
            reply.setState(400);
            reply.setMessage("Not found station to switch!");
            Log::debug("Not found station to switch!");
        }
    }

    void Routes::audio_switch_next(ipc::message::IPCRecived &recive, ipc::message::IPCReply &reply)
    {
        if (database->getDatabase()->size() > 0)
        {
            audio->setStation(
                database->getNext(audio->getStation()));
            reply.setState(200);
            reply.setMessage("Station switch: next");
            Log::debug("Station switch: next");
        }
        else
        {
            reply.setStatus(400);
            reply.setMessage("Not found station to switch!");
            Log::debug("Not found station to switch!");
        }
    }

    void Routes::audio_set_state(ipc::message::IPCRecived &recive, ipc::message::IPCReply &reply)
    {
        try {
            std::string state = recive.getValue()["state"].get<std::string>();
        } catch (nlohmann::json::exception &err){
            reply.setState(400);
            reply.setMessage("State not found! Wrong value given!");
            Log::debug("State not found! Wrong value given!");
            return;
        }
        if (audio->canSetState(state))
        {
            audio->setState(state);
            reply.setState(200);
            reply.setMessage("Audio state is: " + radio::Audio::stateToString(audio->getState()));
            Log::debug("Audio state is:" + radio::Audio::stateToString(audio->getState()));
        }
        else
        {
            reply.setState(400);
            reply.setMessage("Cannot set given state: " + state);
            Log::debug("Cannot set given state: " + state);
        }
    }

    void Routes::audio_set_station(ipc::message::IPCRecived &recive, ipc::message::IPCReply &reply)
    {
        radio::Station station (recived.getValue());
        if (station.getName().size() > 0 && database->getByID(station.getName()) != nullptr)
        {
            audio->setStation(database->getByID(station.getName()));
            reply.setState(200);
            reply.setMessage("Station setted: " + audio->getStation()->getName());
            Log::debug("Station setted: " + audio->getStation()->getName());
        }
        else
        {
            reply.setState(400);
            reply.setMessage("Station does not exist: " + station.getName());
            Log::debug("Station does not exist: " + id);
        }
    }

    void Routes::audio_get_state(ipc::message::IPCRecived &recived, ipc::message::IPCReply &reply)
    {
        nlohmann::json jdata;
        jdata["state"] = audio->getState();
        reply.setState(200);
        reply.setMessage("Station state");
        reply.setValue(jdata);
    }

    void Routes::audio_get_current(ipc::message::IPCRecived &recived, ipc::message::IPCReply &reply)
    {
        radio::Station *station = audio->getStation();
        if (station != nullptr)
        {
            reply.setState(200);
            reply.setMessage("Current station");
            reply.setValue(station->toJson());
        }
        else
        {
            reply.setState(400);
            reply.setMessage("No station loaded!");
            Log::debug("No station loaded!");
        }
    }

} // namespace radio