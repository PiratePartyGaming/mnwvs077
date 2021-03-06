#include "WvsBase.h"
#include <mutex>
#include "..\Logger\WvsLogger.h"
#include "..\String\StringUtility.h"
#include "..\String\StringPool.h"

std::map<unsigned int, SocketBase*> WvsBase::m_mSocketList;

WvsBase::WvsBase()
{
}

WvsBase::~WvsBase()
{
}

asio::io_service& WvsBase::GetIOService()
{
	return m_IOService;
}

const std::map<unsigned int, SocketBase*>& WvsBase::GetSocketList() const
{
	return m_mSocketList;
}

SocketBase * WvsBase::GetSocket(unsigned int nSocketID)
{
	auto findIter = m_mSocketList.find(nSocketID);
	return findIter == m_mSocketList.end() ? nullptr : findIter->second;
}

void WvsBase::Init()
{

}

void WvsBase::CreateAcceptor(short nPort)
{
	WvsLogger::LogFormat(WvsLogger::LEVEL_INFO, "[WvsBase::CreateAcceptor]WvsApp server instance is successfully initialized and listening on port %d.\n", nPort);
	asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), nPort);
	m_pAcceptor = new asio::ip::tcp::acceptor(m_IOService, endpoint);
}

void WvsBase::OnSocketConnected(SocketBase *pSocket)
{
	m_mSocketList.insert({ pSocket->GetSocketID(), pSocket });
	pSocket->SetSocketDisconnectedCallBack(std::bind(&WvsBase::OnSocketDisconnected, this, std::placeholders::_1));
}

void WvsBase::OnSocketDisconnected(SocketBase *pSocket)
{
	auto findIter = m_mSocketList.find(pSocket->GetSocketID());
	if (findIter == m_mSocketList.end())
		return;
	WvsLogger::LogFormat(WvsLogger::LEVEL_WARNING, "[WvsBase::OnSocketDisconnected]Socket is disconnected from server [Socket ID : %u].\n", pSocket->GetSocketID());
	OnNotifySocketDisconnected(pSocket);
	m_mSocketList.erase(pSocket->GetSocketID());
}

void WvsBase::OnNotifySocketDisconnected(SocketBase *pSocket)
{
	//WvsBase is an ADT, so won't become recursive call.
	this->OnNotifySocketDisconnected(pSocket);
}

int* WvsBase::GetExternalIP() const
{
	return (int*)m_aExternalIP;
}

short WvsBase::GetExternalPort() const
{
	return m_nExternalPort;
}

void WvsBase::SetExternalIP(const std::string& ip)
{
	std::vector<std::string> aIP;
	StringUtility::Split(ip, aIP, ".");
	for (int i = 0; i < 4; ++i)
		m_aExternalIP[i] = atoi(aIP[i].c_str());
}

void WvsBase::SetExternalPort(short nPort)
{
	m_nExternalPort = nPort;
}