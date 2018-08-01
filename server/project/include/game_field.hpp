#ifndef SCAV_SERVER_GAME_FIELD_HPP_
#define SCAV_SERVER_GAME_FIELD_HPP_

#include <SFML/Network.hpp>
#include "game_object.hpp"
#include "physics_object.hpp"
#include <map>
#include <list>
#include <ctime>

class GameField {
 private:
	std::map<int, Player*> players;
	std::map<int, PhysicsObject*> objects;
	std::map<int, std::string> nicknames;
	std::map<int, Tile*> tiles;

	std::list<Bullet*> bullets;
	StaticObject* borders[4];
	std::clock_t start_time;	
	std::list<PhysicsObject*> objects_to_delete;
	bool start;


	b2World* world;
	sf::Packet state_packet;
	std::map<int, sf::Packet*> private_packets;
	float size;
	bool pause;

 public:
	GameField();
	~GameField();
	void add_player(int cl_id, float x, float y);

	void add_bullet(Bullet* bullet);
	void add_object(PhysicsObject* obj);
	void add_tile(Tile* tile);
	
	void delete_player(int cl_id);
	void delete_bullet(Bullet* b);
	void delete_object(int id);
	
	void set_start(bool _s);
	bool get_start();
	float get_size();
	bool step();
	void reset();
	void restart();
	
	void add_nickname(int cl_id, const std::string& nick);
	std::string get_nickname(int cl_id);

	sf::Packet* add_private_packet(int cl_id);
	sf::Packet* get_objects(bool reset);

	std::map<int, sf::Packet*>::iterator p_packets_begin();
	std::map<int, sf::Packet*>::iterator p_packets_end();

	PhysicsObject* get_object(int id);
	Player* get_player(int cl_id);
	b2World* get_physics_world();
	sf::Packet* get_state_packet();
	b2World* get_world();
};

#endif