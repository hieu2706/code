#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdio>
#include <fstream>
#include <cstring>
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
    std::string imgpath = "";
    std::string audiopath = "";
    State state;
};

class Setting {
public:
    std::string app_title, splash_state, splash_img_path, questions_answers_path, states_img_path, background_img_path, font_path, background_music_path;
    int app_width, app_height, framerate_limit, block_size, splash_time, not_answered_state, chosen_state, right_state, wrong_state, font_size;
    float characters_xoffset, characters_yoffset, background_music_volume;

    //Setting();

    void getConfig(std::string configFilePath) {
        std::fstream ifs(configFilePath);
        std::string temp;
        while (std::getline(ifs, temp)) {
            std::string delimiter = " = ";

            size_t pos = 0;
            std::string option;
            while ((pos = temp.find(delimiter)) != std::string::npos) {
                option = temp.substr(0, pos);
                temp.erase(0, pos + delimiter.length());
            }
            if (!strcmp(option.c_str(), "app-title"))
                app_title = temp;
            else if (!strcmp(option.c_str(), "app-width"))
                app_width = std::stoi(temp);
            else if (!strcmp(option.c_str(), "app-height"))
                app_height = std::stoi(temp);
            else if (!strcmp(option.c_str(), "framerate-limit"))
                framerate_limit = std::stoi(temp);
            else if (!strcmp(option.c_str(), "block-size"))
                block_size = std::stoi(temp);
            else if (!strcmp(option.c_str(), "splash_state"))
                splash_state = temp;
            else if (!strcmp(option.c_str(), "splash-img-path"))
                splash_img_path = temp;
            else if (!strcmp(option.c_str(), "splash-time"))
                splash_time = std::stoi(temp);
            else if (!strcmp(option.c_str(), "questions-answers-path"))
                questions_answers_path = temp;
            else if (!strcmp(option.c_str(), "states-img-path"))
                states_img_path = temp;
            else if (!strcmp(option.c_str(), "not-answered-state"))
                not_answered_state = std::stoi(temp);
            else if (!strcmp(option.c_str(), "chosen-state"))
                chosen_state = std::stoi(temp);
            else if (!strcmp(option.c_str(), "right-state"))
                right_state = std::stoi(temp);
            else if (!strcmp(option.c_str(), "wrong-state"))
                wrong_state = std::stoi(temp);
            else if (!strcmp(option.c_str(), "background-img-path"))
                background_img_path = temp;
            else if (!strcmp(option.c_str(), "font-path"))
                font_path = temp;
            else if (!strcmp(option.c_str(), "font-size"))
                font_size = std::stoi(temp);
            else if (!strcmp(option.c_str(), "characters-xoffset"))
                characters_xoffset = std::stof(temp);
            else if (!strcmp(option.c_str(), "characters-yoffset"))
                characters_yoffset = std::stof(temp);
            else if (!strcmp(option.c_str(), "background-music-path"))
                background_music_path = temp;
            else if (!strcmp(option.c_str(), "background-music-volume"))
                background_music_volume = std::stof(temp);
        }
        ifs.close();
    }
};

int main() {
    std::string configFilePath = "data/config.ini";
    Setting config;
    config.getConfig(configFilePath);
    sf::VideoMode windowBounds(config.app_width, config.app_height);
    bool isEnd = false;

    // Splash State
    sf::Texture textureSplash;
    if (!textureSplash.loadFromFile(config.splash_img_path))
        return 1;
    textureSplash.setSmooth(true);
    sf::Vector2u splashImgSize = textureSplash.getSize();
    sf::Sprite spriteSplash;
    spriteSplash.setTexture(textureSplash);

    sf::RenderWindow window(sf::VideoMode((float)splashImgSize.x, (float)splashImgSize.y), config.app_title);
    window.setFramerateLimit(config.framerate_limit);
    auto desktop = sf::VideoMode::getDesktopMode();
    window.setPosition(sf::Vector2i(desktop.width / 2 - window.getSize().x / 2, desktop.height / 2 - window.getSize().y / 2));

    //if (config.splash_state == "yes") {
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e))
            if (e.type == sf::Event::Closed)
                window.close();

        window.clear();

        window.draw(spriteSplash);
        window.display();
        sf::sleep(sf::milliseconds(config.splash_time));

        window.close();
    }
    //}

    // Game State
    int lineNum = 1;
    std::vector<Line> lines;
    std::fstream ifs(config.questions_answers_path);
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
                ifs >> lines.at(i).imgpath;
                ifs.get();
                std::getline(ifs, lines.at(i).question);
            }
            if (lines.at(i).question == "Play music") {
                ifs >> lines.at(i).audiopath;
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

    sf::Music music;
    if (!music.openFromFile(config.background_music_path))
        return -1;
    music.setVolume(config.background_music_volume);
    music.setLoop(true);
    music.play();

    sf::Texture squares;
    if (!squares.loadFromFile(config.states_img_path))
        return 1;
    sf::Sprite block;
    block.setTexture(squares);

    sf::Texture t_bg;
    if (!t_bg.loadFromFile(config.background_img_path))
        return 1;
    sf::Sprite s_bg;
    s_bg.setTexture(t_bg);

    window.create(windowBounds, config.app_title);
    window.setPosition(sf::Vector2i(desktop.width / 2 - window.getSize().x / 2, desktop.height / 2 - window.getSize().y / 2));

    sf::Font font;
    font.loadFromFile(config.font_path);
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(config.font_size);

    int chosenLine = 0;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();
            if (e.type == sf::Event::MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                int mouseY = sf::Mouse::getPosition(window).y / config.block_size - 1;
                if (mouseY < 0)
                    break;
                int mouseX = sf::Mouse::getPosition(window).x / config.block_size;
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
                isEnd = true;
                for (int i = 0; i < lineNum; ++i)
                    lines.at(i).state = RIGHT;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && lines.at(chosenLine).state == CHOSEN) {
                if (lines.at(chosenLine).imgpath.length() > 1) {
                    sf::Texture t;
                    if (!t.loadFromFile(lines.at(chosenLine).imgpath))
                        return 1;
                    sf::Sprite s;
                    s.setTexture(t);
                    sf::RenderWindow picture(sf::VideoMode(t.getSize().x, t.getSize().y), lines.at(chosenLine).question, sf::Style::Titlebar);
                    picture.setPosition(sf::Vector2i(desktop.width / 2 - picture.getSize().x / 2, desktop.height / 2 - picture.getSize().y / 2));

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
                    sf::Music audio;
                    bool isPlayed = true;
                    if (lines.at(chosenLine).audiopath.length() > 1) {
                        music.pause();
                        if (!audio.openFromFile(lines.at(chosenLine).audiopath))
                            return -1;
                        audio.play();
                        isPlayed = false;
                    }
                    sf::RenderWindow wnd(sf::VideoMode(1200, 200), "Question", sf::Style::Titlebar | sf::Style::Close);
                    wnd.setPosition(sf::Vector2i(desktop.width / 2 - wnd.getSize().x / 2, desktop.height / 2 - wnd.getSize().y / 2));
                    std::string lettersNum = "(" + std::to_string(lines.at(chosenLine).answer.length()) + " letters)";
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
                            text.setPosition(sf::Vector2f(wnd.getSize().x / 2.0f, wnd.getSize().y / 2.0f - 20.f));
                            wnd.draw(text);
                            text.setString(lines.at(chosenLine).question2);
                            text.setOrigin(text.getLocalBounds().width / 2.f, text.getLocalBounds().height);
                            text.setPosition(sf::Vector2f(wnd.getSize().x / 2.0f, wnd.getSize().y / 2.0f + 20.f));
                            wnd.draw(text);
                        }
                        else {
                            text.setString(lines.at(chosenLine).question);
                            text.setOrigin(text.getLocalBounds().width / 2.f, text.getLocalBounds().height);
                            text.setPosition(sf::Vector2f(wnd.getSize().x / 2.0f, wnd.getSize().y / 2.0f - 20.f));
                            wnd.draw(text);
                            text.setString(lettersNum);
                            text.setOrigin(text.getLocalBounds().width / 2.f, text.getLocalBounds().height);
                            text.setPosition(sf::Vector2f(wnd.getSize().x / 2.0f, wnd.getSize().y / 2.0f + 20.f));
                            wnd.draw(text);
                        }
                        wnd.display();
                    }
                    if (!wnd.isOpen() && !isPlayed) {
                        music.play();
                    }
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && isEnd == true) {
                music.pause();
                sf::Music audio;
                if (!audio.openFromFile("audio/Mistletoe.ogg"))
                    return -1;
                audio.play();
                sf::Texture t;
                if (!t.loadFromFile("img/kissundermistletoe.jpg"))
                    return 1;
                sf::Sprite s;
                s.setTexture(t);
                sf::RenderWindow picture(sf::VideoMode(t.getSize().x, t.getSize().y), "Finally answer", sf::Style::Titlebar | sf::Style::Close);
                picture.setPosition(sf::Vector2i(desktop.width / 2 - picture.getSize().x / 2, desktop.height / 2 - picture.getSize().y / 2));

                while (picture.isOpen()) {
                    while (picture.pollEvent(e))
                        if (e.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                            picture.close();

                    picture.clear();
                    picture.draw(s);
                    picture.display();
                }
            }
        }

        window.clear();
        window.draw(s_bg);

        for (int i = 0; i < lineNum; ++i) {
            text.setString(std::to_string(i+1));
            text.setOrigin(text.getLocalBounds().width / 2.f, text.getLocalBounds().height);
            text.setPosition(30.f, (float)((i + 1) * config.block_size) + 27.f);
            text.setFillColor(sf::Color::Red);
            window.draw(text);

            for (int j = 0; j < lines.at(i).length; ++j) {

                if (lines.at(i).state == NOT_ANSWERD)
                    block.setTextureRect(sf::IntRect(config.not_answered_state * config.block_size, 0, config.block_size, config.block_size));
                else if (lines.at(i).state == CHOSEN)
                    block.setTextureRect(sf::IntRect(config.chosen_state * config.block_size, 0, config.block_size, config.block_size));
                else if (lines.at(i).state == WRONG)
                    block.setTextureRect(sf::IntRect(config.wrong_state * config.block_size, 0, config.block_size, config.block_size));
                else if (lines.at(i).state == RIGHT)
                    block.setTextureRect(sf::IntRect(config.right_state * config.block_size, 0, config.block_size, config.block_size));
                int positionX = config.block_size * lines.at(i).x;
                block.setPosition(positionX + config.block_size * j, config.block_size * (i + 1));
                window.draw(block);
            }
        }
        for (int i = 0; i < lineNum; ++i) {
            for (int j = 0; j < lines.at(i).length; ++j) {
                if (lines.at(i).state == RIGHT) {
                    text.setString(lines.at(i).answer.at(j));
                    text.setOrigin(text.getLocalBounds().width / 2.f, text.getLocalBounds().height);
                    text.setPosition((float)(config.block_size * (lines.at(i).x + j)) + 24.f, (float)((i + 1) * config.block_size) + 27.f);
                    text.setFillColor(sf::Color::Red);
                    if (j + 1 == lines.at(i).keywordNum)
                        text.setFillColor(sf::Color(2,97,16));
                    window.draw(text);
                }
            }
        }

        window.display();
    }

    return 0;
}
