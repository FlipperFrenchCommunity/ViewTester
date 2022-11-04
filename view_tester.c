/*
 * Project: Test View
 * File: view_test.c
 * Create by: Rom1 <rom1@canel.ch> - Flipper French Community - https://github.com/FlipperFrenchCommunity
 * Date: 26 octobre 2022
 * License: GNU GENERAL PUBLIC LICENSE v3
 * Description: Décortiquer et apprendre à faire créer des vues.
 */
#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <input/input.h>
#include <stdlib.h>

typedef struct {
    FuriMessageQueue* event_queue;
    FuriMutex* model_mutex;

    Gui* gui;
    ViewDispatcher* view_dispatcher;
    View* view;
} ViewTesterState;


static void view_dispatcher_view_draw_callback(Canvas* canvas, void* _model) {
    UNUSED(_model);
    canvas_draw_str(canvas, 5, 32, "H4ck the World! with Flipper");
}

static bool view_dispatcher_view_input_callback(InputEvent* event, void* context) {
    UNUSED(event);
    UNUSED(context);
    return false;
}


static uint32_t uart_echo_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

ViewTesterState* view_tester_alloc(){
    ViewTesterState* state = malloc(sizeof(ViewTesterState));

    state->model_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    state->gui = furi_record_open(RECORD_GUI);

    state->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // View dispatcher
    state->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(state->view_dispatcher);
    view_dispatcher_attach_to_gui(state->view_dispatcher, state->gui, ViewDispatcherTypeFullscreen);

    // View
    state->view = view_alloc();
    view_set_draw_callback(state->view, view_dispatcher_view_draw_callback);
    view_set_input_callback(state->view, view_dispatcher_view_input_callback);
    view_set_previous_callback(state->view, uart_echo_exit);

    // View Dispatcher attache la view
    view_dispatcher_add_view(state->view_dispatcher, 0, state->view);
    view_dispatcher_switch_to_view(state->view_dispatcher, 0);
    
    return state;
}

void view_tester_free(ViewTesterState* state){
    view_dispatcher_remove_view(state->view_dispatcher, 0);
    view_free(state->view);
    view_dispatcher_free(state->view_dispatcher);
    furi_record_close(RECORD_GUI);

    furi_message_queue_free(state->event_queue);
    furi_mutex_free(state->model_mutex);

    free(state);
}

int32_t view_tester_app(void* p){
    UNUSED(p);

    ViewTesterState* state = view_tester_alloc();
    view_dispatcher_run(state->view_dispatcher); // Démarre View Dispatcher
    view_tester_free(state);

    return 0;
}
