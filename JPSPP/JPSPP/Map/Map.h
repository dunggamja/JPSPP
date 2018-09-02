#pragma once
#include "Global.h"

class Tile;

class Map
{
private:
	int m_SizeX = 0;
	int m_SizeY = 0;



	//Ÿ�������� ��->��, ��->�Ϸ� ������ ����ȴ�. 
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
//��������Ʈ ���� & ��ó�� ���� �ؾ����� (���������� ����Ǿ�� �Ѵ�.)
//1.�� ��忡�� ���� �÷��׸� �����Ͽ� ��� �⺻ ���� ������ �ĺ��մϴ�.
//2. ���� ���ʿ��� ���������� ��ĵ�ϸ鼭 ��� ���� ���� ���� ���� ������ ���� ���� ���� �Ÿ��� ǥ���Ѵ�.
//3. ���� �����ʿ��� �������� ��ĵ�ϸ鼭 ��� ���� ���� ���� ���� ������ ���� ���� ���� �Ÿ��� ǥ���Ѵ�.
//4. ������ ���ʿ��� �Ʒ������� ��ĵ�ϸ鼭 ��� ��� ���� ���� ���� ���� ������ ���� ���� ���� �Ÿ��� ǥ���Ѵ�.
//5. ������ �Ʒ��ʿ��� �������� ��ĵ�ϸ鼭 ��� ���� �������� ���� ���� ������ ���� ���� ���� �Ÿ��� ǥ���Ѵ�.
//6. ������ �Ʒ��ʿ��� �������� ��ĵ�ϸ鼭 ����/���� �밢�� ���������� ����/������ ���� �Ÿ��� ǥ���Ѵ�.
//7. ������ ���ʿ��� �Ʒ������� ��ĵ�ϸ鼭 �ϼ�/�ϵ� �밢�� ���������� �ϼ�/�ϵ��� ���� �Ÿ��� ǥ���Ѵ�.
	void ProcessUpdateJumpPoint();
	void UpdateAllJumpPoint();
	void UpdateStraightDistance(GAME::DIRECTIONS direction);
	void UpdateDiagonalDistance(GAME::DIRECTIONS direction);
};