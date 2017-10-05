// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2017 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <net/inet4>
#include <service>
#include <timers>
#include <net/ws/websocket.hpp>

#include "Messages.h"
#include "Game.h"
#include "rapidjson/writer.h"

/// Can be used to store game state
std::unique_ptr<Game> game;

static std::map<int, net::WebSocket_ptr> websockets;
static int idx = 0;

  // Implement Webssocket Broadcasting
  // TBD...


/// Called on Websocket connection
void handle_ws(net::WebSocket_ptr ws)
{
  // nullptr means the WS attempt failed
  if(not ws) {
    printf("WS failed\n");
    return;
  }
  printf("WS Connected: %s\n", ws->to_string().c_str());
  
  // Handle incoming events
  ws->on_read = [ws = ws.get()](auto msg) {
    auto json = msg->as_text();
    printf("WS Recv: %s\n", json.c_str());
    handle_message(json, ws, game.get());
  };

  websockets[idx] = std::move(ws);
  // Notify on close
  websockets[idx]->on_close = [key = idx](auto code) {
    printf("WS Closing (%u) %s\n", code, websockets[key]->to_string().c_str());
  };
  idx++;
}

#include <net/http/server.hpp>
#include <memdisk>
std::unique_ptr<http::Server> server;

void Service::start()
{
  // Retreive the stack (configured from outside)
  auto& inet = net::Inet4::stack<0>();
  Expects(inet.is_configured());

  // Init the memdisk
  auto& disk = fs::memdisk();
  disk.init_fs([] (auto err, auto&) {
    Expects(not err);
  });

  // Init the game
  game = std::make_unique<Game>();  

  // Create a HTTP Server and setup request handling
  server = std::make_unique<http::Server>(inet.tcp());
  server->on_request([] (auto req, auto rw)
  {
    // We only support get
    if(req->method() != http::GET) {
      rw->write_header(http::Not_Found);
      return;
    }
    // Serve HTML on /
    if(req->uri() == "/") {
      auto& disk = fs::memdisk();
      auto file = disk.fs().read_file("/index.html");
      Expects(file.is_valid());
      Chunk html{file.data(), file.size()};      
      rw->write(html);
    }
    else if(req->uri() == "/static/js/main.8ea9ab99.js") {
      auto& disk = fs::memdisk();
      auto jsfile = disk.fs().read_file("/static/js/main.8ea9ab99.js");
      Expects(jsfile.is_valid());
      Chunk js{jsfile.data(), jsfile.size()};      
      rw->write(js);
    }    
    else if(req->uri() == "/static/css/main.78b50556.css") {
      auto& disk = fs::memdisk();
      auto cssfile = disk.fs().read_file("/static/css/main.78b50556.css");
      Expects(cssfile.is_valid());
      Chunk css{cssfile.data(), cssfile.size()};          
      rw->write(css);
    }
    else if(req->uri() == "/static/media/goField.776c793a.png") {
      auto& disk = fs::memdisk();      
      auto pngfile = disk.fs().read_file("/static/media/goField.776c793a.png");
      Expects(pngfile.is_valid());
      Chunk png{pngfile.data(), pngfile.size()};          
      rw->write(png);      
    }
    // WebSockets go here
    else if(req->uri() == "/ws") {
      auto ws = net::WebSocket::upgrade(*req, *rw);
      handle_ws(std::move(ws));
    }
    else {
      rw->write_header(http::Not_Found);
    }
  });

  // Start listening on port 80
  server->listen(80);

  Timers::periodic(5s, 15s, [&inet](uint32_t) {
      for(auto& ws : websockets) {
          auto moveAndPlayer = game->end_turn();

          rapidjson::Document doc;
          doc.SetObject();
          auto& alloc = doc.GetAllocator();
          rapidjson::Value res_type;
          res_type.SetString("round ended");
          doc.AddMember("type", res_type, alloc);
          doc.AddMember("move", moveAndPlayer.first, alloc);
          doc.AddMember("team", moveAndPlayer.second, alloc);
          rapidjson::StringBuffer buffer;
          rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
          doc.Accept(writer);

          ws.second->write(buffer.GetString());
      }
  });

  // Start a new round every 10 seconds
  // TBD...
  // For example:
  // Timers::periodic(5s, 10 s,
  //   [&inet] (uint32_t) {
  //     broadcast_round_end();
  //   });    
}
