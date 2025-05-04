#ifndef SNAKE2D_GAME_OBJECT_H
#define SNAKE2D_GAME_OBJECT_H

#include <stdint.h>

namespace Snake2d
{
	class GameObject {
	public:
		enum Rotation { DEGREES_0, DEGREES_90, DEGREES_180, DEGREES_270 };
		enum Type {
			/* Basic */
			NONE,
			SNAKE_BODY,
			SNAKE_TAIL,
			SNAKE_HEAD,
			APPLE,

			/* Fun */
			SNAKE_CORPSE,
			SNAKE_BONE,
			APPLE_LINE_SPAWNER,
		};

		int index;
		Rotation rotation;

		GameObject(int index, Rotation rotation) : index(index), rotation(rotation) {}
		virtual ~GameObject() = default;
		virtual Type getType() const = 0;
		Rotation getRotation() const { return rotation; }
	};

	class None : public GameObject {
	public:
		None(int index) : GameObject(index, Rotation::DEGREES_0) {}
		Type getType() const { return Type::NONE; }
	};

	class SnakeBody : public GameObject {
	public:
		enum Segment { None, Straight, Rotated };
		SnakeBody(int index, Rotation rotation) : GameObject(index, rotation), segment(Segment::None) {}
		Segment segment;
		Type getType() const { return Type::SNAKE_BODY; }
	};

	class SnakeHead : public SnakeBody {
	public:
		SnakeHead(int index, Rotation rotation) : SnakeBody(index, rotation) {}
		Type getType() const { return Type::SNAKE_HEAD; }
	};

	class SnakeTail : public SnakeBody {
	public:
		SnakeTail(int index, Rotation rotation) : SnakeBody(index, rotation) {}
		Type getType() const { return Type::SNAKE_TAIL; }
	};

	class SnakeCorpse : public GameObject {
	private:
		uint64_t createdAt;
		uint64_t delay;
	public:
		SnakeCorpse(int index, uint64_t createdAt, uint64_t delay) : GameObject(index, Rotation::DEGREES_0), createdAt(createdAt), delay(delay) {}
		uint64_t getDelay() const { return delay; }
		uint64_t getCreatedAt() const { return createdAt; }
		Type getType() const { return Type::SNAKE_CORPSE; }
	};

	class SnakeBone : public GameObject{
	public:
		SnakeBone(int index) : GameObject(index, Rotation::DEGREES_0) {}
		Type getType() const { return Type::SNAKE_BONE; }
	};

	class Apple : public GameObject {
	public:
		Apple(int index) : GameObject(index, Rotation::DEGREES_0) {}
		Type getType() const { return Type::APPLE; }
	};

	class AppleLineSpawner : public GameObject {
	public:
		AppleLineSpawner(int index) : GameObject(index, Rotation::DEGREES_0) {}
		Type getType() const { return Type::APPLE_LINE_SPAWNER; }
	};
}

#endif