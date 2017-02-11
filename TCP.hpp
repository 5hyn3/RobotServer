/*======================================================================
 *[TCP.hpp]
 *
 *Copyright (c) [2016] [Yamada Syoya]
 *
 *This software is released under the MIT License.
 *http://opensource.org/licenses/mit-license.php
 *
 *======================================================================
 */

#ifndef TCP_BOOST_H
#define TCP_BOOST_H

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "Communication.hpp"

class TCP :public Communication {
public:
	enum CS {
		SERVER,
		CLIENT
	};
	enum RECEIVE_TYPE {
		UNTIL,
		MORE,
		ALL
	};
	explicit TCP(boost::asio::io_service& io_service, const std::string& address, const int port, const RECEIVE_TYPE _receivetype, const CS cs);
	explicit TCP(boost::asio::io_service& ioService, const int port, const RECEIVE_TYPE receiveType, const CS cs);
	~TCP();
	void start();
	void reStart();
	bool getIsAcceptFailed();
	bool getIsSendFailed();
	void send();
	void receive();
	bool getIsTransferFailed();
	int getPort();
    std::string getClientIP();
	std::string getMatchCondition();
private:
	bool _isTransferFailed;
	bool _isAcceptFailed;
	bool _isSendFailed;
	const int _port;
	std::string _address;
	boost::shared_ptr<boost::asio::ip::tcp::socket> _socket;
	const RECEIVE_TYPE _receiveType;
	const CS _cs;
	void connect();
	void onConnect(const boost::system::error_code& error);
	void startAccept();
	void onAccept(const boost::system::error_code& error);
	void onSend(const boost::system::error_code& error, size_t bytes_transferred);
	void onReceive(const boost::system::error_code& error, size_t bytes_transferred);
	void onTimer(const boost::system::error_code& error);
};

TCP::TCP(boost::asio::io_service& ioService, const std::string& address, const int port, const RECEIVE_TYPE receiveType, const CS cs)
	: Communication(ioService),
	_isTransferFailed(false),
	_isAcceptFailed(false),
	_isSendFailed(false),
	_port(port),
	_address(address),
	_receiveType(receiveType),
	_cs(cs)
{
	boost::shared_ptr<boost::asio::ip::tcp::socket> socketTemp(new boost::asio::ip::tcp::socket(ioService));
	_socket = socketTemp;
}

TCP::TCP(boost::asio::io_service& ioService, const int port, const RECEIVE_TYPE receiveType, const CS cs)
	: Communication(ioService),
	_isTransferFailed(false),
	_isAcceptFailed(false),
	_isSendFailed(false),
	_port(port),
	_receiveType(receiveType),
	_cs(cs)
{
	boost::shared_ptr<boost::asio::ip::tcp::socket> socketTemp(new boost::asio::ip::tcp::socket(ioService));
	_socket = socketTemp;
}


TCP::~TCP()
{
	_socket->close();
}

void TCP::start()
{
	switch (_cs) {
	case SERVER:
		startAccept();
		break;
	case CLIENT:
		connect();
		break;
	}
}

void TCP::reStart()
{
	_ioService.reset();
	_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both);
	_socket->close();
	_socket.reset(new boost::asio::ip::tcp::socket(_ioService));
	start();
}

void TCP::connect()
{
	_socket->async_connect(
		boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(_address), _port),
		boost::bind(&TCP::onConnect, this, boost::asio::placeholders::error));
}

void TCP::onConnect(const boost::system::error_code& error)
{
	if (error) {
		std::cout << "connect failed : " << error.message() << std::endl;
		_isTransferFailed = true;
		return;
	}
	_isTransferFailed = false;
}

void TCP::startAccept()
{
	boost::asio::ip::tcp::acceptor _acceptor(_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), _port));
	_acceptor.async_accept(
		*_socket,
		boost::bind(&TCP::onAccept, this, boost::asio::placeholders::error));
	_ioService.run();
}

void TCP::onAccept(const boost::system::error_code& error)
{
	if (error) {
		std::cout << "accept failed: " << error.message() << std::endl;
		_isAcceptFailed = true;
		_isTransferFailed = true;
		return;
	}
	_isTransferFailed = false;
	_isAcceptFailed = false;
}

bool TCP::getIsAcceptFailed()
{
	return _isAcceptFailed;
}

bool TCP::getIsSendFailed()
{
	return _isSendFailed;
}

void TCP::send()
{
	boost::asio::async_write(
		*_socket,
		boost::asio::buffer(_sendData),
		boost::bind(&TCP::onSend, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void TCP::onSend(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error) {
		std::cout << "send failed: " << error.message() << std::endl;
		_isSendFailed = true;
		_isTransferFailed = true;
		if (_cs == TCP::CS::SERVER) {

		}
	}
	else {
	}
	_isTransferFailed = false;
}

void TCP::receive()
{
	switch (_receiveType) {
	case UNTIL:
		boost::asio::async_read_until(//error
			*_socket,
			_receiveBuff,
			_matchCondition,
			boost::bind(&TCP::onReceive, this,
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		break;
	case MORE:
		boost::asio::async_read(
			*_socket,
			_receiveBuff,
			boost::asio::transfer_at_least(_readByte),
			boost::bind(&TCP::onReceive, this,
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		break;
	case ALL:
		boost::asio::async_read(
			*_socket,
			_receiveBuff,
			boost::asio::transfer_all(),
			boost::bind(&TCP::onReceive, this,
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		break;
	}
	_timer.expires_from_now(boost::posix_time::seconds(_time));
	_timer.async_wait(boost::bind(&TCP::onTimer, this, _1));//error
}

void TCP::onReceive(const boost::system::error_code& error, size_t bytes_transferred)
{
	if (error == boost::asio::error::operation_aborted) {
		std::cout << "timeout" << std::endl;
		_isTransferFailed = true;
		return;
	}
	else {
		_timer.cancel();
		_isCanceled = true;
		if (error) {
			std::cout << "error : " << error.message() << std::endl;
			_isTransferFailed = true;
			return;
		}
		else {
			std::string temp = boost::asio::buffer_cast<const char*>(_receiveBuff.data());
			_receiveDataStr = temp.substr(0, bytes_transferred);
			_receiveBuff.consume(bytes_transferred);
		}
		_isTransferFailed = false;
	}
}

void TCP::onTimer(const boost::system::error_code& error)
{
	if (!error && !_isCanceled) {
		_socket->cancel();
	}
}

bool TCP::getIsTransferFailed()
{
	return _isTransferFailed;
}

inline std::string TCP::getClientIP()
{
    return _socket->remote_endpoint().address().to_string();
}

inline int TCP::getPort()
{
	return _port;
}

inline std::string TCP::getMatchCondition()
{
	return _matchCondition;
}

#endif //TCP_BOOST_H
