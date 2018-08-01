#include "contact_listener.hpp"
#include "physics_object.hpp"
#include <iostream>
void ContactListener::BeginContact(b2Contact* contact) {
    PhysicsObject* bodyUserData = static_cast<PhysicsObject*>(contact->GetFixtureA()->GetBody()->GetUserData());
    Bullet* bullet = NULL;
    PhysicsObject* obj = NULL;
    Entity* e = NULL;

    if (dynamic_cast<Bullet*>(bodyUserData)) {
        bullet = dynamic_cast<Bullet*>(bodyUserData);
    }
    else if (dynamic_cast<Entity*>(bodyUserData)) {
        e = dynamic_cast<Entity*>(bodyUserData);
    } else {
      obj = dynamic_cast<PhysicsObject*>(bodyUserData);
    }
   
    
    bodyUserData = static_cast<PhysicsObject*>(contact->GetFixtureB()->GetBody()->GetUserData());

    if (dynamic_cast<Bullet*>(bodyUserData)) {
        bullet = dynamic_cast<Bullet*>(bodyUserData);
    } else if (dynamic_cast<Entity*>(bodyUserData)) {
        e = dynamic_cast<Entity*>(bodyUserData);
    }
    else {
      obj = dynamic_cast<PhysicsObject*>(bodyUserData);
    }
    if (obj && bullet) {
        actions.push_front(new HitPhysicsAction(bullet, obj));
    }

    if (obj && e) {
        actions.push_front(new EntityPhysicsAction(e, obj));
    }
  
}

void ContactListener::EndContact(b2Contact* contact) {
    (void) contact;
}

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
    (void) impulse;
    (void) contact;
    /*
    PhysicsObject* bodyUserData = static_cast<PhysicsObject*>(contact->GetFixtureA()->GetBody()->GetUserData());
    Bullet* bullet = NULL;

    if (dynamic_cast<Bullet*>(bodyUserData)) {
        bullet = dynamic_cast<Bullet*>(bodyUserData);
    }

    bodyUserData = static_cast<PhysicsObject*>(contact->GetFixtureB()->GetBody()->GetUserData());

    if (dynamic_cast<Bullet*>(bodyUserData)) {
        bullet = dynamic_cast<Bullet*>(bodyUserData);
    }
    if (bullet) {
        delete bullet;
    }*/
}

void ContactListener::execute_actions(GameField& field) {
    for (auto i = actions.begin(); i != actions.end(); ) {
        (*i)->execute(field);
        delete *i;
        i = actions.erase(i);
    }
}

