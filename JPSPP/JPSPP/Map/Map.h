#pragma once
#include "Global.h"

class Tile;

class Map
{
private:
	int m_SizeX = 0;
	int m_SizeY = 0;



	//타일정보는 좌->우, 상->하로 순서로 적재된다. 
	//ex)
	//0 1 2 
	//3 4 5
	//6 7 8
	std::vector<std::unique_ptr<Tile>>	m_TileRepos;



private:
	inline size_t GetRepoIdx(int posX, int posY) const { return posY * m_SizeX + posX; }

public:
	inline int GetSizeX() const { return m_SizeX; }
	inline int GetSizeY() const { return m_SizeY; }

	Tile* GetTile(int posX, int posY) const
	{
		size_t idx = GetRepoIdx(posX, posY);
		if (idx < 0 || m_TileRepos.size() <= idx) return nullptr;


		return m_TileRepos[idx].get();
	}

	decltype(m_TileRepos)& GetTileRepo() { return m_TileRepos; }

private:
//점프포인트 생성 & 후처리 관련 해야할일 (순차적으로 실행되어야 한다.)
//1.각 노드에서 방향 플래그를 설정하여 모든 기본 점프 지점을 식별합니다.
//2. 지도 왼쪽에서 오른쪽으로 스캔하면서 모든 서쪽 방향 직진 점프 지점과 서쪽 방향 벽의 거리로 표시한다.
//3. 지도 오른쪽에서 왼쪽으로 스캔하면서 모든 동쪽 방향 직진 점프 지점과 동쪽 방향 벽의 거리로 표시한다.
//4. 지도를 위쪽에서 아래쪽으로 스캔하면서 모든 모든 북쪽 방향 직진 점프 지점과 북쪽 방향 벽의 거리로 표시한다.
//5. 지도를 아래쪽에서 위쪽으로 스캔하면서 모든 남쪽 방향으로 직선 점프 지점과 남쪽 방향 벽의 거리를 표시한다.
//6. 지도를 아래쪽에서 위쪽으로 스캔하면서 남서/남동 대각선 점프지점과 남서/남동의 벽의 거리를 표시한다.
//7. 지도를 위쪽에서 아래쪽으로 스캔하면서 북서/북동 대각선 점프지점과 북서/북동의 벽의 거리로 표시한다.
	void ProcessUpdateJumpPoint();
	void UpdateAllJumpPoint();
	void UpdateStraightDistance(GAME::DIRECTIONS direction);
	void UpdateDiagonalDistance(GAME::DIRECTIONS direction);
};