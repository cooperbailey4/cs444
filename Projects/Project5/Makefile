
simfs_test: simfs_test.c lib.a
	gcc -Wall -Wextra -DCTEST_ENABLE -o $@ $^

image.o: image.c image.h
	gcc -Wall -Wextra -c $<

block.o: block.c block.h
	gcc -Wall -Wextra -c $<

free.o: free.c free.h
	gcc -Wall -Wextra -c $<

inode.o: inode.c inode.h
	gcc -Wall -Wextra -c $<

mkfs.o: mkfs.c mkfs.h
	gcc -Wall -Wextra -c $<

lib.a: image.o block.o free.o inode.o mkfs.o
	ar rcs $@ $^

.PHONY: test

test: simfs_test
	./simfs_test

clean:
	rm -f simfs_test simfs_test

