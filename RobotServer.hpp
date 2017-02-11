/*======================================================================
 *[RobotServer.hpp]
 *
 *Copyright (c) [2016] [Yamada Syoya]
 *
 *This software is released under the MIT License.
 *http://opensource.org/licenses/mit-license.php
 *
 *======================================================================
 */

#pragma once
#ifndef ROBOT_SERVER_H
#define ROBOT_SERVER_H

#include "TCP.hpp"
#include "TCPBuilder.hpp"
#include "SimpleMapDeSerializer.hpp"
#include "SimpleMapSerializer.hpp"
#include <map>

namespace network{
class RobotServer {
public:
	explicit RobotServer(boost::shared_ptr<TCP> tcp, std::map<std::string, std::map<std::string, int>>& serverData, boost::mutex& mtx,boost::asio::io_service& ioService);
	explicit RobotServer(const int port, const int timeout, const std::string matchCondition, std::map<std::string, std::map<std::string, int> >& serverData, boost::mutex& mtx,boost::asio::io_service& ioService);
	~RobotServer();
	void setByServer(std::map<std::string, int>& data,std::string ip);
	void set(std::string data);
	void accept();
	std::map<std::string, int> get(std::string ip);
	void returnData(std::string data);
	void waitingClient();
	bool isFailed();
	std::string getClientIP();
private:
	bool _isTransferFailed;
	const std::string _prefixSet;
	const std::string _prefixGet;
	const std::string _emptyStr;
	const std::string _notFoundStr;
	boost::mutex& _mtx;
	boost::shared_ptr<TCP> _tcp;
	boost::asio::io_service& _ioService;
	std::map<std::string,std::map<std::string,int>>& _serverData;
	std::string getPrefix(std::string data, char delim);
	std::string deletePrefix(std::string data,char delim);
	std::string deleteMatchCondition(std::string data);
};

RobotServer::RobotServer(boost::shared_ptr<TCP> tcp, std::map<std::string, std::map<std::string, int>>& serverData, boost::mutex& mtx,boost::asio::io_service& ioService)
	: _tcp(tcp),
	_serverData(serverData),
	_isTransferFailed(false),
	_prefixSet("set"),
	_prefixGet("get"),
	_emptyStr(""),
	_notFoundStr("notfound"),
	_mtx(mtx),
	_ioService(ioService)

{
	_tcp->setSendData(_emptyStr + _tcp->getMatchCondition());
}

RobotServer::RobotServer(const int port, const int timeout, const std::string matchCondition, std::map<std::string, std::map<std::string, int> >& serverData, boost::mutex& mtx,boost::asio::io_service& ioService)
	:_serverData(serverData),
	_mtx(mtx),
	_isTransferFailed(false),
	_prefixSet("set"),
	_prefixGet("get"),
	_emptyStr(""),
	_notFoundStr("notfound,1"),
	_ioService(ioService)
{
	TCPBuilder builder;
	_tcp = builder.setCanReceive(true).setCanSend(true).setCS(TCP::CS::SERVER).setMatchCondition(matchCondition).setTimeout(timeout).setPort(port).setIoService(_ioService).build();
	_tcp->setSendData(_emptyStr + _tcp->getMatchCondition());
	_serverData["dummy"]["port"] = port;
}

RobotServer::~RobotServer() {

}

void RobotServer::accept() {
	_tcp->start();
}

void  RobotServer::set(std::string data) {
	if(data.size() == 0){
		return;
	}
	boost::mutex::scoped_lock lock(_mtx);
	SimpleMapSerialization::simpleMapDeSerializer(deletePrefix(data,','),',',_serverData[_tcp->getClientIP()]);
}

void  RobotServer::setByServer(std::map<std::string, int>& data,std::string ip) {
	 for(auto itr = data.begin(); itr != data.end(); ++itr) {
		 _serverData[ip][itr->first] = itr->second;
	 }
}

std::map<std::string, int> RobotServer::get(std::string ip) {
	boost::mutex::scoped_lock lock(_mtx);
	return _serverData[ip];
}

void RobotServer::returnData(std::string data) {
	std::string ip = deleteMatchCondition(deletePrefix(data, ','));
	if(_serverData.find(ip) == _serverData.end()){
		_tcp->setSendData(_notFoundStr + _tcp->getMatchCondition());
	}
	else {
		std::string sendData;
		{
			boost::mutex::scoped_lock lock(_mtx);
			sendData = SimpleMapSerialization::simpleMapSerializer(_serverData[ip], ',');
		}
		_tcp->setSendData(sendData + _tcp->getMatchCondition());
	}
	_tcp->transfer();
}

void RobotServer::waitingClient(){
	_tcp->transfer();
	if (_tcp->getReceiveData() == "") {
		return;
	}
	std::string receiveData = _tcp->getReceiveData();
	if(receiveData.size() == 0){
		return;
	}
	std::string prefix = getPrefix(receiveData, ',');
	if("set" == prefix){
		set(receiveData);
	}
	if("get" == prefix){
		returnData(receiveData);
	}
}

bool RobotServer::isFailed(){
	return _isTransferFailed;
}

std::string RobotServer::getClientIP() {
	return _tcp->getClientIP();
}

std::string RobotServer::getPrefix(std::string data,char delim) {
	return data.substr(0, data.find_first_of(delim));
}

std::string RobotServer::deletePrefix(std::string data, char delim) {
	return data.substr(data.find_first_of(delim) + 1);
}

std::string RobotServer::deleteMatchCondition(std::string data) {
	return data.substr(0,data.size()-_tcp->getMatchCondition().size());
}

}
#endif //ROBOT_SERVER_H
