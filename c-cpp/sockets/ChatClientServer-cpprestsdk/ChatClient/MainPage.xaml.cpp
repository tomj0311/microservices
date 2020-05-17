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
* MainPage.xaml.cpp
* Implementation of the MainPage class.
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/
#include "pch.h"
#include "MainPage.xaml.h"

using namespace ChatClient;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Popups;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace pplx;
using namespace web::experimental::web_sockets::client;
using namespace utility;
using namespace utility::conversions;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage()
{
	InitializeComponent();

	m_client_manager = std::make_shared<ClientManager>(this);
	m_listview_map = ref new Map<String^, ListViewItem^>();
}

Platform::String^ MainPage::GetSelectedClientID()
{
	String^ selected_client_id = nullptr;

	if (clientListView->Items->Size > 0 && clientListView->SelectedIndex != -1)
	{
		selected_client_id = (String^)(((ListViewItem^)clientListView->SelectedItem)->Tag);
	}

	return selected_client_id;
}

void MainPage::UpdateClientListView()
{
	m_listview_map->Clear();
	clientListView->Items->Clear();
	
	String^ selected_client_id = GetSelectedClientID();
	auto& client_list_map = m_client_manager->get_client_list();
	
	for (const auto& iter : client_list_map)
	{
		String^ client_id = ref new String(iter.first.c_str());;

		ListViewItem^ lvi = ref new ListViewItem();
		lvi->Content = ref new String(iter.second.get_screen_name().c_str());
		lvi->Tag = client_id;
		lvi->HorizontalContentAlignment = Windows::UI::Xaml::HorizontalAlignment::Center;
		lvi->VerticalContentAlignment = Windows::UI::Xaml::VerticalAlignment::Center;

		m_listview_map->Insert(client_id, lvi);
		clientListView->Items->Append(lvi);

		if (selected_client_id != nullptr && selected_client_id == client_id)
		{
			clientListView->SelectedItem = lvi;
		}
	}

	// If nothing is selcected, select the first client
	if (clientListView->Items->Size > 0 && clientListView->SelectedIndex == -1)
	{
		clientListView->SelectedIndex = 0;
	}

	UpdateConversationWindow();
}

void MainPage::UpdateConversationWindow()
{
	String^ selected_client_id = GetSelectedClientID();

	if (selected_client_id == nullptr) { return; }

	string_t chat_conversation_str_t = m_client_manager->get_chat_text(string_t(selected_client_id->Data()));
	String^ chat_conversation = ref new String(chat_conversation_str_t.c_str());

	chatConversationTextBlock->Text = chat_conversation;
}

void MainPage::connectButton_Click(Object^ sender, RoutedEventArgs^ e)
{
	// First disable the button and screen name text box until we connect.
	connectButton->IsEnabled = false;
	screenNameTextBox->IsEnabled = false;

	string_t screen_name;

	if (screenNameTextBox->Text != nullptr && !screenNameTextBox->Text->IsEmpty())
	{
		screen_name = string_t(screenNameTextBox->Text->Data());
	}

	// Check if we need to connect or disconnect
	task<void> conn_task = (m_client_manager->is_connected() ? m_client_manager->disconnect() : m_client_manager->connect(screen_name));

	auto current_context = task_continuation_context::use_current();
	auto background_context = task_continuation_context::use_arbitrary();

	conn_task.then([=](task<void> conn_task) {
		try
		{
			conn_task.get();

			if (m_client_manager->is_connected())
			{
				connectButton->Content = "Disconnect";
			}
			else
			{
				// Reset UI components
				m_listview_map->Clear();

				clientListView->Items->Clear();
				chatTextBox->Text = "";
				chatConversationTextBlock->Text = "";
				connectButton->Content = "Connect";
			}

			connectButton->IsEnabled = true;
			screenNameTextBox->IsEnabled = true;
		}
		catch (websocket_exception& wex)
		{
			ProjectUtilities::ShowMessage(ref new Platform::String(to_string_t(std::string(wex.what())).data()));
		}

	}, current_context);
}

void MainPage::sendButton_Click(Object^ sender, RoutedEventArgs^ e)
{
	String^ chat_text_p = chatTextBox->Text;

	if (chat_text_p == nullptr || chat_text_p->IsEmpty())
	{
		return;
	}

	// Update the client manager
	String^ selected_client_id = GetSelectedClientID();
	
	string_t sender_client_id = m_client_manager->get_local_client_info().get_client_id();
	string_t receiver_client_id(selected_client_id->Data());
	string_t chat_text(chat_text_p->Data());
	
	m_client_manager->update_chat_text(sender_client_id, receiver_client_id, chat_text);

	// Update the conversation box
	String^ chat_conversation = ref new String(m_client_manager->get_chat_text(receiver_client_id).c_str());
	chatConversationTextBlock->Text = chat_conversation;

	// Send the message to target client.
	m_client_manager->send_chat_text(receiver_client_id, chat_text);

	// Clear the chat box
	chatTextBox->Text = "";
}

void MainPage::clientListView_SelectionChanged(Object^ sender, SelectionChangedEventArgs^ e)
{
	this->UpdateConversationWindow();
}
