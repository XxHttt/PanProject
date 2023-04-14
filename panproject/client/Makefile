
SRCS:=$(wildcard *.c)
OBJS:=$(SRCS:%.c=%.o)
client:$(OBJS)
	gcc $^ -o $@ -lpthread # 链接
	rm *.o
%.o:%.c
	gcc -c $^ -o $@ -g # 编译
clean:
	$(RM) $(OBJS) client

