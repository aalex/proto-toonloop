all: sigs
	@echo DONE
    
sigs:
	gcc -Wall -Werror -o sigs sigs.c

clean:
	rm -rf sigs
