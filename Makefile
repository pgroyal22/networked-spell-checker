spell_check_server : ConnectionQueue.c ConnectionQueue.h Dictionary.c Dictionary.h LogQueue.c LogQueue.h spell_check_server.c
	gcc -g spell_check_server.c ConnectionQueue.c Dictionary.c LogQueue.c -o spell_check_server -lm -pthread -Wall -Werror