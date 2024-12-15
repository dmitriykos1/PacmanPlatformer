#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include "Levels/l1.h"
#include <math.h>

sf::RenderWindow window(sf::VideoMode(800, 600), "Game");
sf::Clock clk;
unsigned int Level = 1;



class Player {
private:
    float Px, Py;
    float PRadius;
    sf::Sprite shape;
    bool JumpState;
    sf::Texture txtr, txtl;

public:
    Player(float _Px, float _Py, float _PRadius) {
        Px = _Px;
        Py = _Py;
        PRadius = _PRadius;
        txtl.loadFromFile("Images/playerL.png");
        txtr.loadFromFile("Images/playerR.png");
        shape.setTexture(txtr, true);
        shape.setScale(0.1, 0.1);
        shape.setOrigin(PRadius/2, PRadius/2);
        shape.setPosition(Px, Py);
    }
    void Draw() {
        window.draw(shape);
    }

    void Move() {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            Px-=0.1;
            shape.setTexture(txtl, true);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            Px+=0.1;
            shape.setTexture(txtr, true);
        }
        shape.setPosition(Px, Py);
    }

    void Jump() {
        static int i = 130;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && JumpState) {
            i--;
            Py -= 1;
            if(i == 1) {
                i = 130;
                JumpState = 0;
            }
        }
    }

    void GeForce(float Ground) {
        static float a = 0;
        if(shape.getPosition().y + PRadius < Ground) {
            a += 0.01;
            Py += 0.02 * a;

        } else {
            JumpState = 1;
            a = 0;
        }
    }

    sf::Vector2f GetPos() {
        return shape.getPosition();
    }
};


class Map {
private:
    std::vector<sf::Sprite> m;
    sf::Texture Plat;
    sf::Texture Door;
public:
    Map() {}

    void LoadMap(char _m[], int s) {
        m.clear();
        for(int i = 0; i < s; i++) {
            sf::Sprite Obst;
            m.push_back(Obst);
            if(_m[i] == '1') {
                Plat.loadFromFile("Images/platform.jpg");
                m[i].setTexture(Plat, true);
                m[i].setScale(0.1, 0.1);
                m[i].setPosition(float((i % 10) * 80), float((int(i / 10)) * 60));
            }
            if(_m[i] == 'D') {
                Door.loadFromFile("Images/door.png");
                m[i].setTexture(Door, true);
                m[i].setScale(0.2, 0.2);
                m[i].setPosition(float((i % 10) * 80), float((int(i / 10)) * 60));
            }
        }
    }

    void DrawMap() {
        for(int i = 0; i < m.size(); i++) {
            window.draw(m[i]);
        }
    }

    float GetColision(sf::Vector2f p) {
        float g;
        for(int i = 0; i < m.size(); i++) {
            g = 600;
            if(abs(m[i].getPosition().y - (p.y+102.4)) <= 1 && (p.x + 25.6 < m[i].getPosition().x + 80 && p.x + 76.8 > m[i].getPosition().x)) {
                if(m[i].getTexture()->getNativeHandle() == 4)
                    g = m[i].getPosition().y;
                if(m[i].getTexture()->getNativeHandle() == 3)
                    Level += 1;
                break;
            }
        }
        return g;
    }
};


int main() {
    Player* p = new Player(300, 550, 102.4);
    Map* m = new Map();

    m->LoadMap(Map1, 100);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }
        if(clk.getElapsedTime().asMicroseconds() > 500) {
            p->Move();
            p->GeForce(m->GetColision(p->GetPos()));
            p->Jump();
            window.clear();
            m->DrawMap();
            p->Draw();
            if(Level == 2) break;
            window.display();
            clk.restart();
        }
    }
    delete p;
    delete m;
    return 0;
}
