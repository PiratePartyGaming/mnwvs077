#include "WvsShop.h"
#include <thread>
#include <iostream>
#include <functional>
#include "User.h"
#include "ClientSocket.h"
#include "..\WvsLib\Net\OutPacket.h"
#include "..\WvsLib\Task\AsyncScheduler.h"
#include "..\WvsLib\Logger\WvsLogger.h"
#include "..\WvsLib\Common\ConfigLoader.hpp"
#include "..\WvsLib\Common\ServerConstants.hpp"
#include "..\WvsLib\DateTime\GameDateTime.h"

WvsShop::WvsShop()
{
}


WvsShop::~WvsShop()
{
}

std::recursive_mutex& WvsShop::GetUserLock()
{
	return m_mUserLock;
}

std::map<int, ZSharedPtr<User>>& WvsShop::GetConnectedUser()
{
	return m_mUserMap;
}

std::shared_ptr<Center>& WvsShop::GetCenter()
{
	return m_pCenterInstance;
}

void WvsShop::SetConfigLoader(ConfigLoader * pCfg)
{
	m_pCfgLoader = pCfg;
}

void WvsShop::ConnectToCenter()
{
	int nCenterIdx = 0;
	m_pCenterInstance->SetSocketDisconnectedCallBack(std::bind(&Center::OnNotifyCenterDisconnected, m_pCenterInstance.get()));
	m_pCenterInstance->SetCenterIndex(nCenterIdx);
	m_pCenterInstance->Connect(
		m_pCfgLoader->StrValue("Center" + std::to_string(nCenterIdx) + "_IP"),
		m_pCfgLoader->IntValue("Center" + std::to_string(nCenterIdx) + "_Port")
	);

}

void WvsShop::CenterAliveMonitor()
{
	if (m_pCenterInstance->CheckSocketStatus(SocketBase::SocketStatus::eConnecting))
		return;
	WvsLogger::LogRaw(WvsLogger::LEVEL_WARNING, "--------------------Periodically reconnecting to WvsCenter--------------------\n");
	int centerSize = m_pCfgLoader->IntValue("CenterCount");
	if (m_pCenterInstance && m_pCenterInstance->CheckSocketStatus(SocketBase::SocketStatus::eClosed))
	{
		WvsLogger::LogFormat("Failed connecting to Center Server %d, reconnection has been scheduled.\n", 0);
		m_pCenterWorkThread->detach();
		*m_pCenterWorkThread = (std::thread(&WvsShop::ConnectToCenter, this));
	}
}

void WvsShop::InitializeCenter()
{
	m_pCenterServerService = (new asio::io_service());
	m_pCenterInstance = std::make_shared<Center>(*m_pCenterServerService);
	m_pCenterWorkThread = (new std::thread(&WvsShop::ConnectToCenter, this));
	auto fHolderFunc = std::bind(&WvsShop::CenterAliveMonitor, this);
	auto pAliveHolder = AsyncScheduler::CreateTask(fHolderFunc, 10 * 100, true);
	pAliveHolder->Start();


	std::thread tCenterWorkThread([&] {
		asio::io_service::work work(*m_pCenterServerService);
		std::error_code ec;
		m_pCenterServerService->run(ec);
	});
	tCenterWorkThread.detach();
}

ZSharedPtr<User> WvsShop::FindUser(int nUserID)
{
	std::lock_guard<std::recursive_mutex> lockGuard(m_mUserLock);
	auto findIter = m_mUserMap.find(nUserID);
	return (findIter == m_mUserMap.end() ? nullptr : findIter->second);
}

const std::pair<unsigned int, unsigned int>& WvsShop::GetMigratingUser(int nUserID)
{
	static std::pair<unsigned int, unsigned int> prEmpty = { 0, 0 };
	std::lock_guard<std::recursive_mutex> lockGuard(m_mUserLock);
	auto findIter = m_mMigratingUser.find(nUserID);
	return findIter == m_mMigratingUser.end() ? prEmpty : findIter->second;
}

void WvsShop::OnUserMigrating(int nUserID, int nSocketID)
{
	std::lock_guard<std::recursive_mutex> lockGuard(m_mUserLock);
	m_mMigratingUser.insert({ nUserID,{ nSocketID, GameDateTime::GetTime() } });
}

void WvsShop::RemoveMigratingUser(int nUserID)
{
	std::lock_guard<std::recursive_mutex> lockGuard(m_mUserLock);
	m_mMigratingUser.erase(nUserID);
}

void WvsShop::OnUserConnected(ZSharedPtr<User>& pUser)
{
	std::lock_guard<std::recursive_mutex> lockGuard(m_mUserLock);
	m_mUserMap[pUser->GetUserID()] = pUser;
}

void WvsShop::OnNotifySocketDisconnected(SocketBase * pSocket)
{
	std::lock_guard<std::recursive_mutex> lockGuard(m_mUserLock);
	auto pClient = (ClientSocket*)pSocket;
	pClient->OnSocketDisconnected();
	if (pClient->GetUser())
	{
		m_mUserMap.erase(pClient->GetUser()->GetUserID());
		pClient->SetUser(nullptr);
	}
}

void WvsShop::ShutdownService()
{
}
