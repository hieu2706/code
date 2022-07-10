// Author: Nguyễn Hồng Hiệp
// Facebook: www.facebook.com/syaorancode

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <string>
#include <time.h>
#include <random>
#include <ctime>
#include <fstream>

#define WIDTH 1023
#define HEIGHT 666

class Item {
public:
    int x, y, col, row, kind, match, alpha;

    Item() {
        match = 0;
        alpha = 255;
    }
} grid[12][12];

namespace MyRandom {
	std::mt19937 mersenne{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
}
 
int getRandomNumber(int min, int max)
{
	std::uniform_int_distribution<> die{ min, max };
	return die(MyRandom::mersenne);
}

void swapItems(Item i1, Item i2) {
    std::swap(i1.col, i2.col);
    std::swap(i1.row, i2.row);

    grid[i1.row][i1.col] = i1;
    grid[i2.row][i2.col] = i2;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Jewel Christmas Game", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    auto desktop = sf::VideoMode::getDesktopMode();
    window.setPosition(sf::Vector2i(desktop.width/2 - window.getSize().x/2, desktop.height/2 - window.getSize().y/2));

	std::string song = "audio/3.ogg";

    sf::Music music;
    if (!music.openFromFile(song))
        return -1;
    music.setVolume(15.f);
    music.play();

    int highscore = 0, nScore = 0;
    std::ifstream ifs("data/highscore.ini");
    if (ifs.is_open()) {
        ifs >> highscore;
    }
    ifs.close();

    sf::Texture t1, t2, t3;
    t1.loadFromFile("images/background.png");
    t2.loadFromFile("images/items.png");
    t3.loadFromFile("images/cursor.png");

    sf::Sprite background(t1), items(t2), cursor(t3);

    sf::Font font;
    font.loadFromFile("fonts/font.ttf");
    sf::Text text, textHighscore, textScore, textClock, textSpace;

    text.setFont(font);
    textHighscore.setFont(font);
    textScore.setFont(font);
    textClock.setFont(font);
    textSpace.setFont(font);

    text.setFillColor(sf::Color::Red);
    textHighscore.setFillColor(sf::Color::Red);
    textScore.setFillColor(sf::Color::Red);
    textClock.setFillColor(sf::Color::Red);
    textSpace.setFillColor(sf::Color::Red);

    text.setPosition(700.f, 150.f);
    textHighscore.setPosition(700.f, 300.f);
    textScore.setPosition(700.f, 350.f);
    textClock.setPosition(700.f, 450.f);
    textSpace.setPosition(340.f, 620.f);

    text.setCharacterSize(50);
    textHighscore.setCharacterSize(40);
    textScore.setCharacterSize(40);
    textClock.setCharacterSize(40);

    text.setString("Merry Christmas!");
    textHighscore.setString("Highest score : " + std::to_string(highscore));
    textSpace.setString("Press SPACE to play again or X to exit.");

    sf::Clock clock;
    int elapsedTime;

    int blocksize = 54;
    sf::Vector2i offset(109, 78);

    for (int i=1; i<=10; ++i)
        for (int j=1; j<=10; ++j) {
            grid[i][j].kind = getRandomNumber(0, 5);
            grid[i][j].col = j;
            grid[i][j].row = i;
            grid[i][j].x = j*blocksize;
            grid[i][j].y = i*blocksize;
        }

    int x0, y0, x, y, click=0;
    sf::Vector2i pos;
    bool isSwap=false, isMoving=false, isOver = false;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();

            if (e.type == sf::Event::MouseButtonPressed)
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                   if (!isSwap && !isMoving)
                       click++;
                   pos = sf::Mouse::getPosition(window)-offset;
                }
        }

        if (click==1) {
          x0=pos.x/blocksize+1;
          y0=pos.y/blocksize+1;
        }
        if (click==2) {
          x=pos.x/blocksize+1;
          y=pos.y/blocksize+1;
          if (abs(x-x0)+abs(y-y0)==1) {
              swapItems(grid[y0][x0],grid[y][x]);
              isSwap = true;
              click=0;
          }
          else 
              click=1;
        }

        for(int i=1; i<=10; i++)
            for(int j=1; j<=10; j++) {
                if (grid[i][j].kind == grid[i+1][j].kind)
                    if (grid[i][j].kind == grid[i-1][j].kind)
                        for(int n=-1; n<=1; n++)
                            grid[i+n][j].match++;

                if (grid[i][j].kind == grid[i][j+1].kind)
                    if (grid[i][j].kind == grid[i][j-1].kind)
                        for(int n=-1; n<=1; n++)
                            grid[i][j+n].match++;
        }

        isMoving=false;
        for (int i=1; i<=10; i++)
            for (int j=1; j<=10; j++) {
                Item &p = grid[i][j];
                int dx,dy;
                for(int n=0;n<4;n++) {
                    dx = p.x-p.col*blocksize;
                    dy = p.y-p.row*blocksize;
                    if (dx)
                        p.x-=dx/abs(dx);
                    if (dy)
                        p.y-=dy/abs(dy);}
                    if (dx||dy)
                        isMoving = true;
            }

        if (!isMoving)
            for (int i=1; i<=10; i++)
                for (int j=1; j<=10; j++)
                    if (grid[i][j].match)
                        if (grid[i][j].alpha>10) {
                            grid[i][j].alpha-=10;
                            isMoving=true;
                        }

        int score=0;
        for (int i=1; i<=10; i++)
            for (int j=1; j<=10; j++)
                score+=grid[i][j].match;

        if (isSwap && !isMoving) {
            if (!score)
                swapItems(grid[y0][x0],grid[y][x]);
            isSwap = false;
        }

        if (!isMoving) {
            for(int i=10; i>0; i--)
                for(int j=1; j<=10; j++)
                    if (grid[i][j].match)
                        for(int n=i; n>0; n--)
                            if (!grid[n][j].match) {
                                swapItems(grid[n][j],grid[i][j]);
                                break;
                            };

        for(int j=1; j<=10; j++)
            for(int i=10, n=0; i>0; i--)
                if (grid[i][j].match != 0) {
                    grid[i][j].kind = getRandomNumber(0, 5);
                    grid[i][j].y = -blocksize*n++;
                    grid[i][j].match=0;
                    grid[i][j].alpha = 255;
                    nScore += 10;
               }
        }

        window.draw(background);

        for (int i=1; i<=10; i++)
            for (int j=1; j<=10; j++) {
                Item p = grid[i][j];
                items.setTextureRect(sf::IntRect(p.kind*49,0,49,49));
                items.setColor(sf::Color(255,255,255,p.alpha));
                items.setPosition(p.x,p.y);
                items.move(offset.x-blocksize,offset.y-blocksize);
                window.draw(items);
            }

        cursor.setPosition((x0-1)*blocksize + offset.x, (y0-1)*blocksize + offset.y);
        if (click!=0)
            window.draw(cursor);

        window.draw(text);

        elapsedTime = clock.getElapsedTime().asSeconds();
        textClock.setString("Time : " + std::to_string(60 - elapsedTime));
        if (elapsedTime == 60) {
            isOver = true;
            if (nScore > highscore) {
                highscore = nScore;
                textHighscore.setString("Highest score : " + std::to_string(highscore));
                std::ofstream ofs("data/highscore.ini");
                ofs << highscore;
                ofs.close();
            }
            textScore.setString("Your score is " + std::to_string(nScore));
            textClock.setCharacterSize(50);
            textClock.setString("Game Over!");
            music.stop();

            window.draw(textSpace);
        }
        else
            textScore.setString("Score : " + std::to_string(nScore));

        window.draw(textHighscore);
        window.draw(textScore);
        window.draw(textClock);

        window.display();

        while (isOver) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                isOver = false;
                clock.restart();
                nScore = 0;
				song = "audio/" + std::to_string(getRandomNumber(1, 2)) + ".ogg";
				music.openFromFile(song);
                music.play();

                for (int i=1; i<=10; ++i)
                    for (int j=1; j<=10; ++j) {
                        grid[i][j].kind = rand()%6;
                        grid[i][j].col = j;
                        grid[i][j].row = i;
                        grid[i][j].x = j*blocksize;
                        grid[i][j].y = i*blocksize;
                    }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
                isOver = false;
                window.close();
            }
        }
    }


    return 0;
}
