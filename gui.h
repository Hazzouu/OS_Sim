#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "process.h"
#include "queue.h"

typedef struct {
    GtkWidget *window;
    GtkWidget *overview_label;
    GtkListStore *process_store;
    GtkWidget *ready_queue_label;
    GtkWidget *blocking_queue_label;
    GtkWidget *running_process_label;
    GtkWidget *current_instruction_label;
    GtkWidget *add_process_button;
    GtkWidget *arrival_entry;
    GtkWidget *algo_combo;
    GtkWidget *quantum_entry;
    GtkWidget *start_simulation_button;
    GtkWidget *start_button;
    GtkWidget *stop_button;
    GtkWidget *reset_button;
    GtkWidget *step_button;
    GtkWidget *mutex_status_label;
    GtkWidget *blocked_input_label;
    GtkWidget *blocked_output_label;
    GtkWidget *blocked_file_label;
    GtkWidget *memory_view_label;
    GtkWidget *log_text_view;
    int clock_cycle;
    gboolean running;
} AppWidgets;

extern PCB* pcbs_list[50];
extern char* filepaths[50];
extern int programs;

AppWidgets* init_gui(void);
char* get_gui_input(int pid);
void on_add_process_clicked(GtkButton *button, AppWidgets *app);
void on_start_clicked(GtkButton *button, AppWidgets *app);
void on_start_simulation_clicked(GtkButton *button, AppWidgets *app);
void on_stop_clicked(GtkButton *button, AppWidgets *app);
void on_reset_clicked(GtkButton *button, AppWidgets *app);
void on_step_clicked(GtkButton *button, AppWidgets *app);
gboolean update_simulation(gpointer data);
void update_gui(AppWidgets *app);
void update_memory_view(AppWidgets *app);
void update_ready_queue_label(AppWidgets *app);
void update_running_and_blocked_labels(AppWidgets *app);
void update_mutex_status_label(AppWidgets *app);
void update_blocked_labels(AppWidgets *app);
void update_current_instruction_label(AppWidgets *app);

#endif