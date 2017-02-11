/*======================================================================
 *[TCPBuilder.hpp]
 *
 *Copyright (c) [2016] [Yamada Syoya]
 *
 *This software is released under the MIT License.
 *http://opensource.org/licenses/mit-license.php
 *
 *======================================================================
 */

#pragma once
#include "TCP.hpp"
class TCPBuilder
{
public:
	explicit TCPBuilder();
	~TCPBuilder();
	TCPBuilder& setIoService(boost::asio::io_service& ioService);
	TCPBuilder& setPort(int port);
	TCPBuilder& setTimeout(int time);
	TCPBuilder& setCS(TCP::CS cs);
	TCPBuilder& setMatchCondition(const std::string& matchConditon);
	TCPBuilder& setAddress(const std::string& address);
	TCPBuilder& setCanSend(bool canSend);
	TCPBuilder& setCanReceive(bool canReceive);
	int getTimeout();
	int getPort();
	TCP::CS getCS();
	TCP::RECEIVE_TYPE getReceiveType();
	const std::string& getMatchConditon();
	const std::string& getAddress();
	boost::shared_ptr<TCP> build();
private:
	bool _isSetIoService;
	bool _isSetTimeout;
	bool _isSetPort;
	bool _isSetAddress;
	bool _isSetMatchCondition;
	bool _isSetCS;
	bool _isSetReceiveType;
	bool _canSend;
	bool _canReceive;
	int _readByte;
	int _time;
	int _port;
	TCP::RECEIVE_TYPE _receiveType;
	TCP::CS _cs;
	const std::string* _matchCondition;
	const std::string* _address;
	boost::asio::io_service* _ioService;
};

TCPBuilder::TCPBuilder()
	:_isSetIoService(false),
	_isSetTimeout(false),
	_isSetPort(false),
	_isSetAddress(false),
	_isSetMatchCondition(false),
	_isSetCS(false),
	_isSetReceiveType(false),
	_canSend(false),
	_canReceive(false),
	_readByte(0),
	_time(5),
	_port(0),
	_receiveType(TCP::RECEIVE_TYPE::UNTIL),
	_cs(TCP::CS::CLIENT),
	_matchCondition(nullptr),
	_address(nullptr),
	_ioService(nullptr)
{
}


TCPBuilder::~TCPBuilder()
{
}

TCPBuilder& TCPBuilder::setIoService(boost::asio::io_service& ioService)
{
	_isSetIoService = true;
	_ioService = &ioService;
	return *this;
}

TCPBuilder& TCPBuilder::setPort(int port)
{
	_isSetPort = true;
	_port = port;
	return *this;
}

TCPBuilder& TCPBuilder::setTimeout(int time)
{
	_isSetTimeout = true;
	_time = time;
	return *this;
}

TCPBuilder& TCPBuilder::setCS(TCP::CS cs)
{
	_isSetCS = true;
	_cs = cs;
	return *this;
}

TCPBuilder& TCPBuilder::setMatchCondition(const std::string& matchConditon)
{
	_isSetMatchCondition = true;
	_matchCondition = &matchConditon;
	return *this;
}

TCPBuilder& TCPBuilder::setAddress(const std::string& address)
{
	_isSetAddress = true;
	_address = &address;
	return *this;
}


TCPBuilder& TCPBuilder::setCanSend(bool canSend)
{
	_canSend = canSend;
	return *this;
}
TCPBuilder& TCPBuilder::setCanReceive(bool canReceive)
{
	_canReceive = canReceive;
	return *this;
}

int TCPBuilder::getTimeout()
{
	return _time;
}

int TCPBuilder::getPort()
{
	return _port;
}

TCP::CS TCPBuilder::getCS()
{
	return  _cs;
}

TCP::RECEIVE_TYPE TCPBuilder::getReceiveType()
{
	return _receiveType;
}

const std::string& TCPBuilder::getMatchConditon()
{
	return *_matchCondition;
}

const std::string& TCPBuilder::getAddress()
{
	return *_address;
}

boost::shared_ptr<TCP> TCPBuilder::build()
{
	boost::shared_ptr<TCP> tcp;
	if (!_isSetIoService) {

	}
	if (!_isSetCS) {

	}

	if (!_isSetPort) {

	}

	if (!_isSetReceiveType) {

	}

	if (!_isSetAddress) {
		tcp = boost::shared_ptr<TCP>(new TCP(*_ioService,_port, _receiveType, _cs));
	}else{
		tcp = boost::shared_ptr<TCP>(new TCP(*_ioService, *_address, _port, _receiveType, _cs));
	}

	if (!_isSetMatchCondition) {

	}
	tcp->setMatchCondition(*_matchCondition);
	if (!_isSetTimeout) {

	}
	tcp->setTimeout(_time);
	tcp->setCanReceive(_canReceive);
	tcp->setCanSend(_canSend);
	return tcp;
}
