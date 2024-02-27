#include "net/net.h"

double getTime() {
	return std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

class CustomClient : public net::client_interface<CustomMsgTypes> {
public:
	void PingServer() {
		net::message msg;
		msg.hid = (uint32_t)CustomMsgTypes::ServerPing;
		//std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
		double timeNow = getTime();
		//msg << timeNow;
		msg.body.push_back(timeNow);
		m_connection->Send(msg);
	}

	void MessageAll(std::vector<double>& data) {
		net::message msg;
		msg.hid = (uint32_t)CustomMsgTypes::MessageAll;
		/*for (unsigned int i = 0; i < data.size(); i++) {
			msg << data[i];
		}*/
		msg.body = data;
		m_connection->Send(msg);
	}

	void MessageAllFloat(double x) {
		std::vector<double> data = { x };
		MessageAll(data);
	}
	void MessageAllVector2(double x, double y) {
		std::vector<double> data = { x, y };
		MessageAll(data);
	}
	void MessageAllVector3(double x, double y, double z) {
		std::vector<double> data = { x, y, z };
		MessageAll(data);
	}
	void MessageAllArray(std::vector<double>& data) {
		MessageAll(data);
	}
	void MessageAllText(std::vector<double>& data) {
		MessageAll(data);
	}
};

int main(int argc, char* argv[]) {
	std::string ip;
	uint16_t port;
	std::cout << "Enter ip: "; std::cin >> ip;
	std::cout << "Enter port: "; std::cin >> port;
	CustomClient client;
	client.Connect(ip, port); // "127.0.0.1", 7777

	bool key[3] = { false, false, false };
	bool old_key[3] = { false, false, false };

	bool bQuit = false;
	while (!bQuit) {
		if (GetForegroundWindow() == GetConsoleWindow()) {
			key[0] = GetAsyncKeyState('1') & 0x8000;
			key[1] = GetAsyncKeyState('2') & 0x8000;
			key[2] = GetAsyncKeyState('3') & 0x8000;
		}

		if (key[0] && !old_key[0]) {
			client.PingServer();
		}
		if (key[1] && !old_key[1]) {
			uint32_t size;
			std::cout << "Enter count of numbers: "; std::cin >> size;
			std::cout << "Enter your " << size << " numbers: ";
			std::vector<double> data(size);
			for (unsigned int i = 0; i < size; i++) {
				std::cin >> data[i];
			}
			client.MessageAllArray(data);
		}
		if (key[2] && !old_key[2]) {
			bQuit = true;
		}

		for (int i = 0; i < 3; i++) old_key[i] = key[i];

		if (client.IsConnected()) {
			if (!client.Incoming().empty()) {
				net::message msg = client.Incoming().pop_front().msg;
				switch (msg.hid)
				{
				case (uint32_t)CustomMsgTypes::ServerAccept:
				{
					std::cout << "Server Accepted Connection\n";
				}
				break;

				case (uint32_t)CustomMsgTypes::ServerPing:
				{
					//std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
					double timeNow = getTime();
					//double timeThen;
					//msg >> timeThen;
					double timeThen = msg.body[0];
					std::cout << "Ping: " << (timeNow - timeThen) << "\n";
				}
				break;

				case (uint32_t)CustomMsgTypes::ServerMessage:
				{
					//uint32_t clientID;
					//msg >> clientID;
					uint32_t clientID = (uint32_t)msg.body[0];
					//std::cout << "Hello from [" << clientID << "]\n";
					std::cout << "Message from [" << clientID << "]: ";
					for (unsigned int i = 0; i < msg.body.size() - 1; i++) {
						std::cout << msg.body[i + 1] << " ";
					}
					/*while (msg.count) {
						double chunk;
						msg >> chunk;
						std::cout << " " << chunk;
					}*/
					std::cout << "\n";
				}
				break;
				}
			}
		}
		else {
			std::cout << "Server Down\n";
			bQuit = true;
		}
	}
	system("pause");
	return 0;
}