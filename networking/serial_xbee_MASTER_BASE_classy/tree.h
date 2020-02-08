class Tree{
    public:
        char id;
        bool local_win;
        bool button_pressed[2] = {false, false};
        bool button_state_for_sending;
        int last_pressed_time;

    Tree(char tree_id){
        id = tree_id;
        local_win = false;
        last_pressed_time = 0;
    }

    void reset_tree(){
        local_win = false;
        button_pressed[0] = false;
        button_pressed[1] = false;
        last_pressed_time = 0;
        button_state_for_sending = false;
    }

    void update_button_pressed(bool button_state){
        last_pressed_time = millis();

        if(button_state != button_pressed[0] && button_state !=button_pressed[1]){
            button_state_for_sending = button_state;
        }

        button_pressed[0] = button_pressed[1];
        button_pressed[1] = button_state;
    }
};