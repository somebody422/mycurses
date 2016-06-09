#include <mycurses.hpp>


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//SimpleDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

SimpleDialog::SimpleDialog()
	: Dialog() {}
	
SimpleDialog::SimpleDialog(std::string message_i)
	: Dialog(), component_y_start(1), message(message_i) {}
/*SimpleDialog::~SimpleDialog(){
	
}*/

bool SimpleDialog::respondToKeyPress(int ch){
	if(selected_component == nullptr) return false;
	
	if(selected_component->respondToKeyPress(ch)){
		if(continue_button->isFinished()) return true;
		return false;
	}
	
	switch(ch){
		case Terminal::UP_ARROW_KEY:
			if(selected_component->getPrev() != nullptr){
				selected_component->setFocus(false);
				selected_component = selected_component->getPrev();
				selected_component->setFocus(true);
			}
			else{
				selected_component->setFocus(false);
				selected_component = components.back();
				selected_component->setFocus(true);
			}
			break;
		case Terminal::DOWN_ARROW_KEY:
			if(selected_component->getNext() != nullptr){
				selected_component->setFocus(false);
				selected_component = selected_component->getNext();
				selected_component->setFocus(true);
			}
			else{
				selected_component->setFocus(false);
				selected_component = components.front();
				selected_component->setFocus(true);
			}
		}
		//if(continue_button->isFinished()) return true;
		return false;
}

void SimpleDialog::moveAndResize(int x, int y, int new_width, int new_height){
	win.clear();
	win.moveAndResize(x, y, new_width, new_height);
	int width = new_width-2;
	int height = new_height-3;
	int dynamic_component_height = 0;
	if(num_dynamic_components > 0)
		dynamic_component_height = (height - total_static_height) / num_dynamic_components;
	
	int component_y = y + component_y_start + 1;
	for(Component* c : components){
		if(component_is_dynamic[c]){ //component has dynamic height
			c->moveAndResize(x+1, component_y, width, dynamic_component_height);
			component_y += dynamic_component_height;
		}
		else{ //component has static height
			c->moveAndResize(x+1, component_y, width, c->getMinHeight());
			component_y += c->getMinHeight();
		}
	}
}

void SimpleDialog::drawComponents(){
	win.drawBorder();
	win.colorOn(Terminal::GREEN, Terminal::BLACK);
	if(component_y_start){
		for(int x = 0; x < win.getWidth()-2; x++) win.print('#', x, 0);
		win.print(message, (win.getWidth() - 2 - message.length()) / 2, 0);
	}
	win.colorOff(Terminal::GREEN, Terminal::BLACK);
	win.refresh();
	
	for(Component* c : components){
		c->drawComponent();
	}
}

int SimpleDialog::getChar(){
	if(selected_component == nullptr){
		return win.getChar();
	}
	return selected_component->getChar();
}

void SimpleDialog::addComponent(Component* c){
	addComponent(c, 0);
}
void SimpleDialog::addComponent(Component* c, unsigned char flags){
	bool unfocusable = BIT_FROM_UCHAR(flags, 0);
	bool dynamic_height = BIT_FROM_UCHAR(flags, 7);
	
	if(!unfocusable){
		if(first_focusable_component != nullptr){
			last_focusable_component->next = c;
			c->prev = last_focusable_component;
			last_focusable_component = c;
		} else{
			first_focusable_component = c;
			last_focusable_component = c;
			selected_component = c;
			selected_component->setFocus(true);
		}
	}
	components.push_back(c);
	if(dynamic_height) num_dynamic_components += 1;
	else total_static_height += c->getMinHeight();
	component_is_dynamic[c] = dynamic_height;
}
