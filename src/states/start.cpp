#include "start.h"

#include "map/map.h"

sf::Texture StateStart::assetBackground, StateStart::assetLogo;

const sf::Vector2f StateStart::ABS_MENU =
    sf::Vector2f(50.0f / BACKGROUND_WIDTH, 128.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateStart::ABS_CONTROLS =
    sf::Vector2f(28.0f / BACKGROUND_WIDTH, 12.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateStart::ABS_SETTINGS =
    sf::Vector2f(28.0f / BACKGROUND_WIDTH, 112.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateStart::ABS_LOGO =
    sf::Vector2f(128.0f / BACKGROUND_WIDTH, 26.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateStart::MENU_SIZE =
    sf::Vector2f(156.0f / BACKGROUND_WIDTH, 64.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateStart::ABS_MENU_CENTER = ABS_MENU + MENU_SIZE / 2.0f;
const sf::Time StateStart::TIME_MENU_TWEEN = sf::seconds(0.25f);
const sf::Vector2f StateStart::REL_TEXT1(20.0f / BACKGROUND_WIDTH,
                                         12.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateStart::REL_TEXT2(20.0f / BACKGROUND_WIDTH,
                                         28.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateStart::REL_TEXT3(20.0f / BACKGROUND_WIDTH,
                                         44.0f / BACKGROUND_HEIGHT);
const sf::Time StateStart::TIME_FADE_TOTAL = sf::seconds(0.25f);

const sf::Vector2f StateStart::CONTROLS_SIZE =
    sf::Vector2f(200.0f / BACKGROUND_WIDTH, 200.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateStart::ABS_CONTROLS_CENTER =
    ABS_CONTROLS + CONTROLS_SIZE / 2.0f;
const sf::Time StateStart::TIME_CONTROLS_TWEEN = sf::seconds(0.5f);
const sf::Vector2f StateStart::REL_CONTROL0 =
    sf::Vector2f(8.0f / BACKGROUND_WIDTH, 8.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateStart::REL_CONTROLDX =
    sf::Vector2f(110.0f / BACKGROUND_WIDTH, 0.0f);
const sf::Vector2f StateStart::REL_CONTROLDY =
    sf::Vector2f(0.0f, 10.0f / BACKGROUND_HEIGHT);

uint StateStart::resolutionMultiplier = 2;
const sf::Vector2f StateStart::SETTINGS_SIZE =
    sf::Vector2f(200.0f / BACKGROUND_WIDTH, 100.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateStart::ABS_SETTINGS_CENTER =
    ABS_SETTINGS + SETTINGS_SIZE / 2.0f;
const sf::Time StateStart::TIME_SETTINGS_TWEEN = sf::seconds(0.5f);
const sf::Vector2f StateStart::REL_SETTING0 =
    sf::Vector2f(8.0f / BACKGROUND_WIDTH, 8.0f / BACKGROUND_HEIGHT);
const sf::Vector2f StateStart::REL_SETTINGDX =
    sf::Vector2f(118.0f / BACKGROUND_WIDTH, 0.0f);
const sf::Vector2f StateStart::REL_SETTINGDY =
    sf::Vector2f(0.0f, 10.0f / BACKGROUND_HEIGHT);

void StateStart::loadBackgroundAssets(const std::string& assetName,
                                      const sf::IntRect& roiBackground,
                                      const sf::IntRect& roiLogo) {
    assetBackground.loadFromFile(assetName, roiBackground);
    assetBackground.setRepeated(true);
    assetLogo.loadFromFile(assetName, roiLogo);
}

void StateStart::init() {
    currentState = MenuState::NO_MENUS;
    backgroundPosition = 0.0f;
    timeSinceStateChange = sf::Time::Zero;
    selectedOption = 0;
    waitingForKeyPress = false;
    keyChangeRequested = false;
}

void StateStart::handleEvent(const sf::Event& event) {
    switch (currentState) {
        case MenuState::NO_MENUS:
            if (Input::pressed(Key::MENU_UP, event) ||
                Input::pressed(Key::MENU_DOWN, event) ||
                Input::pressed(Key::MENU_LEFT, event) ||
                Input::pressed(Key::MENU_RIGHT, event) ||
                Input::pressed(Key::ACCELERATE,
                               event)) {  // menu says "press X"
                currentState = MenuState::MENU_FADE_IN;
                timeSinceStateChange = sf::Time::Zero;
            } else if (Input::pressed(Key::CANCEL, event)) {
                game.popState();  // exit the game
            }
            break;
        case MenuState::MENU:
            if (Input::pressed(Key::ACCEPT, event) ||
                Input::pressed(Key::ACCELERATE, event)) {
                switch (MenuOption(selectedOption)) {
                    case MenuOption::GRAND_PRIX:
                        currentState = MenuState::GRAND_PRIX_FADE;
                        break;
                    case MenuOption::CONTROLS:
                        currentState = MenuState::CONTROLS_FADE_IN;
                        break;
                    case MenuOption::SETTINGS:
                        currentState = MenuState::SETTINGS_FADE_IN;
                        break;
                    default:
                        break;
                }
                timeSinceStateChange = sf::Time::Zero;
            } else if (Input::pressed(Key::CANCEL, event)) {
                currentState = MenuState::MENU_FADE_OUT;
                timeSinceStateChange = sf::Time::Zero;
            } else if (Input::pressed(Key::MENU_DOWN, event)) {
                selectedOption =
                    (selectedOption + 1) % (uint)MenuOption::__COUNT;
            } else if (Input::pressed(Key::MENU_UP, event)) {
                selectedOption =
                    (selectedOption - 1) % (uint)MenuOption::__COUNT;
            }
            break;
        case MenuState::CONTROLS:
            if (waitingForKeyPress && event.type == sf::Event::KeyPressed &&
                Input::getKeyCodeName(event.key.code) != "?") {
                waitingForKeyPress = false;
                Input::set(Key(selectedOption), event.key.code);
            } else {
                if (Input::pressed(Key::ACCEPT, event) ||
                    Input::pressed(Key::ACCELERATE, event)) {
                    keyChangeRequested = true;
                } else if (Input::pressed(Key::CANCEL, event)) {
                    currentState = MenuState::CONTROLS_FADE_OUT;
                    timeSinceStateChange = sf::Time::Zero;
                } else if (Input::pressed(Key::MENU_UP, event)) {
                    selectedOption = selectedOption == 0
                                         ? (uint)Key::__COUNT - 1
                                         : selectedOption - 1;
                } else if (Input::pressed(Key::MENU_DOWN, event)) {
                    selectedOption = selectedOption == (uint)Key::__COUNT - 1
                                         ? 0
                                         : selectedOption + 1;
                }
            }
            break;
        case MenuState::SETTINGS:
            if (Input::pressed(Key::MENU_LEFT, event)) {
                float volumeMusicPct = Audio::getMusicVolume();
                float volumeSfxPct = Audio::getSfxVolume();
                switch (SettingsOption(selectedOption)) {
                    case SettingsOption::VOLUME_MUSIC:
                        volumeMusicPct =
                            std::fmaxf(volumeMusicPct - 0.1f, 0.0f);
                        break;
                    case SettingsOption::VOLUME_SFX:
                        volumeSfxPct = std::fmaxf(volumeSfxPct - 0.1f, 0.0f);
                        break;
                    case SettingsOption::RESOLUTION: {
                        resolutionMultiplier = resolutionMultiplier > 1
                                                   ? resolutionMultiplier / 2
                                                   : 1;
                        uint width = BASIC_WIDTH * resolutionMultiplier;
                        uint height = BASIC_HEIGHT * resolutionMultiplier;
                        game.setResolution(width, height);
                    } break;
                    default:
                        break;
                }
                Audio::setVolume(volumeMusicPct, volumeSfxPct);
            } else if (Input::pressed(Key::MENU_RIGHT, event)) {
                float volumeMusicPct = Audio::getMusicVolume();
                float volumeSfxPct = Audio::getSfxVolume();
                switch (SettingsOption(selectedOption)) {
                    case SettingsOption::VOLUME_MUSIC:
                        volumeMusicPct =
                            std::fminf(volumeMusicPct + 0.1f, 1.0f);
                        break;
                    case SettingsOption::VOLUME_SFX:
                        volumeSfxPct = std::fminf(volumeSfxPct + 0.1f, 1.0f);
                        break;
                    case SettingsOption::RESOLUTION: {
                        resolutionMultiplier = resolutionMultiplier < 4
                                                   ? resolutionMultiplier * 2
                                                   : 4;
                        uint width = BASIC_WIDTH * resolutionMultiplier;
                        uint height = BASIC_HEIGHT * resolutionMultiplier;
                        game.setResolution(width, height);
                    } break;
                    default:
                        break;
                }
                Audio::setVolume(volumeMusicPct, volumeSfxPct);
            } else if (Input::pressed(Key::CANCEL, event)) {
                currentState = MenuState::SETTINGS_FADE_OUT;
                timeSinceStateChange = sf::Time::Zero;
            } else if (Input::pressed(Key::MENU_UP, event)) {
                selectedOption = selectedOption == 0
                                     ? (uint)SettingsOption::__COUNT - 1
                                     : selectedOption - 1;
            } else if (Input::pressed(Key::MENU_DOWN, event)) {
                selectedOption =
                    selectedOption == (uint)SettingsOption::__COUNT - 1
                        ? 0
                        : selectedOption + 1;
            }
            break;
        default:
            break;
    }
}

void StateStart::update(const sf::Time& deltaTime) {
    timeSinceStateChange += deltaTime;
    if (keyChangeRequested) {
        keyChangeRequested = false;
        waitingForKeyPress = true;
    }

    if (currentState == MenuState::NO_MENUS) {
        backgroundPosition -= BACKGROUND_PPS * deltaTime.asSeconds();
    } else if (currentState == MenuState::MENU_FADE_IN &&
               timeSinceStateChange > TIME_MENU_TWEEN) {
        currentState = MenuState::MENU;
        timeSinceStateChange = sf::Time::Zero;
        selectedOption = 0;
    } else if (currentState == MenuState::MENU_FADE_OUT &&
               timeSinceStateChange > TIME_MENU_TWEEN) {
        currentState = MenuState::NO_MENUS;
        timeSinceStateChange = sf::Time::Zero;
        selectedOption = 0;
    } else if (currentState == MenuState::CONTROLS_FADE_IN &&
               timeSinceStateChange > TIME_MENU_TWEEN + TIME_CONTROLS_TWEEN) {
        currentState = MenuState::CONTROLS;
        timeSinceStateChange = sf::Time::Zero;
        selectedOption = 0;
        keyChangeRequested = false;
        waitingForKeyPress = false;
    } else if (currentState == MenuState::SETTINGS_FADE_IN &&
               timeSinceStateChange > TIME_MENU_TWEEN + TIME_SETTINGS_TWEEN) {
        currentState = MenuState::SETTINGS;
        timeSinceStateChange = sf::Time::Zero;
        selectedOption = 0;
    } else if (currentState == MenuState::CONTROLS_FADE_OUT &&
               timeSinceStateChange > TIME_MENU_TWEEN + TIME_CONTROLS_TWEEN) {
        currentState = MenuState::MENU;
        timeSinceStateChange = sf::Time::Zero;
        selectedOption = 0;
    } else if (currentState == MenuState::SETTINGS_FADE_OUT &&
               timeSinceStateChange > TIME_MENU_TWEEN + TIME_SETTINGS_TWEEN) {
        currentState = MenuState::MENU;
        timeSinceStateChange = sf::Time::Zero;
        selectedOption = 0;
    } else if (currentState == MenuState::GRAND_PRIX_FADE &&
               timeSinceStateChange > TIME_FADE_TOTAL) {
        game.pushState(
            StatePtr(new StateRaceManager(game, RaceMode::GRAND_PRIX_1)));
    }
}

void StateStart::draw(sf::RenderTarget& window) {
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u backgroundSize = assetBackground.getSize();
    float scale = windowSize.y / (float)backgroundSize.y;

    sf::Sprite background(assetBackground);
    int pixelsLeft = backgroundPosition * scale;
    background.setTextureRect(
        {pixelsLeft, 0, (int)backgroundSize.x, (int)backgroundSize.y});
    background.setScale(scale, scale);
    background.setPosition(0.0f, 0.0f);
    window.draw(background);

    sf::Sprite logo(assetLogo);
    logo.setOrigin(assetLogo.getSize().x / 2.0f, 0.0f);
    logo.setScale(scale, scale);
    logo.setPosition(
        sf::Vector2f(ABS_LOGO.x * windowSize.x, ABS_LOGO.y * windowSize.y));
    window.draw(logo);

    // menu black box
    float pctMenu = 0.0f;  // animation pct for black box opening and closing
    if (currentState == MenuState::MENU) {
        pctMenu = 1.0f;
    } else if (currentState == MenuState::MENU_FADE_IN) {
        pctMenu = std::fminf(timeSinceStateChange / TIME_MENU_TWEEN, 1.0f);
    } else if (currentState == MenuState::MENU_FADE_OUT ||
               (currentState == MenuState::CONTROLS_FADE_IN &&
                timeSinceStateChange < TIME_MENU_TWEEN) ||
               (currentState == MenuState::SETTINGS_FADE_IN &&
                timeSinceStateChange < TIME_MENU_TWEEN)) {
        pctMenu =
            std::fmaxf(1.0f - timeSinceStateChange / TIME_MENU_TWEEN, 0.0f);
    } else if (currentState == MenuState::CONTROLS_FADE_OUT &&
               timeSinceStateChange > TIME_CONTROLS_TWEEN) {
        pctMenu = std::fminf(
            (timeSinceStateChange - TIME_CONTROLS_TWEEN) / TIME_MENU_TWEEN,
            1.0f);
    } else if (currentState == MenuState::SETTINGS_FADE_OUT &&
               timeSinceStateChange > TIME_SETTINGS_TWEEN) {
        pctMenu = std::fminf(
            (timeSinceStateChange - TIME_SETTINGS_TWEEN) / TIME_MENU_TWEEN,
            1.0f);
    }
    if (pctMenu > 0.0f) {
        sf::Image black;
        black.create(1, 1, sf::Color::Black);
        sf::Texture blackTex;
        blackTex.loadFromImage(black);
        sf::Sprite menuBackground(blackTex);
        sf::Vector2f menuSize = MENU_SIZE * pctMenu;
        sf::Vector2f menuPos = ABS_MENU_CENTER - menuSize / 2.0f;
        menuBackground.setPosition(menuPos.x * windowSize.x,
                                   menuPos.y * windowSize.y);
        menuBackground.setScale(menuSize.x * windowSize.x,
                                menuSize.y * windowSize.y);
        window.draw(menuBackground);
    }

    // menu text
    if (currentState == MenuState::MENU) {
        sf::Vector2f text1Pos = ABS_MENU + REL_TEXT1;
        sf::Vector2f text2Pos = ABS_MENU + REL_TEXT2;
        sf::Vector2f text3Pos = ABS_MENU + REL_TEXT3;
        sf::Color color1 = sf::Color::White,
                  color2 = sf::Color(150, 150, 170, 255),
                  color3 = sf::Color(150, 150, 170, 255);
        if (selectedOption == 1)
            std::swap(color1, color2);
        else if (selectedOption == 2)
            std::swap(color1, color3);
        TextUtils::write(
            window, "grand prix",
            sf::Vector2f(text1Pos.x * windowSize.x, text1Pos.y * windowSize.y),
            scale, color1);
        TextUtils::write(
            window, "controls",
            sf::Vector2f(text2Pos.x * windowSize.x, text2Pos.y * windowSize.y),
            scale, color2);
        TextUtils::write(
            window, "settings",
            sf::Vector2f(text3Pos.x * windowSize.x, text3Pos.y * windowSize.y),
            scale, color3);
    }

    // controls black box
    float pctControls =
        0.0f;  // animation pct for black box opening and closing
    if (currentState == MenuState::CONTROLS) {
        pctControls = 1.0f;
    } else if (currentState == MenuState::CONTROLS_FADE_IN &&
               timeSinceStateChange > TIME_MENU_TWEEN) {
        pctControls = std::fminf(
            (timeSinceStateChange - TIME_MENU_TWEEN) / TIME_CONTROLS_TWEEN,
            1.0f);
    } else if (currentState == MenuState::CONTROLS_FADE_OUT &&
               timeSinceStateChange < TIME_CONTROLS_TWEEN) {
        pctControls =
            std::fmaxf(1.0f - timeSinceStateChange / TIME_CONTROLS_TWEEN, 0.0f);
    }
    if (pctControls > 0.0f) {
        sf::Image black;
        black.create(1, 1, sf::Color::Black);
        sf::Texture blackTex;
        blackTex.loadFromImage(black);
        sf::Sprite controlsBackground(blackTex);
        sf::Vector2f menuSize = CONTROLS_SIZE * pctControls;
        sf::Vector2f menuPos = ABS_CONTROLS_CENTER - menuSize / 2.0f;
        controlsBackground.setPosition(menuPos.x * windowSize.x,
                                       menuPos.y * windowSize.y);
        controlsBackground.setScale(menuSize.x * windowSize.x,
                                    menuSize.y * windowSize.y);
        window.draw(controlsBackground);
    }

    // controls text
    if (currentState == MenuState::CONTROLS) {
        sf::Vector2f leftPos = ABS_CONTROLS + REL_CONTROL0;
        sf::Vector2f rightPos = leftPos + REL_CONTROLDX;

        TextUtils::write(
            window, "action",
            sf::Vector2f(leftPos.x * windowSize.x, leftPos.y * windowSize.y),
            scale, sf::Color::Green);
        TextUtils::write(
            window, "key",
            sf::Vector2f(rightPos.x * windowSize.x, rightPos.y * windowSize.y),
            scale, sf::Color::Green);
        leftPos += REL_CONTROLDY * 2.0f;
        rightPos += REL_CONTROLDY * 2.0f;

        sf::Color selectedColor =
            waitingForKeyPress ? sf::Color::Yellow : sf::Color::White;
        sf::Color normalColor = sf::Color(150, 150, 170, 255);
        for (uint i = 0; i < (int)Key::__COUNT; i++) {
            TextUtils::write(window, Input::getActionName(Key(i)),
                             sf::Vector2f(leftPos.x * windowSize.x,
                                          leftPos.y * windowSize.y),
                             scale,
                             i == selectedOption ? selectedColor : normalColor);
            TextUtils::write(window, Input::getKeyCodeName(Input::get(Key(i))),
                             sf::Vector2f(rightPos.x * windowSize.x,
                                          rightPos.y * windowSize.y),
                             scale,
                             i == selectedOption ? selectedColor : normalColor);
            leftPos += REL_CONTROLDY;
            rightPos += REL_CONTROLDY;
        }
    }

    // settings black box
    float pctSettings =
        0.0f;  // animation pct for black box opening and closing
    if (currentState == MenuState::SETTINGS) {
        pctSettings = 1.0f;
    } else if (currentState == MenuState::SETTINGS_FADE_IN &&
               timeSinceStateChange > TIME_MENU_TWEEN) {
        pctSettings = std::fminf(
            (timeSinceStateChange - TIME_MENU_TWEEN) / TIME_SETTINGS_TWEEN,
            1.0f);
    } else if (currentState == MenuState::SETTINGS_FADE_OUT &&
               timeSinceStateChange < TIME_SETTINGS_TWEEN) {
        pctSettings =
            std::fmaxf(1.0f - timeSinceStateChange / TIME_SETTINGS_TWEEN, 0.0f);
    }
    if (pctSettings > 0.0f) {
        sf::Image black;
        black.create(1, 1, sf::Color::Black);
        sf::Texture blackTex;
        blackTex.loadFromImage(black);
        sf::Sprite controlsBackground(blackTex);
        sf::Vector2f menuSize = SETTINGS_SIZE * pctSettings;
        sf::Vector2f menuPos = ABS_SETTINGS_CENTER - menuSize / 2.0f;
        controlsBackground.setPosition(menuPos.x * windowSize.x,
                                       menuPos.y * windowSize.y);
        controlsBackground.setScale(menuSize.x * windowSize.x,
                                    menuSize.y * windowSize.y);
        window.draw(controlsBackground);
    }

    // settings text
    if (currentState == MenuState::SETTINGS) {
        sf::Vector2f leftPos = ABS_SETTINGS + REL_SETTING0;
        sf::Vector2f rightPos = leftPos + REL_SETTINGDX;

        TextUtils::write(
            window, "setting",
            sf::Vector2f(leftPos.x * windowSize.x, leftPos.y * windowSize.y),
            scale, sf::Color::Green);
        TextUtils::write(
            window, "value",
            sf::Vector2f(rightPos.x * windowSize.x, rightPos.y * windowSize.y),
            scale, sf::Color::Green);
        leftPos += REL_CONTROLDY * 2.0f;
        rightPos += REL_CONTROLDY * 2.0f;

        sf::Color selectedColor = sf::Color::Yellow;
        sf::Color normalColor = sf::Color(150, 150, 170, 255);

        // it doesn't get much more hard-coded than this
        TextUtils::write(
            window, "music volume",
            sf::Vector2f(leftPos.x * windowSize.x, leftPos.y * windowSize.y),
            scale,
            selectedOption == (uint)SettingsOption::VOLUME_MUSIC ? selectedColor
                                                                 : normalColor);
        TextUtils::write(
            window, std::to_string((int)(Audio::getMusicVolume() * 100)),
            sf::Vector2f(rightPos.x * windowSize.x, rightPos.y * windowSize.y),
            scale,
            selectedOption == (uint)SettingsOption::VOLUME_MUSIC ? selectedColor
                                                                 : normalColor);
        leftPos += REL_CONTROLDY;
        rightPos += REL_CONTROLDY;
        TextUtils::write(
            window, "sfx volume",
            sf::Vector2f(leftPos.x * windowSize.x, leftPos.y * windowSize.y),
            scale,
            selectedOption == (uint)SettingsOption::VOLUME_SFX ? selectedColor
                                                               : normalColor);
        TextUtils::write(
            window, std::to_string((int)(Audio::getSfxVolume() * 100)),
            sf::Vector2f(rightPos.x * windowSize.x, rightPos.y * windowSize.y),
            scale,
            selectedOption == (uint)SettingsOption::VOLUME_SFX ? selectedColor
                                                               : normalColor);
        leftPos += REL_CONTROLDY;
        rightPos += REL_CONTROLDY;
        // resolution
        uint width = BASIC_WIDTH * resolutionMultiplier;
        uint height = BASIC_HEIGHT * resolutionMultiplier;
        TextUtils::write(
            window, "resolution",
            sf::Vector2f(leftPos.x * windowSize.x, leftPos.y * windowSize.y),
            scale,
            selectedOption == (uint)SettingsOption::RESOLUTION ? selectedColor
                                                               : normalColor);
        TextUtils::write(
            window, std::to_string(width) + "x" + std::to_string(height),
            sf::Vector2f(rightPos.x * windowSize.x, rightPos.y * windowSize.y),
            scale,
            selectedOption == (uint)SettingsOption::RESOLUTION ? selectedColor
                                                               : normalColor);
    }

    // fade to black if necessary
    if (currentState == MenuState::GRAND_PRIX_FADE) {
        float pct = timeSinceStateChange / TIME_FADE_TOTAL;
        int alpha = std::min(pct * 255.0f, 255.0f);
        sf::Image black;
        black.create(windowSize.x, windowSize.y, sf::Color::Black);
        sf::Texture blackTex;
        blackTex.loadFromImage(black);
        sf::Sprite blackSprite(blackTex);
        blackSprite.setPosition(0.0f, 0.0f);
        blackSprite.setColor(sf::Color(255, 255, 255, alpha));
        window.draw(blackSprite);
    }
}