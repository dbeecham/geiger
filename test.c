
#line 1 "test.c.rl"
#define _GNU_SOURCE
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#line 13 "test.c"
static const int ping_start = 1;
static const int ping_first_final = 3;
static const int ping_error = 0;

static const int ping_en_main = 1;


#line 23 "test.c.rl"


int main(void) {

    int cs;
    ssize_t bytes_read = 0;
    int ret;
    int aplay_pipe[2];
    ssize_t geiger_len = 0;
    int geiger_fd;
    pid_t aplay_pid;
    char buf[4];
    const char * p;
    const char * pe;
    const char * const eof = 0;

    
#line 39 "test.c"
	{
	cs = ping_start;
	}

#line 40 "test.c.rl"

    char geiger[2048];
    geiger_fd = open("geiger.raw", O_RDONLY);
    if (-1 == geiger_fd) {
        fprintf(stderr, "open: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    geiger_len = read(geiger_fd, geiger, 2047);
    if (geiger_len <= 0) {
        fprintf(stderr, "read: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    geiger[geiger_len] = 0;

    close(geiger_fd);


    ret = pipe(aplay_pipe);
    if (-1 == ret) {
        fprintf(stderr, "pipe: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    switch (aplay_pid = fork()) {
        case -1:
            fprintf(stderr, "fork: %s", strerror(errno));
            exit(EXIT_FAILURE);

        case 0:
            // reading end of the pipe.
            if (-1 == close(aplay_pipe[0])) {
                fprintf(stderr, "close aplay_pipe[0]: %s", strerror(errno));
                exit(EXIT_FAILURE);
            }
            for(;;) {
                bytes_read = read(STDIN_FILENO, buf, 4);
                if (-1 == bytes_read) {
                    fprintf(stderr, "read: %s", strerror(errno));
                    exit(EXIT_FAILURE);
                }
                if (0 == bytes_read) {
                    exit(EXIT_SUCCESS);
                }

                p = buf;
                pe = buf + bytes_read;

                
#line 94 "test.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
	switch( (*p) ) {
		case 46: goto st2;
		case 69: goto tr2;
	}
	goto tr0;
tr0:
#line 19 "test.c.rl"
	{ {goto st1;} }
	goto st0;
#line 113 "test.c"
st0:
cs = 0;
	goto _out;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
	if ( (*p) == 46 )
		goto tr2;
	goto tr0;
tr2:
#line 13 "test.c.rl"
	{
        write(aplay_pipe[1], geiger, geiger_len);
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 134 "test.c"
	goto tr0;
	}
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 1: 
	case 2: 
#line 19 "test.c.rl"
	{ {goto st1;} }
	break;
#line 150 "test.c"
	}
	}

	_out: {}
	}

#line 89 "test.c.rl"

            }

        default:

            // The pipe-reading child process. This process does not use the
            // write-end of the process, so we close it.
            if (-1 == close(aplay_pipe[1])) {
                fprintf(stderr, "close aplay_pipe[1]: %s", strerror(errno));
                exit(EXIT_FAILURE);
            }

            if (aplay_pipe[0] != STDIN_FILENO) {
                if (-1 == dup2(aplay_pipe[0], STDIN_FILENO)) {
                    fprintf(stderr, "dup2 aplay_pipe[0]: %s", strerror(errno));
                    exit(EXIT_FAILURE);
                }
                if (-1 == close(aplay_pipe[0])) {
                    fprintf(stderr, "close aplay_pipe[0]: %s", strerror(errno));
                    exit(EXIT_FAILURE);
                }
            }
            
            // At this point, STDIN_FILENO will point to the read-end of the
            // pipe. execlp will use that.
            execlp("aplay", "aplay", "--quiet", "--file-type=raw", "--format=S16_LE", "--nonblock", "--period-size=128", "--buffer-size=256", "--disable-resample", "--disable-format", (char*)NULL);
            
            // if we reach this point, then execlp failed :(
            fprintf(stderr, "execlp: %s", strerror(errno));
            exit(EXIT_FAILURE);


    }
    


}
