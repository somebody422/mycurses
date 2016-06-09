#include <mycurses.hpp>
#include <iostream>

class MyDialog : public SimpleDialog{
	DynamicMessageComponent* c = nullptr;
public:
	MyDialog(std::string, DynamicMessageComponent*);
	~MyDialog();
};
MyDialog::MyDialog(std::string str, DynamicMessageComponent* c_i) : SimpleDialog(str), c(c_i) {
	addComponent(new SimpleInputComponent("type a message here"));
	addComponent(new FillerComponent(), C_DYNAMIC_HEIGHT | C_UNFOCUSABLE);
	addComponent(continue_button);
}
MyDialog::~MyDialog(){
	c->addMessage(((SimpleInputComponent*)components[0])->getString());
	for(Component* component : components) delete component;
	win.clear();
	win.refresh();
}


class MyInputComponent : public SimpleInputComponent{
	DynamicMessageComponent* c;
public:
	MyInputComponent(DynamicMessageComponent* c);
	bool respondToKeyPress(int);
};
MyInputComponent::MyInputComponent(DynamicMessageComponent* c_i) : SimpleInputComponent(), c(c_i) {
	min_height = 3;
}
bool MyInputComponent::respondToKeyPress(int ch){
	if(ch == '\n'){
		if(!outer_layout->hasDialog()){
			outer_layout->setDialog(new MyDialog(getString(), c));
			return true;
		}
	}
	return SimpleInputComponent::respondToKeyPress(ch);
}



int main(){
	ColumnLayout l1(2);
	ColumnLayout* l2 = new ColumnLayout(3);
	ColumnLayout* l3 = new ColumnLayout(2);
	MessageComponent* c1 = new MessageComponent("one");
	MessageComponent* c2 = new MessageComponent("two");
	MessageComponent* c3 = new MessageComponent("three");
	MessageComponent* c4 = new MessageComponent("four");
	MessageComponent* c5 = new MessageComponent("five");
	MessageComponent* c6 = new MessageComponent("six");
	MessageComponent* c7 = new MessageComponent("seven");
	MessageComponent* c8 = new MessageComponent("eight");
	DynamicMessageComponent* dynamic_1 = new DynamicMessageComponent();
	DynamicMessageComponent* dynamic_2 = new DynamicMessageComponent();
	SimpleInputComponent* input_1 = new SimpleInputComponent("component 1");
	SimpleInputComponent* input_2 = new SimpleInputComponent("input_2");
	MyInputComponent* my_input_1 = new MyInputComponent(dynamic_1);
	ListSelectComponent* select_1 = new ListSelectComponent({"one", "two", "three"});	
	dynamic_1->addMessage("first message in");
	dynamic_1->addMessage("second message in");
	dynamic_1->addMessage("third message in");
	dynamic_2->addMessage("this is the second dynamic component");
	dynamic_2->addMessage("yay!");
	
	MessageComponent* l2_1 = new MessageComponent("first l2 component");
	MessageComponent* l2_2 = new MessageComponent("second l2 component");
	DynamicMessageComponent* l2_dynamic_1 = new DynamicMessageComponent();
	DynamicMessageComponent* l2_dynamic_2 = new DynamicMessageComponent();
	DynamicMessageComponent* l2_dynamic_3 = new DynamicMessageComponent();
	l2_dynamic_1->addMessage("message one");
	l2_dynamic_2->addMessage("second dynamic component");
	l2_dynamic_2->addMessage("third dynamic component");
	
	l2->addComponent(l2_dynamic_1, 0, l2->C_DYNAMIC_HEIGHT);
	l2->addComponent(input_2, 0, 0);
	l2->addComponent(l2_2, 1, l2->C_UNFOCUSABLE);
	l2->addComponent(l2_dynamic_2, 1, l2->C_DYNAMIC_HEIGHT | l2->C_UNFOCUSABLE);
	l2->addComponent(l2_dynamic_3, 2, l2->C_DYNAMIC_HEIGHT);
	
	l1.addComponent(dynamic_2, 0, l1.C_DYNAMIC_HEIGHT);
	l1.addComponent(my_input_1, 0, 0);
	l1.addComponent(dynamic_1, 0, .5, l1.C_DYNAMIC_HEIGHT);
	l1.addComponent(select_1, 1, 0);
	l1.addComponent((Component*)l2, 1, l1.C_DYNAMIC_HEIGHT);
	l1.setColumnWeight(0, .5);
	
	
	l1.initAsOuterLayout();
	
	//c1->getChar();
	
	//l.resizeAndDrawComponents();
	
	//Terminal* t = Terminal::instance();
	int ch;
	while((ch = l1.getChar()) != 'q'){
		l1.keyPressed(ch);
	}
	
	delete c1; delete c2; delete c3; delete c4;
	
	/*MessageComponent* c = new MessageComponent("A component! it worked!");
	c->moveAndResize(1, 1, 10, c->getMinHeight());
	c->resized();
	c->getChar();
	c->moveAndResize(1, 10, 10, c->getMinHeight());
	c->getChar();
	delete c;*/
	
	
	delete Terminal::instance();
	return 0;
}