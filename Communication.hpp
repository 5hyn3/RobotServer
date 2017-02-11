/*======================================================================
 *[Communication.hpp]
 *
 *Copyright (c) [2016] [Yamada Syoya]
 *
 *This software is released under the MIT License.
 *http://opensource.org/licenses/mit-license.php
 *
 *======================================================================
 */

#pragma once
#ifndef COMMUNICATION_HPP
#define COMMUNICATION_HPP

#include <boost/asio.hpp>
class Communication
{
public:
	Communication(boost::asio::io_service& ioService);
	virtual ~Communication();
	virtual void start() = 0;
	void transfer();
	virtual void setSendData(const std::string& sendData);
	void setMatchCondition(const std::string& matchConditon);
	void setReadByte(int byte);
	void setTimeout(int time);
	void setCanSend(bool canSend);
	void setCanReceive(bool canReceive);
	void ioServiceRun();
	std::string getReceiveData() const;
	//	boost::asio::streambuf getRawReceiveData() const;
protected:
	bool _canSend;
	bool _canReceive;
	bool _isCanceled;
	int _readByte;
	int _time;
	std::string _matchCondition;
	std::string _sendData;
	std::string _receiveDataStr;
	boost::asio::streambuf _receiveBuff;
	boost::asio::deadline_timer _timer;
	boost::asio::io_service& _ioService;
	virtual void send() = 0;
	virtual void onSend(const boost::system::error_code& error, size_t bytes_transferred) = 0;
	virtual void receive() = 0;
	virtual void onReceive(const boost::system::error_code& error, size_t bytes_transferred) = 0;
	virtual void onTimer(const boost::system::error_code& error) = 0;
};

Communication::Communication(boost::asio::io_service& ioService)
	:_canSend(false),
	_canReceive(false),
	_isCanceled(false),
	_readByte(0),
	_time(0),
	_ioService(ioService),
	_timer(ioService)
{

}


Communication::~Communication()
{

}

void Communication::transfer()
{
	_ioService.reset();
	if (_canSend) {
		send();
	}
	if (_canReceive) {
		receive();
	}
	_ioService.run();
}

void Communication::setSendData(const std::string& sendData)
{
	_sendData = sendData;
}

void Communication::setMatchCondition(const std::string& matchCondition)
{
	_matchCondition = matchCondition;
}
void Communication::setTimeout(int time)
{
	_time = time;
}
void Communication::setReadByte(int byte)
{
	_readByte = byte;
}

void Communication::setCanSend(bool canSend)
{
	_canSend = canSend;
}

void Communication::setCanReceive(bool canReceive)
{
	_canReceive = canReceive;
}

std::string Communication::getReceiveData() const
{
	return _receiveDataStr;
}

void Communication::ioServiceRun()
{
	_ioService.run();
}


/*
boost::asio::streambuf Communication::getRawReceiveData() const
{
return _receiveBuff;
}
*/

#endif //COMMUNICATION_HPP
