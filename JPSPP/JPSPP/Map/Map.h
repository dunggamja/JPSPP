#pragma once
#include "Global.h"
#include "Tile/Tile.h"




class Map : public std::enable_shared_from_this<Map>
{
public:
	using SharedPtr = std::shared_ptr<Map>;
	using WeakPtr = std::weak_ptr<Map>;
	
private:
	int m_SizeX = 0;
	int m_SizeY = 0;



	//타일정보는 좌->우, 상->하로 순서로 적재된다. 
	//ex)  m_SizeX = 3, m_SizeY = 3 일때 
	//0 1 2 
	//3 4 5
	//6 7 8
	std::vector<Tile::SharedPtr>	m_TileRepos;

public:
	Map(int sizeX, int sizeY)
		: m_SizeX(sizeX)
		, m_SizeY(sizeY)
	{ }

private:
	inline int GetRepoIdx(int posX, int posY) const 
	{ 
		if (posY < 0 || m_SizeY <= posY
		||  posX < 0 || m_SizeX <= posX)
			return -1;

		return posY * m_SizeX + posX; 
	}
	inline std::tuple<int, int> GetXY(size_t idx) const { return std::make_tuple( static_cast<int>(idx % m_SizeX), static_cast<int>(idx / m_SizeX)); }

public:
	inline int GetSizeX() const { return m_SizeX; }
	inline int GetSizeY() const { return m_SizeY; }

	Tile* GetTile(int posX, int posY) const
	{
		int idx = GetRepoIdx(posX, posY);
		if (idx < 0 || (int)m_TileRepos.size() <= idx) return nullptr;


		return m_TileRepos[idx].get();
	}

	decltype(m_TileRepos)& GetTileRepo() { return m_TileRepos; }


//점프포인트 생성 & 후처리 관련 해야할일 (순차적으로 실행되어야 한다.)
//1.각 노드에서 방향 플래그를 설정하여 모든 기본 점프 지점을 식별합니다.
//2. 지도 왼쪽에서 오른쪽으로 스캔하면서 모든 서쪽 방향 직진 점프 지점과 서쪽 방향 벽의 거리로 표시한다.
//3. 지도 오른쪽에서 왼쪽으로 스캔하면서 모든 동쪽 방향 직진 점프 지점과 동쪽 방향 벽의 거리로 표시한다.
//4. 지도를 위쪽에서 아래쪽으로 스캔하면서 모든 모든 북쪽 방향 직진 점프 지점과 북쪽 방향 벽의 거리로 표시한다.
//5. 지도를 아래쪽에서 위쪽으로 스캔하면서 모든 남쪽 방향으로 직선 점프 지점과 남쪽 방향 벽의 거리를 표시한다.
//6. 지도를 아래쪽에서 위쪽으로 스캔하면서 남서/남동 대각선 점프지점과 남서/남동의 벽의 거리를 표시한다.
//7. 지도를 위쪽에서 아래쪽으로 스캔하면서 북서/북동 대각선 점프지점과 북서/북동의 벽의 거리로 표시한다.
public:
	void InitializeTileRepository();
	void ProcessUpdateJumpPoint();
private:
	void UpdateJumpPoint();
	void UpdateDistance_Straight(GAME::DIRECTIONS direction);
	void UpdateDistance_Diagonal(GAME::DIRECTIONS direction);	

public:
	void DisplayMap_OnConsole();
	void SetObstacle(int x, int y);


public:
	std::vector<GAME::POS_INFO> FindPath(GAME::POS_INFO startPos, GAME::POS_INFO endPos) const;

private:	
	struct NODE_INFO
	{
		Tile*				m_pTile					= nullptr;
		NODE_INFO*			m_pParent				= nullptr;
		float				m_AccumDistance			= 0.f;
		float				m_ExpectTotalDistance	= 0.f;
		GAME::DIRECTIONS	m_Dir					= GAME::DIRECTIONS::INVALID;
		

		NODE_INFO() = default;
		NODE_INFO(Tile* pTile, NODE_INFO* pParent, float accumDistance, float expectTotalDistance, GAME::DIRECTIONS dir)
			: m_pTile(pTile)
			, m_pParent(pParent)
			, m_AccumDistance(accumDistance)
			, m_ExpectTotalDistance(expectTotalDistance)
			, m_Dir(dir)
		{}

		using UniquePtr = std::unique_ptr<NODE_INFO>;
	};

	

	bool						CheckPos2PosConnected(int startX, int startY, int endX, int endY) const;
	std::list<GAME::POS_INFO>	FindNodeForCheckConnect(int startX, int startY, int endX, int endY) const;
	std::vector<GAME::POS_INFO> CreatePath(const NODE_INFO* pNodeInfo, int endX, int endY) const;
	GAME::DIRECTIONS			CalcDirectionPos2Pos(int startX, int startY, int endX, int endY) const;
};
