#include "GameWindow.h"
#include <iostream>

#define WHITE al_map_rgb(255, 255, 255)
#define BLACK al_map_rgb(0, 0, 0)
#define ORANGE_LIGHT al_map_rgb(255, 196, 87)
#define ORANGE_DARK al_map_rgb(255, 142, 71)
#define PURPLE al_map_rgb(149, 128, 255)
#define BLUE al_map_rgb(77, 129, 179)

#define min(a, b) ((a) < (b)? (a) : (b))
#define max(a, b) ((a) > (b)? (a) : (b))

using namespace std;

void
GameWindow::init()
{
    std::string buffer;

    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    icon = al_load_bitmap("assets/images/icon.png");
    background = al_load_bitmap("assets/images/map.png");
    
    screen = al_get_target_bitmap();
    fbo = al_create_bitmap(al_get_bitmap_width(background), al_get_bitmap_height(background));

    al_set_display_icon(display, icon);
    al_reserve_samples(3);

    sample = al_load_sample("assets/audio/Arrow.wav");
    startSound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(startSound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(startSound, al_get_default_mixer());

    sample = al_load_sample("assets/audio/BackgroundMusic.ogg");
    backgroundSound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(backgroundSound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(backgroundSound, al_get_default_mixer());
}

bool
GameWindow::mouse_hover(int startx, int starty, int width, int height)
{
    if(mouse_x >= startx && mouse_x <= startx + width)
        if(mouse_y >= starty && mouse_y <= starty + height)
            return true;

    return false;
}

void
GameWindow::play()
{
    int msg;

    srand(time(NULL));

    msg = -1;
    reset();
    begin();

    while(msg != GAME_EXIT)
    {
        msg = run();
    }

    show_err_msg(msg);
}

void
GameWindow::show_err_msg(int msg)
{
    if(msg == GAME_TERMINATE)
        fprintf(stderr, "Game Terminated...\n");
    else if(msg == GAME_EXIT) 
        fprintf(stderr, "Game Exit...\n");
    else
        fprintf(stderr, "unexpected msg: %d\n", msg);

    destroy();
    exit(9);
}

GameWindow::GameWindow()
{
    if (!al_init())
        show_err_msg(-1);

    printf("Game Initializing...\n");

    ALLEGRO_MONITOR_INFO info;
    al_get_monitor_info(0, &info);
    width = info.x2 - info.x1;
    height = info.y2 - info.y1;
    // width = 1920;
    height = 1080;
    // al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    display = al_create_display(width, height);
    event_queue = al_create_event_queue();

    timer = al_create_timer(1.0 / FPS);

    if(timer == NULL)
        show_err_msg(-1);

    if (display == NULL || event_queue == NULL)
        show_err_msg(-1);

    al_init_primitives_addon();
    al_init_font_addon(); // initialize the font addon
    al_init_ttf_addon(); // initialize the ttf (True Type Font) addon
    al_init_image_addon(); // initialize the image addon
    al_init_acodec_addon(); // initialize acodec addon

    al_install_keyboard(); // install keyboard event
    al_install_mouse();    // install mouse event
    al_install_audio();    // install audio event

    font = al_load_ttf_font("../assets/fonts/Caviar_Dreams_Bold.ttf",12,0); // load small font
    Medium_font = al_load_ttf_font("../assets/fonts/Caviar_Dreams_Bold.ttf",24,0); //load medium font
    Large_font = al_load_ttf_font("../assets/fonts/Caviar_Dreams_Bold.ttf",36,0); //load large font

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    init();
}

void
GameWindow::begin()
{
    draw();

    // al_play_sample_instance(backgroundSound);

    al_start_timer(timer);
}

int
GameWindow::run()
{
    int error = GAME_CONTINUE;

    if (!al_is_event_queue_empty(event_queue)) {

        error = process_event();
    }
    return error;
}

int
GameWindow::update()
{
    // models' status ex. moving

    crew1.update();
    crew2.update();
    return GAME_CONTINUE;
}

void
GameWindow::reset()
{
    crew1 = Crew(0);
    crew2 = Crew(1);
    mute = false;
    redraw = false;

    // stop sample instance
    al_stop_sample_instance(backgroundSound);
    al_stop_sample_instance(startSound);

    // stop timer
    al_stop_timer(timer);
}

void
GameWindow::destroy()
{
    reset();

    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_font(font);
    al_destroy_font(Medium_font);
    al_destroy_font(Large_font);

    al_destroy_timer(timer);

    al_destroy_bitmap(icon);
    al_destroy_bitmap(background);

    al_destroy_sample(sample);
    al_destroy_sample_instance(startSound);
    al_destroy_sample_instance(backgroundSound);

}

int
GameWindow::process_event()
{
    int i;
    int instruction = GAME_CONTINUE;

    al_wait_for_event(event_queue, &event);
    redraw = false;

    if(event.type == ALLEGRO_EVENT_TIMER) {
        if(event.timer.source == timer) {
            redraw = true;
        }
        else {

        }
    }
    else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        return GAME_EXIT;
    }
    else if(event.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch(event.keyboard.keycode) {
            case ALLEGRO_KEY_UP:
                crew1.set_direction(UP);
                break;
            case ALLEGRO_KEY_DOWN:
                crew1.set_direction(DOWN);
                break;
            case ALLEGRO_KEY_LEFT:
                crew1.set_direction(LEFT);
                break;
            case ALLEGRO_KEY_RIGHT:
                crew1.set_direction(RIGHT);
                break;
            case ALLEGRO_KEY_P:
                /*TODO: handle pause event here*/
                break;
            case ALLEGRO_KEY_M:
                mute = !mute;
                if(mute)
                    al_stop_sample_instance(backgroundSound);
                else
                    al_play_sample_instance(backgroundSound);
                break;
        }
    }
    else if(event.type == ALLEGRO_EVENT_KEY_UP) {
        switch(event.keyboard.keycode) {
            case ALLEGRO_KEY_UP:
                crew1.remove_direction(UP);
                break;
            case ALLEGRO_KEY_DOWN:
                crew1.remove_direction(DOWN);
                break;
            case ALLEGRO_KEY_LEFT:
                crew1.remove_direction(LEFT);
                break;
            case ALLEGRO_KEY_RIGHT:
                crew1.remove_direction(RIGHT);
                break;
        }
    }
    else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        if(event.mouse.button == 1) {
            // int x = event.mouse.x;
            // int y = event.mouse.y;
            // crew1.moveTo(x, y);
        }
        else if(event.mouse.button == 2) {

        }
    }
    else if(event.type == ALLEGRO_EVENT_MOUSE_AXES){
        mouse_x = event.mouse.x;
        mouse_y = event.mouse.y;

    }

    if(redraw) {
        // update each object in game
        instruction = update();

        // Re-draw map
        draw();
        redraw = false;
    }

    return instruction;
}

void
GameWindow::draw()
{
    screen = al_get_target_bitmap();
    al_set_target_bitmap(fbo);
    al_draw_bitmap(background, 0, 0, 0);
    // ALLEGRO_TRANSFORM trans, prevTrans;
    // al_copy_transform(&prevTrans, al_get_current_transform());
    // al_identity_transform(&trans);
    // float scale_factor = 2.5*height/1080;
    // al_scale_transform(&trans, scale_factor, scale_factor); //1920, 1080 1992*2.5->1080 1992*x->1440
    // al_use_transform(&trans);
    // al_draw_bitmap_region(background, crew1.getPosition()['x']-width/(4*scale_factor), crew1.getPosition()['y']-height/(2*scale_factor), width/2, height, 0, 0, 0);
    // al_draw_bitmap_region(background, crew2.getPosition()['x']-width/(4*scale_factor), crew2.getPosition()['y']-height/(2*scale_factor), width/2, height, width/(2*scale_factor), 0, 0);
    // al_use_transform(&prevTrans);
    // al_draw_filled_rectangle(width/2-5, 0, width/2+5, height, al_map_rgb(0, 0, 0));
    al_draw_filled_rectangle(0, 0, 500, 500, al_map_rgb(255, 0, 0));
    crew1.draw(width, height);
    // crew2.draw(width, height);
    al_set_target_bitmap(screen);

    ALLEGRO_TRANSFORM trans, prevTrans;
    al_copy_transform(&prevTrans, al_get_current_transform());
    al_identity_transform(&trans);
    float scale_factor = 2.5*height/1080;
    al_scale_transform(&trans, scale_factor, scale_factor); //1920, 1080 1992*2.5->1080 1992*x->1440
    al_use_transform(&trans);
    al_draw_bitmap(fbo, 0, 0, 0);
    // al_draw_bitmap_region(fbo, crew1.getPosition()['x']-width/(4*scale_factor), crew1.getPosition()['y']-height/(2*scale_factor), width/2, height, 0, 0, 0);
    al_use_transform(&prevTrans);


    al_flip_display();

}
