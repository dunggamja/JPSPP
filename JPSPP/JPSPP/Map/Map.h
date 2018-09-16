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



	//Ÿ�������� ��->��, ��->�Ϸ� ������ ����ȴ�. 
	//ex)  m_SizeX = 3, m_SizeY = 3 �϶� 
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
	inline size_t GetRepoIdx(int posX, int posY) const { return posY * m_SizeX + posX; }
	inline std::tuple<int, int> GetXY(size_t idx) const { return std::make_tuple( static_cast<int>(idx % m_SizeX), static_cast<int>(idx / m_SizeX)); }

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


//��������Ʈ ���� & ��ó�� ���� �ؾ����� (���������� ����Ǿ�� �Ѵ�.)
//1.�� ��忡�� ���� �÷��׸� �����Ͽ� ��� �⺻ ���� ������ �ĺ��մϴ�.
//2. ���� ���ʿ��� ���������� ��ĵ�ϸ鼭 ��� ���� ���� ���� ���� ������ ���� ���� ���� �Ÿ��� ǥ���Ѵ�.
//3. ���� �����ʿ��� �������� ��ĵ�ϸ鼭 ��� ���� ���� ���� ���� ������ ���� ���� ���� �Ÿ��� ǥ���Ѵ�.
//4. ������ ���ʿ��� �Ʒ������� ��ĵ�ϸ鼭 ��� ��� ���� ���� ���� ���� ������ ���� ���� ���� �Ÿ��� ǥ���Ѵ�.
//5. ������ �Ʒ��ʿ��� �������� ��ĵ�ϸ鼭 ��� ���� �������� ���� ���� ������ ���� ���� ���� �Ÿ��� ǥ���Ѵ�.
//6. ������ �Ʒ��ʿ��� �������� ��ĵ�ϸ鼭 ����/���� �밢�� ���������� ����/������ ���� �Ÿ��� ǥ���Ѵ�.
//7. ������ ���ʿ��� �Ʒ������� ��ĵ�ϸ鼭 �ϼ�/�ϵ� �밢�� ���������� �ϼ�/�ϵ��� ���� �Ÿ��� ǥ���Ѵ�.
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
	std::vector<GAME::POS_INFO> FindPath(GAME::POS_INFO startPos, GAME::POS_INFO endPos);
};
