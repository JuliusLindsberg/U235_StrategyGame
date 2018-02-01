
#include "gui.hpp"

bool purriGUI::GUI::freeInteractable(purriGUI::Interactable* interactable) {
    if(interactable == nullptr) {
        std::cerr << "Error in purriGUI::GUI::freeInteractable(): null pointer inserted into function!\n";
        return false;
    }
    purriGUI::Interactable* interactablePointer = /*(purriGUI::Interactable*)*/interactable;
    //check if the said pointer even exists in the gui, if it doesn't, the gui should be able to communicate an exception with the host program
    for(auto it = interactables.begin(); it != interactables.end(); it ++) {
        if((*it) == interactablePointer) {
            (*it)->free();
            return true;
        }
    }
    //no purriGUI::Interactable of such adress was found! returns false!
    return false;
}

bool purriGUI::GUI::freeListener(void* listener) {
    if(listener == nullptr) {
        std::cerr << "Error in purriGUI::GUI::freelistener(): null pointer inserted into function!\n";
        return false;
    }
    purriGUI::SignalListener* listenerPointer = (purriGUI::SignalListener*)listener;
    //check if the said pointer even exists in the gui, if it doesn't, the gui should be able to communicate an exception with the host program
    for(auto it = listeners.begin(); it != listeners.end(); it ++) {
        if(*it == listenerPointer) {
            (*it)->free();
            return true;
        }
    }
    //no purriGUI::Interactable of such adress was found! returns false!
    return false;
}

bool purriGUI::GUI::freeDrawable(void* drawable) {
    if(drawable == nullptr) {
        std::cerr << "Error in purriGUI::GUI::freelistener(): null pointer inserted into function!\n";
        return false;
    }
    sf::Drawable* drawablePointer = (sf::Drawable*)drawable;
    //check if the said pointer even exists in the gui, if it doesn't, the gui should be able to communicate an exception with the host program
    for(auto it = drawables.begin(); it != drawables.end(); it ++) {
        if((*it).second == drawablePointer) {
            (*it).first.free();
            return true;
        }
    }
    //no purriGUI::Interactable of such adress was found! returns false!
    return false;
}

sf::String purriGUI::handleTextInput(sf::String input, sf::String oldText) {
    if(input.getSize() == 1) {
        //if the text being forwarded to the textfield is a backspace character, then erase the last character in the string
        switch ( *(input.begin()) ) {

        case 0x08:
            {
                if( oldText.isEmpty() ) {
                    return oldText;
                }
                else {
                    oldText.erase(oldText.getSize()-1);
                    return oldText;
                }
            }
        //escape
        case 27:
            {
                return oldText;
            }
        //enter
        case 0x000D:
            {
                return oldText;
            }
        }
    }
    return oldText + input;
}

void purriGUI::GUI::manageListeners() {
    for(auto it = listeners.begin(); it != listeners.end(); it++) {
        (*it)->handleMe();
    }
}

void purriGUI::GUI::manageInteractables(sf::RenderWindow& window) {
    for(auto it = interactables.begin(); it != interactables.end(); it++) {
        (*it)->loopMe(window);
    }
}

void purriGUI::GUI::manageDrawables(sf::RenderWindow& window) {
    for(auto it = drawables.begin(); it != drawables.end(); it++) {
        window.draw(*(*it).second);
    }
}

void purriGUI::GUI::destroyListeners() {
    for(auto it = listeners.begin(); it != listeners.end(); it++) {
        if((*it) != NULL) {
            delete (*it);
            listeners.erase(it);
        }
    }
    listeners.clear();
}

void purriGUI::GUI::freeListeners() {
    for(auto it = listeners.begin(); it != listeners.end(); it++) {
        if((*it) != NULL) {
            (*it)->free();
        }
    }
}

void purriGUI::GUI::forwardKeyPresses(sf::Event::KeyEvent keyEvent) {
    for(auto it = interactables.begin(); it != interactables.end(); it++) {
        if( (*it)->isSelected() ) {
            (*it)->onForwardedKeyEvents(keyEvent);
        }
    }
}

void purriGUI::GUI::forwardTextInput(sf::String textInput) {
    for(auto it = interactables.begin(); it != interactables.end(); it++) {
        if( (*it)->isSelected() ) {
            (*it)->onForwardedText(textInput);
        }
    }
}
void purriGUI::GUI::handleGUIEventActions(sf::Event event) {
    switch(event.type) {
        case sf::Event::TextEntered:
        {
            forwardTextInput(event.text.unicode);
        }
        case sf::Event::KeyPressed:
        {
            forwardKeyPresses(event.key);
        }
        default:
        {
            break;
        }
    }
}

void purriGUI::GUI::addConsole(sf::RectangleShape mesh, float textSize, int amountofTextRows, int numberOfCharactersDisplayedPerRow) {
    if(console == nullptr) {
        console = new GuiConsole(textSize, amountofTextRows, numberOfCharactersDisplayedPerRow, mesh, this);
    }
    else {
        std::cerr << "Error in purriGUI::GUI::addConsole(): a GuiConsole already existed in this hud when calling addConsole(console pointer was not null)";
    }
}

void purriGUI::GUI::destroyInteractables() {
    std::cout << "GUI::destroyInteractables()\n";
    for(auto it = interactables.begin(); it != interactables.end(); it++) {
        if((*it) != NULL) {
            delete (*it);
            interactables.erase(it);
        }
    }
    interactables.clear();
    std::cout << "GUI::destroyInteractables() end\n";
}
void purriGUI::GUI::freeInteractables() {
    for(auto it = interactables.begin(); it != interactables.end(); it++) {
        if((*it) != NULL) {
            (*it)->free();
        }
    }
}

void purriGUI::GUI::freeDrawables() {
    for(auto it = drawables.begin(); it != drawables.end(); it++) {
        if((*it).second != NULL) {
            (*it).first.free();
        }
    }
}

void purriGUI::GUI::destroyDrawables() {
    for(auto it = drawables.begin(); it != drawables.end(); it++) {
        if((*it).second != NULL) {
            delete (*it).second;
            drawables.erase(it);
        }
    }
    drawables.clear();
}

void purriGUI::GUI::selectInteractableExclusively(Interactable* interactable) {
    interactable->select();
    for(auto it = interactables.begin(); it != interactables.end(); it++) {
        if( (*it) != interactable && (*it)->isActive()) {
            (*it)->deselect();
        }
    }
}

void purriGUI::GUI::manageHUD(sf::RenderWindow& window) {
    manageInteractables(window);
    manageListeners();
    manageDrawables(window);
    destroyFreedObjects();
}

//should not actually be a part of gui library, rather math library, but this own function makes the gui library more math library independant while having the convention of using the function
//in it's internal calculations
float purriGUI::getDistance( float x1, float x2, float y1, float y2) {
    return sqrtf( (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) );
}

purriGUI::Interactable* purriGUI::GUI::addInteractableToList(purriGUI::Interactable* interactable) {
    interactables.push_back(interactable);
    return interactables.back();
}

void purriGUI::Interactable::onForwardedKeyEvents(sf::Event::KeyEvent keyEvent) {
    switch(keyEvent.code)
    {
    case sf::Keyboard::Escape:
        {
            deselect();
        }
    case sf::Keyboard::Return:
        {
            deselect();
        }
    default:
        {
            break;
        }
    }
}

void purriGUI::Interactable::loopMe(sf::RenderWindow& window) {
    if( !isActive() ) { return; }
    float tempX = (float)sf::Mouse::getPosition(window).x;
    float tempY = (float)sf::Mouse::getPosition(window).y;
    sf::Vector2f mouseCoord = sf::Vector2f(tempX, tempY );
    //the code below will send a signal, if the button is pressed in an intuitive way
    if( reactionShape->isContainedWithin(mouseCoord) ) {
        if( sf::Mouse::isButtonPressed(sf::Mouse::Left) ) {
            if( previousMousePressedState == false ) {
                if(clickingExclusiveSelects) {
                    selectExclusively();
                }
                else {
                    select();
                }
                previousMousePressedState = true;
            }
        }
        else {
            previousMousePressedState = false;
        }
    }
    else {
        //if the cursor is not contained inside the button, previousMouseButtonPresedState = true
        //this is so that someone dragging the cursor within the button, while holding the mouse key down will not trigger the button, which would be counter-intuitive
        previousMousePressedState = true;
    }

    (*reactionShape).drawMe(window);
    loopChildFunctionality(window);
}

void purriGUI::Interactable::selectExclusively() {
    hud->selectInteractableExclusively(this);
}

purriGUI::SignalListener* purriGUI::GUI::addListenerToList(purriGUI::SignalListener* listener) {
    listeners.push_back(listener);
    return listeners.back();
}

std::pair<purriGUI::DrawableData, sf::Drawable*>* purriGUI::GUI::addDrawableToList(std::pair<purriGUI::DrawableData, sf::Drawable*> drawable) {
    drawables.push_back(drawable);
    return &drawables.back();
}

std::pair<purriGUI::DrawableData, sf::Drawable*>* purriGUI::GUI::addDrawableToList(sf::Drawable* drawable) {
    std::pair<purriGUI::DrawableData, sf::Drawable*> couple;
    couple.second = drawable;
    couple.first = purriGUI::DrawableData();
    drawables.push_back(couple);
    return &drawables.back();
}

void purriGUI::GUI::destroyFreedObjects() {
    std::cout << "destroying listeners\n";
    for(auto it = listeners.begin(); it != listeners.end();) {
        if((*it)->isFreed()) {
            delete (*it);
            it = listeners.erase(it);
            continue;
        }
        it++;
    }
    std::cout << "destroying interactables\n";
    for(auto it = interactables.begin(); it != interactables.end();) {
        if((*it)->isFreed()) {
            delete (*it);
            it = interactables.erase(it);
            continue;
        }
        it++;
    }
    std::cout << "destroying drawables\n";
    for(auto it = drawables.begin(); it != drawables.end();) {
        if((*it).first.isFreed()) {
            delete (*it).second;
            it = drawables.erase(it);
            continue;
        }
        it++;
    }
    std::cout << "destroyingFreedObjects returning \n";
}

void purriGUI::Interactable::select() {
    if(xorSelectMode) {
        if(selected) {
            deselect();
            return;
        }
    }
    if( selectingTriggers ) {
        if( signalSlot != nullptr ) {
            signalSlot->trigger();
        }
    }
    selected = true;
    onSelection();
}

void purriGUI::Interactable::deselect() {
    selected = false;
    if( deselectingTriggers ) {
        if(signalSlot != nullptr) {
            signalSlot->trigger();
        }
    }
    onDeselection();
}

purriGUI::Interactable::Interactable(sf::CircleShape buttonMesh, GUI* _hud, SignalListener* _signalSlot) {
    buttonMesh.setOrigin(buttonMesh.getRadius(), buttonMesh.getRadius());
    reactionShape = std::unique_ptr<ButtonShape>((ButtonShape*)new CircleButtonShape(buttonMesh));
    activate();
    dead = false;
    clickingExclusiveSelects = false;
    hud = _hud;
    selected = false;
    signalSlot = _signalSlot;
    selectingTriggers = true;
    deselectingTriggers = false;
    xorSelectMode = false;
    freed = false;
}
purriGUI::Interactable::Interactable(sf::RectangleShape buttonMesh, GUI* _hud, SignalListener* _signalSlot) {
    reactionShape = std::unique_ptr<ButtonShape>((ButtonShape*)new RectangleButtonShape(buttonMesh));
    activate();
    hud = _hud;
    dead = false;
    clickingExclusiveSelects = false;
    selected = false;
    signalSlot = _signalSlot;
    selectingTriggers = true;
    deselectingTriggers = false;
    xorSelectMode = false;
    freed = false;
}

purriGUI::Button* purriGUI::TextField::createButton(purriGUI::GUI& hud, sf::CircleShape shape, sf::Text text, SignalListener* slot) {
    Button* button = new Button(shape, &hud, text, slot);
    hud.addInteractableToList((Interactable*)button);
    return button;
}

purriGUI::Button* purriGUI::TextField::createButton(purriGUI::GUI& hud, sf::RectangleShape shape, sf::Text text, SignalListener* slot) {
    Button* button = new Button(shape, &hud, text, slot);
    hud.addInteractableToList((Interactable*)button);
    return button;
}

purriGUI::TextField* purriGUI::TextField::createTextField(purriGUI::GUI& hud, sf::RectangleShape shape, sf::Text defaultText, SignalListener* slot) {
    TextField* textField = new TextField(shape, &hud, defaultText, slot);
    hud.addInteractableToList((Interactable*)textField);
    return textField;
}

purriGUI::TextField::TextField(sf::RectangleShape shape, GUI* _hud, sf::Text defaultText, SignalListener* _listener): Button(shape, _hud, defaultText, _listener) {
    setClickingExclusiveSelectsState(true);
    selectingEmpties = false;
    firstSelect = true;
    firstSelectingEmpties = true;
}

purriGUI::MenuSwitchListener* purriGUI::MenuSwitchListener::createMenuSwitchListener( GUI& hud) {
    MenuSwitchListener* listener = new MenuSwitchListener();
    hud.addListenerToList((SignalListener*)listener);
    return listener;
}

bool purriGUI::MenuSwitchListener::menuSwitchChanged(sf::Time currentTime) {
    if(menuSwitch != positionSinceLastChangedCall) {
        positionSinceLastChangedCall = menuSwitch;
        return true;
    }
    return false;
}

purriGUI::InputBox* purriGUI::InputBox::createInputBox( GUI& hud) {
    InputBox* listener = new InputBox();
    hud.addListenerToList((SignalListener*)listener);
    return listener;
}
