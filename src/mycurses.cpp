#include "mycurses.hpp"
#include <iostream>

//todo: manage possibility of no color support by terminal

Terminal* Terminal::terminal = nullptr;
bool Terminal::interrupt_next_input = false;
Border Border::DEFAULT = {'|', '|', '-', '-', '+', '+', '+', '+', ColorPair(Terminal::WHITE, Terminal::BLACK)};
Border Border::DEFAULT_RED = {'|', '|', '-', '-', '+', '+', '+', '+', ColorPair(Terminal::RED, Terminal::BLACK)};
Border Border::DIALOG_DEFAULT = {'<', '>', '^', '^', 'O', 'O', 'O', 'O', ColorPair(Terminal::GREEN, Terminal::BLACK)};
//const Terminal::BOLD = 




Terminal::Terminal(){
  initscr();
  noecho(); //dont echo back input from getch()
  raw(); //disable line buffering
  curs_set(0); //hide cursor
  //nodelay(stdscr, true); // getch and wgetch do not block
  if(has_colors()){
    has_color = true;
    start_color();
  }else has_color = false;
  getmaxyx(stdscr, height, width);
}

Terminal::~Terminal(){
  endwin();
}

void Terminal::print(char c, int x, int y){
  mvaddch(y, x, c);
}
void Terminal::print(char c, int attributes, int x, int y){
  mvaddch(y, x, c | attributes);
}

void Terminal::print(std::string str, int x, int y){
  mvaddstr(y, x, str.c_str());
}
void Terminal::print(std::string str, int attributes, int x, int y){
  attron(attributes);
  mvaddstr(y, x, str.c_str());
  attroff(attributes);
}
void Terminal::print(std::string str, int fg_color, int bg_color, int x, int y){
  int c = getColorAttributeValue(fg_color, bg_color);
  attron(c);
  mvaddstr(y, x, str.c_str());
  attroff(c);
}

void Terminal::refresh(){
  refresh();
}
int Terminal::getChar(){
  int ch = getch();
  if(ch == Terminal::TERMINAL_RESIZED)
    resized();
  return ch;
  /*int ch;
  while(true){
    if(interrupt_next_input){
      interrupt_next_input = false;
      throw MycursesInterruptedException();
    }
    ch = getch();
    if(ch == ERR) std::this_thread::yield();
    else break;
  }
  if(ch == Terminal::TERMINAL_RESIZED)
    resized();
  return ch;*/
}
void Terminal::clear(){
  clear();
}
unsigned int Terminal::getWidth(){
  return width;
}
unsigned int Terminal::getHeight(){
  return height;
}
bool Terminal::hasColor(){
  return has_color;
}

//attributes
void Terminal::attributeOn(int a){
  attron(a);
}
void Terminal::attributeOff(int a){
  attroff(a);
}

int Terminal::getColorAttributeValue(int c1, int c2){
  ColorPair cp {c1, c2};
  if(color_map.count(cp) == 0){
    init_pair(next_color, c1, c2);
    color_map[cp] = next_color++;
  }
  return COLOR_PAIR(color_map[cp]);
}
void Terminal::resized(){
  getmaxyx(stdscr, height, width);
}

void Terminal::end(){
  delete terminal;
}

void Terminal::interrupt(){
  interrupt_next_input = true;
}

Terminal* Terminal::instance(){
  if(terminal == nullptr){
    terminal = new Terminal();
  }
  return terminal;
}




//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Window
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Window::Window()
  : x(0), y(0), width(0), height(0), has_border(false), o(0), b(Border::DEFAULT)
{
  t = Terminal::instance();
  win = newwin(0, 0, 0, 0);
  keypad(win, true);
  //nodelay(win, true);
}
Window::Window(Border b_i)
  : x(0), y(0), width(0), height(0), has_border(true), o(1), b(b_i)
{
  t = Terminal::instance();
  win = newwin(0, 0, 0, 0);
  keypad(win, true);
  //nodelay(win, true);
}
Window::Window(int x_i, int y_i, int width_i, int height_i)
  : x(x_i), y(y_i), width(width_i), height(height_i), has_border(false), o(0), b(Border::DEFAULT)
{
  t = Terminal::instance();
  win = newwin(height, width, y, x);
  keypad(win, true);
  //nodelay(win, true);
  //has_border = false; o = 0;
  //wrefresh(win);
}
Window::Window(int x_i, int y_i, int width_i, int height_i, Border b_i)
  : x(x_i), y(y_i), width(width_i), height(height_i), has_border(true), o(1), b(b_i)
{
  t = Terminal::instance();
  win = newwin(height, width, y, x);
  keypad(win, true);
  //nodelay(win, true);
  //has_border = true; o = 1;
  //drawBorder();
  //wrefresh(win);
}
Window::~Window(){
  delwin(win);
}

void Window::print(char c, int x, int y){
  mvwaddch(win, y+o, x+o, c);
}
void Window::print(char c, int attributes, int x, int y){
  mvwaddch(win, y+o, x+o, c | attributes);
}

void Window::print(std::string str, int x, int y){
  mvwaddstr(win, y+o, x+o, str.c_str());
}
void Window::print(std::string str, int attributes, int x, int y){
  wattron(win, attributes);
  mvwaddstr(win, y+o, x+o, str.c_str());
  wattroff(win, attributes);
}
void Window::print(std::string str, int fg_color, int bg_color, int x, int y){
  int c = t->getColorAttributeValue(fg_color, bg_color);
  wattron(win, c);
  mvwaddstr(win, y+o, x+o, str.c_str());
  wattroff(win, c);
}

int Window::getChar(){
  /*while(true){
    if(Terminal::interrupt_next_input){
      Terminal::interrupt_next_input = false;
      throw MycursesInterruptedException();
    }
    int ch = wgetch(win);
    if(ch != ERR) return ch;
    std::this_thread::yield();
  }*/
  
  return wgetch(win);
}
void Window::clear(){
  wclear(win);
  drawBorder();
  wrefresh(win);
}
void Window::setBorder(bool border){
  has_border = border;
  o = border ? 1 : 0;
}
void Window::move(int x, int y){
  wclear(win);
  wrefresh(win); //not sure why, but it won't clear without this
  mvwin(win, y, x);
  drawBorder();
}
void Window::resize(int width, int height){
  wclear(win);
  wrefresh(win); //not sure why, but it won't clear without this
  wresize(win, height, width);
  drawBorder();
}
void Window::moveAndResize(int x, int y, int new_width, int new_height){
  width = new_width; height = new_height;
  wclear(win);
  wrefresh(win); //not sure why, but it won't clear without this
  wresize(win, height, width);
  mvwin(win, y, x);
}
void Window::refresh(){
  wrefresh(win);
}
void Window::drawBorder(){
  if(!has_border) return;
  int c = t->getColorAttributeValue(b.color.color_one, b.color.color_two);
  wattron(win, c);
  
  //top and bottom
  for(unsigned int i = 0; i < width; i++){
    mvwaddch(win, 0, i, b.ts);
    mvwaddch(win, height-1, i, b.bs);
  }
  
  //left and right
  for(unsigned int i = 0; i < height; i++){
    mvwaddch(win, i, 0, b.ts);
    mvwaddch(win, i, width-1, b.bs);
  }
  
  //corners
  mvwaddch(win, 0, 0, b.tl);
  mvwaddch(win, height-1, 0, b.bl);
  mvwaddch(win, 0, width-1, b.tr);
  mvwaddch(win, height-1, width-1, b.br);
  wattroff(win, c);
} //drawBorder
unsigned int Window::getX(){
  return x;
}
unsigned int Window::getY(){
  return y;
}
unsigned int Window::getWidth(){
  return width;
}
unsigned int Window::getHeight(){
  return height;
}

//attributes
void Window::attributeOn(int a){
  wattron(win, a);
}
void Window::attributeOff(int a){
  wattroff(win, a);
}
void Window::colorOn(int fg, int bg){
  wattron(win, t->getColorAttributeValue(fg, bg));
}
void Window::colorOff(int fg, int bg){
  wattroff(win, t->getColorAttributeValue(fg, bg));
}



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Component
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Component::Component() : win(Border::DEFAULT) {}
void Component::draw(){
  if(outer_layout != nullptr && outer_layout->hasDialog()) return;
  drawComponent();
}
void Component::setFocus(bool focus){
  is_focus = focus;
  win.b = focus ? Border::DEFAULT_RED : Border::DEFAULT;
  win.drawBorder();
  win.refresh();
}
void Component::moveAndResize(int x, int y, int new_width, int new_height){
  width = new_width; height = new_height;
  win.clear();
  win.moveAndResize(x, y, width, height);
  //win.refresh();
  resized();
}
int Component::getChar(){
  return win.getChar();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Dialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Dialog::Dialog()
  : win(Border::DIALOG_DEFAULT){}

/*Dialog::Dialog()
  : t(Terminal::instance()), win(3, 3, t->width() - 6, t->height() - 6)
{
  
}*/
/*Dialog::Dialog(){
  Terminal* t = Terminal::instance();
  int x_offset = t->getWidth() / 10;
  int y_offset = t->getHeight() / 10;
  win.moveAndResize(x_offset, y_offset, t->getWidth()-x_offset, t->getHeight()-y_offset);
}

Dialog::~Dialog(){
  
}

bool respondToKeyPress(int ch){
  
}*/
