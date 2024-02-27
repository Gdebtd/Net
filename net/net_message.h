#ifndef NET_MESSAGE_H
#define NET_MESSAGE_H

#include "net_common.h"

namespace net {
	struct message {
		//uint32_t uid; // user
		//uint32_t hid; // header
		uint32_t hid; // header id
		//uint32_t hsize; // header size
		//uint32_t count = 0;
		//std::vector<char> body;
		//std::vector<uint8_t> body;
		std::vector<double> body;

		friend std::ostream& operator << (std::ostream& os, const message& msg) {
			os/* << msg.uid << " " */<< msg.hid << " " << msg.body.size();
			//os/* << msg.uid << " " */<< msg.hid << " " << msg.body.size() << " " << msg.count;
			for (unsigned int i = 0; i < msg.body.size(); i++) {
				os << " " << msg.body[i];
			}
			return os;
		}

		friend std::istream& operator >> (std::istream& is, message& msg) {
			uint32_t size;
			is/* >> msg.uid */>> msg.hid >> size;
			//is/* >> msg.uid */>> msg.hid >> size >> msg.count;
			msg.body = std::vector<double>(size);
			for (unsigned int i = 0; i < size; i++) {
				is >> msg.body[i];
			}
			return is;
		}

		//size_t size() const {
		//	return sizeof(uint32_t) + sizeof(uint32_t) + body.size();
		//}

		//template <typename DataType>
		//friend message& operator << (message& msg, const DataType& data) {
		//	static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

		//	// cache current size of vector
		//	size_t i = msg.body.size();

		//	// resize the vector by the size of the data being pushed
		//	msg.body.resize(msg.body.size() + sizeof(DataType));

		//	// physically copy the data into the newly allocated vector space
		//	std::memcpy(msg.body.data() + i, &data, sizeof(DataType));

		//	// recalculate the message size
		//	msg.hsize = msg.size();

		//	msg.count++;

		//	// return the target message so it can be "chained"
		//	return msg;
		//}

		//template<typename DataType>
		//friend message& operator >> (message& msg, DataType& data) {
		//	static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");

		//	// cache the location towards the end of the vector where the pulled data starts
		//	size_t i = msg.body.size() - sizeof(DataType);

		//	// physically copy the data from the vector into the user variable
		//	std::memcpy(&data, msg.body.data() + i, sizeof(DataType));

		//	// shrink the vector to remove read bytes and reset end position
		//	msg.body.resize(i);

		//	// recalculate the message size
		//	msg.hsize = msg.size();

		//	msg.count--;

		//	// return the target message so it can be "chained"
		//	return msg;
		//}
	};

	template <typename T>
	class connection;

	template <typename T>
	struct owned_message {
		std::shared_ptr<connection<T>> remote = nullptr;
		message msg;

		friend std::ostream& operator << (std::ostream& os, const owned_message<T>& msg) {
			os << msg.msg;
			return os;
		}
	};
}

#endif /* NET_MESSAGE_H */