#define _GNU_SOURCE
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

%%{

    machine ping;

    action geiger {
        write(aplay_pipe[1], geiger, geiger_len);
    }

    main := ( '..' @geiger 
            | 'E'  @geiger 
            ) $err{ fgoto main; };

    write data;

}%%

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

    %% write init;

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

                %% write exec;

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
