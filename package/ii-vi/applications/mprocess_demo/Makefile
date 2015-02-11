
all: mprocess_demo

mprocess_demo:
	@$(CC) -o mprocess_demo -DPROCESS=m mprocess_demo.c
	@$(CC) -o demo_process1 -DPROCESS=1 mprocess_demo.c
	@$(CC) -o demo_process2 -DPROCESS=2 mprocess_demo.c

clean:
	@rm -rf main_process demo_process1 demo_process2
