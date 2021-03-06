#pragma once
#include <map>
#include <mutex>
#include <vector>
#include <functional>
#include "FieldPoint.h"
#include "FieldRect.h"

class LifePool;
class Mob;
class Portal;
class PortalMap;
class TownPortalPool;
class ReactorPool;
class Reactor;
class DropPool;
class FieldSet;
class User;
class AsyncScheduler;
class InPacket;
class OutPacket;
class WvsPhysicalSpace2D;
class SummonedPool;
class AffectedAreaPool;
class FieldObj;

class Field
{
protected:
	static const int FIELD_STAT_CHANGE_PERIOD = 3 * 1000;

	struct BalloonEntry
	{
		enum BalloonType
		{
			e_Balloon_EntrustedShop = 0x02,
		};

		int nBalloonType = 0;
		int nHostX = 0, nHostY = 0;
	};

	enum FieldEffect
	{
		e_FieldEffect_Object = 0x02,
		e_FieldEffect_Screen = 0x03,
		e_FieldEffect_Sound = 0x04
	};

	std::recursive_mutex m_mtxFieldLock;
	std::map<int, User*> m_mUser; //m_lUser in WvsGame.idb
	std::map<std::string, FieldRect> m_mAreaRect;
	std::vector<std::pair<FieldPoint, bool>> m_aSeat;
	std::map<int, int> m_mUserSeat;
	int m_nFieldID = 0;
	LifePool *m_pLifePool;
	PortalMap *m_pPortalMap;
	DropPool *m_pDropPool;
	TownPortalPool *m_pTownPortalPool;
	ReactorPool *m_pReactorPool;
	SummonedPool* m_pSummonedPool;
	FieldSet* m_pParentFieldSet = nullptr;
	WvsPhysicalSpace2D* m_pSpace2D;
	FieldPoint m_ptLeftTop, m_szMap;
	AffectedAreaPool* m_pAffectedAreaPool;

	std::string m_sStreetName, 
				m_sMapName, 
				m_sWeatherMsg; //商城心情道具訊息?

	int m_nWeatherItemID, //商城心情道具物品ID?
		m_nJukeBoxItemID; //Juke Box?

	double m_dIncRate_EXP = 1.0, 
		   m_dRecoveryRate = 1.0,
		   m_dIncRate_Drop = 1.0,
			m_dMobRate = 1.0; //加倍?

	bool m_bCloud, 
		 m_bTown, 
		 m_bSwim, 
		 m_bFly;

	int m_nReturnMap,
		m_nForcedReturn,
		m_nFieldType,
		m_nFieldLimit,
		m_nAutoDecHP = 0,
		m_nAutoDecMP = 0,
		m_nProtectItem = 0,
		m_nCreateMobInterval,
		m_nFixedMobCapacity;

	unsigned int m_tLastStatChangeByField = 0;
	std::string m_strFirstUserEnter, 
				m_strUserEnter;

public:
	Field(void *pData, int nFieldID);
	~Field();

	void SetCould(bool cloud);
	bool IsCloud() const;
	void SetTown(bool town);
	bool IsTown() const;
	void SetSwim(bool swim);
	bool IsSwim() const;
	void SetFly(bool fly);
	bool IsFly() const;
	void SetFieldID(int nFieldID);
	int GetFieldID() const;
	void SetReturnMap(int returnMap);
	int GetReturnMap() const;
	void SetForcedReturn(int forcedReturn);
	int GetForcedReturn() const;
	void SetMobRate(double dMobRate);
	double GetMobRate() const;
	void SetFieldType(int fieldType);
	int GetFieldType() const;
	void SetFieldLimit(int fieldLimit);
	int GetFieldLimit() const;
	void SetCreateMobInterval(int interval);
	int GetCreateMobInterval() const;
	void SetFiexdMobCapacity(int capacity);
	int GetFixedMobCapacity() const;
	void SetFirstUserEnter(const std::string& script);
	const std::string& GetFirstUserEnter() const;
	void SetUserEnter(const std::string& script);
	const std::string& GetUserEnter() const;
	void SetMapSize(int x, int y);
	const FieldPoint& GetMapSize() const;
	void SetLeftTop(int x, int y);
	const FieldPoint& GetLeftTop() const;
	double GetIncEXPRate() const;
	void SetRecoveryRate(double dRate);
	double GetRecoveryRate() const;

	void SetFieldSet(FieldSet *pFieldSet);
	FieldSet *GetFieldSet();

	void InitLifePool();
	LifePool *GetLifePool();
	DropPool *GetDropPool();

	virtual void OnEnter(User *pUser);
	virtual void OnLeave(User *pUser);

	//發送oPacket給該地圖的其他User，其中pExcept是例外對象
	void SplitSendPacket(OutPacket* oPacket, User* pExcept);
	void BroadcastPacket(OutPacket* oPacket);
	void BroadcastPacket(OutPacket* oPacket, std::vector<int>& anCharacterID);
	void RegisterFieldObj(FieldObj *pNew, OutPacket *oPacketEnter);

	void OnMobMove(User* pCtrl, Mob* pMob, InPacket* iPacket);
	virtual void OnPacket(User* pUser, InPacket* iPacket);
	void OnUserMove(User* pUser, InPacket *iPacket);

	PortalMap* GetPortalMap();
	TownPortalPool* GetTownPortalPool();
	ReactorPool* GetReactorPool();
	SummonedPool* GetSummonedPool();
	std::recursive_mutex& GetFieldLock();
	WvsPhysicalSpace2D* GetSpace2D();
	AffectedAreaPool* GetAffectedAreaPool();
	const std::map<int, User*>& GetUsers();

	virtual void AddCP(int nLastDamageCharacterID, int nAddCP);
	void TransferAll(int nFieldID, const std::string& sPortal);
	void LoadAreaRect(void *pData);
	int CountFemaleInArea(const std::string& sArea);
	int CountMaleInArea(const std::string& sArea);
	int CountUserInArea(const std::string& sArea);
	void EffectScreen(const std::string& sEffect);
	void EffectScreen(const std::string& sEffect, std::vector<int>& anCharacterID);
	void EffectSound(const std::string& sEffect);
	void EffectSound(const std::string& sEffect, std::vector<int>& anCharacterID);
	void EffectObject(const std::string& sEffect);
	void EnablePortal(const std::string& sPortal, bool bEnable);
	void OnContiMoveState(User *pUser, InPacket *iPacket);
	bool OnSitRequest(User *pUser, int nSeatID);

	virtual void OnReactorDestroyed(Reactor *pReactor);
	void CheckReactorAction(const std::string& sReactorName, unsigned tEventTime);
	virtual void Reset(bool bShuffleReactor);
	virtual void OnStatChangeByField(unsigned int tCur);
	virtual void Update();
};

