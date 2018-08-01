
//#include "view.hpp"
#include "gamefield.hpp"
#include <iostream>
#include <unistd.h>

void GameField::draw_border(float x, float y) {
    Vector2f size(x, y);
    field_border.setSize(size);
    field_border.setOrigin(field_border.getLocalBounds().width / 2, field_border.getLocalBounds().height / 2);
    window.draw(field_border);
}


GameField::GameField(Textures* txt): size(2000), world(new b2World(b2Vec2(0, 0))), t_cont(txt), g_map(size / 100, size / 100, t_cont->get_texture(4)), interface(Interface(&window)), g_curs(t_cont->get_texture(14)), inv(Inventor(&window)), was_shot(false), last_shot(0), start(false), border_pos(size), pause(false) {
    player = nullptr;
    window.create(sf::VideoMode(640, 480), "S.C.A.V.");
    window.setFramerateLimit(60);
    borders[0] = new StaticObject(world, b2Vec2(20, 2* size), b2Vec2(-size, 0));
    borders[1] = new StaticObject(world, b2Vec2(20, 2* size), b2Vec2(size, 0));
    borders[2] = new StaticObject(world, b2Vec2(2* size, 20), b2Vec2(0, size));
    borders[3] = new StaticObject(world, b2Vec2(2* size, 20), b2Vec2(0, -size));
    field_border.setSize(Vector2f(1000.f, 1000.f));
    field_border.setFillColor(Color::Transparent);
    field_border.setOutlineColor(Color::Red);
    field_border.setOutlineThickness(10);
    field_border.setOrigin(field_border.getLocalBounds().width / 2, field_border.getLocalBounds().height / 2);

}

MapConst& GameField::get_map() {
    return g_map;
}

GameField::~GameField() {
    for (int i = 0; i < 4; ++i) {
        delete borders[i];
    }
    
    delete_all();
    delete world;
}

b2World* GameField::get_physics_world() {
    return world;
}

Player* GameField::find_player(int obj_id) {

    auto iter = players.find(obj_id);
    if (iter != players.end()) {
        return iter->second;
    }

    return nullptr;

}
void GameField::set_winner(int w) {
    winner = w;
    pause = true;
}
void GameField::resume() {
    pause = false;
}
bool GameField::get_action(sf::Packet& packet) {
    if (!start) {
        return true;
    }

    if (player != nullptr) {
        packet << 1 << player->get_pos().x <<player->get_pos().y << player->get_rotation();
        if (was_shot) {
            Weapon* w = inv.get_current();
            packet << 14 << w->get_id() << player->get_pos().x <<player->get_pos().y << player->get_rotation();
            was_shot = false;
        }
        return true;
    }
    return false;
}
void GameField::set_start(bool _s) {
    start = _s;
}

bool GameField::get_start() {
    return start;
}

void GameField::shoot() {
    Weapon* w = inv.get_current();

    if (!w) {
    	return;
    }
    int ammo =  w->get_ammo();
	if (last_shot > w->get_speed() && ammo > 0) {
	    was_shot = true;
	    last_shot = 0;
        w->set_ammo(ammo - 1);
	}
}
void GameField::show_message(const std::string& msg) {
    g_cam.setSize(640,480);
    g_cam.setCenter(320, 240);
    window.setView(g_cam);
    interface.showMessage(std::string(msg), std::string(""), sf::Color(158, 236, 255, 255),sf::Color(0, 0, 0, 255));
    g_cam.setSize(1920,1080);
            
}

void GameField::check_key() {
    if (player->get_hp() > 0) {
        b2Vec2 speed(0, 0);

        if(Keyboard::isKeyPressed(Keyboard::A)) {
            speed += b2Vec2(-200.f, 0.f);
        }
        if(Keyboard::isKeyPressed(Keyboard::D)) {
            speed += b2Vec2(200.f, 0.f);
        }
        if(Keyboard::isKeyPressed(Keyboard::W)) {
            speed += b2Vec2(0.f, -200.f);
        }
        if(Keyboard::isKeyPressed(Keyboard::S)) {
            speed += b2Vec2(0.f, 200.f);
        }
        player->set_speed(speed.x, speed.y);

        player->mouse_rotation(window);

        if (Mouse::isButtonPressed(Mouse::Left)) {
            shoot();
        }
    }
}

void GameField::draw() {
    if (player == nullptr) {
        return;
    }
    b2Vec2 p_pos = player->get_pos();

    for (auto iter = objects.begin(); iter != objects.end(); iter++) {
        
        DrawableObject* obj = dynamic_cast<DrawableObject*>(iter->second);
        if (obj != nullptr) {
            const Vector2f& pos = obj->get_sprite_pos(); 
            if (pos.x > p_pos.x - 1000 && pos.x < p_pos.x + 1000 && pos.y > p_pos.y - 700 && pos.y < p_pos.y + 700) {
                obj->draw(window);
            }
        }
    }
    for (auto iter = bullets.begin(); iter != bullets.end(); iter++) {
        (*iter)->draw(window);
    }
    for (auto iter = players.begin(); iter != players.end(); iter++) {
        iter->second->draw(window);
        b2Vec2 pos = iter->second->get_pos();
        interface.drawLine(iter->second->get_nickname(), pos.x, pos.y);
    }
    g_curs.draw(window);
    draw_border(2 * border_pos, 2 * border_pos);
}

void GameField::inventor_interact() {
    if (player!=nullptr) {
        interface.draw(player->get_pos().x, player->get_pos().y);
        Weapon* w = inv.get_current();
        if (w) {
            interface.set_ammo(w->get_ammo());
        }
        else {
            interface.set_ammo(0);
        }

        inv.check_key();
        inv.draw(player->get_pos().x, player->get_pos().y);
    }
}
bool GameField::render() {
    if (window.isOpen())
    {
    	if (last_shot < 100000) {
    		last_shot++;
    	}

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            window.close();
        }

        window.clear();
        if (!start) {
            show_message("Waiting for players");
            if (window.isOpen()) {
               return true;
            }
            else {
                return false;
            }
        }

        if (pause) {
            auto i = players.find(winner);
            if (i != players.end()) {
                show_message(std::string("Winner: ") + i->second->get_nickname());
            }
            else {
                show_message("Winner left");
            }
            if (window.isOpen()) {
               return true;
            }
            else {
                return false;
            }
        }

        mtx.lock();
        if (player != nullptr) {
            check_key();
            interface.set_hp(player->get_hp());

            g_cam.setCenter(player->get_pos().x, player->get_pos().y);
            g_map.draw(window, player->get_pos().x, player->get_pos().y);
            draw();
            
        }

        window.setView(g_cam);

        
        inventor_interact();
        tmp_a_cont.draw(window);
        window.display();
        mtx.unlock();
    }
    else {
    	return false;
    }
    return true;
}

Inventor* GameField::get_inventor() {
    return &inv;
}

std::mutex& GameField::get_mutex() {
	return mtx;
}
int GameField::add_player(Player* obj, int new_id) {
    players.emplace(new_id, obj);
    return 0;
}

int GameField::add_object(PhysicsObject* obj, int new_id) {
    objects.emplace(new_id, obj);
    return 0;
}

int GameField::add_bullet(DrawableBullet* obj) {
    bullets.push_front(obj);
    return 0;
}

void GameField::set_player(int player_id) {
    player = find_player(player_id);
}

Player* GameField::get_player() {
    return player;
}

void GameField::delete_bullet(DrawableBullet* b) {
	for (auto i = bullets.begin(); i != bullets.end(); ++i) {
		if (*i == b) {
			bullets.erase(i);
			delete b;
			return;
		}
	}
}

void GameField::delete_player(int cl_id) {
    auto p = players.find(cl_id);
    if (p!=players.end()) {
    	delete p->second;
    	players.erase(p);
	}
}

void GameField::delete_object(int id) {
    
    auto w = objects.find(id);
    if (w == objects.end()) {
        return;
    }
    DrawableObject* d_obj = dynamic_cast<DrawableObject*>(w->second);
    if (d_obj != nullptr) {
        d_obj->get_delete_sprite(tmp_a_cont);
    }

    delete w->second;
    objects.erase(w);
}

PhysicsObject* GameField::get_object(int id) {
    auto iter = objects.find(id);
    if (iter != objects.end()) {
        return iter->second;
    }

    return nullptr;
}

void GameField::delete_all() {
   
    inv.clear();
    for (auto i = objects.begin(); i != objects.end();) {
        delete i->second;
        i = objects.erase(i);
    }
    for (auto i = players.begin(); i != players.end();) {
        delete i->second;
        i = players.erase(i);
    }
    for (auto i = bullets.begin(); i != bullets.end();) {
        delete *i;
        i = bullets.erase(i);
    }
    player = nullptr;

}

RenderWindow* GameField::get_window() {
    return &window;
}

void GameField::move_border(float pos) {
    border_pos = pos;
}
