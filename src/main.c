#include <gtk/gtk.h>
#include <string.h>
#include "bt-5.0.0/inc/btree.h"
#include "dctnr.h"

gchar curWord[WORD_MAX];
BTA * data = NULL;
GtkWidget   *window;
GObject     *meanLabel;
GObject     *searchEntry;
GObject     *statusLabel;
GtkListStore    *listStore;

static void destroy_dialog(GtkWidget * widget, gpointer dialog);
static void show_about_dialog(GtkWidget *widget, gpointer none);
static void show_add_dialog(GtkWidget *widget, gpointer none);
static void show_delete_dialog(GtkWidget *widget, gpointer none);
    static void button_delete_clicked(GtkWidget * widget, gpointer dialog);
static void show_edit_dialog(GtkWidget *widget, gpointer none);
static void search_entry_activate(GtkEntry *entry, gpointer none);
static void edit_or_add(GtkWidget *widget, gpointer none);
    static void addit_a_word(GtkWidget *widget, gpointer * none);
static void press_a_key(GtkWidget * entry, GdkEvent * event, gpointer No_need);
    static void show_completion(char *, int );

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
	g_signal_connect(searchEntry, "key-press-event", G_CALLBACK(press_a_key), NULL);
    g_signal_connect(searchEntry, "activate", G_CALLBACK(search_entry_activate), NULL);

    //Completion
    GtkEntryCompletion *comple;
    comple = gtk_entry_completion_new();
	gtk_entry_completion_set_text_column(comple, 0);
	listStore = gtk_list_store_new(1,  G_TYPE_STRING);

	gtk_entry_completion_set_model(comple, GTK_TREE_MODEL(listStore));
	gtk_entry_set_completion(GTK_ENTRY(searchEntry), comple);


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

//CALLBACK func
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

static void show_add_dialog(GtkWidget *widget, gpointer none)
{
    /** Check existed word*/
    g_stpcpy (curWord, gtk_entry_get_text(searchEntry));

    if (existed_word(data, curWord)) gtk_label_set_label(GTK_LABEL(statusLabel), "Từ đã tồn tại");
    else edit_or_add(NULL, NULL);
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

    //OK Button
    GObject     *OKButton;
    OKButton = gtk_builder_get_object(builder, "ok_del_button");
    g_signal_connect (OKButton, "clicked", G_CALLBACK (button_delete_clicked), deleteDialog);

    //Cancel Button
    GObject     *CancelButton;
    CancelButton = gtk_builder_get_object(builder, "cancel_del_button");
    g_signal_connect (CancelButton, "clicked", G_CALLBACK (destroy_dialog), deleteDialog);

    //Delete Message
    GObject     *DeleteLabel;
    DeleteLabel = gtk_builder_get_object(builder, "delete_label");
    char delText[255] = "Do you want to delete: ";
    strcat(delText, curWord);
    strcat(delText, " ?");
    gtk_label_set_text(GTK_LABEL(DeleteLabel), delText);

    //life circyle
    gtk_dialog_run(deleteDialog);
    gtk_widget_destroy (deleteDialog);
}

static void show_edit_dialog(GtkWidget *widget, gpointer none)
{
     /** Check existed word*/
    g_stpcpy (curWord, gtk_entry_get_text(searchEntry));

    if (!existed_word(data, curWord)) gtk_label_set_label(GTK_LABEL(statusLabel), "Từ không tồn tại");
    else edit_or_add(NULL, NULL);
}

static void search_entry_activate(GtkEntry *entry, gpointer none)
{
    char mean[MEAN_MAX];
    g_stpcpy (curWord, gtk_entry_get_text(entry));
    int found = find_meaning_word(data, curWord, mean);
    if (!found) gtk_label_set_label(GTK_LABEL(meanLabel), "Rất tiếc, từ này hiện không có trong từ điển."
		                                                "\n\nGợi ý:\t-Nhấn tab để tìm từ gần giống từ vừa nhập!"
		                                                "\n\t\t-Hoặc nhấn nút \"Thêm từ\", để bổ sung vào từ điển.");
    else gtk_label_set_label(GTK_LABEL(meanLabel), mean);
}

void press_a_key(GtkWidget * entry, GdkEvent * event, gpointer none) 
{
	GdkEventKey *keyEvent = (GdkEventKey *)event;
	char word[WORD_MAX];

    strcpy(word, gtk_entry_get_text(GTK_ENTRY(entry)));
    int existedWord = existed_word(data, word);
	if (keyEvent->keyval == GDK_KEY_Tab) {
		if (existedWord) 
        {
            find_next_word(data, word);
            gtk_entry_set_text(entry, word);
            search_entry_activate(entry, NULL);
        }
        else 
        {
            strcpy(curWord, word);
            add_a_word(data, curWord, "#");
            find_next_word(data, curWord);
            gtk_entry_set_text(entry, curWord);
            search_entry_activate(entry, NULL);
            delete_meaning_word(data, word);
        }
        gtk_editable_set_position (GTK_EDITABLE(entry), 0);
	} 
    else 
    {
        if (keyEvent->keyval != GDK_KEY_downarrow);
        else if (keyEvent->keyval != GDK_KEY_uparrow);
        else if (keyEvent->keyval != GDK_KEY_BackSpace)
        {
            //Phải khởi tạo biến cho strlen() ở ngoài vì sau khi gán phần tử cuốí cùng của word[] là key_valua thì không có '\0' kí tử rác ở sau làm strlen() không ổn định
            int l = strlen(word);
            word[l] = keyEvent->keyval;
            word[l+1] = '\0';
        } 
        else word[strlen(word)-1] = '\0';
        show_completion(word, existedWord);
    }
}

static void edit_or_add(GtkWidget *widget, gpointer none)
{
    /*Make Dialog*/
    GtkBuilder      *builder;
    GtkWidget       *aeDialog;
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "glade/doc_dialog.glade", NULL);
    aeDialog = GTK_WIDGET(gtk_builder_get_object(builder, "addit_dialog"));
    gtk_window_set_transient_for(GTK_WINDOW(aeDialog), GTK_WINDOW(window));

    GObject     *OKButton;
    GObject     *CancelButton;
    GObject     *WordEntry;
    GObject     *MeanTextView;
    GObject     *AdditStatus;

    OKButton = gtk_builder_get_object(builder, "ok_ae_button");
    CancelButton = gtk_builder_get_object(builder, "cancel_ae_button");
    WordEntry = gtk_builder_get_object(builder, "ae_word_entry");
    MeanTextView = gtk_builder_get_object(builder, "ae_text_view");
    AdditStatus = gtk_builder_get_object(builder, "ae_status_label");

    gpointer     *object[4];
    object[0] = (gpointer) MeanTextView;
    object[1] = (gpointer) AdditStatus;
    object[2] = (gpointer) (aeDialog);

    g_stpcpy (curWord, gtk_entry_get_text(searchEntry));
    int found = existed_word(data, curWord);

    GObject     *textBuffer;
    textBuffer = gtk_text_buffer_new(NULL);

    if (found)
    {
        char mean[MEAN_MAX];
        find_meaning_word(data, curWord, mean);
        gtk_entry_set_text(GTK_ENTRY(WordEntry), curWord) ;
        gtk_text_buffer_set_text(textBuffer, mean, -1);
        gtk_text_view_set_buffer(GTK_TEXT_VIEW(MeanTextView), textBuffer);
        gtk_label_set_label(GTK_LABEL(AdditStatus), "Từ đã tồn tại");
        gtk_button_set_label(GTK_BUTTON(OKButton), "Edit");
    } 
    else 
    {
        gtk_entry_set_text(GTK_ENTRY(WordEntry), curWord) ;
        gtk_label_set_label(GTK_LABEL(AdditStatus), "Từ mới");
        gtk_button_set_label(GTK_BUTTON(OKButton), "Add");
    }
    printf("%d\n", found);
    object[3] = (gpointer) found;

    //g_signal_connect (WordEntry, "key-press-event", G_CALLBACK (), object);
    g_signal_connect (OKButton, "clicked", G_CALLBACK (addit_a_word), object);
    g_signal_connect (CancelButton, "clicked", G_CALLBACK (destroy_dialog), aeDialog);


     //show & destroy
    gtk_dialog_run(aeDialog);
    gtk_widget_destroy (aeDialog);
} 

static void button_delete_clicked(GtkWidget *widget, gpointer dialog)
{
    int deleted = delete_meaning_word(data, curWord);
    char status[255];
    if (deleted==0) strcpy(status, "Xóa thành công: ");
    else strcpy(status, "Không thể xóa: ");
    strcat(status, curWord);
    gtk_label_set_text(GTK_LABEL(statusLabel), status);
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void addit_a_word(GtkWidget *widget, gpointer * object)
{
    /*
    gpointer meanTextView = object[0];
    gpointer statusAdditLabel = object[1];
    gpointer dialog = object[2];
    gpointer isExistedWord = object[3];
    */
    GtkTextIter startIter;
	GtkTextIter endIter;

    gtk_text_buffer_get_start_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(object[0])), &startIter);
    gtk_text_buffer_get_end_iter (gtk_text_view_get_buffer(GTK_TEXT_VIEW(object[0])), &endIter);

    char * mean =  gtk_text_buffer_get_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(object[0])), &startIter, &endIter, FALSE);

    if (isBlank(mean))  gtk_label_set_text(GTK_LABEL(object[1]), "Không được bỏ trống");
    else 
    {
        if ((int)object[3])
        {
            int result = update_a_word(data, curWord, mean);
            if (!result) 
            {
                gtk_label_set_text(GTK_LABEL(statusLabel), "Sửa thành công!");
                gtk_label_set_text(GTK_LABEL(meanLabel), mean);
            }
            else gtk_label_set_text(GTK_LABEL(statusLabel), "Có lỗi bất ngờ xẩy ra!");
            gtk_widget_destroy (object[2]);
        }
        else 
        {
            int result = add_a_word(data, curWord, mean);
            if (!result) 
            {
                gtk_label_set_text(GTK_LABEL(statusLabel), "Thêm thành công!");
                gtk_label_set_text(GTK_LABEL(meanLabel), mean);
            }
            else gtk_label_set_text(GTK_LABEL(statusLabel), "Có lỗi bất ngờ xẩy ra!");
            gtk_widget_destroy (object[2]);
        }
    }
}

int prefix(const char * big, const char * small) 
{
	int small_len = strlen(small);
	int big_len = strlen(big);
	int i;
	if (big_len < small_len)
		return 0;
	for (i = 0; i < small_len; i++)
		if (big[i] != small[i])
			return 0;
	return 1;
}

static void show_completion(char * word, int existed)
{
    GtkTreeIter Iter;
    gtk_list_store_clear(listStore);

    char nextword[WORD_MAX];
    if (existed)
    {
        gtk_list_store_append(listStore, &Iter);
		gtk_list_store_set(listStore, &Iter, 0, word, -1 );
    }
    else add_a_word(data, word, "#");
    strcpy(nextword, word);
    int i;
    for (i = 0; i < 20; i++) 
    {
		find_next_word(data, nextword);
        if (prefix(nextword, word))
        {
            gtk_list_store_append(listStore, &Iter);
            gtk_list_store_set(listStore, &Iter, 0, nextword, -1 );
        }
	}
    if (!existed) delete_meaning_word(data, word);
}