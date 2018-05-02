#include <gtk/gtk.h>
#include <string.h>
#include "bt-5.0.0/inc/btree.h"
#include "dctnr.h"

gchar curWord[255];
BTA * data = NULL;
GtkWidget   *window;
GObject     *meanLabel;
GObject     *searchEntry;
GObject     *statusLabel;


static void print_hello (GtkWidget *widget, gpointer data)
{
    g_print ("Hello World\n");
} 

static void destroy_dialog(GtkWidget *widget, gpointer dialog)
{
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void show_about_dialog(GtkWidget *widget, gpointer none)
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

static void edit_a_word(GtkWidget *widget, gpointer *object)
{
    GtkTextIter startIter;
	GtkTextIter endIter;

    gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(object[0])), &startIter);
    gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(object[0])), &endIter);

    char * mean =  gtk_text_buffer_get_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(object[0])), &startIter, &endIter, FALSE);
    int i=0, l=strlen(mean), blank=1;
    while (i<l)
    {
        if (!((mean[i]==' ') || (mean[i]=='\n'))) { blank=0; break;}
        else i++;
    }

    if (blank)  gtk_label_set_text(GTK_LABEL(object[1]), "Không được bỏ trống");
    else 
    {
        int result = btupd(data, curWord, mean, strlen(mean) + 1);
        if (!result) 
        {
            gtk_label_set_text(GTK_LABEL(statusLabel), "Sửa thành công!");
            gtk_label_set_text(GTK_LABEL(meanLabel), mean);
        }
        else gtk_label_set_text(GTK_LABEL(statusLabel), "Có lỗi bất ngờ xẩy ra!");
        gtk_widget_destroy (object[2]);
    }

    
}

static void show_edit_dialog(GtkWidget *widget, gpointer none)
{
     /** Make About_Dialog*/
    GtkBuilder      *builder;
    GtkWidget       *editDialog;
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/doc_dialog.glade", NULL);
    editDialog = GTK_WIDGET(gtk_builder_get_object(builder, "edit_dialog"));
    gtk_window_set_transient_for(GTK_WINDOW(editDialog), GTK_WINDOW(window));
    
    g_stpcpy (curWord, gtk_entry_get_text(searchEntry));

    int rsize;
    if (bfndky(data, curWord, &rsize)!=0) 
    {
        char status[255] = "Không tìm thấy: ";
        strcat(status, curWord);
        gtk_label_set_text(statusLabel, status);
        gtk_widget_destroy (editDialog);
        return;
    }

    GObject     *OKButton;
    GObject     *CancelButton;
    GObject     *WordEntry;
    GObject     *MeanTextView;
    GObject     *EditStatus;

    OKButton = gtk_builder_get_object(builder, "ok_edit_button");
    CancelButton = gtk_builder_get_object(builder, "cancel_edit_button");
    WordEntry = gtk_builder_get_object(builder, "edit_word_entry");
    MeanTextView = gtk_builder_get_object(builder, "edit_text_view");
    EditStatus = gtk_builder_get_object(builder, "edit_status_label");

    GObject     *object_use[3];
    object_use[0] = MeanTextView;
    object_use[1] = EditStatus;
    object_use[2] = G_OBJECT(editDialog);

    char mean[MEAN_MAX];
    int r;
    btsel(data, curWord, mean, MEAN_MAX, &r);
    GObject     *textBuffer;
    textBuffer = gtk_text_buffer_new(NULL);
    gtk_text_buffer_set_text(textBuffer, mean, -1);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(MeanTextView), textBuffer);
    g_signal_connect (OKButton, "clicked", G_CALLBACK (edit_a_word), object_use);
    g_signal_connect (CancelButton, "clicked", G_CALLBACK (destroy_dialog), editDialog);
    gtk_entry_set_text (GTK_ENTRY(WordEntry), curWord);

    //show & destroy
    gtk_dialog_run(editDialog);
    gtk_widget_destroy (editDialog);
}

static void delete_a_word(GtkWidget *widget, gpointer dialog)
{
    int result = btdel(data, curWord);
    char status[255];
    if (!result) strcpy(status, "Xóa thành công: ");
    else strcpy(status, "Không thể xóa: ");
    strcat(status, curWord);
    gtk_label_set_text(GTK_LABEL(statusLabel), status);
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void show_delete_dialog(GtkWidget *widget, gpointer none)
{
     /** Make About_Dialog*/
    GtkBuilder      *builder;
    GtkWidget       *deleteDialog;
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/doc_dialog.glade", NULL);
    deleteDialog = GTK_WIDGET(gtk_builder_get_object(builder, "del_dialog"));
    gtk_window_set_transient_for(GTK_WINDOW(deleteDialog), GTK_WINDOW(window));

    g_stpcpy (curWord, gtk_entry_get_text(searchEntry));
    
    GObject     *OKButton;
    GObject     *CancelButton;
    GObject     *DeleteLabel;

    OKButton = gtk_builder_get_object(builder, "ok_del_button");
    CancelButton = gtk_builder_get_object(builder, "cancel_del_button");
    DeleteLabel = gtk_builder_get_object(builder, "delete_label");
    
    g_signal_connect (OKButton, "clicked", G_CALLBACK (delete_a_word), deleteDialog);
    g_signal_connect (CancelButton, "clicked", G_CALLBACK (destroy_dialog), deleteDialog);
    char delText[255] = "Do you want to delete: ";
    strcat(delText, curWord);
    strcat(delText, "?");
    gtk_label_set_text(GTK_LABEL(DeleteLabel), delText);

    //life circyle
    gtk_dialog_run(deleteDialog);
    gtk_widget_destroy (deleteDialog);
}

static void add_a_word(GtkWidget *widget, gpointer *object)
{
    char * word = gtk_entry_get_text(GTK_ENTRY(object[3]));
    int rsize;
    if (bfndky(data, word, &rsize)==0) 
    {
        char status[255] = "Từ đã tồn tại: ";
        strcat(status, word);
        gtk_label_set_text(GTK_LABEL(object[1]), status);
        return;
    }


    GtkTextIter startIter;
	GtkTextIter endIter;

    gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(object[0])), &startIter);
    gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(object[0])), &endIter);

    char * mean =  gtk_text_buffer_get_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(object[0])), &startIter, &endIter, FALSE);
    int i=0, l=strlen(mean), blank=1;
    while (i<l)
    {
        if (!((mean[i]==' ') || (mean[i]=='\n'))) { blank=0; break;}
        else i++;
    }

    if (blank)  gtk_label_set_text(GTK_LABEL(object[1]), "Không được bỏ trống");
    else 
    {
        int result = btins(data, curWord, mean, strlen(mean) + 1);
        if (!result) 
        {
            gtk_label_set_text(GTK_LABEL(statusLabel), "Thêm thành công!");
            gtk_label_set_text(GTK_LABEL(meanLabel), mean);
        }
        else gtk_label_set_text(GTK_LABEL(statusLabel), "Có lỗi bất ngờ xẩy ra!");
        gtk_widget_destroy (object[2]);
    }
}

static void show_add_dialog(GtkWidget *widget, gpointer none)
{
     /** Make About_Dialog*/
    GtkBuilder      *builder;
    GtkWidget       *addDialog;
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/doc_dialog.glade", NULL);
    addDialog = GTK_WIDGET(gtk_builder_get_object(builder, "add_dialog"));
    gtk_window_set_transient_for(GTK_WINDOW(addDialog), GTK_WINDOW(window));

    g_stpcpy (curWord, gtk_entry_get_text(searchEntry));

    GObject     *OKButton;
    GObject     *CancelButton;
    GObject     *WordEntry;
    GObject     *MeanTextView;
    GObject     *AddStatus;

    OKButton = gtk_builder_get_object(builder, "ok_add_button");
    CancelButton = gtk_builder_get_object(builder, "cancel_add_button");
    WordEntry = gtk_builder_get_object(builder, "add_word_entry");
    MeanTextView = gtk_builder_get_object(builder, "add_text_view");
    AddStatus = gtk_builder_get_object(builder, "add_status_label");

    GObject     *object_use[4];
    object_use[0] = MeanTextView;
    object_use[1] = AddStatus;
    object_use[2] = G_OBJECT(addDialog);
    object_use[3] = WordEntry;

    g_signal_connect (OKButton, "clicked", G_CALLBACK (add_a_word), object_use);
    g_signal_connect (CancelButton, "clicked", G_CALLBACK (destroy_dialog), addDialog);
    gtk_entry_set_text(GTK_ENTRY(WordEntry), curWord);

    //show & destroy
    gtk_dialog_run(addDialog);
    gtk_widget_destroy (addDialog);
}

static void search_entry_activate(GtkEntry *entry, gpointer none)
{
    int rsize = 0;
    char mean[MEAN_MAX];
    g_stpcpy (curWord, gtk_entry_get_text(entry));
    btsel(data, curWord, mean, 5000, &rsize);
    if (rsize == 0)
		gtk_label_set_label(GTK_LABEL(meanLabel), "Rất tiếc, từ này hiện không có trong từ điển."
		                  "\n\nGợi ý:\t-Nhấn tab để tìm từ gần giống từ vừa nhập!"
		                  "\n\t\t-Hoặc nhấn nút \"Thêm từ\", để bổ sung vào từ điển.");
	else
		gtk_label_set_label(GTK_LABEL(meanLabel), mean);
}

static void edit_or_add(GtkWidget *widget, gpointer none)
{
    g_stpcpy (curWord, gtk_entry_get_text(searchEntry));

    int i=0, l=strlen(curWord), blank=1;
    while (i<l)
    {
        if (!((curWord[i]==' '))) {blank=0; break;}
        else i++;
    }

    int rsize;
    if (blank) show_add_dialog(widget, NULL);
    else if (bfndky(data, curWord, &rsize)!=0) show_add_dialog(widget, NULL);
    else show_edit_dialog(widget, NULL);
}

static void key_press(GtkWidget *searchBox, GdkEvent *event, gpointer none)
{
    GdkEventKey *keyEvent = (GdkEventKey*)event;
    g_stpcpy (curWord, gtk_entry_get_text(searchEntry));
    if (keyEvent->keyval == GDK_KEY_Tab) 
    {
        int value;
        if (bfndky(data, curWord, &value)) btins(data, curWord, "", 1); 

        
        bnxtky(data, curWord, &value);
        gtk_entry_set_text (GTK_ENTRY(searchEntry), curWord);
        //gtk_editable_set_position(GTK_EDITABLE(entry), 5);
        gtk_widget_grab_focus(GTK_WIDGET(searchEntry));
    }
}

int main(int argc, char *argv[])
{
    /** Export dictionary File 
     */
    if (argc == 2)    
    {
        convert_text_to_bt(argv[1]);
        g_print("export to Resource.dat\n");
    }

    data = btopn("resource.dat", 0, 1);
/** template-form 
    */
    GtkBuilder      *builder;
    //GtkWidget       *window;
 
    gtk_init(&argc, &argv);
 
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/window_main.glade", NULL);
 
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
    gtk_builder_connect_signals(builder, NULL);
    //end template-form


/** build and signals Object
    */
    //Label 
    //GObject      *meanLabel;
    meanLabel = gtk_builder_get_object(builder, "meanLabel");
    gtk_label_set_label(meanLabel, curWord);

    //edit/Add Button
    GObject     *editButton;
    editButton = gtk_builder_get_object(builder, "editButton");
    g_signal_connect (editButton, "clicked", G_CALLBACK (edit_or_add), NULL);

    //Add Menu Item
    GObject     *addItem;
    addItem = gtk_builder_get_object(builder, "addItem");
    g_signal_connect (addItem, "activate", G_CALLBACK (show_add_dialog), NULL);

    //Delete Menu Item
    GObject     *deleteItem;
    deleteItem = gtk_builder_get_object(builder, "deleteItem");
    g_signal_connect (deleteItem, "activate", G_CALLBACK (show_delete_dialog), NULL);

    //edit Menu Item
    GObject     *editItem;
    editItem = gtk_builder_get_object(builder, "editItem");
    g_signal_connect (editItem, "activate", G_CALLBACK (show_edit_dialog), NULL);

    //About Menu Item
    GObject     *aboutItem;
    aboutItem = gtk_builder_get_object(builder, "aboutItem");
    g_signal_connect (aboutItem, "activate", G_CALLBACK (show_about_dialog), NULL);    

    //Quit Menu Item
    GObject     *quitItem;
    quitItem = gtk_builder_get_object(builder, "quitItem");
    g_signal_connect (quitItem, "activate", G_CALLBACK (gtk_main_quit), NULL);   

    //Search Entry
    //GObject     *searchEntry;
    searchEntry = gtk_builder_get_object(builder, "searchEntry");
	g_signal_connect(searchEntry, "activate", G_CALLBACK(search_entry_activate), NULL);

    //Press Key on Search Entry
    GObject     *searchBox;
    searchBox = gtk_builder_get_object(builder, "search_box");
    g_signal_connect(searchBox, "key-press-event", G_CALLBACK(key_press), NULL);

    //statusLabel
    statusLabel = gtk_builder_get_object(builder, "statusLabel");

// free
    g_object_unref(builder);
 
    gtk_widget_show(window);                
    gtk_main();

    btcls(data);
 
    return 0;
}
 
// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}
