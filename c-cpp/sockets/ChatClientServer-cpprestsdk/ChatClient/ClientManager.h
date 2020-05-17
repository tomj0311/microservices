/***
* ==++==
*
* Copyright (c) Microsoft Corporation. All rights reserved.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* ClientManager.h
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/
#pragma once

#include <map>

#include "MainPage.xaml.h"

#include "..\Common\ClientInfo.h"
#include "..\Common\ProjectUtilities.h"
#include "..\Common\ServerClientPackets.h"

#include "cpprest\streams.h"
#include "cpprest\astreambuf.h"
#include "cpprest\containerstream.h"
#include "cpprest\ws_client.h"
#include "cpprest\asyncrt_utils.h"

namespace ChatClient
{
	ref class MainPage;

	class ClientManager
	{
	public:
		ClientManager(MainPage^ client_ui);

		pplx::task<void> connect(const utility::string_t& screen_name);
		pplx::task<void> disconnect();

		bool is_connected();
		void reset();

		void listen_to_incoming_message();

		const ClientInfo& get_local_client_info();

		const std::map<utility::string_t, ClientInfo>& get_client_list();
		void add_new_clients(const std::vector<ClientInfo>& client_list);
		void remove_clients(const std::vector<ClientInfo>& client_list);

		void update_chat_text(const utility::string_t& sender_client_id, const utility::string_t& receiver_client_id, const utility::string_t& chat_text);
		pplx::task<void> send_chat_text(const utility::string_t& receiver_client_id, const utility::string_t& chat_text);

		const ClientInfo& get_client_info(const utility::string_t& client_id);
		const utility::string_t& get_chat_text(const utility::string_t& client_id);

	private:
		void receive_message(ServerClientPacket& packet);

		MainPage^ m_client_ui;
		ClientInfo m_local_client_info;
		web::experimental::web_sockets::client::websocket_client m_ws_client;
		bool m_is_connected;

		std::map<utility::string_t, ClientInfo> m_client_list;
		std::map<utility::string_t, utility::string_t> m_chat_text_list;
	};
}

