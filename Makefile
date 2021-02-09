test: test.o

%.c: %.c.rl
	ragel -G2 -o $@ $<
