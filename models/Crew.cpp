#include "Crew.h"
#include <cmath>
#include <iostream>

const int draw_frequency = 3;
const Map game_map;

Crew::Crew(int id) {
    this->id = id;
    if(id==0) {
        pos_x = 1440;
        pos_y = 986;
        color = "black";
    }
    else if(id==1) {
        pos_x = 2040;
        pos_y = 596;
        color = "red";
    }
    speed = 3;
    counter = 0;
    direction = 0;
    for(int i=0; i<4; i++) direction_ary[i] = false;
    load_images();
}

Crew::~Crew() {

}

void Crew::load_images() {
    string buf;
    for(int i=1; i<=23; i++) {
        buf = "assets/images/"+color+"/"+to_string(i)+".png";
        ALLEGRO_BITMAP *img;
        img = al_load_bitmap(buf.c_str());
        if(img)
            images.push_back(img);
    }
}

map<char, int> Crew::getPosition() {
    map<char, int> m;
    m.insert(pair<char, int>('x', pos_x));
    m.insert(pair<char, int>('y', pos_y));
    return m;
}

ostream& operator<<(ostream &os, Crew &hero) {
    return os;
}

void Crew::draw(int width, int height) {
    int w, h;
    ALLEGRO_TRANSFORM trans, prevTrans;
    al_copy_transform(&prevTrans, al_get_current_transform());
    al_identity_transform(&trans);
    if(this->direction==0 || this->direction==15 || this->direction==3 || this->direction==12) {
        w = al_get_bitmap_width(images[22]);
        h = al_get_bitmap_height(images[22]);
        float scale_factor = 0.38*height/1080;
        al_scale_transform(&trans, scale_factor, scale_factor);
        al_use_transform(&trans);
        if(id==0)
            al_draw_bitmap(images[22], width/(4*scale_factor) - w/2, height/(2*scale_factor) - h/2 + 20, flag);
        if(id==1)
            al_draw_bitmap(images[22], 3*width/(4*scale_factor) - w/2, height/(2*scale_factor) - h/2 + 20, flag);
    }
    else {
        w = al_get_bitmap_width(images[sprite_pos]);
        h = al_get_bitmap_height(images[sprite_pos]);
        // float scale_factor = 0.2*height/1080;
        // al_scale_transform(&trans, scale_factor, scale_factor);
        // al_use_transform(&trans);
        if(id==0) {
            al_draw_bitmap(images[sprite_pos], pos_x - w/2, pos_y - h/2, flag);
            // al_draw_bitmap(images[sprite_pos], width/(4*scale_factor) - w/2, height/(2*scale_factor) - h/2, flag);
            // al_draw_rectangle(width/(4*scale_factor) - w/2, height/(2*scale_factor) - h/2,width/(4*scale_factor) - w/2+ w, height/(2*scale_factor) - h/2+h, al_map_rgb(255, 0, 0), 10);
        }
        // if(id==1) {
        //     al_draw_bitmap(images[sprite_pos], 3*width/(4*scale_factor) - w/2, height/(2*scale_factor) - h/2, flag);
        //     al_draw_rectangle(3*width/(4*scale_factor) - w/2, height/(2*scale_factor) - h/2, 3*width/(4*scale_factor) - w/2+ w, height/(2*scale_factor) - h/2+h, al_map_rgb(255, 0, 0), 10);
        // }
    }
    // al_use_transform(&prevTrans);
}

void Crew::update() {
    int w, h;
    float scale_factor = 0.2*1080/1080;
    w = al_get_bitmap_width(images[sprite_pos])*scale_factor;
    h = al_get_bitmap_height(images[sprite_pos])*scale_factor;
    counter++;
    if(direction_ary[LEFT] && !direction_ary[RIGHT]) flag = 1;
    if(!direction_ary[LEFT] && direction_ary[RIGHT]) flag = 0;
    if(this->direction==0 || this->direction==15 || this->direction==3 || this->direction==12) {

    }
    else {
        sprite_pos = (sprite_pos + 1) % 22;
        float scale = sqrt(2); 
        int y1_offset = 25;
        int y2_offset = 30;
        int x1_offset = 25;
        int x2_offset = 15;
        int stuck_up = false;
        int stuck_down = false;
        int stuck_left = false;
        int stuck_right = false;
        for(auto line: game_map.tiles) {
            if(line.side==0) {// face down
                if(pos_y >= line.y1-y1_offset-speed && pos_y<=line.y1 - y1_offset && pos_x>line.x1-x1_offset && pos_x<line.x2+x2_offset
                    && (this->direction==4 || this->direction==5 || this->direction==6 || this->direction==7)) {
                    pos_y = line.y1 - y1_offset;
                    stuck_up = true;
                }
            }
            if(line.side==1) {// face up
                if(pos_y <= line.y1-y2_offset+speed && pos_y>=line.y1 - y2_offset && pos_x>line.x1-x1_offset && pos_x<line.x2+x2_offset
                    && (this->direction==8 || this->direction==9 || this->direction==10 || this->direction==11)) {
                    pos_y = line.y1 - y2_offset;
                    stuck_down = true;
                }
            }
            if(line.side==2) {// face left
                if(pos_x <= line.x1-x1_offset+speed && pos_x>=line.x1 - x1_offset && pos_y>line.y1-y1_offset && pos_y<line.y2-y2_offset
                    && (this->direction==2 || this->direction==6 || this->direction==10 || this->direction==14)) {
                    pos_x = line.x1 - x1_offset;
                    stuck_right = true;
                }
            }
            if(line.side==3) {// face right
                if(pos_x >= line.x1+x2_offset-speed && pos_x<=line.x1+x2_offset && pos_y>line.y1-y1_offset && pos_y<line.y2-y2_offset
                    && (this->direction==1 || this->direction==5 || this->direction==9 || this->direction==13)) {
                    pos_x = line.x1+x2_offset;
                    stuck_left = true;
                }
            }
        }
        switch(this->direction) {
            case 1: if(!stuck_left) pos_x -= speed; break;
            case 2: if(!stuck_right) pos_x += speed; break;
            case 4: if(!stuck_up) pos_y -= speed; break;
            case 5: 
                if(!stuck_left) pos_x -= speed/scale; 
                if(!stuck_up) pos_y -= speed/scale; 
            break;
            case 6: 
                if(!stuck_right) pos_x += speed/scale; 
                if(!stuck_up) pos_y -= speed/scale; 
            break;
            case 7: if(!stuck_up) pos_y -= speed; break;
            case 8: if(!stuck_down) pos_y += speed; break;
            case 9: 
                if(!stuck_left) pos_x -= speed/scale; 
                if(!stuck_down) pos_y += speed/scale; 
            break;
            case 10: 
                if(!stuck_right) pos_x += speed/scale; 
                if(!stuck_down) pos_y += speed/scale; 
            break;
            case 11: if(!stuck_down) pos_y += speed; break;
            case 13: if(!stuck_left) pos_x -= speed; break;
            case 14: if(!stuck_right) pos_x += speed; break;
        }
    }
}

void Crew::set_direction(int dir) {
    direction_ary[dir] = true;
    int sum = 0;
    for(int i=0; i<4; i++) {
        if(direction_ary[i]) sum+=1<<i;
    }
    this->direction = sum;
}
void Crew::remove_direction(int dir) {
    direction_ary[dir] = false;
    int sum = 0;
    for(int i=0; i<4; i++) {
        if(direction_ary[i]) sum+=1<<i;
    }
    this->direction = sum;
}

int Crew::get_direction() {
    return -1;
}