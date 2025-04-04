FROM gcc:latest

WORKDIR /app

COPY . .

RUN make

RUN make test

CMD ["./build/ggpicture"]
