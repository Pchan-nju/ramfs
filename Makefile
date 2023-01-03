.PHONY: all run binary clean submit git gdb

all: ramfs

ramfs: git
	gcc -g -std=c17 -O2 main.c ramfs.c -o ramfs

run: ramfs
	./ramfs

gdb: ramfs
	gdb ramfs

git:
	git add -A
	git commit --allow-empty -m "compile"

clean:
	@rm test

submit:
	$(eval TEMP := $(shell mktemp -d))
	$(eval BASE := $(shell basename $(CURDIR)))
	$(eval FILE := ${TEMP}/${TOKEN}.zip)
	@cd .. && zip -qr ${FILE} ${BASE}/.git
	@echo "Created submission archive ${FILE}"
	@curl -m 5 -w "\n" -X POST -F "TOKEN=${TOKEN}" -F "FILE=@${FILE}" \
		https://oj.cpl.icu/api/v2/submission/lab
	@rm -r ${TEMP}
