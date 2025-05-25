#include "world.h"

namespace Snake2d {
	World::World(const int height, const int width): height(height), width(width) {
		initializeField();
		initializeSnake();
		initializeCorpse();
		initializeBone();
		initializeDirection();
	}
	void World::initializeField() {
		field.resize(height);
		for (int y = 0; y < height; y++)
		{
			field[y].resize(width);
			for (int x = 0; x < width; x++)
				field[y][x] = new None(getIndex(Coordinate{ x, y }));
		}
	}
	void World::initializeSnake() {
		std::deque<SnakeBody*> snakeBody;

		Coordinate coordinate0{ 0, 0 };
		SnakeTail* tail = new SnakeTail(getIndex(coordinate0), GameObject::Rotation::DEGREES_0);
		delete field[coordinate0.y][coordinate0.x];
		field[coordinate0.y][coordinate0.x] = tail;
		snakeBody.push_front(tail);

		Coordinate coordinate1{ 0, 1 };
		SnakeBody* body = new SnakeBody(getIndex(coordinate1), GameObject::Rotation::DEGREES_0);
		delete field[coordinate1.y][coordinate1.x];
		field[coordinate1.y][coordinate1.x] = body;
		snakeBody.push_front(body);

		Coordinate coordinate2{ 0, 2 };
		SnakeHead* head = new SnakeHead(getIndex(coordinate2), GameObject::Rotation::DEGREES_0);
		delete field[coordinate2.y][coordinate2.x];
		field[coordinate2.y][coordinate2.x] = head;
		snakeBody.push_front(head);

		this->snake = new Snake(snakeBody);
	}
	void World::initializeCorpse() {
		corpse = new Corpse();
	}
	void World::initializeBone() {
		bone = new Bone();
	}
	void World::initializeDirection() {
		UserInput::updateDirection(UserInput::RIGHT);
	}
	World::~World() {
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++) {
				delete field[y][x];
				field[y][x] = nullptr;
			}
		field.clear();
		delete snake;
		snake = nullptr;
		delete corpse;
		corpse = nullptr;
		delete bone;
		bone = nullptr;
	}

	int World::getIndex(Coordinate coordinate) {
		return width * (coordinate.y + 1) - width + coordinate.x;
	}
	Coordinate World::getCoordinate(int index) {
		int x = index % width;
		int y = index / width;
		return Coordinate{ x, y };
	}

	Coordinate World::getNextSnakeHead(UserInput::Direction direction, Coordinate coordinate) {
		int x = coordinate.x, y = coordinate.y;

		switch (direction)
		{
		case UserInput::Direction::UP:		y--; break;
		case UserInput::Direction::DOWN:	y++; break;
		case UserInput::Direction::LEFT:	x--; break;
		case UserInput::Direction::RIGHT:	x++; break;
		}

		if (y == -1)		y = height - 1;
		if (y == height)	y = 0;
		if (x == -1)		x = width - 1;
		if (x == width)		x = 0;

		return Coordinate{ x, y };
	}
	void World::swapDirectionIfHeadCollidesWithNeck(Coordinate snakeNeck, Coordinate nextSnakeHead) {
		if (snakeNeck.x == nextSnakeHead.x && snakeNeck.y == nextSnakeHead.y)
			switch (UserInput::direction)
			{
			case UserInput::Direction::UP:		UserInput::direction = UserInput::Direction::DOWN;	break;
			case UserInput::Direction::DOWN:	UserInput::direction = UserInput::Direction::UP;	break;
			case UserInput::Direction::LEFT:	UserInput::direction = UserInput::Direction::RIGHT;	break;
			case UserInput::Direction::RIGHT:	UserInput::direction = UserInput::Direction::LEFT;	break;
			}
	}
	
	void World::setSnakeRotation() {
		// Set Snake Head Rotation
		SnakeHead* head = snake->getHead();
		switch (UserInput::direction) {
			case UserInput::Direction::RIGHT:	head->rotation = GameObject::Rotation::DEGREES_0;	break;
			case UserInput::Direction::DOWN:	head->rotation = GameObject::Rotation::DEGREES_90;	break;
			case UserInput::Direction::LEFT:	head->rotation = GameObject::Rotation::DEGREES_180;	break;
			case UserInput::Direction::UP:		head->rotation = GameObject::Rotation::DEGREES_270;	break;
		}

		enum Position { Left, Right, Top, Bottom };

		// Set Snake Body Rotation
		std::deque<SnakeBody*> snakeBody = snake->getBody();
		for (int i = 1; i < snakeBody.size() - 1; i++) {
			// Head is next | Tail is prev
			SnakeBody* next = snakeBody[i - 1];
			SnakeBody* crnt = snakeBody[i];
			SnakeBody* prev = snakeBody[i + 1];

			Coordinate nextCoord = getCoordinate(next->index);
			Coordinate crntCoord = getCoordinate(crnt->index);
			Coordinate prevCoord = getCoordinate(prev->index);

			// Sets current Position to: TL, TR, BR, BL; TB, LR
			// Where crntPos[0] is Position relative to next SnakeBody (in SnakeHead direction)
			// and crntPos[1] is Position relative to prev SnakeBody (in SnakeTail direction)
			Position crntPos[2]{};
			if		(crnt->index == next->index - 1)
				crntPos[0] = Position::Left;
			else if (crnt->index == next->index + 1)
				crntPos[0] = Position::Right;
			else if (crnt->index == next->index - width)
				crntPos[0] = Position::Top;
			else if (crnt->index == next->index + width)
				crntPos[0] = Position::Bottom;
			else {
				if ((nextCoord.x + 1) % width == crntCoord.x && nextCoord.y == crntCoord.y)
					crntPos[0] = Position::Right;
				else if ((nextCoord.x - 1 + width) % width == crntCoord.x && nextCoord.y == crntCoord.y)
					crntPos[0] = Position::Left;
				else if ((nextCoord.y + 1) % height == crntCoord.y && nextCoord.x == crntCoord.x)
					crntPos[0] = Position::Bottom;
				else if ((nextCoord.y - 1 + height) % height == crntCoord.y && nextCoord.x == crntCoord.x)
					crntPos[0] = Position::Top;
				else
					std::cout << "WARNING: crnt & next: " << crnt->index << " : " << next->index << std::endl;
			}

			if (crnt->index == prev->index - 1)
				crntPos[1] = Position::Left;
			else if (crnt->index == prev->index + 1)
				crntPos[1] = Position::Right;
			else if (crnt->index == prev->index - width)
				crntPos[1] = Position::Top;
			else if (crnt->index == prev->index + width)
				crntPos[1] = Position::Bottom;
			else {
				if ((prevCoord.x + 1) % width == crntCoord.x && prevCoord.y == crntCoord.y)
					crntPos[1] = Position::Right;
				else if ((prevCoord.x - 1 + width) % width == crntCoord.x && prevCoord.y == crntCoord.y)
					crntPos[1] = Position::Left;
				else if ((prevCoord.y + 1) % height == crntCoord.y && prevCoord.x == crntCoord.x)
					crntPos[1] = Position::Bottom;
				else if ((prevCoord.y - 1 + height) % height == crntCoord.y && prevCoord.x == crntCoord.x)
					crntPos[1] = Position::Top;
				else
					std::cout << "WARNING: crnt & prev: " << crnt->index << " : " << prev->index << std::endl;
			}

			// LR / RL => straight 0*
			if (crntPos[0] == Position::Left && crntPos[1] == Position::Right) {
				crnt->rotation = GameObject::Rotation::DEGREES_0;
				crnt->segment = SnakeBody::Segment::Straight;
			}
			else if (crntPos[0] == Position::Right && crntPos[1] == Position::Left) {
				crnt->rotation = GameObject::Rotation::DEGREES_0;
				crnt->segment = SnakeBody::Segment::Straight;
			}
			// TB / BT => straight 90*
			else if (crntPos[0] == Position::Top && crntPos[1] == Position::Bottom) {
				crnt->rotation = GameObject::Rotation::DEGREES_90;
				crnt->segment = SnakeBody::Segment::Straight;
			}
			else if (crntPos[0] == Position::Bottom && crntPos[1] == Position::Top) {
				crnt->rotation = GameObject::Rotation::DEGREES_90;
				crnt->segment = SnakeBody::Segment::Straight;
			}
			
			// TT => 
			else if (crntPos[0] == Position::Top && crntPos[1] == Position::Top) {
				crnt->rotation = GameObject::Rotation::DEGREES_90;
				crnt->segment = SnakeBody::Segment::Straight;
			}
			// BB =>
			else if (crntPos[0] == Position::Bottom && crntPos[1] == Position::Bottom) {
				crnt->rotation = GameObject::Rotation::DEGREES_90;
				crnt->segment = SnakeBody::Segment::Straight;
			}
			// LL =>
			else if (crntPos[0] == Position::Left && crntPos[1] == Position::Left) {
				crnt->rotation = GameObject::Rotation::DEGREES_0;
				crnt->segment = SnakeBody::Segment::Straight;
			}
			// RR =>
			else if (crntPos[0] == Position::Right && crntPos[1] == Position::Right) {
				crnt->rotation = GameObject::Rotation::DEGREES_0;
				crnt->segment = SnakeBody::Segment::Straight;
			}

			// TR / RT => rotated 0*
			else if (crntPos[0] == Position::Top && crntPos[1] == Position::Right) {
				crnt->rotation = GameObject::Rotation::DEGREES_0;
				crnt->segment = SnakeBody::Segment::Rotated;
			}
			else if (crntPos[0] == Position::Right && crntPos[1] == Position::Top) {
				crnt->rotation = GameObject::Rotation::DEGREES_0;
				crnt->segment = SnakeBody::Segment::Rotated;
			}
			// BR / RB => rotated 90*
			else if (crntPos[0] == Position::Bottom && crntPos[1] == Position::Right) {
				crnt->rotation = GameObject::Rotation::DEGREES_90;
				crnt->segment = SnakeBody::Segment::Rotated;
			}
			else if (crntPos[0] == Position::Right && crntPos[1] == Position::Bottom) {
				crnt->rotation = GameObject::Rotation::DEGREES_90;
				crnt->segment = SnakeBody::Segment::Rotated;
			}
			// BL / LB => rotated 180*
			else if (crntPos[0] == Position::Bottom && crntPos[1] == Position::Left) {
				crnt->rotation = GameObject::Rotation::DEGREES_180;
				crnt->segment = SnakeBody::Segment::Rotated;
			}
			else if (crntPos[0] == Position::Left && crntPos[1] == Position::Bottom) {
				crnt->rotation = GameObject::Rotation::DEGREES_180;
				crnt->segment = SnakeBody::Segment::Rotated;
			}
			// TL / LT => rotated 270*
			else if (crntPos[0] == Position::Top && crntPos[1] == Position::Left) {
				crnt->rotation = GameObject::Rotation::DEGREES_270;
				crnt->segment = SnakeBody::Segment::Rotated;
			}
			else if (crntPos[0] == Position::Left && crntPos[1] == Position::Top) {
				crnt->rotation = GameObject::Rotation::DEGREES_270;
				crnt->segment = SnakeBody::Segment::Rotated;
			}
			else {
				std::cout << "WARNING: unhandled combination of positions for GameObject: " << crntPos[0] << ":" << crntPos[1] << std::endl;
			}
		}

		// Set Snake Tail Rotation
		SnakeTail* tail = snake->getTail();
		SnakeBody* beforeTail = snake->getBodyBeforeTail();
		if		(tail->index == beforeTail->index - 1)
			tail->rotation = GameObject::Rotation::DEGREES_180;
		else if (tail->index == beforeTail->index + 1)
			tail->rotation = GameObject::Rotation::DEGREES_0;
		else if (tail->index == beforeTail->index - width)
			tail->rotation = GameObject::Rotation::DEGREES_270;
		else if (tail->index == beforeTail->index + width)
			tail->rotation = GameObject::Rotation::DEGREES_90;
		else {
			Coordinate tailCoordinate = getCoordinate(tail->index);
			Coordinate beforeTailCoordinate = getCoordinate(beforeTail->index);
			if ((beforeTailCoordinate.x + 1) % width == tailCoordinate.x && beforeTailCoordinate.y == tailCoordinate.y)
				tail->rotation = GameObject::Rotation::DEGREES_0;
			else if ((beforeTailCoordinate.x - 1 + width) % width == tailCoordinate.x && beforeTailCoordinate.y == tailCoordinate.y)
				tail->rotation = GameObject::Rotation::DEGREES_180;
			else if ((beforeTailCoordinate.y + 1) % height == tailCoordinate.y && beforeTailCoordinate.x == tailCoordinate.x)
				tail->rotation = GameObject::Rotation::DEGREES_90;
			else if ((beforeTailCoordinate.y - 1 + height) % height == tailCoordinate.y && beforeTailCoordinate.x == tailCoordinate.x)
				tail->rotation = GameObject::Rotation::DEGREES_270;
			else
				std::cout << "WARNING: tail ROTATION not specified: " << std::endl;
		}
	}

	void World::assertOnlyOneHead() {
		int count = 0;
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
				if (field[y][x]->getType() == GameObject::SNAKE_HEAD)
					count++;
		if (count != 1)
			std::cout << count << " - 'SNAKE_HEAD' detected but ONLY ONE is expected!\n";
	}
	void World::assertOnlyOneTail() {
		int count = 0;
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
				if (field[y][x]->getType() == GameObject::SNAKE_TAIL)
					count++;
		if (count != 1)
			std::cout << count << " - 'SNAKE_TAIL' detected but ONLY ONE is expected!\n";
	}
	void World::assertSnake() {
		std::vector<GameObject*> worldSnakeBody;
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
				if (field[y][x]->getType() == GameObject::SNAKE_BODY || field[y][x]->getType() == GameObject::SNAKE_HEAD || field[y][x]->getType() == GameObject::SNAKE_TAIL)
					worldSnakeBody.push_back(field[y][x]);

		std::deque<SnakeBody*> body = snake->getBody();
		if (body.size() != worldSnakeBody.size())
			std::cout << "Actual Snake size[" << body.size() << "] differs from Snake size in World[" << worldSnakeBody.size() << "]" << std::endl;


		for (int i = 0; i < body.size(); i++)
		{
			bool foundEqualSnakeBodyPart = false;
			for (int j = 0; j < worldSnakeBody.size(); j++)
				if (worldSnakeBody[j]->index == body[i]->index && worldSnakeBody[j]->getType() == body[i]->getType())
				{
					foundEqualSnakeBodyPart = true;
					break;
				}
			if (!foundEqualSnakeBodyPart)
				std::cout << "Missmatch found between Snake and World body parts at Snake index[" << body[i]->index << "]\n";
		}
	}
	void World::assertZeroApples() {
		int count = 0;
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
				if (field[y][x]->getType() == GameObject::APPLE)
					count++;
		if (count != 0)
			std::cout << count + " - 'APPLE' detected but ZERO is expected!\n";
	}
	void World::assertOneApple() {
		int count = 0;
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
				if (field[y][x]->getType() == GameObject::APPLE)
					count++;
		if (count != 1)
			std::cout << count << " - 'APPLE' detected but ONLY ONE is expected\n!";
	}
	void World::assertPointers() {
		std::vector<GameObject*> worldSnake;
		std::vector<GameObject*> worldCorpse;
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++) {
				GameObject* go = field[y][x];
				if (go->getType() == GameObject::APPLE)
					//worldApple = go;
					continue;
				else if (go->getType() == GameObject::SNAKE_BODY || go->getType() == GameObject::SNAKE_HEAD || go->getType() == GameObject::SNAKE_TAIL)
					worldSnake.push_back(go);
				else if (go->getType() == GameObject::SNAKE_CORPSE)
					worldCorpse.push_back(go);
			}

		std::deque<SnakeBody*> snakeBody = snake->getBody();
		for (int i = 0; i < snakeBody.size(); i++) {
			bool hasDifferentPointers = true;
			for (int j = 0; j < worldSnake.size(); j++)
				if (worldSnake[j] == static_cast<GameObject*>(snakeBody[i])) {
					hasDifferentPointers = false;
					break;
				}
			if (hasDifferentPointers)
				std::cout << "World snake & Snake body pointers are different!\n";
		}

		std::vector<SnakeCorpse*> snakeCorpse = corpse->getCorpses();
		for (int i = 0; i < snakeCorpse.size(); i++) {
			bool hasDifferentPointers = true;
			for (int j = 0; j < worldCorpse.size(); j++)
				if (worldCorpse[j] == static_cast<GameObject*>(snakeCorpse[i])) {
					hasDifferentPointers = false;
					break;
				}
			if (hasDifferentPointers)
				std::cout << "World corpse & Corpse class pointers are different!\n";
		}
	}

	void World::generate(GameObject::Type goType) {
		std::vector<int> noneIndexes;
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++) {
				GameObject* go = field[y][x];
				if (go->getType() == GameObject::Type::NONE)
					noneIndexes.push_back(go->index);
			}

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, noneIndexes.size() - 1);
		int randomIndex = dist(gen);
		int index = noneIndexes[randomIndex];

		Coordinate coordinate = getCoordinate(index);
		switch (goType) {
			case Snake2d::GameObject::APPLE: {
				delete field[coordinate.y][coordinate.x];
				field[coordinate.y][coordinate.x] = new Apple(index);
				break;
			}
			case Snake2d::GameObject::APPLE_LINE_SPAWNER: {
				delete field[coordinate.y][coordinate.x];
				field[coordinate.y][coordinate.x] = new AppleLineSpawner(index);
				break;
			}
			case Snake2d::GameObject::SNAKE_BONE_DESTROYER: {
				delete field[coordinate.y][coordinate.x];
				field[coordinate.y][coordinate.x] = new SnakeBoneDestroyer(index);
				break;
			}
			default: {
				std::cout << "WARNING::Unexpected GameObject generation - " << goType << std::endl;
				break;
			}
		}
	}
	int World::count(GameObject::Type type) {
		int count = 0;
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
				if (field[y][x]->getType() == type)
					count++;
		return count;
	}

	// BUG with direction (Fixed but signature not updated).
	// Wrong direction memory argument is used in method and nested methods.
	World::WorldUpdateResult World::update(UserInput::Direction direction, uint64_t now) {
		WorldUpdateResult worldUpdateResult;

		// Calculate next head position depending on direction
		SnakeHead* head = snake->getHead();
		int headIndex = head->index;
		Coordinate nextSnakeHead = getNextSnakeHead(UserInput::direction, getCoordinate(headIndex));

		// Check if position is out of bounds and update next position accordingly
		SnakeBody* neck = snake->getNeck();
		int neckIndex = neck->index;
		Coordinate snakeNeck = getCoordinate(neckIndex);

		// Swaps direction and recalculates next head position again
		swapDirectionIfHeadCollidesWithNeck(snakeNeck, nextSnakeHead);
		nextSnakeHead = getNextSnakeHead(UserInput::direction, getCoordinate(headIndex));
		SnakeHead* newSnakeHead = new SnakeHead(getIndex(nextSnakeHead), GameObject::Rotation::DEGREES_0);

		// REPLACEMENT OF OLD HEAD WITH NECK
		Coordinate headCoordinate = getCoordinate(headIndex);
		SnakeBody* newSnakeNeck = new SnakeBody(headIndex, GameObject::Rotation::DEGREES_0);

		GameObject* go = field[nextSnakeHead.y][nextSnakeHead.x];

		worldUpdateResult.collisionWith = go->getType();
		worldUpdateResult.collisionAt = getCoordinate(go->index);
		switch (go->getType())
		{
		case GameObject::Type::APPLE: {
			// ADD NEW HEAD (instead of old apple)
			delete field[nextSnakeHead.y][nextSnakeHead.x];
			field[nextSnakeHead.y][nextSnakeHead.x] = newSnakeHead;
			// FIXME: do something with assertions
			//assertZeroApples();

			// ADD NEW NECK (instead of old head)
			delete field[headCoordinate.y][headCoordinate.x];
			field[headCoordinate.y][headCoordinate.x] = newSnakeNeck;
			// TAIL manipulations are not needed when Apple is eaten we keep tail

			// UPDATE SNAKE WITH NEW NECK AND HEAD
			snake->increase(newSnakeNeck, newSnakeHead);

			// FIXME: do something with assertions
			//assertOneApple();

			worldUpdateResult.scoreDelta += 3;
			break;
		}
		case GameObject::Type::SNAKE_TAIL:
		case GameObject::Type::SNAKE_BODY: {
			// delete left TAIL parts
			std::vector<SnakeBody*> cutOffSnakeBody = snake->cutTailAt(newSnakeHead);
			for (int i = 0; i < cutOffSnakeBody.size(); i++) {
				int index = cutOffSnakeBody[i]->index;
				Coordinate snakeCorpseCoordinate = getCoordinate(index);
				//delete cutOffSnakeBody[i];
				delete field[snakeCorpseCoordinate.y][snakeCorpseCoordinate.x];
				// FIXME: add correct value for createdAt
				SnakeCorpse* snakeCorpse = new SnakeCorpse(index, now, 5000);
				field[snakeCorpseCoordinate.y][snakeCorpseCoordinate.x] = snakeCorpse;
				corpse->add(snakeCorpse);
			}

			// ADD NEW HEAD (instead of old body)
			delete field[nextSnakeHead.y][nextSnakeHead.x];
			field[nextSnakeHead.y][nextSnakeHead.x] = newSnakeHead;
			// ADD NEW NECK (instead of old head)
			delete field[headCoordinate.y][headCoordinate.x];
			field[headCoordinate.y][headCoordinate.x] = newSnakeNeck;

			// REPLACEMENT OF OLD TAIL WITH 
			SnakeBody* tail = snake->getBody()[snake->getBody().size() - 1];
			int tailIndex = tail->index;
			Coordinate tailCoordinate = getCoordinate(tailIndex);
			None* newNone = new None(tailIndex);
			// ADD NEW NONE (instead of old tail)
			delete field[tailCoordinate.y][tailCoordinate.x];
			field[tailCoordinate.y][tailCoordinate.x] = newNone;

			// REPLACEMENT OF OLD BODY WITH TAIL
			SnakeBody* bodyBeforeTail = snake->getBodyBeforeTail();
			int bodyBeforeTailIndex = bodyBeforeTail->index;
			SnakeTail* newTail = new SnakeTail(bodyBeforeTailIndex, GameObject::Rotation::DEGREES_0);
			Coordinate bodyBeforeTailCoords = getCoordinate(bodyBeforeTailIndex);
			// ADD NEW TAIL (instead of old body)
			delete field[bodyBeforeTailCoords.y][bodyBeforeTailCoords.x];
			field[bodyBeforeTailCoords.y][bodyBeforeTailCoords.x] = newTail;

			snake->move(newTail, newSnakeNeck, newSnakeHead);
			break;
		}
		case GameObject::Type::SNAKE_CORPSE: {
			std::vector<int> corpsesToRemove;
			corpsesToRemove.push_back(go->index);
			corpse->remove(corpsesToRemove);

			// ADD NEW HEAD (instead of old none)
			delete field[nextSnakeHead.y][nextSnakeHead.x];
			field[nextSnakeHead.y][nextSnakeHead.x] = newSnakeHead;
			// ADD NEW NECK (instead of old head)
			delete field[headCoordinate.y][headCoordinate.x];
			field[headCoordinate.y][headCoordinate.x] = newSnakeNeck;

			// REPLACEMENT OF OLD TAIL WITH 
			SnakeTail* tail = snake->getTail();
			int tailIndex = tail->index;
			Coordinate tailCoordinate = getCoordinate(tailIndex);
			None* newNone = new None(tailIndex);
			// ADD NEW NONE (instead of old tail)
			delete field[tailCoordinate.y][tailCoordinate.x];
			field[tailCoordinate.y][tailCoordinate.x] = newNone;

			// REPLACEMENT OF OLD BODY WITH TAIL
			SnakeBody* bodyBeforeTail = snake->getBodyBeforeTail();
			int bodyBeforeTailIndex = bodyBeforeTail->index;
			SnakeTail* newTail = new SnakeTail(bodyBeforeTailIndex, GameObject::Rotation::DEGREES_0);
			Coordinate bodyBeforeTailCoords = getCoordinate(bodyBeforeTailIndex);
			// ADD NEW TAIL (instead of old body)
			delete field[bodyBeforeTailCoords.y][bodyBeforeTailCoords.x];
			field[bodyBeforeTailCoords.y][bodyBeforeTailCoords.x] = newTail;

			snake->move(newTail, newSnakeNeck, newSnakeHead);

			worldUpdateResult.scoreDelta++;
			break;
		}
		case GameObject::Type::SNAKE_BONE: {
			std::vector<int> bonesToRemove;
			bonesToRemove.push_back(go->index);
			bone->remove(bonesToRemove);

			// ADD NEW HEAD (instead of old none)
			delete field[nextSnakeHead.y][nextSnakeHead.x];
			field[nextSnakeHead.y][nextSnakeHead.x] = newSnakeHead;
			// ADD NEW NECK (instead of old head)
			delete field[headCoordinate.y][headCoordinate.x];
			field[headCoordinate.y][headCoordinate.x] = newSnakeNeck;

			// REPLACEMENT OF OLD TAIL WITH 
			SnakeTail* tail = snake->getTail();
			int tailIndex = tail->index;
			Coordinate tailCoordinate = getCoordinate(tailIndex);
			None* newNone = new None(tailIndex);
			// ADD NEW NONE (instead of old tail)
			delete field[tailCoordinate.y][tailCoordinate.x];
			field[tailCoordinate.y][tailCoordinate.x] = newNone;

			// REPLACEMENT OF OLD BODY WITH TAIL
			SnakeBody* bodyBeforeTail = snake->getBodyBeforeTail();
			int bodyBeforeTailIndex = bodyBeforeTail->index;
			SnakeTail* newTail = new SnakeTail(bodyBeforeTailIndex, GameObject::Rotation::DEGREES_0);
			Coordinate bodyBeforeTailCoords = getCoordinate(bodyBeforeTailIndex);
			// ADD NEW TAIL (instead of old body)
			delete field[bodyBeforeTailCoords.y][bodyBeforeTailCoords.x];
			field[bodyBeforeTailCoords.y][bodyBeforeTailCoords.x] = newTail;

			snake->move(newTail, newSnakeNeck, newSnakeHead);

			worldUpdateResult.lifeDelta--;
			break;
		}
		case GameObject::Type::APPLE_LINE_SPAWNER: {
			int x = nextSnakeHead.x, y = nextSnakeHead.y;
			switch (UserInput::direction) {
			case UserInput::UP: {
				for (; y >= 0; y -= 2)
					if (field[y][x]->getType() == GameObject::Type::NONE) {
						delete field[y][x];
						field[y][x] = new Apple(getIndex(Coordinate{ x, y }));
					}
				break;
			}
			case UserInput::DOWN: {
				for (; y < height; y += 2)
					if (field[y][x]->getType() == GameObject::Type::NONE) {
						delete field[y][x];
						field[y][x] = new Apple(getIndex(Coordinate{ x, y }));
					}
				break;
			}
			case UserInput::LEFT: {
				for (; x >= 0; x -= 2)
					if (field[y][x]->getType() == GameObject::Type::NONE) {
						delete field[y][x];
						field[y][x] = new Apple(getIndex(Coordinate{ x, y }));
					}
				break;
			}
			case UserInput::RIGHT: {
				for (; x < width; x += 2)
					if (field[y][x]->getType() == GameObject::Type::NONE) {
						delete field[y][x];
						field[y][x] = new Apple(getIndex(Coordinate{ x, y }));
					}
				break;
			}
			}

			// ADD NEW HEAD (instead of old none)
			delete field[nextSnakeHead.y][nextSnakeHead.x];
			field[nextSnakeHead.y][nextSnakeHead.x] = newSnakeHead;
			// ADD NEW NECK (instead of old head)
			delete field[headCoordinate.y][headCoordinate.x];
			field[headCoordinate.y][headCoordinate.x] = newSnakeNeck;

			// REPLACEMENT OF OLD TAIL WITH 
			SnakeTail* tail = snake->getTail();
			int tailIndex = tail->index;
			Coordinate tailCoordinate = getCoordinate(tailIndex);
			None* newNone = new None(tailIndex);
			// ADD NEW NONE (instead of old tail)
			delete field[tailCoordinate.y][tailCoordinate.x];
			field[tailCoordinate.y][tailCoordinate.x] = newNone;

			// REPLACEMENT OF OLD BODY WITH TAIL
			SnakeBody* bodyBeforeTail = snake->getBodyBeforeTail();
			int bodyBeforeTailIndex = bodyBeforeTail->index;
			SnakeTail* newTail = new SnakeTail(bodyBeforeTailIndex, GameObject::Rotation::DEGREES_0);
			Coordinate bodyBeforeTailCoords = getCoordinate(bodyBeforeTailIndex);
			// ADD NEW TAIL (instead of old body)
			delete field[bodyBeforeTailCoords.y][bodyBeforeTailCoords.x];
			field[bodyBeforeTailCoords.y][bodyBeforeTailCoords.x] = newTail;

			snake->move(newTail, newSnakeNeck, newSnakeHead);
			break;
		}
		case GameObject::Type::SNAKE_BONE_DESTROYER: {
			std::vector<Coordinate> destroyedSnakeBones;
			for (SnakeBone* bone : bone->getBones()) {
				int index = bone->index;
				Coordinate snakeBoneCoordinate = getCoordinate(index);
				destroyedSnakeBones.push_back(snakeBoneCoordinate);
				delete field[snakeBoneCoordinate.y][snakeBoneCoordinate.x];
				None* none = new None(index);
				field[snakeBoneCoordinate.y][snakeBoneCoordinate.x] = none;
			}
			worldUpdateResult.destroyedSnakeBones = destroyedSnakeBones;
			bone->clear();

			// ADD NEW HEAD (instead of old none)
			delete field[nextSnakeHead.y][nextSnakeHead.x];
			field[nextSnakeHead.y][nextSnakeHead.x] = newSnakeHead;
			// ADD NEW NECK (instead of old head)
			delete field[headCoordinate.y][headCoordinate.x];
			field[headCoordinate.y][headCoordinate.x] = newSnakeNeck;

			// REPLACEMENT OF OLD TAIL WITH 
			SnakeTail* tail = snake->getTail();
			int tailIndex = tail->index;
			Coordinate tailCoordinate = getCoordinate(tailIndex);
			None* newNone = new None(tailIndex);
			// ADD NEW NONE (instead of old tail)
			delete field[tailCoordinate.y][tailCoordinate.x];
			field[tailCoordinate.y][tailCoordinate.x] = newNone;

			// REPLACEMENT OF OLD BODY WITH TAIL
			SnakeBody* bodyBeforeTail = snake->getBodyBeforeTail();
			int bodyBeforeTailIndex = bodyBeforeTail->index;
			SnakeTail* newTail = new SnakeTail(bodyBeforeTailIndex, GameObject::Rotation::DEGREES_0);
			Coordinate bodyBeforeTailCoords = getCoordinate(bodyBeforeTailIndex);
			// ADD NEW TAIL (instead of old body)
			delete field[bodyBeforeTailCoords.y][bodyBeforeTailCoords.x];
			field[bodyBeforeTailCoords.y][bodyBeforeTailCoords.x] = newTail;

			snake->move(newTail, newSnakeNeck, newSnakeHead);
			break;
		}
		default: {
			// ADD NEW HEAD (instead of old none)
			delete field[nextSnakeHead.y][nextSnakeHead.x];
			field[nextSnakeHead.y][nextSnakeHead.x] = newSnakeHead;
			// ADD NEW NECK (instead of old head)
			delete field[headCoordinate.y][headCoordinate.x];
			field[headCoordinate.y][headCoordinate.x] = newSnakeNeck;

			// REPLACEMENT OF OLD BODY WITH TAIL
			SnakeBody* bodyBeforeTail = snake->getBodyBeforeTail();
			int bodyBeforeTailIndex = bodyBeforeTail->index;
			SnakeTail* newTail = new SnakeTail(bodyBeforeTailIndex, GameObject::Rotation::DEGREES_0);
			Coordinate bodyBeforeTailCoords = getCoordinate(bodyBeforeTailIndex);
			// ADD NEW TAIL (instead of old body)
			delete field[bodyBeforeTailCoords.y][bodyBeforeTailCoords.x];
			field[bodyBeforeTailCoords.y][bodyBeforeTailCoords.x] = newTail;

			// REPLACEMENT OF OLD TAIL WITH NONE
			SnakeTail* tail = snake->getTail();
			int tailIndex = tail->index;
			Coordinate tailCoordinate = getCoordinate(tailIndex);
			None* newNone = new None(tailIndex);
			// ADD NEW NONE (instead of old tail)
			delete field[tailCoordinate.y][tailCoordinate.x];
			field[tailCoordinate.y][tailCoordinate.x] = newNone;

			snake->move(newTail, newSnakeNeck, newSnakeHead);
			break;
		}
		}

		// TODO: replace this with separate method
		std::vector<SnakeCorpse*> corpses = corpse->getCorpses();
		std::vector<int> corpsesToRemove;
		for (SnakeCorpse* sc : corpses) {
			sc->setTimeLeft(-(now - (sc->getCreatedAt() + sc->getDelay())));
			if (now >= sc->getCreatedAt() + sc->getDelay())
				corpsesToRemove.push_back(sc->index);
		}

		corpse->remove(corpsesToRemove);
		for (int index : corpsesToRemove) {
			Coordinate coordinate = getCoordinate(index);
			delete field[coordinate.y][coordinate.x];
			SnakeBone* sb = new SnakeBone(index);
			field[coordinate.y][coordinate.x] = sb;
			bone->add(sb);
		}

		setSnakeRotation();

		// TODO: assertion for 'SNAKE_BONE' is needed and separate class for it
		assertPointers();
		assertOnlyOneHead();
		assertOnlyOneTail();
		assertSnake();

		worldUpdateResult.applesInWorld = count(GameObject::Type::APPLE);

		return worldUpdateResult;
	}
	void World::afterUpdate(std::vector<WorldCondition*> postConditions) {
		std::vector<WorldCondition*> metConditions;
		for (WorldCondition* condition : postConditions)
			if (condition->isMet())
				metConditions.push_back(condition);

		for (WorldCondition* condition : metConditions)
			switch (condition->getType()) {
				case WorldConditionType::GENERATE_APPLE_LINE_SPAWNER: {
					generate(GameObject::Type::APPLE_LINE_SPAWNER);
					break;
				}
				case WorldConditionType::GENERATE_SNAKE_BONE_DESTROYER: {
					generate(GameObject::Type::SNAKE_BONE_DESTROYER);
					break;
				}
				case WorldConditionType::GENERATE_APPLE: {
					generate(GameObject::Type::APPLE);
					break;
				}
				default: {
					std::cout << "INFO::This type of WorldConditionType not handled yet - " << condition->getType() << std::endl;
					break;
				}
			}

		for (WorldCondition* condition : metConditions)
			condition->afterMet();
	}
}