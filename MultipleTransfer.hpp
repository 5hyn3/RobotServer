/*======================================================================
 *[MultipleTransfer.hpp]
 *
 *Copyright (c) [2016] [Yamada Syoya]
 *
 *This software is released under the MIT License.
 *http://opensource.org/licenses/mit-license.php
 *
 *======================================================================
 */

#pragma once
#ifndef MULTIPLE_TRANSFER_H
#define MULTIPLE_TRANSFER_H

#include "RobotServer.hpp"

namespace network{
class MultipleTransfer {
public:
	explicit MultipleTransfer(int size,int port,std::map<std::string,std::map<std::string,int>>& data,boost::mutex& mtx);
	~MultipleTransfer();
	void start();
	void stop();
private:
	void accepter();
	void transfer(network::RobotServer& server,std::map<std::string,std::map<std::string,int>>& data,std::string ip,boost::mutex& mtx);
	bool _stop;
	int _count;
	int _port;

	std::vector<boost::shared_ptr<std::thread>> _threadVec;
	std::vector<boost::shared_ptr<boost::asio::io_service>> _iosVec;
	std::vector<boost::shared_ptr<network::RobotServer>> _serverVec;

	std::map<std::string,std::map<std::string,int>>& _serverData;

	std::thread _accepterThread;

	boost::mutex& _mtx;
};

MultipleTransfer::MultipleTransfer(int size,int port,std::map<std::string,std::map<std::string,int>>& data,boost::mutex& mtx):
		_stop(false),
		_port(port),
		_serverData(data),
		_mtx(mtx)
{
	_threadVec.resize(size);
	_iosVec.resize(size);
	_serverVec.resize(size);
}

MultipleTransfer::~MultipleTransfer() {
	stop();
}

void MultipleTransfer::start(){
	_stop = false;
	_accepterThread = std::thread(&MultipleTransfer::accepter,this);
}

void MultipleTransfer::stop(){
    if(_stop == true){
        return;
    }
	_stop = true;
	for(int i = 0;i < _count;i++){
		_threadVec[i]->join();
	}
	_accepterThread.join();
}


void MultipleTransfer::accepter(){
	while(_stop == false){
		boost::shared_ptr<boost::asio::io_service> iosPtr(new boost::asio::io_service);
		_iosVec.push_back(iosPtr);
		boost::shared_ptr<network::RobotServer> serverPtr(new network::RobotServer(_port,10,"\n",_serverData,_mtx,*iosPtr));
		_serverVec.push_back(serverPtr);
		std::cout << "start accepting... " << std::endl;
		serverPtr->accept();
		std::string ip(serverPtr->getClientIP());
		std::cout << "accepted:" << ip << std::endl;
		_threadVec.push_back(boost::shared_ptr<std::thread>(new std::thread(&MultipleTransfer::transfer,this,std::ref(*serverPtr),std::ref(_serverData),ip,std::ref(_mtx))));
		_count++;
	}
}

void MultipleTransfer::transfer(network::RobotServer& server,std::map<std::string,std::map<std::string,int>>& data,std::string ip,boost::mutex& mtx){
	while (_stop == false) {
		try{
			server.waitingClient();
		}catch(...){
			{
				boost::mutex::scoped_lock lock(mtx);
				data.erase(ip);
			}
			std::cout << "leave:" << ip <<std::endl;
			return;
		}
	}
}

}
#endif //MULTIPLE_TRANSFER_H
