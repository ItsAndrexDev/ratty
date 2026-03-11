#pragma once

enum Action {
    WATCH = 0,
    LOG,
    EXIT,
    UNINSTALL,
};
enum Status {
    SUCCESS = 0,
    FAILURE,
    WARN,
};
struct Message {
    int session;
    int value;
    Action action;
    Status status;
    Message() = default;

    Message(int session, int value, Status status, Action action) : session(session), value(value), status(status), action(action) {};
};