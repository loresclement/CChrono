#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int started = 0;
int paused = 0;

double timeInSecond = 0;
long hours = 0, minutes = 0, secondes = 0;

GtkWidget *label;
GtkWidget *reset_button;
GtkWidget *start_button;
GtkWidget *stop_button;
GtkWidget *resume_button;
GtkWidget *box;

pthread_t chrono;

void init_gtk(int *argc, char ***argv);
void create_window();
void *chronoVoid();

char* default_text = "00:00:00,0";

int main(int argc, char *argv[]) 
{
    init_gtk(&argc, &argv);
    create_window();
    gtk_main();
    return 0;
}

void *chronoVoid()
{
    struct timespec pause_time, start_time, current_time, difference_current_time, stop_current_time;
    double elapsed_time, total_elapsed_time = 0;
    int chronoInitialized = 0, paused_time_initialized = 0;
    started = 1;

    while (started)
    {
        pthread_testcancel();//Sans cette ligne besoin de cliquer 2x sur reset pour que la thread se stop

        if(!chronoInitialized)
        {
            // Obtenir l'heure de départ
            clock_gettime(CLOCK_REALTIME, &start_time);
            chronoInitialized = 1;
        }

        if(paused && !paused_time_initialized)
        {
            // Obtenir le temps de pause
            clock_gettime(CLOCK_REALTIME, &pause_time);
            paused_time_initialized = 1;
        }

        if(!paused && paused_time_initialized)
        {
            // Ajouter le temps de pause au temps total écoulé
            clock_gettime(CLOCK_REALTIME, &stop_current_time);
            total_elapsed_time += (stop_current_time.tv_sec - pause_time.tv_sec) * 1000.0
                + (stop_current_time.tv_nsec - pause_time.tv_nsec) / 1000000.0;
            paused_time_initialized = 0;
        }

        if(chronoInitialized && !paused)
        {
            // Obtenir l'heure actuelle
            clock_gettime(CLOCK_REALTIME, &current_time);

            // Calculer le temps écoulé en millisecondes
            elapsed_time = ((current_time.tv_sec - start_time.tv_sec) * 1000.0
                + (current_time.tv_nsec - start_time.tv_nsec) / 1000000.0) - total_elapsed_time;

            timeInSecond = (double) elapsed_time / 1000;

            hours = timeInSecond / 3600;
            minutes = (long)(timeInSecond / 60) % 60;
            double seconds_decimal = fmod(timeInSecond, 60.0);
            secondes = (long) round(seconds_decimal);
            long milliseconds = (long) round(fmod(seconds_decimal, 1) * 100);

            char texte[50];
            sprintf(texte, "%02ld:%02ld:%04.1f", hours, minutes, seconds_decimal);
            gtk_label_set_text_with_mnemonic(GTK_LABEL(label), texte);
            //usleep(500000);

            usleep(100000);
        }
    }
}

void reset_time()
{
    hours = 0;
    minutes = 0;
    secondes = 0;
    timeInSecond = 0;
}

void on_started_clicked(GtkWidget *widget, gpointer data) 
{
    paused = 0;
    gtk_widget_hide(start_button); 
    gtk_widget_hide(resume_button); 
    gtk_widget_show(stop_button);
    gtk_widget_show(reset_button);
    pthread_create(&chrono, NULL, chronoVoid, NULL);
}

void on_stop_clicked(GtkWidget *widget, gpointer data) 
{
    if(started)
    {
        gtk_widget_show(resume_button); 
        gtk_widget_hide(stop_button);
        paused = 1;
    }
}

void on_resume_clicked(GtkWidget *widget, gpointer data) 
{
    if(paused)
    {
        gtk_widget_show(stop_button);
        gtk_widget_hide(resume_button);
        paused = 0;
    }
}

void on_reset_clicked(GtkWidget *widget, gpointer data)
{
    if(started == 1 && pthread_cancel(chrono) == 0)
    {
        started = 0;
        paused = 0;
        reset_time();
        gtk_label_set_text(GTK_LABEL(label), default_text);
        gtk_widget_show(start_button); 
        gtk_widget_hide(stop_button); 
        gtk_widget_hide(reset_button);
        gtk_widget_hide(resume_button);
    }
}

void init_gtk(int *argc, char ***argv)
{
    gtk_init(argc, argv);
}

void on_window_destroy()
{ 
    gtk_main_quit();
}

void create_window()
{
    // Création de la fenêtre
    GtkWidget *window;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Chronometer");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    // Connecter la fonction on_window_destroy au signal destroy de la fenêtre
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(on_window_destroy), NULL);

    // Création de la boîte horizontale pour les boutons
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *vertical = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(vertical), box2);
    gtk_container_add(GTK_CONTAINER(vertical), box);
    gtk_container_add(GTK_CONTAINER(window), vertical);

    // Création du bouton start
    start_button = gtk_button_new_with_label("Start");
    g_signal_connect(start_button, "clicked", G_CALLBACK(on_started_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box), start_button, TRUE, TRUE, 0);

    // Création du bouton restart
    resume_button = gtk_button_new_with_label("Resume");
    g_signal_connect(resume_button, "clicked", G_CALLBACK(on_resume_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box), resume_button, TRUE, TRUE, 0);

    // Création du bouton stop
    stop_button = gtk_button_new_with_label("Stop");
    g_signal_connect(stop_button, "clicked", G_CALLBACK(on_stop_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box), stop_button, TRUE, TRUE, 0);

    // Création du bouton reset
    reset_button = gtk_button_new_with_label("Reset");
    g_signal_connect(reset_button, "clicked", G_CALLBACK(on_reset_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(box), reset_button, TRUE, TRUE, 0);

    // Création du label
    label = gtk_label_new(default_text);
    // Définition du CSS
    const char* css = "label { font-size: 30pt; }";
    GtkCssProvider* provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    // Application du CSS au label
    GtkStyleContext* context = gtk_widget_get_style_context(label);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    // Ajout du label à la fenêtre
    gtk_box_pack_start(GTK_BOX(box2), label, TRUE, TRUE, 0);
    // Libération de la mémoire
    g_object_unref(provider);


    // Affichage de la fenêtre et de son contenu
    gtk_widget_show_all(window);

    gtk_widget_hide(stop_button);
    gtk_widget_hide(resume_button);
    gtk_widget_hide(reset_button);
}
