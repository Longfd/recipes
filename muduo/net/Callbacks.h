// this is a public header file, it must only include public header file

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "../base/TimeStamp.h"

#include <functional>
#include <memory>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

// All clients visible 
class Buffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void()> TimerCallback;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void (const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;

// the data has been read to (buf, len)
//typedef std::function<void (const TcpConnectionPtr&,
//							Buffer*,
//							Timestamp)> MessageCallback;
typedef std::function<void (const TcpConnectionPtr&,
							const char*,
							size_t)> MessageCallback;

void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn,
							Buffer* buffer,
							Timestamp receiveTime);

#endif // CALLBACKS_H















