#include <mycurses.hpp>


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//ColumnLayout
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
 * Methods only used by the outer-most layout
 */
void ColumnLayout::initAsOuterLayout(){
	Terminal* t = Terminal::instance();
	width = t->getWidth();
	height = t->getHeight();
	
	if(empty) return;
	
	for(unsigned int col = 0; col < components.size(); col++){
		if(!(components[col].empty())){
			selected_col = col;
			break;
		}
	}
	
	for(Column column : components){
		for(Component* c : column){
			c->setOuterLayout(this);
		}
	}
	
	resizeAndDrawComponents();
	components[selected_col].selected_component->setFocus(true);
}

void ColumnLayout::keyPressed(int ch){
	if(empty) return;
	Component* c = components[selected_col].selected_component;
	Column column = components[selected_col];
	
	if(ch == Terminal::TERMINAL_RESIZED){
		Terminal* t = Terminal::instance();
		t->resized();
		/*width = t->getWidth();
		height = t->getHeight();*/
		resizeAndDrawComponents();
		return;
	}
	
	if(dialog != nullptr){
		if(dialog->respondToKeyPress(ch)){
			delete dialog;
			dialog = nullptr;
			//resizeAndDrawComponents();
			drawComponent();
		}
		return;
	}
	
	if(c != nullptr && c->respondToKeyPress(ch)) return;
	
	switch(ch){
		case Terminal::UP_ARROW_KEY:
			if(c == nullptr) break;
			if(c->getPrev() != nullptr){
				c->setFocus(false);
				components[selected_col].selected_component = c->getPrev();
				components[selected_col].selected_component->setFocus(true);
			}
			else{
				c->setFocus(false);
				components[selected_col].selected_component = column.last_focusable_component;
				components[selected_col].selected_component->setFocus(true);
			}
			break;
		case Terminal::DOWN_ARROW_KEY:
			if(c == nullptr) break;
			if(c->getNext() != nullptr){
				c->setFocus(false);
				components[selected_col].selected_component = c->getNext();
				components[selected_col].selected_component->setFocus(true);
			}
			else{
				c->setFocus(false);
				components[selected_col].selected_component = column.first_focusable_component;
				components[selected_col].selected_component->setFocus(true);
			}
			break;
		case Terminal::RIGHT_ARROW_KEY:
			if(num_cols <= 1) break;
			for(int col = 1; col < num_cols; col++){
				Column* other_column = &components[(selected_col + col) % num_cols];
				if(other_column->selected_component != nullptr){
					c->setFocus(false);
					other_column->selected_component->setFocus(true);
					selected_col = (selected_col + col) % num_cols;
					break;
				}
			}
			break;
		case Terminal::LEFT_ARROW_KEY:
			if(num_cols <= 1) break;
			for(int col = selected_col-1; col != selected_col; col--){
				if(col < 0) col = num_cols - 1;
				Column* other_column = &components[col];
				if(other_column->selected_component != nullptr){
					column.selected_component->setFocus(false);
					other_column->selected_component->setFocus(true);
					selected_col = col;
					break;
				}
			}
			break;
	}
} //void keyPressed

void ColumnLayout::resizeAndDrawComponents(){
	Terminal* t = Terminal::instance();
	moveAndResize(0, 0, t->getWidth(), t->getHeight());
	if(dialog != nullptr){
			int dialog_width_offset = width / 8;
			int dialog_height_offset = height / 8;
			dialog->moveAndResize(dialog_width_offset, dialog_height_offset, width - 2*dialog_width_offset, height - 2*dialog_height_offset);
		}
		drawComponent();
}

bool ColumnLayout::hasDialog(){
	return dialog != nullptr;
}

void ColumnLayout::setDialog(Dialog* d){
	dialog = d;
	resizeAndDrawComponents();
}





/*
 * Methods used by any columnlayout
 */
ColumnLayout::ColumnLayout(int num_cols_i)
	: num_cols(num_cols_i), components(num_cols_i)
{
	//total_horizontal_weight = num_cols;
	for(int i = 0; i < num_cols; i++) total_horizontal_weight += 1.0;
}

bool ColumnLayout::respondToKeyPress(int ch){
	if(empty) return false;
	Component* c = components[selected_col].selected_component;
	Column column = components[selected_col];
	
	//give the component a chance to respond first
	if(c != nullptr && c->respondToKeyPress(ch)) return true;
	
	switch(ch){
		case Terminal::UP_ARROW_KEY:
			if(c == nullptr) break;
			if(c->getPrev() != nullptr){
				c->setFocus(false);
				components[selected_col].selected_component = c->getPrev();
				components[selected_col].selected_component->setFocus(true);
				return true;
			}
			break;
		case Terminal::DOWN_ARROW_KEY:
			if(c == nullptr) break;
			if(c->getNext() != nullptr){
				c->setFocus(false);
				components[selected_col].selected_component = c->getNext();
				components[selected_col].selected_component->setFocus(true);
				return true;
			}
			break;
		case Terminal::RIGHT_ARROW_KEY:
			if(num_cols <= 1) break;
			for(int col = selected_col+1; col < num_cols; col++){
				if(components[col].selected_component != nullptr){
					c->setFocus(false);
					components[col].selected_component->setFocus(true);
					selected_col = col;
					return true;
				}
			}
			break;
		case Terminal::LEFT_ARROW_KEY:
			if(num_cols <= 1) break;
			for(int col = selected_col-1; col >= 0; col--){
				if(components[col].selected_component != nullptr){
					c->setFocus(false);
					components[col].selected_component->setFocus(true);
					selected_col = col;
					return true;
				}
			}
			//break;
	}
	return false;
} //bool respondToKeyPress


void ColumnLayout::moveAndResize(int new_x, int new_y, int new_width, int new_height){
	x = new_x; y = new_y; width = new_width; height = new_height;
	//int component_x = (width % num_cols) / 2 + x;
	int component_x = x;
	for(Column& column : components){
		int column_width = column.horizontal_weight / total_horizontal_weight * (width);
		int component_y = y;
		for(Component* c : column){
			if(component_is_dynamic[c]){ //component has dynamic height
				int component_height = component_vertical_weight[c] / column.total_vertical_weight * (height - column.total_static_height);
				c->moveAndResize(component_x, component_y, column_width, component_height);
				component_y += component_height;
			}
			else{ //component has static height
				c->moveAndResize(component_x, component_y, column_width, c->getMinHeight());
				component_y += c->getMinHeight();
			}
		}
		component_x += column_width;
	} //for(Column& column : components)
}

void ColumnLayout::drawComponent(){
	for(Column& column : components){
		for(Component* component : column){
			component->drawComponent();
		}
	}
	if(dialog != nullptr) dialog->drawComponents();
}


int ColumnLayout::getChar(){
	if(dialog != nullptr) return dialog->getChar();
	if(empty) return win.getChar();
	return components[selected_col].selected_component->getChar();
	//return win.getChar();
}

void ColumnLayout::setOuterLayout(Layout* l){
	for(Column& col : components){
		for(Component* c : col){
			c->setOuterLayout(l);
		}
	}
	outer_layout = l;
}

void ColumnLayout::setColumnWeight(int col, double weight){
	total_horizontal_weight -= components[col].horizontal_weight;
	components[col].horizontal_weight = weight;
	total_horizontal_weight += weight;
}

//todo: handle throwing of errors, for ex if col is higher than components.size()
/*void ColumnLayout::addComponent(Component* c, int col){
	addComponent(c, col, 1, 0);
}
void ColumnLayout::addComponent(Component* c, int col, double weight){
	addComponent(c, col, weight, 0);
}*/
void ColumnLayout::addComponent(Component* c, int col, unsigned char flags){
	addComponent(c, col, 1, flags);
}
void ColumnLayout::addComponent(Component* c, int col, double weight, unsigned char flags){
	Column& column = components[col];
	bool unfocusable = BIT_FROM_UCHAR(flags, 0);
	bool dynamic_height = BIT_FROM_UCHAR(flags, 7);
	if(!unfocusable){
		if(column.first_focusable_component == nullptr){
			column.first_focusable_component = c;
			column.last_focusable_component = c;
			column.selected_component = c;
		}
		else{
			column.back()->next = c;
			c->prev = column.back();
			column.last_focusable_component = c;
		}
	}
	if(dynamic_height){
		column.total_vertical_weight += weight;
	}
	else column.total_static_height += c->getMinHeight();
	column.push_back(c);
	if(empty && !unfocusable){
		selected_col = col;
		empty = false;
	}
	component_is_dynamic[c] = dynamic_height;
	component_vertical_weight[c] = weight;
}


void ColumnLayout::setFocus(bool b){
	if(empty) return;
	Component* c = components[selected_col].selected_component;
	if(c != nullptr) c->setFocus(b);
}


void ColumnLayout::resized(){
	for(Column& column : components){
		for(Component* c : column) c->resized();
	}
}
