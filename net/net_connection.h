#ifndef NET_CONNECTION_H
#define NET_CONNECTION_H

#include "net_common.h"
#include "net_message.h"
#include "net_tsqueue.h"

namespace net {
	template <typename T>
	class connection : public std::enable_shared_from_this<connection<T>> {
	public:
		enum class owner {
			server,
			client
		};

		connection(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, tsqueue<owned_message<T>>& qIn)
			: m_asioContext(asioContext), m_asioSocket(std::move(socket)), m_qMessagesIn(qIn) {
			m_nOwnerType = parent;
		}

		virtual ~connection() {}

		uint32_t GetID() const {
			return id;
		}

	public:
		void ConnectToClient(uint32_t uid = 0) {
			if (m_nOwnerType == owner::server) {
				if (m_asioSocket.is_open()) {
					id = uid;
					ReadData();
				}
			}
		}

		void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints) {
			if (m_nOwnerType == owner::client) {
				asio::async_connect(m_asioSocket, endpoints,
					[this](std::error_code ec, asio::ip::tcp::endpoint endpoint) {
						if (!ec) {
							ReadData();
						}
					});
			}
		}

		void Disconnect() {
			if (IsConnected()) {
				asio::post(m_asioContext, [this]() { m_asioSocket.close(); });
			}
		}

		bool IsConnected() const {
			return m_asioSocket.is_open();
		}

	public:
		void Send(message& msg) {
			asio::post(m_asioContext,
				[this, msg]() {
					WriteData(msg);
				});
		}

	private:
		//std::vector<char> vBuffer = std::vector<char>(1 * 1024, '\0');
		std::vector<double> vBuffer = std::vector<double>(1 * 1024);
		//std::string vBuffer = std::string(1 * 512, '\0');
		void ReadData() {
			try {
				m_asioSocket.async_read_some(asio::buffer(vBuffer),
					[this](std::error_code ec, std::size_t length) {
						if (!ec) {
							AddToIncomingMessageQueue(vBuffer, length);
							//std::string data(vBuffer.data(), length);
							//AddToIncomingMessageQueue(data);
							//AddToIncomingMessageQueue(vBuffer, length);
						}
						else {
							std::cout << "[" << id << "] Read Fail.\n";
							m_asioSocket.close();
						}
					}
				);
			}
			catch (std::exception& e) {
				std::cerr << "Exception while reading: " << e.what() << "\n";
				return;
			}
		}

		void WriteData(const message& msg) {
			try {
				std::vector<double> buff = msg.body;
				std::vector<double> add_data = { double(msg.hid), double(msg.body.size()) };
				buff.insert(buff.begin(), add_data.begin(), add_data.end());
				asio::mutable_buffers_1 buffer = asio::buffer(buff);
				m_asioSocket.send(buffer);
				//asio::streambuf sb;
				//std::ostream out(&sb);
				//out << msg << "\n";
				//std::stringstream ss;
				//ss << out.rdbuf();
				//std::string str = ss.str();
				////std::vector<char> data;
				////data.insert(data.end(), str.begin(), str.end());
				////m_asioSocket.send(asio::buffer(data));
				////data.clear();
				//for (unsigned int i = 0; i < str.length(); i++) {
				//	d[i] = str[i];
				//}
				//m_asioSocket.send(asio::buffer(d.data(), str.length()));
				/*asio::streambuf sb;
				std::ostream os(&sb);
				os << msg << "\n";
				// try sending some data in input sequence
				size_t n = m_asioSocket.send(sb.data());
				sb.consume(n); // sent data is removed from input sequence*/

				/*std::vector<uint8_t> buff = msg.body;
				buff.insert(buff.begin(), double(msg.body.size()));
				buff.insert(buff.begin(), double(msg.hid));
				asio::mutable_buffers_1 buffer = asio::buffer(buff);
				m_asioSocket.send(buffer);*/

				//std::vector<char> data = { '0', ' ', '0', '\n' };
				/*message _msg = msg;
				std::vector<char> data;
				add_char(_msg, '0');
				add_char(_msg, ' ');
				add_char(_msg, '0');
				add_char(_msg, '\n');
				m_asioSocket.send(asio::buffer(_msg.body));*/
			}
			catch (std::exception& e) {
				std::cerr << "Exception while writing: " << e.what() << "\n";
				return;
			}
		}

		//void AddToIncomingMessageQueue(size_t length) {
		//	std::vector<char> data = vBuffer;
		//	std::cout << "Len: " << length << "\n";
		//	std::vector<message> msgs;
		//	msgs.push_back({ 0 });
		//	/*std::string line;
		//	std::istringstream is(data);
		//	while (std::getline(is, line)) {
		//		asio::streambuf buff;
		//		std::ostream out(&buff);
		//		out << line;
		//		std::istream in(&buff);
		//		message msg;
		//		in >> msg;
		//		msgs.push_back(msg);
		//	}*/
		//	for (unsigned int j = 0; j < msgs.size(); j++) {
		//		if (m_nOwnerType == owner::server) {
		//			m_qMessagesIn.push_back({ this->shared_from_this(), msgs[j] });
		//		}
		//		else {
		//			m_qMessagesIn.push_back({ nullptr, msgs[j] });
		//		}
		//	}
		//	ReadData();
		//}

		/*void AddToIncomingMessageQueue(std::string& data) {
			std::vector<message> msgs;
			std::string line;
			std::istringstream is(data);
			while (std::getline(is, line)) {
				asio::streambuf buff;
				std::ostream out(&buff);
				out << line;
				std::istream in(&buff);
				message msg;
				in >> msg;
				msgs.push_back(msg);
			}
			for (unsigned int j = 0; j < msgs.size(); j++) {
				if (m_nOwnerType == owner::server) {
					m_qMessagesIn.push_back({ this->shared_from_this(), msgs[j] });
				}
				else {
					m_qMessagesIn.push_back({ nullptr, msgs[j] });
				}
			}
			ReadData();
		}*/

		void AddToIncomingMessageQueue(std::vector<double>& data, size_t length) {
			length /= sizeof(double);
			std::vector<message> msgs;
			message msg;
			unsigned int i = 0;
			while (i < length) {
				msg.hid = (uint32_t)data[i++];
				uint32_t size = (uint32_t)data[i++];
				msg.body = std::vector<double>(size);
				unsigned int ind = 0;
				while (ind < size) {
					msg.body[ind++] = data[i++];
				}
				msgs.push_back(msg);
				msg = {};
			}
			for (unsigned int j = 0; j < msgs.size(); j++) {
				if (m_nOwnerType == owner::server) {
					m_qMessagesIn.push_back({ this->shared_from_this(), msgs[j] });
				}
				else {
					m_qMessagesIn.push_back({ nullptr, msgs[j] });
				}
			}
			ReadData();
		}
	protected:
		asio::ip::tcp::socket m_asioSocket;
		asio::io_context& m_asioContext;
		tsqueue<owned_message<T>>& m_qMessagesIn;
		owner m_nOwnerType = owner::server;
		uint32_t id = 0;
	};
}

#endif /* NET_CONNECTION_H */