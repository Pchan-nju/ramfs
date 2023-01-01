.PHONY: run binary clean submit git

test: git
	gcc test.c ramfs.c -o test

run: test
	./test

git:
	git add -A
	git commit -am "compile"

clean:
	-rm test

submit:
  curl -X POST -F "TOKEN=$(TOKEN)" -F "FILE=@archive" https://oj.cpl.icu/api/v2/submission/lab

