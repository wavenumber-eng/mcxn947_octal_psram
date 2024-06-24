
#ifndef BUNNY_DEBUG_H_
#define BUNNY_DEBUG_H_

#define BUNNY_DBG(...)   				PRINTF(__VA_ARGS__)

#define VT100_RED    					"\033[31;40m"
#define VT100_GREEN   					"\033[32;40m"
#define VT100_YELLOW  					"\033[33;40m"
#define VT100_BLUE   					"\033[34;40m"
#define VT100_MAGENTA 					"\033[35;40m"
#define VT100_CYAN    					"\033[36;40m"
#define VT100_WHITE 			   		"\033[37;40m"

#define VT100_CLEAR_SCREEN  			"\033[2J"
#define VT100_HIDE_CURSOR 				"\033[?25l"
#define VT100_CURSOR_HOME				"\033[H"

#define VT100_CLEAR_LINE			    "\033[2K"
#define VT100_MOVE_CURSOR_START	        "\033[1G"
#define VT100_MOVE_CURSOR_TO_COLUMN     "\033[%dG"

#define CLEAR_LINE() 			  		BUNNY_DBG(VT100_CLEAR_LINE)
#define MOVE_CURSOR_START() 	  		BUNNY_DBG(VT100_MOVE_CURSOR_START)
#define MOVE_CURSOR_TO_COLUMN(c)   		BUNNY_DBG(VT100_MOVE_CURSOR_TO_COLUMN, c)
#define PRINT_AT_COLUMN(c,...) 		    MOVE_CURSOR_TO_COLUMN(c);BUNNY_DBG(__VA_ARGS__)

#endif /* BUNNY_DEBUG_H_ */
