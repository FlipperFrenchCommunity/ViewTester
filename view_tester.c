/*
 * Project: View Tester
 * File: view_tester.c
 * Create by: Rom1 <rom1@canel.ch> - Flipper French Community - https://github.com/FlipperFrenchCommunity
 * Date: 26 octobre 2022
 * License: GNU GENERAL PUBLIC LICENSE v3
 * Description: Décortiquer et apprendre à créer des vues et de les dispatcher.
 */
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>

/* Ajouter l'en-tête pour view_dispatcher */
#include <gui/view_dispatcher.h>

typedef struct {
    FuriMutex* model_mutex;

    Gui* gui;
    ViewDispatcher* view_dispatcher;    // Ajoute la structure d'état pour le dispatcher
    View* view;                         // Et la structure d'état pour la vue (courant?)
} ViewTesterState;

/**
 * Fonction de rappel pour dessiner l'affichage
 *
 * @param       Canvas pour l'affichage de l'écran
 * @param       _model ??
 */
static void view_dispatcher_view_draw_callback(Canvas* canvas, void* _model) {
    UNUSED(_model);
    canvas_draw_str(canvas, 5, 32, "H4ck the World! with Flipper");
}

/**
 * Fonction de rappel pour terminer le service dispatcher. Elle retourne VIEW_NONE
 * qui demande à view_dispatcher de se terminer proprement.
 * 
 * @param       Context contenant la variable d'état
 */
static uint32_t uart_echo_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

/**
 * Fonction pour initialiser le programme. Elle va allouer les différentes ressources 
 * du programme dans la structure (ViewTesterState) contenant l'état du programme.
 *
 * @return      Structure contenant l'état du programme
 */
ViewTesterState* view_tester_alloc(){
    ViewTesterState* state = malloc(sizeof(ViewTesterState));
    state->gui = furi_record_open(RECORD_GUI);


    /*
     * Allouer les ressources pour faire fonctionner view_dispatcher et les initialiser.
     */
    /* Alloue la structure d'état */
    state->view_dispatcher = view_dispatcher_alloc();
    /* Attache le dispatcher à la session graphique (GUI) */
    view_dispatcher_attach_to_gui(state->view_dispatcher, state->gui, ViewDispatcherTypeFullscreen);
    /* Active une file de messages pour transmettre les événements */
    view_dispatcher_enable_queue(state->view_dispatcher);

    /* Créer notre première vue 0 (view) en allouant la structure d'état */
    state->view = view_alloc();
    /* Définir la fonction de rappel pour l'affichage. */
    view_set_draw_callback(state->view, view_dispatcher_view_draw_callback);
    /* Définir la fonction de rappel pour la bouton de retour. */
    view_set_previous_callback(state->view, uart_echo_exit);

    /* Ajouter la vue dans le dispatcher avec comme index 0 */
    view_dispatcher_add_view(state->view_dispatcher, 0, state->view);
    /* Transmettre à view_port pour afficher la vue 0 */
    view_dispatcher_switch_to_view(state->view_dispatcher, 0);
    

    return state;
}

/**
 * Fonction pour terminer le programme en libérant les ressources.
 *
 * @param       State contenant l'état du programme
 */
void view_tester_free(ViewTesterState* state){
    /* Ne pas oublier de libérer les ressource crée précédemment */
    view_dispatcher_remove_view(state->view_dispatcher, 0);
    view_free(state->view);
    view_dispatcher_free(state->view_dispatcher);


    furi_record_close(RECORD_GUI);
    free(state);
}


/**
 * Point d'entrée principal du programme.
 */
int32_t view_tester_app(void* p){
    UNUSED(p);

    ViewTesterState* state = view_tester_alloc();

    /* Démarrer view_dispatcher */
    view_dispatcher_run(state->view_dispatcher);


    view_tester_free(state);

    return 0;
}
