#include <mycurses.hpp>



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//MessageComponent
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MessageComponent::MessageComponent(std::string message_i)
	: Component(), message(message_i)
{
	min_height = 3;
}

void MessageComponent::drawComponent(){
	win.clear();
	win.print(message, 0, 0);
	win.drawBorder();
	win.refresh();
}



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//DynamicMessageComponent
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DynamicMessageComponent::DynamicMessageComponent()
	: Component()
{
	min_height = 3;
	//win.b = Border::DEFAULT;
}

void DynamicMessageComponent::drawComponent(){
	win.clear();
	if(messages.empty()) return;
	
	unsigned int y = 0, max_y = win.getHeight()-2, max_x = getWidth()-2;
	for(std::string message : messages){
		if(y >= max_y) break;
		
		if(message.length() >= max_x){
			unsigned int next_sub_str_start = 0;
			while(next_sub_str_start < message.length() && y < max_y){
				win.print(message.substr(next_sub_str_start, max_x), 0, y);
				next_sub_str_start += max_x;
				y += 1;
			}
		}
		else win.print(message, 0, y);
		y++;
	}
	win.refresh();
}

void DynamicMessageComponent::addMessage(std::string message){
	messages.push_front(message);
	if(message.size() > MAX_MESSAGES) messages.pop_back();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//SimpleInputComponent
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
 * Handles a single line of input, with an optional label at the top
 */
SimpleInputComponent::SimpleInputComponent()
	: Component()
{
	min_height = 3;
}

SimpleInputComponent::SimpleInputComponent(std::string label_i)
	: Component(), label(label_i), has_label(true), y(1)
{
	min_height = 4;
}

void SimpleInputComponent::drawComponent(){
	win.clear();
	if(has_label){
		win.colorOn(Terminal::YELLOW, Terminal::BLACK);
		for(int x = 0; x < win.getWidth()-2; x++) win.print('=', x, 0);
		win.print(label, (win.getWidth() - 2 - label.length()) / 2, 0);
		win.colorOff(Terminal::YELLOW, Terminal::BLACK);
	}
	
	win.print('>', 0, y);
	for(int i = 0; i < input_length; i++)
		win.print(input[i], i + INPUT_OFFSET, y);
	win.print(" ", Terminal::WHITE, Terminal::WHITE, input_length + INPUT_OFFSET, y);
	win.refresh();
}

bool SimpleInputComponent::respondToKeyPress(int ch){
	//use if rather than switch because if can catch ranges of characters
	if(ch == Terminal::BACKSPACE_KEY){
		if(input_length <= 0) return true;
		win.print(' ', --input_length + INPUT_OFFSET, y);
		win.print(" ", Terminal::BLACK, Terminal::BLACK, input_length + INPUT_OFFSET + 1, y);
		win.print(" ", Terminal::WHITE, Terminal::WHITE, input_length + INPUT_OFFSET, y);
		return true;
	}
	else if(	(ch >= 'a' && ch <= 'z') ||
						(ch >= 'A' && ch <= 'Z') ||
						(ch >= '0' && ch <= '9') ||
						ch == ' ')
	{ //is a letter or number
		if(input_length + INPUT_OFFSET + 3 >= win.getWidth()){
			clearInput();
			return true;
		}
		win.print((int)ch, input_length + INPUT_OFFSET, y);
		input[input_length++] = ch;
		win.print(" ", Terminal::WHITE, Terminal::WHITE, input_length + INPUT_OFFSET, y);
		win.refresh();
		return true;
	}
	return false;
}

void SimpleInputComponent::clearInput(){
	input_length = 0;
	draw();
}

std::string SimpleInputComponent::getString(){
	input[input_length] = '\0';
	return std::string(input);
}



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//DialogContinueButton
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DialogContinueButton::DialogContinueButton()
	: Component()
{
	min_height = 5;
}
void DialogContinueButton::drawComponent(){
	win.clear();
	int x = (win.getWidth() - 8) / 2;
	win.print("Continue", x, 1);
	win.refresh();
}
bool DialogContinueButton::respondToKeyPress(int ch){
	if(ch == '\n'){
		finished = true;
		return true;
	}
	return false;
}




//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//FillerComponent
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FillerComponent::FillerComponent(){
	min_height = 1;
}
FillerComponent::FillerComponent(int height){
	min_height = height;
}





//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//ListSelectComponent
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ListSelectComponent::ListSelectComponent(std::vector<std::string> options_i)
	: Component(), options(options_i)
{
	min_height = 3;
}
ListSelectComponent::ListSelectComponent(std::vector<std::string> options_i, std::string label_i)
	: Component(), options(options_i), label(label_i), y(1)
{
	min_height = 4;
}
void ListSelectComponent::drawComponent(){
	win.clear();
	if(selected_option > 0) win.print("<", Terminal::GREEN, Terminal::BLACK, 0, y);
	else win.print('<', 0, y);
	if(selected_option < options.size()-1) win.print(">", Terminal::GREEN, Terminal::BLACK, win.getWidth()-3, y);
	else win.print('>', win.getWidth()-3, y);
	if(options.size() > 0){
		std::string& s = options[selected_option];
		if(s.length() > win.getWidth()-4){
			 win.print(s.substr(0, win.getWidth()-4), 1, y);
		} else{
			win.print(s, (win.getWidth() - 4 - s.length()) / 2, y);
		}
	}
	win.refresh();
}
bool ListSelectComponent::respondToKeyPress(int ch){
	switch(ch){
		case Terminal::RIGHT_ARROW_KEY:
			if(selected_option >= options.size()-1) return false;
			selected_option += 1;
			drawComponent();
			return true;
		case Terminal::LEFT_ARROW_KEY:	
			if(selected_option <= 0) return false;
			selected_option -= 1;
			drawComponent();
			return true;
	}
	return false;
}
std::string ListSelectComponent::getSelectedOption(){
	return options[selected_option];
}