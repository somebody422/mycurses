#include "mycurses.hpp"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//SimpleDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

SimpleDialog::SimpleDialog()
  : Dialog(), layout(1)
{
  layout.setFocus(true);
}
  
SimpleDialog::SimpleDialog(std::string message_i)
  : Dialog(), component_y_start(1), message(message_i), layout(1)
{
  layout.setFocus(true);
}

//void SimpleDialog::respondToKeyPress(int ch){
//  layout.respondToKeyPress(ch);
//}

void SimpleDialog::moveAndResize(int x, int y, int new_width, int new_height){
  win.clear();
  win.moveAndResize(x, y, new_width, new_height);
  int width = new_width-2;
  int height = new_height-3;
  layout.moveAndResize(x+1, y+2, width, height);
}

void SimpleDialog::drawComponents(){
  win.drawBorder();
  win.colorOn(Terminal::GREEN, Terminal::BLACK);
  if(component_y_start){
    for(unsigned int x = 0; x < win.getWidth()-2; x++) win.print('#', x, 0);
    win.print(message, (win.getWidth() - 2 - message.length()) / 2, 0);
  }
  win.colorOff(Terminal::GREEN, Terminal::BLACK);
  win.refresh();


  layout.drawComponent();
}


int SimpleDialog::getChar(){
  return win.getChar();
}

/*
void SimpleDialog::addComponent(Component* c){
  layout.addComponent(c, 0, 0);
}
void SimpleDialog::addComponent(Component* c, unsigned char flags){
  layout.addComponent(c, 0, flags);
}
*/
