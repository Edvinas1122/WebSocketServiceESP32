#include "WebSocketService.h"

WebSocketService::Message::Message(
    websockets::WebsocketsMessage message
) {
    deserializeJson(json, message.data());
}

WebSocketService::Message::Message(
    const char *message
) {
    deserializeJson(json, message);
}

const String WebSocketService::Message::type() {
    if (json.containsKey("event")) {
        return json["event"].as<String>();
    }
    return "";
}

const String WebSocketService::Message::payload() {
    if (json.containsKey("data")) {
        return json["data"].as<String>();
    }
    return "";
}

const String WebSocketService::Message::handlerHint() {
    if (json.containsKey("channel")) {
        return json["channel"].as<String>();
    }
    return "";
}

const String WebSocketService::Message::senderIdentity() {
    if (json.containsKey("user_id")) {
        return json["user_id"].as<String>();
    }
    return "";
}

const String WebSocketService::Message::getItem(
    const char *item
) {
    if (json.containsKey(item)) {
        return json[item].as<String>();
    }
    return "";
}
