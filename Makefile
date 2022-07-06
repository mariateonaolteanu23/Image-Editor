CFLAGS=-Wall -Wextra -std=c99

TARGETS=image_editor
build: $(TARGETS)

image_editor: image_editor.o editor_utils.o image_utils.o matrix_utils.o
	$(CC) $(CFLAGS) image_editor.o matrix_utils.o editor_utils.o  image_utils.o  -lm  -o image_editor

editor_utils: editor_utils.h editor_utils.c
	$(CC) $(CFLAGS) editor_utils.c -c -lm  -o editor_utils.o

matrix_utils: matrix_utils.h matrix_utils.c
	$(CC) $(CFLAGS) matrix_utils.c -c -lm  -o matrix_utils.o

image_utils: image_utils.h image_utils.c
	$(CC) $(CFLAGS) image_utils.c -c -lm -o image_utils.o

image_editor.o: editor_utils.c
	$(CC) $(CFLAGS) image_editor.c -lm -c -o image_editor.o


clean:
	rm -f *.o $(TARGETS) image_editor *.h.gch

pack:
	zip -FSr 321CA_Olteanu_Maria_Teona_tema3.zip Makefile README *.c *.h