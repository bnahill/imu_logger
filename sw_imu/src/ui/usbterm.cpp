#include "usbterm.h"

ShellCommand const USBTerm::commands[] = {
	{"listcmds", "s()", (ShellCommand::shell_callback_t)&USBTerm::cmd_listcmds},
	{"help", "s()", (ShellCommand::shell_callback_t)&USBTerm::cmd_help},
	{"settime", "b(date:datetime)", (ShellCommand::shell_callback_t)&USBTerm::cmd_settime},
	{"ping", "s()", (ShellCommand::shell_callback_t)&USBTerm::cmd_ping},
};

uint32_t const USBTerm::num_commands = sizeof(commands)/sizeof(*commands);

static inline bool is_ascii_num(char n){return (n >= '0') && (n <= '9');}


msg_t USBTerm::terminate(){
	chThdTerminate(thread);
	return chThdWait(thread);
}

void USBTerm::thread_action(){
	//Euclidean3_f32 m;
	uint8_t command[64];
	uint8_t *iter;
	uint32_t i;
	
	usbserial1.init();
	
	iter = command;
	
	while(!chThdShouldTerminate()){
		if(!usbserial1.read(iter, 1, MS2ST(10)))
			continue;
		
		if('\r' == *iter){
			iter = command;
			for(i = 0; i < num_commands; i++){
				if(commands[i].match(command)){
					if(0 == commands[i].call(command, *this)){
						chprintf(usbserial1.stream(), "OK\r\n");
					} else {
						chprintf(usbserial1.stream(), "ER\r\n");
					}
					break;
				}
			}
			if(i == num_commands){
				// No matching command
				chprintf(usbserial1.stream(), "NO\r\n");
			}
			continue;
		}
		
		iter += 1;
		
		//chThdSleep(MS2ST(10));
		//acc1.get_reading(m);
		//chprintf(usbserial1.stream(),
		//         "%f -- %f -- %f\r", m.x, m.y, m.z);
	}
	usbserial1.stop();
}

int32_t USBTerm::cmd_help(const char* cmd){
	chprintf(usbserial1.stream(), "You did something wrong\r\n");
	return 0;
}

int32_t USBTerm::cmd_settime(const char* cmd){
	rtc1::rtc_time_t time;
	
	while((*cmd != ' ') && (*cmd != '\r')) cmd++;
	if(*cmd == '\r'){
		return 1;
	}
	// Go to the first character of the date/time
	while(*cmd == ' ') cmd++;
	
	if(!is_ascii_num(cmd[0])) goto error;
	if(!is_ascii_num(cmd[1])) goto error;
	if(cmd[2] != ':') goto error;
	if(!is_ascii_num(cmd[3])) goto error;
	if(!is_ascii_num(cmd[4])) goto error;
	if(cmd[5] != ':') goto error;
	if(!is_ascii_num(cmd[6])) goto error;
	if(!is_ascii_num(cmd[7])) goto error;
	
	
	time.years_ten = cmd[0] - '0';
	time.years_u = cmd[1] - '0';
	time.months_ten = cmd[3] - '0';
	time.months_u = cmd[4] - '0';
	time.days_ten = cmd[6] - '0';
	time.days_u = cmd[7] - '0';
	
	cmd = &cmd[8];
	while(*cmd == ' ') cmd++;
	
	if(!is_ascii_num(cmd[0])) goto error;
	if(!is_ascii_num(cmd[1])) goto error;
	if(cmd[2] != ':') goto error;
	if(!is_ascii_num(cmd[3])) goto error;
	if(!is_ascii_num(cmd[4])) goto error;
	if(cmd[2] != ':') goto error;
	if(!is_ascii_num(cmd[6])) goto error;
	if(!is_ascii_num(cmd[7])) goto error;
	
	time.hours_ten = cmd[0] - '0';
	time.hours_u = cmd[1] - '0';
	time.minutes_ten = cmd[3] - '0';
	time.minutes_u = cmd[4] - '0';
	time.seconds_ten = cmd[6] - '0';
	time.seconds_u = cmd[7] - '0';
	
	rtc1::set_time(time);
	
	return 0;
	
error:
	chprintf(usbserial1.stream(), "Try again...\r\n");
	chprintf(usbserial1.stream(), "Format: settime YY:MM:DD HH:mm:SS\r\n");
	return 1;
}

int32_t USBTerm::cmd_ping(const char* cmd){
	chprintf(usbserial1.stream(), "pong\r\n");
	return 0;
}

int32_t USBTerm::cmd_listcmds(const char* cmd){
	uint_fast8_t i;
	for(i = 0; i < num_commands; i++){
		chprintf(usbserial1.stream(), "%s %s\r\n", commands[i].get_root(), commands[i].get_args());
	}
	return 0;
}

