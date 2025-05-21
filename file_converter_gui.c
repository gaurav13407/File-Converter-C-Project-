
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cairo.h>
#include <cairo-pdf.h>
#include <pango/pangocairo.h>
#include <stdio.h>

#define CMD_SIZE 1024
#define MAX 256

// Global widgets
GtkWidget *window;
GtkWidget *main_box;
GtkWidget *stack;
GtkWidget *stack_switcher;
GtkWidget *status_bar;
GtkWidget *input_file_entry;
GtkWidget *output_file_entry;
GtkWidget *content_text_view;
GtkTextBuffer *content_buffer;
GtkWidget *search_entry;
GtkWidget *result_text_view;
GtkTextBuffer *result_buffer;

// Function declarations
void write_log(const char *message);
void show_message(const char *message);
void on_convert_button_clicked(GtkWidget *widget, gpointer data);
void on_browse_input_clicked(GtkWidget *widget, gpointer data);
void on_browse_output_clicked(GtkWidget *widget, gpointer data);
void on_create_file_clicked(GtkWidget *widget, gpointer data);
void on_delete_file_clicked(GtkWidget *widget, gpointer data);
void on_read_file_clicked(GtkWidget *widget, gpointer data);
void on_write_file_clicked(GtkWidget *widget, gpointer data);
void on_modify_file_clicked(GtkWidget *widget, gpointer data);
void on_search_file_clicked(GtkWidget *widget, gpointer data);
void on_view_logs_clicked(GtkWidget *widget, gpointer data);

// Conversion functions
void convert_txt_to_csv(const char *input_file, const char *output_file);
void convert_csv_to_txt(const char *input_file, const char *output_file);
void convert_pdf_to_txt(const char *input_file, const char *output_file);
void convert_txt_to_pdf(const char *input_file, const char *output_file);
void convert_txt_to_html(const char *input_file, const char *output_file);
void convert_html_to_txt(const char *input_file, const char *output_file);
void convert_json_to_txt(const char *input_file, const char *output_file);
void convert_txt_to_json(const char *input_file, const char *output_file);

// File operations
void create_file(const char *filename, const char *content);
void delete_file(const char *filename);
char *read_file(const char *filename);
void write_file(const char *filename, const char *content);
void modify_file(const char *filename, const char *content);
char *search_in_file(const char *filename, const char *search_term);
// Implementation of file operation functions

void create_file(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (file) {
        if (content && strlen(content) > 0) {
            fputs(content, file);
        }
        fclose(file);
        char message[256];
        snprintf(message, sizeof(message), "File '%s' created.", filename);
        show_message(message);
        write_log(message);
    } else {
        show_message("Failed to create file.");
        write_log("Failed to create file.");
    }
}

void delete_file(const char *filename) {
    if (remove(filename) == 0) {
        char message[256];
        snprintf(message, sizeof(message), "File '%s' deleted.", filename);
        show_message(message);
        write_log(message);
    } else {
        show_message("Could not delete file.");
        write_log("Failed to delete file.");
    }
}

char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        show_message("Cannot open file for reading.");
        write_log("Failed to open file for reading.");
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate memory for file content
    char *content = malloc(fsize + 1);
    if (!content) {
        fclose(file);
        show_message("Memory allocation failed.");
        write_log("Memory allocation failed during file read.");
        return NULL;
    }
    
    // Read file content
    size_t bytes_read = fread(content, 1, fsize, file);
    content[bytes_read] = '\0';  // Null terminate the string
    
    fclose(file);
    
    char message[256];
    snprintf(message, sizeof(message), "File '%s' read successfully.", filename);
    show_message(message);
    write_log(message);
    
    return content;
}

void write_file(const char *filename, const char *content) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        show_message("Cannot open file for writing.");
        write_log("Failed to open file for writing.");
        return;
    }
    
    if (content && fputs(content, file) >= 0) {
        char message[256];
        snprintf(message, sizeof(message), "Content written to '%s'.", filename);
        show_message(message);
        write_log(message);
    } else {
        show_message("Error writing to file.");
        write_log("Error writing to file.");
    }
    
    fclose(file);
}

void modify_file(const char *filename, const char *content) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        show_message("Cannot open file for appending.");
        write_log("Failed to open file for appending.");
        return;
    }
    
    if (content && fputs(content, file) >= 0) {
        char message[256];
        snprintf(message, sizeof(message), "Content appended to '%s'.", filename);
        show_message(message);
        write_log(message);
    } else {
        show_message("Error appending to file.");
        write_log("Error appending to file.");
    }
    
    fclose(file);
}

char *search_in_file(const char *filename, const char *search_term) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        show_message("Cannot open file for searching.");
        write_log("Failed to open file for searching.");
        return NULL;
    }
    
    // Allocate buffer for results
    char *results = malloc(4096);  // Start with 4KB buffer
    if (!results) {
        fclose(file);
        show_message("Memory allocation failed.");
        write_log("Memory allocation failed during search.");
        return NULL;
    }
    
    // Initialize results
    results[0] = '\0';
    size_t results_size = 4096;
    size_t results_len = 0;
    
    char line[MAX];
    int line_number = 1;
    int found = 0;
    
    // Buffer for storing a single result line
    char result_line[MAX + 64];  // Line plus some extra for the line number prefix
    
    while (fgets(line, MAX, file)) {
        if (strstr(line, search_term)) {
            found = 1;
            
            // Format the result line
            snprintf(result_line, sizeof(result_line), "Line %d: %s", line_number, line);
            size_t line_len = strlen(result_line);
            
            // Ensure buffer is large enough
            if (results_len + line_len + 1 > results_size) {
                results_size *= 2;
                char *new_results = realloc(results, results_size);
                if (!new_results) {
                    free(results);
                    fclose(file);
                    show_message("Memory allocation failed.");
                    write_log("Memory reallocation failed during search.");
                    return NULL;
                }
                results = new_results;
            }
            
            // Append the line to results
            strcat(results + results_len, result_line);
            results_len += line_len;
        }
        line_number++;
    }
    
    fclose(file);
    
    if (!found) {
        snprintf(results, results_size, "'%s' not found in the file.", search_term);
        write_log("Search term not found in file.");
    } else {
        char message[256];
        snprintf(message, sizeof(message), "Search completed, found matches for '%s'.", search_term);
        show_message(message);
        write_log(message);
    }
    
    return results;
}

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);
    
    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "File Format Conversion System");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Create the main vertical box
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);
    
    // Create a stack to switch between different views
    stack = gtk_stack_new();
    stack_switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(stack_switcher), GTK_STACK(stack));
    gtk_box_pack_start(GTK_BOX(main_box), stack_switcher, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), stack, TRUE, TRUE, 0);
    
    // Status bar at the bottom
    status_bar = gtk_statusbar_new();
    gtk_box_pack_end(GTK_BOX(main_box), status_bar, FALSE, FALSE, 0);
    
    // Create the different pages
    
    // 1. Conversion Page
    GtkWidget *conversion_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_stack_add_titled(GTK_STACK(stack), conversion_page, "convert", "Convert Files");
    
    // File selection section
    GtkWidget *file_frame = gtk_frame_new("File Selection");
    gtk_box_pack_start(GTK_BOX(conversion_page), file_frame, FALSE, FALSE, 0);
    
    GtkWidget *file_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(file_frame), file_box);
    
    // Input file selection
    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(file_box), input_box, FALSE, FALSE, 5);
    
    GtkWidget *input_label = gtk_label_new("Input File:");
    gtk_box_pack_start(GTK_BOX(input_box), input_label, FALSE, FALSE, 5);
    
    input_file_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(input_box), input_file_entry, TRUE, TRUE, 0);
    
    GtkWidget *input_browse = gtk_button_new_with_label("Browse");
    gtk_box_pack_start(GTK_BOX(input_box), input_browse, FALSE, FALSE, 0);
    g_signal_connect(input_browse, "clicked", G_CALLBACK(on_browse_input_clicked), NULL);
    
    // Output file selection
    GtkWidget *output_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(file_box), output_box, FALSE, FALSE, 5);
    
    GtkWidget *output_label = gtk_label_new("Output File:");
    gtk_box_pack_start(GTK_BOX(output_box), output_label, FALSE, FALSE, 5);
    
    output_file_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(output_box), output_file_entry, TRUE, TRUE, 0);
    
    GtkWidget *output_browse = gtk_button_new_with_label("Browse");
    gtk_box_pack_start(GTK_BOX(output_box), output_browse, FALSE, FALSE, 0);
    g_signal_connect(output_browse, "clicked", G_CALLBACK(on_browse_output_clicked), NULL);
    
    // Conversion options
    GtkWidget *conversion_frame = gtk_frame_new("Conversion Options");
    gtk_box_pack_start(GTK_BOX(conversion_page), conversion_frame, FALSE, FALSE, 0);
    
    GtkWidget *conversion_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(conversion_grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(conversion_grid), 10);
    gtk_container_add(GTK_CONTAINER(conversion_frame), conversion_grid);
    
    // Conversion buttons
    GtkWidget *txt_to_csv = gtk_button_new_with_label("TXT to CSV");
    g_signal_connect(txt_to_csv, "clicked", G_CALLBACK(on_convert_button_clicked), (gpointer)1);
    gtk_grid_attach(GTK_GRID(conversion_grid), txt_to_csv, 0, 0, 1, 1);
    
    GtkWidget *csv_to_txt = gtk_button_new_with_label("CSV to TXT");
    g_signal_connect(csv_to_txt, "clicked", G_CALLBACK(on_convert_button_clicked), (gpointer)2);
    gtk_grid_attach(GTK_GRID(conversion_grid), csv_to_txt, 1, 0, 1, 1);
    
    GtkWidget *pdf_to_txt = gtk_button_new_with_label("PDF to TXT");
    g_signal_connect(pdf_to_txt, "clicked", G_CALLBACK(on_convert_button_clicked), (gpointer)3);
    gtk_grid_attach(GTK_GRID(conversion_grid), pdf_to_txt, 2, 0, 1, 1);
    
    GtkWidget *txt_to_pdf = gtk_button_new_with_label("TXT to PDF");
    g_signal_connect(txt_to_pdf, "clicked", G_CALLBACK(on_convert_button_clicked), (gpointer)4);
    gtk_grid_attach(GTK_GRID(conversion_grid), txt_to_pdf, 3, 0, 1, 1);
    
    GtkWidget *txt_to_html = gtk_button_new_with_label("TXT to HTML");
    g_signal_connect(txt_to_html, "clicked", G_CALLBACK(on_convert_button_clicked), (gpointer)5);
    gtk_grid_attach(GTK_GRID(conversion_grid), txt_to_html, 0, 1, 1, 1);
    
    GtkWidget *html_to_txt = gtk_button_new_with_label("HTML to TXT");
    g_signal_connect(html_to_txt, "clicked", G_CALLBACK(on_convert_button_clicked), (gpointer)6);
    gtk_grid_attach(GTK_GRID(conversion_grid), html_to_txt, 1, 1, 1, 1);
    
    GtkWidget *json_to_txt = gtk_button_new_with_label("JSON to TXT");
    g_signal_connect(json_to_txt, "clicked", G_CALLBACK(on_convert_button_clicked), (gpointer)7);
    gtk_grid_attach(GTK_GRID(conversion_grid), json_to_txt, 2, 1, 1, 1);
    
    GtkWidget *txt_to_json = gtk_button_new_with_label("TXT to JSON");
    g_signal_connect(txt_to_json, "clicked", G_CALLBACK(on_convert_button_clicked), (gpointer)8);
    gtk_grid_attach(GTK_GRID(conversion_grid), txt_to_json, 3, 1, 1, 1);
    
    // 2. File Operations Page
    GtkWidget *file_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_stack_add_titled(GTK_STACK(stack), file_page, "files", "File Operations");
    
    // File operations frame
    GtkWidget *operations_frame = gtk_frame_new("File Operations");
    gtk_box_pack_start(GTK_BOX(file_page), operations_frame, FALSE, FALSE, 0);
    
    GtkWidget *operations_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_add(GTK_CONTAINER(operations_frame), operations_box);
    
    // File operation buttons
    GtkWidget *create_button = gtk_button_new_with_label("Create File");
    g_signal_connect(create_button, "clicked", G_CALLBACK(on_create_file_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(operations_box), create_button, TRUE, TRUE, 5);
    
    GtkWidget *delete_button = gtk_button_new_with_label("Delete File");
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_file_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(operations_box), delete_button, TRUE, TRUE, 5);
    
    GtkWidget *read_button = gtk_button_new_with_label("Read File");
    g_signal_connect(read_button, "clicked", G_CALLBACK(on_read_file_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(operations_box), read_button, TRUE, TRUE, 5);
    
    GtkWidget *write_button = gtk_button_new_with_label("Write File");
    g_signal_connect(write_button, "clicked", G_CALLBACK(on_write_file_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(operations_box), write_button, TRUE, TRUE, 5);
    
    GtkWidget *modify_button = gtk_button_new_with_label("Modify File");
    g_signal_connect(modify_button, "clicked", G_CALLBACK(on_modify_file_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(operations_box), modify_button, TRUE, TRUE, 5);
    
    // Content text area
    GtkWidget *text_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(text_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(file_page), text_scroll, TRUE, TRUE, 0);
    
    content_text_view = gtk_text_view_new();
    content_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(content_text_view));
    gtk_container_add(GTK_CONTAINER(text_scroll), content_text_view);
    
    // 3. Search Page
    GtkWidget *search_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_stack_add_titled(GTK_STACK(stack), search_page, "search", "Search in File");
    
    // Search input area
    GtkWidget *search_frame = gtk_frame_new("Search");
    gtk_box_pack_start(GTK_BOX(search_page), search_frame, FALSE, FALSE, 0);
    
    GtkWidget *search_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(search_frame), search_box);
    
    GtkWidget *file_label = gtk_label_new("File:");
    gtk_box_pack_start(GTK_BOX(search_box), file_label, FALSE, FALSE, 5);
    
    GtkWidget *search_file_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(search_box), search_file_entry, TRUE, TRUE, 0);
    
    GtkWidget *search_browse = gtk_button_new_with_label("Browse");
    gtk_box_pack_start(GTK_BOX(search_box), search_browse, FALSE, FALSE, 0);
    g_signal_connect(search_browse, "clicked", G_CALLBACK(on_browse_input_clicked), (gpointer)search_file_entry);
    
    GtkWidget *term_label = gtk_label_new("Search Term:");
    gtk_box_pack_start(GTK_BOX(search_box), term_label, FALSE, FALSE, 5);
    
    search_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(search_box), search_entry, TRUE, TRUE, 0);
    
    GtkWidget *search_button = gtk_button_new_with_label("Search");
    g_signal_connect(search_button, "clicked", G_CALLBACK(on_search_file_clicked), (gpointer)search_file_entry);
    gtk_box_pack_start(GTK_BOX(search_box), search_button, FALSE, FALSE, 0);
    
    // Search results
    GtkWidget *result_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(result_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(search_page), result_scroll, TRUE, TRUE, 0);
    
    result_text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(result_text_view), FALSE);
    result_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(result_text_view));
    gtk_container_add(GTK_CONTAINER(result_scroll), result_text_view);
    
    // 4. Logs Page
    GtkWidget *logs_page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_stack_add_titled(GTK_STACK(stack), logs_page, "logs", "View Logs");
    
    GtkWidget *logs_button = gtk_button_new_with_label("Refresh Logs");
    g_signal_connect(logs_button, "clicked", G_CALLBACK(on_view_logs_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(logs_page), logs_button, FALSE, FALSE, 0);
    
    GtkWidget *logs_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(logs_scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(logs_page), logs_scroll, TRUE, TRUE, 0);
    
    GtkWidget *logs_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(logs_view), FALSE);
    GtkTextBuffer *logs_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(logs_view));
    gtk_container_add(GTK_CONTAINER(logs_scroll), logs_view);
    
    // Set the logs view to also update when the page is selected
    g_signal_connect(logs_page, "map", G_CALLBACK(on_view_logs_clicked), (gpointer)logs_buffer);
    
    // Show all widgets
    gtk_widget_show_all(window);
    
    // Start the GTK main loop
    gtk_main();
    
    return 0;
}

// File dialog handler for input file
void on_browse_input_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;
    
    dialog = gtk_file_chooser_dialog_new("Open File",
                                        GTK_WINDOW(window),
                                        action,
                                        "_Cancel",
                                        GTK_RESPONSE_CANCEL,
                                        "_Open",
                                        GTK_RESPONSE_ACCEPT,
                                        NULL);
    
    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        
        if (data == NULL) {
            gtk_entry_set_text(GTK_ENTRY(input_file_entry), filename);
        } else {
            gtk_entry_set_text(GTK_ENTRY((GtkEntry *)data), filename);
        }
        
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}

// File dialog handler for output file
void on_browse_output_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;
    
    dialog = gtk_file_chooser_dialog_new("Save File",
                                        GTK_WINDOW(window),
                                        action,
                                        "_Cancel",
                                        GTK_RESPONSE_CANCEL,
                                        "_Save",
                                        GTK_RESPONSE_ACCEPT,
                                        NULL);
    
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    
    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        gtk_entry_set_text(GTK_ENTRY(output_file_entry), filename);
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}

// Handle conversion button clicks
void on_convert_button_clicked(GtkWidget *widget, gpointer data) {
    int conversion_type = (int)(long)data;
    const char *input_file = gtk_entry_get_text(GTK_ENTRY(input_file_entry));
    const char *output_file = gtk_entry_get_text(GTK_ENTRY(output_file_entry));
    
    if (strlen(input_file) == 0 || strlen(output_file) == 0) {
        show_message("Please select both input and output files");
        return;
    }
    
    switch (conversion_type) {
        case 1: // TXT to CSV
            convert_txt_to_csv(input_file, output_file);
            break;
        case 2: // CSV to TXT
            convert_csv_to_txt(input_file, output_file);
            break;
        case 3: // PDF to TXT
            convert_pdf_to_txt(input_file, output_file);
            break;
        case 4: // TXT to PDF
            convert_txt_to_pdf(input_file, output_file);
            break;
        case 5: // TXT to HTML
            convert_txt_to_html(input_file, output_file);
            break;
        case 6: // HTML to TXT
            convert_html_to_txt(input_file, output_file);
            break;
        case 7: // JSON to TXT
            convert_json_to_txt(input_file, output_file);
            break;
        case 8: // TXT to JSON
            convert_txt_to_json(input_file, output_file);
            break;
        default:
            show_message("Invalid conversion type");
    }
}

// Create file button handler
void on_create_file_clicked(GtkWidget *widget, gpointer data) {
    const char *filename = gtk_entry_get_text(GTK_ENTRY(input_file_entry));
    
    if (strlen(filename) == 0) {
        show_message("Please enter a filename");
        return;
    }
    
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(content_buffer, &start, &end);
    char *content = gtk_text_buffer_get_text(content_buffer, &start, &end, FALSE);
    
    create_file(filename, content);
    g_free(content);
}

// Delete file button handler
void on_delete_file_clicked(GtkWidget *widget, gpointer data) {
    const char *filename = gtk_entry_get_text(GTK_ENTRY(input_file_entry));
    
    if (strlen(filename) == 0) {
        show_message("Please enter a filename");
        return;
    }
    
    delete_file(filename);
}

// Read file button handler
void on_read_file_clicked(GtkWidget *widget, gpointer data) {
    const char *filename = gtk_entry_get_text(GTK_ENTRY(input_file_entry));
    
    if (strlen(filename) == 0) {
        show_message("Please enter a filename");
        return;
    }
    
    char *content = read_file(filename);
    if (content) {
        gtk_text_buffer_set_text(content_buffer, content, -1);
        free(content);
    }
}

// Write file button handler
void on_write_file_clicked(GtkWidget *widget, gpointer data) {
    const char *filename = gtk_entry_get_text(GTK_ENTRY(input_file_entry));
    
    if (strlen(filename) == 0) {
        show_message("Please enter a filename");
        return;
    }
    
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(content_buffer, &start, &end);
    char *content = gtk_text_buffer_get_text(content_buffer, &start, &end, FALSE);
    
    write_file(filename, content);
    g_free(content);
}

// Modify file button handler
void on_modify_file_clicked(GtkWidget *widget, gpointer data) {
    const char *filename = gtk_entry_get_text(GTK_ENTRY(input_file_entry));
    
    if (strlen(filename) == 0) {
        show_message("Please enter a filename");
        return;
    }
    
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(content_buffer, &start, &end);
    char *content = gtk_text_buffer_get_text(content_buffer, &start, &end, FALSE);
    
    modify_file(filename, content);
    g_free(content);
}

// Search in file button handler
void on_search_file_clicked(GtkWidget *widget, gpointer data) {
    GtkEntry *file_entry = GTK_ENTRY(data);
    const char *filename = gtk_entry_get_text(file_entry);
    const char *search_term = gtk_entry_get_text(GTK_ENTRY(search_entry));
    
    if (strlen(filename) == 0 || strlen(search_term) == 0) {
        show_message("Please enter both filename and search term");
        return;
    }
    
    char *results = search_in_file(filename, search_term);
    if (results) {
        gtk_text_buffer_set_text(result_buffer, results, -1);
        free(results);
    } else {
        gtk_text_buffer_set_text(result_buffer, "No matches found or error reading file", -1);
    }
}

// View logs button handler
void on_view_logs_clicked(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *logs_buffer;
    
    if (data == NULL) {
        // Find the text view in the logs page
        GtkWidget *logs_page = gtk_stack_get_child_by_name(GTK_STACK(stack), "logs");
        GtkWidget *logs_scroll = gtk_container_get_children(GTK_CONTAINER(logs_page))->next->data;
        GtkWidget *logs_view = gtk_bin_get_child(GTK_BIN(logs_scroll));
        logs_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(logs_view));
    } else {
        logs_buffer = GTK_TEXT_BUFFER(data);
    }
    
    // Read logs file and display contents
    char *logs_content = read_file("logs.txt");
    if (logs_content) {
        gtk_text_buffer_set_text(logs_buffer, logs_content, -1);
        free(logs_content);
    } else {
        gtk_text_buffer_set_text(logs_buffer, "No logs found", -1);
    }
}

// Helper function to write to log
void write_log(const char *message) {
    FILE *log = fopen("logs.txt", "a");
    if (log) {
        time_t now = time(NULL);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
        fprintf(log, "[%s] %s\n", timestamp, message);
        fclose(log);
    }
}

// Helper function to show message in status bar
void show_message(const char *message) {
    gtk_statusbar_push(GTK_STATUSBAR(status_bar), 0, message);
    // Auto-remove after 3 seconds
    g_timeout_add_seconds(3, (GSourceFunc)gtk_statusbar_pop, GTK_STATUSBAR(status_bar));
    write_log(message);
}

// Implementation of conversion functions

void convert_txt_to_csv(const char *input_file, const char *output_file) {
    FILE *in, *out;
    char buffer[MAX];
    
    in = fopen(input_file, "r");
    out = fopen(output_file, "w");
    
    if (!in || !out) {
        show_message("File error. Check paths.");
        write_log("Error in TXT to CSV conversion.");
        return;
    }
    
    while (fgets(buffer, MAX, in)) {
        for (int i = 0; buffer[i]; i++) {
            if (buffer[i] == ' ') buffer[i] = ',';
        }
        fprintf(out, "%s", buffer);
    }
    
    fclose(in);
    fclose(out);
    show_message("TXT to CSV conversion complete.");
    write_log("TXT to CSV conversion successful.");
}

void convert_csv_to_txt(const char *input_file, const char *output_file) {
    FILE *in, *out;
    char buffer[MAX];
    
    in = fopen(input_file, "r");
    out = fopen(output_file, "w");
    
    if (!in || !out) {
        show_message("File error. Check paths.");
        write_log("Error in CSV to TXT conversion.");
        return;
    }
    
    while (fgets(buffer, MAX, in)) {
        for (int i = 0; buffer[i]; i++) {
            if (buffer[i] == ',') buffer[i] = ' ';
        }
        fprintf(out, "%s", buffer);
    }
    
    fclose(in);
    fclose(out);
    show_message("CSV to TXT conversion complete.");
    write_log("CSV to TXT conversion successful.");
}

void convert_pdf_to_txt(const char *input_file, const char *output_file) {
    char command[CMD_SIZE];
    
    snprintf(command, sizeof(command), "pdftotext \"%s\" \"%s\"", input_file, output_file);
    
    if (system(command) == 0) {
        show_message("PDF to TXT conversion successful.");
        write_log("PDF to TXT conversion successful.");
    } else {
        show_message("Conversion failed. Make sure `pdftotext` is installed.");
        write_log("PDF to TXT conversion failed.");
    }
}

void convert_txt_to_pdf(const char *input_file, const char *output_file) {
    FILE *file = fopen(input_file, "r");
    if (!file) {
        show_message("Failed to open input file.");
        write_log("TXT to PDF conversion failed: Input file not found.");
        return;
    }

    cairo_surface_t *surface = cairo_pdf_surface_create(output_file, 595, 842); // A4
    cairo_t *cr = cairo_create(surface);

    PangoLayout *layout = pango_cairo_create_layout(cr);
    PangoFontDescription *font = pango_font_description_from_string("Monospace 12");
    pango_layout_set_font_description(layout, font);

    char line[1024];
    int y = 20;

    while (fgets(line, sizeof(line), file)) {
        cairo_move_to(cr, 40, y);
        pango_layout_set_text(layout, line, -1);
        pango_cairo_show_layout(cr, layout);
        y += 18;

        if (y > 800) {
            cairo_show_page(cr);
            y = 20;
        }
    }

    fclose(file);
    pango_font_description_free(font);
    g_object_unref(layout);
    cairo_destroy(cr);
    cairo_surface_destroy(surface);

    show_message("TXT to PDF conversion successful.");
    write_log("TXT to PDF conversion successful.");
}

void convert_txt_to_html(const char *input_file, const char *output_file) {
    FILE *in, *out;
    char buffer[MAX];
    
    in = fopen(input_file, "r");
    out = fopen(output_file, "w");
    
    if (!in || !out) {
        show_message("File error. Check paths.");
        write_log("Error in TXT to HTML conversion.");
        return;
    }
    
    fprintf(out, "<html><body><pre>\n");
    while (fgets(buffer, MAX, in)) {
        fprintf(out, "%s", buffer);
    }
    fprintf(out, "</pre></body></html>\n");
    
    fclose(in);
    fclose(out);
    show_message("TXT to HTML conversion complete.");
    write_log("TXT to HTML conversion successful.");
}

void convert_html_to_txt(const char *input_file, const char *output_file) {
    FILE *in, *out;
    int ch;
    int insideTag = 0;
    
    in = fopen(input_file, "r");
    out = fopen(output_file, "w");
    
    if (!in || !out) {
        show_message("File error. Check paths.");
        write_log("Error in HTML to TXT conversion.");
        return;
    }
    
    while ((ch = fgetc(in)) != EOF) {
        if (ch == '<') {
            insideTag = 1;
        } else if (ch == '>') {
            insideTag = 0;
        } else if (!insideTag) {
            fputc(ch, out);
        }
    }
    
    fclose(in);
    fclose(out);
    show_message("HTML to TXT conversion complete.");
    write_log("HTML to TXT conversion successful.");
}

void convert_json_to_txt(const char *input_file, const char *output_file) {
    FILE *in, *out;
    char buffer[MAX];
    
    in = fopen(input_file, "r");
    out = fopen(output_file, "w");
    
    if (!in || !out) {
        show_message("File error. Check paths.");
        write_log("Error in JSON to TXT conversion.");
        return;
    }
    
    while (fgets(buffer, MAX, in)) {
        fprintf(out, "%s", buffer);
    }
    
    fclose(in);
    fclose(out);
    show_message("JSON to TXT conversion complete.");
    write_log("JSON to TXT conversion successful.");
}

void convert_txt_to_json(const char *input_file, const char *output_file) {
    FILE *in, *out;
    char buffer[MAX];
    
    in = fopen(input_file, "r");
    out = fopen(output_file, "w");
    
    if (!in || !out) {
        show_message("File error. Check paths.");
        write_log("Error in TXT to JSON conversion.");
        return;
    }
    
    fprintf(out, "[\n");
    int first = 1;
    while (fgets(buffer, MAX, in)) {
        buffer[strcspn(buffer, "\n")] = 0;
        if (!first) fprintf(out, ",\n");
        fprintf(out, "  \"%s\"", buffer);
        first = 0;
    }
    fprintf(out, "\n]\n");
    
    fclose(in);
    fclose(out);
    show_message("TXT to JSON conversion complete.");
    write_log("TXT to JSON conversion successful.");
}