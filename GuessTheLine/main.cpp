#include <SFML/Graphics.hpp>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

enum State {
    NOT_ANSWERD,
    CHOSEN,
    RIGHT,
    WRONG
};

struct Line {
    std::string question;
    std::string question2;
    int questionLine = 0;
    std::string answer;
    int length;
    int x;
    int keywordNum;
    std::string path = "";
    State state;
};

int main() {
    std::fstream ifs("data/config.ini");
    std::string title = "None";
    sf::VideoMode windowBounds(800, 600);
    unsigned framerateLimit = 60;
    
    if (ifs.is_open()) {
        std::getline(ifs, title);
        ifs >> windowBounds.width >> windowBounds.height;
        ifs >> framerateLimit;
    }
    ifs.close();
    
    // Splash State
    sf::Texture fpt_logo;
    if (!fpt_logo.loadFromFile("img/fptuni.png"))
        return 1;
    fpt_logo.setSmooth(true);
    sf::Vector2u textureSize = fpt_logo.getSize();
    sf::Sprite fptSprite;
    fptSprite.setTexture(fpt_logo);

    sf::RenderWindow window(sf::VideoMode((float)textureSize.x, (float)textureSize.y), title);
    window.setFramerateLimit(framerateLimit);
    auto desktop = sf::VideoMode::getDesktopMode();
    window.setPosition(sf::Vector2i(desktop.width/2 - window.getSize().x/2, desktop.height/2 - window.getSize().y/2));

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e))
            if (e.type == sf::Event::Closed)
                window.close();

        window.clear();

        window.draw(fptSprite);
        window.display();
        sf::sleep(sf::milliseconds(3000));

        window.close();
    }

    // Game State
    int lineNum = 1;
    std::vector<Line> lines;
    ifs.open("data/questions_and_answers.ini");
    if (ifs.is_open()) {
        std::string s;
        ifs >> lineNum;
        ifs.get();
        std::getline(ifs, s);
        lines.resize(lineNum);
        for (int i = 0; i < lineNum; ++i) {
            std::getline(ifs, lines.at(i).question);
            if (lines.at(i).question.length() == 1) {
                lines.at(i).questionLine = std::stoi(lines.at(i).question);
                std::getline(ifs, lines.at(i).question);
                std::getline(ifs, lines.at(i).question2);
            }
            if (lines.at(i).question == "Show picture") {
                ifs >> lines.at(i).path;
                ifs.get();
                std::getline(ifs, lines.at(i).question);
            }
            ifs >> lines.at(i).answer >> lines.at(i).keywordNum >> lines.at(i).x;
            ifs.get();
            std::getline(ifs, s);
            lines.at(i).length = lines.at(i).answer.length();
            lines.at(i).state = NOT_ANSWERD;
        }
    }
    else
        return 1;
    ifs.close();

    sf::Texture squares;
    if (!squares.loadFromFile("img/states.png"))
        return 1;
    sf::Sprite block;
    block.setTexture(squares);

    sf::Texture t_bg;
    if (!t_bg.loadFromFile("img/background.jpeg"))
        return 1;
    sf::Sprite s_bg;
    s_bg.setTexture(t_bg);

    window.create(windowBounds, title);
    window.setPosition(sf::Vector2i(desktop.width/2 - window.getSize().x/2, desktop.height/2 - window.getSize().y/2));

    sf::Font font;
    font.loadFromFile("fonts/arial.ttf");
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(26);

    int chosenLine = 0;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();
            if (e.type == sf::Event::MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                int mouseY = sf::Mouse::getPosition(window).y / 32 - 1;
                if (mouseY < 0)
                    break;
                int mouseX = sf::Mouse::getPosition(window).x / 32;
                if (mouseX >= lines.at(mouseY).x && mouseX < lines.at(mouseY).length + lines.at(mouseY).x 
                            && lines.at(chosenLine).state != CHOSEN && lines.at(mouseY).state == NOT_ANSWERD) {
                    lines.at(mouseY).state = CHOSEN;
                    chosenLine = mouseY;
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::X) && lines.at(chosenLine).state == CHOSEN)
                lines.at(chosenLine).state = WRONG;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::V) && lines.at(chosenLine).state == CHOSEN)
                lines.at(chosenLine).state = RIGHT;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                for (int i = 0; i < lineNum; ++i)
                    lines.at(i).state = RIGHT;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && lines.at(chosenLine).state == CHOSEN) {
                if (lines.at(chosenLine).path.length() > 1) {
                    sf::Texture t;
                    if (!t.loadFromFile(lines.at(chosenLine).path))
                        return 1;
                    sf::Sprite s;
                    s.setTexture(t);
                    sf::RenderWindow picture(sf::VideoMode(t.getSize().x, t.getSize().y), lines.at(chosenLine).question, sf::Style::Titlebar);
                    picture.setPosition(sf::Vector2i(desktop.width/2 - picture.getSize().x/2, desktop.height/2 - picture.getSize().y/2));

                    while (picture.isOpen()) {
                        while (picture.pollEvent(e))
                            if (e.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                                picture.close();

                        picture.clear();
                        picture.draw(s);
                        picture.display();
                    }
                }
                else {
                    sf::RenderWindow wnd(sf::VideoMode(1200, 200), "Question", sf::Style::Titlebar);
                    wnd.setPosition(sf::Vector2i(desktop.width/2 - wnd.getSize().x/2, desktop.height/2 - wnd.getSize().y/2));
                    while (wnd.isOpen()) {
                        while (wnd.pollEvent(e))
                            if (e.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                                wnd.close();

                        wnd.clear(sf::Color::White);

                        text.setCharacterSize(30);
                        text.setFillColor(sf::Color::Black);
                        if (lines.at(chosenLine).questionLine == 2) {
                            text.setString(lines.at(chosenLine).question);
                            text.setOrigin(text.getLocalBounds().width / 2.f, text.getLocalBounds().height);
                            text.setPosition(sf::Vector2f(wnd.getSize().x/2.0f, wnd.getSize().y/2.0f - 20.f));
                            wnd.draw(text);
                            text.setString(lines.at(chosenLine).question2);
                            text.setOrigin(text.getLocalBounds().width / 2.f, text.getLocalBounds().height);
                            text.setPosition(sf::Vector2f(wnd.getSize().x/2.0f, wnd.getSize().y/2.0f + 20.f));
                            wnd.draw(text);
                        }
                        else {
                            text.setString(lines.at(chosenLine).question);
                            text.setOrigin(text.getLocalBounds().width / 2.f, text.getLocalBounds().height);
                            text.setPosition(sf::Vector2f(wnd.getSize().x/2.0f, wnd.getSize().y/2.0f));
                            wnd.draw(text);
                        }
                        wnd.display();
                    }
                }
            }
        }

        window.clear();
        window.draw(s_bg);

        for (int i = 0; i < lineNum; ++i) {
            for (int j = 0; j < lines.at(i).length; ++j) {

                if (lines.at(i).state == NOT_ANSWERD)
                    block.setTextureRect(sf::IntRect(13 * 32, 0, 32, 32));
                else if (lines.at(i).state == CHOSEN)
                    block.setTextureRect(sf::IntRect(11 * 32, 0, 32, 32));
                else if (lines.at(i).state == WRONG)
                    block.setTextureRect(sf::IntRect(10 * 32, 0, 32, 32));
                else if (lines.at(i).state == RIGHT)
                    block.setTextureRect(sf::IntRect(0, 0, 32, 32));
                int positionX = 32 * lines.at(i).x;
                block.setPosition(positionX + 32 * j, 32 * (i + 1));
                window.draw(block);
            }
        }
        for (int i = 0; i < lineNum; ++i) {
            for (int j = 0; j < lines.at(i).length; ++j) {
                if (lines.at(i).state == RIGHT) {
                    text.setString(lines.at(i).answer.at(j));
                    text.setOrigin(text.getLocalBounds().width / 2.f, text.getLocalBounds().height);
                    text.setPosition((float)(32 * (lines.at(i).x + j)) + 15.f, (float)((i + 1) * 32) + 18.f);
                    text.setFillColor(sf::Color::Red);
                    if (j+1 == lines.at(i).keywordNum)
                        text.setFillColor(sf::Color::Black);
                    window.draw(text);
                }
            }
        }

        window.display();
    }

    return 0;
}
