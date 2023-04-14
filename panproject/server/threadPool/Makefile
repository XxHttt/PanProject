SRCS:=$(wildcard *.c)
OBJS:=$(SRCS:%.c=%.o)
ftpserver:$(OBJS)
	gcc $^ -o $@ -lpthread -lcrypt # 链接
	rm *.o
%.o:%.c
	gcc -c $^ -o $@ -g # 编译
clean:
	$(RM) $(OBJS) server

