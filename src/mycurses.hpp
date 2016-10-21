
#ifndef MY_CURSES_HPP
#define MY_CURSES_HPP

#include <ncurses.h>
#include <map>
#include <vector>
#include <string>
#include <deque>
#include <thread> //yield
#include <exception>
//todo: this is not finished!!
//#include <mutex>

//todo: a few more convenient draw methods for Window like drawLine,
// drawBox, ect

//todo: Maybe not for this project, but struct representing 2d array of
// characters, each character has color value as well, methods provided
// to transform 'image' (change color, draw, stuff like that)

//b=0 is left-most bit, b=7 is right-most
#define BIT_FROM_UCHAR(c, b) (c >> b & 0x01)

//returns the start x value to print the string, which will
// result in it being centered.
#define CENTER_STRING_X(str, line_len) ((str.length() >= line_len) ? 0 : (line_len-str.length())/2)

class Window;
class Layout;
class Dialog;

struct ColorPair{
  int color_one, color_two;
  ColorPair(int c1, int c2) : color_one(c1), color_two(c2) {}
  //define less than comparison so this can be used in STL map
  bool operator<(const ColorPair& cp) const {
    return color_one + color_two < cp.color_one + cp.color_two;
  }
};

class MycursesInterruptedException : public std::exception{
  //todo
};

class Terminal{
  unsigned int width, height;
  bool has_color;
  std::map<ColorPair, int> color_map;
  unsigned int next_color = 1;
  static bool interrupt_next_input;
  
public:
  ~Terminal();
  //todo: first check if input will fit on screen, return if not
  void print(char, int attribute, int x, int y);
  void print(char, int x, int y);
  void print(std::string, int x, int y);
  void print(std::string, int attributes, int x, int y);
  void print(std::string, int fg_color, int bg_color, int x, int y);
  
  void refresh();
  int getChar(); //blocking call waiting for user input
  void clear();
  unsigned int getWidth();
  unsigned int getHeight();
  void resized();
  bool hasColor();
  
  //attributes
  void attributeOn(int);
  void attributeOff(int);
  static const int BOLD = A_BOLD;
  static const int UNDERLINE = A_UNDERLINE;
  static const int BLINK = A_BLINK;
  static const int BLACK = 0;
  static const int RED = 1;
  static const int GREEN = 2;
  static const int YELLOW = 3;
  static const int BLUE = 4;
  static const int MAGENTA = 5;
  static const int CYAN = 6;
  static const int WHITE = 7;
  
  static const int RIGHT_ARROW_KEY = KEY_RIGHT;
  static const int DOWN_ARROW_KEY = KEY_DOWN;
  static const int UP_ARROW_KEY = KEY_UP;
  static const int LEFT_ARROW_KEY = KEY_LEFT;
  //static const int ENTER_KEY = KEY_ENTER; //use '\n' instead
  //todo: ncurses got some codes wrong on my machine. It may
  // be different than mine on a different machine. not having a way to
  // determine this dynamically could lead to problems
  static const int BACKSPACE_KEY = 127;
  static const int TERMINAL_RESIZED = 410;
  
  static void interrupt();
  
private:
  int getColorAttributeValue(int c1, int c2);
  
  //singleton
public:
  void end();
  static Terminal* instance();
private:
  static Terminal* terminal;
  Terminal();
  
  //Terminal has friends!
  friend class Window;
};


struct Border{
  char ls, rs, ts, bs, tl, tr, bl, br;
  ColorPair color;
  static Border DEFAULT;
  static Border DEFAULT_RED;
  static Border DIALOG_DEFAULT;
};


class Window{
  Terminal* t;
  WINDOW* win;
  unsigned int x, y, width, height;
  bool has_border;
  int o; //offset is 1 if border, 0 if no border
  
  void refreshThis();
public:
  Border b;
//WINDOW* win;
  Window();
  Window(Border border);
  Window(int x, int y, int width, int height);
  Window(int x, int y, int width, int height, Border border);
  ~Window();
  
  void print(char, int x, int y);
  void print(char, int attribute, int x, int y);
  void print(std::string, int x, int y);
  void print(std::string, int attributes, int x, int y);
  void print(std::string, int fg_color, int bg_color, int x, int y);
  
  int getChar(); //puts this window on top
  void clear();
  void setBorder(bool);
  //move and resize will clear window
  //if calling both move and resize, call resize first
  void move(int x, int y);
  void resize(int width, int height);
  void moveAndResize(int x, int y, int width, int height);
  void refresh();
  void drawBorder(); //redraw the border
  unsigned int getX(); unsigned int getY();
  unsigned int getWidth();
  unsigned int getHeight();
  
  void attributeOn(int);
  void attributeOff(int);
  void colorOn(int fg, int bg);
  void colorOff(int fg, int bg);
};


class Component{
protected:
  Window win;
  Layout* outer_layout = nullptr;
  bool is_focus = false;
  unsigned int height, width;
  unsigned int min_height = 0, min_width = 0;
  
public:
  Component* prev = nullptr;
  Component* next = nullptr;
  
  Component();
  
  //called by draw
  virtual void drawComponent() = 0;
  
  /* called by other components and objects */
  virtual void draw();
  virtual ~Component(){}
  virtual void resized(){}
  virtual void setFocus(bool);
  //return false if no action is taken
  virtual bool respondToKeyPress(int ch){ return false; }
  virtual void moveAndResize(int x, int y, int width, int height);
  virtual int getChar();
  virtual void setOuterLayout(Layout* l){ outer_layout = l; }
  
  Component* getPrev(){ return prev; }
  Component* getNext(){ return next; }
  unsigned int getWidth() { return width; }
  unsigned int getHeight() { return height; }
  unsigned int getMinHeight() { return min_height; }
  unsigned int getMinWidth() { return min_width; }
};


//todo: is a layout responsible for destroying its components?
class Layout : public Component{
protected:
  //Dialog* dialog = nullptr;
  int x = 0, y = 0;
  
public:  
  /* only the outside layout will be using these */
  virtual void initAsOuterLayout() = 0;
  virtual void keyPressed(int ch) = 0;
  virtual bool hasDialog() = 0;
  virtual void setDialog(Dialog* dialog) = 0;
  virtual void resizeAndDrawComponents() = 0;
  
  //component methods
  //virtual bool respondToKeyPress(int ch);
  virtual void moveAndResize(int x, int y, int width, int height) = 0;
  virtual void drawComponent() = 0;
  virtual int getChar() = 0;
  virtual void setOuterLayout(Layout* l) = 0;
  virtual void resized() = 0;
  virtual void setFocus(bool) = 0;
  
  /*
   * Flags for adding components:
   * X X X X  X X X F
   * 
   * Bit 0/F: Component cannot be focused
   */
   const unsigned char C_UNFOCUSABLE = 0x01;
};


class Dialog{
protected:
  Window win;
  bool finished = false;
public:
  Dialog();
  virtual ~Dialog(){}
  //returns true if dialog should be closed
  virtual void respondToKeyPress(int ch) = 0;
  virtual void moveAndResize(int x, int y, int width, int height) = 0;
  virtual void drawComponents() = 0;
  virtual void addComponent(Component*) = 0;
  virtual void addComponent(Component*, unsigned char flags) = 0;
  virtual int getChar() = 0;

  virtual void setFinished() {finished = true;}
  bool isFinished() {return finished;}
  /*
   * Flags for adding components:
   * X X X X  X X X F
   * 
   * Bit 0/F: Component cannot be focused
   */
   const unsigned char C_UNFOCUSABLE = 0x01;
};





//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//some implementations
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class ColumnLayout : public Layout{
  /*class CL_Component{
    Component* component;
    bool is_dynamic;
    double weight = 1;
    CL_Component* next = nullptr;
    CL_Component* = prev = nullptr;
    CL_Component(Component* c) : component(c), is_dynamic(false){}
    CL_Component(Component* c, bool is_dynamic_i)
      : component(c), is_dynamic(is_dynamic_i) {}
    CL_Component(Component* c, double weight_i)
      : component(c), is_dynamic(true), weight(weight_i) {}
    CL_Component(Component* c, bool is_dynamic_i, double weight_i)
      : component(c), is_dynamic(is_dynamic_i), weight(weight_i) {}
    virtual void drawComponent(){ component->drawComponent(); }
    
    virtual void draw(){ component->draw(); }
    //virtual ~Component(){}
    virtual void resized(){ component->resized(); }
    virtual void setFocus(bool b){ return component->setFocus(b); }
    virtual bool respondToKeyPress(int ch){ return component->respondToKeyPress(ch); }
    virtual void moveAndResize(int x, int y, int width, int height){ component->moveAndResize(x, y, width, height); }
    virtual int getChar(){ return component->getChar(); }
    virtual void setOuterLayout(Layout* l){ component->draw(); }
    CL_Component getNext(){ return next; }
    CL_Component getPrev(){ return prev; }
    friend class ColumnLayout;
  };*/
  class Column : public std::vector<Component*>{
    Component* selected_component = nullptr;
    Component* first_focusable_component = nullptr;
    Component* last_focusable_component = nullptr;
    double horizontal_weight = 1;
    int total_static_height = 0;
    double total_vertical_weight = 0;
    friend class ColumnLayout;
  };
  int num_cols, selected_col = 0;
  Dialog* dialog = nullptr;
  std::vector<Column> components;
  std::map<Component*, bool> component_is_dynamic;
  std::map<Component*, double> component_vertical_weight;
  double total_horizontal_weight = 0;
  bool empty = true;
public:
  
  //outermost layout
  void initAsOuterLayout();
  void keyPressed(int);
  void resizeAndDrawComponents();
  bool hasDialog();
  void setDialog(Dialog*);
  
  //any ColumnLayout instance
  ColumnLayout(int num_cols);
  void resized();
  int getChar();
  void setOuterLayout(Layout* l);
  void setFocus(bool);
  bool respondToKeyPress(int ch);
  void moveAndResize(int new_x, int new_y, int new_width, int new_height);
  void drawComponent();
  void setColumnWeight(int col, double weight);
  //void addComponent(Component*, int col);
  //void addComponent(Component*, int col, double weight);
  void addComponent(Component*, int col, unsigned char flags);
  void addComponent(Component*, int col, double weight, unsigned char flags);
  /*
   * Flags for adding components:
   * D X X X  X X X F
   * 
   * Bit 0/F: Component cannot be focused
   * Bit 7/D: Component has dynamic height
   */
   const unsigned char C_DYNAMIC_HEIGHT = 0x80;
};


//a single column of components
//child classes handle logic in constructor, destructor, and done
class SimpleDialog : public Dialog{
protected:
  int component_y_start = 0;
  std::string message;
  ColumnLayout layout;
  //std::vector<Component*> components;
  //Component* selected_component = components.begin();
  //Component* selected_component = nullptr;
  //Component* first_focusable_component = nullptr;
  //Component* last_focusable_component = nullptr;
  //DialogContinueButton* continue_button = new DialogContinueButton();
  //std::map<Component*, bool> component_is_dynamic;
  //int num_dynamic_components = 0;
  //int total_static_height = 0; //total height on non-dynamic components
public:
  SimpleDialog();
  SimpleDialog(std::string message);
  //virtual ~SimpleDialog();
  void respondToKeyPress(int ch){
    layout.respondToKeyPress(ch);
  }
  void drawComponent();
  void moveAndResize(int x, int y, int width, int height);
  void drawComponents();
  int getChar();
  void addComponent(Component* c){
    layout.addComponent(c, 0, 0);
  }
  void addComponent(Component* c, unsigned char flags){
    layout.addComponent(c, 0, flags);
  }
  virtual void done() {} //implement in child
  
  /*
   * Flags for adding components:
   * D X X X  X X X F
   * 
   * Bit 0/F: Component cannot be focused
   * Bit 7/D: Component has dynamic height
   */
   const unsigned char C_DYNAMIC_HEIGHT = 0x80;
};


class SimpleDialogContinueButton : public Component{
  //bool finished = false;
  SimpleDialog* d;
public:
  SimpleDialogContinueButton(SimpleDialog*);
  void drawComponent();
  bool respondToKeyPress(int);
  //bool isFinished(){ return finished; }
};

class SimpleDialogCancelButton : public Component{
  SimpleDialog* d;
public:
  SimpleDialogCancelButton(SimpleDialog*);
  void drawComponent();
  bool respondToKeyPress(int);
};




class MessageComponent : public Component{
  std::string message;
public:
  MessageComponent(std::string message);
  void drawComponent();
};


class DynamicMessageComponent : public Component{
  const unsigned int MAX_MESSAGES = 60;
  std::deque<std::string> messages;
  
public:
  DynamicMessageComponent();
  void drawComponent();
  void addMessage(std::string message);
};


class SimpleInputComponent : public Component{
  std::string label;
  bool has_label = false;
  int y = 0;
  const int INPUT_OFFSET = 2;
  unsigned int input_length = 0;
  char input[100];
public:
  SimpleInputComponent();
  SimpleInputComponent(std::string label);
  virtual void drawComponent();
  virtual bool respondToKeyPress(int ch);
  
  void clearInput();
  std::string getString();
};


class FillerComponent : public Component{
public:
  FillerComponent();
  FillerComponent(int height);
  void drawComponent(){}
  void repondToKeyPress(int ch){}
};


class ListSelectComponent : public Component{
  std::vector<std::string> options;
  std::string label;
  int y = 0;
  unsigned int selected_option = 0;
  
public:
  ListSelectComponent(std::vector<std::string> options);
  ListSelectComponent(std::vector<std::string> options, std::string label);
  void drawComponent();
  bool respondToKeyPress(int ch);
  std::string getSelectedOption();
};


/*
 *
 */
class ScrollableListComponent : public Component{
  std::vector<std::string> options;
  int options_offset;
  bool can_scroll_up = false;
  bool can_scroll_down = false;
  //int selected_option_index;
  //std::string selected_option;
public:
  ScrollableListComponent(std::vector<std::string>);
  void drawComponent();
  bool respondToKeyPress(int ch);
  std::string getSelectedOption();
};

#endif
