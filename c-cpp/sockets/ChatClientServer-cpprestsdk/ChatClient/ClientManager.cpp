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
* ClientManager.cpp
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/
#include "pch.h"
#include "ClientManager.h"

using namespace ChatClient;
using namespace Platform;
using namespace pplx;
using namespace utility;
using namespace utility::conversions;
using namespace web;
using namespace web::experimental::web_sockets::client;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;

ClientManager::ClientManager(MainPage^ client_ui) 
: m_local_client_info(ProjectUtilities::generate_guid(), ProjectUtilities::default_screen_name()), m_ws_client(CHAT_SERVER_URL), m_is_connected(false)
{
	m_client_ui = client_ui;
}

task<void> ClientManager::connect(const string_t& screen_name)
{
	//Connect to the server and send local client information
	return m_ws_client.connect().then([=](task<void> conn_task) {

		conn_task.get();
		m_is_connected = true;

		if (!screen_name.empty())
		{
			m_local_client_info.set_screen_name(screen_name);
		}

		// Start listening to any incoming message from server.
		listen_to_incoming_message();

		// Prepare packet for client login
		ServerClientPacket client_login_packet;
		client_login_packet.set_content_type(packet_content_type::CLIENT_LOGIN);
		client_login_packet.set_local_client(m_local_client_info);
				
		websocket_outgoing_message msg;
		msg.set_utf8_message(to_utf8string(client_login_packet.as_json().serialize()));
		return m_ws_client.send(msg);
	});
}

task<void> ClientManager::disconnect()
{
	// First inform the server that client wants to logout and then
	// close the connection. Prepare the packet to send to the server
	ServerClientPacket client_logout_packet;
	client_logout_packet.set_content_type(CLIENT_LOGOUT);
	client_logout_packet.set_local_client(m_local_client_info);

	websocket_outgoing_message msg;
	msg.set_utf8_message(to_utf8string(client_logout_packet.as_json().serialize()));

	return m_ws_client.send(msg).then([=](task<void> send_task) {
		send_task.get();

		return m_ws_client.close().then([=](task<void> close_task) {
			close_task.get();
			reset();
		});
	});
}

bool ClientManager::is_connected()
{
	return m_is_connected;
}

void ClientManager::listen_to_incoming_message()
{
	auto background_context = task_continuation_context::use_arbitrary();
	auto current_context = task_continuation_context::use_current();

	create_task([=]() {
		ProjectUtilities::async_do_while([=](){
			return m_ws_client.receive().then([=](pplx::task<websocket_incoming_message> in_msg_task){
				websocket_incoming_message in_msg = in_msg_task.get();

				return in_msg.extract_string().then([=](pplx::task<std::string> str_tsk) {

					std::string msg_str = str_tsk.get();
					string_t msg_str_t = to_string_t(msg_str);
					json::value json_val = json::value::parse(msg_str_t);
					ServerClientPacket packet(json_val);
					receive_message(packet);

				}, current_context);

			}).then([](pplx::task<void> end_task){
				try
				{
					end_task.get();
					return true;
				}
				catch (websocket_exception)
				{
					//
					// Do error handling or eat the exception as needed.
					//
				}
				catch (...)
				{
					//
					// Do error handling or eat the exception as needed.
					//
				}

				// We are here means we encountered some exception.
				// Return false to break the asynchronous loop.
				return false;
			});
		});
	}, background_context);
}

void ClientManager::receive_message(ServerClientPacket& packet)
{
	packet_content_type content_type = packet.get_content_type();

	if (content_type == packet_content_type::NEW_CONNECTED_CLIENTS)
	{
		// Update the client manager with new clients
		this->add_new_clients(packet.get_client_list());
		m_client_ui->UpdateClientListView();
	}
	else if (content_type == packet_content_type::NEW_DISCONNECTED_CLIENTS)
	{
		// Remove the offline clients
		this->remove_clients(packet.get_client_list());
		m_client_ui->UpdateClientListView();
	}
	else if (content_type == packet_content_type::CHAT_TEXT_MSG)
	{
		this->update_chat_text(packet.get_sender_client_id(), packet.get_receiver_client_id(), packet.get_chat_text());

		// Update the conversation window if this message is
		// for currently selected client in client ListView
		if (packet.get_sender_client_id() == string_t(m_client_ui->GetSelectedClientID()->Data()))
		{
			m_client_ui->UpdateConversationWindow();
		}
	}
}

const std::map<utility::string_t, ClientInfo>& ClientManager::get_client_list()
{
	return m_client_list;
}

void ClientManager::add_new_clients(const std::vector<ClientInfo>& client_list)
{
	for (const auto& iter : client_list)
	{
		m_client_list[iter.get_client_id()] = iter;
	}
}

void ClientManager::remove_clients(const std::vector<ClientInfo>& client_list)
{
	for (const auto& iter : client_list)
	{
		m_client_list.erase(iter.get_client_id());
	}
}

const ClientInfo& ClientManager::get_local_client_info()
{
	return m_local_client_info;
}

const ClientInfo& ClientManager::get_client_info(const utility::string_t& client_id)
{
	return m_client_list[client_id];
}

const utility::string_t& ClientManager::get_chat_text(const utility::string_t& client_id)
{
	if (m_chat_text_list.find(client_id) == m_chat_text_list.end())
	{
		m_chat_text_list[client_id] = utility::string_t();
	}

	return m_chat_text_list[client_id];
}

void ClientManager::update_chat_text(const string_t& sender_client_id, const string_t& receiver_client_id, const string_t& chat_text)
{
	if (m_local_client_info.get_client_id() == sender_client_id)
	{
		// It is outgoing message.
		const string_t& screenName = m_local_client_info.get_screen_name();
		const string_t& formatted_chat_text = U("\n\n") + screenName + U(": ") + chat_text;
		m_chat_text_list[receiver_client_id] += formatted_chat_text;
	}
	else
	{
		// It is incoming message.
		const string_t& screenName = this->get_client_info(sender_client_id).get_screen_name();
		const string_t& formatted_chat_text = U("\n\n") + screenName + U(": ") + chat_text;
		m_chat_text_list[sender_client_id] += formatted_chat_text;
	}	
}

task<void> ClientManager::send_chat_text(const string_t& receiver_client_id, const string_t& chat_text)
{
	// Prepare the packet to send to the server
	ServerClientPacket chat_text_packet;
	chat_text_packet.set_content_type(packet_content_type::CHAT_TEXT_MSG);
	chat_text_packet.set_sender_client_id(m_local_client_info.get_client_id());
	chat_text_packet.set_receiver_client_id(receiver_client_id);
	chat_text_packet.set_chat_text(chat_text);

	websocket_outgoing_message msg;
	msg.set_utf8_message(to_utf8string(chat_text_packet.as_json().serialize()));
	return m_ws_client.send(msg);
}

void ClientManager::reset()
{
	m_ws_client = websocket_client(CHAT_SERVER_URL);
	m_is_connected = false;
	m_client_list.clear();
	m_chat_text_list.clear();
}