#include <gtk/gtk.h>
#include <string.h>
#include "bt-5.0.0/inc/btree.h"

gchar curWord[50];

static void print_hello (GtkWidget *widget, gpointer data)
{
    g_print ("Hello World\n");
} 

static void show_about_dialog(GtkWidget *widget, gpointer window)
{
    /** Make About_Dialog*/
    GtkBuilder      *builder;
    GtkWidget       *aboutDialog;
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/about_dialog.glade", NULL);
    aboutDialog = GTK_WIDGET(gtk_builder_get_object(builder, "about_dialog"));
    gtk_window_set_transient_for(GTK_WINDOW(aboutDialog), GTK_WINDOW(window));
    gtk_dialog_run(aboutDialog);
    gtk_widget_destroy (aboutDialog);
}

static void show_edit_dialog(GtkWidget *widget, gpointer window)
{
     /** Make About_Dialog*/
    GtkBuilder      *builder;
    GtkWidget       *aboutDialog;
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/doc_dialog.glade", NULL);
    aboutDialog = GTK_WIDGET(gtk_builder_get_object(builder, "edit_dialog"));
    gtk_window_set_transient_for(GTK_WINDOW(aboutDialog), GTK_WINDOW(window));
    gtk_dialog_run(aboutDialog);
    gtk_widget_destroy (aboutDialog);
}

static void show_delete_dialog(GtkWidget *widget, gpointer window)
{
     /** Make About_Dialog*/
    GtkBuilder      *builder;
    GtkWidget       *aboutDialog;
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/doc_dialog.glade", NULL);
    aboutDialog = GTK_WIDGET(gtk_builder_get_object(builder, "del_dialog"));
    gtk_window_set_transient_for(GTK_WINDOW(aboutDialog), GTK_WINDOW(window));
    gtk_dialog_run(aboutDialog);
    gtk_widget_destroy (aboutDialog);
}

static void show_add_dialog(GtkWidget *widget, gpointer window)
{
     /** Make About_Dialog*/
    GtkBuilder      *builder;
    GtkWidget       *aboutDialog;
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/doc_dialog.glade", NULL);
    aboutDialog = GTK_WIDGET(gtk_builder_get_object(builder, "add_dialog"));
    gtk_window_set_transient_for(GTK_WINDOW(aboutDialog), GTK_WINDOW(window));
    gtk_dialog_run(aboutDialog);
    gtk_widget_destroy (aboutDialog);
}

static void search_entry_activate(GtkEntry *entry, gpointer meanLabel)
{
    g_stpcpy (curWord, gtk_entry_get_text(entry));
    gtk_label_set_label(GTK_LABEL(meanLabel), gtk_entry_get_text(entry));
}

/*FALSE
static gboolean key_press(GtkWidget *entry, GdkEvent *event, gpointer data)
{
    GdkEventKey *keyEvent = (GdkEventKey*)event;
    int len;
    strcpy(curWord, gtk_entry_get_text(entry));
    if (keyEvent->keyval == GDK_KEY_Tab) 
    {
        gtk_entry_set_text(entry, "Vinh");
    }
    else
    {
        if (keyEvent->keyval != GDK_KEY_BackSpace) 
        {
            len = strlen(curWord);
            curWord[len] = keyEvent->keyval;
            curWord[len + 1] = '\0';
        }
        else 
        {
            len = strlen(curWord);
            curWord[len - 1] = '\0';
        }
        gtk_entry_set_text(entry, curWord);
    }
    return FALSE;
}*/

int main(int argc, char *argv[])
{
    /** template-form 
    */
    GtkBuilder      *builder;
    GtkWidget       *window;
 
    gtk_init(&argc, &argv);
 
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/window_main.glade", NULL);
 
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);
    //end template-form


    /** build and signals Object
    */
    //Label 
    GObject      *meanLabel;
    meanLabel = gtk_builder_get_object(builder, "meanLabel");
    gtk_label_set_label(meanLabel, curWord);

    //edit/Add Button
    GObject     *editButton;
    editButton = gtk_builder_get_object(builder, "editButton");
    g_signal_connect (editButton, "clicked", G_CALLBACK (show_edit_dialog), window);

    //Add Menu Item
    GObject     *addItem;
    addItem = gtk_builder_get_object(builder, "addItem");
    g_signal_connect (addItem, "activate", G_CALLBACK (show_add_dialog), window);

    //Delete Menu Item
    GObject     *deleteItem;
    deleteItem = gtk_builder_get_object(builder, "deleteItem");
    g_signal_connect (deleteItem, "activate", G_CALLBACK (show_delete_dialog), window);

    //edit Menu Item
    GObject     *editItem;
    editItem = gtk_builder_get_object(builder, "editItem");
    g_signal_connect (editItem, "activate", G_CALLBACK (show_edit_dialog), window);

    //About Menu Item
    GObject     *aboutItem;
    aboutItem = gtk_builder_get_object(builder, "aboutItem");
    g_signal_connect (aboutItem, "activate", G_CALLBACK (show_about_dialog), window);    

    //Quit Menu Item
    GObject     *quitItem;
    quitItem = gtk_builder_get_object(builder, "quitItem");
    g_signal_connect (quitItem, "activate", G_CALLBACK (gtk_main_quit), NULL);   

    //Press Key on Search Entry
    GObject     *searchEntry;
    searchEntry = gtk_builder_get_object(builder, "searchEntry");
    //g_signal_connect(searchEntry, "key-press-event", G_CALLBACK(key_press), NULL);
	g_signal_connect(searchEntry, "activate", G_CALLBACK(search_entry_activate), meanLabel);

    // free
    g_object_unref(builder);
 
    gtk_widget_show(window);                
    gtk_main();
 
    return 0;
}
 
// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}
