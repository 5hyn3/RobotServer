/*======================================================================
 *[SimpleMapDeSerializer.hpp]
 *
 *Copyright (c) [2016] [Yamada Syoya]
 *
 *This software is released under the MIT License.
 *http://opensource.org/licenses/mit-license.php
 *
 *======================================================================
 */

#pragma once
#ifndef SIMPLE_MAP_DESERIALIZER_HPP
#define SIMPLE_MAP_DESERIALIZER_HPP

#include <string>
#include <map>
#include <deque>

namespace SimpleMapSerialization {
	template <class T, class U>
	void simpleMapDeSerializer(const std::string &str, char delim, std::map<T, U>& result) {
		std::deque<std::string> res;
		size_t current = 0, found;
		while ((found = str.find_first_of(delim, current)) != std::string::npos) {
			res.push_back(std::string(str, current, found - current));
			current = found + 1;
		}
		res.push_back(std::string(str, current, str.size() - current));
		for (std::deque<std::string>::iterator it = res.begin(); it != res.end(); it += 2) {
			std::stringstream ss1(*it);
			std::stringstream ss2(*(it + 1));
			T key;
			U value;
			ss1 >> key;
			ss2 >> value;
			result[key] = value;
		}
	}
}

#endif //SIMPLE_MAP_DESERIALIZER_HPP
