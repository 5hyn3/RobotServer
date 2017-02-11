/*======================================================================
 *[SimpleMapSerializer.hpp]
 *
 *Copyright (c) [2016] [Yamada Syoya]
 *
 *This software is released under the MIT License.
 *http://opensource.org/licenses/mit-license.php
 *
 *======================================================================
 */

#pragma once
#ifndef SIMPLE_MAP_SERIALIZER_HPP
#define SIMPLE_MAP_SERIALIZER_HPP

#include <string>
#include <map>

namespace SimpleMapSerialization {
	template <class T, class U>
	std::string simpleMapSerializer(std::map<T, U> map, char delim)
	{
		std::stringstream ss;
		for (typename std::map<T, U>::iterator mapIte = map.begin(); mapIte != map.end(); mapIte++) {
			T key = mapIte->first;
			U val = mapIte->second;
			ss << key << delim << val << delim;
		}
		std::string str = ss.str();
		if (str == "") {
			return str;
		}
		str.resize(str.size() - 1);
		return str;
	}
}

#endif //SIMPLE_MAP_SERIALIZER_HPP
