#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Mouse.hpp>
#include "Levels/l1.h"
#include <math.h>


sf::RenderWindow window(sf::VideoMode(800, 600, 32), "Game", sf::Style::Default);
sf::Clock clk;
unsigned int Level = 1;
bool Life = 1;
bool Start = 0;

class Player {
private:
    float Px, Py;
    float PRadius;
    sf::Sprite shape;
    bool JumpState;
    bool LeftRight;
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
        if(!Life) Start = 0;
         if(LeftRight) {
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                Px -= 0.1;
                shape.setTexture(txtl, true);
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                Px += 0.1;
                shape.setTexture(txtr, true);
            }
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

    void GeForce(sf::Vector2f Ground) {
        static float a = 0;

        if (Ground.x > 0) {
            LeftRight = 0;
            JumpState = 1;
        } else {
            LeftRight = 1;
            if(shape.getPosition().y + PRadius < Ground.y) {
                a += 0.01;
                Py += 0.02 * a;
            } else {
                JumpState = 1;
                a = 0;
            }
        }
    }

    sf::Vector2f GetPos() {
        return shape.getPosition();
    }

    void setPos(float x, float y) {
        shape.setPosition(x, y);
    }
};


class RunningEnemy {
private:
    float Ex, Ey;
    float ERadius;
    sf::Sprite Esprite;
    sf::Texture Etxtr, Etxtl;
    bool JumpState = 1;
    bool NeedToJump = 0;
    float Ground = 600;

    void Jump(bool NTJ) {
        static int i = 130;
        if(NTJ && JumpState) {
            i--;
            Ey -= 1;
            if(i == 1) {
                i = 130;
                JumpState = 0;
                NeedToJump = 0;
            }
        }
    }

    void GeForce() {
        static float a = 0;
        if(Esprite.getPosition().y + ERadius < Ground) {
            a += 0.01;
            Ey += 0.02 * a;

        } else {
            JumpState = 1;
            a = 0;
        }
    }

public:
    RunningEnemy(float _Ex, float _Ey, float _ERadius) {
        Ex = _Ex;
        Ey = _Ey;
        ERadius = _ERadius;
        Etxtl.loadFromFile("Images/renemyL.png");
        Etxtr.loadFromFile("Images/renemyR.png");
        Esprite.setTexture(Etxtr, true);
        Esprite.setScale(0.1, 0.1);
        Esprite.setOrigin(ERadius/2, ERadius/2);
        Esprite.setPosition(Ex, Ey);
    }

    sf::Sprite GetSprite() {
        return Esprite;
    }

    void Draw() {
        window.draw(Esprite);
    }

    void Collide(sf::Vector2f cv) {
        Ground = cv.y;
        if(cv.x > 0) {
            NeedToJump = 1;
        }
    }

    void Move(sf::Vector2f v) {
        GeForce();
        Jump(NeedToJump);
        if(Ex > v.x && !NeedToJump) {
            Ex -= 0.05f;
            Esprite.setTexture(Etxtl, true);
        }
        if(Ex < v.x && !NeedToJump) {
            Ex += 0.05f;
            Esprite.setTexture(Etxtr, true);
        }

        Esprite.setPosition(Ex, Ey);
    }

    void Catch(sf::Vector2f v, float pw) {
        if((Ex > v.x && Ex < v.x + pw) && (Ey > v.y && Ey < v.y + pw)) {
            Life = 0;
        }
    }

    sf::Vector2f getPos() {
        return Esprite.getPosition();
    }
};


class FlyingEnemy {
private:
    float Ex, Ey;
    float ERadius;
    sf::Sprite Esprite;
    sf::Texture Etxtr, Etxtl;

public:
    FlyingEnemy(float _Ex, float _Ey, float _ERadius) {
        Ex = _Ex;
        Ey = _Ey;
        ERadius = _ERadius;
        Etxtl.loadFromFile("Images/fenemyL.jpg");
        Etxtr.loadFromFile("Images/fenemyR.jpg");
        Esprite.setTexture(Etxtr, true);
        Esprite.setScale(0.1, 0.1);
        Esprite.setOrigin(ERadius/2, ERadius/2);
        Esprite.setPosition(Ex, Ey);
    }

    sf::Sprite GetSprite() {
        return Esprite;
    }

    void Draw() {
        window.draw(Esprite);
    }

    void Move(sf::Vector2f v) {
        if(Ex > v.x) {
            Ex -= 0.05f;
            Esprite.setTexture(Etxtl, true);
        }
        if(Ex < v.x) {
            Ex += 0.05f;
            Esprite.setTexture(Etxtr, true);
        }
        if(Ey > v.y) Ey -= 0.02f;
        if(Ey < v.y) Ey += 0.02f;
        Esprite.setPosition(Ex, Ey);
    }

    void Catch(sf::Vector2f v, float pw) {
        if((Ex > v.x && Ex < v.x + pw) && (Ey > v.y && Ey < v.y + pw)) {
            Life = 0;
        }
    }
};


class Map {
private:
    std::vector<sf::Sprite> m;
    std::vector<FlyingEnemy> Fenemies;
    std::vector<RunningEnemy> Renemies;
    sf::Texture Plat;
    sf::Texture Door;
public:
    Map() {}

    void LoadMap(char _m[], int s) {
        m.clear();
        Fenemies.clear();
        Renemies.clear();
        for(int i = 0; i < s; i++) {
            sf::Sprite Obst;
            m.push_back(Obst);
            if(_m[i] == '1') {
                Plat.loadFromFile("Images/platform.jpg");
                m[i].setTexture(Plat, false);
                m[i].setScale(0.1, 0.1);
                m[i].setColor(sf::Color(255, 255, 255));
                m[i].setPosition(float((i % 10) * 80), float((int(i / 10)) * 60));
            }
            if(_m[i] == 'D') {
                Door.loadFromFile("Images/door.png");
                m[i].setTexture(Door, false);
                m[i].setScale(0.2, 0.2);
                m[i].setColor(sf::Color(250, 255, 255));
                m[i].setPosition(float((i % 10) * 80), float((int(i / 10)) * 60));
            }
            if(_m[i] == 'F') {
                FlyingEnemy Fe(float((i % 10) * 80), float((int(i / 10)) * 60), 102.4);
                Fenemies.push_back(Fe);
            }
            if(_m[i] == 'R') {
                RunningEnemy Re(float((i % 10) * 80), float((int(i / 10)) * 60), 102.4);
                Renemies.push_back(Re);
            }
        }
    }

    sf::Vector2f GetColision(sf::Vector2f p) {
        sf::Vector2f g;

        for(unsigned int i = 0; i < m.size(); i++) {
            g.y = 600;
            g.x = 0;



            if(abs(m[i].getPosition().y - (p.y+102.4)) <= 1 && (p.x < m[i].getPosition().x + 80 && p.x + 76.8 > m[i].getPosition().x)) {
                if(m[i].getColor().r == 255)
                    g.y = m[i].getPosition().y;
                if(m[i].getColor().r == 250)
                    Level += 1;
               break;
            }
            if((floor(p.y+102.4) - m[i].getPosition().y == 60) && (m[i].getPosition().x + 80 >= p.x && m[i].getPosition().x <= p.x+76.8)) {
                if(m[i].getColor().r == 255)
                    g.x = 1;
                if(m[i].getColor().r == 250)
                    Level += 1;
                break;
            }
        }
        return g;
    }

    void Live(sf::Vector2f pp) {
        for(int i = 0; i < m.size(); i++) {
            window.draw(m[i]);
        }
        for(int i = 0; i < Fenemies.size(); i++) {
            Fenemies[i].Move(pp);
            Fenemies[i].Catch(pp, 102.4);
            Fenemies[i].Draw();
        }
        for(int i = 0; i < Renemies.size(); i++) {
            Renemies[i].Move(pp);
            Renemies[i].Collide(GetColision(Renemies[i].getPos()));
            Renemies[i].Catch(pp, 102.4);
            Renemies[i].Draw();
        }
    }
};

class Menu {
private:
    sf::Sprite Play, Quit, Resume;
    sf::Texture PlayText, QuitText, ResumeText;
    sf::Vector2i CurPos;

public:
    Menu() {
        PlayText.loadFromFile("Images/PlayButton.png");
        Play.setTexture(PlayText);
        QuitText.loadFromFile("Images/QuitButton.png");
        Quit.setTexture(QuitText);
        ResumeText.loadFromFile("Images/ResumeButton.png");
        Resume.setTexture(ResumeText);

        Play.setPosition(sf::Vector2f(300, 50));
        Resume.setPosition(sf::Vector2f(300, 250));
        Quit.setPosition(sf::Vector2f(300, 450));
    }


    int HandleMouse() {
        CurPos = sf::Mouse::getPosition(window);
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if((300 <= CurPos.x && CurPos.x <= 500) && (50 <= CurPos.y && CurPos.y <= 150)) {
                std::cout << "Play" << std::endl;
                Start = 1;
                Life = 1;
            }
            if((300 <= CurPos.x && CurPos.x <= 500) && (250 <= CurPos.y && CurPos.y <= 350)) {
                std::cout << "Resume" << std::endl;
            }
            if((300 <= CurPos.x && CurPos.x <= 500) && (450 <= CurPos.y && CurPos.y <= 550)) {
                return -1;
            }
            std::cout << CurPos.x << "   " << CurPos.y << std::endl;
        }
    }

    void DrawElements() {
        window.clear(sf::Color(255, 255, 255));
        window.draw(Play);
        window.draw(Quit);
        window.draw(Resume);
    }
};

int main() {
    Player* p = new Player(100, 420, 102.4);
    Map* m = new Map();
    Menu* menu = new Menu();


    m->LoadMap(Map1, 100);
    p->setPos(PlayerPos1[0], PlayerPos1[1]);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }
        if(clk.getElapsedTime().asMicroseconds() > 500) {
            if(Start) {
                p->Move();
                p->GeForce(m->GetColision(p->GetPos()));
                p->Jump();
                window.clear();
                m->Live(p->GetPos());
                p->Draw();
                if(Level == 2) break;
                clk.restart();
            } else {
                if(menu->HandleMouse() == -1) {
                    break;
                }
                menu->DrawElements();
            }
        }
        window.display();
    }
    delete p;
    delete m;
    delete menu;
    return 0;
}
