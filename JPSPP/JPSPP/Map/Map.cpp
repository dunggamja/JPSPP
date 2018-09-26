#include "Map.h"

void Map::ProcessUpdateJumpPoint()
{
	//��������Ʈ ����
	UpdateJumpPoint();

	//�����Ÿ� ���.
	for (size_t idx = Enum2Num(GAME::DIRECTIONS::STRAIGHT_BEGIN); idx < Enum2Num(GAME::DIRECTIONS::STRAIGHT_END); ++idx)
	{
		UpdateDistance_Straight((GAME::DIRECTIONS)idx);
	}

	//�밢���Ÿ� ���.
	UpdateDistance_Diagonal(GAME::DIRECTIONS::TOP);
	UpdateDistance_Diagonal(GAME::DIRECTIONS::BOTTOM);
}


void Map::UpdateJumpPoint()
{
	auto CalcTileType = [&](const Tile* pTileInfo)->GAME::TILE_TYPE
	{
		if (nullptr == pTileInfo)
			return GAME::TILE_TYPE::INVALID;

		return (pTileInfo->IsObstacle()) ? GAME::TILE_TYPE::OBSTACLE : GAME::TILE_TYPE::EMPTY;
	};

	for (auto& tileInfo : m_TileRepos)
	{
		tileInfo->UpdateJumpPoint();
	}
}



void Map::InitializeTileRepository()
{
	m_TileRepos.clear();
	m_TileRepos.resize(m_SizeX * m_SizeY);

	for (size_t i = 0; i < m_TileRepos.size(); ++i)
	{
		int x = 0;
		int y = 0;
		std::tie(x, y) = GetXY(i);
		m_TileRepos[i] = std::make_shared<Tile>(weak_from_this(), x, y);
	}
}

void Map::DisplayMap_OnConsole()
{
	auto GetCharacter = [](Tile* pTile)->char 
	{
		if (pTile->IsJumpPoint())
			return 'J';
		else if (pTile->IsObstacle())
			return 'X';
		else
			return 'O';
	};

	for (int y = 0; y < m_SizeY; ++y)
	{
		for (int x = 0; x < m_SizeX; ++x)
		{
			auto pTile = GetTile(x, y);
			if(nullptr != pTile)
			{
				std::cout << GetCharacter(pTile);
			}
		}
		std::cout << std::endl;
	}
}

void Map::SetObstacle(int x, int y)
{
	auto pTile = GetTile(x, y);
	if (pTile)
		pTile->SetIsObstacle(true);
}

void Map::UpdateDistance_Straight(GAME::DIRECTIONS direction)
{
	if (!IsStraightDirection(direction)) return;

	bool isHorizonward = (GAME::DIRECTIONS::LEFT == direction || GAME::DIRECTIONS::RIGHT == direction);

	auto calcDistance = [&](int posX, int posY, GAME::DIRECTIONS distanceDir)->int
	{
		int x = 0, y = 0;
		std::tie(x, y) = GetNeighborPos(posX, posY, distanceDir);

		auto pTile = GetTile(x, y);

		//1.��ֹ� 0
		//2.��������Ʈ 1
		//3.��ֹ� �Ǵ� ��������Ʈ���� �Ÿ� -1 / +1

		if (IS_NULL(pTile) || pTile->IsObstacle())
		{
			return 0;
		}
		else if (pTile->IsJumpPointAcessFrom(GetReverseDirection(distanceDir)))
		{
			return 1;
		}
		else
		{
			int distValue = pTile->GetDistanceInfo(distanceDir);
			return (0 < distValue) ? distValue + 1 : distValue - 1;
		}


		return 0; 
	};

	if(isHorizonward)
	{
		concurrency::parallel_for(0, m_SizeY, [&](int y)
		{
			bool isRightWard = (GAME::DIRECTIONS::LEFT == direction);

			const int startX	= isRightWard ? 0 : m_SizeX - 1;
			const int endX		= isRightWard ? m_SizeX : -1;
			const int addX		= isRightWard ? 1 : -1;

			for (int x = startX; x != endX; x += addX)
			{
				auto pTile = GetTile(x, y);
				if (IS_NOT_NULL(pTile))
					pTile->SetDistanceInfo(direction, calcDistance(x, y, direction));
			}
		});
	}
	else
	{ 
		concurrency::parallel_for(0, m_SizeX, [&](int x)
		{
			bool isDownWard = (GAME::DIRECTIONS::TOP == direction);

			const int startY = isDownWard ? 0 : m_SizeY - 1;
			const int endY = isDownWard ? m_SizeY : -1;
			const int addY = isDownWard ? 1 : -1;

			for (int y = startY; y != endY; y += addY)
			{
				auto pTile = GetTile(x, y);
				if (IS_NOT_NULL(pTile))
					pTile->SetDistanceInfo(direction, calcDistance(x, y, direction));
			}
		});
	}

}

void Map::UpdateDistance_Diagonal(GAME::DIRECTIONS direction)
{
	bool isDownWard = (GAME::DIRECTIONS::TOP == direction);

	std::initializer_list<GAME::DIRECTIONS> diagonalDirs;
	if (isDownWard) diagonalDirs = {GAME::DIRECTIONS::LEFTTOP, GAME::DIRECTIONS::RIGHTTOP};
	else            diagonalDirs = {GAME::DIRECTIONS::LEFTBOTTOM, GAME::DIRECTIONS::RIGHTBOTTOM};

	auto calcDistance = [&](int posX, int posY, GAME::DIRECTIONS distanceDir)->int 
	{
		int x = 0, y = 0;
		std::tie(x, y) = GetNeighborPos(posX, posY, distanceDir);
		auto pTile = GetTile(x, y);

		GAME::DIRECTIONS reverseDir = GetReverseDirection(distanceDir);

		//1.��ֹ� 0
		//2.��������Ʈ 1, 
		//3.��������Ʈ�� ����� ���� 1
		//4.��ֹ� �Ǵ� ��������Ʈ���� �Ÿ� -1 / +1


		//������̺�.
		auto& lookUpDirs = GetLookUpDirs(distanceDir);
		auto& reverseLookUpDirs = GetLookUpDirs(reverseDir);

		//��ֹ�üũ. 
		auto checkObstacle = [&]()->bool
		{
			return std::any_of(lookUpDirs.cbegin(), lookUpDirs.cend(), [&](const GAME::DIRECTIONS& dir)->bool
			{
				auto neighborXY = GetNeighborPos(posX, posY, dir);
				auto pNeighborTile = GetTile(std::get<0>(neighborXY), std::get<1>(neighborXY));
				return (IS_NULL(pNeighborTile) || pNeighborTile->IsObstacle());
			});
		};

		//��������Ʈ üũ
		auto checkJumpPoint = [&](const Tile* pTargetTile)->bool 
		{
			if (IS_NULL(pTargetTile)) 
				return false;

			return std::any_of(reverseLookUpDirs.cbegin(), reverseLookUpDirs.cend(), [&](const GAME::DIRECTIONS dir)->bool 
			{
				return pTargetTile->IsJumpPointAcessFrom(dir);
			});
		};

		//��������Ʈ�� ����� ����üũ.
		auto checkJumpPointLine = [&](const Tile* pTargetTile)->bool
		{
			if (IS_NULL(pTargetTile)) 
				return false;

			return std::any_of(lookUpDirs.cbegin(), lookUpDirs.cend(), [&](const GAME::DIRECTIONS dir)->bool 
			{
				if (IsDiagonalDirection(dir))
					return false;
				
				return (0 < pTargetTile->GetDistanceInfo(dir));
			});
		};

		

		//1.��ֹ� 0
		//2.��������Ʈ 1, 
		//3.��������Ʈ�� ����� ���� 1
		//4.��ֹ� �Ǵ� ��������Ʈ���� �Ÿ� -1 / +1
		if (IS_NULL(pTile) || checkObstacle())
		{
			return 0;
		}
		else if (checkJumpPoint(pTile) || checkJumpPointLine(pTile))
		{
			return 1;
		}
		else
		{
			int distValue = pTile->GetDistanceInfo(distanceDir);
			return (0 < distValue) ? distValue + 1 : distValue - 1;
		}

		return 0;
	};

	int startY = isDownWard ? 0 : m_SizeY - 1;
	int endY   = isDownWard ? m_SizeY : -1;
	int addY   = isDownWard ? 1 : -1;

	for(int y = startY; y != endY; y += addY)
	{ 
		for (int x = 0; x < m_SizeX; ++x)
		{
			for (auto& dir : diagonalDirs)
			{
				auto pTile = GetTile(x, y);
				if (IS_NOT_NULL(pTile))
				{
					pTile->SetDistanceInfo(dir, calcDistance(x, y, dir));
				}
			}
		}
	}

}

std::vector<GAME::POS_INFO> Map::FindPath(GAME::POS_INFO startPos, GAME::POS_INFO endPos) const
{
	auto pTileStart = GetTile(std::get<GAME::POS_INFO_X>(startPos), std::get<GAME::POS_INFO_Y>(startPos));
	auto pTileEnd   = GetTile(std::get<GAME::POS_INFO_X>(endPos), std::get<GAME::POS_INFO_Y>(endPos));

	std::vector<GAME::POS_INFO> vecPaths;

	

	std::list<NODE_INFO::UniquePtr> closelist;
	std::list<NODE_INFO::UniquePtr> openlist;
	if (IS_NULL(pTileStart) 
	||  IS_NULL(pTileEnd) 
	||  pTileStart->IsObstacle() 
	||  pTileEnd->IsObstacle()
	||  startPos == endPos)
		return std::vector<GAME::POS_INFO>();



	openlist.emplace_back(std::make_unique<NODE_INFO>(pTileStart, nullptr, 0.f, 0.f, GAME::DIRECTIONS::INVALID));

	//0.��ǥ�����̸� ����.
	//1.��ǥ������ �������� �̾����� ������ �ִ��� Ž��.  �ش��������� ��ǥ�������� �̾����� Ž������.
	//2.�ش��������� �������� ���� �ִ� ������ ��ǥ������ ���������� �̾����� ������ �ִ��� Ž���մϴ�. openlist�� �߰�.
	//3.�� �������� ���ٰ����� ��������Ʈ or ��������Ʈ ���� �������� openlist�� �־��ݴϴ�. 
	//  �̹� closelist�� ���� ������ ���ܵ˴ϴ�.
	//  �̹� openlist�� �ִٸ� �Ÿ� �� �� �־��ݴϴ�. 
	
	//Ž���� ��������� closelist�� �־��ش�. 
	
	do 
	{
		//���� �Ÿ��� ª�� ��带 ã�Ƽ� openlist���� ����, closelist�� �߰�.
		{
			auto iterFind = openlist.begin();
			for (auto iterS = openlist.begin(); iterS != openlist.end(); ++iterS)
			{
				if ((*iterS)->m_ExpectTotalDistance < (*iterFind)->m_ExpectTotalDistance)
					iterFind = iterS;
			}

			closelist.emplace_back(std::move(*iterFind));
			openlist.erase(iterFind);
		}

		auto& nodeInfo = closelist.back();
		auto  pTile = nodeInfo->m_pTile;
		if (IS_NOT_NULL(nodeInfo) && IS_NOT_NULL(pTile))
		{
			//1.
			if (CheckPos2PosConnected(pTile->GetPosX(), pTile->GetPosY(), pTileEnd->GetPosX(), pTileEnd->GetPosY()))
			{
				return CreatePath((nodeInfo.get()), pTileEnd->GetPosX(), pTileEnd->GetPosY());
			}

			//2.
			auto listForCheckConnect = FindNodeForCheckConnect(pTile->GetPosX(), pTile->GetPosY(), pTileEnd->GetPosX(), pTileEnd->GetPosY());
			for(auto& posInfo : listForCheckConnect)
			{
				int tempX = std::get<GAME::POS_INFO_X>(posInfo);
				int tempY = std::get<GAME::POS_INFO_Y>(posInfo);

				if (CheckPos2PosConnected(pTile->GetPosX(), pTile->GetPosY(), tempX, tempY)
				&&  CheckPos2PosConnected(tempX, tempY, pTileEnd->GetPosX(), pTileEnd->GetPosY()))
				{
					int diffX1 = pTile->GetPosX() - tempX;
					int diffY1 = pTile->GetPosY() - tempY;
					int diffX2 = pTileEnd->GetPosX() - tempX;
					int diffY2 = pTileEnd->GetPosY() - tempY;

					float distanceDiff1 = std::sqrtf((float)(diffX1 * diffX1 + diffY1 * diffY1));
					float distanceDiff2 = std::sqrtf((float)(diffX2 * diffX2 + diffY2 * diffY2));

					auto pTempTile = GetTile(tempX, tempY);
					if (IS_NOT_NULL(pTempTile))
						openlist.emplace_back(std::make_unique<NODE_INFO>(
							  pTempTile
							, nodeInfo.get()
							, nodeInfo->m_AccumDistance + distanceDiff1
							, nodeInfo->m_AccumDistance + distanceDiff1 + distanceDiff2
							, CalcDirectionPos2Pos(pTile->GetPosX(), pTile->GetPosY(), tempX, tempY)));
				}
			}

			//3.
			for (auto& lookDir : GetLookUpDirs(nodeInfo->m_Dir))
			{
				int distValue = pTile->GetDistanceInfo(lookDir);
				
				if (0 < distValue)
				{
					int dirX = 0;
					int dirY = 0;
					std::tie(dirX, dirY) = GetAdjacentDir(lookDir);

					dirX *= distValue;
					dirY *= distValue;

					auto pAddTile = GetTile(pTile->GetPosX() + dirX, pTile->GetPosY() + dirY);
					if (nullptr != pAddTile)
					{
						if(std::find_if(closelist.begin(), closelist.end(), [&](const NODE_INFO::UniquePtr& tempNode)->bool
						{
							return (IS_NOT_NULL(tempNode)
								&& IS_NOT_NULL(tempNode->m_pTile)
								&& tempNode->m_pTile->GetPosX() == pAddTile->GetPosX()
								&& tempNode->m_pTile->GetPosY() == pAddTile->GetPosY());
						}) != closelist.end())
							continue;

						auto iterFind_Open	= std::find_if(openlist.begin(), openlist.end(), [&](const NODE_INFO::UniquePtr& tempNode)->bool 
						{
							return (IS_NOT_NULL(tempNode)
								 && IS_NOT_NULL(tempNode->m_pTile)
								 && tempNode->m_pTile->GetPosX() == pAddTile->GetPosX()
								 && tempNode->m_pTile->GetPosY() == pAddTile->GetPosY());
						});

						int distanceToGoalX = pAddTile->GetPosX() - pTileEnd->GetPosX();
						int distanceToGoalY = pAddTile->GetPosY() - pTileEnd->GetPosY();
						
						float distanceToAdjacent	= std::sqrtf((float)(dirX * dirX + dirY * dirY));
						float distanceToGoal		= std::sqrtf((float)(distanceToGoalX * distanceToGoalX + distanceToGoalY * distanceToGoalY));

						float newAccumDistance = nodeInfo->m_AccumDistance + distanceToAdjacent;
						float newTotalDistance = newAccumDistance + distanceToGoal;
						
						if (iterFind_Open != openlist.end())
						{
							if (newTotalDistance < (*iterFind_Open)->m_ExpectTotalDistance)
							{
								(*iterFind_Open)->m_pParent = nodeInfo.get();
								(*iterFind_Open)->m_Dir = lookDir;
								(*iterFind_Open)->m_AccumDistance = newAccumDistance;
								(*iterFind_Open)->m_ExpectTotalDistance = newTotalDistance;
							}
						}
						else
						{
							openlist.emplace_back(std::make_unique<NODE_INFO>(
								  pAddTile
								, nodeInfo.get()
								, newAccumDistance
								, newTotalDistance
								, lookDir));
						}
					}
				}
			}
		}
	}while (!openlist.empty());

	return std::vector<GAME::POS_INFO>();
}


//�� ������ ���� �̾������� üũ�մϴ�. 
bool Map::CheckPos2PosConnected(int startX, int startY, int endX, int endY) const
{
	//1.������ �´��� üũ.
	//2.������ ������ startXY -> endXY ���� ���µ� ��ֹ��� �ִ��� üũ.

	int diffX = endX - startX;
	int diffY = endY - startY;
	int distToGoal = std::max(std::abs(diffX), std::abs(diffY));

	//������ �����̸� ��ֹ� ����!
	if (diffX == 0 && diffY == 0)
		return true;

	const GAME::DIRECTIONS dir = CalcDirectionPos2Pos(startX, startY, endX, endY);

	//�������� 8���� �������� ������ �Ǵ��� üũ�Ѵ�. �ƴϸ� ��ȿ���� ���� �����̴�.
	if (dir == GAME::DIRECTIONS::INVALID)
		return false;


	//�����ִ� ����� �߰��Ҷ����� ��������� Ž���Ѵ�. 
	auto pTile = GetTile(startX, startY);
	while (IS_NOT_NULL(pTile))
	{
		int distValue = pTile->GetDistanceInfo(dir);
		if (distValue <= 0)
		{
			//��ֹ����� Ÿ���� �� ������ �ִ�. 
			if (distToGoal <= std::abs(distValue))
				return true;
			else
				return false;
		}
		else
		{
			//���� ��������Ʈ���� Ÿ���� �� ������ �ִ�. 
			if (distToGoal <= distValue)
			{
				return true;
			}
			else
			{
				//������ ��������Ʈ�� �ִٸ� �ű⼭ üũ�Ѵ�. 
				int dirX = 0;
				int dirY = 0;
				std::tie(dirX, dirY) = GetAdjacentDir(dir);
				dirX *= distValue;
				dirY *= distValue;

				pTile = GetTile(pTile->GetPosX() + dirX, pTile->GetPosY() + dirY);
				distToGoal -= distValue;
			}
		}
	}

	return false;
};

GAME::DIRECTIONS Map::CalcDirectionPos2Pos(int startX, int startY, int endX, int endY) const
{
	int diffX = endX - startX;
	int diffY = endY - startY;

	if (std::abs(diffX) != std::abs(diffY)
		&& 0 != diffX
		&& 0 != diffY)
		return GAME::DIRECTIONS::INVALID;

	if (0 == diffX && diffY < 0) return GAME::DIRECTIONS::TOP;
	if (0 == diffX && 0 < diffY) return GAME::DIRECTIONS::BOTTOM;
	if (0 < diffX && 0 == diffY) return GAME::DIRECTIONS::RIGHT;
	if (diffX < 0 && 0 == diffY) return GAME::DIRECTIONS::LEFT;
	if (diffX < 0 && diffY < 0)  return GAME::DIRECTIONS::LEFTTOP;
	if (0 < diffX && diffY < 0)  return GAME::DIRECTIONS::RIGHTTOP;
	if (diffX < 0 && 0 < diffY)  return GAME::DIRECTIONS::LEFTBOTTOM;
	if (0 < diffX && 0 < diffY)  return GAME::DIRECTIONS::RIGHTBOTTOM;

	return GAME::DIRECTIONS::INVALID;
}


//�� ������ ������ ��ġ�� ��带 ã���ϴ�. 
std::list<GAME::POS_INFO> Map::FindNodeForCheckConnect(int startX, int startY, int endX, int endY) const
{
	std::list<GAME::POS_INFO> contactPos;

	int diffX = endX - startX;
	int diffY = endY - startY;

	if (diffX == 0 && diffY == 0)
		return contactPos;

	//startX, startY ���� ������� y = x + a1, y = -x + a2�� ���Ѵ�. 
	int a1 = startY - startX;
	int a2 = startY + startX;
	//endX, endY ���� ������� y = x + b1,  y= -x + b2�� ���Ѵ�.
	int b1 = endY - endX;
	int b2 = endY + endX;

	auto checkIsContain = [&](int x, int y)->bool 
	{
		return std::any_of(contactPos.cbegin(), contactPos.cend(), [x, y](const GAME::POS_INFO& posInfo)->bool 
		{
			return (std::get<0>(posInfo) == x && std::get<1>(posInfo) == y); 
		});
	};


	//y = startY (x��� ������ ����)	
	if(!checkIsContain(endX, startY))			contactPos.emplace_back(std::make_tuple(endX, startY));
	if(!checkIsContain(startY - b1, startY))	contactPos.emplace_back(std::make_tuple(startY - b1, startY));
	if(!checkIsContain(b2 - startY, startY))	contactPos.emplace_back(std::make_tuple(b2 - startY, startY));

	//x = startX (y��� ������ ����)
	if (!checkIsContain(startX, endY))			contactPos.emplace_back(std::make_tuple(startX, endY));
	if (!checkIsContain(startX, startX + b1))	contactPos.emplace_back(std::make_tuple(startX, startX + b1));
	if (!checkIsContain(startX, -startX + b2))	contactPos.emplace_back(std::make_tuple(startX, -startX + b2));

	//y = x + a1 �����϶� ����. �ᱹ ������ y��, x��, y=-x + b2 �� �񱳸� �ϸ�ȴ�. 
	{
		//y = x + a1, y = -x + b2 ������ ���Ѵ�.
		//X' : (b2 - a1) / (1 - (-1))  = (b2 - a1) / (2)
		//Y' : X' + a1

		int Xd = b2 - a1;

		if (!checkIsContain(endX, endX + a1))	contactPos.emplace_back(std::make_tuple(endX, endX + a1));
		if (!checkIsContain(endY - a1, endY))	contactPos.emplace_back(std::make_tuple(endY - a1, endY));

		//X'�� �������� �˻��մϴ�. 
		if ((Xd % 2) == 0)
		{
			Xd >>= 1;
			if (!checkIsContain(Xd, Xd + a1))	contactPos.emplace_back(std::make_tuple(Xd, Xd + a1));
		}
	}

	//y = -x + a2 �����϶� ����.�ᱹ ������ y��, x��, y= x + b1 �� �񱳸� �ϸ�ȴ�. 
	{
		//X'' : (b1 - a2) / (-1 - 1)  = (a2 - b1) / (2)
		//Y'' : X' + b1

		int Xdd = a2 - b1;

		if (!checkIsContain(endX, endX + b1))	contactPos.emplace_back(std::make_tuple(endX, endX + b1));
		if (!checkIsContain(endY - b1, endY))	contactPos.emplace_back(std::make_tuple(endY - b1, endY));

		//X''�� �������� �˻��մϴ�. 
		if ((Xdd % 2) == 0)
		{
			Xdd >>= 1;
			if (!checkIsContain(Xdd, Xdd + b1))	contactPos.emplace_back(std::make_tuple(Xdd, Xdd + b1));
		}
	}


	return contactPos;
};

std::vector<GAME::POS_INFO> Map::CreatePath(const NODE_INFO* pNodeInfo, int endX, int endY) const
{
	std::vector<GAME::POS_INFO> vecPaths;
	vecPaths.reserve(16);

	vecPaths.emplace_back(endX, endY);

	while (IS_NOT_NULL(pNodeInfo))
	{
		auto pTile = pNodeInfo->m_pTile;
		if (IS_NOT_NULL(pTile))
			vecPaths.emplace_back(pTile->GetPosX(), pTile->GetPosY());
		
		pNodeInfo = pNodeInfo->m_pParent;
	}

	return vecPaths;
}