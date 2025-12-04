TARGET = kxo
RL_USE_HT=YES
kxo-objs = main.o game.o xoroshiro.o mcts.o negamax.o zobrist.o
kxo-objs += reinforcement_learning.o
obj-m := $(TARGET).o
OBJS:=

ccflags-y := -std=gnu99 -Wno-declaration-after-statement
KDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

GIT_HOOKS := .git/hooks/applied

LDFLAGS:=
CFLAGS:=
CFLAGS+=-g
CFLAGS+=-I./neco

OBJS:=
OBJS+=xo-user.o
OBJS+=tui.o
OBJS+=./neco/neco.o

ifeq ($(RL_USE_HT), YES)
kxo-objs += rl_find_state_ht.o
else
kxo-objs += rl_find_state_rb.o
endif

all: kmod xo-user

kmod: $(GIT_HOOKS) main.c
	$(MAKE) -C $(KDIR) M=$(PWD) modules

xo-user: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %c
	$(CC) $< $(CFLAGS) -c -o $@

logo:
	cat logo.txt | lolcat -f > logof.txt && cat ./logof.txt

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

insmod: kmod
	sudo insmod $(TARGET).ko

rmmod:
	sudo rmmod $(TARGET).ko

format:
	clang-format *.[ch] -i

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(RM) xo-user
