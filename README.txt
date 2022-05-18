How to Run the Program
	
	List of defined flags:
	    --system
        	indicates that only the system usage should be generated

	    --user
        	indicates that only the users usage should be generated

	    --graphics
        	includes graphical output in the cases where a graphical outcome
		refer to "how to interpret output for graphics" to understand more

	    --samples=N (where N is an integer)
	        indicate that N number of statistics is going to be collected and displayed
		if not passed, default value is 10

	    --tdelay=T (where T is an integer)
	        indicate the time interval in seconds between each collection and display of statistics
		if not passed, default value is 1


    Additional Note: 
    - If both system and user flags are raised, then the output will provide information about both system and user.
    - Similarly if none of system and user flags are raised, then the output will provide information about both system and user.
    - Undefined flags will be ignored and program will run as if the undefined flag was not passed.


    If the last 2 arguments respectively consists of digits only (integers), 
    then they would be the "positional arguments" in the order of "samples" then "tdelay".

    Additional Note:
    - If only 1 positional argument is provided, then the following would correspond to "samples".
    - If both flag and positional argument corresponding to the same value is provided, the positional argument has higher precedence
    (i.e. ./a.out --samples=15 --tdelay=2 8 3 causes the program to run with 8 samples at the rate of every 3 seconds)




How to Interpret the Output for graphics
	
	For ### Memory ### Section
	
	Change in Phys. Used from previous to current sample's value is represented as the following

	::::::@  total relative negative change
	######*  total relative positive change
	(each : or # represents a change of 0.01 GB)

	|o    zero=
	|*    zero+
	|@    zero-

	To be printed in the same line as "%.2f GB / %.2f GB -- %.2f GB / %.2f GB  |" and
	before " net change (Phys.Used of current sample)\n"



	For ### CPU Usage ### Section

	The first 3 bars "         |||" represents the base value of the cpu usage percentage (0 %)
	The remaining bars '|' each represent 1% of cpu usage percentage
	Example: 6.5% would have "         |||||||||" which is 3 + 6 bars.

	The corresponding cpu usage percentage would be printed beside the bars in each line.
	Example: "         |||||||| 5.67 %"




Known 'Issues'

	- Output may sometimes flash due to clearing screen before every reprint using escape codes
	- Output may sometimes jitter if terminal does not have enough space for all the output
	- Graphical output for cpu usage percentage may sometimes suffer from floating point conversion




Overview of the data structures and functions defined

struct memory_linked_list_node
/*
    A struct that is used to define a node in the memory linked list to store data about memory (phy used total and virtual used total).
    Used in functions related to getting Memory values and printing Memory values.
*/


struct cpu_linked_list_node
/*
    A struct that is used to define a node in the cpu linked list to store data about cpu usage (cpu usage percentage).
    Used in functions related to getting CPU Usage Percentage and printing CPU Usage Percentage.
*/


int digit_check(char *string)
/*
    Returns "integer boolean" (1 if true, 0 if false) whether the char array(string) string consists of only digits (i.e. integer).
    A helper function for the function parse_input, used to check the value provided alongside flags "--samples=" and "--tdelay=".
*/


float round_two_dec(float num)
/*
    Returns a float corresponding to the value of float num when rounded off to 2 decimal places.
    A helper function for the function print_memory_nodes_graphics.
    Removing the need to import math.h library as it may cause compilation difficulties (additional flags).
*/


void parse_input(int *system_bool_ptr, int *user_bool_ptr, int *graphics_bool_ptr, int * samples_ptr, int *tdelay_ptr, int argc, char*argv[])
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


void print_border()
/*
    Prints to terminal the appropriate border to divide information output to appropriate sections.
*/


void print_overall_header(int samples, int tdelay)
/*
    Prints to terminal the overall header which displays the values of the integers samples and tdelay
*/


void print_memory_header()
/*
    Prints to terminal the header of the Memory section.
*/


memory_node *initialize_memory_nodes(int samples, float phy_total, float vir_total)
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


memory_node *initialize_memory_root(int samples)
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


void set_memory_node(memory_node * node, int depth)
/* 
    Traverse to the memory node of distance 'depth' and set the value of phy_used and vir_used of the memory node in that depth.
    
    Data and Information collected from sys/sysinfo.h library.
    - formula for Phys.used = totalram - freeram (from sysinfo)
    - formula for Vir used = totalram + totalswap - freeram - freeswap (from sysinfo)
*/


void print_memory_nodes(memory_node * node, int depth)
/*
    Prints all the information in each node until the node in depth 'depth'.
    Follows the format "%.2f GB / %.2f GB -- %.2f GB / %.2f GB\n" where the values are
    in the order of Phys.used, Phys.Tot, Virtual Used, Virtual Tot, accordingly to
    the header of the Memory section

    Note: 
    - Only to be called when graphics flag is not raised (i.e. graphics_bool == 0)
*/


void print_memory_nodes_graphics(memory_node * node, int depth, float prev)
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


void free_memory_nodes(memory_node * root)
/*
    Delete (free) memory nodes in the linked list.

    Note:
    - We need to free the nodes as we previously used malloc when intializing the linked list
    - To be called in main after all information is shown and right before program terminates
*/


void print_cpu_header_info(float perc)
/*
    Prints to terminal the header of the CPU section for the final iteration.

    note:
    - CPU Usage header includes dispaying the final cpu usage percentage
*/


void print_cpu_header_blank()
/*
    Prints to terminal the header of the CPU section for the remaining non final iterations.

    note:
    - Provide space for perc to be displayed by the print_cpu_header_info function
*/


cpu_node *initialize_cpu_nodes(int samples)
/*
    Returns a pointer to cpu_node struct corresponding to a non root/head of the linked list.
    Integer samples is used to determine the length of the linked list of memory nodes.

    Note:
    - cpu_used_perc is initialized to -1.0 which corresponds to "no value allocated" yet
    - This function is only to be called by initialize_cpu_root
*/


cpu_node *initialize_cpu_root(int samples)
/*
    Returns a pointer to cpu_node struct corresponding to the root/head of the linked list.
    Calls the function initialize_cpu_node for the remaining cpu nodes of the linked list.
    Integer samples is used to determine the length of the linked list of memory nodes.

    Note:
    - cpu_used_perc is initialized to -1.0 which corresponds to "no value allocated" yet
*/


void set_cpu_node(cpu_node * node, int depth, float* perc)
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


void print_cpu_nodes(cpu_node * node, int depth)
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


void free_cpu_nodes(cpu_node * root)
/*
    Delete (free) cpu nodes in the linked list.

    Note:
    - We need to free the nodes as we previously used malloc when intializing the linked list
    - To be called in main after all information is shown and right before program terminates
*/


void print_session_user_header(){
/*
    Prints to terminal the header of the Sessions/users section.
*/


void print_session_user()
/*
    Prints to terminal all the users logged in the current machine.

    Data and information extracted from the utmp.h library
    - username(ut_user)     - session(ut_line)     - IP(ut_host)

    output follows the format "username session (IP)"
*/


void print_sysinfo_header()
/*
    Prints to terminal the header of the System Information section.
*/


void print_sysinfo()
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


void display_output(int system_bool, int user_bool, int graphics_bool, int samples, int tdelay)
/*
    Print information to terminal accordingly to the values in "integer booleans" system_bool, user_bool, graphics_bool
    samples amount of recorded data outputted with interval of tdelay seconds.

    Clear screen after every iteration and reprint information to provide the illusion of outputting information at 2 points simultaneously

    CPU node and Memory node linked lists are initialized within this function and also freed before this function finishes executing
*/




General Idea of How the Program works

	Imports Used (Libraries and Files):
	#include <string.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <ctype.h>
	#include <unistd.h>
	#include <sys/utsname.h>
	#include <sys/sysinfo.h>
	#include <utmp.h>
	
	Define 'boolean integers' for system and user, set value to 2 which indicates that the boolean has not been altered
	Setting value to 2 is important for parse_input to handle system and user flag cases
	Define 'boolean integer' graphics_bool=0 (false) and integers samples=10 and tdelay=1 accordingly to default values for the following
	Pass the address integers (including boolean integers) defined to parse_input which set their values accordingly to the command line arguments passed
	
	Once the integers are set accordingly, call display_output to print information to terminal, passing all the integers defined as arguments
	The idea behind display_output is to Clear screen after every iteration and reprint information from previous samples
	plus new information from current sample to provide the illusion of outputting information at 2 points simultaneously

	Inside display_output:

	Initialize memory node and cpu node linked lists to store previous samples' values by calling initialize_memory_root and initialize_cpu_root
	Initialize float perc to store current sample value of cpu usage percent which will be displayed in the final iteration's header of cpu usage
	Notice that Linked lists are used because we do not have a fixed amount of samples to be displayed

	Iterate samples amount of times to get sample amount of information from the appropriate libaries and files.

	Clear the screen in the beginning of every iteration using escape codes, empty terminal ready for printing/reprinting output
	Print the overall header, displaying samples and tdelay with print_overall_header

	Evaluates system_bool if system needs to be printed and if so print the header of the memory section print_memory_header
	
	Extract information regarding Phys Used/Tot and Virtual Used/Tot for the i th node in terms of depth using set_memory_node
        Note that i refers to the number of iteration it is currently at

        If graphics flag is raised, prints information graphically using print_memory_nodes_graphics
        Note that the prev argument should be set to -1.0 because we are calling it for the first sample and the rest is done recursively
	If graphics flag is not raised, prints information regularly using print_memory_nodes
	Cover remaining lines (samples - i lines) with \n (newline)

	Evaluates user_bool if user/sessions needs to be printed. If so, call set_cpu_node for the i th sample
	
	If it is already the final iteration, display cpu usage percentage stored in perc alongside header by calling print_cpu_header
	If not, display cpu usage header only by calling print_cpu_header_blank
	
        Note that address of perc is passed as an argument to store the value of cpu usage percentage for that sample
	If graphics flag is raised, print information graphically using print_cpu_nodes, cover remaining lines (samples - i lines) with \n (newline)

	
	If it is also in the final iteration, print out system information to the terminal using print_sysinfo_header and print_sysinfo

	Pause for tdelay amount of seconds before performing the iteration again

	In the end after all the iterations/samples out, free memory allocated for the 2 linked lists using free_memory_nodes and free_cpu_nodes