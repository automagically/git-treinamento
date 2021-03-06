#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>


/*
 * Updates leds states, index 0 through 5, other bits are ignored
 *
 *	data is a pointer to user data.
 */
unsigned char updateOnKeyPress(unsigned char state, unsigned char key, void** data) {
	
	// randomically turns leds on and off
	int i = rand() % 6;
	return state ^ (1 << i);
}  


/*
 * WARNING BLACK MAGIC BELOW
 * procede by your own risk 
 */

int main()
{
	struct termios old_tio, new_tio;
	unsigned char c;

	/* get the terminal settings for stdin */
	tcgetattr(STDIN_FILENO,&old_tio);

	/* we want to keep the old setting to restore them a the end */
	new_tio=old_tio;

	/* disable canonical mode (buffered i/o) and local echo */
	new_tio.c_lflag &=(~ICANON & ~ECHO);

	/* set the new settings immediately */
	tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);

	int tem = fcntl(0, F_GETFL, 0);
	fcntl (0, F_SETFL, (tem | O_NDELAY));
	unsigned char state = 0;
	void* data = NULL;
	do {
		//int n = read(0, &c, 1);
		c = getchar();

		if (c != 255) {
			state = updateOnKeyPress(state, c, &data);
		}

		printf("\e[2K\r");
		for(int i = 0; i < 6; i++){
			if (state & (1 << i)) {
				printf("\e[1;3%dm", i+1);		
			} else {
				printf("\e[0m");
			}
			printf("▣ ");
		}
		if (c != 255){
			printf("\e[0m %d", c);
		}
		usleep(50*1000);
 	} while(c!='q'	);

									
	/* restore the former settings */
	tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
	fcntl(0, F_SETFL, tem);
	return 0;
}
