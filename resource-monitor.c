#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <utmp.h>

typedef struct memory_linked_list_node{
/*
    A struct that is used to define a node in the memory linked list to store data about memory (phy used total and virtual used total).
    Used in functions related to getting Memory values and printing Memory values.
*/

    float phy_used;
    float phy_total;
    float vir_used;
    float vir_total;

    struct memory_linked_list_node *next;
} memory_node;

typedef struct cpu_linked_list_node{
/*
    A struct that is used to define a node in the cpu linked list to store data about cpu usage (cpu usage percentage).
    Used in functions related to getting CPU Usage Percentage and printing CPU Usage Percentage.
*/

    float cpu_used_perc;

    struct cpu_linked_list_node *next;
} cpu_node;


int digit_check(char *string){
/*
    Returns "integer boolean" (1 if true, 0 if false) whether the char array(string) string consists of only digits (i.e. integer).
    A helper function for the function parse_input, used to check the value provided alongside flags "--samples=" and "--tdelay=".
*/
    
    for(int i = 0; *(string+i) != '\0'; i++){
        if( isdigit(*(string+i)) == 0){
            return 0;
        }
    }
    return 1;
}


float round_two_dec(float num){
/*
    Returns a float corresponding to the value of float num when rounded off to 2 decimal places.
    A helper function for the function print_memory_nodes_graphics.
    Removing the need to import math.h library as it may cause compilation difficulties (additional flags).
*/

    int temp = (int)(num*100.0+0.5); //by the truncating property of float to integer conversion
    float temp_two = (float)temp / 100.0;

    return temp_two;
}


void parse_input(int *system_bool_ptr, int *user_bool_ptr, int *graphics_bool_ptr, int * samples_ptr, int *tdelay_ptr, int argc, char*argv[]){
/*
    Seperately iterates through the array of strings argv of length argc which corresponds to the command line arguments.
    Compares command line arguments with the defined strings for flags, setting the 'integer booleans' (1 if true, 0 if false) appropriately
    which are stored at the address system_bool_ptr, user_bool_ptr, graphics_bool_ptr and the integer values which are stored at the address
    samples_ptr and tdelay_ptr.

    List of defined flags:
    --system
        set "boolean integer" pointed by system_bool_ptr to 1

    --user
        set "boolean integer" pointed by user_bool_ptr to 1


    --graphics
        set "boolean integer" pointed by graphics_bool_ptr to 1


    --samples=N
        set integer pointed by samples_ptr to N


    --tdelay=T
        set integer pointed by tdelay_ptr to T


    Note: 
    - If both system and user flags are raised (both equals to 1), then the output will provide information about both system and user.
    - Similarly if none of system and user flags are raised (both equals to 2 which is default value), then the output will provide information about both system and user.

    Checks if the last 2 arguments respectively consists of digits only (using digit_check helper function), 
    if so, they would be the "positional arguments" in the order of "samples" and "tdelay".

    Note:
    - If only 1 positional argument is provided, then the following would correspond to "samples".
    - If both flag and positional argument corresponding to the same value is provided, the positional argument has higher precedence
    (i.e. ./a.out --samples=15 --tdelay=2 8 3 causes the program to run with 8 samples at the rate of every 3 seconds)

*/

    // --system flag check
    for(int i=1; i<argc; i++){
        if(strcmp("--system", argv[i])==0){
            *system_bool_ptr = 1;
        }
    }

    // --user flag check
    for(int i=1; i<argc; i++){
        if(strcmp("--user", argv[i])==0){
            *user_bool_ptr = 1;
        }
    }

    // Dealing with the case if both or none of system and user flag is raised
    if(*system_bool_ptr==*user_bool_ptr){
        *system_bool_ptr = 1;
        *user_bool_ptr = 1;
    }
    else{
        if(*system_bool_ptr == 2){
            *system_bool_ptr = 0;
        }
        
        else if(*user_bool_ptr == 2){
            *user_bool_ptr = 0;
        }
    }

    // --graphics flag check
    for(int i=1; i<argc; i++){
        if(strcmp("--graphics", argv[i])==0){
            *graphics_bool_ptr = 1;
        }
    }

    // --samples flag check
    for(int i=1; i<argc; i++){
        if(strncmp("--samples=", argv[i], 10)==0){

            char delim[] = "=";
            char* token = strtok(argv[i], delim);
            token = strtok(NULL, delim);

            // Dealing with non integer value passed
            if(digit_check(token) == 1){
                *samples_ptr = (int)strtol(token, NULL, 10);
            }
        }
    }

    // --tdelay flag check
    for(int i=1; i<argc; i++){
        if(strncmp("--tdelay=", argv[i], 9)==0){

            char delim[] = "=";
            char* token = strtok(argv[i], delim);
            token = strtok(NULL, delim);

            // Dealing with non integer value passed
            if(digit_check(token) == 1){
                *tdelay_ptr = (int)strtol(token, NULL, 10);
            }
        }
    }

    // Temporary value holders for positional arguments before passing to samples and tdelay
    int pos_arg_sample;
    int pos_arg_tdelay;

    if(digit_check(argv[argc-1])==1){ // Checks if the last argument is an integer
        pos_arg_sample = (int)strtol(argv[argc-1], NULL, 10); // Store value in temporary value holder

        // Indirectly dealing with the case where only 1 positional argument is passed
        if(digit_check(argv[argc-2])==1){ // Checks if the second last argument is an integer
            
            // Pass values to their appropriate temporary value holder
            pos_arg_tdelay = pos_arg_sample;
            pos_arg_sample = (int)strtol(argv[argc-2], NULL, 10);

            *tdelay_ptr = pos_arg_tdelay;
        }

        *samples_ptr = pos_arg_sample;
    }
}


void print_border(){
/*
    Prints to terminal the appropriate border to divide information output to appropriate sections.
*/

    printf("---------------------------------------\n");
}


void print_overall_header(int samples, int tdelay){
/*
    Prints to terminal the overall header which displays the values of the integers samples and tdelay
*/

    printf("Number of samples: %d -- every %d secs\n", samples, tdelay);
    print_border();
}


void print_memory_header(){
/*
    Prints to terminal the header of the Memory section.
*/

    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
}


memory_node *initialize_memory_nodes(int samples, float phy_total, float vir_total){
/*
    Returns a pointer to memory_node struct corresponding to a non root/head of the linked list.
    Initializes value for floats phy_total and vir_total which was passed to the remaining nodes
    by the root/head of the linked list when calling the function initialize_memory_node.
    Integer samples is used to determine the length of the linked list of memory nodes.

    Note:
    - phy_used is initialized to -1.0 which corresponds to "no value allocated" yet
    - vir_used is initialized to -1.0 which corresponds to "no value allocated" yet
    - possible to predetermine value of Phys. Tot and Virtual Tot because they are constants
    - This function is only to be called by initialize_memory_root
*/

    //base case: returns NULL
    if(samples == 0){
        return NULL;
    }
    
    memory_node *node_ptr = (memory_node *)malloc(sizeof(memory_node));

    // Set phy_total and vir_total accordingly to argument passed in
    // Set phy_used and vir_used as "no value allocated"
    node_ptr->phy_total = phy_total;
    node_ptr->phy_used = -1.0;

    node_ptr->vir_total = vir_total;
    node_ptr->vir_used = -1.0;

    // initalizes remaining memory_nodes
    node_ptr->next = initialize_memory_nodes(samples-1,  phy_total, vir_total);

    return node_ptr;
}


memory_node *initialize_memory_root(int samples){
/*
    Returns a pointer to memory_node struct corresponding to the root/head of the linked list.
    Initializes value for floats phy_total and vir_total which is to be passed to the remaining nodes
    by calling the function initialize_memory_node for the memory node that root/head's next points.
    Integer samples is used to determine the length of the linked list of memory nodes.

    Data and Information collected from sys/sysinfo.h library
    - formula for Phys.Tot = totalram (from sysinfo)
    - formula for Virtual Tot = totalram + totalswap (from sysinfo)

    Note:
    - phy_used is initialized to -1.0 which corresponds to "no value allocated" yet
    - vir_used is initialized to -1.0 which corresponds to "no value allocated" yet
    - possible to predetermine value of Phys. Tot and Virtual Tot because they are constants
*/

    memory_node *ll_root = (memory_node *)malloc(sizeof(memory_node));

    // extract information from sysinfo
    struct sysinfo info;
    sysinfo(&info);

    // formula for Phys.Tot = totalram (from sysinfo)
    float phy_total = (float)info.totalram;
    //formula for Virtual Tot = totalram + totalswap (from sysinfo)
    float vir_total = (float)info.totalswap + phy_total;

    //conversion from kilobytes to gigabytes by division of 1024^3
    ll_root->phy_total = phy_total/1073741824.0;
    ll_root->vir_total = vir_total/1073741824.0;

    // no value allocated yet
    ll_root->phy_used = -1.0;
    ll_root->vir_used = -1.0;

    // initalizes remaining memory_nodes
    ll_root->next = initialize_memory_nodes(samples-1,  ll_root->phy_total, ll_root->vir_total);


    return ll_root;
}


void set_memory_node(memory_node * node, int depth){
/* 
    Traverse to the memory node of distance 'depth' and set the value of phy_used and vir_used of the memory node in that depth.
    
    Data and Information collected from sys/sysinfo.h library.
    - formula for Phys.used = totalram - freeram (from sysinfo)
    - formula for Vir used = totalram + totalswap - freeram - freeswap (from sysinfo)
*/

    if(depth>0){
        // calls the function recursively for the next node in the linkedlist
        set_memory_node(node->next, depth-1);
    }
    else if(depth == 0){
            
        struct sysinfo info;
        sysinfo(&info);

        // convert kilobyte to gigabyte by dividing by 1024^3
        node->phy_used = ((float)info.totalram - (float)info.freeram)/1073741824.0;
        node->vir_used = ((float)info.totalram + (float)info.totalswap - (float)info.freeram - (float)info.freeswap)/1073741824.0;
    }
}


void print_memory_nodes(memory_node * node, int depth){
/*
    Prints all the information in each node until the node in depth 'depth'.
    Follows the format "%.2f GB / %.2f GB -- %.2f GB / %.2f GB\n" where the values are
    in the order of Phys.used, Phys.Tot, Virtual Used, Virtual Tot, accordingly to
    the header of the Memory section

    Note: 
    - Only to be called when graphics flag is not raised (i.e. graphics_bool == 0)
*/

    if(depth==0){
        printf("%.2f GB / %.2f GB -- %.2f GB / %.2f GB\n", node->phy_used, node->phy_total, node->vir_used, node->vir_total);
    }
    else{
        printf("%.2f GB / %.2f GB -- %.2f GB / %.2f GB\n", node->phy_used, node->phy_total, node->vir_used, node->vir_total);
        print_memory_nodes(node->next, depth-1);
    }
}


void print_memory_nodes_graphics(memory_node * node, int depth, float prev){
/*
    Prints all the information in each node until the node in depth 'depth'.
    
    Follows the format "%.2f GB / %.2f GB -- %.2f GB / %.2f GB  |" where the values are
    in the order of Phys.used, Phys.Tot, Virtual Used, Virtual Tot, accordingly to
    the header of the Memory section.

    Additionally, the change in Phys. Used from previous to current sample's value is represented as the following

    ::::::@  total relative negative change
    ######*  total relative positive change
    (each : or # represents a change of 0.01 GB)

    |o    zero=
    |*    zero+
    |@    zero-

    which is to be printed in the same line as "%.2f GB / %.2f GB -- %.2f GB / %.2f GB  |" and
    before " net_change (Phys.Used of current sample)"

    Note:
    - Only to be called when graphics flag is raised (i.e. graphics_bool == 1).
    - When function is called from main, set prev = -1.0 which representing the root/head of linked list output
    (i.e. the net change is always zero= because there is no previous value to compare to)
    - helper function round_two_dec is used for rounding values to work around the truncation when converting float to int

*/
    if(depth==0){

        // To work around the truncating property of float to integer conversion
        float round_prev = round_two_dec(prev);
        float round_used = round_two_dec(node->phy_used);
        float round_change = round_used - round_prev;

        float net_change = node->phy_used - prev;

        printf("%.2f GB / %.2f GB -- %.2f GB / %.2f GB   |", node->phy_used, node->phy_total, node->vir_used, node->vir_total);

        // test if net_change is positive, negative or zero
        // prev == -1.0 denotes the first row, representing the root/head of linked list output
        if(prev == -1.0){
            printf("o 0.00 (%.2f)\n",node->phy_used);
        }
        else if(net_change == 0.0){
            printf("o %.2f (%.2f)\n", net_change, node->phy_used);
        }
        else if(net_change < 0.0){
            
            round_change = -1.0 * round_change;
            // 0.1 added to work around floating point error when converting to int
            int count = (int)(round_change*100+0.1);

            // prevent printing -0.00 as net change
            if(round_change == 0.0){
                round_change = 0.00;
            }

            for(int i = 1; i <= count; i++){
                printf(":");
            }

            printf("@ %.2f (%.2f)\n", round_change, node->phy_used);
        }
        else{

            // 0.1 added to work around floating point error when converting to int
            int count = (int)(round_change*100+0.1);

            for(int i = 1; i <= count; i++){
                printf("#");
            }

            printf("* %.2f (%.2f)\n", round_change, node->phy_used);
        }
    }
    else{

        // To work around the truncating property of float to integer conversion
        float round_prev = round_two_dec(prev);
        float round_used = round_two_dec(node->phy_used);
        float round_change = round_used - round_prev;
        float net_change = node->phy_used - prev;

        printf("%.2f GB / %.2f GB -- %.2f GB / %.2f GB   |", node->phy_used, node->phy_total, node->vir_used, node->vir_total);

        // test if net_change is positive, negative or zero
        // prev == -1.0 denotes the first row, representing the root/head of linked list output
        if(prev == -1.0){
            printf("o 0.00 (%.2f)\n",node->phy_used);
        }
        else if(net_change == 0.0){
            printf("o %.2f (%.2f)\n", net_change, node->phy_used);
        }
        else if(net_change < 0.0){
            
            round_change = -1.0 * round_change;
            // 0.1 added to work around floating point error when converting to int
            int count = (int)(round_change*100+0.1);

            // prevent printing -0.00 as net change
            if(round_change == 0.0){
                round_change = 0.00;
            }

            for(int i = 1; i <= count; i++){
                printf(":");
            }

            printf("@ %.2f (%.2f)\n", round_change, node->phy_used);
        }
        else{
            
            // 0.1 added to work around floating point error when converting to int
            int count = (int)(round_change*100+0.1);

            for(int i = 1; i <= count; i++){
                printf("#");
            }

            printf("* %.2f (%.2f)\n", round_change, node->phy_used);
        }

        // recursively call the print function for the next node in the linked list
        print_memory_nodes_graphics(node->next, depth-1, node->phy_used);
    }
}


void free_memory_nodes(memory_node * root){
/*
    Delete (free) memory nodes in the linked list.

    Note:
    - We need to free the nodes as we previously used malloc when intializing the linked list
    - To be called in main after all information is shown and right before program terminates
*/

    memory_node *curr = NULL;
    memory_node *next = NULL;

    curr = root;
    while(curr!=NULL){
        next = curr->next;
        free(curr);
        curr = next;
    }

}


void print_cpu_header_info(float perc){
/*
    Prints to terminal the header of the CPU section for the final iteration.

    note:
    - CPU Usage header includes dispaying the final cpu usage percentage
*/
    printf("### CPU Usage ###\n total cpu use =  %.2f %%\n", perc);
}


void print_cpu_header_blank(){
/*
    Prints to terminal the header of the CPU section for the remaining non final iterations.

    note:
    - Provide space for perc to be displayed by the print_cpu_header_info function
*/
    printf("### CPU Usage ###\n total cpu use =\n");
}


cpu_node *initialize_cpu_nodes(int samples){
/*
    Returns a pointer to cpu_node struct corresponding to a non root/head of the linked list.
    Integer samples is used to determine the length of the linked list of memory nodes.

    Note:
    - cpu_used_perc is initialized to -1.0 which corresponds to "no value allocated" yet
    - This function is only to be called by initialize_cpu_root
*/

    //base case: samples == 0
    if(samples == 0){
        return NULL;
    }
    cpu_node *node_ptr = (cpu_node *)malloc(sizeof(cpu_node));
    node_ptr->cpu_used_perc = -1.0;

    // initializes remaining cpu nodes
    node_ptr->next = initialize_cpu_nodes(samples-1);

    return node_ptr;
}


cpu_node *initialize_cpu_root(int samples){
/*
    Returns a pointer to cpu_node struct corresponding to the root/head of the linked list.
    Calls the function initialize_cpu_node for the remaining cpu nodes of the linked list.
    Integer samples is used to determine the length of the linked list of memory nodes.

    Note:
    - cpu_used_perc is initialized to -1.0 which corresponds to "no value allocated" yet
*/

    cpu_node *ll_root = (cpu_node *)malloc(sizeof(cpu_node));
    ll_root->cpu_used_perc = -1.0;

    // initializes remaining cpu nodes in the linked list
    ll_root->next = initialize_cpu_nodes(samples-1);

    return ll_root;

}


void set_cpu_node(cpu_node * node, int depth, float* perc){
/* 
    Traverse to the cpu node of distance 'depth' and set the value of cpu_used_perc of the cpu node in that depth.
    Set value of integer at address of pointer perc to the value of cpu_used_perc which will be used when printing the
    header of the section in the final iteration of the output.
    
    Data and Information read from /proc/stat file.
    - formula for cpu total = sum of all the values in the first row
    (note that the first row aggregates all the values in the individual CPU entries below)
    - formula for CPU Percentage = 100.0 - (idle*100.0)/(cpu_total)
    (note that idle is the value represented by the fourth column)
*/


    if(depth>0){
        // traverse until the depth is reach
        set_cpu_node(node->next, depth-1, perc);
    }
    else if(depth == 0){
        char line[256];
        char delim[] = " ";

        FILE * file = fopen("/proc/stat", "r");
        fgets(line, sizeof(line), file);

        // read over first token which is "cpu"
        char* token = strtok(line, delim);

        float cpu_total = 0;
        float idle;

        // cpu total = sum of all the values in the first row (ignoring the first string that says "cpu")
        for(int i = 0; i<8; i++){
            token = strtok(NULL, delim);
            
            // idle is the value represented by the fourth column
            if(i == 3){
                idle = (float)strtol(token, NULL, 10);
            }

            cpu_total += (float)strtol(token, NULL, 10);
        }

        //set value for cpu_used_perc and *perc
        node->cpu_used_perc = 100.0 - (idle*100/cpu_total);
        *perc = 100.0 - (idle*100/cpu_total);

        fclose(file);
    }
}


void print_cpu_nodes(cpu_node * node, int depth){
/*
    Prints all the information in each node until the node in depth 'depth'.
    
    The first 3 bars "         |||" represents the base value of the cpu usage percentage (0 %)
    The remaining bars '|' each represent 1% of cpu usage percentage
    Example: 6.5% would have "         |||||||||" which is 3 + 6 bars.

    The corresponding cpu usage percentage would be printed beside the bars in each line.
    Example: "         |||||||| 5.67 %"

    Note:
    - Only to be called when graphics flag is raised (i.e. graphics_bool == 1).

*/
    if(depth==0){
        printf("         |||");

        for(int i = 0; i<(int)node->cpu_used_perc; i++){
            printf("|");
        }

        printf(" %.2f %%\n", node->cpu_used_perc);
    }
    else{
        printf("         |||"); 

        for(int i = 0; i<(int)node->cpu_used_perc; i++){
            printf("|");
        }

        printf(" %.2f %%\n", node->cpu_used_perc);
        print_cpu_nodes(node->next, depth-1);
    }
}


void free_cpu_nodes(cpu_node * root){
/*
    Delete (free) cpu nodes in the linked list.

    Note:
    - We need to free the nodes as we previously used malloc when intializing the linked list
    - To be called in main after all information is shown and right before program terminates
*/

    cpu_node *curr = NULL;
    cpu_node *next = NULL;

    curr = root;
    while(curr!=NULL){
        next = curr->next;
        free(curr);
        curr = next;
    }

}


void print_session_user_header(){
/*
    Prints to terminal the header of the Sessions/users section.
*/
    printf("### Sessions/users ###\n");
}


void print_session_user(){
/*
    Prints to terminal all the users logged in the current machine.

    Data and information extracted from the utmp.h library
    - username(ut_user)     - session(ut_line)     - IP(ut_host)

    output follows the format "username session (IP)"
*/

    struct utmp* info;
    setutent();
    info = getutent();

    // similar to while not End of File
    while(info != NULL){

        // check if the user type is of Normal Process
        if(info->ut_type == USER_PROCESS){
            printf(" %-14s%-7s(%s)\n", info->ut_user, info->ut_line, info->ut_host);
        }

        // reads next utmp struct
        info = getutent();

    }
    print_border();
}


void print_sysinfo_header(){
/*
    Prints to terminal the header of the System Information section.
*/
    printf("### System Information ###\n");
}


void print_sysinfo(){
/*
    Prints to terminal information about the current machine.

    Data and information extracted from the sys/utsname.h library
    - System Name (sysname)
    - Machine Name (nodename)
    - Version (version)
    - Release (release)
    - Architecture (machine)

    output follows the order as above, each seperated by a newline
*/
    
    // extract struct utsname using uname
    struct utsname info;
    uname(&info);

    printf(" System Name = %s\n", info.sysname);
    printf(" Machine Name = %s\n", info.nodename);
    printf(" Version = %s\n", info.version);
    printf(" Release = %s\n", info.release);
    printf(" Architecture = %s\n", info.machine);

    print_border();
}

void display_output(int system_bool, int user_bool, int graphics_bool, int samples, int tdelay){
/*
    Print information to terminal accordingly to the values in "integer booleans" system_bool, user_bool, graphics_bool
    samples amount of recorded data outputted with interval of tdelay seconds.

    Clear screen after every iteration and reprint information to provide the illusion of outputting information at 2 points simultaneously

    CPU node and Memory node linked lists to be used are initialized within this function and also freed before this function finishes execution
*/

    // initialize memory node and cpu node linked lists
    memory_node* memory_root = initialize_memory_root(samples);
    cpu_node* cpu_root = initialize_cpu_root(samples);
    
    // initialize float perc to store cpu_usage_perc
    float perc;

    for(int i = 0; i<samples; i++){

        // clear the screen using escape codes
        printf("\033c");
        
        print_overall_header(samples, tdelay);
        
        if(system_bool == 1){

            print_memory_header();

            set_memory_node(memory_root, i);

            if(graphics_bool == 1){
                // Note that the prev argument should be set to -1.0 because we are calling it for the first sample and the rest is done recursively
                print_memory_nodes_graphics(memory_root, i, -1.0);
            }
            else{
                print_memory_nodes(memory_root, i);
            }

            // Cover remaining lines (samples - i lines) with \n (newline)
            for(int s = i; s<samples-1;s++){
            printf("\n");
            }

            print_border();
        }

        if(user_bool == 1){
            print_session_user_header();
            print_session_user();
        }

        if(system_bool == 1){
            // note that address of perc is passed as an argument to store the value of cpu usage percentage for that sample
            set_cpu_node(cpu_root, i, &perc);
            
            // if it is already the final iteration, display cpu usage percentage stored in perc alongside header
            if(i == samples - 1){
                print_cpu_header_info(perc);
            }
            else{
                print_cpu_header_blank();
            }
            
            if(graphics_bool== 1){
                print_cpu_nodes(cpu_root, i);

                // Cover remaining lines (samples - i lines) with \n (newline)
                for(int s = i; s<samples-1;s++){
                    printf("\n");
                }
            }

            print_border();
        }

        // Print system information in the final iteration
        if(i == samples - 1){
            print_sysinfo_header();
            print_sysinfo();
        }

        // pause for tdelay amount of seconds
        sleep(tdelay);
        
    }

    // free memory allocated for the 2 linked lists
    free_memory_nodes(memory_root);
    free_cpu_nodes(cpu_root);

}

int main( int argc, char *argv[] )  {

    // integer boolean value of 2 indicates that the boolean has not been altered
    int system_bool = 2;
    int user_bool = 2;
    
    // set accordingly to default values defined
    int graphics_bool = 0;
    int samples = 10;
    int tdelay = 1;

    // set values defined above accordingly to the arguments passed
    parse_input(&system_bool, &user_bool, &graphics_bool, &samples, &tdelay, argc, argv);

    // print information to terminal accordingly
    display_output(system_bool, user_bool, graphics_bool, samples, tdelay);   
}