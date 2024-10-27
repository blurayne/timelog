SHELL:=/bin/bash
.SHELLFLAGS:=-eu -o pipefail -c
CONTAINER_CMD=bash -uo pipefail
ENTRYPOINT?=

.SHELLFLAGS:=+e -c
prepare-builders:
	buildah bud --jobs=2 --platform linux/arm64/v8,linux/amd64 --manifest localhost/timelog-builder:al2023 --target al2023 .
	buildah bud --jobs=2 --platform linux/arm64/v8,linux/amd64 --manifest localhost/timelog-builder:alpine --target alpine .

.SHELLFLAGS:=+e -c
clean-images:
	buildah rmi localhost/timelog-build:al2023
	buildah rmi localhost/timelog-builder:alpine 

run-alpine-amd64:
	exec podman run --platform linux/amd64 -it --rm -e ENTRYPOINT -v $$PWD:/usr/local/src/timelog -w /usr/local/src/timelog localhost/timelog-builder:alpine $(CONTAINER_CMD)

run-alpine-arm64:
	exec podman run --platform linux/arm64 -it --rm -e ENTRYPOINT -v $$PWD:/usr/local/src/timelog -w /usr/local/src/timelog localhost/timelog-builder:alpine $(CONTAINER_CMD)

run-al2023-amd64:
	exec podman run --platform linux/amd64 -it --rm -e ENTRYPOINT -v $$PWD:/usr/local/src/timelog -w /usr/local/src/timelog localhost/timelog-builder:al2023 $(CONTAINER_CMD)

run-al2023-arm64:
	exec podman run --platform linux/arm64 -it --rm -e ENTRYPOINT -v $$PWD:/usr/local/src/timelog -w /usr/local/src/timelog localhost/timelog-builder:al2023 $(CONTAINER_CMD)

.ONESHELL:

CONTAINER_CMD=bash -ueo pipefail -c 'eval "$$ENTRYPOINT"'
build-all:
	ENTRYPOINT="$$(make -sn build-static)" make run-alpine-amd64 
	ENTRYPOINT="$$(make -sn build-static)" make run-alpine-arm64 
	ENTRYPOINT="$$(make -sn build-static)" make run-al2023-amd64
	ENTRYPOINT="$$(make -sn build-static)" make run-al2023-arm64
	ENTRYPOINT="$$(make -sn build)" make run-al2023-amd64
	ENTRYPOINT="$$(make -sn build)" make run-al2023-arm64

.ONESHELL:
build:
	set -x
	source /etc/os-release 
	dist_target="dist/$$HOSTTYPE-$$OSTYPE-$$ID$$VERSION_ID"
	echo $$dist_target
	mkdir -m755 -pv "$$dist_target/"{bin,src/timelog}
	install -vD -m744 timelog.c "$$dist_target/src/timelog"
	gcc -O3 -s -o "$$dist_target/bin/timelog" timelog.c
	chmod 755 "$$dist_target/bin/timelog"
	tar -C "$$dist_target" -czvf "dist/timelog-$$HOSTTYPE-$$OSTYPE-$$ID$$VERSION_ID.tar.gz" .


.ONESHELL:
build-static:
	set -x
	source /etc/os-release 
	dist_target="dist/$$HOSTTYPE-$$OSTYPE-static"
	echo $$dist_target
	mkdir -m755 -pv "$$dist_target/"{bin,src/timelog}
	install -vD -m744 timelog.c "$$dist_target/src/timelog"
	gcc -static -O3 -s -o "$$dist_target/bin/timelog" timelog.c
	chmod 755 "$$dist_target/bin/timelog"
	tar -C "$$dist_target" -czvf "dist/timelog-$$HOSTTYPE-$$OSTYPE-static.tar.gz" .


test-output:
	@(for i in {1..100}; do printf "rand-$$i-%d\n" "$$RANDOM"; done; bash -c 'read -p "Enter text: "' ) 2>&1

compile-all:
	while read -r f; do (set -x; gcc -s -o $${f%.c} $${f}); done < <(find -not -path "./dist/*" -name "*.c" -type f) 


