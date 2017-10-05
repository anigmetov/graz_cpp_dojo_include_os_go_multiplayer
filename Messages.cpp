#include <iostream>

#include "Messages.h"

using namespace rapidjson;

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "Game.h"
#include "GameSettings.h"

void handle_message(const std::string& json, net::WebSocket* ws, Game* game) {
  // Handle incoming messages
  // TBD...

    Document doc;
    doc.Parse(json.c_str());

    std::cout << "Enter handle_message, type = " << doc["type"].GetString()  << std::endl;

    if (0 == strcmp(doc["type"].GetString(), "submit move")) {
        std::cout << "submit_move detected" << std::endl;
        int move = doc["move"].GetInt();
        int result = game->submit_move(move);
        std::cout << "game.result = " << result << std::endl;
        Document json_result;
        json_result.SetObject();
        auto& alloc = json_result.GetAllocator();
        Value res_type;
        res_type.SetString("move confirmed");
        json_result.AddMember("type", res_type, alloc);
        json_result.AddMember("move", result, alloc);
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        json_result.Accept(writer);
        ws->write(buffer.GetString());
        std::cout << "ws->write called, msg = " << buffer.GetString() << std::endl;
    }


  // Read JSON using rapidjson example:
  // doc.Parse(json.c_str());
  // assert(doc["type"].IsString());
  // assert(doc["move"].IsInt());
  // printf("Handling 'submit move' Event!\n");
  // int submitted_move = doc["move"].GetInt();
  
  // Write JSON using rapidjson example:
  // StringBuffer sb;
  // Writer<StringBuffer> writer(sb);
  // writer.StartObject();
  // writer.Key("type");
  // writer.String("initial state");
  // writer.Key("board");
  // writer.StartArray();
  // for (auto&& field : game->board) {
  //     writer.Int(field);
  // }
  // writer.EndArray();    
  // writer.EndObject();

  // Sending to client example:
  // ws->write(sb.GetString());
}
