.PHONY: run binary clean submit git

ramfs: git
	gcc test.c ramfs.c -o ramfs

run: ramfs
	./ramfs

git:
	git add -A
	git commit --allow-empty -m "compile"

clean:
	-rm test

submit:
  curl -X POST -F "TOKEN=$(TOKEN)" -F "FILE=@archive" https://oj.cpl.icu/api/v2/submission/lab

