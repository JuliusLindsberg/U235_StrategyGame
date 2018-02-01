#ifndef _GUI_HPP
#define _GUI_HPP

#include <iostream>
#include <SFML/Graphics.hpp>
#include <list>
#include <cmath>
#include <memory>

#include "world.hpp"

namespace purriGUI {

    //function for handling sf::String formed text input into all kinds of textfields and consoles and stuff like that
    sf::String handleTextInput(sf::String input, sf::String oldText);

    class GUI;

    float getDistance( float x1, float x2, float y1, float y2);
    //ButtonShape is the base class for all the differently shaped buttons. HUD stores a container containing all the different kind of interactable buttons in a base-class pointer.
    class ButtonShape {
        public:
        virtual void setPosition(float x, float y) {
            std::cerr << "in purriGUI::ButtonShape.setPosition(): program tried to use the native virtual function of class ButtonShape, which doesn't implement anything";
        }
        virtual bool isContainedWithin(sf::Vector2f point) {
            std::cerr << "in purriGUI::ButtonShape.isContainedWithin(): program tried to use the native virtual function of class ButtonShape, which doesn't implement anything";
            return false;
        }
        virtual void drawMe(sf::RenderWindow& window) {
            std::cerr << "in purriGUI::ButtonShape.drawMe(): program tried to use the native virtual function of class ButtonShape, which doesn't implement anything";
            return;
        }
        virtual sf::Vector2f getPosition() {
            std::cerr << "in purriGUI::ButtonShape.getPosition(): program tried to use the native virtual function of class ButtonShape, which doesn't implement anything\n";
            return sf::Vector2f(0.0f, 0.0f);
        }
        virtual ~ButtonShape() {}
    };

    class CircleButtonShape: public ButtonShape {
        public:
        sf::CircleShape body;
        CircleButtonShape(sf::CircleShape referenceShape) {
            body = referenceShape;
        }
        virtual void setPosition(float x, float y) {
            body.setPosition(x, y);
        }
        virtual bool isContainedWithin(sf::Vector2f point) {

            if( getDistance(point.x, body.getPosition().x, point.y, body.getPosition().y) < body.getRadius() ) {
                return true;
            }
            return false;
        }
        virtual void drawMe(sf::RenderWindow& window) {
            window.draw(body);
        }
        virtual ~CircleButtonShape() {}
        virtual sf::Vector2f getPosition() {
            return body.getPosition();
        }
    };

    class RectangleButtonShape: public ButtonShape {
        sf::RectangleShape body;
    public:
        RectangleButtonShape(sf::RectangleShape referenceShape) {
            body = referenceShape;
        }
        virtual void setPosition(float x, float y) {
            body.setPosition(x, y);
        }
        virtual bool isContainedWithin(sf::Vector2f point) {
            float width = body.getSize().x;
            float height = body.getSize().y;
            if( point.x > body.getPosition().x && point.x < body.getPosition().x+width && point.y > body.getPosition().y && point.y < body.getPosition().y+height ) {
                return true;
            }
            return false;
        }
        virtual void drawMe(sf::RenderWindow& window) {
            window.draw(body);
        }
        virtual ~RectangleButtonShape() {}
        virtual sf::Vector2f getPosition() {
            return body.getPosition();
        }
    };

    class DrawableData {
        bool freed;
    public:
        DrawableData() {
            freed = false;
        }
        void free() {
            freed = true;
        }
        bool isFreed() {
            return freed;
        }
    };

    class SignalListener {
        bool triggered;
        bool dead;
        bool freed;
        public:
        SignalListener() {
            triggered = false;
            dead = false;
            freed = false;
        }
        virtual void onTrigger() {

        }
        void trigger() {
            triggered = true;
        }
        void kill() {
            dead = true;
        }
        void free() {
            freed = true;
        }
        bool isFreed() {
            return freed;
        }
        void handleMe() {
            if(triggered) {
                onTrigger();
                triggered = false;
            }
        }
        virtual ~SignalListener() {}
    };

    class Interactable {
        bool freed;
        std::unique_ptr<purriGUI::ButtonShape> reactionShape;
        bool previousMousePressedState;
        bool selected;
        bool selectingTriggers;
        bool deselectingTriggers;
        bool xorSelectMode;
    protected:
        SignalListener* signalSlot;

        virtual void onSelection() {}
        virtual void onDeselection() {}

        bool active;
        bool dead;
        bool clickingExclusiveSelects;
        GUI* hud;
    public:
        bool getSelectingTriggers() {
            return selectingTriggers;
        }
        void free() {
            freed = true;
        }
        bool isFreed() {
            return freed;
        }
        bool getXorSelectMode() {
            return xorSelectMode;
        }
        sf::Vector2f getPosition() {
            return reactionShape->getPosition();
        }
        bool getDeselectingTriggers() {
            return deselectingTriggers;
        }
        void setSelectingTriggers(bool value) {
            selectingTriggers = value;
        }
        void setDeselectingTriggers(bool value) {
            deselectingTriggers = value;
        }
        bool isSelected() {
            return selected;
        }
        bool isActive() {
            return active;
        }
        void activate() {
            active = true;
        }
        void kill() {
            dead = true;
        }
        void deactivate() {
            active = false;
        }
        void setActiveState(bool state) {
            active = state;
        }
        bool getClickingExclusiveSelectsState() {
            return clickingExclusiveSelects;
        }
        void setClickingExclusiveSelectsState(bool state) {
            clickingExclusiveSelects = state;
        }
        void setXorSelectMode(bool state) {
            xorSelectMode = state;
        }
        GUI* getHud() {
            return hud;
        }
        void loopMe(sf::RenderWindow& window);
        void selectExclusively();
        void select();
        void deselect();
        virtual void loopChildFunctionality(sf::RenderWindow& window) {}
        virtual void onForwardedText(sf::String textForwarded) {}
        virtual void onForwardedKeyEvents(sf::Event::KeyEvent keyEvent);

        Interactable(sf::CircleShape buttonMesh, GUI* _hud, SignalListener* _signalSlot);
        Interactable(sf::RectangleShape buttonMesh, GUI* _hud, SignalListener* _signalSlot);
        virtual ~Interactable() {}
    };
    //console is always a rectangle shape. It has input field either at the bottom or top
    //and it shows all the most recent messages in the stream it points to
    class GuiConsole: public Interactable {
        int textRows;
        int charactersPerTextRow;
    public:
        GuiConsole(float textSize, int amountOfTextRows, int numberOfCharactersDisplayedPerRow, sf::RectangleShape _mesh, purriGUI::GUI* _hud): Interactable(_mesh, _hud, nullptr) {
            textRows = amountOfTextRows;
            charactersPerTextRow = numberOfCharactersDisplayedPerRow;
        }
        virtual ~GuiConsole() {}
    };

    //gui controls a single logical whole of a user interface. There are supposed to be able to be multiple guis in a single program at the same time.
    class GUI {
    private:
        sf::Font* font;
        std::list<SignalListener*> listeners;
        std::list<Interactable*> interactables;
        std::list<std::pair<DrawableData, sf::Drawable*>> drawables;
        void manageListeners();
        void manageInteractables(sf::RenderWindow& window);
        void manageDrawables(sf::RenderWindow& window);
        void forwardTextInput(sf::String forwardedText);
        void destroyListeners();
        void destroyInteractables();
        void destroyDrawables();
        void destroyFreedObjects();
    public:
        Interactable* addInteractableToList(purriGUI::Interactable* interactable);
        SignalListener* addListenerToList(purriGUI::SignalListener* listener);
        sf::Font* getFont() {
            return font;
        }
        std::pair<DrawableData, sf::Drawable*>* addDrawableToList(std::pair<DrawableData, sf::Drawable*> drawable);
        std::pair<DrawableData, sf::Drawable*>* addDrawableToList(sf::Drawable* drawable);
        void selectInteractableExclusively(Interactable* interactable);
        GuiConsole* console;
        void freeInteractables();
        void freeListeners();
        void freeDrawables();
        bool freeInteractable(Interactable* interactable);
        bool freeListener(void* listener);
        bool freeDrawable(void* drawable);
        void forwardKeyPresses(sf::Event::KeyEvent keyEvent);
        void handleGUIEventActions(sf::Event event);
        void addConsole(sf::RectangleShape mesh, float textSize, int amountofTextRows, int numberOfCharactersDisplayedPerRow);
        GUI(sf::Font* _font, float bordersSize) {
            console = nullptr;
            font = _font;
        }
        virtual ~GUI() {
            //Free all memory of triggers and interactables
            freeListeners();
            freeInteractables();
            if(console != nullptr) {
                delete console;
            }
            destroyListeners();
            destroyInteractables();
        };
        void manageHUD(sf::RenderWindow& window);
    };

        //a Button, basically an interactable with the modification of having a text label in it
    class Button: public Interactable {
    public:
        sf::Text buttonText;
        Button(sf::CircleShape shape, GUI* _hud, sf::Text _buttonText, SignalListener* slot = nullptr): Interactable(shape, _hud, slot) {
            buttonText = _buttonText;
        }
        Button(sf::RectangleShape shape, GUI* _hud, sf::Text _buttonText,  SignalListener* slot = nullptr): Interactable(shape, _hud, slot) {
            buttonText = _buttonText;
        }
        virtual ~Button() {
        }
        static Button* createButton(purriGUI::GUI& hud, sf::CircleShape shape, sf::Text text, SignalListener* slot = nullptr);
        static Button* createButton(purriGUI::GUI& hud, sf::RectangleShape shape, sf::Text text, SignalListener* slot = nullptr);
        virtual void loopChildFunctionality(sf::RenderWindow& window) {
            window.draw(buttonText);
        }
    };
    //Textfield, when selected the user can type text into it
    class TextField: public Button {
        bool selectingEmpties;
        bool firstSelectingEmpties;
        bool firstSelect;
        //declaring Button's creation functions private, so they are unusable from TextField, instead, the createTextField is supposed to be used as a factory function
        //I'm not completely sure yet whether this is a legit strategy to implement encapsulation though...
        static Button* createButton(purriGUI::GUI& hud, sf::CircleShape shape, sf::Text text, SignalListener* slot = nullptr);
        static Button* createButton(purriGUI::GUI& hud, sf::RectangleShape shape, sf::Text text, SignalListener* slot = nullptr);
    public:
        static TextField* createTextField(purriGUI::GUI& hud, sf::RectangleShape shape, sf::Text defaultText, SignalListener* slot = nullptr);
        TextField(sf::RectangleShape shape, GUI* _hud, sf::Text defaultText, SignalListener* _listener);
        void setSelectingEmpties(bool value) {
            selectingEmpties = value;
        }
        bool getSelectingEmpties() {
            return selectingEmpties;
        }
        void setFirstSelectingEmpties(bool value) {
            firstSelectingEmpties = value;
        }
        bool getFirstSelectingEmpties() {
            return firstSelectingEmpties;
        }
        virtual void onForwardedText(sf::String textForwarded) {
            buttonText.setString(purriGUI::handleTextInput(textForwarded, buttonText.getString()));
        }
        virtual void onSelection() {
            if(selectingEmpties || (firstSelect && firstSelectingEmpties) ) {
                buttonText.setString("");
                firstSelect = false;
            }
        }
        virtual ~TextField() {}
    };
    //a simple instance of a switch, it will change 1 to 0 and 0 back to 1 when it recieves signal
    //NOTE: THE CLASS IS NOT IMPLEMENTED PROPERLY AND IS OBOSOLETE
    class MenuSwitchListener: public SignalListener {
        int menuSwitch;
        int positionSinceLastChangedCall;
    public:
        MenuSwitchListener() {
            menuSwitch = 0;
            positionSinceLastChangedCall = 0;
        }
        virtual ~MenuSwitchListener() {}
        static MenuSwitchListener* createMenuSwitchListener( GUI& hud);
        virtual void onTrigger() {
            menuSwitch = !menuSwitch;
        }
        bool menuSwitchChanged(sf::Time currentTime);
        int getMenuSwitch() {
            return menuSwitch;
        }
    };

    class InputBox: public SignalListener {
        bool ready;
    public:
        InputBox() {
            ready = false;
        }
        virtual ~InputBox() {}
        static InputBox* createInputBox( GUI& hud);
        bool isReady() {
            return ready;
        }
        void setReady(bool value) {
            ready = value;
        }
        virtual void onTrigger() {
            ready = true;
        }
    };
};

#endif
