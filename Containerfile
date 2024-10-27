FROM alpine:latest AS alpine

RUN apk add gcc make curl tar bash musl-dev

FROM amazonlinux:2023 AS al2023

RUN yum install -y gcc make tar gzip glibc-static